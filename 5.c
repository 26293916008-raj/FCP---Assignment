// Structures - Interactive Rectangle Manager
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECTANGLES 1000
#define INPUT_SIZE 256
#define SQUARE_RELATIVE_TOLERANCE 1e-9

typedef struct {
    int id;
    double width;
    double height;
    double area;
    double perimeter;
} Rectangle;

typedef enum {
    LINE_READ,
    LINE_EOF,
    LINE_TOO_LONG
} LineStatus;

static LineStatus read_line(const char *prompt, char *buffer, size_t size) {
    fputs(prompt, stdout);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return LINE_EOF;
    }

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
        return LINE_READ;
    }
    if (feof(stdin)) {
        return LINE_READ;
    }

    int character;
    while ((character = getchar()) != '\n' && character != EOF) {
    }
    return LINE_TOO_LONG;
}

static bool only_trailing_whitespace(const char *text) {
    while (*text != '\0') {
        if (!isspace((unsigned char)*text)) {
            return false;
        }
        text++;
    }
    return true;
}

static bool read_integer(const char *prompt, int minimum, int maximum,
                         int *value) {
    char input[INPUT_SIZE];

    for (;;) {
        LineStatus status = read_line(prompt, input, sizeof(input));
        if (status == LINE_EOF) {
            return false;
        }
        if (status == LINE_TOO_LONG) {
            puts("Input is too long. Please try again.");
            continue;
        }

        errno = 0;
        char *end = NULL;
        long parsed = strtol(input, &end, 10);
        if (end != input && errno != ERANGE &&
            only_trailing_whitespace(end) && parsed >= minimum &&
            parsed <= maximum) {
            *value = (int)parsed;
            return true;
        }
        printf("Enter a whole number from %d to %d.\n", minimum, maximum);
    }
}

static bool read_finite_number(const char *prompt, double *value) {
    char input[INPUT_SIZE];

    for (;;) {
        LineStatus status = read_line(prompt, input, sizeof(input));
        if (status == LINE_EOF) {
            return false;
        }
        if (status == LINE_TOO_LONG) {
            puts("Input is too long. Please try again.");
            continue;
        }

        errno = 0;
        char *end = NULL;
        double parsed = strtod(input, &end);
        if (end != input && errno != ERANGE &&
            only_trailing_whitespace(end) && isfinite(parsed)) {
            *value = parsed;
            return true;
        }
        puts("Enter a finite number.");
    }
}

static bool calculate_geometry(double width, double height, double *area,
                               double *perimeter) {
    if (!isfinite(width) || !isfinite(height) ||
        width <= 0.0 || height <= 0.0) {
        return false;
    }

    *area = width * height;
    *perimeter = 2.0 * (width + height);
    return isfinite(*area) && *area > 0.0 && isfinite(*perimeter);
}

static bool read_dimensions(double *width, double *height, double *area,
                            double *perimeter) {
    for (;;) {
        if (!read_finite_number("Width: ", width) ||
            !read_finite_number("Height: ", height)) {
            return false;
        }
        if (calculate_geometry(*width, *height, area, perimeter)) {
            return true;
        }
        puts("Dimensions must be positive, and area and perimeter must be finite.");
    }
}

static bool contains_id(const Rectangle *rectangles, size_t count, int id) {
    for (size_t i = 0; i < count; i++) {
        if (rectangles[i].id == id) {
            return true;
        }
    }
    return false;
}

static bool reserve_rectangles(Rectangle **rectangles, size_t *capacity,
                               size_t required_capacity) {
    if (required_capacity <= *capacity) {
        return true;
    }
    if (required_capacity > MAX_RECTANGLES ||
        required_capacity > SIZE_MAX / sizeof(**rectangles)) {
        return false;
    }

    size_t new_capacity = *capacity == 0 ? 1 : *capacity;
    while (new_capacity < required_capacity) {
        if (new_capacity > MAX_RECTANGLES / 2) {
            new_capacity = MAX_RECTANGLES;
        } else {
            new_capacity *= 2;
        }
    }
    if (new_capacity > SIZE_MAX / sizeof(**rectangles)) {
        return false;
    }

    Rectangle *resized =
        realloc(*rectangles, new_capacity * sizeof(**rectangles));
    if (resized == NULL) {
        return false;
    }
    *rectangles = resized;
    *capacity = new_capacity;
    return true;
}

static size_t find_rectangle(const Rectangle *rectangles, size_t count,
                             int id) {
    for (size_t i = 0; i < count; i++) {
        if (rectangles[i].id == id) {
            return i;
        }
    }
    return count;
}

