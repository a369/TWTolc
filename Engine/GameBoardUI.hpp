//
//  GameBoardUI.hpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 22/11/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

/*
Game graphics that are not part of the GUI (Board).
 */

#ifndef GameBoardUI_hpp
#define GameBoardUI_hpp

#include <stdio.h>
#include "olcPixelGameEngine.h"
#include "Objects.hpp"

class BoardUI{
    bool inScreen(int x, int y, int w, int h);
    void drawIndustry(olc::PixelGameEngine& _pge, int _x, int _y, float _s, int _t);
public:
    float DX;
    float DY;
    int X;
    int Y;
    float S;
    BoardUI(int _x, int _y, int _s){
        DX = 0.5;
        DY = 0.833;
        X = _x;
        Y = _y;
        S = _s;
    }
    void DrawBoard(olc::PixelGameEngine& _pge, std::shared_ptr<GameState> _gs, std::vector<Point> _tps);
    void DrawHex(olc::PixelGameEngine& _pge, int _x, int _y, float _s, olc::Pixel _c);
    void DrawCity(olc::PixelGameEngine& _pge, int _x, int _y, float _s, std::shared_ptr<City> _c, std::shared_ptr<GameState> _gs);
    void Zoom(int _z, int _x, int _y);
    void Move(int _x, int _y);
    Point ToScreenCoords(Point _p);
    Point ScreenToTile(int _x, int _y);
};

#endif /* GameBoardUI_hpp */
