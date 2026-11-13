#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_SIZE 64
#define INPUT_SIZE 512
#define DESCRIPTION_SIZE 120

typedef enum {
    CMD_ADD,
    CMD_DELETE,
    CMD_VIEW,
    CMD_UPDATE,
    CMD_COMPLETE,
    CMD_FIND,
    CMD_HELP,
    CMD_EXIT,
    CMD_UNKNOWN
} CommandType;

typedef enum {
    PENDING,
    DONE
} TaskStatus;

typedef struct {
    int id;
    char description[DESCRIPTION_SIZE];
    TaskStatus status;
} Task;

typedef enum {
    INPUT_OK,
    INPUT_EOF,
    INPUT_TOO_LONG
} InputResult;

static InputResult read_line(const char *prompt, char *buffer, size_t size) {
    fputs(prompt, stdout);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return INPUT_EOF;
    }

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
        return INPUT_OK;
    }
    if (feof(stdin)) {
        return INPUT_OK;
    }

    int character;
    while ((character = getchar()) != '\n' && character != EOF) {
    }
    return INPUT_TOO_LONG;
}

static void trim_whitespace(char *text) {
    char *start = text;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (start != text) {
        memmove(text, start, strlen(start) + 1);
    }

    size_t length = strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1])) {
        text[--length] = '\0';
    }
}

static CommandType get_command_type(const char *command) {
    char normalized[COMMAND_SIZE];
    size_t length = strlen(command);
    if (length >= sizeof(normalized)) {
        return CMD_UNKNOWN;
    }

    for (size_t i = 0; i <= length; i++) {
        normalized[i] = (char)toupper((unsigned char)command[i]);
    }

    if (strcmp(normalized, "ADD") == 0) return CMD_ADD;
    if (strcmp(normalized, "DELETE") == 0) return CMD_DELETE;
    if (strcmp(normalized, "VIEW") == 0) return CMD_VIEW;
    if (strcmp(normalized, "UPDATE") == 0) return CMD_UPDATE;
    if (strcmp(normalized, "COMPLETE") == 0) return CMD_COMPLETE;
    if (strcmp(normalized, "FIND") == 0) return CMD_FIND;
    if (strcmp(normalized, "HELP") == 0) return CMD_HELP;
    if (strcmp(normalized, "EXIT") == 0) return CMD_EXIT;
    return CMD_UNKNOWN;
}

static int parse_task_id(const char *input, int *id) {
    errno = 0;
    char *end = NULL;
    long parsed = strtol(input, &end, 10);
    if (end == input || errno == ERANGE || parsed <= 0 ||
        parsed > INT_MAX) {
        return 0;
    }
    while (isspace((unsigned char)*end)) {
        end++;
    }
    if (*end != '\0') {
        return 0;
    }
    *id = (int)parsed;
    return 1;
}

static int read_task_id(int *id) {
    char input[INPUT_SIZE];
    InputResult result = read_line("Task ID: ", input, sizeof(input));
    if (result == INPUT_EOF) {
        return 0;
    }
    if (result == INPUT_TOO_LONG) {
        puts("Task ID input is too long.");
        return -1;
    }
    trim_whitespace(input);
    if (!parse_task_id(input, id)) {
        puts("Invalid task ID. Enter a positive whole number.");
        return -1;
    }
    return 1;
}

static size_t find_task(const Task *tasks, size_t count, int id) {
    for (size_t i = 0; i < count; i++) {
        if (tasks[i].id == id) {
            return i;
        }
    }
    return count;
}

static int read_description(char description[DESCRIPTION_SIZE]) {
    char input[INPUT_SIZE];
    InputResult result = read_line("Description: ", input, sizeof(input));
    if (result == INPUT_EOF) {
        return 0;
    }
    if (result == INPUT_TOO_LONG || strlen(input) >= DESCRIPTION_SIZE) {
        puts("Description is too long (maximum 119 characters).");
        return -1;
    }

    trim_whitespace(input);
    if (input[0] == '\0') {
        puts("Description cannot be empty.");
        return -1;
    }

    strcpy(description, input);
    return 1;
}

static int reserve_tasks(Task **tasks, size_t *capacity, size_t required) {
    if (required <= *capacity) {
        return 1;
    }
    if (required > SIZE_MAX / sizeof(**tasks)) {
        return 0;
    }

    size_t new_capacity = *capacity == 0 ? 4 : *capacity;
    while (new_capacity < required) {
        if (new_capacity > SIZE_MAX / 2) {
            new_capacity = required;
            break;
        }
        new_capacity *= 2;
    }
    if (new_capacity > SIZE_MAX / sizeof(**tasks)) {
        return 0;
    }

    Task *resized = realloc(*tasks, new_capacity * sizeof(**tasks));
    if (resized == NULL) {
        return 0;
    }
    *tasks = resized;
    *capacity = new_capacity;
    return 1;
}

