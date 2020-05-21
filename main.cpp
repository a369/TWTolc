#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include "UIFrames.hpp"
#include "GameUI.hpp"
#include "GameSetting.hpp"
#include "GameBoardUI.hpp"
#include "GenerateGame.hpp"
#include "Serializer.hpp"


class Example : public olc::PixelGameEngine {
    atk::UI ui;
    std::shared_ptr<GameUI> uiState;
    BoardUI bui;
    int xd;
    int yd;
    Point ko;
    std::vector<Point> testPoints;
public:
    Example(std::shared_ptr<GameUI> _gui) : ui(_gui), bui(100, 100, 20), ko(-1, -1){
        sAppName = "Example";
        uiState = _gui;
        xd = -1;
        yd = -1;
    }
    
    bool OnUserCreate() override {
        // Called once at the start, so create things here
        uiState->InitUI(ui);
        return true;
    }
    
    void test(olc::PixelGameEngine& _pge, int _i){
        if(_i > 0){
            _pge.FillRect(_i, _i, 400, 400, olc::BLACK);
            test(_pge, _i - 1);
        }
    }
    
    bool OnUserUpdate(float fElapsedTime) override {
        Clear(olc::Pixel(180, 200, 255));
        //Clear(olc::Pixel(20, 20, 20));
        auto& t = *this;
        // called once per frame
        auto mp = Point(GetMouseX(), GetMouseY());
        if(GetMouse(0).bPressed && ui.InUI(mp.X, mp.Y) == -1 && mp.Y > 0){
            xd = mp.X;
            yd = mp.Y;
            ko = mp;
        }
        if(xd >= 0){
            if(GetMouse(0).bHeld){
                bui.Move(mp.X - xd, mp.Y - yd);
                xd = mp.X;
                yd = mp.Y;
            }
            if(GetMouse(0).bReleased){
                if(ko.Distance(mp) < 20){
                    HandleBoardKlik();
                }
                xd = -1;
                yd = -1;
                ko = Point(xd, yd);
            }
        }
        if(GetMouseWheel() != 0 && ui.InUI(mp.X, mp.Y) == -1){
            bui.Zoom(mp.X, mp.Y, GetMouseWheel());
        }
        if(GetMouse(1).bPressed && ui.InUI(mp.X, mp.Y) == -1 && mp.Y > 0){
            auto bc = bui.ScreenToTile(mp.X, mp.Y);
            auto c = uiState->Runner->GState->CityFromLocation(bc);
            if(c >= 0){
                ui.InsertString(std::to_string(c));
            }
        }
        
        bui.DrawBoard(static_cast<olc::PixelGameEngine&>(t), uiState->Runner->GState, testPoints);
        ui.UserUpdate(static_cast<olc::PixelGameEngine&>(t));
        ui.Draw(static_cast<olc::PixelGameEngine&>(t));
        return true;
    }
    
    void HandleBoardKlik(){
        auto bc = bui.ScreenToTile(ko.X, ko.Y);
        auto c = uiState->Runner->GState->CityFromLocation(bc);
        if(c >= 0){
            uiState->AddCity(c, ui);
        }
    }
};

int main(int argc, char const *argv[]) {
    std::string setting;
    std::string line;
    std::ifstream myfile ("/Users/adriaan/Desktop/Hobby/TWTBarbosa/TWTOLC/TWTolc/TWTolc/setting.txt");
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            setting += line;
        }
        myfile.close();
    }
    auto gs = MakeGameState(std::make_shared<DinamicGameSettings>(setting));
    auto gr = std::make_shared<GameRunner>(gs);
    auto gui = std::make_shared<GameUI>(gr);
	Example demo(gui);
    auto scale = 1;
    auto w = 400 * 3;
    auto h = 250 * 3;
	if (demo.Construct(w / scale, h / scale, scale, scale))
		demo.Start();

	return 0;
}


