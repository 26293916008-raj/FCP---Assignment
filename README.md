# FCP Programming Assignments: Structures, Unions, and Enumerations

For each source (`.C`) file, the **Current Problem Statement** describes what the supplied code actually demonstrates. The **Extended Problem Statement** is an extended assignment to be implemented by modifying and enhancing the corresponding file.

## Assignment index

| Source file | Original exercise | Extended assignment |
|---|---|---|
| `4.c` | Dynamically allocated student records | Student Records and Academic Analytics System |
| `5.c` | Dynamically allocated rectangles and area calculation | Rectangle Inventory and Geometry Analysis |
| `6.c` | Comparing memory layout of structures and unions | Memory-Efficient Typed Records and Safe Union Access |
| `7.c` | String-to-enum command interpretation | Interactive Command-Driven Record Manager |
| `10.c` | A tagged union for four data types | Safe Heterogeneous Data Container Library |

---

## Problem 1 — Student Records Using a Dynamically Allocated Structure Array

**Source:** `4.c` 

### A. Current Problem Statement

Write a C program to represent a student using a structure named `Student`, containing a name (up to 49 characters plus a null terminator), age (`int`), and GPA (`float`). Define a function that allocates memory dynamically for a specified number of student records, a function that assigns a student's details at a given array index, a function that displays all stored records, and a function that releases the allocated memory. In `main`, allocate space for three students; insert **Aladdin (20, 3.5)**, **Batman (21, 3.7)**, and **Catwoman (22, 3.8)**; print their details to two decimal places for GPA; and deallocate the array before termination. No runtime user input is required in this demonstration.

### B. Extended Problem Statement — Student Records and Academic Analytics System

Upgrade the fixed three-record demonstration into a menu-driven student records system supporting user input, validation, modification, and academic analysis.

**Required functionality**

1. Read an initial capacity from the user, allocate a `Student` array dynamically, and maintain separate **record count** and **allocated capacity** values. Handle zero, negative, excessively large, or allocation-failing requests gracefully.
2. Expand each record to contain a unique positive **roll number**, name, age, and GPA. Accept names containing spaces, reject names exceeding the storage limit, validate age within **16–100**, and validate GPA within **0.00–4.00**.
3. Implement distinct operations to **add**, **display**, **search by roll number**, **update**, and **delete** student records. Reject duplicate roll numbers. Preserve the other records when one record is deleted.
4. When the array is full, grow its capacity dynamically using `realloc`. Keep the original pointer unchanged if resizing fails, and prevent arithmetic overflow when computing allocation sizes.
5. Display the student with the **highest GPA**, the student with the **lowest GPA**, and the **average GPA** across all stored records. Define an appropriate message when the collection is empty.
6. Display students **sorted by GPA in descending order**, breaking ties by ascending roll number. The sorting operation may rearrange the array or work on a copy, but its choice must be documented.
7. Show clear success and error messages and release all allocated memory on normal program exit.

**Illustrative usage scenario:** A user creates a student collection, adds roll numbers 101, 102, and 103, searches for 102, updates its GPA, removes 101, views the sorted list, and requests the GPA summary.

**Required edge-case tests:** empty collection; duplicate roll number; invalid age/GPA; maximum-length name; name with spaces; search / delete of a non-existent roll number; insertion that triggers reallocation; no available memory (handled in code).

**Expected deliverable:** An updated `4.c` with well-separated functions for records management, allocation, validation, sorting, and summary reporting.

---

## Problem 2 — Rectangle Areas Using Structures and Dynamic Memory

**Source:** `5.c`

### A. Current Problem Statement

Define a `Rectangle` structure with three `float` fields: width, height, and area. Implement a function to compute and store a rectangle's area using **area = width × height**, a function to dynamically allocate an array of rectangle structures, a function to populate the width, height, and computed area of a selected rectangle, a function to display all rectangles, and a function to deallocate the array. In `main`, allocate space for two rectangles with dimensions **5.0 × 4.0** and **6.5 × 3.5**, compute their areas (**20.00** and **22.75**), print width, height, and area to two decimal places, and free the memory. The dimensions are hard-coded in the supplied demonstration.

