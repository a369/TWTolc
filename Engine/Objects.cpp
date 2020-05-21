//
//  Objects.cpp
//  TWT2
//
//  Created by Adriaan Kisjes on 11/10/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#include "Objects.hpp"

struct bfsNode{
    Point P;
    float F;
    float C;
    int S;
    Point Pref;
    bfsNode(Point _p, Point _pref = Point(-1, -1)){
        P = _p;
        Pref = _pref;
        F = 0.0;
        C = 0.0;
        S = 0;
    }
    bfsNode(){
        F = 0.0;
        C = 0.0;
        S = 0;
    }
};

int pToI(Point _p, int _m = 1000){
    return _p.Y + _p.X * _m;
}

void insertBfsNode(std::vector<bfsNode>& _ns, bfsNode _bn){
    int i = 0;
    for(auto it = _ns.begin(); it != _ns.end(); it++){
        if(it->F > _bn.F){
            break;
        }
        i++;
    }
    _ns.insert(_ns.begin() + i, _bn);
}

bfsNode bfsNodesPop(std::vector<bfsNode>& _ns){
    auto res = _ns[0];
    _ns.erase(_ns.begin());
    return res;
}

//////////////////////////////////////////////////////////////////
// BOARD /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

Board::Board(SDL::Data _data){
    X = _data.Col["x"].GetInt();
    Y = _data.Col["y"].GetInt();
    auto tiles = _data.Col["tiles"].GetList();
    auto height = _data.Col["height"].GetList();
    for(auto& t : tiles){
        Tiles.push_back(t.GetInt());
    }
    for(auto& h : height){
        Hight.push_back(h.GetFloat());
    }
}

Scalar Board::Real(Point _p){
    float s = 1.0;
    float dx = 0.5;
    float dy = 0.833;
    float x = _p.X * (dx + s);
    auto y = _p.Y * (2 * dy);
    if(_p.X % 2 == 0)
        y += dy;
    return Scalar(x, y);
}

std::vector<Point> Board::GetNNeighbouring(Point _p, int _n, std::set<int> _allowed){
    std::vector<Point> res = {};
    std::set<int> visited = {};
    std::map<int, Point> toVisit = {};
    toVisit[_p.ToIndex()] = _p;
    Point cur;
    float md;
    float ndist;
    while(res.size() < _n && toVisit.size() > 0){
        md = X * Y * X;
        for(const auto& tv : toVisit){
            ndist = Real(_p).Distance(Real(tv.second));
            if(ndist < md){
                cur = tv.second;
                md = ndist;
            }
        }
        visited.insert(cur.ToIndex());
        res.push_back(cur);
        toVisit.erase(cur.ToIndex());
        for(auto& n : GetNeighbours(cur)){
            if(visited.find(n.ToIndex()) == visited.end()){
                if(_allowed.size() == 0 || _allowed.find(GetTile(n)) != _allowed.end())
                    toVisit[n.ToIndex()] = n;
                else
                    visited.insert(n.ToIndex());
            }
        }
    }
    res.erase(res.begin());
    return res;
}

std::vector<Point> Board::GetNeighbours(Point _p){
    std::vector<Point> res = {Point(_p.X + 1, _p.Y), Point(_p.X - 1, _p.Y), Point(_p.X, _p.Y + 1), Point(_p.X, _p.Y - 1)};
    if(_p.X % 2 == 0){
        res.push_back(Point(Point(_p.X + 1, _p.Y + 1)));
        res.push_back(Point(Point(_p.X - 1, _p.Y + 1)));
    }
    else{
        res.push_back(Point(Point(_p.X + 1, _p.Y - 1)));
        res.push_back(Point(Point(_p.X - 1, _p.Y - 1)));
    }
    return RemoveOutOfBound(res);
}

std::vector<Point> Board::RemoveOutOfBound(std::vector<Point>& _ps){
    auto res = std::vector<Point>();
    for(const auto& p : _ps){
        if(InBound(p))
            res.push_back(p);
    }
    return res;
}

bool Board::InBound(Point _p){
    if(_p.X >= X)
        return false;
    if(_p.Y >= Y)
        return false;
    if(_p.X < 0)
        return false;
    if(_p.Y < 0)
        return false;
    return true;
}

float Board::GetCost(Point _p0, Point _p1){
    auto res = 1.0;
    auto h0 = std::abs(GetHight(_p0)) + std::abs(GetHight(_p1));
    res *= std::pow(h0 + 1.0, 1.4);
    if(GetTile(_p0) != 1)
        res += 50;
    if(GetTile(_p1) != 1)
        res += 50;
    return res;
}

