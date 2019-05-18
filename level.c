#include "level.h"

Level *level_init(int l_number, int l_num_bricks)
{
  Level *level;
  Brick *bricks;

  int i, r, row, col;
  float posX, posY;

  /* allocate memory/initialise level */
  level = (Level *)safe_malloc(sizeof(Level));
  level->level_number = l_number;
  level->num_bricks = l_num_bricks;
  level->bricks = NULL;

  /* allocate memory for bricks */
  bricks = (Brick *)safe_malloc(sizeof(Brick) * level->num_bricks);

  /* Level 1 */
  if (level->level_number == LEVEL1) {
    /* initialise brick/shape attributes */
    posX = 0;
    posY = arena->max.y - BRICK_SIDE;
    row = 0; /* row */
    col = 0; /* column */
    for (i = 0; i < level->num_bricks; i++) {
      /* initialise position */
      posX = (col++ * BRICK_SIDE);
      if (posX > arena->max.x-BRICK_SIDE+EPSILON)
      {
        posY = arena->max.y-BRICK_SIDE - (++row*BRICK_SIDE) ;
        posX = 0;
        col = 1; /* set to 1, because first column for next row is set in this iteration */
      }
      bricks[i].position.x = posX;
      bricks[i].position.y = posY;

      float x0 = BRICK_SPACING*0.5; float side_len_temp = (BRICK_SIDE-BRICK_SPACING)*0.5;
      bricks[i].position.x += x0 + side_len_temp+EPSILON;
      bricks[i].position.y += side_len_temp+EPSILON;

      r = rand();
      /* initialise random rotation (0-359 degrees) */
      bricks[i].rotation = (r%360);
      /* initialise random shape */
      if (r % TRIANGLE == 0)
        bricks[i].shape_t = TRIANGLE;
      else if (r % SQUARE == 0)
        bricks[i].shape_t = SQUARE;
      else if (r % PENTAGON == 0)
        bricks[i].shape_t = PENTAGON;
      else
        bricks[i].shape_t = CIRCLE;
      /* initialise alive=true */
      bricks[i].alive = true;
      /* set side len */
      bricks[i].side_len = (BRICK_SIDE-BRICK_SPACING)* 0.5f;
      /* set num vertices */
      bricks[i].num_vertices = bricks[i].shape_t;
      /* initialise vertices and bounding box */
      getVerticesAndBoundingBox(&bricks[i]);
    }
  } else if (level->level_number == LEVEL2) {
    /* two discs of bricks */
    float centerX, centerY;
    float discRadius, coreRadius;
    int numBricksInRing, count;
    float rotation;
    centerX = arena->max.x*0.5f; centerY = arena->max.y*0.5f;
    coreRadius = arena->max.y * 0.1f; /* one-tenth */
    discRadius = coreRadius + 2.0*(BRICK_SIDE+BRICK_SPACING);

    numBricksInRing = (int)(2*M_PI*discRadius/BRICK_SIDE)-1;
    count = 0;
    row = 0; /* row */
    col = 0; /* column */
    for (i = 0; i < level->num_bricks; i++) {
      ++count;
      if (count > numBricksInRing) {
        discRadius += (BRICK_SIDE+BRICK_SPACING);
        count = 0;
      }

      rotation = i*(BRICK_SIDE+BRICK_SPACING);
      /* initialise brick/shape attributes */
      posX = centerX + (discRadius * cos(rotation));
      posY = centerY + (discRadius * sin(rotation));
      /* initialise position */
      bricks[i].position.x = posX;
      bricks[i].position.y = posY;

      r = rand();
      /* initialise random rotation (0-359 degrees) */
      bricks[i].rotation = (r%360);
      /* initialise random shape */
      if (r % TRIANGLE == 0)
        bricks[i].shape_t = TRIANGLE;
      else if (r % SQUARE == 0)
        bricks[i].shape_t = SQUARE;
      else if (r % PENTAGON == 0)
        bricks[i].shape_t = PENTAGON;
      else
        bricks[i].shape_t = CIRCLE;
      /* initialise alive=true */
      bricks[i].alive = true;
      /* set side len */
      bricks[i].side_len = (BRICK_SIDE-BRICK_SPACING)* 0.5f;
      /* set num vertices */
      bricks[i].num_vertices = bricks[i].shape_t;
      /* initialise vertices and bounding box */
      getVerticesAndBoundingBox(&bricks[i]);
    }  
  }

  /* set level's brick pointer */
  level->bricks = bricks;
  return level;
}

