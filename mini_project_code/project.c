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
// Draws rectangle border
void draw_rectangle(int x, int y, int w, int h) {
    for (int i = 0; i < w; i++) {
        int px = x + i;
        if (px >= 0 && px < COLS) {
            if (y >= 0 && y < ROWS) canvas[y][px] = '*';
            if (y + h - 1 >= 0 && y + h - 1 < ROWS) canvas[y + h - 1][px] = '*';
        }
    }
    for (int i = 0; i < h; i++) {
        int py = y + i;
        if (py >= 0 && py < ROWS) {
            if (x >= 0 && x < COLS) canvas[py][x] = '*';
            if (x + w - 1 >= 0 && x + w - 1 < COLS) canvas[py][x + w - 1] = '*';
        }
    }
}
// Midpoint Circle Algorithm helpers
void plot_circle_points(int cx, int cy, int x, int y) {
    int px[8] = { cx + x, cx - x, cx + x, cx - x, cx + y, cx - y, cx + y, cx - y };
    int py[8] = { cy + y, cy + y, cy - y, cy - y, cy + x, cy + x, cy - x, cy - x };
    for (int i = 0; i < 8; i++) {
        if (px[i] >= 0 && px[i] < COLS && py[i] >= 0 && py[i] < ROWS) {
            canvas[py[i]][px[i]] = '*';
        }
    }
}
void draw_circle(int cx, int cy, int r) {
    if (r < 0) return;
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    plot_circle_points(cx, cy, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot_circle_points(cx, cy, x, y);
    }
}
// Draws triangle by connecting 3 points
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}
// Redraws all shapes onto canvas
void render_all() {
    clear_canvas();
    for (int i = 0; i < num_shapes; i++) {
        if (shapes[i].is_active) {
            switch (shapes[i].type) {
                case CIRCLE:
                    draw_circle(shapes[i].x1, shapes[i].y1, shapes[i].radius);
                    break;
                case RECTANGLE:
                    draw_rectangle(shapes[i].x1, shapes[i].y1, shapes[i].width, shapes[i].height);
                    break;
                case LINE:
                    draw_line(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2);
                    break;
                case TRIANGLE:
                    draw_triangle(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2, shapes[i].x3, shapes[i].y3);
                    break;
            }
        }
    }
}
// Displays canvas to ncurses window
void update_canvas_window() {
    werase(canvas_win);
    box(canvas_win, 0, 0);
    
    // Draw title on the border
    wattron(canvas_win, COLOR_PAIR(2) | A_BOLD);
    mvwprintw(canvas_win, 0, 2, " Drawing Canvas (2D grid: %dx%d) ", COLS, ROWS);
    wattroff(canvas_win, COLOR_PAIR(2) | A_BOLD);
    // Render characters
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            char ch = canvas[r][c];
            if (ch == '*') {
                wattron(canvas_win, COLOR_PAIR(3) | A_BOLD);
                mvwaddch(canvas_win, r + 1, c + 1, ch);
                wattroff(canvas_win, COLOR_PAIR(3) | A_BOLD);
            } else {
                wattron(canvas_win, COLOR_PAIR(1));
                mvwaddch(canvas_win, r + 1, c + 1, ch);
                wattroff(canvas_win, COLOR_PAIR(1));
            }
        }
    }
    wrefresh(canvas_win);
}