//
//  UIFrames.hpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 17/10/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

/*
Custom GUI lib.
 */

#ifndef UIFrames_hpp
#define UIFrames_hpp

#include "olcPixelGameEngine.h"
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <unordered_set>
#include <tuple>

namespace atk{
    class UI;
    
    static const olc::Pixel BackGroundColor(200, 200, 200), BorderColor(0, 0, 0), TextColor(0, 0, 0), HighLightColor(255, 123, 0), AccentColler(200, 150, 150);
    //static const olc::Pixel BackGroundColor(0, 0, 0), BorderColor(100, 100, 100), TextColor(255, 255, 255), HighLightColor(0, 0, 0), AccentColler(0, 0, 0);
    
    enum Direction {dUp, dDown, dLeft, dRight, dNone};
    
    // Virtual class that needs to be inherited to handle GUI events.
    class UIState{
    public:
        virtual void HandleEvent(UI & _ui, std::string _input) = 0;
        std::tuple<std::string, std::vector<std::string>> StandartParse(std::string _input, std::string _sep = ":"){
            std::string key = _input;
            auto ids = std::vector<std::string>();
            auto pos = _input.find(_sep);
            if(pos != std::string::npos){
                key = _input.substr(0, pos);
                auto rem = _input.substr(pos + 1);
                auto pos_ = rem.find(",");
                while(pos_ != std::string::npos){
                    ids.push_back(rem.substr(0, pos_));
                    rem = rem.substr(pos_ + 1);
                    pos_ = rem.find(",");
                }
                ids.push_back(rem);
            }
            return std::tuple<std::string, std::vector<std::string>>(key, ids);
        }
    };
    
