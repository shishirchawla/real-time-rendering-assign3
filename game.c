#include "game.h"

static int num_bricks_in_levels[] = {0, 80, 42};

Game *game_init(void)
{
  Game *game;

  /* initialise game */
  game = (Game *)safe_malloc(sizeof(Game));
  /* set num of levels */
  game->num_levels = GAME_LEVELS;
  /* initialise player */
  game->player = safe_malloc(sizeof(Player));
  game->player->lives = PLAYER_LIVES;
  game->player->score = 0;
  /* set game over status */
  game->gameover = false;

  /* initialise ball position */
  game->ball.position.x = arena->max.x * 0.5f;
  game->ball.position.y = 1.0f;
  game->ball.vel.x = 0.0f;
  game->ball.vel.y = 1.0f; /* ball moving straight up */
  game->ball.radius = BALL_RADIUS;

  /* initialise level - load level 1 */
  game->current_level = level_init(LEVEL1, num_bricks_in_levels[LEVEL1]); /* load level 1 */
  game->num_bricks_left = num_bricks_in_levels[LEVEL1];

  /* initialise paddle for level 1 */
  vec2f paddle_pos = {arena->max.x*0.5f, 0.5f};
  game->paddle = paddle_init(arena->max, paddle_pos, PADDLE_SPEED, -90, FLAT);

  return game;
}

void game_status_checks(Game *game)
{
  /* check for game over */
  if (game->gameover)
    return;

  /* check for player lives */
  if (game->player->lives <= 0) {
    game->gameover = true;
    return;
  }

  /* check if next level should be loaded */
  if (game->num_bricks_left <= 0) {
    /* pause for 1/4 second before next level */
    sleep_millisec(250);
    /* check if next level exists */
    if (game->current_level->level_number < game->num_levels) {
      /* load next level */
      int levelNumber = game->current_level->level_number+1;
      /* free current level */
      level_cleanup(game->current_level);
      game->current_level = level_init(levelNumber, num_bricks_in_levels[levelNumber]);
      game->num_bricks_left = num_bricks_in_levels[levelNumber];
      /* load paddle */
      if (game->current_level->level_number == LEVEL2) {
        /* free old paddle */
        paddle_cleanup(game->paddle);
        /* load circular paddle */
        vec2f paddle_pos = {arena->max.x*0.5f, arena->max.y*0.5f};
        game->paddle = paddle_init(arena->max, paddle_pos, PADDLE_SPEED, -90, CIRCULAR);
      } else {
        /* free old paddle */
        paddle_cleanup(game->paddle);
        /* load flat paddle */
        vec2f paddle_pos = {arena->max.x*0.5f, 0.5f};
        game->paddle = paddle_init(arena->max, paddle_pos, PADDLE_SPEED, -90, FLAT);
      }

      /* initialise ball position */
      game->ball.position.x = arena->max.x * 0.5f;
      game->ball.position.y = 1.0f;
      game->ball.vel.x = 0.0f;
      game->ball.vel.y = 1.0f; /* ball moving straight up */
      game->ball.radius = BALL_RADIUS;
    } else
      game->gameover = true;
  }
}

void display_collision_time(float dt, double collisionTime)
{
  static float time = 0.0f;
  time += dt;
  if (time > 1.0f) {
    printf("Collision Time - %.4f\n", collisionTime);
    time = 0;
  }  
}

void game_update(Game *game, float dt)
{
  bool brickCollision;

  game_status_checks(game);

  if (!game->gameover) {
    /* move the ball and update balls's current position */
    game->ball.position.x += game->ball.vel.x * dt;
    game->ball.position.y += game->ball.vel.y * dt;

    /* bounce ball off sides */
    level_walls_collide(game->current_level, &game->ball, game->player, dt);

    /* calculate collision detection time */
    struct timeval t1, t2;
    double collisionTime;
    gettimeofday(&t1, NULL); /* start time */

    /* detect collision with bricks */
    if (col_det_method == BRUTE_FORCE)
      brickCollision = bricks_collide_bruteforce(game->current_level->bricks, &game->ball, game->current_level->num_bricks, dt);
    else if (col_det_method == UNIFORM_GRID)
      brickCollision = bricks_collide_uniformgrid(game->current_level->bricks, &game->ball, game->current_level->num_bricks, dt);

    gettimeofday(&t2, NULL); /* end time */
    collisionTime = (t2.tv_sec - t1.tv_sec) * MILLI_SECOND_MULTIPLIER; /* convert to milliseconds */
    collisionTime += (t2.tv_usec - t1.tv_usec) / MILLI_SECOND_MULTIPLIER; /* convert to milliseconds */

    if (debug[DEBUG_COLLISION_TIME]) /* display time if debug flag set */
      display_collision_time(dt, collisionTime);

    if (brickCollision) /* if collision detected, reduce number of bricks */
      game->num_bricks_left--;  

    /* move paddle */
    paddle_move(game->paddle, arena->max, dt);

    /* collide ball with paddle */
    paddle_collide(game->paddle, &game->ball, dt);
  }
}

void game_draw(Game *game)
{
  float bpx, bpy, br;
  int lives, i;

  bpx = game->ball.position.x; bpy = game->ball.position.y;
  br = game->ball.radius;

  if (!game->gameover) {
    /* draw backdrop */
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.8f, 0.8f);
    glVertex2f(arena->min.x, arena->min.y);
    glVertex2f(arena->max.x, arena->min.y);
    glColor3f(0.2f, 0.2f, 0.2f);
    glVertex2f(arena->max.x, arena->max.y);
    glVertex2f(arena->min.x, arena->max.y);
    glEnd();

    /* draw the level */
    level_draw(game->current_level, debug[DEBUG_BOUNDINGBOX]); 

    /* draw the ball */
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(bpx, bpy, 0.0f);
    drawPolygon(br, 32, (vec3f){0.0f,0.2f,0.2f});
    glPopMatrix();

    /* draw the paddle */
    paddle_draw(game->paddle, debug[DEBUG_BOUNDINGBOX]);

    /* draw player lives */
    lives = game->player->lives;
    if (lives > 10)
      lives = 10;
    for (i = 0; i < lives; i++) {
      glPushMatrix();
      glTranslatef((arena->max.x*BALL_RADIUS)+(i*BALL_RADIUS*3.0f), BALL_RADIUS*2.0f, 0.0); 
      drawPolygon(BALL_RADIUS, HEXAGON, (vec3f){0.3, 0.1, 0.5});
      glPopMatrix();
    }
  } else { /* show dead screen */
    /* draw backdrop */
    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.0f, 0.0f);
    glVertex2f(arena->min.x, arena->min.y);
    glVertex2f(arena->max.x, arena->min.y);
    glColor3f(0.2f, 0.8f, 0.0f);
    glVertex2f(arena->max.x, arena->max.y);
    glVertex2f(arena->min.x, arena->max.y);
    glEnd();
  }
}

void game_cleanup(Game *game)
{
  if (game) {
    if (game->player)
      free(game->player);
    if (game->current_level)
      level_cleanup(game->current_level);
    if (game->paddle)
      paddle_cleanup(game->paddle);
    free(game);
  }
}
