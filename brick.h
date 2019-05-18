#ifndef BRICK_H
#define BRICK_H

#include "util.h"
#include "arena.h"

typedef struct {
  Shape shape_t;
  vec2f position;
  float side_len;
  float rotation;
  int num_vertices;
  vec2f *vertices;
  BoundingBox2d bbox;
  bool alive;
  float debug_color;
} Brick;

bool checkBrickCollision(Brick *brick, Ball *ball, float dt);
bool bricks_collide_bruteforce(Brick *bricks, Ball *ball, int numBricks, float dt);
bool bricks_collide_uniformgrid(Brick *bricks, Ball *ball, int numBricks, float dt);
void brick_cleanup(Brick *brick);

#endif
