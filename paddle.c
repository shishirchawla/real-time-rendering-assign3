#include "paddle.h"

Paddle *paddle_init(vec2f levelBounds, vec2f position, float velocity, float rotation, PaddleType pt)
{
  Paddle *paddle = safe_malloc(sizeof(Paddle));

  paddle->pad_type = pt;
  paddle->vel = velocity;
  if (pt == FLAT) {
    paddle->prop.fp.position = position;
  } else if (pt == CIRCULAR) {
    paddle->prop.cp.first.position = position;
    paddle->prop.cp.first.rotation = rotation;
    paddle->prop.cp.first.shape_t = 90;
    paddle->prop.cp.first.num_vertices = 10;
    paddle->prop.cp.first.side_len = levelBounds.y*0.5f;

    paddle->prop.cp.second.position = position;
    paddle->prop.cp.second.rotation = rotation+180;
    paddle->prop.cp.second.shape_t = 90;
    paddle->prop.cp.second.num_vertices = 10;
    paddle->prop.cp.second.side_len = levelBounds.y*0.5f;
  }

  return paddle;
}

void paddle_move(Paddle *paddle, vec2f levelBounds, float dt)
{
  float pv, velMultiplier;
  pv = paddle->vel;

  /* shift - double speed */
  if (keys[SDLK_LSHIFT])
    velMultiplier = 2.0f;
  else  
    velMultiplier = 1.0f;

  if (keys[SDLK_LEFT])
    pv = velMultiplier * -PADDLE_SPEED;
  else if (keys[SDLK_RIGHT])
    pv = velMultiplier * PADDLE_SPEED;
  else
    pv = 0.0f;

  if (paddle->pad_type == FLAT) {
    float ppx;
    ppx = paddle->prop.fp.position.x;

    ppx += pv * dt;
    if (ppx < 0.0f) ppx = 0.0f;
    if (ppx > levelBounds.x) ppx = levelBounds.x;

    paddle->prop.fp.position.x = ppx;
  } else if (paddle->pad_type == CIRCULAR) {
    float pr;
    pr = paddle->prop.cp.first.rotation;

    pr += pv * dt * PADDLE_SPEED_CIRCULAR;
    if (pr > 360.0)
      pr = (int)pr%360;

    paddle->prop.cp.first.rotation = pr;
    paddle->prop.cp.second.rotation = pr+180.0f;

    brick_cleanup(&paddle->prop.cp.first); 
    getVerticesAndBoundingBox(&paddle->prop.cp.first);
    brick_cleanup(&paddle->prop.cp.second); 
    getVerticesAndBoundingBox(&paddle->prop.cp.second);
  }  

  paddle->vel = pv;
}

void paddle_collide(Paddle *paddle, Ball *ball, float dt)
{
  if (paddle->pad_type == FLAT) {
    float br, bpx, bpy, bvx, bvy, ppx, ppy, pv;
    br = ball->radius;
    bpx = ball->position.x; bpy = ball->position.y;
    bvx = ball->vel.x; bvy = ball->vel.y;

    ppx = paddle->prop.fp.position.x; ppy = paddle->prop.fp.position.y;
    pv = paddle->vel;

    if (bpx > ppx - PADDLE_XSIZE*0.5f && bpx < ppx + PADDLE_XSIZE*0.5f &&
        bpy < ppy + br && bpy > ppy)
    {
      /* reflect ball's position */
      bpy += 2.0 * (ppy + br - bpy);
      /* reflect ball's velocity */
      bvy = fabs(bvy);
      /* add paddle's velocity. you may want to tweak this */
      if (pv != 0.0f) {
        bvx = (bvx + pv) * 0.5;
      }
    }

    ball->position.x = bpx; ball->position.y = bpy;
    ball->vel.x = bvx; ball->vel.y = bvy;
  } else if (paddle->pad_type == CIRCULAR){
    checkBrickCollision(&paddle->prop.cp.first, ball, dt);
    checkBrickCollision(&paddle->prop.cp.second, ball, dt);
  }
}

void paddle_draw(Paddle *paddle, bool showBB)
{
  if (paddle->pad_type == CIRCULAR) {
    int i;
    glLineWidth(4.0f);
    glColor3f(paddle->prop.cp.first.debug_color, 0.2f, paddle->prop.cp.second.debug_color*0.5f);
    glBegin(GL_LINE_LOOP);
    for(i = 0; i < paddle->prop.cp.first.num_vertices; i++)
      glVertex2f(paddle->prop.cp.first.vertices[i].x, paddle->prop.cp.first.vertices[i].y);
    glEnd();
    glBegin(GL_LINE_LOOP);
    for(i = 0; i < paddle->prop.cp.second.num_vertices; i++)
      glVertex2f(paddle->prop.cp.second.vertices[i].x, paddle->prop.cp.second.vertices[i].y);
    glEnd();
    glLineWidth(1.0f);

    /* show bounding box - for debugging */
    if (showBB) {
      drawBoundingBox(paddle->prop.cp.first.bbox);
      drawBoundingBox(paddle->prop.cp.second.bbox);
    }  
  } else if (paddle->pad_type == FLAT) {
    float ppx, ppy;
    ppx = paddle->prop.fp.position.x; ppy = paddle->prop.fp.position.y;

    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.2f, 0.2f);
    glVertex2f(ppx - PADDLE_XSIZE*0.5f, ppy);
    glVertex2f(ppx - PADDLE_XSIZE*0.5f, ppy-PADDLE_YSIZE*0.5f);
    glVertex2f(ppx + PADDLE_XSIZE*0.5f, ppy-PADDLE_YSIZE*0.5f);
    glVertex2f(ppx + PADDLE_XSIZE*0.5f, ppy);
    glEnd();
  }	
}

void paddle_cleanup(Paddle *paddle)
{
  if (paddle) {
    if (paddle->pad_type == CIRCULAR) {
      brick_cleanup(&paddle->prop.cp.first); 
      brick_cleanup(&paddle->prop.cp.second); 
    }
    free(paddle);
  }
}
