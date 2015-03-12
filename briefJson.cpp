//
//  briefJson.cpp
//  Brief-JSON
//
//  Created by 范文捷 on 15/3/10.
//  Copyright (c) 2015年 fanwenjie. All rights reserved.
//

#include <map>
#include <sstream>
#include <vector>
#include "briefJson.h"
using namespace std;

namespace briefJson{
    
    static const type_info& TYPE_DOUBLE = typeid(double);
    static const type_info& TYPE_INT = typeid(int);
    static const type_info& TYPE_LONG = typeid(long);
    static const type_info& TYPE_BOOL = typeid(bool);
    static const type_info& TYPE_STRING = typeid(wstring);
    static const type_info& TYPE_NULL = typeid(nullptr);
    static const type_info& TYPE_MAP = typeid(map<wstring, value_t>);
    static const type_info& TYPE_VECTOR = typeid(vector<value_t>);
    
    static wchar_t nextToken(const wstring& json, int& position){
        while (json.at(position++) <= ' ');
        return json.at(position - 1);
    }
    
    static value_t wrapValue(void *ptr, const type_info& type){
        value_t v;
        v.data = ptr;
        v.type = &type;
        return v;
    }
    
    static exception_t exception(wstring message,const wstring& json, int& position){
        exception_t e;
        e.json = json;
        e.message = message;
        e.position = position;
        return e;
    }
    
    static value_t nextValue(const wstring& json, int& position){
        try{
            wchar_t c = nextToken(json, position);
            switch (c)
            {
                case '[':
                {
                    vector<value_t> *valueVector = new vector<value_t>();
                    try{
                        if (nextToken(json, position) != ']'){
                            --position;
                            while (true)
                            {
                                if (nextToken(json, position) == ','){
                                    --position;
                                    valueVector->push_back(wrapValue(nullptr, TYPE_NULL));
                                }
                                else{
                                    --position;
                                    valueVector->push_back(nextValue(json, position));
                                }
                                switch (nextToken(json, position))
                                {
                                    case ',':
                                        if (nextToken(json, position) == ']')
                                            return wrapValue(valueVector, TYPE_VECTOR);
                                        --position;
                                        break;
                                    case ']':
                                        return wrapValue(valueVector, TYPE_VECTOR);
                                    default:
                                        throw exception(L":Expected a ',' or ']'", json, position);
                                        break;
                                }
                            }
                        }
                        else	return wrapValue(valueVector, TYPE_VECTOR);
                    }
                    catch (out_of_range e){
                        value_t v = wrapValue(valueVector, TYPE_VECTOR);
                        dispose(v);
                        throw e;
                    }
                    catch (exception_t e){
                        value_t v = wrapValue(valueVector, TYPE_VECTOR);
                        dispose(v);
                        throw e;
                    }
                    break;
                }
                    
                case '{':
                {
                    map<wstring, value_t> *valueMap = new map<wstring, value_t>();
                    try{
                        if (nextToken(json, position) != '}')
                        {
                            --position;
                            while (true)
                            {
                                value_t key = nextValue(json, position);
                                if ((*key.type) != TYPE_STRING)
                                    throw exception(L"Key of pair must be string", json, position);
                                if (nextToken(json, position) != ':')
                                    throw exception(L"Expected a ':' after a key", json, position);
                                valueMap->insert(pair<wstring, value_t>(*(wstring *) key.data, nextValue(json, position)));
                                switch (nextToken(json, position))
                                {
                                    case ';':
                                    case ',':
                                        if (nextToken(json, position) == '}')
                                            return wrapValue(valueMap, TYPE_MAP);
                                        --position;
                                        break;
                                    case '}':
                                        return wrapValue(valueMap, TYPE_MAP);
                                    default:
                                        throw exception(L"Expected a ',' or '}'", json, position);
                                }
                            }
                        }
                        else	return wrapValue(valueMap, TYPE_MAP);
                    }
                    catch (out_of_range e){
                        value_t v = wrapValue(valueMap, TYPE_MAP);
                        dispose(v);
                        throw e;
                    }
                    catch (exception_t e){
                        value_t v = wrapValue(valueMap, TYPE_MAP);
                        dispose(v);
                        throw e;
                    }
                    break;
                }
                    
                case '\'':
                case '"':
                {
                    wstring *str = new wstring();
                    try{
                        while (true) {
                            wchar_t ch = json.at(position++);
                            switch (ch) {
                                case '\n':
                                case '\r':
                                    throw exception(L"Unterminated string", json, position);
                                    break;
                                case '\\':
                                    ch = json.at(position++);
                                    switch (ch) {
                                        case 'b':
                                            (*str) += '\b';
                                            break;
                                        case 't':
                                            (*str) += '\t';
                                            break;
                                        case 'n':
                                            (*str) += '\n';
                                            break;
                                        case 'f':
                                            (*str) += '\f';
                                            break;
                                        case 'r':
                                            (*str) += '\r';
                                            break;
                                        case '"':
                                        case '\'':
                                        case '\\':
                                        case '/':
                                            (*str) += ch;
                                            break;
                                        case 'u':{
                                            wchar_t num = 0;
                                            for (int i = 3; i >= 0; --i){
                                                wchar_t tmp = json.at(position++);
                                                if (tmp >= '0'&&tmp <= '9')
                                                    tmp = tmp - '0';
                                                else if (tmp >= 'A'&&tmp <= 'F')
                                                    tmp = tmp - ('A' - 10);
                                                else if (tmp >= 'a'&&tmp <= 'f')
                                                    tmp = tmp - ('a' - 10);
                                                else tmp = 0;
                                                num += tmp << (i * 4);
                                            }
                                            (*str) += num;
                                            break;
                                        }
                                        default:
                                            throw exception(L"Illegal escape", json, position);
                                            break;
                                    }
                                    break;
                                default:
                                    if (ch == c)
                                        return wrapValue(str, TYPE_STRING);
                                    (*str) += ch;
                                    break;
                            }
                        }
                    }
                    catch (out_of_range e){
                        delete str;
                        throw e;
                    }
                    catch (exception_t e){
                        delete str;
                        throw e;
                    }
                }
            }
            
            const wchar_t keychar [] = { ',', ':', ']', '}', '/', '\\', '\"', '[', '{', ';', '=', '#' };
            const int keycharCount = 12;
            int length = 0;
            while (c >= ' ') {
                int i = 0;
                for (; i < keycharCount; ++i)
                    if (keychar[i] == c)
                        break;
                if (i != keycharCount)
                    break;
                ++length;
                c = json.at(position++);
            }
            const wstring substr = json.substr((--position) - length, length);
            if (substr.length() == 0)
                return wrapValue(new wstring(), TYPE_STRING);
            if (substr == L"true" || substr == L"TRUE")
                return wrapValue(new bool(true), TYPE_BOOL);
            else if (substr == L"false" || substr == L"FALSE")
                return wrapValue(new bool(false), TYPE_BOOL);
            else if (substr == L"null" || substr == L"NULL")
                return wrapValue(nullptr, TYPE_NULL);
            if (substr.find('.') == substr.npos&&substr.find('e') == substr.npos&&substr.find('E') == substr.npos){
                try{
                    long l = stol(substr);
                    int i = (int) l;
                    if (l == i)
                        return wrapValue(new int(i), TYPE_INT);
                    else
                        return wrapValue(new long(l), TYPE_LONG);
                }
                catch (invalid_argument){
                }
            }
            else{
                try{
                    return wrapValue(new double(stod(substr)), TYPE_DOUBLE);
                }
                catch (invalid_argument){
                }
            }
            return wrapValue(new wstring(substr), TYPE_STRING);
        }
        catch (out_of_range){
            throw exception(L"Unexpected end", json, position);
        }
    }
    
