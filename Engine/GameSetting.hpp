//
//  GameSetting.hpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 16/11/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

/*
Specifies the game rulles of a specific game.
*/

#ifndef GameSetting_hpp
#define GameSetting_hpp

#include <stdio.h>
#include "Objects.hpp"
#include "Actionslots.hpp"
#include "Serializer.hpp"

class DinamicGameSettings : public GameSettings{
    std::map<std::string, SDL::Data> actionSlotData;
public:
    DinamicGameSettings(std::string _inp);
    std::shared_ptr<ActionSlot> GetActionSlot(std::string _actionSlotType, std::vector<std::string> _input) override;
    bool ValidateInput(InputDescriptor _ids, std::string _input) override;
    std::string InputTypeDescription(InputDescriptor _id) override;
    void PerformTurn(int _player, Turn& _t, std::shared_ptr<GameState> _gs) override;
    ResourceOwnerType GetOwner(std::string _rt) override;
};

#endif /* GameSetting_hpp */