static void display_rectangle(const Rectangle *rectangle) {
    double scale = fmax(1.0, fmax(rectangle->width, rectangle->height));
    bool is_square =
        fabs(rectangle->width - rectangle->height) <=
        SQUARE_RELATIVE_TOLERANCE * scale;

    printf("ID: %d | Width: %.10g | Height: %.10g | Area: %.10g | "
           "Perimeter: %.10g | %s\n",
           rectangle->id, rectangle->width, rectangle->height,
           rectangle->area, rectangle->perimeter,
           is_square ? "Square" : "Non-square");
}

static void display_all(const Rectangle *rectangles, size_t count) {
    if (count == 0) {
        puts("The collection is empty.");
        return;
    }
    for (size_t i = 0; i < count; i++) {
        display_rectangle(&rectangles[i]);
    }
}

static bool read_unique_id(const Rectangle *rectangles, size_t count,
                           const char *prompt, int *id) {
    for (;;) {
        if (!read_integer(prompt, 1, INT_MAX, id)) {
            return false;
        }
        if (!contains_id(rectangles, count, *id)) {
            return true;
        }
        printf("ID %d already exists; IDs must be unique.\n", *id);
    }
}

static bool add_rectangle(Rectangle **rectangles, size_t *count,
                          size_t *capacity) {
    if (*count >= MAX_RECTANGLES) {
        printf("The collection cannot exceed %d rectangles.\n",
               MAX_RECTANGLES);
        return true;
    }

    Rectangle rectangle = {0};
    if (!read_unique_id(*rectangles, *count, "Rectangle ID: ",
                        &rectangle.id) ||
        !read_dimensions(&rectangle.width, &rectangle.height,
                        &rectangle.area, &rectangle.perimeter)) {
        return false;
    }

    if (!reserve_rectangles(rectangles, capacity, *count + 1)) {
        puts("Could not grow the rectangle array; no rectangle was added.");
        return true;
    }
    (*rectangles)[*count] = rectangle;
    (*count)++;
    puts("Rectangle added successfully.");
    return true;
}

static bool display_by_id(const Rectangle *rectangles, size_t count) {
    int id;
    if (!read_integer("Rectangle ID to search for: ", 1, INT_MAX, &id)) {
        return false;
    }

    size_t index = find_rectangle(rectangles, count, id);
    if (index == count) {
        printf("No rectangle has ID %d.\n", id);
    } else {
        display_rectangle(&rectangles[index]);
    }
    return true;
}

static bool update_rectangle(Rectangle *rectangles, size_t count) {
    int id;
    if (!read_integer("Rectangle ID to update: ", 1, INT_MAX, &id)) {
        return false;
    }

    size_t index = find_rectangle(rectangles, count, id);
    if (index == count) {
        printf("No rectangle has ID %d.\n", id);
        return true;
    }

    double width;
    double height;
    double area;
    double perimeter;
    if (!read_dimensions(&width, &height, &area, &perimeter)) {
        return false;
    }

    rectangles[index].width = width;
    rectangles[index].height = height;
    rectangles[index].area = area;
    rectangles[index].perimeter = perimeter;
    puts("Rectangle updated successfully.");
    return true;
}

static bool delete_rectangle(Rectangle *rectangles, size_t *count) {
    int id;
    if (!read_integer("Rectangle ID to delete: ", 1, INT_MAX, &id)) {
        return false;
    }

    size_t index = find_rectangle(rectangles, *count, id);
    if (index == *count) {
        printf("No rectangle has ID %d; nothing was deleted.\n", id);
        return true;
    }

    if (index + 1 < *count) {
        memmove(&rectangles[index], &rectangles[index + 1],
                (*count - index - 1) * sizeof(*rectangles));
    }
    (*count)--;
    printf("Rectangle %d deleted successfully.\n", id);
    return true;
}

static void display_area_summary(const Rectangle *rectangles, size_t count) {
    if (count == 0) {
        puts("The collection is empty; area statistics are unavailable.");
        return;
    }

    size_t smallest = 0;
    size_t largest = 0;
    long double total_area = 0.0L;
    long double average_area = 0.0L;
    for (size_t i = 0; i < count; i++) {
        if (rectangles[i].area < rectangles[smallest].area) {
            smallest = i;
        }
        if (rectangles[i].area > rectangles[largest].area) {
            largest = i;
        }
        total_area += (long double)rectangles[i].area;
        average_area += ((long double)rectangles[i].area - average_area) /
                        (long double)(i + 1);
    }

    printf("Smallest area: ID %d (%.10g)\n", rectangles[smallest].id,
           rectangles[smallest].area);
    printf("Largest area: ID %d (%.10g)\n", rectangles[largest].id,
           rectangles[largest].area);
    if (isfinite(total_area)) {
        printf("Total area: %.17Lg\n", total_area);
    } else {
        puts("Total area exceeds the representable range.");
    }
    printf("Average area: %.17Lg\n", average_area);
}

