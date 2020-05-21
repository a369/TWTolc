//
//  GenerateGame.hpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 24/11/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

/*
 Method that generates the board.
 */

#ifndef GenerateGame_hpp
#define GenerateGame_hpp

#include <stdio.h>
#include "Objects.hpp"

std::shared_ptr<GameState> MakeGameState(std::shared_ptr<GameSettings> _gs);

#endif /* GenerateGame_hpp */
