//
//  DelayedActions.cpp
//  TWT2
//
//  Created by Adriaan Kisjes on 16/10/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#include "DelayedActions.hpp"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


AddResources::AddResources(std::string _name, int _host, int _time, ValueCollection _resources) : DelayedAction(_name, _host, _time){
    Resources = _resources;
}

void AddResources::Perform(City& _host, std::shared_ptr<GameState> _gs){
    for(const auto& r : Resources.Values){
        auto v = _gs->GetResource(_host, r.first);
        _gs->SetResource(_host, r.first, v + r.second);
    }
}