static int add_task(Task **tasks, size_t *count, size_t *capacity) {
    int id;
    int input_status = read_task_id(&id);
    if (input_status <= 0) {
        return input_status == 0 ? 0 : 1;
    }
    if (find_task(*tasks, *count, id) != *count) {
        printf("Task ID %d already exists; task was not added.\n", id);
        return 1;
    }

    Task task = {0};
    task.id = id;
    input_status = read_description(task.description);
    if (input_status <= 0) {
        return input_status == 0 ? 0 : 1;
    }
    task.status = PENDING;

    if (*count == SIZE_MAX || !reserve_tasks(tasks, capacity, *count + 1)) {
        puts("Could not allocate memory; task was not added.");
        return 1;
    }
    (*tasks)[*count] = task;
    (*count)++;
    printf("Task %d added successfully.\n", id);
    return 1;
}

static void display_task(const Task *task) {
    const char *status = task->status == PENDING ? "PENDING" : "DONE";
    printf("ID: %d | Description: %s | Status: %s\n", task->id,
           task->description, status);
}

static void view_tasks(const Task *tasks, size_t count) {
    if (count == 0) {
        puts("The task list is empty.");
        return;
    }
    for (size_t i = 0; i < count; i++) {
        display_task(&tasks[i]);
    }
}

static int find_task_command(const Task *tasks, size_t count) {
    int id;
    int input_status = read_task_id(&id);
    if (input_status <= 0) {
        return input_status == 0 ? 0 : 1;
    }
    size_t index = find_task(tasks, count, id);
    if (index == count) {
        printf("No task has ID %d.\n", id);
    } else {
        display_task(&tasks[index]);
    }
    return 1;
}

static int update_task(Task *tasks, size_t count) {
    int id;
    int input_status = read_task_id(&id);
    if (input_status <= 0) {
        return input_status == 0 ? 0 : 1;
    }
    size_t index = find_task(tasks, count, id);
    if (index == count) {
        printf("No task has ID %d; nothing was updated.\n", id);
        return 1;
    }

    char description[DESCRIPTION_SIZE];
    input_status = read_description(description);
    if (input_status <= 0) {
        return input_status == 0 ? 0 : 1;
    }
    strcpy(tasks[index].description, description);
    printf("Task %d updated successfully.\n", id);
    return 1;
}

static int complete_task(Task *tasks, size_t count) {
    int id;
    int input_status = read_task_id(&id);
    if (input_status <= 0) {
        return input_status == 0 ? 0 : 1;
    }
    size_t index = find_task(tasks, count, id);
    if (index == count) {
        printf("No task has ID %d; nothing was completed.\n", id);
        return 1;
    }
    tasks[index].status = DONE;
    printf("Task %d marked DONE.\n", id);
    return 1;
}

static int delete_task(Task *tasks, size_t *count) {
    int id;
    int input_status = read_task_id(&id);
    if (input_status <= 0) {
        return input_status == 0 ? 0 : 1;
    }
    size_t index = find_task(tasks, *count, id);
    if (index == *count) {
        printf("No task has ID %d; nothing was deleted.\n", id);
        return 1;
    }
    if (index + 1 < *count) {
        memmove(&tasks[index], &tasks[index + 1],
                (*count - index - 1) * sizeof(*tasks));
    }
    (*count)--;
    printf("Task %d deleted successfully.\n", id);
    return 1;
}

static void display_help(void) {
    puts("Recognized commands (case-insensitive):");
    puts("ADD      - Add a task with a positive ID and description.");
    puts("DELETE   - Delete a task by ID.");
    puts("VIEW     - List all tasks.");
    puts("UPDATE   - Change the description of a task by ID.");
    puts("COMPLETE - Mark a task DONE by ID.");
    puts("FIND     - Display one task by ID.");
    puts("HELP     - Show this command list.");
    puts("EXIT     - Exit and release allocated memory.");
}

int main(void) {
    Task *tasks = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char command_line[COMMAND_SIZE];
    int running = 1;

    puts("In-memory task list. Enter HELP to see available commands.");
    while (running) {
        InputResult input_status =
            read_line("\nCommand: ", command_line, sizeof(command_line));
        if (input_status == INPUT_EOF) {
            puts("\nInput ended; closing the task list.");
            break;
        }
        if (input_status == INPUT_TOO_LONG) {
            puts("Command is too long; enter a command of at most 63 characters.");
            continue;
        }
        trim_whitespace(command_line);

        switch (get_command_type(command_line)) {
            case CMD_ADD:
                running = add_task(&tasks, &count, &capacity);
                break;
            case CMD_DELETE:
                running = delete_task(tasks, &count);
                break;
            case CMD_VIEW:
                view_tasks(tasks, count);
                break;
            case CMD_UPDATE:
                running = update_task(tasks, count);
                break;
            case CMD_COMPLETE:
                running = complete_task(tasks, count);
                break;
            case CMD_FIND:
                running = find_task_command(tasks, count);
                break;
            case CMD_HELP:
                display_help();
                break;
            case CMD_EXIT:
                running = 0;
                break;
            case CMD_UNKNOWN:
                puts("Unknown command. Enter HELP to see available commands.");
                break;
        }
    }

    free(tasks);
    puts("Task list memory released.");
    return 0;
}
