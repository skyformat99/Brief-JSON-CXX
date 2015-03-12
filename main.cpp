//
//  main.cpp
//  briefJson
//
//  Created by 范文捷 on 15/3/12.
//  Copyright (c) 2015年 fanwenjie.org. All rights reserved.
//
#include <iostream>
#include <sstream>
#include <time.h>
#include <map>
#include <vector>
#include "briefJson.h"
using namespace std;
using namespace briefJson;

void print(value_t& value, int n = 0){
    if (*value.type == typeid(nullptr)){
        for (int i = 0; i < n; ++i)
            wcout << "| ";
        wcout << "NULL" << endl;
    }
    else if (*value.type == typeid(bool)){
        bool b = *(bool *) value.data;
        wstring str;
        if (b)	str = L"true";
        else	str = L"false";
        for (int i = 0; i < n; ++i)
            wcout << "| ";
        wcout << "BOOL:\t" << str << endl;
    }
    else if (*value.type == typeid(int)){
        int i = *(int *) value.data;
        wostringstream woss;
        woss << i;
        for (int i = 0; i < n; ++i)
            wcout << "| ";
        wcout << "INT:\t" << woss.str() << endl;
    }
    else if (*value.type == typeid(long)){
        long l = *(long *) value.data;
        wostringstream woss;
        woss << l;
        for (int i = 0; i < n; ++i)
            wcout << "| ";
        wcout << "LONG:\t" << woss.str() << endl;
    }
    else if (*value.type == typeid(double)){
        double d = *(double *) value.data;
        wostringstream woss;
        woss << d;
        for (int i = 0; i < n; ++i)
            wcout << "| ";
        wcout << "DOUBLE:\t" << woss.str() << endl;
    }
    else if (*value.type == typeid(wstring)){
        wstring s = *(wstring *) value.data;
        for (int i = 0; i < n; ++i)
            wcout << "| ";
        wcout << "STRING:\t" << s << endl;
    }
    else if (*value.type == typeid(vector<value_t>)){
        for (int i = 0; i < n; ++i)
            wcout << "| ";
        wcout << "VECTOR:\t" << endl;
        vector<value_t> *v = (vector<value_t> *) value.data;
        for (vector<value_t>::iterator iter = v->begin(); iter != v->end(); ++iter)
            print(*iter, n + 1);
    }
    else if (*value.type == typeid(map<wstring, value_t>)){
        for (int i = 0; i < n; ++i)
            wcout << "| ";
        wcout << "MAP:\t" << endl;
        map<wstring, value_t> *m = (map<wstring, value_t> *)value.data;
        int j = 0;
        for (map<wstring, value_t>::iterator iter = m->begin(); iter != m->end(); ++iter, ++j){
            wstring k = iter->first;
            value_t v = iter->second;
            for (int i = 0; i < n; ++i)
                wcout << "| ";
            wcout << "|KEY:\t" << k << endl;
            print(v, n + 1);
        }
    }
}

void test(){
    try{
        wstring json = L"{\"Name\":\"\\u0053\\u0070\\u0069\\u006b\\u0065\",\"Sex\":true,\"Age\":75,\"Friend\":[{\"Name\":\"Tom\",\"Sex\":true,\"Age\":75,\"Friend\":[]},{\"Name\":\"Jerry\",\"Sex\":true,\"Age\":75,\"Friend\":[{\"Name\":\"Tom\",\"Sex\":true,\"Age\":75,\"Friend\":[]}]}]}";
        briefJson::value_t v = briefJson::deserialize(json);
        print(v);
        briefJson::dispose(v);
    }
    catch (briefJson::exception_t e){
        wcout << e.message << endl;
    }
}

int main(int argc, char *argv []){
    test();
    return 0;
}
