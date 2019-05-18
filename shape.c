#include "shape.h"

void drawPolygon(float side, int n_sides, vec3f color)
{
  int i;
  double factor;

  factor = 2.0 * M_PI/n_sides;

  glBegin(GL_TRIANGLE_FAN);
  glColor3f(color.x, color.y, color.z);
  glVertex2f(0.0f, 0.0f);
  for (i = 0; i <= n_sides; ++i)
    glVertex2f(side * cos(i * factor), side * sin(i * factor));
  glEnd();
}

/* Returns vertices(vertex positions) and bounding box for the brick */ 
void getVerticesAndBoundingBox(Brick *brick)
{
  /* brick variables */
  vec2f pos;
  float rotation;
  float side_len;
  Shape shape;
  int n_sides;

  int i;
  double factor;
  float x, y, xr, yr, xf, yf;

  pos = brick->position;
  rotation = brick->rotation * RADIAN_MULTIPLIER; /* convert to radians */
  side_len = brick->side_len;
  shape = brick->shape_t;
  n_sides = brick->num_vertices;

  vec2f *vertices = safe_malloc(sizeof(vec2f)*n_sides); /* allocate memory for vertices */
  BoundingBox2d bb = {.minx = INT_MAX, .miny = INT_MAX, .maxx = INT_MIN, .maxy = INT_MIN};

  factor = 2.0 * M_PI/shape;
  for (i = 0; i < n_sides; i++) {
    x = cos(i * factor);
    y = sin(i * factor);
    /* x and y after applying rotation */
    xr = x*cos(rotation) - y*sin(rotation);
    yr = x*sin(rotation) + y*cos(rotation);
    /* x and y after rotation, scaling and translation */
    xf = pos.x + (side_len * xr);
    yf = pos.y + (side_len * yr);
    /* set vertex position */
    vertices[i].x = xf; vertices[i].y = yf;

    /* compute bounding box */
    if (xf < bb.minx) bb.minx = xf;
    if (yf < bb.miny) bb.miny = yf;
    if (xf > bb.maxx) bb.maxx = xf;
    if (yf > bb.maxy) bb.maxy = yf;
  }

  brick->vertices = vertices;
  brick->bbox = bb;
}

void drawBoundingBox(BoundingBox2d bb)
{
  glBegin(GL_LINE_LOOP);
  glColor3f(0.0f, 0.2f, 0.0f);
  glVertex2f(bb.minx, bb.miny);
  glVertex2f(bb.maxx, bb.miny);
  glVertex2f(bb.maxx, bb.maxy);
  glVertex2f(bb.minx, bb.maxy);
  glEnd();
}
