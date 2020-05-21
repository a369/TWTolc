//
//  Objects.hpp
//  TWT2
//
//  Created by Adriaan Kisjes on 11/10/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

/*
 Objects and virtual classes that are used in the game engine.
 */

#ifndef Objects_hpp
#define Objects_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <cmath>
#include <set>
#include "PerlinNoise.hpp"
#include "Serializer.hpp"

struct City;
struct DAQueue;
struct GameState;

struct Point{
    int X;
    int Y;
    Point(int _x, int _y){ X = _x; Y = _y; }
    Point(){ X = 0; Y = 0; }
    Point(SDL::Data _data){X = _data.Col["x"].GetInt(); Y = _data.Col["y"].GetInt();}
    float Distance(Point _p){ return std::sqrt(std::pow(_p.X - X, 2) + std::pow(_p.Y - Y, 2)); }
    int ToIndex(int _m = 1000){ return X * 1000 + Y; }
    SDL::Data ToSDL(){SDL::Data res; res.Col["x"] = SDL::Data(X); res.Col["y"] = SDL::Data(Y); return res;}
};

struct Scalar{
    float X;
    float Y;
    Scalar(float _x, float _y){ X = _x; Y = _y; }
    Scalar(){ X = 0.0; Y = 0.0; }
    float Distance(Scalar _s){ return std::sqrt(std::pow(_s.X - X, 2) + std::pow(_s.Y - Y, 2)); }
};

struct Board{
    std::vector<int> Tiles;
    std::vector<float> Hight;
    int X;
    int Y;
    static float Vies;
    Board(int _x, int _y){ X = _x; Y = _y; }
    Board(SDL::Data _data);
    void SetTile(Point _p, int _t){ Tiles[_p.Y + _p.X * Y] = _t; }
    int GetTile(Point _p){ return Tiles[_p.Y + _p.X * Y]; }
    void SetHight(Point _p, float _t){ Hight[_p.Y + _p.X * Y] = _t; }
    float GetHight(Point _p){ return Hight[_p.Y + _p.X * Y]; }
    Scalar Real(Point _p);
    std::vector<Point> GetNNeighbouring(Point _p, int _n, std::set<int> _allowed = {});
    std::vector<Point> GetNeighbours(Point _p);
    std::vector<Point> RemoveOutOfBound(std::vector<Point>& _ps);
    bool InBound(Point _p);
    float GetCost(Point _p0, Point _p1);
    std::vector<Point> GetRout(Point _p0, Point _p1, std::vector<Point> _np = {}, int _max = 1000, float& _cost = Vies);
    SDL::Data ToSDL();
};

struct ValueCollection{
public:
    std::map<std::string, int> Values;
    ValueCollection();
    ValueCollection(std::string _type, int _value);
    ValueCollection(SDL::Data _data);
    ValueCollection* C(std::string _type, int _value);
    bool Encapsulates(ValueCollection &_vc);
    void Remove(ValueCollection  &_vc);
    void Add(ValueCollection &_vc);
    void Add(std::string _type, int _value);
    int Get(std::string _type);
    void Set(std::string _type, int _value);
    void Normalize();
    ValueCollection Intersect(ValueCollection &_vc);
    ValueCollection Difference(ValueCollection &_vc);
    int Size(bool _absolute = false);
    SDL::Data ToSDL();
};

struct ActionSlot{
    std::string Name;
    int TimeOutEnd;
    bool On;
    ActionSlot(std::string _name);
    virtual bool Actionable(City& _host, std::shared_ptr<GameState> _gs ){return true;};
    virtual void Perform(City& _host, std::shared_ptr<GameState> _gs) = 0;
    bool Constructable(City& _host, std::shared_ptr<GameState> _gs){return true;};
    void Construct(City& _host, std::shared_ptr<GameState> _gs){};
    bool Distructable(City& _host, std::shared_ptr<GameState> _gs){return true;};
    void Distruct(City& _host, std::shared_ptr<GameState> _gs){};
};

