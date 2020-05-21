//
//  GameUI.cpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 07/11/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#include "GameUI.hpp"

GameState InitGameState();

GameUI::GameUI(std::shared_ptr<GameRunner> _gr){
    Runner = _gr;
}

void GameUI::InitUI(atk::UI &_ui){
    auto bt = std::make_shared<atk::Button>("Run", "RUN");
    auto bt1 = std::make_shared<atk::Button>("Turn Info", "TURNINFO");
    auto box = std::make_shared<atk::Box>(0, 0, 400 * 3, bt->Height + 2 * 3);
    box->Pad = 3;
    box->AutoArange(bt, true, true);
    box->AddChild(bt);
    box->AutoArange(bt1, true, true, atk::dLeft, bt);
    box->AddChild(bt1);
    _ui.AddRootFrame(box);
}

void GameUI:: HandleEvent(atk::UI & _ui, std::string _input){
    auto inp = StandartParse(_input);
    if(std::get<0>(inp) == "RUN"){
        Runner->Run();
        UpdateUI();
    }
    if(std::get<0>(inp) == "TURNINFO"){
        if(tiWindow == nullptr){
            tiWindow = std::make_shared<TurnInfoWindow>(Runner->GState->Players[0]);
        }
        _ui.AddRootFrame(tiWindow);
        UpdateUI();
    }
    if(std::get<0>(inp) == "SELECTAS"){
        auto selector = std::make_shared<ActionSlotSelector>(std::get<1>(inp)[0], Runner->GState->Settings);
        _ui.AddRootFrame(selector);
    }
    if(std::get<0>(inp) == "MAKEAS"){
        //city,astype,wid
        auto c = std::get<1>(inp)[0];
        auto ast = std::get<1>(inp)[1];
        auto inpv = Runner->GState->Settings->ActionSlots[ast];
        if(inpv.InputType.size() == 0){
            auto t = Turn();
            t.TurnType = ttAddActionSlot;
            auto cID = Runner->GState->citiesByName[c];
            t.Input = {std::to_string(cID), ast};
            Runner->GState->Players[0].Turns.push_back(t);
            UpdateUI();
        }
        else{
            auto dts = ActionVarForm::Descriptors(inpv.InputType);
            auto f = std::make_shared<ActionVarForm>(inpv.InputType, dts, Runner->GState->Settings, "ASVALS:" + c + "," + ast);
            _ui.AddRootFrame(f);
        }
        _ui.RemoveRoot(std::stoi(std::get<1>(inp)[2]));
    }
    if(std::get<0>(inp) == "ASVALS"){
        auto c = std::get<1>(inp)[0];
        auto ast = std::get<1>(inp)[1];
        auto id = std::stoi(std::get<1>(inp)[2]);
        auto f = std::dynamic_pointer_cast<atk::Form>(_ui.GetFrame(id));
        if(f != nullptr){
            auto t = Turn();
            t.TurnType = ttAddActionSlot;
            auto cID = Runner->GState->citiesByName[c];
            t.Input = {std::to_string(cID), ast};
            t.Input.insert(t.Input.end(), f->Output.begin(), f->Output.end());
            Runner->GState->Players[0].Turns.push_back(t);
            UpdateUI();
        }
    }
}


void GameUI::AddCity(int _c, atk::UI &_ui){
    auto c = Runner->GState->Cities[_c];
    auto t = std::tuple<int, std::shared_ptr<CityWindow>>(_c, std::make_shared<CityWindow>(*c, Runner->GState, 400));
    cities_.push_back(t);
    _ui.AddRootFrame(std::get<1>(t));
}

void GameUI::UpdateUI(){
    for(auto& ct : cities_){
        auto c = std::get<1>(ct);
        c->Update(*Runner->GState->Cities[std::get<0>(ct)], Runner->GState);
    }
    if(tiWindow != nullptr){
        tiWindow->Update(Runner->GState->Players[0]);
    }
}

Resource::Resource(std::string _t, int _v) : atk::Box(0, 0, 30, 30){
    Pad = 3;
    auto t = std::make_shared<atk::Text>(0, 0, _t.substr(0, 3), -1, 1);
    AutoArange(t, true, true);
    val = std::make_shared<atk::Text>(0, 0, std::to_string(_v), -1, 1);
    AutoArange(val, true, false);
    AddChild(t);
    AddChild(val);
    ToolTip = _t;
}

void Resource::Update(int _v){
    val->UpdateText(std::to_string(_v));
}

Resources::Resources(ValueCollection& _vc, int _pad) : atk::Frame(){
    Pad = _pad;
    Height = Pad;
    Width = 0;
    std::shared_ptr<Resource> last = nullptr;
    for(const auto& v:_vc.Values){
        resources[v.first] = std::make_shared<Resource>(v.first, v.second);
        if(last == nullptr){
            Height += resources[v.first]->Height + Pad;
            AutoArange(resources[v.first], true, true);
        }
        else{
            AutoArange(resources[v.first], true, true, atk::dLeft, last);
        }
        Width = resources[v.first]->X + resources[v.first]->Width;
        AddChild(resources[v.first]);
        last = resources[v.first];
    }
}