### B. Extended Problem Statement — Rectangle Inventory and Geometry Analysis

Extend the code into an interactive geometry program that stores, updates, compares, and filters dynamically allocated rectangles.

**Required functionality**

1. Read the number of rectangles at runtime and dynamically allocate an array. Require a positive number within a documented upper bound (for example, **1–1,000**), with allocation failure handling.
2. For each rectangle, read a unique positive **rectangle ID**, width, and height. Accept only **finite positive** dimensions; reject zero, negative values, `NaN`, and infinity.
3. Store and maintain **area** and **perimeter**, where `area = width * height` and `perimeter = 2 * (width + height)`. All derived fields must be recomputed after any dimension change.
4. Implement operations to **add a rectangle**, **display all rectangles**, **search by ID**, **update dimensions by ID**, and **delete by ID**. Reject duplicate IDs and invalid indices.
5. Identify rectangles with the **largest** and **smallest area** and calculate the **total area** and **average area** of the collection. Handle the empty-collection case after deletions.
6. Classify each rectangle as a **square** or **non-square** using a documented floating-point comparison tolerance rather than exact equality.
7. List rectangles in **ascending order of area**, breaking ties by rectangle ID. Add a filter that displays only rectangles whose area exceeds a user-entered threshold.
8. Manage capacity correctly as rectangles are added or removed, avoid out-of-bounds accesses, and free allocated memory before termination.

**Illustrative usage scenario:** The user inserts rectangles 201 (5 × 4), 202 (6.5 × 3.5), and 203 (4 × 4), views the sorted list and square classifications, updates rectangle 201, then requests the total and average area.

**Required edge-case tests:** invalid dimensions; `NaN`/infinite input; duplicate ID; single rectangle; equal areas; square with non-integer dimensions; empty list after deletion; resizing failure.

**Expected deliverable:** An updated `5.c` in which each operation is implemented as a separate function and the geometry fields remain consistent after updates.

---

## Problem 3 — Memory Representation of Structures and Unions

**Source:** `6.c` 

### A. Current Problem Statement

Create a union named `UnionData` and a structure named `StructData`, each declaring the same three types of members: an integer, a floating-point number, and a character array of 20 bytes. Assign **10**, **220.5**, and a short string to the structure's members, and demonstrate assigning these kinds of values to the union **one at a time**. Print the union member values immediately after their respective assignments, display the addresses of all members in both the union and the structure, and compare `sizeof(UnionData)` with `sizeof(StructData)`. Explain why union members overlap in memory whereas structure members normally occupy separate locations, and why alignment/padding may affect their sizes.

### B. Extended Problem Statement — Memory-Efficient Typed Records and Safe Union Access

Redesign the demonstration as a memory-analysis utility and a safe typed-value record system that uses an enumeration to track which union member is currently valid.

**Required functionality**

1. Retain `StructData` and `UnionData` with matching member declarations. Print the **size and alignment** of each type and the address of each member, formatting pointer values with `%p` after conversion to `(void *)` where appropriate.
2. For the structure, use `offsetof` from `<stddef.h>` to print the byte offset of every field. Explain the relationship between these offsets, type alignment, and possible padding.
3. Show that a union's members begin at the same address, while each structure field has its own offset. Compare the memory needed for **100 structure records** and **100 union records** using `sizeof`, without assuming a specific memory layout across platforms.
4. Define `enum ValueType { VALUE_INT, VALUE_FLOAT, VALUE_TEXT }` (or an equivalent tagged enumeration), then define a structure containing a `UnionData` value **and** a `ValueType` tag.
5. Implement validated functions to **set an integer**, **set a float**, or **set a short string** in this tagged record, updating the tag each time. Strings must fit in the 20-byte array including the null terminator.
6. Implement a function to display **only the currently active member**, based on its tag. Do not read a stale/inactive member as if it still contained the previous value, and never print arbitrary overwritten bytes as a C string.
7. Store at least **five tagged records of mixed types** and display each safely. Report the size of the tagged record and compare it with the plain union and plain structure.
8. Include a short observation section describing why a union can save memory when a record holds **one of several types at a time**, but does not allow those values to coexist independently.