static int compare_by_area(const void *left, const void *right) {
    const Rectangle *a = left;
    const Rectangle *b = right;
    if (a->area < b->area) {
        return -1;
    }
    if (a->area > b->area) {
        return 1;
    }
    if (a->id < b->id) {
        return -1;
    }
    if (a->id > b->id) {
        return 1;
    }
    return 0;
}

static void display_sorted(const Rectangle *rectangles, size_t count) {
    if (count == 0) {
        puts("The collection is empty.");
        return;
    }
    if (count > SIZE_MAX / sizeof(*rectangles)) {
        puts("Cannot allocate a sorted copy: collection size is too large.");
        return;
    }

    Rectangle *sorted = malloc(count * sizeof(*sorted));
    if (sorted == NULL) {
        puts("Could not allocate memory to sort the rectangles.");
        return;
    }
    memcpy(sorted, rectangles, count * sizeof(*sorted));
    qsort(sorted, count, sizeof(*sorted), compare_by_area);
    puts("Rectangles sorted by ascending area, then ID (a copy; original order is unchanged):");
    display_all(sorted, count);
    free(sorted);
}

static bool filter_by_area(const Rectangle *rectangles, size_t count) {
    double threshold;
    if (!read_finite_number("Show rectangles with area greater than: ",
                            &threshold)) {
        return false;
    }

    bool found = false;
    for (size_t i = 0; i < count; i++) {
        if (rectangles[i].area > threshold) {
            display_rectangle(&rectangles[i]);
            found = true;
        }
    }
    if (!found) {
        puts("No rectangles have an area above that threshold.");
    }
    return true;
}

static bool read_initial_rectangle(Rectangle **rectangles, size_t *count,
                                   size_t *capacity, int ordinal,
                                   int total) {
    Rectangle rectangle = {0};
    printf("\nRectangle %d of %d:\n", ordinal, total);
    if (!read_unique_id(*rectangles, *count, "Rectangle ID: ",
                        &rectangle.id) ||
        !read_dimensions(&rectangle.width, &rectangle.height,
                        &rectangle.area, &rectangle.perimeter)) {
        return false;
    }

    if (!reserve_rectangles(rectangles, capacity, *count + 1)) {
        puts("Could not allocate memory for a rectangle.");
        return false;
    }
    (*rectangles)[*count] = rectangle;
    (*count)++;
    return true;
}

static void display_menu(void) {
    puts("\n1. Add rectangle");
    puts("2. Display all rectangles");
    puts("3. Search by ID");
    puts("4. Update dimensions by ID");
    puts("5. Delete by ID");
    puts("6. Area statistics");
    puts("7. Display sorted by area");
    puts("8. Filter by area threshold");
    puts("0. Exit");
}

int main(void) {
    int initial_count;
    printf("Maximum collection size: %d rectangles.\n", MAX_RECTANGLES);
    printf("Square tolerance: |width - height| <= %.0e * max(1, width, height).\n",
           SQUARE_RELATIVE_TOLERANCE);
    if (!read_integer("Initial number of rectangles (1-1000): ", 1,
                      MAX_RECTANGLES, &initial_count)) {
        return 0;
    }

    size_t capacity = (size_t)initial_count;
    size_t count = 0;
    if (capacity > SIZE_MAX / sizeof(Rectangle)) {
        puts("Requested collection size is too large.");
        return 1;
    }

    Rectangle *rectangles = malloc(capacity * sizeof(*rectangles));
    if (rectangles == NULL) {
        puts("Could not allocate memory for the rectangle array.");
        return 1;
    }

    for (int i = 0; i < initial_count; i++) {
        if (!read_initial_rectangle(&rectangles, &count, &capacity, i + 1,
                                    initial_count)) {
            free(rectangles);
            puts("\nInput ended or initial rectangle could not be allocated; memory released.");
            return 0;
        }
    }

    bool running = true;
    while (running) {
        int choice;
        display_menu();
        if (!read_integer("Choose an operation: ", 0, 8, &choice)) {
            break;
        }

        switch (choice) {
            case 1:
                running = add_rectangle(&rectangles, &count, &capacity);
                break;
            case 2:
                display_all(rectangles, count);
                break;
            case 3:
                running = display_by_id(rectangles, count);
                break;
            case 4:
                running = update_rectangle(rectangles, count);
                break;
            case 5:
                running = delete_rectangle(rectangles, &count);
                break;
            case 6:
                display_area_summary(rectangles, count);
                break;
            case 7:
                display_sorted(rectangles, count);
                break;
            case 8:
                running = filter_by_area(rectangles, count);
                break;
            case 0:
                running = false;
                break;
        }
    }

    free(rectangles);
    puts("Rectangle records closed; allocated memory released.");
    return 0;
}
