#ifndef GAME_H
#define GAME_H

#include "util.h"
#include "level.h"
#include "paddle.h"

/* Game constants */
#define GAME_LEVELS 2

/**
 * Game
 *
 * @paddle - paddle - (paddle_init) can be used to initialise 
 *          different style of paddles
 * @ball - ball
 * @num_levels - num of levels in the game
 * @current_level - current level pointer (level_init)
 * @num_bricks_left - keep track of bricks remaining in current level
 * @player - player (score and lives)
 * @gameover - game status
 */
typedef struct {
  Paddle *paddle;
  Ball ball;
  int num_levels;
  Level* current_level;
  int num_bricks_left;
  Player *player;
  bool gameover;
} Game;

Game *game_init(void);
void game_update(Game *game, float dt);
void game_draw(Game *game);
void game_cleanup(Game *game);

#endif