**Illustrative usage scenario:** Record 1 holds integer 10; record 2 holds float 220.5; record 3 holds `"Union Programming"`; records 4 and 5 contain another valid integer and short string. Each is printed according to its stored tag.

**Required edge-case tests:** maximum-length valid string (19 characters); overlength string; switching a single record from integer to text to float; invalid tag handling; differences in `sizeof` and alignment on the compilation platform.

**Expected deliverable:** An updated `6.c` that demonstrates memory layout without invoking unsafe union-member reads, plus a brief written interpretation of observed output.

---

## Problem 4 — Command Interpretation Using Enumerations

**Source:** `7.c`  

### A. Current Problem Statement

Build a console-based C program that repeatedly accepts one of four text commands: **ADD**, **DELETE**, **VIEW**, or **EXIT**. Define a `CommandType` enumeration containing a constant for each supported command and another constant for unknown commands. Implement a function that maps an entered command string to the corresponding enumeration value using string comparisons. In `main`, use a loop and `switch` statement to display the message corresponding to each command, report unsupported commands, and terminate only when `EXIT` is entered. In the supplied code, ADD, DELETE, and VIEW **only print confirmation messages**; they do not yet manipulate records. Commands are case-sensitive.

### B. Extended Problem Statement — Interactive Command-Driven Record Manager

Convert the command interpreter into a working in-memory **task-list manager** in which the enumeration directs real operations instead of merely printing placeholder messages.

**Required functionality**

1. Maintain a dynamic array of `Task` structures, each with a unique positive **task ID**, short **description**, and status (`PENDING` or `DONE` represented by an enum).
2. Support commands **ADD**, **DELETE**, **VIEW**, **UPDATE**, **COMPLETE**, **FIND**, **HELP**, and **EXIT**. Expand the command enumeration and string-to-enum conversion function accordingly.
3. For `ADD`, read a task ID and description (including spaces), reject duplicate IDs, and append the task. If capacity is reached, expand it using `realloc` with error handling.
4. For `DELETE`, request an ID, delete the matching task, and close the gap in the array. For `UPDATE`, change an existing task's description. For `COMPLETE`, mark the matching task `DONE`.
5. For `VIEW`, list all tasks with ID, description, and status; for `FIND`, display one task by ID; for `HELP`, show the recognized command names and their meanings.
6. Make command names **case-insensitive** (for example, `add`, `Add`, and `ADD` must be equivalent). Treat unrecognized commands as errors without terminating the session.
7. Read command text and task descriptions with **bounded input operations** (such as `fgets`); detect and handle an overlong line rather than relying on unbounded `%s`.
8. Handle malformed numeric IDs, missing IDs, empty descriptions, an empty task list, and end-of-file (`EOF`) gracefully. Release all allocated memory on `EXIT` or input termination.
9. After every successful mutation, print a confirmation that includes the affected task ID. No file persistence is required; the task list exists only while the program runs.

**Illustrative usage scenario:** `add` creates task 101; `ADD` creates task 102; `view` displays both; `complete` marks 101 done; `find` retrieves 101; `delete` removes 102; `help` lists commands; `exit` terminates.

**Required edge-case tests:** mixed-case commands; unknown command; excessive command length; duplicate task ID; nonexistent task ID; spaces in descriptions; empty collection; input `EOF`; dynamic reallocation.

**Expected deliverable:** An updated `7.c` with an enumeration-driven parser, functions for each command, and actual persistent-in-session task-list behavior.

---

## Problem 5 — A Typed Container Using a Structure, Union, and Enumeration

**Source:** `10.c`  

### A. Current Problem Statement

