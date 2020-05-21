//
//  GameUI.hpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 07/11/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

/*
 GUI implementation for game.
 */

#ifndef GameUI_hpp
#define GameUI_hpp

#include <stdio.h>
#include "UIFrames.hpp"
#include "Objects.hpp"
#include "Actionslots.hpp"
#include "GameController.hpp"

class Resource : public atk::Box{
    std::shared_ptr<atk::Text> val;
public:
    Resource(std::string _t, int _v);
    void Update(int _v);
};

class Resources : public atk::Frame{
    std::shared_ptr<Frame> content;
    std::map<std::string, std::shared_ptr<Resource>> resources;
public:
    Resources(ValueCollection& _vc, int _pad);
    void Update(ValueCollection& _vc);
    void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override;
};

class Constraints : public atk::ContentBox{
    void fill(ValueCollection& _static, ValueCollection& _running);
public:
    Constraints(ValueCollection& _static, ValueCollection& _running, int _w, int _pad);
    void Update(ValueCollection& _static, ValueCollection& _running);
};

class ProductionSlot : public atk::Box{
public:
    ProductionSlot(std::shared_ptr<ActionSlot> _actionSlot, std::shared_ptr<GameState> _gs, int _w);
};

class ProductionSlots : public atk::ListBox{
public:
    ProductionSlots(std::vector<std::shared_ptr<ActionSlot>> _actionSlots, std::shared_ptr<GameState> _gs, int _w, int _h, int _bSize);
    void Update(std::vector<std::shared_ptr<ActionSlot>> _actionSlots, std::shared_ptr<GameState> _gs);
};

class CityWindow : public atk::ContentWindow{
    std::shared_ptr<Resources> r;
    std::shared_ptr<Constraints> c;
    std::shared_ptr<ProductionSlots> ps;
public:
    CityWindow(City& _c, std::shared_ptr<GameState> _gs, int _w);
    void Update(City& _c, std::shared_ptr<GameState> _gs);
};

class ActionSlotSelector : public atk::ContentWindow{
public:
    ActionSlotSelector(std::string _c, std::shared_ptr<GameSettings> _gs);
    void SetID(int _id, std::vector<int> _parents) override;
};

class TurnInfo : public atk::ContentBox{
public:
    TurnInfo(Turn& _t, int _w, int _pad);
};

class TurnInfoWindow : public atk::ContentWindow{
    std::shared_ptr<atk::ListBox> list;
public:
    TurnInfoWindow(Player& _p);
    void Update(Player& _p);
};

class ActionVarForm : public atk::Form{
    std::vector<InputDescriptor> ids;
    std::shared_ptr<GameSettings> gs;
public:
    ActionVarForm(std::vector<InputDescriptor> _ids, std::vector<std::string> _descr, std::shared_ptr<GameSettings> _gs, std::string _event);
    std::string Validate() override;
    static std::vector<std::string> Descriptors(std::vector<InputDescriptor> _ids);
};

class GameUI : public atk::UIState{
    std::map<int, std::shared_ptr<CityWindow>> cities;
    std::vector<std::tuple<int, std::shared_ptr<CityWindow>>> cities_;
    std::shared_ptr<TurnInfoWindow> tiWindow;
public:
    std::shared_ptr<GameRunner> Runner;
    GameUI(std::shared_ptr<GameRunner> _gr);
    void HandleEvent(atk::UI & _ui, std::string _input) override;
    void InitUI(atk::UI& _ui);
    void AddCity(int _c, atk::UI &_ui);
    void UpdateUI();
};
#endif /* GameUI_hpp */
