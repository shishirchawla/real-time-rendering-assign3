#include "sdl-base.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"

bool keys[NUM_KEYS]; /* key states - up/down */
bool debug[NUM_DEBUG_FLAGS] = {false, false, false, false, false}; /* debug states */
Arena *arena; /* define Arena */
CollisionDetectionMethod col_det_method = BRUTE_FORCE; /* default collision detection - brute force */

Game *game;

void update_renderstate()
{
  glPolygonMode(GL_FRONT_AND_BACK, debug[DEBUG_WIREFRAME] ? GL_LINE : GL_FILL);
}

void init()
{
  srand(time(0)); /* everytime program is run, set a new seed for rand() based on time */

  /* initialise arena - reason for arena not being part of game is that arena is used as a global constant
     , so different files can easily access things like bounds */
  vec2f min = {0.0f, 0.0f}; vec2f max = {4.0f, 3.0f};
  arena = arena_init(min, max, BRICK_SIDE);
  /* initialise game */
  game = game_init();

  update_renderstate();
}

void reshape(int w, int h)
{
  /* set orthographic projection with arena bounds */
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(arena->min.x, arena->max.x, arena->min.y, arena->max.y, -1, 1);
  glMatrixMode(GL_MODELVIEW);
}

void update(int milliseconds)
{
  float dt = (float)milliseconds * 0.001f;
  game_update(game, dt);
}

void display(SDL_Surface *screen)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  game_draw(game);

  if (debug[DEBUG_UNIFORM_GRID])
    arena_draw_uniformgrid(arena);
}

void keyDown(SDLKey key)
{
  switch (key)
  {
    case SDLK_ESCAPE: /* exit app on escape */
      quit();
      break;
    case SDLK_b: /* toggle debug option - Bounding Box */
      debug[DEBUG_BOUNDINGBOX] = !debug[DEBUG_BOUNDINGBOX];
      printf("DEBUG_BOUNDINGBOX(b) - %d\n", debug[DEBUG_BOUNDINGBOX]);
      break;
    case SDLK_c: /* Change collision detection method. */
      col_det_method = (col_det_method+1) % NUM_CDMETHODS;
      if (col_det_method == BRUTE_FORCE)
        printf("COLLISION DETECTION METHOD(c) - Brute Force\n");
      else if (col_det_method == UNIFORM_GRID)
        printf("COLLISION DETECTION METHOD(c) - Uniform Grid\n");
      break;
    case SDLK_d: /* Display collision time */
      debug[DEBUG_COLLISION_TIME] = !debug[DEBUG_COLLISION_TIME];
      printf("DEBUG_COLLISION_TIME(d) - %d\n", debug[DEBUG_COLLISION_TIME]);
      break;
    case SDLK_g: /* display uniform grid */
      debug[DEBUG_UNIFORM_GRID] = !debug[DEBUG_UNIFORM_GRID];
      printf("DEBUG_UNIFORM_GRID(g) - %d\n", debug[DEBUG_UNIFORM_GRID]);
      break;
    case SDLK_i: /* set infinite player lives */
      game->player->lives = INT_MAX;
      break;
    case SDLK_r: /* restart game */
      game_cleanup(game);
      game = game_init();
      break;  
    case SDLK_s: /* toggle step over option - use 'n' to step over */
      debug[DEBUG_STEPOVER] = !debug[DEBUG_STEPOVER];
      printf("DEBUG_STEPOVER(s) - %d\n", debug[DEBUG_STEPOVER]);
      break;
    case SDLK_n: /* step over - debug option DEBUG_STEPOVER must be enabled */
      if (debug[DEBUG_STEPOVER]) {
        float dt = 3.0; /* 3 milliseconds */
        update(dt);
      }
      break;
    case SDLK_f: /* fast step over - debug option DEBUG_STEPOVER must be enabled */
      if (debug[DEBUG_STEPOVER]) {
        float dt = 10.0; /* 3 milliseconds */
        update(dt);
      }  
      break;
    case SDLK_w: /* toggle debug option - WIREFRAME:FILL */
      debug[DEBUG_WIREFRAME] = !debug[DEBUG_WIREFRAME];
      update_renderstate();
      break;
    case SDLK_x: /* jump to next level */
      game->num_bricks_left = 0;
      break;
    case SDLK_EQUALS: /* double ball speed */
      game->ball.vel.x *= 2.0;
      game->ball.vel.y *= 2.0;
      break;
    case SDLK_MINUS: /* halve ball speed */
      game->ball.vel.x /= 2.0;
      game->ball.vel.y /= 2.0;
      break;  
    default:
      break;
  }

  /* set the current key state as down */
  if (key < NUM_KEYS)
    keys[key] = true;
}

void keyUp(SDLKey key)
{
  /* set the current key state as up */
  if (key < NUM_KEYS)
    keys[key] = false;
}

void event(SDL_Event *event)
{
  switch (event->type)
  {
    case SDL_KEYDOWN:
      keyDown(event->key.keysym.sym);
      break;
    case SDL_KEYUP:
      keyUp(event->key.keysym.sym);
      break;
    default:
      break;
  }
}

void cleanup()
{
  game_cleanup(game);
  arena_cleanup(arena);
}
