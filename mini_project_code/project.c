#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Helper absolute value function
int absolute(int n) {
    return n < 0 ? -n : n;
}

// Fills the 2D array canvas with underscores
void clear_canvas() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            canvas[r][c] = '_';
        }
    }
}

// Function to display the picture
void display_picture() {
    printf("\n--- CURRENT PICTURE CANVAS ---\n");
    printf("   ");
    for (int c = 0; c < COLS; c++) printf("%d", c % 10);
    printf("\n  +");
    for (int c = 0; c < COLS; c++) printf("-");
    printf("+\n");

    for (int r = 0; r < ROWS; r++) {
        printf("%2d|", r);
        for (int c = 0; c < COLS; c++) {
            printf("%c", canvas[r][c]);
        }
        printf("|\n");
    }

    printf("  +");
    for (int c = 0; c < COLS; c++) printf("-");
    printf("+\n");
    printf("------------------------------\n");
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

// Redraws all shapes onto canvas and displays it
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
    // Added automatically so changes display immediately on screen
    display_picture(); 
}

// Helper to get checked integer input
int get_int(const char* prompt, int min_val, int max_val) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        buffer[strcspn(buffer, "\n")] = '\0';
        char *endptr;
        long parsed = strtol(buffer, &endptr, 10);
        if (endptr == buffer || *endptr != '\0' || parsed < min_val || parsed > max_val) {
            printf("Error: Please enter a number between %d and %d.\n", min_val, max_val);
            continue;
        }
        val = (int)parsed;
        return val;
    }
}

// Helper to list shapes
void print_shapes_list() {
    printf("\n--- Object Database ---\n");
    int count = 0;
    for (int i = 0; i < num_shapes; i++) {
        if (shapes[i].is_active) {
            count++;
            printf("[%d] ", shapes[i].id);
            if (shapes[i].type == CIRCLE) {
                printf("Circle: Center(%d,%d), Radius: %d\n", shapes[i].x1, shapes[i].y1, shapes[i].radius);
            } else if (shapes[i].type == RECTANGLE) {
                printf("Rectangle: Top-Left(%d,%d), Size: %dx%d\n", shapes[i].x1, shapes[i].y1, shapes[i].width, shapes[i].height);
            } else if (shapes[i].type == LINE) {
                printf("Line: (%d,%d) to (%d,%d)\n", shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2);
            } else if (shapes[i].type == TRIANGLE) {
                printf("Triangle: V1(%d,%d), V2(%d,%d), V3(%d,%d)\n", shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2, shapes[i].x3, shapes[i].y3);
            }
        }
    }
    if (count == 0) {
        printf("(No objects added yet)\n");
    }
    printf("-----------------------\n");
}

void add_object_menu() {
    if (num_shapes >= MAX_SHAPES) {
        printf("Error: Maximum shape limit reached!\n");
        return;
    }

    printf("\n--- Add Object Menu ---\n");
    printf("1. Circle\n");
    printf("2. Rectangle\n");
    printf("3. Line\n");
    printf("4. Triangle\n");
    printf("5. Back to Main Menu\n");
    int sub_choice = get_int("Choose shape to add: ", 1, 5);

    if (sub_choice == 5) return;

    Shape *s = &shapes[num_shapes];
    s->id = num_shapes + 1;
    s->is_active = 1;

    switch (sub_choice) {
        case 1:
            s->type = CIRCLE;
            s->x1 = get_int("Enter Center X (0-59): ", 0, COLS - 1);
            s->y1 = get_int("Enter Center Y (0-19): ", 0, ROWS - 1);
            s->radius = get_int("Enter Radius (0-30): ", 0, 30);
            printf("Added Circle with ID [%d]\n", s->id);
            break;
        case 2:
            s->type = RECTANGLE;
            s->x1 = get_int("Enter Top-Left X (0-59): ", 0, COLS - 1);
            s->y1 = get_int("Enter Top-Left Y (0-19): ", 0, ROWS - 1);
            s->width = get_int("Enter Width (1-60): ", 1, COLS);
            s->height = get_int("Enter Height (1-20): ", 1, ROWS);
            printf("Added Rectangle with ID [%d]\n", s->id);
            break;
        case 3:
            s->type = LINE;
            s->x1 = get_int("Enter X1 (0-59): ", 0, COLS - 1);
            s->y1 = get_int("Enter Y1 (0-19): ", 0, ROWS - 1);
            s->x2 = get_int("Enter X2 (0-59): ", 0, COLS - 1);
            s->y2 = get_int("Enter Y2 (0-19): ", 0, ROWS - 1);
            printf("Added Line with ID [%d]\n", s->id);
            break;
        case 4:
            s->type = TRIANGLE;
            s->x1 = get_int("Enter V1 X (0-59): ", 0, COLS - 1);
            s->y1 = get_int("Enter V1 Y (0-19): ", 0, ROWS - 1);
            s->x2 = get_int("Enter V2 X (0-59): ", 0, COLS - 1);
            s->y2 = get_int("Enter V2 Y (0-19): ", 0, ROWS - 1);
            s->x3 = get_int("Enter V3 X (0-59): ", 0, COLS - 1);
            s->y3 = get_int("Enter V3 Y (0-19): ", 0, ROWS - 1);
            printf("Added Triangle with ID [%d]\n", s->id);
            break;
    }
    num_shapes++;
    render_all();
}