std::vector<Point> Board::GetRout(Point _p0, Point _p1, std::vector<Point> _np, int _max, float& _cost){
    auto v = std::map<int, bfsNode>();
    for(const auto& n : _np){
        auto index = pToI(n);
        if(index != pToI(_p0) && index != pToI(_p1)){
            v[index] = bfsNode();
        }
    }
    bfsNode cur = bfsNode(_p0);
    std::vector<bfsNode> q = {cur};
    v[pToI(cur.P)] = cur;
    auto found = false;
    while(q.size() > 0){
        cur = bfsNodesPop(q);
        if(cur.P.X == _p1.X && cur.P.Y == _p1.Y){
            found = true;
            break;
        }
        auto ns = GetNeighbours(cur.P);
        if(cur.S < _max){
            for(const auto& n : ns){
                if(v.find(pToI(n)) == v.end()){
                    auto nbn = bfsNode(n, cur.P);
                    nbn.C = cur.C + GetCost(n, cur.P);
                    nbn.F = nbn.C + _p1.Distance(n);
                    nbn.S = cur.S + 1;
                    insertBfsNode(q, nbn);
                    v[pToI(n)] = nbn;
                }
            }
        }
    }
    auto res = std::vector<Point>();
    if(found){
        _cost = cur.C;
        while(cur.Pref.X >= 0){
            res.push_back(cur.P);
            cur = v[pToI(cur.Pref)];
        }
        res.push_back(cur.P);
    }
    return res;
}

SDL::Data Board::ToSDL(){
    SDL::Data res;
    res.DataType = SDL::Dict;
    SDL::Data tiles;
    tiles.DataType = SDL::List;
    for(const auto& t : Tiles){
        tiles.Append(SDL::Data(t));
    }
    res.Col["tiles"] = tiles;
    SDL::Data height;
    tiles.DataType = SDL::List;
    for(const auto& h : Hight){
        tiles.Append(SDL::Data(h));
    }
    res.Col["height"] = height;
    res.Col["x"] = X;
    res.Col["y"] = Y;
    return res;
}

//////////////////////////////////////////////////////////////////
// ACTION SLOT ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

ActionSlot::ActionSlot(std::string _name)
{
    On = true;
    Name = _name;
    TimeOutEnd = 0;
}

DelayedAction::DelayedAction(std::string _name, int _host, int _time)
{
    Name = _name;
    Host = _host;
    Time = _time;
}

//////////////////////////////////////////////////////////////////
// VALUE COLLECTION //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

ValueCollection::ValueCollection(){
}

ValueCollection::ValueCollection(std::string _type, int _value){
    Values.insert(std::pair<std::string, int>(_type, _value));
}

ValueCollection::ValueCollection(SDL::Data _data){
    for(auto& v : _data.Col){
        Add(v.first, std::stoi(v.second.Val));
    }
}

ValueCollection* ValueCollection::C(std::string _type, int _value){
    Add(_type, _value);
    return this;
}

bool ValueCollection::Encapsulates(ValueCollection &_vc){
    for(auto j = _vc.Values.begin(); j != _vc.Values.end(); j++){
        auto i = Values.find(j->first);
        if(i == Values.end() || i->second < j->second){
            return false;
        }
    }
    return true;
}

void ValueCollection::Add(ValueCollection &_vc){
    for(auto j = _vc.Values.begin(); j != _vc.Values.end(); j++){
        auto i = Values.find(j->first);
        if(i == Values.end()){
            Values[j->first] = j->second;
        }
        else{
            Values[j->first] += j->second;
        }
    }
}

void ValueCollection::Add(std::string _type, int _value){
    if(Values.find(_type) == Values.end()){
        Values[_type] = _value;
    }
    else{
        Values[_type] += _value;
    }
}

void ValueCollection::Remove(ValueCollection &_r){
    for(const auto& j : _r.Values){
        auto i = Values.find(j.first);
        if(i == Values.end()){
            Values.insert(std::pair<std::string, int>(j.first, j.second * -1));
        }
        else{
            i->second -= j.second;
        }
    }
}

void ValueCollection::Normalize(){
    auto del = std::vector<std::string>();
    for(const auto& v : Values){
        if(v.second == 0){
            del.push_back(v.first);
        }
    }
    for(const auto& d : del){
        Values.erase(d);
    }
}

ValueCollection ValueCollection::Difference(ValueCollection &_vc){
    auto res = ValueCollection();
    auto types = std::vector<std::string>();
    for(const auto& r : Values){
        types.push_back(r.first);
    }
    for(const auto& r : _vc.Values){
        if(Values.find(r.first) == Values.end()){
            types.push_back(r.first);
        }
    }
    for(const auto& r : types){
        auto v = 0;
        auto i = Values.find(r);
        if(i != Values.end()){
            v += i->second;
        }
        i = _vc.Values.find(r);
        if(i != _vc.Values.end()){
            v -= i->second;
        }
        res.Add(r, v);
    }
    return res;
}

int ValueCollection::Get(std::string _type){
    auto it = Values.find(_type);
    if(it == Values.end())
    {
        return 0;
    }
    return it->second;
}

void ValueCollection::Set(std::string _type, int _value){
    Values[_type] = _value;
}

ValueCollection ValueCollection::Intersect(ValueCollection &_vc){
    auto res = ValueCollection();
    for(const auto& v : Values){
        auto i = _vc.Values.find(v.first);
        if(i != _vc.Values.end()){
            res.Add(v.first, std::min(i->second, v.second));
        }
    }
    return res;
}

