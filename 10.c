#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TYPE_EMPTY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING
} DataType;

typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

typedef struct {
    Data data;
    DataType type;
} Container;

/*
 * API ownership contract:
 * - Call container_init before using a Container with any other API function.
 * - A Container owns its string value; do not shallow-copy Containers.
 * - Setters and container_copy return 1 on success and 0 on failure.
 * - A failed string assignment or copy leaves the destination unchanged.
 */
void container_init(Container *container);
int container_set_int(Container *container, int value);
int container_set_float(Container *container, float value);
int container_set_char(Container *container, char value);
int container_set_string(Container *container, const char *value);
int container_get_int(const Container *container, int *out_value);
int container_get_float(const Container *container, float *out_value);
int container_get_char(const Container *container, char *out_value);
int container_get_string(const Container *container, const char **out_value);
int container_copy(Container *destination, const Container *source);
void container_clear(Container *container);
const char *container_type_name(const Container *container);
void container_print(const Container *container);

static int container_has_valid_type(const Container *container) {
    return container != NULL &&
           (container->type == TYPE_EMPTY || container->type == TYPE_INT ||
            container->type == TYPE_FLOAT || container->type == TYPE_CHAR ||
            container->type == TYPE_STRING);
}

void container_init(Container *container) {
    if (container == NULL) {
        return;
    }
    container->data.string_value = NULL;
    container->type = TYPE_EMPTY;
}

void container_clear(Container *container) {
    if (container == NULL) {
        return;
    }
    if (container->type == TYPE_STRING) {
        free(container->data.string_value);
    }
    container->data.string_value = NULL;
    container->type = TYPE_EMPTY;
}

int container_set_int(Container *container, int value) {
    if (!container_has_valid_type(container)) {
        return 0;
    }
    container_clear(container);
    container->data.int_value = value;
    container->type = TYPE_INT;
    return 1;
}

int container_set_float(Container *container, float value) {
    if (!container_has_valid_type(container)) {
        return 0;
    }
    container_clear(container);
    container->data.float_value = value;
    container->type = TYPE_FLOAT;
    return 1;
}

int container_set_char(Container *container, char value) {
    if (!container_has_valid_type(container)) {
        return 0;
    }
    container_clear(container);
    container->data.char_value = value;
    container->type = TYPE_CHAR;
    return 1;
}

int container_set_string(Container *container, const char *value) {
    if (!container_has_valid_type(container) || value == NULL) {
        return 0;
    }

    size_t length = strlen(value);
    if (length == SIZE_MAX) {
        return 0;
    }
    char *copy = malloc(length + 1);
    if (copy == NULL) {
        return 0;
    }
    memcpy(copy, value, length + 1);

    container_clear(container);
    container->data.string_value = copy;
    container->type = TYPE_STRING;
    return 1;
}

int container_get_int(const Container *container, int *out_value) {
    if (out_value == NULL || !container_has_valid_type(container) ||
        container->type != TYPE_INT) {
        return 0;
    }
    *out_value = container->data.int_value;
    return 1;
}

int container_get_float(const Container *container, float *out_value) {
    if (out_value == NULL || !container_has_valid_type(container) ||
        container->type != TYPE_FLOAT) {
        return 0;
    }
    *out_value = container->data.float_value;
    return 1;
}

int container_get_char(const Container *container, char *out_value) {
    if (out_value == NULL || !container_has_valid_type(container) ||
        container->type != TYPE_CHAR) {
        return 0;
    }
    *out_value = container->data.char_value;
    return 1;
}

int container_get_string(const Container *container, const char **out_value) {
    if (out_value == NULL || !container_has_valid_type(container) ||
        container->type != TYPE_STRING ||
        container->data.string_value == NULL) {
        return 0;
    }
    *out_value = container->data.string_value;
    return 1;
}

int container_copy(Container *destination, const Container *source) {
    if (!container_has_valid_type(destination) ||
        !container_has_valid_type(source)) {
        return 0;
    }
    if (destination == source) {
        return 1;
    }
    if (source->type == TYPE_STRING && source->data.string_value == NULL) {
        return 0;
    }

    Data copied_data;
    if (source->type == TYPE_STRING) {
        size_t length = strlen(source->data.string_value);
        if (length == SIZE_MAX) {
            return 0;
        }
        copied_data.string_value = malloc(length + 1);
        if (copied_data.string_value == NULL) {
            return 0;
        }
        memcpy(copied_data.string_value, source->data.string_value,
               length + 1);
    } else {
        copied_data = source->data;
    }

    container_clear(destination);
    destination->data = copied_data;
    destination->type = source->type;
    return 1;
}

const char *container_type_name(const Container *container) {
    if (!container_has_valid_type(container)) {
        return "INVALID";
    }
    switch (container->type) {
        case TYPE_EMPTY:
            return "EMPTY";
        case TYPE_INT:
            return "INT";
        case TYPE_FLOAT:
            return "FLOAT";
        case TYPE_CHAR:
            return "CHAR";
        case TYPE_STRING:
            return "STRING";
    }
    return "INVALID";
}