struct DelayedAction{
    std::string Name;
    int Host;
    int Time;
    DelayedAction(std::string _name, int _host, int _time);
    virtual void Perform(City& _host, std::shared_ptr<GameState> _gs) = 0;
};

struct Road{
    int CityA;
    int CityB;
    int Distance;
    Road(){}
    Road(SDL::Data _data){
        CityA = _data.Col["a"].GetInt();
        CityB = _data.Col["b"].GetInt();
        Distance = _data.Col["dist"].GetInt();
    }
    SDL::Data ToSDL(){
        SDL::Data res;
        res.Col["a"] = SDL::Data(CityA);
        res.Col["b"] = SDL::Data(CityB);
        res.Col["dist"] = SDL::Data(Distance);
        return res;
    }
};

struct DAQueue{
    std::vector<std::shared_ptr<DelayedAction>> Actions;
    void InsertAction(std::shared_ptr<DelayedAction> _action);
    bool NextActionBefore(int _time);
    std::shared_ptr<DelayedAction> GetNextAction();
};

struct City{
    int ID;
    std::string Name;
    int Owner;
    std::vector<std::shared_ptr<ActionSlot>> ActionSlots;
    std::map<int, Road> Neighbours;
    ValueCollection Resources;
    ValueCollection RunningConstraints;
    ValueCollection Combat;
    Point Location;
    City(std::string _name, int _id);
    City(SDL::Data _data);
    ValueCollection StaticConstraints();
    SDL::Data ToSDL();
};

enum TurnTypes{
    ttAddActionSlot,
};

struct Turn{
    TurnTypes TurnType;
    std::vector<std::string> Input;
};

struct Player{
    std::vector<Turn> Turns;
    ValueCollection PlayerResources;
};

enum ResourceOwnerType{
    rCity,
    rConstraint,
    rCombat,
    rPlayer,
    rGlobal
};

enum InputTypes{
    itInt,
    itString,
    itTag,
};

struct InputDescriptor{
    std::string Description;
    InputTypes InputType;
    std::vector<std::string> Var;
};

struct ResourceMeta{
    ResourceOwnerType Owner;
    bool CanBeNegative;
    ValueCollection MaintainCost;
};

struct ActionSlotMeta{
    std::vector<InputDescriptor> InputType;
    int VisualType;
};

class GameSettings{
public:
    std::map<std::string, ResourceMeta> Resources;
    std::map<std::string, ActionSlotMeta> ActionSlots;
    GameSettings(){};
    virtual std::shared_ptr<ActionSlot> GetActionSlot(std::string _actionSlotType, std::vector<std::string> _input) = 0;
    virtual bool ValidateInput(InputDescriptor _ids, std::string _input) = 0;
    virtual std::string InputTypeDescription(InputDescriptor _id) = 0;
    virtual void PerformTurn(int _player, Turn& _t, std::shared_ptr<GameState> _gs) = 0;
    virtual ResourceOwnerType GetOwner(std::string _rt) = 0;
};

struct GameState{
    DAQueue DelayedActions;
    std::map<int, int> citiesByLocation;
    std::map<std::string, int> citiesByName;
    int Time;
    int IDCounter;
public:
    std::map<int, Player> Players;
    std::map<int, std::shared_ptr<City>> Cities;
    std::vector<std::vector<Point>> Routes;
    std::shared_ptr<GameSettings> Settings;
    std::shared_ptr<Board> B;
    GameState(std::shared_ptr<GameSettings> _settings, std::shared_ptr<Board> _b);
    void AddNewCity(std::string);
    std::shared_ptr<City> GetCity(std::string _name);
    void MakeNeighbours(int _a, int _b, int _distance);
    bool AreNeighbours(std::string _a, std::string _b);
    int GetResource(City &_c, std::string _r);
    void SetResource(City &_c, std::string _r, int _value);
    void InitCityLocations();
    int  CityFromLocation(Point _p);
};


#endif /* Objects_hpp */
