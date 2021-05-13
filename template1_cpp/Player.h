#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include <fstream>

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords), mas_coords(pos), floor() {};

  bool Moved() const;
  char ProcessInput(MovementDir dir, int lim , int *pos_massive,  int &map_i ,  int &map_j, int &prev_move );
  void Draw(Image &screen, int* pos_massive = nullptr);
  Point& Coords(){return coords;};
  Point& Old_coords(){return old_coords;};
  Point& Mas_coords(){return mas_coords;};
  Image& Floor(){return floor;};
  Image& Treasure(){return treasure;};
  Image& Player_image(){return player_image;}; 
  Image& Treasure_taken(){return treasure_taken;};
  int HPs = 3; 
  Point spawn_coords{.x = 0, .y = 0};

private:
  Image floor;
  Image treasure;
  Image treasure_taken;
  Image player_image;
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Point mas_coords {.x = 0, .y = 0};
  Pixel color {.r = 0, .g = 0, .b = 0, .a = 255};
  int move_speed = 2;
  int treasures = 0;

};

#endif //MAIN_PLAYER_H
