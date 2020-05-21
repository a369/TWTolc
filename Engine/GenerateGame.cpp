//
//  GenerateGame.cpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 24/11/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#include "GenerateGame.hpp"
#include "PerlinNoise.hpp"
#include "Objects.hpp"
#include "Actionslots.hpp"
#include <set>
#include <algorithm>
#include <ctime>


std::string MakeName(){
    std::vector<std::string> imp = {"arn", "asth", "din", "djal", "fith", "flar", "harl", "harn", "kan", "kiz", "nau", "naz", "ran", "saad", "tev", "thad", "tsanth", "tscho", "xun", "yahb", "ba", "baad", "bal", "banth", "dloth", "dole", "dom", "dor", "fren", "gaw", "ghaz", "gho", "hath", "haz", "henj", "hsar", "kho", "krell", "kull", "mar", "neb", "nen", "nind", "nuth", "sharn", "sheb", "shenx", "shup", "thang", "tild", "tine", "treen", "tsith", "tu", "tul", "ur", "yan", "yest", "yos", "za", "bhur", "bol", "chun", "drul", "duhl", "fen", "golm", "gren", "han", "ilb", "irb", "jad", "mox", "na", "nal", "ob", "orn", "pand", "silch", "snal", "taw", "trell", "trogh", "trilt", "weft", "weng", "xal", "zilx", "zon", "zor"};
    std::vector<std::string> sep = {"a", "i", "o", "u", "e", " "};
    std::string res = "";
    res += imp[rand() % imp.size()];
    res += sep[rand() % sep.size()] + imp[rand() % imp.size()];
    if(rand() % 10 == 0)
        res += sep[rand() % sep.size()] + imp[rand() % imp.size()];
    if(rand() % 20 == 0)
        res += sep[rand() % sep.size()] + imp[rand() % imp.size()];
    return res;
}

void MakeCity(std::shared_ptr<GameState> _gs, std::string _name, int _size = 1, Point _p = Point(0, 0))
{
    _gs->AddNewCity(_name);
    _gs->GetCity(_name)->Owner = 0;
    _gs->GetCity(_name)->RunningConstraints.Add("population", _size * 2);
    _gs->GetCity(_name)->RunningConstraints.Add("production", _size);
    _gs->GetCity(_name)->Resources.Add("food", _size * 6);
    auto ma = std::make_shared<MaintainanceSlot>("Maintain");
    _gs->GetCity(_name)->ActionSlots.push_back(ma);
    _gs->GetCity(_name)->Location = _p;
}

std::vector<Point> CityCoords(std::shared_ptr<Board> _b){
    auto tres0 = std::vector<Point>();
    for(auto i = 0; i < _b->X; i++){
        for(auto j = 0; j < _b->Y; j++){
            auto h = _b->GetHight(Point(i, j)) * 100;
            
            if(h > 0 && h < 50 && rand() % 50 > h && rand() % 3 == 0){
                tres0.push_back(Point(i, j));
            }
        }
    }
    auto res = std::vector<Point>();
    while(tres0.size() > 0){
        auto ns = tres0[rand() % tres0.size()];
        auto tres1 = std::vector<Point>();
        for(const auto& r : tres0){
            if(!(ns.Distance(r) < 5 + rand() % 12)){
                tres1.push_back(r);
            }
        }
        tres0 = tres1;
        res.push_back(ns);
    }
    return res;
}

int vertID(int _c0, int _c1){
    if(_c0 < _c1){
        return _c1 * 10000 + _c0;
    }
    return _c0 * 10000 + _c1;
}

bool compare(const std::tuple<float, std::tuple<int, int>>& _a, const std::tuple<float, std::tuple<int, int>>& _b){
    return std::get<0>(_a) < std::get<0>(_b);
}

std::map<int, int> dijkstra(std::shared_ptr<GameState> _gs, int _c){
    auto pref = std::map<int, int>();
    auto dist = std::map<int, int>();
    auto cities = std::set<int>();
    for(const auto& c : _gs->Cities){
        cities.insert(c.first);
        dist[c.first] = INT_MAX;
    }
    dist[_c] = 0;
    while(cities.size() != 0){
        auto cur = -1;
        auto dm = INT_MAX;
        for(const auto& c : cities){
            if(dist[c] <= dm){
                cur = c;
                dm = dist[c];
            }
        }
        cities.erase(cur);
        for(const auto& n : _gs->Cities[cur]->Neighbours){
            if(cities.find(n.first) != cities.end()){
                int nDist = dist[cur] + n.second.Distance;
                if(dist[n.first] > nDist){
                    dist[n.first] = nDist;
                    pref[n.first] = cur;
                }
            }
        }
    }
    return dist;
}

