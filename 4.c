// Structures - Student Records
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 10000
#define NAME_SIZE 100
#define INPUT_SIZE 512

typedef struct {
    int roll_number;
    char name[NAME_SIZE];
    int age;
    double gpa;
} Student;

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

static bool read_valid_gpa(double *gpa) {
    char input[INPUT_SIZE];

    for (;;) {
        LineStatus status = read_line("GPA (0.00-4.00): ", input,
                                      sizeof(input));
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
            only_trailing_whitespace(end) && isfinite(parsed) &&
            parsed >= 0.0 && parsed <= 4.0) {
            *gpa = parsed;
            return true;
        }
        puts("GPA must be a finite number from 0.00 to 4.00.");
    }
}

static bool name_is_valid(const char *name) {
    if (name[0] == '\0' || strlen(name) >= NAME_SIZE) {
        return false;
    }
    for (const unsigned char *character = (const unsigned char *)name;
         *character != '\0'; character++) {
        if (!isspace(*character)) {
            return true;
        }
    }
    return false;
}

static bool read_valid_name(char name[NAME_SIZE]) {
    char input[INPUT_SIZE];

    for (;;) {
        LineStatus status = read_line("Name: ", input, sizeof(input));
        if (status == LINE_EOF) {
            return false;
        }
        if (status == LINE_TOO_LONG) {
            puts("Name is too long. Maximum length is 99 characters.");
            continue;
        }
        if (strlen(input) >= NAME_SIZE) {
            puts("Name is too long. Maximum length is 99 characters.");
            continue;
        }
        if (!name_is_valid(input)) {
            puts("Name cannot be empty or contain only whitespace.");
            continue;
        }

        strcpy(name, input);
        return true;
    }
}

static bool read_student_details(Student *student) {
    if (!read_valid_name(student->name) ||
        !read_integer("Age (16-100): ", 16, 100, &student->age) ||
        !read_valid_gpa(&student->gpa)) {
        return false;
    }
    return true;
}

static bool reserve_students(Student **students, size_t *capacity,
                             size_t required_capacity) {
    if (required_capacity <= *capacity) {
        return true;
    }
    if (required_capacity > MAX_STUDENTS ||
        required_capacity > SIZE_MAX / sizeof(**students)) {
        return false;
    }

    size_t new_capacity = *capacity == 0 ? 1 : *capacity;
    while (new_capacity < required_capacity) {
        if (new_capacity > MAX_STUDENTS / 2) {
            new_capacity = MAX_STUDENTS;
        } else {
            new_capacity *= 2;
        }
    }
    if (new_capacity > SIZE_MAX / sizeof(**students)) {
        return false;
    }

    Student *resized = realloc(*students, new_capacity * sizeof(**students));
    if (resized == NULL) {
        return false;
    }

    *students = resized;
    *capacity = new_capacity;
    return true;
}

static size_t find_student(const Student *students, size_t count,
                           int roll_number) {
    for (size_t i = 0; i < count; i++) {
        if (students[i].roll_number == roll_number) {
            return i;
        }
    }
    return count;
}

static void display_student(const Student *student) {
    printf("Roll number: %d | Name: %s | Age: %d | GPA: %.2f\n",
           student->roll_number, student->name, student->age, student->gpa);
}

static void display_students(const Student *students, size_t count) {
    if (count == 0) {
        puts("The student collection is empty.");
        return;
    }

    for (size_t i = 0; i < count; i++) {
        display_student(&students[i]);
    }
}

static bool add_student(Student **students, size_t *count, size_t *capacity) {
    if (*count >= MAX_STUDENTS) {
        printf("The collection is at its maximum of %d students.\n",
               MAX_STUDENTS);
        return true;
    }

    Student student = {0};
    if (!read_integer("Roll number: ", 1, INT_MAX, &student.roll_number)) {
        return false;
    }
    if (find_student(*students, *count, student.roll_number) != *count) {
        printf("Roll number %d already exists; no student was added.\n",
               student.roll_number);
        return true;
    }
    if (!read_student_details(&student)) {
        return false;
    }

    if (!reserve_students(students, capacity, *count + 1)) {
        puts("Could not allocate more memory; no student was added.");
        return true;
    }
    (*students)[*count] = student;
    (*count)++;
    puts("Student added successfully.");
    return true;
}

static bool search_student(const Student *students, size_t count) {
    int roll_number;
    if (!read_integer("Roll number to search: ", 1, INT_MAX, &roll_number)) {
        return false;
    }

    size_t index = find_student(students, count, roll_number);
    if (index == count) {
        printf("No student has roll number %d.\n", roll_number);
    } else {
        display_student(&students[index]);
    }
    return true;
}

