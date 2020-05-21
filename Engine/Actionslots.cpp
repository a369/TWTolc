//
//  Actionslots.cpp
//  TWT2
//
//  Created by Adriaan Kisjes on 16/10/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#include "Actionslots.hpp"


//////////////////////////////////////////////////////////////////
// PRODUCE ///////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

ProduceActionSlot::ProduceActionSlot(std::string _name, ValueCollection _input, ValueCollection _output, ValueCollection _construction, ValueCollection _destruction, int _productionTime) : ActionSlot(_name){
    Input = _input;
    Output = _output;
    Construction = _construction;
    Destruction = _destruction;
    ProductionTime = _productionTime;
}

bool ProduceActionSlot::Actionable(City &_host, std::shared_ptr<GameState> _gs){
    if(Input.Size(true) + Output.Size(true) == 0 || !On || _gs->Time < TimeOutEnd)
        return false;
    for(const auto& r : Input.Values){
        if(_gs->GetResource(_host, r.first) < r.second){
            return false;
        }
    }
    return true;
}

void ProduceActionSlot::Perform(City &_host, std::shared_ptr<GameState> _gs){
    auto out_ = Output;
    auto in_ = Input;
    for(const auto& r : Output.Values){
        if(_gs->Settings->GetOwner(r.first) == rConstraint){
            out_.Add("_" + r.first, r.second * -1);
            in_.Add("_" + r.first, r.second * -1);
        }
    }
    for(const auto& r : in_.Values){
        auto v = _gs->GetResource(_host, r.first);
        _gs->SetResource(_host, r.first, v - r.second);
    }
    TimeOutEnd = ProductionTime + _gs->Time;
    _gs->DelayedActions.InsertAction(std::make_shared<AddResources>("txt", _host.ID, TimeOutEnd, out_));
}

//////////////////////////////////////////////////////////////////
// TRANSPORT /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

TransportActionSlot::TransportActionSlot(std::string _name, ValueCollection _constraints, ValueCollection _goods, int _destination) : ActionSlot(_name){
    Goods = _goods;
    Destination = _destination;
    Constraints = _constraints;
}

bool TransportActionSlot::Actionable(City& _host, std::shared_ptr<GameState> _gs){
    if(!On)
        return false;
    if(_gs->Time < TimeOutEnd)
        return false;
    if(_host.Resources.Encapsulates(Goods) && _host.RunningConstraints.Encapsulates(Constraints) && _host.Neighbours.find(Destination) != _host.Neighbours.end()){
        return true;
    }
    return false;
}

void TransportActionSlot::Perform(City& _host, std::shared_ptr<GameState> _gs){
    TimeOutEnd = _gs->Time + _host.Neighbours[Destination].Distance;
    _host.Resources.Remove(Goods);
    _host.RunningConstraints.Remove(Constraints);
    auto da = std::make_shared<AddResources>("transport arival", Destination, TimeOutEnd, Goods);
    auto da2 = std::make_shared<AddResources>("transport restore", _host.ID, TimeOutEnd, Constraints);
    _gs->DelayedActions.InsertAction(da);
    _gs->DelayedActions.InsertAction(da2);
}

//////////////////////////////////////////////////////////////////
// MAINTAINANCE //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

MaintainanceSlot::MaintainanceSlot(std::string _name) : ActionSlot(_name){
}

bool MaintainanceSlot::Actionable(City &_host, std::shared_ptr<GameState> _gs){
    return true;
}

void MaintainanceSlot::Perform(City &_host, std::shared_ptr<GameState> _gs){
    auto staticC = _host.StaticConstraints();
    for(const auto& sc : staticC.Values){
        auto loss = 0;
        for(const auto& c : _gs->Settings->Resources[sc.first].MaintainCost.Values){
            auto totalCost = sc.second * c.second;
            auto shortage = _host.Resources.Get(c.first) - totalCost;
            if(shortage < 0){
                auto nloss = (shortage * -1) / c.second;
                if(nloss / 2 == (nloss - 1) / 2)
                    nloss = nloss / 2 + 1;
                else
                    nloss = nloss / 2;
                if(nloss > loss)
                    loss = nloss;
                _host.Resources.Set(c.first, 0);
            }
            else{
                _host.Resources.Set(c.first, _host.Resources.Get(c.first) - totalCost);
            }
        }
        _host.RunningConstraints.Set(sc.first, _host.RunningConstraints.Get(sc.first) - loss);
    }
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