void makeConnectionMST(std::shared_ptr<GameState> _gs){
    auto maxSearchDist = 50;
    auto cityGroups = std::map<int, std::set<int>>();
    auto vertixes = std::vector<std::tuple<float, std::tuple<int, int>>>();
    auto visited = std::set<int>();
    for(const auto& c0 : _gs->Cities){
        cityGroups[c0.first] = {c0.first};
        for(const auto& c1 : _gs->Cities){
            auto vid = vertID(c0.first, c1.first);
            if(c0.first == c1.first)
                visited.insert(vid);
            if(visited.find(vid) == visited.end() && c0.second->Location.Distance(c1.second->Location) < 40){
                float c = 0.0;
                auto r = _gs->B->GetRout(c0.second->Location, c1.second->Location, {}, maxSearchDist, c);
                if(r.size() != 0){
                    vertixes.push_back(std::tuple<float, std::tuple<int, int>>(c, std::tuple<int, int>(c0.first, c1.first)));
                }
                visited.insert(vid);
            }
        }
    }
    std::sort(vertixes.begin(), vertixes.end(), compare);
    auto uVertixes = std::vector<std::tuple<int, int>>();
    auto nonPassable = std::vector<Point>();
    for(const auto& v : vertixes){
        auto c0 = std::get<0>(std::get<1>(v));
        auto c1 = std::get<1>(std::get<1>(v));
        if(cityGroups[c0].find(c1) == cityGroups[c0].end()){
            float dom = 0.0;
            auto r = _gs->B->GetRout(_gs->Cities[c0]->Location, _gs->Cities[c1]->Location, {}, maxSearchDist, dom);
            if(r.size() != 0){
                _gs->MakeNeighbours(c0, c1, r.size());
                _gs->Routes.push_back(r);
                nonPassable.insert(nonPassable.begin(), r.begin(), r.end());
                std::set<int> cUnion(cityGroups[c0]);
                cUnion.insert(cityGroups[c1].begin(), cityGroups[c1].end());
                for(const auto& ci : cUnion)
                    cityGroups[ci] = cUnion;
            }
        }
        else{
            uVertixes.push_back(std::tuple<int, int>(c0, c1));
        }
    }
    for(const auto& v : uVertixes){
        auto c0 = std::get<0>(v);
        auto c1 = std::get<1>(v);
        auto dc0 = dijkstra(_gs, c0);
        auto d = dc0[c1];
        float cost = 0.0;
        auto r = _gs->B->GetRout(_gs->Cities[c0]->Location, _gs->Cities[c1]->Location, {}, maxSearchDist, cost);
        if(r.size() * 1.8 < d && cost < 100){
            _gs->MakeNeighbours(c0, c1, r.size());
            _gs->Routes.push_back(r);
        }
        else if(r.size() * 7 < d){
            _gs->MakeNeighbours(c0, c1, r.size());
            _gs->Routes.push_back(r);
        }
    }
}

std::shared_ptr<GameState> MakeGameState(std::shared_ptr<GameSettings> _gs){
    srand(time(NULL));
    auto b = std::make_shared<Board>(77, 50);
    auto p = PerlinGen({PerlinMap(0.02, 0.03, 1, 0.2), PerlinMap(0.09, 0.09, 0.6, 0) /*, PerlinMap(0.15, 0.15, 0.2, 0)*/});
    for(int i = 0; i < b->X; i++){
        for(int j = 0; j < b->Y; j++){
            auto pt = Point(i, j);
            auto sc = b->Real(pt);
            float r = p.Value(sc.X, sc.Y);
            if(r > 0)
                r = r * r * r;
            else
                r = r * r * r;
            b->Hight.push_back(r);
            if(r > 0.9){
                b->Tiles.push_back(2);
            }
            else if (r > 0){
                b->Tiles.push_back(1);
            }
            else{
                b->Tiles.push_back(0);
            }
        }
    }
    
    auto gs = std::make_shared<GameState>(_gs, b);
    
    auto cityCoords = CityCoords(b);
    
    for(const auto& cc : cityCoords){
        MakeCity(gs, MakeName(), 1 , cc);
    }
    
    gs->InitCityLocations();
    gs->Time = 0;
    
    makeConnectionMST(gs);
    
    gs->Players[0] = Player();
    
    return gs;
}