void container_print(const Container *container) {
    if (!container_has_valid_type(container)) {
        fputs("<invalid container>", stdout);
        return;
    }

    switch (container->type) {
        case TYPE_EMPTY:
            fputs("<empty>", stdout);
            break;
        case TYPE_INT:
            printf("%d", container->data.int_value);
            break;
        case TYPE_FLOAT:
            printf("%g", (double)container->data.float_value);
            break;
        case TYPE_CHAR:
            printf("'%c'", container->data.char_value);
            break;
        case TYPE_STRING:
            if (container->data.string_value == NULL) {
                fputs("<invalid string>", stdout);
            } else {
                printf("\"%s\"", container->data.string_value);
            }
            break;
    }
}

static int set_or_report(int succeeded, const char *operation) {
    if (!succeeded) {
        printf("Failed to %s.\n", operation);
    }
    return succeeded;
}

int main(void) {
    const size_t record_count = 6;
    Container *records = malloc(record_count * sizeof(*records));
    if (records == NULL) {
        fputs("Could not allocate the container array.\n", stderr);
        return 1;
    }
    for (size_t i = 0; i < record_count; i++) {
        container_init(&records[i]);
    }

    Container copied_record;
    Container transition_record;
    Container empty_string_record;
    container_init(&copied_record);
    container_init(&transition_record);
    container_init(&empty_string_record);

    int success =
        set_or_report(container_set_int(&records[0], 10), "set record 0") &&
        set_or_report(container_set_float(&records[1], 3.14f),
                      "set record 1") &&
        set_or_report(container_set_char(&records[2], 'A'), "set record 2") &&
        set_or_report(container_set_string(&records[3], "Hello"),
                      "set record 3") &&
        set_or_report(container_set_int(&records[4], 0), "set record 4") &&
        set_or_report(container_set_string(&records[5], "Data"),
                      "set record 5");
    if (!success) {
        goto cleanup;
    }

    puts("Mixed-type container records:");
    for (size_t i = 0; i < record_count; i++) {
        printf("[%zu] %s: ", i, container_type_name(&records[i]));
        container_print(&records[i]);
        putchar('\n');
    }

    if (!set_or_report(container_copy(&copied_record, &records[5]),
                       "deep-copy record 5")) {
        success = 0;
        goto cleanup;
    }
    if (!set_or_report(container_set_string(&records[5], "Changed original"),
                       "change the original string")) {
        success = 0;
        goto cleanup;
    }
    printf("\nAfter changing the source string:\nOriginal: ");
    container_print(&records[5]);
    printf("\nDeep copy: ");
    container_print(&copied_record);
    putchar('\n');

    const char *same_source;
    if (!container_get_string(&records[5], &same_source) ||
        !container_set_string(&records[5], same_source)) {
        puts("Self-referential string replacement failed.");
        success = 0;
        goto cleanup;
    }
    puts("String replacement from the container's own string pointer succeeded.");

    puts("\nType transitions (STRING -> INT -> STRING -> FLOAT):");
    if (!set_or_report(container_set_string(&transition_record, "First"),
                       "set transition string") ||
        !set_or_report(container_set_int(&transition_record, 7),
                       "switch transition to int") ||
        !set_or_report(container_set_string(&transition_record, "Second"),
                       "switch transition to string") ||
        !set_or_report(container_set_float(&transition_record, 2.5f),
                       "switch transition to float")) {
        success = 0;
        goto cleanup;
    }
    printf("Final transition value (%s): ", container_type_name(&transition_record));
    container_print(&transition_record);
    putchar('\n');

    if (!set_or_report(container_set_string(&empty_string_record, ""),
                       "set an empty string")) {
        success = 0;
        goto cleanup;
    }
    printf("\nEmpty string value (%s): ", container_type_name(&empty_string_record));
    container_print(&empty_string_record);
    putchar('\n');
    const char *empty_string_value;
    if (!container_get_string(&empty_string_record, &empty_string_value) ||
        empty_string_value[0] != '\0') {
        puts("Empty string getter check failed.");
        success = 0;
        goto cleanup;
    }

    int integer_value = -1;
    if (container_get_int(&records[4], &integer_value)) {
        printf("Integer zero is a valid value: %d\n", integer_value);
    } else {
        puts("Unexpected failure reading integer record.");
        success = 0;
        goto cleanup;
    }
    float float_value;
    printf("Wrong-type float getter reports %s.\n",
           container_get_float(&records[4], &float_value) ? "success" : "failure");
    char character_value;
    if (!container_get_char(&records[2], &character_value) ||
        character_value != 'A') {
        puts("Character getter check failed.");
        success = 0;
        goto cleanup;
    }
    if (!container_get_float(&transition_record, &float_value) ||
        float_value != 2.5f) {
        puts("Float getter check failed.");
        success = 0;
        goto cleanup;
    }
    printf("Self-copy reports %s.\n",
           container_copy(&records[4], &records[4]) ? "success" : "failure");

    container_clear(&transition_record);
    container_clear(&transition_record);
    printf("Repeated clear leaves type %s: ", container_type_name(&transition_record));
    container_print(&transition_record);
    putchar('\n');

cleanup:
    for (size_t i = 0; i < record_count; i++) {
        container_clear(&records[i]);
    }
    container_clear(&copied_record);
    container_clear(&transition_record);
    container_clear(&empty_string_record);
    free(records);
    if (!success) {
        return 1;
    }
    puts("All containers cleared; memory released.");
    return 0;
}

