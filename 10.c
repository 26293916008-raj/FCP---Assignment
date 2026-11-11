#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum to track the type of data stored
typedef enum {
    INT, FLOAT, CHAR, STRING
} DataType;

// Union to store different types of data
typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

// Struct to encapsulate the data and its type
typedef struct {
    Data data;
    DataType type;
} Container;

// Function declarations
void set_int(Container *c, int value);
void set_float(Container *c, float value);
void set_char(Container *c, char value);
void set_string(Container *c, const char *value);
int get_int(Container *c);
float get_float(Container *c);
char get_char(Container *c);
char* get_string(Container *c);
void free_container(Container *c);

int main() {
    Container c;

    set_int(&c, 10);
    printf("Int: %d\n", get_int(&c));

    set_float(&c, 3.14);
    printf("Float: %f\n", get_float(&c));

    set_char(&c, 'A');
    printf("Char: %c\n", get_char(&c));

    set_string(&c, "Hello, World!");
    printf("String: %s\n", get_string(&c));

    // Free any allocated memory
    free_container(&c);

    return 0;
}

// Function definitions
void set_int(Container *c, int value) {
    c->data.int_value = value;
    c->type = INT;
}

void set_float(Container *c, float value) {
    c->data.float_value = value;
    c->type = FLOAT;
}

void set_char(Container *c, char value) {
    c->data.char_value = value;
    c->type = CHAR;
}

void set_string(Container *c, const char *value) {
    // Free previous string if any
    if (c->type == STRING) {
        free(c->data.string_value);
    }
    c->data.string_value = strdup(value);
    c->type = STRING;
}

int get_int(Container *c) {
    if (c->type != INT) {
        printf("Error: Not an int\n");
        return 0;
    }
    return c->data.int_value;
}

float get_float(Container *c) {
    if (c->type != FLOAT) {
        printf("Error: Not a float\n");
        return 0.0;
    }
    return c->data.float_value;
}

char get_char(Container *c) {
    if (c->type != CHAR) {
        printf("Error: Not a char\n");
        return '\0';
    }
    return c->data.char_value;
}

char* get_string(Container *c) {
    if (c->type != STRING) {
        printf("Error: Not a string\n");
        return NULL;
    }
    return c->data.string_value;
}

void free_container(Container *c) {
    if (c->type == STRING) {
        free(c->data.string_value);
    }
}
