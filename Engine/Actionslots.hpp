//
//  Actionslots.hpp
//  TWT2
//
//  Created by Adriaan Kisjes on 16/10/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#ifndef Actionslots_hpp
#define Actionslots_hpp

#include <stdio.h>
#include "Objects.hpp"
#include "DelayedActions.hpp"

struct ProduceActionSlot : public ActionSlot{
    ValueCollection Input;
    ValueCollection Output;
    ValueCollection Construction;
    ValueCollection Destruction;
    int ProductionTime;
    ProduceActionSlot(std::string _name, ValueCollection _input, ValueCollection _output, ValueCollection _construction, ValueCollection _destruction, int _productionTime);
    bool Actionable(City& _host, std::shared_ptr<GameState> _gs) override;
    void Perform(City& _host, std::shared_ptr<GameState> _gs) override;
};

struct TransportActionSlot : public ActionSlot
{
    ValueCollection Goods;
    ValueCollection Constraints;
    int Destination;
    TransportActionSlot(std::string _name, ValueCollection _constraints, ValueCollection _goods, int _destination);
    bool Actionable(City& _host, std::shared_ptr<GameState> _gs) override;
    void Perform(City& _host, std::shared_ptr<GameState> _gs) override;
};

struct MaintainanceSlot : public ActionSlot
{
    MaintainanceSlot(std::string _name);
    bool Actionable(City& _host, std::shared_ptr<GameState> _gs) override;
    void Perform(City& _host, std::shared_ptr<GameState> _gs) override;
};


#endif /* Actionslots_hpp */
