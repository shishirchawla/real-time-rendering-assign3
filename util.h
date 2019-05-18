#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <limits.h>
#include <errno.h>
#include <SDL/SDL.h>
#include <OpenGL/gl.h>

/* stuff math.h misses */
#ifndef fabs
#define fabs(x) ((x)<0?-(x):x)
#endif
#ifndef M_PI
#define M_PI 3.1416f
#endif

/* add the bool type if not found */
#ifndef bool
typedef char bool;
#define true 1
#define false 0
#endif

#define EPSILON 1.0e-6

#define MILLI_SECOND_MULTIPLIER 1000.0 /* 1s = 1,000 milliseconds */
#define NANO_SECOND_MULTIPLIER 1000000 /* 1ms = 1,000,000 nanoseconds */

#define RADIAN_MULTIPLIER M_PI/180.0f

#define COLLISION_ON_EDGE 0
#define COLLISION_ON_ENDPOINT_1 1
#define COLLISION_ON_ENDPOINT_2 2

#define BALL_RADIUS 0.04f

#define PADDLE_XSIZE 0.6f
#define PADDLE_YSIZE 0.2f
#define PADDLE_SPEED 2.0f
#define PADDLE_SPEED_CIRCULAR 30.0f

#define BRICK_SIDE 0.25
#define BRICK_SPACING 0.05

#define PLAYER_LIVES 3

#define LEVEL1 1
#define LEVEL2 2

typedef struct {
  float x;
  float y;
} vec2f;

typedef struct {
  float x;
  float y;
  float z;
} vec3f;

typedef struct {
  float minx;
  float miny;
  float maxx;
  float maxy;
} BoundingBox2d;

/* Shape enums */
typedef enum {
  TRIANGLE = 3,
  SQUARE,
  PENTAGON,
  HEXAGON,
  CIRCLE = 32
} Shape;

/* Debugging controls .*/
typedef enum {
  DEBUG_WIREFRAME,
  DEBUG_STEPOVER,
  DEBUG_BOUNDINGBOX,
  DEBUG_UNIFORM_GRID,
  DEBUG_COLLISION_TIME,
  NUM_DEBUG_FLAGS /* Muse be last */
} DebugFlags;

/* Collision detection method. */
typedef enum {
  BRUTE_FORCE,
  UNIFORM_GRID,
  NUM_CDMETHODS /* must be last */
} CollisionDetectionMethod;

typedef struct {
  float radius;
  vec2f position;
  vec2f vel;
} Ball;

typedef struct {
  int lives;
  int score;
} Player;

extern bool debug[NUM_DEBUG_FLAGS];
extern CollisionDetectionMethod col_det_method;

#define NUM_KEYS 1024 /* FIXME: replace with maximum SDLKey enumerator value */
extern bool keys[NUM_KEYS]; /* key states - up/down */

struct arena; /* fix for circular dependency */
extern struct arena *arena;

void panic(const char *m);
/* sleep for specified time (in milliseconds) */
void sleep_millisec(float sleepTime);
void *safe_malloc(size_t size);

/* helper functions for collisions */
void eulerStepSingleParticle(Ball *ball, float h);
float point_circle_collision(vec2f point_pos, vec2f circle_pos, float circle_radius);
bool aabb_circle_collision(BoundingBox2d bb, vec2f circle_pos, float circle_radius);
vec3f closest_point_on_edge(vec2f seg_a, vec2f seg_b, vec2f circle_pos);

#endif