void delete_object_menu() {
    print_shapes_list();
    int id = get_int("Enter Object ID to delete (or 0 to cancel): ", 0, num_shapes);
    if (id == 0) return;

    int found = 0;
    for (int i = 0; i < num_shapes; i++) {
        if (shapes[i].id == id && shapes[i].is_active) {
            shapes[i].is_active = 0;
            found = 1;
            printf("Deleted Object [%d] successfully.\n", id);
            break;
        }
    }
    if (!found) {
        printf("Error: Active object with ID [%d] not found.\n", id);
    }
    render_all();
}

void modify_object_menu() {
    print_shapes_list();
    int id = get_int("Enter Object ID to modify (or 0 to cancel): ", 0, num_shapes);
    if (id == 0) return;

    int idx = -1;
    for (int i = 0; i < num_shapes; i++) {
        if (shapes[i].id == id && shapes[i].is_active) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        printf("Error: Active object with ID [%d] not found.\n", id);
        return;
    }

    Shape *s = &shapes[idx];
    printf("Modifying Object [%d]:\n", id);
    if (s->type == CIRCLE) {
        s->x1 = get_int("Enter New Center X (0-59): ", 0, COLS - 1);
        s->y1 = get_int("Enter New Center Y (0-19): ", 0, ROWS - 1);
        s->radius = get_int("Enter New Radius (0-30): ", 0, 30);
    } else if (s->type == RECTANGLE) {
        s->x1 = get_int("Enter New Top-Left X (0-59): ", 0, COLS - 1);
        s->y1 = get_int("Enter New Top-Left Y (0-19): ", 0, ROWS - 1);
        s->width = get_int("Enter New Width (1-60): ", 1, COLS);
        s->height = get_int("Enter New Height (1-20): ", 1, ROWS);
    } else if (s->type == LINE) {
        s->x1 = get_int("Enter New X1 (0-59): ", 0, COLS - 1);
        s->y1 = get_int("Enter New Y1 (0-19): ", 0, ROWS - 1);
        s->x2 = get_int("Enter New X2 (0-59): ", 0, COLS - 1);
        s->y2 = get_int("Enter New Y2 (0-19): ", 0, ROWS - 1);
    } else if (s->type == TRIANGLE) {
        s->x1 = get_int("Enter New V1 X (0-59): ", 0, COLS - 1);
        s->y1 = get_int("Enter New V1 Y (0-19): ", 0, ROWS - 1);
        s->x2 = get_int("Enter New V2 X (0-59): ", 0, COLS - 1);
        s->y2 = get_int("Enter New V2 Y (0-19): ", 0, ROWS - 1);
        s->x3 = get_int("Enter New V3 X (0-59): ", 0, COLS - 1);
        s->y3 = get_int("Enter New V3 Y (0-19): ", 0, ROWS - 1);
    }
    printf("Object [%d] modified successfully.\n", id);
    render_all();
}

int main() {
    render_all();
    
    while (1) {
        printf("\n=============================\n");
        printf("    2D GRAPHICS EDITOR MENU  \n");
        printf("=============================\n");
        printf("1. Add Object\n");
        printf("2. Delete Object\n");
        printf("3. Modify Object\n");
        printf("4. Display Picture\n");
        printf("5. Exit\n");
        printf("=============================\n");

        int choice = get_int("Enter selection (1-5): ", 1, 5);

        if (choice == 5) {
            printf("\nExiting editor. Goodbye!\n");
            break;
        }

        switch (choice) {
            case 1:
                add_object_menu();
                break;
            case 2:
                delete_object_menu();
                break;
            case 3:
                modify_object_menu();
                break;
            case 4:
                display_picture();
                break;
        }
    }
    return 0;
}