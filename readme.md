## Why

This is a demo for an unfinished resource management and strategy game. I programmed this primarily to learn and practice c++, and I made it opensource to showcase my programming capabilities. I quit development for now because of time constraints.

## About the game 

In this game you control cities and trade routes between cities, it is currently possible to navigate the world and interact with the cities in it. You can build factories and transport goods from one city to another. Planned features are: City ownership; the player owns certain cities and other players (or AIs) control other clusters of cities. Combat; you can use resources to build units that can be used to take over other cities. 

## About the code 

I programmed this game with the olc pixel game engine ([youtube](https://www.youtube.com/watch?v=kRH6oJLFYxY), [github]( https://github.com/OneLoneCoder/olcPixelGameEngine)) because it is very minimal, easy to use, and cross platform.  One disadvantage is that it does not have a GUI so I had to programmed that myself. 

## About the GUI

Because my game idea needed a GUI I decided to build a little GUI library using the olc pixel game engine [UIFames.hpp](https://github.com/a369/TWTolc/blob/master/UIFrame/UIFrames.hpp). It consists of a Frame class where all UI elements inherit from, Frames can have other frames as children, they can handle user actions like text insertion, clicks, and dragging. There is also a UI class that manages the drawing of the frames and handles user events. [Here](https://github.com/a369/TWTolc/blob/master/Engine/GameUI.cpp) is an example of atk::UIFrames being used for the main game.
