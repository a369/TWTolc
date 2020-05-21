//
//  Serializer.hpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 02/12/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#ifndef Serializer_hpp
#define Serializer_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <vector>

namespace SDL {

    enum DataTypes{
        String,
        Int,
        Float,
        List,
        Dict,
        Nill
    };

    struct Data {
    private:
        int iter;
    public:
        DataTypes DataType;
        std::string Val;
        std::map<std::string, Data> Col;
        
        Data(){iter = 0;}
        
        Data(std::string _str){
            iter = 0;
            DataType = String;
            Val = _str;
        }
        
        Data(int _int){
            iter = 0;
            DataType = Int;
            Val = std::to_string(_int);
        }
        
        Data(float _fl){
            iter = 0;
            DataType = Float;
            Val = std::to_string(_fl);
        }
        
        void Append(Data _d, std::string _ind = ""){
            if(_ind == ""){
                _ind = std::to_string(iter);
                iter++;
            }
            Col[_ind] = _d;
        }
        
        std::vector<Data> GetList(){
            std::vector<Data> res;
            for(int i = 0; i < iter; i++){
                res.push_back(Col[std::to_string(i)]);
            }
            return res;
        }
        
        int GetInt(){
            return std::stoi(Val);
        }
        
        float GetFloat(){
            return std::stof(Val);
        }
    };
    
    Data Deserialize(std::string _inp);
    
    std::string Serialize(Data _inp);

}


#endif /* Serializer_hpp */