    value_t deserialize(wstring& json){
        int start = 0;
        return nextValue(json, start);
    }
    
    wstring serialize(value_t& value){
        if (*value.type == TYPE_NULL){
            return L"null";
        }
        else if (*value.type == TYPE_BOOL){
            bool b = *(bool *) value.data;
            if (b)	return L"true";
            else	return L"false";
        }
        else if (*value.type == TYPE_INT){
            int i = *(int *) value.data;
            wostringstream woss;
            woss << i;
            return woss.str();
        }
        else if (*value.type == TYPE_LONG){
            long l = *(long *) value.data;
            wostringstream woss;
            woss << l;
            return woss.str();
        }
        else if (*value.type == TYPE_DOUBLE){
            double d = *(double *) value.data;
            wostringstream woss;
            woss << d;
            return woss.str();
        }
        else if (*value.type == TYPE_STRING){
            wstring s = *(wstring *) value.data;
            return L"\"" + s + L"\"";
        }
        else if (*value.type == TYPE_VECTOR){
            wstring s = L"[";
            vector<value_t> *v = (vector<value_t> *) value.data;
            for (vector<value_t>::iterator iter = v->begin(); iter != v->end(); ++iter){
                if (iter != v->begin())
                    s += L",";
                s += serialize(*iter);
            }
            s += L"]";
            return s;
        }
        else if (*value.type == TYPE_MAP){
            wstring s = L"{";
            map<wstring, value_t> *m = (map<wstring, value_t> *)value.data;
            for (map<wstring, value_t>::iterator iter = m->begin(); iter != m->end(); ++iter){
                wstring k = L"\"" + iter->first + L"\"";
                wstring v = serialize(iter->second);
                if (iter != m->begin())
                    s += L",";
                s += (k + L":" + v);
            }
            s += L"}";
            return s;
        }
        return L"";
    }
    
    void dispose(value_t& value){
        if (*value.type == TYPE_NULL){
            return;
        }
        else if (*value.type == TYPE_BOOL){
            delete (bool *)value.data;
        }
        else if (*value.type == TYPE_INT){
            delete (int *)value.data;
        }
        else if (*value.type == TYPE_LONG){
            delete (long *)value.data;
        }
        else if (*value.type == TYPE_DOUBLE){
            delete (double *)value.data;
        }
        else if (*value.type == TYPE_STRING){
            delete (wstring *)value.data;
        }
        else if (*value.type == TYPE_VECTOR){
            wstring s = L"[";
            vector<value_t> *v = (vector<value_t> *) value.data;
            for (vector<value_t>::iterator iter = v->begin(); iter != v->end(); ++iter)
                dispose(*iter);
            delete (vector<value_t> *)value.data;
        }
        else if (*value.type == TYPE_MAP){
            map<wstring, value_t> *m = (map<wstring, value_t> *)value.data;
            for (map<wstring, value_t>::iterator iter = m->begin(); iter != m->end(); ++iter)
                dispose(iter->second);
            delete (map<wstring, value_t> *)value.data;
        }
        value.type = &TYPE_NULL;
    }
}
