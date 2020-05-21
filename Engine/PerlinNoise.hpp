//
//  PerlinNoise.hpp
//  TWTolc
//
//  Created by Adriaan Kisjes on 24/11/2019.
//  Copyright © 2019 Adriaan Kisjes. All rights reserved.
//

#ifndef PerlinNoise_hpp
#define PerlinNoise_hpp

#include <stdio.h>
#include <vector>
#include <tuple>
#include <string>
#include <map>


class PerlinNoise {
    // The permutation vector
    std::vector<int> p;
public:
    // Initialize with the reference values for the permutation vector
    PerlinNoise();
    // Generate a new permutation vector based on the value of seed
    PerlinNoise(unsigned int seed);
    // Get a noise value, for 2D images z can have any value
    double Value(double x, double y);
private:
    double fade(double t);
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y, double z);
};

struct PerlinMap {
    double XScale;
    double YScale;
    double ValueScale;
    double ValueAdd;
    PerlinMap(double _xs, double _ys, double _vs, double _va){
        XScale = _xs;
        YScale = _ys;
        ValueScale = _vs;
        ValueAdd = _va;
    }
};

class PerlinGen{
    std::vector<std::tuple<PerlinMap, PerlinNoise*>> pns;
public:
    PerlinGen(std::vector<PerlinMap> _pms){
        for(const auto& pm : _pms){
            auto v = std::tuple<PerlinMap, PerlinNoise*>(pm, new PerlinNoise(rand()));
            pns.push_back(v);
        }
    }
    double Value(double _x, double _y){
        double res = 0;
        for(const auto& t : pns){
            auto pm = std::get<0>(t);
            res += (std::get<1>(t)->Value( pm.XScale * _x, pm.YScale * _y) * 4 - 2 )* pm.ValueScale + pm.ValueAdd;
        }
        return res;
    }
};

#endif /* PerlinNoise_hpp */