void Resources::Update(ValueCollection& _vc){
    for(const auto& f : resources){
        RemoveChild(f.second);
    }
    resources.erase(resources.begin(), resources.end());
    std::shared_ptr<Resource> last = nullptr;
    Width = 0;
    for(const auto& v:_vc.Values){
        resources[v.first] = std::make_shared<Resource>(v.first.substr(0, 3), v.second);
        if(last == nullptr){
            AutoArange(resources[v.first], true, true);
        }
        else{
            AutoArange(resources[v.first], true, true, atk::dLeft, last);
        }
        Width = resources[v.first]->X + resources[v.first]->Width;
        AddChild(resources[v.first]);
        last = resources[v.first];
    }
}

void Resources::ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge){
    
}

Constraints::Constraints(ValueCollection& _static, ValueCollection& _running, int _w, int _pad) : atk::ContentBox(0, 0, _w, _pad){
    Pad = _pad;
    fill(_static, _running);
}

void Constraints::Update(ValueCollection& _static, ValueCollection& _running){
    RemoveAllChildren();
    fill(_static, _running);
    Restructure();
}

void Constraints::fill(ValueCollection& _static, ValueCollection& _running){
    auto width = Width - (2 * Pad);
    for(const auto& v:_static.Values){
        auto rRes = 0;
        if(_running.Values.find(v.first) != _running.Values.end()){
            rRes = _running.Get(v.first);
        }
        auto str = v.first.substr(0, 3) + ":" + std::to_string(rRes) +"/" + std::to_string(v.second);
        auto vt = std::make_shared<atk::Text>(0, Pad, str, width);
        vt->ToolTip = v.first;
        AddChild(vt);
    }
}

ProductionSlot::ProductionSlot(std::shared_ptr<ActionSlot> _actionSlot, std::shared_ptr<GameState> _gs, int _w) : atk::Box(0, 0, _w, 0){
    Pad = 3;
    auto timeToEx = " :" + std::to_string(_actionSlot->TimeOutEnd > _gs->Time ? _actionSlot->TimeOutEnd - _gs->Time : 0) ;
    auto tt = std::make_shared<atk::Text>(Pad, Pad, _actionSlot->Name + timeToEx);
    AddChild(tt);
    Height = 60 + 7 * Pad;
    auto prsl = std::dynamic_pointer_cast<ProduceActionSlot>(_actionSlot);
    if(prsl != nullptr){
        auto constraints = prsl->Input.Intersect(prsl->Output);
        auto input = prsl->Input.Difference(constraints);
        input.Normalize();
        auto output = prsl->Output.Difference(constraints);
        output.Normalize();
        auto inp = std::make_shared<Resources>(input, Pad);
        AutoArange(inp, true, false);
        AddChild(inp);
        auto arrow = std::make_shared<atk::Text>(0, 0, "=>");
        AutoArange(arrow, true, false, atk::dLeft, inp);
        arrow->Y -= 5;
        AddChild(arrow);
        auto out = std::make_shared<Resources>(output, Pad);
        AutoArange(out, true, false, atk::dLeft, arrow);
        AddChild(out);
        auto cons = std::make_shared<Resources>(constraints, Pad);
        AutoArange(cons, false, true);
        AddChild(cons);
    }
    auto trsl = std::dynamic_pointer_cast<TransportActionSlot>(_actionSlot);
    if(trsl != nullptr){
        auto goods = std::make_shared<Resources>(trsl->Goods, Pad);
        AutoArange(goods, true, false);
        AddChild(goods);
        auto text = std::make_shared<atk::Text>(0, 0, "To " + std::to_string(trsl->Destination));
        AutoArange(text, true, false, atk::dLeft, goods);
        text->Y -= 5;
        AddChild(text);
        auto cons = std::make_shared<Resources>(trsl->Constraints, Pad);
        AutoArange(cons, false, true);
        AddChild(cons);
    }
}

ProductionSlots::ProductionSlots(std::vector<std::shared_ptr<ActionSlot>> _actionSlots, std::shared_ptr<GameState> _gs, int _w, int _h, int _bSize) : atk::ListBox(0, 0, _w, _h, _bSize){
    for(const auto& a : _actionSlots){
        auto ps = std::make_shared<ProductionSlot>(a, _gs, ListItemWidth);
        AddItem(ps);
    }
}


void ProductionSlots::Update(std::vector<std::shared_ptr<ActionSlot>> _actionSlots, std::shared_ptr<GameState> _gs){
    EmptyList();
    for(const auto& a : _actionSlots){
        auto ps = std::make_shared<ProductionSlot>(a, _gs, ListItemWidth);
        AddItem(ps);
    }
}