int ValueCollection::Size(bool _absolute){
    auto res = 0;
    for(const auto& v : Values){
        if(_absolute)
            res += std::abs(v.second);
        else
            res += v.second;
    }
    return res;
}

SDL::Data ValueCollection::ToSDL(){
    SDL::Data res;
    res.DataType = SDL::Dict;
    for(const auto& v : Values){
        res.Col[v.first] = SDL::Data(v.second);
    }
    return res;
}

//////////////////////////////////////////////////////////////////
// OTHER /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

void DAQueue::InsertAction(std::shared_ptr<DelayedAction> _action)
{
    int i = 0;
    for(auto it = Actions.begin(); it != Actions.end(); it++)
    {
        auto e = *it;
        if(e->Time > _action->Time)
        {
            break;
        }
        i++;
    }
    Actions.insert(Actions.begin() + i, _action);
}

bool DAQueue::NextActionBefore(int _time){
    if(Actions.begin() != Actions.end())
    {
        return Actions[0]->Time < _time;
    }
    return false;
}

std::shared_ptr<DelayedAction> DAQueue::GetNextAction(){
    std::shared_ptr<DelayedAction> res = Actions[0];
    Actions.erase(Actions.begin());
    return res;
}

City::City(std::string _name, int _id){
    Name = _name;
    ID = _id;
}

City::City(SDL::Data _data){
    ID = _data.Col["id"].GetInt();
    Name = _data.Col["name"].Val;
    Owner = _data.Col["owner"].GetInt();
    //actionslots
    for(auto& n : _data.Col["neighbours"].Col){
        Neighbours[std::stoi(n.first)] = Road(n.second);
    }
    Resources = ValueCollection(_data.Col["resources"]);
    RunningConstraints = ValueCollection(_data.Col["constraints"]);
    Combat = ValueCollection(_data.Col["combat"]);
    Location = Point(_data.Col["location"]);
}

ValueCollection City::StaticConstraints(){
    auto res = ValueCollection();
    for(const auto& v : RunningConstraints.Values){
        if(v.first[0] == '_'){
            res.Add(v.first.substr(1, v.first.size()), v.second);
        }
        else
            res.Add(v.first, v.second);
    }
    return res;
}

SDL::Data City::ToSDL(){
    SDL::Data res;
    res.Col["id"] = SDL::Data(ID);
    res.Col["name"] = SDL::Data(Name);
    res.Col["owner"] = SDL::Data(Owner);
    //actionslots
    SDL::Data neighbours;
    for(auto& n : Neighbours){
        neighbours.Col[std::to_string(n.first)] = n.second.ToSDL();
    }
    res.Col["neighbours"] = neighbours;
    res.Col["resources"] = Resources.ToSDL();
    res.Col["constraints"] = RunningConstraints.ToSDL();
    res.Col["combat"] = Combat.ToSDL();
    res.Col["location"] = Location.ToSDL();
    return res;
}

//////////////////////////////////////////////////////////////////
// GAME STATE ////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

GameState::GameState(std::shared_ptr<GameSettings> _settings, std::shared_ptr<Board> _b){
    Settings = _settings;
    B = _b;
    IDCounter = 0;
}

void GameState::AddNewCity(std::string _name){
    auto id = IDCounter++;
    Cities.insert(std::pair<int, std::shared_ptr<City>>(id, std::make_shared<City>(_name, id)));
    citiesByName[_name] = id;
}

std::shared_ptr<City> GameState::GetCity(std::string _name){
    return Cities[citiesByName[_name]];
}

void GameState::MakeNeighbours(int _a, int _b, int _distance){
    Road r;
    r.CityA = _a;
    r.CityB = _b;
    r.Distance = _distance;
    Cities[_a]->Neighbours[_b] = r;
    Cities[_b]->Neighbours[_a] = r;
}

int GameState::GetResource(City &_c, std::string _r){
    auto owner = Settings->GetOwner(_r);
    switch (owner) {
        case rCity:
            return _c.Resources.Get(_r);
            break;
        case rConstraint:
            return _c.RunningConstraints.Get(_r);
            break;
        case rCombat:
            return _c.Combat.Get(_r);
            break;
        case rPlayer:
            return 0;
            break;
        case rGlobal:
            return 0;
            break;
        default:
            return 0;
            break;
    }
}

void GameState::SetResource(City &_c, std::string _r, int _value){
    auto owner = Settings->GetOwner(_r);
    switch (owner) {
        case rCity:
            _c.Resources.Set(_r, _value);
            break;
        case rConstraint:
            _c.RunningConstraints.Set(_r, _value);
            break;
        case rCombat:
            _c.Combat.Set(_r, _value);
            break;
        case rPlayer:
            break;
        case rGlobal:
            break;
        default:
            break;
    }
}


void GameState::InitCityLocations(){
    for(const auto& cp : Cities){
        auto p = cp.second->Location;
        auto index = p.Y + p.X * B->Y;
        citiesByLocation[index] = cp.first;
    }
}

int GameState::CityFromLocation(Point _p){
    auto index = _p.Y + _p.X * B->Y;
    if(citiesByLocation.find(index) == citiesByLocation.end())
        return -1;
    return citiesByLocation[index];
}






