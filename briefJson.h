//
//  briefJson.h
//  briefJson
//
//  Created by 范文捷 on 15/3/12.
//  Copyright (c) 2015年 fanwenjie.org. All rights reserved.
//

#ifndef BRIEFJSON_H
#define BRIEFJSON_H

namespace briefJson{
    
    //value template of json object
    //@params
    //	data	:point of value's data
    //	type	:point of data's type_info,which can be bool,int,long,double,wstring,nullptr,vector,map
    typedef struct value
    {
        void *data;
        const std::type_info* type;
    }value_t;
    
    //exception thrown when parse an illegal json
    //@params
    //	position	:position where exception  caused in json
    //	json		:illegal json text
    //	message		:message about exception
    typedef struct exception
    {
        int position;
        std::wstring json;
        std::wstring message;
    }exception_t;
    
    //dispose value of json object and free memory
    //@params
    //	value	:json value which will be disposed
    void dispose(value_t& value);
    
    //deserialize json text to json object
    //@params
    //	json	:json text which will be parsed
    //@return
    //	the value of json object
    value_t deserialize(std::wstring& json);
    
    //serialize json object to json text
    //@params
    //	value	:json object which will be serialized
    //@return
    //	json text created from json object
    std::wstring serialize(value_t& value);
}


#endif	//BRIEFJSON_H
