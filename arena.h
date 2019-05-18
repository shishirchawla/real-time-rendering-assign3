#ifndef ARENA_H
#define ARENA_H

#include "util.h"

typedef struct arena {
  /* arena bounds */
  vec2f min, max;
  /* uniform grid variables */
  float gridCellSize[2];
  int **gridCellBrickCount, **gridCellBrickListEnd, *gridCellBrickList;
  int gridNumCells[2], gridSize;
} Arena;

/* initializes arena and uniform grid variables. */
Arena *arena_init(vec2f min, vec2f max, float cellSize);
/* helper function to calculate index on grid from vertex position */
void arena_calc_gridindex(Arena *a, vec2f position, int *index);
void arena_draw_uniformgrid(Arena *arena);
void arena_cleanup(Arena *arena);

#endif