Implement a container that can hold **exactly one active value at a time**, selected from four types: `int`, `float`, `char`, and a dynamically allocated string. Define a `DataType` enumeration identifying the stored type, a `Data` union containing a member for each type, and a `Container` structure combining the union with its type tag. Provide a setter and getter for each type and a cleanup function for memory allocated to a string. Demonstrate setting an integer (`10`), a float (`3.14`), a character (`'A'`), and a string (`"Hello, World!"`) sequentially in the same container and printing each through its corresponding getter.

### B. Extended Problem Statement — Safe Heterogeneous Data Container Library

Build a reusable, memory-safe typed-value container capable of storing and replacing values of different C types, including text, and demonstrate it in a collection of mixed-type records.

**Required functionality**

1. Add a **`TYPE_EMPTY`** enumeration value and implement `container_init` so that every new container starts in a known, valid state. A container must never rely on an uninitialized type tag.
2. Implement setters for integer, float, character, and string values. Before replacing the current value, free any string owned by the container when necessary. String assignment must make an **independent heap-allocated copy** of the input text; do not retain a pointer to the caller's temporary buffer.
3. Check all allocation operations. If allocating a replacement string fails, **preserve the container's previous valid value** and return a failure status.
4. Implement **type-safe getters** that indicate success or failure separately from the returned value, for example `int container_get_int(const Container *c, int *out_value)`. Do not use valid data values such as `0` or `0.0` as error sentinels.
5. Implement `container_clear` to release owned memory and return the container to `TYPE_EMPTY`. Repeated cleanup of an already empty container must be safe. A container with a live string must not be shallow-copied into another owning container.
6. Add `container_print` to display a value according to its active type and handle an empty container. Add `container_type_name` to return a readable type label.
7. Allocate an array of **at least six containers**, populate it with a mixture of integers, floats, characters, and strings, and print each entry's array index, type label, and value.
8. Implement **deep copy** using a function such as `container_copy(Container *destination, const Container *source)`. For strings, allocate a separate buffer; changing or clearing the source must not invalidate the copied value. Define and handle copying onto oneself and allocation failure.
9. Demonstrate several type transitions, including **STRING → INT → STRING → FLOAT**, and verify that there are no leaks, double frees, or use-after-free accesses. Use `malloc` plus `strlen`/`memcpy` (or an equivalent ISO C solution) instead of depending on non-standard `strdup`.
10. Use `const` correctly for getters and display functions; validate pointer arguments; clean up the entire container array before exiting.

**Illustrative usage scenario:** Create containers holding 10, 3.14, `'A'`, `"Hello"`, 0, and `"Data"`; copy the last string container; change the original string; show that the copy still contains the initial text; then clear all containers safely.

**Required edge-case tests:** valid integer zero; wrong-type getter; empty container; empty string; allocation failure path; switching away from a string; repeated clear; deep copying a string; self-copy; string replacement with the same source pointer.

**Expected deliverable:** An updated `10.c` implementing a documented container API. Optionally split its declarations and definitions into `container.h` and `container.c`, with a separate `main.c` demonstration; if split, provide build instructions.

---

## Common submission and coding requirements

The following requirements apply to all five extensions:

- **Use C (ISO C11 or later).** Explain any library or compiler-specific dependency, and compile with warnings enabled (for example, `-std=c11 -Wall -Wextra -Wpedantic`).
- Follow **snake_case** naming for variables and functions. Use meaningful, descriptive function names, and return a success/error code from operations that can fail. Use `void` only where the function genuinely has no result to report.
- **Use braces** for every `if`, `else`, loop, and `switch` case block where needed, including single-statement branches. Group related statements with blank lines and put a blank line before the function's final `return`.
- Keep input/validation, processing, display, and memory management in **separate functions** as far as practical. Avoid unsafe functions and unchecked input/allocation operations.
- Include a short comment at the beginning of each solution describing the task; show a sample run and the edge cases tested in the comment(s).
- **Do not rewrite the source merely to replicate the existing output.** The purpose is to implement the extended features while retaining the original underlying concept.

---

**Submission:** One enhanced solution for each selected source file in the same Git Repository.

---
