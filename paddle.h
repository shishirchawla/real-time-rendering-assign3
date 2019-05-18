#ifndef PADDLE_H
#define PADDLE_H

#include "util.h"
#include "shape.h"
#include "brick.h"

/* Paddle types - flat and circular */
typedef enum {
  FLAT,
  CIRCULAR
} PaddleType;

typedef struct {
  vec2f position;
} FlatPaddle;

/* Circular paddle has two circular paddles - 
   second paddle mirror image of first one */
typedef struct {
  Brick first;
  Brick second;
} CircularPaddle;

/* Paddle struct
 *
 * @pad_type - paddle type
 * @vel - paddle velocity
 * @prop - circular or flat
 */
typedef struct {
  PaddleType pad_type;
  float vel;
  union {
    FlatPaddle fp;
    CircularPaddle cp;
  } prop;
} Paddle;

Paddle *paddle_init(vec2f levelBounds, vec2f position, float velocity, float rotation, PaddleType pt);
void paddle_move(Paddle *paddle, vec2f levelBounds, float dt);
void paddle_collide(Paddle *paddle, Ball *ball, float dt);
void paddle_draw(Paddle *paddle, bool showBB);
void paddle_cleanup(Paddle *paddle);

#endif
