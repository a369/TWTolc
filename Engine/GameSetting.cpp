//
//  GameSetting.cpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 16/11/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#include "GameSetting.hpp"
#include "Serializer.hpp"

ActionSlotMeta MakeStandartASM(int _vt, std::vector<std::string> _inp = std::vector<std::string>()){
    auto am = ActionSlotMeta();
    am.VisualType = _vt;
    for(const auto& i : _inp){
        if(i == "Good"){
            auto ids = InputDescriptor();
            ids.Description = i;
            ids.InputType = itString;
            ids.Var = std::vector<std::string>();
            am.InputType.push_back(ids);
        }
        else if(i == "Value"){
            auto ids = InputDescriptor();
            ids.Description = i;
            ids.InputType = itInt;
            ids.Var = std::vector<std::string>();
            am.InputType.push_back(ids);
        }
        else if(i == "City"){
            auto ids = InputDescriptor();
            ids.Description = i;
            ids.InputType = itInt;
            ids.Var = std::vector<std::string>();
            am.InputType.push_back(ids);
        }
    }
    return am;
}

ResourceMeta rmtFromSDLD(SDL::Data& _d){
    auto rm = ResourceMeta();
    if(_d.Col.find("canbenegative") == _d.Col.end())
        rm.CanBeNegative = false;
    else
        rm.CanBeNegative = _d.Col["canbenegative"].Val == "true";
    
    if(_d.Col.find("owner") == _d.Col.end())
        rm.Owner = rCity;
    else{
        auto owner = _d.Col["owner"].Val;
        if(owner == "city")
            rm.Owner = rCity;
        else if (owner == "cons")
            rm.Owner = rConstraint;
    }
    if(_d.Col.find("maintainance") != _d.Col.end())
        rm.MaintainCost = ValueCollection(_d.Col["maintainance"]);
    return rm;
}

ActionSlotMeta asmtFromSDLD(SDL::Data& _d){
    auto asmt = ActionSlotMeta();
    return asmt;
}

DinamicGameSettings::DinamicGameSettings(std::string _inp){
    auto d = SDL::Deserialize(_inp);
    auto resources = d.Col["resources"];
    auto production = d.Col["productionslots"];
    for(auto& r : resources.Col){
        Resources[r.first] = rmtFromSDLD(r.second);
    }
    for(auto& p : production.Col){
        actionSlotData[p.first] = p.second;
        ActionSlots[p.first] = asmtFromSDLD(p.second);
    }
    ActionSlots["transport"] = MakeStandartASM(0, {"Good", "Value", "City"});
}

std::shared_ptr<ActionSlot> DinamicGameSettings::GetActionSlot(std::string _actionSlotType, std::vector<std::string> _input){
    if(actionSlotData.find(_actionSlotType) != actionSlotData.end()){
        ValueCollection input;
        ValueCollection output;
        ValueCollection construction;
        ValueCollection destruction;
        int time = 0;
        auto d = actionSlotData[_actionSlotType];
        auto end = d.Col.end();
        if(d.Col.find("input") != end)
            input = ValueCollection(d.Col["input"]);
        if(d.Col.find("output") != end)
            output = ValueCollection(d.Col["output"]);
        if(d.Col.find("construction") != end)
            construction = ValueCollection(d.Col["construction"]);
        if(d.Col.find("destruction") != end)
            destruction = ValueCollection(d.Col["destruction"]);
        if(d.Col.find("time") != end)
            time = std::stoi(d.Col["time"].Val);
        return std::make_shared<ProduceActionSlot>(_actionSlotType,
                                                       input,
                                                       output,
                                                       construction,
                                                       destruction,
                                                       time);
    }
    else if(_actionSlotType == "transport"){
        int value = std::stoi(_input[1]);
        int pop = value / 6 + 1;
        return std::make_shared<TransportActionSlot>(_actionSlotType, ValueCollection("population", pop), ValueCollection(_input[0], value), std::stoi(_input[2]));
    }
    return nullptr;
}

bool DinamicGameSettings::ValidateInput(InputDescriptor _ids, std::string _input){
    auto res = true;
    switch (_ids.InputType) {
        case itInt:
        {
            auto testb = (_input.find_first_not_of("0123456789") == std::string::npos);
            return testb;
        }
        case itString:
            return true;
        case itTag:
            return true;
        default:
            break;
    }
    return res;
}

std::string DinamicGameSettings::InputTypeDescription(InputDescriptor _id){
    switch (_id.InputType) {
        case itInt:
            return "Integer";
        case itString:
            return "String";
        case itTag:
            return "Tag";
        default:
            break;
    }
    return "";
};

void DinamicGameSettings::PerformTurn(int _player, Turn& _t, std::shared_ptr<GameState> _gs){
    switch (_t.TurnType) {
        case ttAddActionSlot:
        {
            auto inp = _t.Input;
            auto city = std::stoi(inp[0]);
            inp.erase(inp.begin());
            auto pName = inp[0];
            inp.erase(inp.begin());
            auto as = GetActionSlot(pName, inp);
            _gs->Cities[city]->ActionSlots.push_back(as);
            break;
        }
        default:
            break;
    }
}

ResourceOwnerType DinamicGameSettings::GetOwner(std::string _rt){
    if(_rt[0] == '_'){
        return Resources[_rt.substr(1, _rt.size())].Owner;
    }
    return Resources[_rt].Owner;
}