    // Virtual base class to al UI elements
    class Frame{
        int newChildCounter;
    public:
        int Parent;
        int ID;
        int X;
        int Y;
        int Width;
        int Height;
        int Pad;
        bool Visible;
        std::string ToolTip;
        std::vector<std::shared_ptr<Frame>> Children;
        std::vector<int> RemovedChildren;
        std::vector<int> AddedChildren;
        Frame(){
            Visible = true;
            Pad = -1;
            RemovedChildren = std::vector<int>();
            Parent = -2;
            newChildCounter = -1;
            ToolTip = "";
        };
        // Sets _id of frame, should only be called by UI controller.
        virtual void SetID(int _id, std::vector<int> _parents){
            ID = _id;
        };
        // Add Child Frame to This Frame.
        virtual void AddChild(std::shared_ptr<Frame> _frame){
            _frame->ID = newChildCounter--;
            Children.push_back(_frame);
            if(_frame->Pad < 0)
                _frame->Pad = Pad;
        };
        // Draws the Frame itself.
        virtual void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) = 0;
        // Calls the concrete draw functions of the children of this Frame.
        void Draw(int _x, int _y, olc::PixelGameEngine& _pge){
            if(Visible){
                ConcreteDraw(_x, _y, _pge);
                for(auto const& child : Children){
                    child->Draw(_x + X, _y + Y, _pge);
                }
            }
        };
        // Returns true if point is in this Frame.
        bool InFrame(int _x, int _y){
            return Visible && _x >= X && _x <= X + Width && _y >= Y && _y <= Y + Height;
        };
        // Calls onKlick of children.
        std::string OnKlick(int _x, int _y){
            auto cx = _x - X;
            auto cy = _y - Y;
            for(auto const& child : Children){
                if(child->InFrame(cx, cy)){
                    auto res = child->OnKlick(cx, cy);
                    if(res.length() != 0){
                        return res;
                    }
                }
            }
            return ConcreteOnKlick();
        };
        virtual std::string ConcreteOnKlick(){
            return "";
        };
        void SetVisibility(bool _v){
            Visible = _v;
            for(auto const& c : Children)
                c->SetVisibility(_v);
        }
        void RemoveChild(std::shared_ptr<Frame> _frame){
            auto id = _frame->ID;
            int ind = -1;
            for(int i = 0; i < Children.size(); i++){
                if(Children[i]->ID == id){
                    ind = i;
                    break;
                }
            }
            if(ind != -1){
                if(_frame->ID >= 0){
                RemovedChildren.push_back(id);
                }
                Children.erase(Children.begin() + ind);
            }
        }
        void RemoveAllChildren(){
            for(auto const& c : Children){
                if(c->ID >= 0){
                    RemovedChildren.push_back(c->ID);
                }
            }
            Children.clear();
        }
        void ReplaceChild(std::shared_ptr<Frame> _old, std::shared_ptr<Frame> _new){
            auto ind = -1;
            for(int i = 0; i < Children.size(); i++){
                if(_old->ID == Children[i]->ID){
                    ind = i;
                }
            }
            if(ind  != -1){
                RemovedChildren.push_back(Children[ind]->ID);
                Children[ind] = _new;
            }
        }
        std::string GetToolTip(int _x, int _y){
            auto cx = _x - X;
            auto cy = _y - Y;
            for(auto const& child : Children){
                if(child->InFrame(cx, cy)){
                    auto res = child->GetToolTip(cx, cy);
                    if(res.length() != 0){
                        return res;
                    }
                }
            }
            return ToolTip;
        }
        // helper functions
        // Graphical helpers
        void DrawBox(olc::PixelGameEngine& _pge, int _x, int _y, int _padding = 0, olc::Pixel _color = BorderColor){
            _pge.DrawRect(_x + X + _padding, _y + Y + _padding, Width - (2 * _padding), Height - (2 * _padding), _color);
        };
        void FillWithStripes(olc::PixelGameEngine& _pge, int _x, int _y, int _sHeight, int _sInterfal, olc::Pixel _c = AccentColler, int _w = -1, int _h = -1){
            if(_w < 0) _w = Width;
            if(_h < 0) _h = Height;
            auto end = 0;
            auto stripe = false;
            while(end < _h){
                if(stripe){
                    _pge.FillRect(_x, _y + end, _w, _sHeight, _c);
                    end += _sHeight;
                    stripe = false;
                }
                else{
                    end += _sInterfal;
                    stripe = true;
                }
            }
        }
        // Arange helpers
        void AutoArange(std::shared_ptr<Frame> _aranged, bool _left, bool _up, Direction _clip, std::shared_ptr<Frame> _comparer){
            auto clip = 0;
            switch (_clip) {
                case dUp:
                    clip = _comparer->Y + _comparer->Height;
                    break;
                case dDown:
                    clip = _comparer->Y;
                    break;
                case dLeft:
                    clip = _comparer->X + _comparer->Width;
                    break;
                case dRight:
                    clip = _comparer->X;
                    break;
            }
            AutoArange(_aranged, _left, _up, _clip, clip);
        }
        void AutoArange(std::shared_ptr<Frame> _aranged, bool _left, bool _up, Direction _clip = dNone, int _clipv = 0){
            auto xmin = 0;
            auto xmax = Width;
            auto ymin = 0;
            auto ymax = Height;
            switch (_clip) {
                case dUp:
                    ymin = _clipv;
                    break;
                case dDown:
                    ymax = _clipv;
                    break;
                case dLeft:
                    xmin = _clipv;
                    break;
                case dRight:
                    xmax = _clipv;
                    break;
            }
            if(_left){
                _aranged->X = xmin + Pad;
            }
            else{
                _aranged->X = xmax - _aranged->Width - Pad;
            }
            if(_up){
                _aranged->Y = ymin + Pad;
            }
            else{
                _aranged->Y = ymax - _aranged->Height - Pad;
            }
        };
        void CenterX(std::shared_ptr<Frame> _frame){
            _frame->X = Width / 2 - _frame->Width / 2;
        };
        void CenterY(std::shared_ptr<Frame> _frame){
            _frame->Y = Height / 2 - _frame->Height / 2;
        };
        int TextWidth(std::string _txt, int _size){
            auto maxL = 0;
            auto curL = 0;
            for(const auto& c : _txt){
                if(c != '\n')
                    curL++;
                else{
                    if(curL > maxL){
                        maxL = curL;
                    }
                    curL = 0;
                }
            }
            if(curL > maxL){
                maxL = curL;
            }
            return 8 * _size * maxL;
        }
        int TextHeight(std::string _txt, int _size){
            auto lines = 1;
            for(const auto& c : _txt){
                if(c == '\n')
                    lines++;
            }
            return 8 * _size * lines;
        }
    };
    
    class Text : public Frame{
    protected:
        std::string txt;
        std::string printTxt;
        int size;
        int maxChar;
        void updatePrintTxt(){
            if(txt.size() > 0){
                if(maxChar > 0){
                    if(Index > txt.size())
                        Index = txt.size() - 1;
                    auto cul = maxChar + Index;
                    if(cul > txt.size())
                        cul = txt.size();
                    printTxt = txt.substr(Index, cul);
                }
                else{
                    printTxt = txt;
                }
            }
            else{
                printTxt = txt;
            }
            if(!FixedSize){
                reSize();
            }
        };
        void reSize(){
            Width = TextWidth(printTxt, size);
            Height = TextHeight(printTxt, size);
        }
    public:
        int Index;
        bool FixedSize;
        olc::Pixel Color;
        Text(int _x, int _y, std::string _txt, int _maxWidth = -1, int _size = 2) : Frame(){
            Index = 0;
            size = _size;
            X = _x;
            Y = _y;
            txt = _txt;
            printTxt = txt;
            Color = TextColor;
            maxChar = -1;
            FixedSize = false;
            if(_maxWidth > 0){
                maxChar = _maxWidth / (_size * 8);
                FixedSize = true;
            }
            Width = _maxWidth;
            Height = TextHeight(txt, size);
            updatePrintTxt();
        };
        Text(std::string _txt, int _maxWidth = -1, int _size = 2) : Text(0, 0, _txt, _maxWidth, _size){
            
        }
        void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override {
            _pge.DrawString(_x + X, _y + Y, printTxt, Color, size);
        };
        void UpdateText(std::string _txt){
            txt = _txt;
            updatePrintTxt();
        };
        void AddStr(std::string _txt){
            txt += _txt;
            updatePrintTxt();
        }
        void RemStr(int _r){
            txt = txt.substr(0, txt.size() - _r);
            updatePrintTxt();
        }
        void SetIndexToEnd(){
            Index = txt.size() - maxChar;
            if(Index < 0)
                Index = 0;
            updatePrintTxt();
        }
        std::string GetText(){
            return txt;
        }
    };
    
    class Button : public Frame{
        int tSize;
    public:
        std::shared_ptr<Text> Txt;
        std::string OnClick;
        Button(int _x, int _y, int _w, int _h, std::string _txt, std::string _onClick) : Frame(){
            X = _x;
            Y = _y;
            Height = _h;
            Width = _w;
            Txt = std::make_shared<Text>(4, 4, _txt);
            AddChild(Txt);
            OnClick = _onClick;
            tSize = 2;
        };
        Button(std::string _txt, std::string _onClick, int _size = 2) : Frame(){
            X = 0;
            Y = 0;
            Txt = std::make_shared<Text>(2 * _size, 2 * _size, _txt);
            Width = Txt->Width + 2 * 2 * _size;
            Height = Txt->Height + 2 * 2 * _size;
            AddChild(Txt);
            OnClick = _onClick;
            tSize = _size;
        }
        void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override {
            _pge.FillTriangle(X + _x, Y + _y, X + _x, Y + _y + Height, X + _x + Width, Y + _y, olc::WHITE);
            _pge.FillTriangle(X + _x + Width, Y + _y + Height, X + _x, Y + _y + Height, X + _x + Width, Y + _y, olc::BLACK);
            _pge.FillRect(X + _x + 2, Y + _y + 2, Width - 4, Height - 4, BackGroundColor);
            //DrawBox(_pge, _x, _y, 0);
        };
        std::string ConcreteOnKlick() override {
            return OnClick;
        };
        virtual void SetText(std::string _txt, bool _dynamic = false){
            Txt->UpdateText(_txt);
            if(_dynamic){
                Width = Txt->Width + 2 * 2 * tSize;
                Height = Txt->Height + 2 * 2 * tSize;
            }
        }
        std::string GetText(){
            return Txt->GetText();
        }
    };
    
    class TextField : public Text{
    public:
        bool Active;
        TextField(int _w) : Text(0, 0, "", _w, 2){
            Active = false;
            Height = 25;
            Width = _w;
        };
        void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override {
            if(txt.size() == 0){
                _pge.DrawString(_x + X + 2, _y + Y + 5, "...", olc::DARK_GREY, 2);
            }
            else{
                _pge.DrawString(_x + X + 2, _y + Y + 5, printTxt, TextColor, 2);
            }
            DrawBox(_pge, _x, _y, 0);
            if(Active)
                DrawBox(_pge, _x, _y, 1);
        };
        std::string ConcreteOnKlick() override {
            return "TEXTF#" + std::to_string(ID);
        };
        void AddLetter(std::string _c){
            AddStr(_c);
            SetIndexToEnd();
        }
        void RemoveLetter(){
            RemStr(1);
            SetIndexToEnd();
        }
    };
    
    class Box : public Frame{
    public:
        olc::Pixel Color;
        Box(int _x, int _y, int _w, int _h, olc::Pixel _c = BackGroundColor) : Frame(){
            X = _x;
            Y = _y;
            Width = _w;
            Height = _h;
            Color = _c;
        };
        void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override {
            _pge.FillRect(X + _x, Y + _y, Width, Height, Color);
            DrawBox(_pge, _x, _y);
        };
    };
    
    class ContentBox : public Box{
    public:
        ContentBox(int _x, int _y, int _w, int _pad, olc::Pixel _c = BackGroundColor) : Box(_x, _y, _w, _pad, _c){
            Pad = _pad;
        };
        ContentBox(int _w) : Box(0, 0, _w, 3, BackGroundColor){
            Pad = 3;
        };
        void AddChild(std::shared_ptr<Frame> _frame) override{
            _frame->X = Pad;
            _frame->Y = Height;
            Height += Pad + _frame->Height;
            Frame::AddChild(_frame);
        };
        void Restructure(){
            Height = Pad;
            for(auto const& c : Children){
                c->X = Pad;
                c->Y = Height;
                Height += Pad + c->Height;
            }
        };
    };
    
    class DragBox : public Button{
    public:
        DragBox(int _x, int _y, int _w, int _h, std::string _txt) : Button(_x, _y, _w, _h, _txt, "DRAG#-1"){
            CenterX(Txt);
        }
        DragBox(std::string _txt) : Button(_txt, "DRAG#-1"){
            
        }
        void SetText(std::string _txt, bool _dynamic = false) override {
            Button::SetText(_txt);
            CenterX(Txt);
        }
        void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override {
            DrawBox(_pge, _x, _y);
        };
    };
    
    class ListElement : public Frame{
        int firstListItem;
        int lastListItem;
    public:
        olc::Pixel Color;
        ListElement(int _x, int _y, int _w, int _h, olc::Pixel _c = BackGroundColor) : Frame(){
            X = _x;
            Y = _y;
            Width = _w;
            Height = _h;
            Color = _c;
            firstListItem = 0;
            lastListItem = 0;
        };
        void AddChild(std::shared_ptr<Frame> _frame) override {
            Frame::AddChild(_frame);
            UpdateListItems();
        };
        void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override {
            _pge.FillRect(X + _x, Y + _y, Width, Height, Color);
            DrawBox(_pge, _x, _y, 0);
        };
        void UpdateListItems(){
            /*UpdateLastListItem*/{
                auto res = firstListItem;
                auto height = 0;
                if(res < Children.size())
                    height = (2 * Pad) + Children[res]->Height;
                while(res < Children.size() && height <= Height){
                    height += Pad + Children[res]->Height;
                    res++;
                }
                lastListItem = res;
            }
            auto height = Pad;
            for(int i = 0; i < Children.size(); i++){
                if(i >= firstListItem && i < lastListItem){
                    Children[i]->SetVisibility(true);
                    Children[i]->X = Pad;
                    Children[i]->Y = height;
                    height += Pad + Children[i]->Height;
                }
                else
                    Children[i]->SetVisibility(false);
                }
        };
        void Scroll(int _direction){
            auto nd = firstListItem + _direction;
            if(nd >= 0 && nd < Children.size()){
                firstListItem = nd;
                UpdateListItems();
            }
        };
        void RemoveAllChildren(){
            Frame::RemoveAllChildren();
            firstListItem = 0;
            lastListItem = 0;
        }
    };
    
    class TabBox : public Box{
        std::map<std::string, std::shared_ptr<Frame>> elements;
        std::vector<std::shared_ptr<Button>> buttons;
    public:
        TabBox(int _w, int _h, int _pad, int _tSize, std::map<std::string, std::shared_ptr<Frame>> _views) : Box(_pad, _pad, _w, _h){
            Pad = _pad;
            elements = _views;
            for(const auto& v : elements){
                auto b = std::make_shared<Button>(v.first.substr(0, 3), v.first);
                b->ToolTip = v.first;
                if(buttons.size() == 0){
                    AutoArange(b, true, true);
                    v.second->SetVisibility(true);
                }
                else{
                    AutoArange(b, true, true, dLeft, buttons[buttons.size() - 1]);
                    v.second->SetVisibility(false);
                }
                buttons.push_back(b);
                AutoArange(v.second, true, true, dUp, b);
                auto mh = v.second->Height + v.second->Y + Pad;
                if(mh > Height)
                    Height = mh;
                AddChild(b);
                AddChild(v.second);
            }
        }
        void SetID(int _id, std::vector<int> _parents) override{
            ID = _id;
            auto idstr = std::to_string(_id);
            for(const auto& b : buttons){
                b->OnClick = "TABBOX#" + idstr + "," + b->OnClick;
            }
        };
        void SwitchTo(std::string _tap){
            for(const auto& e : elements){
                if(e.first == _tap){
                    e.second->SetVisibility(true);
                }
                else{
                    e.second->SetVisibility(false);
                }
            }
        }
        
    };
    
    class Header : public Box{
    public:
        Header(int _x, int _y, int _w, int _h) : Box(_x, _y, _w, _h){
            
        }
        void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override {
            FillWithStripes(_pge, X + _x, Y + _y, 2, 2);
            DrawBox(_pge, _x, _y);
        };
    };
    
    class Window : public Frame{
    protected:
        std::shared_ptr<Button> bClose;
        std::shared_ptr<DragBox> bDrag;
        std::shared_ptr<Header> header;
        int bSize;
    public:
        olc::Pixel Color;
        Window(int _x, int _y, int _w, int _h, olc::Pixel _c = BackGroundColor) : Frame(){
            X = _x;
            Y = _y;
            Width = _w;
            Height = _h;
            Color = _c;
            Pad = 3;
            bSize = 20;
            bClose = std::make_shared<Button>(_w - (Pad + bSize), Pad, bSize, bSize, "X", "X");
            bDrag = std::make_shared<DragBox>(Pad, Pad, _w - (3 * Pad + bSize), bSize, "Test");
            header = std::make_shared<Header>(0, 0, Width, bClose->Height + 2 * Pad);
            AddChild(header);
            AddChild(bClose);
            AddChild(bDrag);
        };
        void SetID(int _id, std::vector<int> _parents) override{
            ID = _id;
            auto idstr = std::to_string(_id);
            bClose->OnClick = "X#" + idstr;
        };
        void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override {
            _pge.FillRect(X + _x + 4, Y + _y + 4, Width, Height, olc::Pixel(20, 20, 20));
            _pge.FillRect(X + _x, Y + _y, Width, Height, Color);
            FillWithStripes(_pge, X + _x, Y + _y, 2, 2, AccentColler, Width, bClose->Height + 2 * Pad);
            DrawBox(_pge, _x, _y);
        };
        void SetTitle(std::string _title){
            bDrag->SetText(_title);
        };
    };
    
    class Form : public Window{
        std::vector<std::shared_ptr<TextField>> tfs;
        std::shared_ptr<Button> bCancel;
        std::shared_ptr<Button> bOk;
    public:
        std::string Event;
        std::vector<std::string> Output;
        std::vector<std::string> Questions;
        Form(std::vector<std::string>& _qs, std::string _event) : Window(300, 300, 400, 0){
            Pad = 3;
            std::shared_ptr<Frame> last = header;
            Questions = _qs;
            for(const auto& q: _qs){
                auto qf = std::make_shared<Text>(0, 0, q);
                AutoArange(qf, true, true, dUp, last);
                AddChild(qf);
                auto af = std::make_shared<TextField>(200);
                AutoArange(af, false, true, dUp, last);
                AddChild(af);
                tfs.push_back(af);
                last = af;
                SetTitle("Form");
            }
            bCancel = std::make_shared<Button>("Cancel", "");
            AutoArange(bCancel, true, true, dUp, last);
            AddChild(bCancel);
            bOk = std::make_shared<Button>("OK", "");
            AutoArange(bOk, false, true, dUp, last);
            AddChild(bOk);
            last = bCancel;
            Height = last->Y + last->Height + Pad;
            Event = _event;
        };
        void SetID(int _id, std::vector<int> _parents) override {
            Window::SetID(_id, _parents);
            auto idstr = std::to_string(_id);
            bCancel->OnClick = "FORMC#" + idstr;
            bOk->OnClick = "FORMK#" + idstr;
        };
        virtual std::string Validate(){
            for(const auto& tf : tfs){
                if(tf->GetText() == ""){
                    return "Not Filled In";
                }
            }
            return "";
        };
        void SetOutput(){
            for(const auto& tf : tfs){
                Output.push_back(tf->GetText());
            }
        };
    };
    
    class Error : public Window{
        std::shared_ptr<Text> msg;
    public:
        Error(std::string _msg) : Window(400, 400, TextWidth(_msg, 2) + 6, 63){
            msg = std::make_shared<Text>(0, 0, _msg);
            AutoArange(msg, true, true, dUp, header);
            msg->Y += 10;
            AddChild(msg);
            SetTitle("Error");
        }
    };

    class ListBox : public Box{
    protected:
        std::shared_ptr<Button> bUp;
        std::shared_ptr<Button> bDown;
        std::shared_ptr<ListElement> lb;
    public:
        int ListItemWidth;
        ListBox(int _x, int _y, int _w, int _h, int _bSize, olc::Pixel _c = BackGroundColor) : Box(_x, _y, _w, _h, _c){
            Pad = 3;
            ListItemWidth = _w - (5 * Pad + _bSize);
            lb = std::make_shared<ListElement>(Pad, Pad, _w - (3 * Pad + _bSize), _h - 2 * Pad, _c);
            AddChild(lb);
            bUp = std::make_shared<Button>(_w - (Pad + _bSize), Pad, _bSize, _bSize, "U", "UP");
            AddChild(bUp);
            bDown = std::make_shared<Button>(_w - (Pad + _bSize), _h - (Pad + _bSize), _bSize, _bSize, "D", "DOWN");
            AddChild(bDown);
        };
        void SetID(int _id, std::vector<int> _parents) override{
            Box::SetID(_id, _parents);
            auto idstr = std::to_string(_id);
            bUp->OnClick = "UP#" + idstr;
            bDown->OnClick = "DOWN#" + idstr;
        };
        void AddItem(std::shared_ptr<Frame> _frame){
            lb->AddChild(_frame);
        };
        void ConcreteDraw(int _x, int _y, olc::PixelGameEngine& _pge) override {
            _pge.FillRect(X + _x, Y + _y, Width, Height, Color);
            DrawBox(_pge, _x, _y);
        };
        int GetLBID(){
            return lb->ID;
        };
        void Scroll(int _direction){
            lb->Scroll(_direction);
        };
        void EmptyList(){
            lb->RemoveAllChildren();
        }
    };

    class ContentWindow : public Window{
    public:
        ContentWindow(int _x, int _y, int _w) : Window(_x, _y, _w, 0, BackGroundColor){
            Height = header->Height + Pad;
        }
        void AddChild(std::shared_ptr<Frame> _frame) override{
            _frame->X = Pad;
            _frame->Y = Height;
            Height += Pad + _frame->Height;
            Frame::AddChild(_frame);
        };
        void Restructure(){
            Height = header->Height + Pad;
            for(auto const& c : Children){
                if(c->ID != bClose->ID && c->ID != bDrag->ID && c->ID != header->ID){
                    c->X = Pad;
                    c->Y = Height;
                    Height += Pad + c->Height;
                }
            }
        };
    };
    
    // UI Class runs and controlls the UI and handles events.
    class UI{
    private:
        std::map<olc::Key, std::string> keyMap;
        // Map of all frames.
        std::map<int, std::shared_ptr<Frame>> frames;
        // Subclass of UIState to handle events.
        std::shared_ptr<UIState> State;
        // List of Root Frame IDs
        std::vector<int> roots;
        int idCounter;
        int xPr;
        int yPr;
        std::shared_ptr<TextField> tf;
        int getNewID(){
            return idCounter++;
        };
        void addChildren(std::shared_ptr<Frame> _frame, std::vector<int> _parents){
            _parents.push_back(_frame->ID);
            for(const auto& c : _frame->Children){
                c->SetID(getNewID(), _parents);
                c->Parent = _frame->ID;
                frames[c->ID] = c;
                if(!c->Children.empty()){
                    addChildren(c, _parents);
                }
            }
        };
        void removeChildren(){
            for(auto const& r : roots){
                removeForChild(frames[r]);
            }
        };
        void removeForChild(std::shared_ptr<Frame> _c){
            for(auto const& id : _c->RemovedChildren){
                frames.erase(id);
            }
            _c->RemovedChildren.empty();
            for(auto const& c : _c->Children){
                removeForChild(c);
            }
        };
        void addNewChildren(){
            for(const auto& r : roots){
                addNewChildren_(frames[r], std::vector<int>());
            }
        }
        void addNewChildren_(std::shared_ptr<Frame> _c, std::vector<int> _parents){
            _parents.push_back(_c->ID);
            for(const auto& c : _c->Children){
                if(c->Parent == -2){
                    c->SetID(getNewID(), _parents);
                    c->Parent = _c->ID;
                    addChildren(c, _parents);
                }
                else{
                    addNewChildren_(c, _parents);
                }
            }
        }
        void handleClick(std::string _imp){
            auto pRes = standartParse(_imp, "#");
            auto key = std::get<0>(pRes);
            auto var = std::get<1>(pRes);
            if(var.size() == 0){
                State->HandleEvent(*this, _imp);
            }
            else{
                auto com = key;
                auto id = std::stoi(var[0]);
                if(com == "X"){
                    RemoveRoot(id);
                }
                else if(com == "DOWN"){
                    auto frame = std::dynamic_pointer_cast<ListBox>(frames[id]);
                    frame->Scroll(1);
                }
                else if(com == "UP"){
                    auto frame = std::dynamic_pointer_cast<ListBox>(frames[id]);
                    frame->Scroll(-1);
                }
                else if(com == "TEXTF"){
                    tf = std::dynamic_pointer_cast<TextField>(frames[id]);
                    tf->Active = true;
                }
                else if(com == "FORMC"){
                    frames.erase(frames.find(id));
                    auto it = std::find(roots.begin(), roots.end(), id);
                    roots.erase(it);
                }
                else if(com == "FORMK"){
                    auto frame = std::dynamic_pointer_cast<Form>(frames[id]);
                    frame->SetOutput();
                    auto error = frame->Validate();
                    if(error == ""){
                        State->HandleEvent(*this, frame->Event + "," + std::to_string(id));
                        RemoveRoot(id);
                    }
                    else{
                        AddRootFrame(std::make_shared<Error>(error));
                    }
                }
                else if(com == "TABBOX"){
                    auto frame = std::dynamic_pointer_cast<TabBox>(frames[id]);
                    frame->SwitchTo(var[1]);
                }
            }
        };
        std::vector<int> getParentChain(int _id){
            auto res = std::find(roots.begin(), roots.end(), _id);
            if(res == roots.end()){
                auto v = getParentChain(frames[_id]->Parent);
                v.push_back(frames[_id]->Parent);
                return v;
            }
            else{
                return std::vector<int>();
            }
        }
        std::string clickRes(int _x, int _y){
            std::string res = "";
            for(const auto& r : roots){
                if(frames[r]->InFrame(_x, _y)){
                    res = frames[r]->OnKlick(_x, _y);
                    if(res != ""){
                        return res;
                    }
                    break;
                }
            }
            return res;
        }
        std::string getKeyText(olc::PixelGameEngine& _pge){
            std::string res = "";
            for(const auto& k : keyMap){
                auto b = _pge.GetKey(k.first);
                if(b.bPressed){
                    res = k.second;
                }
            }
            return res;
        }
        void drawToolTip(olc::PixelGameEngine& _pge){
            auto x = _pge.GetMouseX();
            auto y = _pge.GetMouseY();
            std::string res = "";
            for(const auto& r : roots){
                if(frames[r]->InFrame(x, y)){
                    res = frames[r]->GetToolTip(x, y);
                    if(res != ""){
                        break;
                    }
                    break;
                }
            }
            x += 12;
            y += 6;
            int w = 16 * res.size();
            auto h = 16;
            if(res != ""){
                _pge.FillRect(x, y, w, h, BackGroundColor);
                _pge.DrawRect(x, y, w, h, BorderColor);
                _pge.DrawString(x, y, res, TextColor, 2);
            }
        }
        void initKeymap(){
            keyMap[olc::NONE] = ""; keyMap[olc::A] = "a"; keyMap[olc::B] = "b"; keyMap[olc::C] = "c"; keyMap[olc::D] = "d"; keyMap[olc::E] = "e"; keyMap[olc::F] = "f"; keyMap[olc::G] = "g"; keyMap[olc::H] = "h"; keyMap[olc::I] = "i"; keyMap[olc::J] = "j"; keyMap[olc::K] = "k"; keyMap[olc::L] = "l"; keyMap[olc::M] = "m"; keyMap[olc::N] = "n"; keyMap[olc::O] = "o"; keyMap[olc::P] = "p"; keyMap[olc::Q] = "g"; keyMap[olc::R] = "r"; keyMap[olc::S] = "s"; keyMap[olc::T] = "t"; keyMap[olc::U] = "u"; keyMap[olc::V] = "v"; keyMap[olc::W] = "w"; keyMap[olc::X] = "x"; keyMap[olc::Y] = "y"; keyMap[olc::Z] = "z"; keyMap[olc::K0] = "0"; keyMap[olc::K1] = "1"; keyMap[olc::K2] = "2"; keyMap[olc::K3] = "3"; keyMap[olc::K4] = "4"; keyMap[olc::K5] = "5"; keyMap[olc::K6] = "6"; keyMap[olc::K7] = "7"; keyMap[olc::K8] = "8"; keyMap[olc::K9] = "9"; keyMap[olc::F1] = ""; keyMap[olc::F2] = ""; keyMap[olc::F3] = ""; keyMap[olc::F4] = ""; keyMap[olc::F5] = ""; keyMap[olc::F6] = ""; keyMap[olc::F7] = ""; keyMap[olc::F8] = ""; keyMap[olc::F9] = ""; keyMap[olc::F10] = ""; keyMap[olc::F11] = ""; keyMap[olc::F12] = ""; keyMap[olc::UP] = ""; keyMap[olc::DOWN] = ""; keyMap[olc::LEFT] = ""; keyMap[olc::RIGHT] = ""; keyMap[olc::SPACE] = ""; keyMap[olc::TAB] = ""; keyMap[olc::SHIFT] = ""; keyMap[olc::CTRL] = ""; keyMap[olc::INS] = ""; keyMap[olc::DEL] = ""; keyMap[olc::HOME] = ""; keyMap[olc::END] = ""; keyMap[olc::PGUP] = ""; keyMap[olc::PGDN] = ""; keyMap[olc::BACK] = ""; keyMap[olc::ESCAPE] = ""; keyMap[olc::RETURN] = ""; keyMap[olc::ENTER] = ""; keyMap[olc::PAUSE] = ""; keyMap[olc::SCROLL] = ""; keyMap[olc::NP0] = "0"; keyMap[olc::NP1] = "1"; keyMap[olc::NP2] = "2"; keyMap[olc::NP3] = "3"; keyMap[olc::NP4] = "4"; keyMap[olc::NP5] = "5"; keyMap[olc::NP6] = "6"; keyMap[olc::NP7] = "7"; keyMap[olc::NP8] = "8"; keyMap[olc::NP9] = "9"; keyMap[olc::NP_MUL] = ""; keyMap[olc::NP_DIV] = ""; keyMap[olc::NP_ADD] = ""; keyMap[olc::NP_SUB] = ""; keyMap[olc::NP_DECIMAL];
        }
    public:
        UI(std::shared_ptr<UIState> _uiState){
            initKeymap();
            State = _uiState;
            idCounter = 0;
        };
        // Add a frame to the UI that has no parrent.
        void AddRootFrame(std::shared_ptr<Frame> _frame){
            _frame->SetID(getNewID(), std::vector<int>());
            _frame->Parent = -1;
            frames[_frame->ID] = _frame;
            roots.insert(roots.begin(), _frame->ID);
            if(!_frame->Children.empty()){
                addChildren(_frame, std::vector<int>());
            }
        };
        // Draw the UI.
        void Draw(olc::PixelGameEngine& _pge){
            for(int i = roots.size() - 1; i >= 0; i--){
                auto r = roots[i];
                frames[r]->Draw(0, 0, _pge);
            }
            drawToolTip(_pge);
        };
        // Handles User Updates.
        void UserUpdate(olc::PixelGameEngine& _pge){
            removeChildren();
            addNewChildren();
            if(_pge.GetMouse(0).bPressed){
                auto rID = InUI(_pge.GetMouseX(), _pge.GetMouseY());
                if(rID >= 0)
                    PushForward(rID);
                if(clickRes(_pge.GetMouseX(), _pge.GetMouseY()) == "DRAG#-1"){
                    xPr = _pge.GetMouseX();
                    yPr = _pge.GetMouseY();
                }
                else{
                    xPr = -1;
                    yPr = -1;
                }
            }
            if(_pge.GetMouse(0).bHeld){
                if(xPr >= 0){
                    for(auto const& f:roots){
                        if(frames[f]->InFrame(xPr, yPr)){
                            frames[f]->X += _pge.GetMouseX() - xPr;
                            frames[f]->Y += _pge.GetMouseY() - yPr;
                            xPr = _pge.GetMouseX();
                            yPr = _pge.GetMouseY();
                        }
                    }
                }
            }
            if(_pge.GetMouse(0).bReleased){
                if(tf != nullptr){
                    tf->Active = false;
                    tf = nullptr;
                }
                xPr = -1;
                yPr = -1;
                handleClick(clickRes(_pge.GetMouseX(), _pge.GetMouseY()));
            }
            if(tf != nullptr){
                if(_pge.GetKey(olc::BACK).bPressed){
                    tf->RemoveLetter();
                }
                else{
                    tf->AddLetter(getKeyText(_pge));
                }
            }
        };
        // Push a UI Root Frame to the front.
        void PushForward(int _id){
            auto it = std::find(roots.begin(), roots.end(), _id);
            roots.erase(it);
            roots.insert(roots.begin(), _id);
        };
        // Check if point of screen is in a Root Frame and returns its ID otherwise it returns -1.
        int InUI(int _x, int _y){
            for(const auto& r : roots){
                if(frames[r]->InFrame(_x, _y))
                    return r;
            }
            return -1;
        };
        // Returns the width of element with ID _id.
        int WidthOfElement(int _id){
            auto it = frames.find(_id);
            if(it != frames.end()){
                return frames[_id]->Width;
            }
            return 0;
        };
        // Removes Root Frame from UI.
        void RemoveRoot(int _id){
            frames.erase(frames.find(_id));
            auto it = std::find(roots.begin(), roots.end(), _id);
            roots.erase(it);
        }
        // Returns frame with ID _id.
        std::shared_ptr<Frame> GetFrame(int _id){
            if(frames.find(_id) != frames.end()){
                return frames[_id];
            }
            return nullptr;
        }
        // Inserts external string in selected TextField.
        void InsertString(std::string _inp){
            if(tf != nullptr){
                tf->AddLetter(_inp);
            }
        }
    private:
        std::tuple<std::string, std::vector<std::string>> standartParse(std::string _input, std::string _sep = ":"){
            std::string key = _input;
            auto ids = std::vector<std::string>();
            auto pos = _input.find(_sep);
            if(pos != std::string::npos){
                key = _input.substr(0, pos);
                auto rem = _input.substr(pos + 1);
                auto pos_ = rem.find(",");
                while(pos_ != std::string::npos){
                    ids.push_back(rem.substr(0, pos_));
                    rem = rem.substr(pos_ + 1);
                    pos_ = rem.find(",");
                }
                ids.push_back(rem);
            }
            return std::tuple<std::string, std::vector<std::string>>(key, ids);
        }
    };
}

#endif /* UIFrames_hpp */
