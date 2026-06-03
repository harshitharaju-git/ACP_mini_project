#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#define ROWS 20
#define COLS 60
#define MAX_SHAPES 50
typedef enum {
    CIRCLE,
    RECTANGLE,
    LINE,
    TRIANGLE
} ShapeType;
typedef struct {
    int id;
    ShapeType type;
    int is_active;
    int x1, y1;
    int x2, y2;
    int x3, y3;
    int radius;
    int width, height;
} Shape;
Shape shapes[MAX_SHAPES];
int num_shapes = 0;
char canvas[ROWS][COLS];
WINDOW *canvas_win;
WINDOW *menu_win;
int absolute(int n) {
    return n < 0 ? -n : n;
}
void clear_canvas() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            canvas[r][c] = '_';
        }
    }
}
// Bresenham's Line Algorithm
void draw_line(int x1, int y1, int x2, int y2) {
    int dx = absolute(x2 - x1);
    int dy = absolute(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        if (x1 >= 0 && x1 < COLS && y1 >= 0 && y1 < ROWS) {
            canvas[y1][x1] = '*';
        }
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}
