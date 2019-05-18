#include "brick.h"

bool checkBrickCollision(Brick *brick, Ball *ball, float dt)
{
  vec2f ballPos;
  float ballRadius;
  int i, currentIndex, nextIndex; /* used in inside loop to keep track of current and next vertex */
  vec3f closestPoint; /* the closest point to the ball on the edge */
  int numVertices;
  vec2f *vertices;

  ballPos = ball->position; ballRadius = ball->radius;

  /* STEP 1 - Check AABB Collision */
  if (!aabb_circle_collision(brick->bbox, ballPos, ballRadius)) {
    brick->debug_color = 0.0f;
    return false;
  }
  /* STEP 2 (ONLY IF STEP 1 RETURNS TRUE) - Check collison with edges */
  numVertices = brick->num_vertices;
  vertices = brick->vertices;
  for(i = 0; i < numVertices; i++) {
    currentIndex = i;
    nextIndex = (i+1)%numVertices;

    /* get closest point on edge */
    closestPoint = closest_point_on_edge(vertices[currentIndex], vertices[nextIndex], ballPos);
    /* check collision with point */
    float dist = point_circle_collision((vec2f){closestPoint.x, closestPoint.y}, ballPos, ballRadius);
    brick->debug_color = (brick->side_len*0.5f)/fabs(dist);
    if (dist < 0.0f) {
      vec2f edge, normal; 
      float normalLen, VdotN;
      if (numVertices < CIRCLE) {
        if (closestPoint.z == COLLISION_ON_EDGE) { /* collision between end points of the edge */
          /* calculate edge vector and normal to compute ball reflection */
          edge.x = vertices[nextIndex].x-vertices[currentIndex].x; edge.y = vertices[nextIndex].y-vertices[currentIndex].y;
          normal.x = -edge.y; normal.y = edge.x;
        } else { /* collision on end point */
          /* collision on end point treated as though the point is on circle */
          normal.x = closestPoint.x-ball->position.x; normal.y = closestPoint.y-ball->position.y;
        }
      } else {
        normal.x = closestPoint.x-ball->position.x; normal.y = closestPoint.y-ball->position.y;
      }
      /* normalize normal vector */
      normalLen = sqrt(normal.x*normal.x + normal.y*normal.y);
      normal.x /= normalLen; normal.y /= normalLen;

      /* step backward */
      eulerStepSingleParticle(ball, -dt);

      /* update velocity */
      VdotN = (ball->vel.x * normal.x) + (ball->vel.y * normal.y);
      ball->vel.x -= (2.0f * VdotN * normal.x);
      ball->vel.y -= (2.0f * VdotN * normal.y);

      return true;
    }
  } 
  return false;
}

bool bricks_collide_bruteforce(Brick *bricks, Ball *ball, int numBricks, float dt)
{
  int i;
  bool collision;

  /* set collision status to false */
  collision = false;

  for (i = 0; i < numBricks; i++)
    if (bricks[i].alive == true)
      if (checkBrickCollision(&bricks[i], ball, dt)) {
        bricks[i].alive = false;
        collision = true;
        break; 
      }

  return collision;
}

bool bricks_collide_uniformgrid(Brick *bricks, Ball *ball, int numBricks, float dt)
{
  float gridCellSize[2];
  int **gridCellBrickCount, **gridCellBrickListEnd, *gridCellBrickList;
  int gridNumCells[2], gridSize, gridIndex[2], gridCellBrickListStart;
  int gridIndexMin[2], gridIndexMax[2];
  int i, j, s, t, bi, total;
  bool collision;

  /* set collision status to false */
  collision = false;

  gridCellSize[0] = arena->gridCellSize[0]; gridCellSize[1] = arena->gridCellSize[1];
  gridCellBrickCount = arena->gridCellBrickCount;
  gridCellBrickListEnd = arena->gridCellBrickListEnd;
  gridCellBrickList = arena->gridCellBrickList;
  gridNumCells[0] = arena->gridNumCells[0]; gridNumCells[1] = arena->gridNumCells[1];
  gridSize = arena->gridSize;

  /* Initialise grid brick count. */
  for (i = 0; i < gridNumCells[0]; i++)
    for (j = 0; j < gridNumCells[1]; j++)
      gridCellBrickCount[i][j] = 0;

  /* Cell counts. */
  for (i = 0; i < numBricks; i++) {
    if (bricks[i].alive == true) { /* check for brick status */
      arena_calc_gridindex(arena, bricks[i].position, (int *)&gridIndex);
      gridCellBrickCount[gridIndex[0]][gridIndex[1]] += 1;
    }
  }

  /* Work out end of cell lists by accumulating cell counts. */
  for (i = 0; i < gridNumCells[0]; i++)
    for (j = 0; j < gridNumCells[1]; j++)
      gridCellBrickListEnd[i][j] = 0;

  total = 0;
  for (i = 0; i < gridNumCells[0]; i++)
    for (j = 0; j < gridNumCells[1]; j++) {
      total = total + gridCellBrickCount[i][j];
      gridCellBrickListEnd[i][j] = total - 1;
    }

  /* Build brick lists. */
  for (i = 0; i < gridNumCells[0]; i++)
    for (j = 0; j < gridNumCells[1]; j++)
      gridCellBrickCount[i][j] = 0;

  for (i = 0; i < numBricks; i++) {
    if (bricks[i].alive == true) { /* check for brick status */
      arena_calc_gridindex(arena, bricks[i].position, (int *)&gridIndex);
      gridCellBrickList[gridCellBrickListEnd[gridIndex[0]][gridIndex[1]] - 
        gridCellBrickCount[gridIndex[0]][gridIndex[1]]] = i;
      gridCellBrickCount[gridIndex[0]][gridIndex[1]] += 1;
    }
  }

  /* calculate grid index for the ball */
  arena_calc_gridindex(arena, ball->position, (int *)&gridIndex);

  /* Grid index bounds for the ball. */
  gridIndexMin[0] = gridIndex[0] - 1;
  if (gridIndexMin[0] < 0) 
    gridIndexMin[0] = 0;
  gridIndexMin[1] = gridIndex[1] - 1;
  if (gridIndexMin[1] < 0) 
    gridIndexMin[1] = 0;
  gridIndexMax[0] = gridIndex[0] + 1;
  if (gridIndexMax[0] > gridNumCells[0] - 1) 
    gridIndexMax[0] = gridNumCells[0] - 1;
  gridIndexMax[1] = gridIndex[1] + 1;
  if (gridIndexMax[1] > gridNumCells[1] - 1) 
    gridIndexMax[1] = gridNumCells[1] - 1;

  /* check for collision in the 3 * 3 grid around the ball */
  for (s = gridIndexMin[0]; s <= gridIndexMax[0]; s++) {
    for (t = gridIndexMin[1]; t <= gridIndexMax[1]; t++) {
      gridCellBrickListStart = 
        gridCellBrickListEnd[s][t] - gridCellBrickCount[s][t] + 1;
      for (j = gridCellBrickListStart; 
          j <= gridCellBrickListEnd[s][t];
          j++) {
        bi = gridCellBrickList[j];
        if (checkBrickCollision(&bricks[bi], ball, dt)) {
          bricks[bi].alive = false;
          collision = true;
          return collision;
        }
      }
    }
  }

  return collision;
}

void brick_cleanup(Brick *brick)
{
  if (brick)
    free(brick->vertices);
}