CityWindow::CityWindow(City& _c, std::shared_ptr<GameState> _gs, int _w) : atk::ContentWindow(30, 30, _w){
    bDrag->SetText(_c.Name + ": " + std::to_string(_c.ID));
    auto width = Width - (2 * Pad);
    auto tt = std::make_shared<atk::Text>(0, Pad, _c.Name, width, 3);
    AddChild(tt);
    auto selector = std::make_shared<atk::Button>("New Action", "SELECTAS:" + _c.Name);
    auto buttonBox = std::make_shared<atk::Box>(0, 0, width, selector->Height + 2 * Pad);
    buttonBox->Pad = Pad;
    buttonBox->AutoArange(selector, true, true);
    buttonBox->AddChild(selector);
    AddChild(buttonBox);
    auto staticC = _c.StaticConstraints();
    c = std::make_shared<Constraints>(staticC, _c.RunningConstraints, width, Pad);
    r = std::make_shared<Resources>(_c.Resources, Pad);
    ps = std::make_shared<ProductionSlots>(_c.ActionSlots, _gs, width, 429, 20);
    auto nw = std::make_shared<atk::ContentBox>(width - 2 * Pad);
    for(const auto& n : _c.Neighbours){
        auto name = std::to_string(n.first);
        auto txt = std::make_shared<atk::Text>(name, nw->Width - 2 * nw->Pad);
        nw->AddChild(txt);
    }
    std::map<std::string, std::shared_ptr<atk::Frame>> tapImp;
    tapImp["constraints"] = c;
    tapImp["resources"] = r;
    tapImp["action slots"] = ps;
    tapImp["neighbours"] = nw;
    auto tw = std::make_shared<atk::TabBox>(width, 0, Pad, 2, tapImp);
    for(const auto& t : tapImp){
        t.second->X -= Pad;
        t.second->Y -= Pad;
    }
    AddChild(tw);
}

void CityWindow::Update(City& _c, std::shared_ptr<GameState> _gs){
    auto staticC = _c.StaticConstraints();
    c->Update(staticC, _c.RunningConstraints);
    r->Update(_c.Resources);
    ps->Update(_c.ActionSlots, _gs);
    Restructure();
}

ActionSlotSelector::ActionSlotSelector(std::string _c, std::shared_ptr<GameSettings> _gs) : atk::ContentWindow(400, 400, 200){
    bDrag->SetText("Select");
    auto last = bClose;
    for(auto const& asmt : _gs->ActionSlots){
        auto as = std::make_shared<atk::Button>(0, 0, 194, 30, asmt.first, "MAKEAS:" + _c + "," + asmt.first);
        AutoArange(as, true, true, atk::dUp, bClose);
        AddChild(as);
    }
}

TurnInfo::TurnInfo(Turn& _t, int _w, int _pad) : atk::ContentBox(0, 0, _w, _pad){
    auto width = Width - 2 * Pad;
    std::string text = "Add " + _t.Input[1] + " To " + _t.Input[0];
    auto title = std::make_shared<atk::Text>(0, 0, text, width, 1);
    AddChild(title);
    text = "";
    for(int i = 2; i < _t.Input.size(); i++)
        text += _t.Input[i] + " ";
    auto values = std::make_shared<atk::Text>(0, 0, text, width, 1);
    AddChild(values);
}
    
TurnInfoWindow::TurnInfoWindow(Player& _p) : atk::ContentWindow(500, 200, 300){
    bDrag->SetText("Queued Turns");
    auto width = Width - 2 * Pad;
    list = std::make_shared<atk::ListBox>(0, 0, width, 300, bSize);
    AddChild(list);
    Update(_p);
}

void TurnInfoWindow::Update(Player& _p){
    list->EmptyList();
    for(auto& t : _p.Turns){
        auto te = std::make_shared<TurnInfo>(t, list->ListItemWidth, Pad);
        list->AddItem(te);
    }
}

void ActionSlotSelector::SetID(int _id, std::vector<int> _parents){
    Window::SetID(_id, _parents);
    for(const auto& c : Children){
        auto b = std::dynamic_pointer_cast<atk::Button>(c);
        if(b != nullptr && b->OnClick.substr(0, 6) == "MAKEAS"){
            b->OnClick += "," + std::to_string(_id);
        }
    }
}

ActionVarForm::ActionVarForm(std::vector<InputDescriptor> _ids, std::vector<std::string> _descr, std::shared_ptr<GameSettings> _gs, std::string _event) : atk::Form(_descr, _event){
    ids = _ids;
    gs = _gs;
};

std::string ActionVarForm::Validate(){
    auto res = atk::Form::Validate();
    if(res == ""){
        for(auto i = 0; i < Output.size(); i++){
            auto correct = gs->ValidateInput(ids[i], Output[i]);
            if(!correct){
                return Questions[i] + " is not " + gs->InputTypeDescription(ids[i]);
            }
        }
    }
    return res;
}

std::vector<std::string> ActionVarForm::Descriptors(std::vector<InputDescriptor> _ids){
    auto res = std::vector<std::string>();
    for(const auto& id : _ids){
        res.push_back(id.Description);
    }
    return res;
}







