#include "util.h"

void panic(const char *m)
{
  printf("%s", m);
  exit(1);
}

void sleep_millisec(float sleepTime)
{
  struct timespec req, rem;
  req.tv_sec = 0;
  req.tv_nsec = 0;

  /* if sleep time greater or equal to one second, split in seconds and milliseconds */
  if (sleepTime >= MILLI_SECOND_MULTIPLIER)
    req.tv_sec = (int)(sleepTime/MILLI_SECOND_MULTIPLIER);
  req.tv_nsec = (long)(((int)sleepTime - (int)(req.tv_sec*MILLI_SECOND_MULTIPLIER)) * NANO_SECOND_MULTIPLIER);

  /* force sleep */
  int res;
  while ((res = nanosleep(&req, &rem)) && errno == EINTR) {
    req.tv_sec = rem.tv_sec;
    req.tv_nsec = rem.tv_nsec;
  };

  if (res != 0) {
    perror("nanosleep");
    exit(1);
  }
}

void *safe_malloc(size_t size)
{
  void *mem_block = NULL;
  if ((mem_block = calloc(1, size)) == NULL) {
    fprintf(stderr, "ERROR - calloc()");
    exit(EXIT_FAILURE);
  }

  return mem_block;
}

void eulerStepSingleParticle(Ball *ball, float h)
{
  ball->position.x += h * ball->vel.x;
  ball->position.y += h * ball->vel.y;
}

/* Point-Circle Collision */
float point_circle_collision(vec2f point_pos, vec2f circle_pos, float circle_radius)
{
  vec2f dist_v;
  float dsqr, rsqr;

  rsqr = circle_radius*circle_radius;

  dist_v.x = circle_pos.x-point_pos.x; dist_v.y = circle_pos.y-point_pos.y;
  dsqr = (dist_v.x*dist_v.x)+(dist_v.y*dist_v.y);
  /*return dsqr < rsqr;*/
  return dsqr-rsqr;
}

/* AABB-Circle Collision */
bool aabb_circle_collision(BoundingBox2d bb, vec2f circle_pos, float circle_radius)
{
  float d = 0, rsqr = circle_radius*circle_radius;

  if (circle_pos.x < bb.minx) {d += (circle_pos.x - bb.minx)*(circle_pos.x - bb.minx);}
  else if (circle_pos.x > bb.maxx) {d += (circle_pos.x - bb.maxx)*(circle_pos.x - bb.maxx);}
  if (circle_pos.y < bb.miny) {d += (circle_pos.y - bb.miny)*(circle_pos.y - bb.miny);}
  else if (circle_pos.y > bb.maxy) {d += (circle_pos.y - bb.maxy)*(circle_pos.y - bb.maxy);}

  if (d < rsqr)
    return true;
  return false;
}

/* Returns closest point to the circle's center on the segment. */
vec3f closest_point_on_edge(vec2f seg_a, vec2f seg_b, vec2f circle_pos)
{
  vec2f seg_v, pt_v, proj_v;
  float seg_v_len, proj_v_len;
  vec3f closest;

  seg_v.x = seg_b.x - seg_a.x; seg_v.y = seg_b.y - seg_a.y;
  seg_v_len = sqrt((seg_v.x*seg_v.x) + (seg_v.y*seg_v.y));

  pt_v.x = circle_pos.x - seg_a.x; pt_v.y = circle_pos.y - seg_a.y;

  /* calculate projection along vector */
  proj_v_len = (pt_v.x * seg_v.x/seg_v_len) + (pt_v.y * seg_v.y/seg_v_len);
  if (proj_v_len <= 0.0f) {
    closest.x = seg_a.x; closest.y = seg_a.y;
    closest.z = COLLISION_ON_ENDPOINT_1;
    return closest;
  }
  if (proj_v_len >= seg_v_len) {
    closest.x = seg_b.x; closest.y = seg_b.y;
    closest.z = COLLISION_ON_ENDPOINT_2;
    return closest;
  }

  proj_v.x = proj_v_len * seg_v.x/seg_v_len; proj_v.y = proj_v_len * seg_v.y/seg_v_len;

  closest.x = proj_v.x + seg_a.x; closest.y = proj_v.y + seg_a.y;
  closest.z = COLLISION_ON_EDGE;
  return closest;
}
