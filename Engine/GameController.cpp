//
//  GameController.cpp
//  TWT2
//
//  Created by Adriaan Kisjes on 11/10/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#include "GameController.hpp"

GameRunner::GameRunner(std::shared_ptr<GameState> _gs){
    GState = _gs;
    TimeInterfal = 10;
}

void GameRunner::Run(){
    for(auto& p : GState->Players){
        for(auto& t : p.second.Turns){
            GState->Settings->PerformTurn(p.first, t, GState);
        }
        p.second.Turns = {};
    }
    for(auto city = GState->Cities.begin(); city != GState->Cities.end(); city++){
        for(auto as = city->second->ActionSlots.begin(); as != city->second->ActionSlots.end(); as++){
            auto r = *as;
            if(r->Actionable(*city->second, GState)){
                r->Perform(*city->second, GState);
            }
        }
    }
    while(GState->DelayedActions.NextActionBefore(GState->Time + TimeInterfal)){
        auto da = GState->DelayedActions.GetNextAction();
        auto host = GState->Cities[da->Host];
        da->Perform(*host, GState);
    }
    GState->Time += TimeInterfal;
}

void GameRunner::SetTurn(){
    
}
