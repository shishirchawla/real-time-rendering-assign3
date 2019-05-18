#ifndef SHAPE_H
#define SHAPE_H

#include "util.h"
#include "brick.h"

void getVerticesAndBoundingBox(Brick *brick);
void drawPolygon(float side, int n_sides, vec3f color);
void drawBoundingBox(BoundingBox2d bb);

#endif
