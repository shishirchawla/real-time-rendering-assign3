#ifndef LEVEL_H
#define LEVEL_H

#include "util.h"
#include "brick.h"
#include "shape.h"

typedef struct {
  int level_number; /* level number */
  int num_bricks; /* num of bricks in level */
  Brick *bricks; /* bricks array */
} Level;

Level *level_init(int l_number, int l_num_bricks);
void level_walls_collide(Level *level, Ball *ball, Player *player, float dt);
void level_draw(Level *level, bool showBB);
void level_cleanup(Level *level);

#endif
