//
//  GameController.hpp
//  TWT2
//
//  Created by Adriaan Kisjes on 11/10/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

/*
 Controller class that runs the game.
 */

#ifndef GameController_hpp
#define GameController_hpp

#include <stdio.h>
#include <iostream>
#include "Objects.hpp"

class GameRunner{
public:
    std::shared_ptr<GameState> GState;
    int TimeInterfal;
    GameRunner(std::shared_ptr<GameState> _gs);
    void Run();
    void SetTurn();
};

void RunGame(GameState &_gs);

#endif /* GameController_hpp */