void level_walls_collide(Level *level, Ball *ball, Player *player, float dt)
{
  float bpx, bpy, bvx, bvy, br;
  int levelNumber;

  bpx = ball->position.x; bpy = ball->position.y;
  bvx = ball->vel.x; bvy = ball->vel.y;
  br = ball->radius;
  levelNumber = level->level_number;

  if (levelNumber == LEVEL1) { /* LEVEL 1 bounds check */
    if (bpx < 0.0f + br) {bpx -= 2.0f * (bpx - br); bvx = fabs(bvx);}
    if (bpx > arena->max.x - br) {bpx -= 2.0f * (bpx - arena->max.x + br); bvx = -fabs(bvx);}
    if (bpy > arena->max.y - br) {bpy -= 2.0f * (bpy - arena->max.y + br); bvy = -fabs(bvy);}
    if (bpy < 0.0f - br)
    {
      /* lose */
      bpx = arena->max.x * 0.5f;
      bpy = 1.0f;
      bvy = fabs(bvy);

      --player->lives; /* reduce player life */
    }
  } else if (levelNumber == LEVEL2) { /* LEVEL 2 bounds check */
    /* boundary check */
    if (bpx < 0.0f - br || bpx > arena->max.x - br || bpy < 0.0f - br ||  bpy > arena->max.y - br)
    {
      /* lose */
      bpx = arena->max.x * 0.5f;
      bpy = 1.0f;
      bvx = 0.0f; bvy = 1.0f;

      --player->lives; /* reduce player life */
    }
    /* center core check */
    vec2f coreCenter = {arena->max.x*0.5, arena->max.y*0.5};
    float coreRadius = arena->max.y * 0.1f; /* one-tenth */
    float sumRadius = coreRadius + br;
    float collisionDepth = point_circle_collision((vec2f){bpx, bpy}, coreCenter, sumRadius);
    if (collisionDepth <= 0.0f) { /* collision detected */
      /* step backward */
      eulerStepSingleParticle(ball, -dt);
      bpx = ball->position.x;
      bpy = ball->position.y;
      /* rebound */
      vec2f normal; float normalLen;
      normal.x = coreCenter.x - bpx; normal.y = coreCenter.y - bpy;
      normalLen = sqrt((normal.x*normal.x) + (normal.y*normal.y));
      normal.x /= normalLen; normal.y /= normalLen;
      float VdotN = (bvx * normal.x) + (bvy * normal.y);
      bvx -= 2.0 * VdotN * normal.x;
      bvy -= 2.0 * VdotN * normal.y;
    }
  }

  /* update ball position and velocity */
  ball->position.x = bpx; ball->position.y = bpy;
  ball->vel.x = bvx; ball->vel.y = bvy;
}

void level_draw(Level *level, bool showBB)
{
  int i;
  Brick *bricks;
  float side_len;

  bricks = level->bricks;
  side_len = (BRICK_SIDE-BRICK_SPACING)*0.5;

  for (i = 0; i < level->num_bricks; i++) {
    if (bricks[i].alive == true) {
      glPushMatrix();
      /* Apply transformation */
      glTranslatef(bricks[i].position.x, bricks[i].position.y, 0.0f);
      /* Rotate about z-axis */
      glRotatef(bricks[i].rotation, 0.0, 0.0, 1.0);
      /* Draw shape */
      drawPolygon(side_len, bricks[i].shape_t, (vec3f){bricks[i].debug_color,0.0,0.3});
      glPopMatrix();
      if (showBB) /* show bounding box - for debugging */
        drawBoundingBox(bricks[i].bbox);
    }    
  }

  if (level->level_number == LEVEL2) { /* draw center core */
    float coreRadius = arena->max.y*0.1f; /* one-tenth */
    glPushMatrix();
    glTranslatef(arena->max.x*0.5f, arena->max.y*0.5f, 0.0f);
    drawPolygon(coreRadius, 32, (vec3f){0.5f, 0.2f, 0.2f});
    glPopMatrix();
  }
}

void level_cleanup(Level *level)
{
  int i;

  if (level) {
    if (level->bricks) {
      for (i = 0; i < level->num_bricks; i++)
        brick_cleanup(&level->bricks[i]);
      free(level->bricks);
    }
    free(level);
  }
}
