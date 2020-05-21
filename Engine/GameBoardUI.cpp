//
//  GameBoardUI.cpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 22/11/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#include "GameBoardUI.hpp"

void drawIndustry(olc::PixelGameEngine& _pge, int _x, int _y, float _s, int _t);

void BoardUI::DrawBoard(olc::PixelGameEngine& _pge, std::shared_ptr<GameState> _gs, std::vector<Point> _tps){
    auto scrw = _pge.ScreenWidth();
    auto scrh = _pge.ScreenHeight();
    for(int i = 0; i < _gs->B->X; i++){
        for(int j = 0; j < _gs->B->Y; j++){
            auto sp = ToScreenCoords(Point(i, j));
            if(inScreen(sp.X, sp.Y, scrw, scrh)){
                auto p = Point(i, j);
                auto tile = _gs->B->GetTile(p);
                if(tile == 1){
                    DrawHex(_pge, sp.X, sp.Y, S, olc::Pixel(20, 100 + _gs->B->GetHight(p) * 30, 40));
                }
                else if(tile == 0){
                    int blue = 200 + _gs->B->GetHight(p) * 30;
                    if(blue < 50)
                        blue = 50;
                    DrawHex(_pge, sp.X, sp.Y, S, olc::Pixel(20, 40, blue));
                }
                else if(tile == 2){
                    if(_gs->B->GetHight(p) >= 3)
                        DrawHex(_pge, sp.X, sp.Y, S, olc::Pixel(240, 240, 240));
                    else
                        DrawHex(_pge, sp.X, sp.Y, S, olc::Pixel(190, 190, 190));
                }
            }
        }
    }
    auto pref = Point(-1, -1);
    for(const auto& r : _gs->Routes){
        for(const auto& p : r){
            if(pref.X >= 0){
                auto a = ToScreenCoords(pref);
                auto b = ToScreenCoords(p);
                _pge.DrawLine(a.X, a.Y, b.X, b.Y, olc::BLACK);
            }
            pref = p;
        }
        pref.X = -1;
    }
    for(const auto& c : _gs->Cities){
        auto sp = ToScreenCoords(c.second->Location);
        DrawCity(_pge, sp.X, sp.Y, S, c.second, _gs);
    }
    for(const auto& p : _tps){
        auto sp = ToScreenCoords(p);
        DrawHex(_pge, sp.X, sp.Y, S, olc::BLACK);
    }
}

Point BoardUI::ToScreenCoords(Point _p){
    float s = S;
    int dx = s * DX;
    int dy = s * DY;
    auto x = _p.X * (dx + s) + X;
    auto y = _p.Y * (2 * dy) + Y;
    if(_p.X % 2 == 0)
        y += dy;
    return Point(x, y);
}

void BoardUI::DrawHex(olc::PixelGameEngine& _pge, int _x, int _y, float _s, olc::Pixel _c){
    int dx = _s * DX;
    int dy = _s * DY;
    _pge.FillRect(_x - dx, _y - dy, 2 * dx + 1, 2 * dy + 1, _c);
    _pge.FillTriangle(_x + dx, _y - dy, _x + _s , _y , _x + dx, _y + dy, _c);
    _pge.FillTriangle(_x - dx, _y - dy, _x - _s , _y , _x - dx, _y + dy, _c);
}

void BoardUI::DrawCity(olc::PixelGameEngine& _pge, int _x, int _y, float _s, std::shared_ptr<City> _c, std::shared_ptr<GameState> _gs){
    DrawHex(_pge, _x, _y, _s * 0.6, olc::BLACK);
    auto n = _gs->B->GetNNeighbouring(_c->Location, _c->ActionSlots.size(), {1});
    auto nIt = 0;
    for(int i = 0; i < _c->ActionSlots.size() && nIt < n.size(); i++){
        auto ast = _c->ActionSlots[i]->Name;
        if(_gs->Settings->ActionSlots.find(ast) != _gs->Settings->ActionSlots.end()){
            auto type = _gs->Settings->ActionSlots[ast].VisualType;
            if(type != 0){
                auto sp = ToScreenCoords(n[nIt]);
                drawIndustry(_pge, sp.X, sp.Y, _s, type);
                nIt++;
            }
        }
    }
    if(S > 10){
        if(S > 20){
            if(S > 40){
                if(S > 80)
                    _pge.DrawString(_x, _y, _c->Name, olc::WHITE, 4);
                else
                    _pge.DrawString(_x, _y, _c->Name, olc::WHITE, 3);
            }
            else
                _pge.DrawString(_x, _y, _c->Name, olc::WHITE, 2);
        }
        else
            _pge.DrawString(_x, _y, _c->Name, olc::WHITE, 1);
    }
}


void BoardUI::drawIndustry(olc::PixelGameEngine& _pge, int _x, int _y, float _s, int _t){
    auto s = 0.5 * _s;
    auto color = olc::Pixel(75, 75, 75);
    switch (_t) {
        case 1:
            DrawHex(_pge, _x, _y, s, color);
            break;
        case 2:
            _pge.FillCircle(_x, _y, s, color);
            break;
        case 3:
            _pge.FillRect(_x - s, _y - s, 2 * s, 2 * s, color);
            break;
        default:
            break;
    }
}

void BoardUI::Zoom(int _x, int _y, int _z){
    if ((S > 7 || _z > 0) && (S < 100 || _z < 0)){
        float z = 0;
        if(_z < -2)
            z = -1;
        if(_z > 2)
            z = 1;
        auto dx = _x - X;
        auto dy = _y - Y;
        float scale = 1.0 + 0.1 * z;
        S *= scale;
        dx *= scale;
        dy *= scale;
        X = _x - dx;
        Y = _y - dy;
    }
}

void BoardUI::Move(int _x, int _y){
    X += _x;
    Y += _y;
}

bool BoardUI::inScreen(int x, int y, int w, int h){
    if(x + S < -1)
        return false;
    if(y + S < -1)
        return false;
    if(x - S > w + 1)
        return false;
    if(y - S > h + 1)
        return false;
    return true;
}

Point BoardUI::ScreenToTile(int _x, int _y){
    auto impP = Point(_x, _y);
    int xt = (_x - X) / (S + DX * S);
    int yt = (_y - Y) / (2 * DY * S);
    std::vector<int> xs {xt, xt + 1};
    std::vector<int> ys {yt - 1, yt, yt + 1};
    auto res = Point(-1, -1);
    float md = 100000.0;
    for(const auto& x : xs){
        for(const auto& y : ys){
            auto nres = Point(x, y);
            auto tp = ToScreenCoords(nres);
            auto nmd = tp.Distance(impP);
            if(md > nmd){
                res = nres;
                md = nmd;
            }
        }
    }
    return res;
}
