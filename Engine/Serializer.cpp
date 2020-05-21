//
//  Serializer.cpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 02/12/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#include "Serializer.hpp"

bool isWhiteSpace(char _c){
    if(_c == ' ')
        return true;
    if(_c == '\n')
        return true;
    if(_c == '\t')
        return true;
    return false;
}

SDL::Data parseAll(std::string& _inp, int& _i);

SDL::Data parseString(std::string& _inp, int& _i){
    SDL::Data res;
    res.DataType = SDL::String;
    bool end = false;
    while(!end && _i < _inp.size()){
        if(_inp[_i] == '\"'){
            end = true;
            _i++;
        }
        else{
            res.Val += _inp[_i];
            _i++;
        }
    }
    return res;
}

SDL::Data parseNum(std::string& _inp, int& _i){
    SDL::Data res;
    res.DataType = SDL::Int;
    bool end = false;
    std::string numstr = "0123456789";
    while(!end && _i < _inp.size()){
        if(numstr.find(_inp[_i]) != std::string::npos){
            res.Val += _inp[_i];
            _i++;
        }
        else{
            end = true;
        }
    }
    return res;
}

SDL::Data parseList(std::string& _inp, int& _i){
    SDL::Data res;
    res.DataType = SDL::List;
    bool end = false;
    bool findElem = true;
    while(!end && _i < _inp.size()){
        if(_inp[_i] == ']'){
            end = true;
            _i++;
        }
        else if(findElem){
            res.Append(parseAll(_inp, _i));
            findElem = false;
        }
        else if(_inp[_i] == ','){
            findElem = true;
            _i++;
        }
        else if(isWhiteSpace(_inp[_i])){
            _i++;
        }
        else{
            SDL::Data error;
            error.DataType = SDL::Nill;
            error.Val = std::to_string(_i);
        }
    }
    return res;
}

SDL::Data parseDict(std::string& _inp, int& _i){
    SDL::Data res;
    res.DataType = SDL::Dict;
    bool end = false;
    bool findElem = false;
    std::string key = "";
    while(!end && _i < _inp.size()){
        if(_inp[_i] == '}'){
            end = true;
            _i++;
        }
        else if(key.size() == 0){
            auto d = parseAll(_inp, _i);
            key = d.Val;
        }
        else if(findElem){
            res.Col[key] = parseAll(_inp, _i);
            findElem = false;
        }
        else if(_inp[_i] == ','){
            key = "";
            _i++;
        }
        else if(_inp[_i] == ':'){
            findElem = true;
            _i++;
        }
        else if(isWhiteSpace(_inp[_i])){
            _i++;
        }
        else{
            SDL::Data error;
            error.DataType = SDL::Nill;
            error.Val = std::to_string(_i);
        }
    }
    return res;
}

SDL::Data parseAll(std::string& _inp, int& _i){
    SDL::Data res;
    res.DataType = SDL::Nill;
    std::string numstr = "0123456789";
    bool end = false;
    while(!end && _i < _inp.size()){
        if(_inp[_i] == '\"'){
            _i++;
            return parseString(_inp, _i);
        }
        else if(_inp[_i] == '['){
            _i++;
            return parseList(_inp, _i);
        }
        else if(_inp[_i] == '{'){
            _i++;
            return parseDict(_inp, _i);
        }
        else if(numstr.find(_inp[_i]) != std::string::npos){
            return parseNum(_inp, _i);
        }
        else if(isWhiteSpace(_inp[_i])){
            _i++;
        }
        else{
            SDL::Data error;
            error.DataType = SDL::Nill;
            error.Val = std::to_string(_i);
        }
    }
    return res;
}

SDL::Data SDL::Deserialize(std::string _inp){
    int i = 0;
    return parseAll(_inp, i);
}



std::string SDL::Serialize(SDL::Data _inp){
    std::string res = "";
    switch (_inp.DataType) {
        case SDL::Int:{
            res += _inp.Val;
            break;
        }
            
        case SDL::String:{
            res += "\"" + _inp.Val + "\"";
            break;
        }
            
        case SDL::List:{
            res += "[";
            bool first = true;
            auto l = _inp.GetList();
            for(auto& d : l){
                if(!first)
                    res += ",";
                res += SDL::Serialize(d);
                first = false;
            }
            res += "]";
            break;
        }
            
        case SDL::Dict:{
            res += "{";
            bool first = true;
            for(auto& d :_inp.Col){
                if(!first)
                    res += ",";
                res += "\"" + d.first + "\":";
                res += SDL::Serialize(d.second);
                first = false;
            }
            res += "}";
            break;
        }
        case SDL::Nill:{
            res += "ERROR At " + _inp.Val;
            break;
        }
    }
    return res;
}