static bool update_student(Student *students, size_t count) {
    int roll_number;
    if (!read_integer("Roll number to update: ", 1, INT_MAX, &roll_number)) {
        return false;
    }

    size_t index = find_student(students, count, roll_number);
    if (index == count) {
        printf("No student has roll number %d.\n", roll_number);
        return true;
    }

    Student updated = students[index];
    puts("Enter the student's updated details (roll number is unchanged):");
    if (!read_student_details(&updated)) {
        return false;
    }

    students[index] = updated;
    puts("Student updated successfully.");
    return true;
}

static bool delete_student(Student *students, size_t *count) {
    int roll_number;
    if (!read_integer("Roll number to delete: ", 1, INT_MAX, &roll_number)) {
        return false;
    }

    size_t index = find_student(students, *count, roll_number);
    if (index == *count) {
        printf("No student has roll number %d; nothing was deleted.\n",
               roll_number);
        return true;
    }

    if (index + 1 < *count) {
        memmove(&students[index], &students[index + 1],
                (*count - index - 1) * sizeof(*students));
    }
    (*count)--;
    printf("Student with roll number %d deleted successfully.\n", roll_number);
    return true;
}

static int compare_by_gpa_descending(const void *left, const void *right) {
    const Student *a = left;
    const Student *b = right;
    if (a->gpa > b->gpa) {
        return -1;
    }
    if (a->gpa < b->gpa) {
        return 1;
    }
    if (a->roll_number < b->roll_number) {
        return -1;
    }
    if (a->roll_number > b->roll_number) {
        return 1;
    }
    return 0;
}

static void display_sorted_students(const Student *students, size_t count) {
    if (count == 0) {
        puts("The student collection is empty.");
        return;
    }

    if (count > SIZE_MAX / sizeof(*students)) {
        puts("Cannot create a sorted copy: allocation size is too large.");
        return;
    }
    Student *sorted = malloc(count * sizeof(*sorted));
    if (sorted == NULL) {
        puts("Could not allocate memory to sort the student records.");
        return;
    }

    memcpy(sorted, students, count * sizeof(*sorted));
    qsort(sorted, count, sizeof(*sorted), compare_by_gpa_descending);
    puts("Students sorted by descending GPA (a copy; original order is unchanged):");
    display_students(sorted, count);
    free(sorted);
}

static void display_gpa_summary(const Student *students, size_t count) {
    if (count == 0) {
        puts("The student collection is empty; GPA summary is unavailable.");
        return;
    }

    size_t highest = 0;
    size_t lowest = 0;
    long double average = 0.0L;
    for (size_t i = 0; i < count; i++) {
        if (students[i].gpa > students[highest].gpa) {
            highest = i;
        }
        if (students[i].gpa < students[lowest].gpa) {
            lowest = i;
        }
        average += ((long double)students[i].gpa - average) /
                   (long double)(i + 1);
    }

    puts("Student with the highest GPA:");
    display_student(&students[highest]);
    puts("Student with the lowest GPA:");
    display_student(&students[lowest]);
    printf("Average GPA: %.2Lf\n", average);
}

static void display_menu(void) {
    puts("\n1. Add student");
    puts("2. Display all students");
    puts("3. Search by roll number");
    puts("4. Update student");
    puts("5. Delete student");
    puts("6. GPA summary");
    puts("7. Display sorted by GPA");
    puts("0. Exit");
}

int main(void) {
    int initial_capacity;
    printf("Maximum collection capacity: %d students.\n", MAX_STUDENTS);
    if (!read_integer("Initial capacity (0-10000): ", 0, MAX_STUDENTS,
                      &initial_capacity)) {
        return 0;
    }

    size_t capacity = (size_t)initial_capacity;
    size_t count = 0;
    Student *students = NULL;
    if (capacity > 0) {
        if (capacity > SIZE_MAX / sizeof(*students)) {
            puts("Requested capacity is too large.");
            return 1;
        }
        students = malloc(capacity * sizeof(*students));
        if (students == NULL) {
            puts("Could not allocate memory for the requested capacity.");
            return 1;
        }
    }

    if (capacity == 0) {
        puts("Started with an empty, zero-capacity collection; it will grow when a student is added.");
    } else {
        printf("Created an empty collection with capacity for %zu students.\n",
               capacity);
    }

    bool running = true;
    while (running) {
        int choice;
        display_menu();
        if (!read_integer("Choose an operation: ", 0, 7, &choice)) {
            break;
        }

        switch (choice) {
            case 1:
                running = add_student(&students, &count, &capacity);
                break;
            case 2:
                display_students(students, count);
                break;
            case 3:
                running = search_student(students, count);
                break;
            case 4:
                running = update_student(students, count);
                break;
            case 5:
                running = delete_student(students, &count);
                break;
            case 6:
                display_gpa_summary(students, count);
                break;
            case 7:
                display_sorted_students(students, count);
                break;
            case 0:
                running = false;
                break;
        }
    }

    free(students);
    puts("Student records closed; allocated memory released.");
    return 0;
}
