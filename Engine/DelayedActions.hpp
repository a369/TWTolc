//
//  DelayedActions.hpp
//  TWT2
//
//  Created by Adriaan Kisjes on 16/10/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#ifndef DelayedActions_hpp
#define DelayedActions_hpp

#include <stdio.h>
#include "Objects.hpp"

struct AddResources : public DelayedAction{
    ValueCollection Resources;
    AddResources(std::string _name, int _host, int _time, ValueCollection _resources);
    void Perform(City& _host, std::shared_ptr<GameState> _gs) override;
};

#endif /* DelayedActions_hpp */
