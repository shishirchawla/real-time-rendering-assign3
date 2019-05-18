#include "arena.h"

Arena *arena_init(vec2f min, vec2f max, float cellSize)
{
  Arena *arena;
  int i;

  arena = safe_malloc(sizeof(Arena));
  arena->min.x = min.x; arena->min.y = min.y;
  arena->max.x = max.x; arena->max.y = max.y;

  arena->gridCellSize[0] = cellSize;
  arena->gridCellSize[1] = cellSize;
  /* using fixed grid cell size for better grid structure */
  arena->gridNumCells[0] = (int)((arena->max.x - arena->min.x) / arena->gridCellSize[0]);
  arena->gridNumCells[1] = (int)((arena->max.y - arena->min.y) / arena->gridCellSize[1]);
  arena->gridSize = arena->gridNumCells[0] * arena->gridNumCells[1];

  /* Allocate arrays. */
  arena->gridCellBrickCount = (int **)safe_malloc(arena->gridNumCells[0] * sizeof(int *));
  arena->gridCellBrickListEnd = (int **)safe_malloc(arena->gridNumCells[0] * sizeof(int *));
  for (i = 0; i < arena->gridNumCells[0]; i++) {
    arena->gridCellBrickCount[i] = (int *)safe_malloc(arena->gridNumCells[1] * sizeof(int));
    arena->gridCellBrickListEnd[i] = (int *)safe_malloc(arena->gridNumCells[1] * sizeof(int));
  }
  arena->gridCellBrickList = (int *)safe_malloc(arena->gridSize * sizeof(int));

  return arena;
}

/* returns grid index based on point position */
void arena_calc_gridindex(Arena *a, vec2f position, int *index)
{
  index[0] = (int)((position.x - a->min.x) / a->gridCellSize[0]);
  index[1] = (int)((position.y - a->min.y) / a->gridCellSize[1]);
}

void arena_draw_uniformgrid(Arena *arena)
{
  int i, j;
  float x0, y0;

  x0 = 0.0f;
  y0 = 0.0f;

  glColor3f(0.3f, 0.5f, 0.5f);
  glLineWidth(2.0f);
  /* draw horizontal lines */
  for (i = 0; i <= arena->gridNumCells[1]; i++) {
    glBegin(GL_LINE_STRIP);
    for (j = 0; j <= arena->gridNumCells[0]; j++)
      glVertex2f(x0+(arena->gridCellSize[0]*j), y0+(arena->gridCellSize[1]*i)); 
    glEnd();
  }
  /* draw vertical lines */
  for (i = 0; i <= arena->gridNumCells[0]; i++) {
    glBegin(GL_LINE_STRIP);
    for (j = 0; j <= arena->gridNumCells[1]; j++)
      glVertex2f(x0+(arena->gridCellSize[0]*i), y0+(arena->gridCellSize[1]*j)); 
    glEnd();
  }
  /* set line width back to 1 */ 
  glLineWidth(1.0f);
}

void arena_cleanup(Arena *arena)
{
  int i;

  if (arena) {
    for (i = 0; i < arena->gridNumCells[0]; i++) {
      free(arena->gridCellBrickCount[i]);
      free(arena->gridCellBrickListEnd[i]);
    }
    free(arena->gridCellBrickCount);
    free(arena->gridCellBrickListEnd);
    free(arena->gridCellBrickList);
    free(arena);
  }
}
