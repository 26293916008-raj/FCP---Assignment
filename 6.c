// Union memory layout and tagged values
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef union {
    int i;
    float f;
    char str[20];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[20];
} StructData;

typedef enum ValueType {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_TEXT
} ValueType;

typedef struct {
    UnionData value;
    ValueType type;
} TaggedValue;

static int set_integer(TaggedValue *record, int value) {
    if (record == NULL) {
        return 0;
    }
    record->value.i = value;
    record->type = VALUE_INT;
    return 1;
}

static int set_float(TaggedValue *record, float value) {
    if (record == NULL || !isfinite(value)) {
        return 0;
    }
    record->value.f = value;
    record->type = VALUE_FLOAT;
    return 1;
}

static int set_text(TaggedValue *record, const char *text) {
    if (record == NULL || text == NULL) {
        return 0;
    }

    size_t length = strlen(text);
    if (length >= sizeof(record->value.str)) {
        return 0;
    }

    memcpy(record->value.str, text, length + 1);
    record->type = VALUE_TEXT;
    return 1;
}

static void display_tagged_value(const TaggedValue *record) {
    if (record == NULL) {
        puts("Invalid tagged record.");
        return;
    }

    switch (record->type) {
        case VALUE_INT:
            printf("Integer: %d\n", record->value.i);
            break;
        case VALUE_FLOAT:
            printf("Float: %.2f\n", record->value.f);
            break;
        case VALUE_TEXT:
            printf("Text: %s\n", record->value.str);
            break;
        default:
            printf("Invalid value tag: %d\n", (int)record->type);
            break;
    }
}

static void print_memory_layout(void) {
    UnionData union_data = {0};
    StructData struct_data = {0};

    puts("Memory layout (sizes and alignments are platform-dependent):");
    printf("sizeof(UnionData) = %zu, _Alignof(UnionData) = %zu\n",
           sizeof(UnionData), _Alignof(UnionData));
    printf("Union member addresses: i=%p, f=%p, str=%p\n",
           (void *)&union_data.i, (void *)&union_data.f,
           (void *)&union_data.str);
    printf("sizeof(StructData) = %zu, _Alignof(StructData) = %zu\n",
           sizeof(StructData), _Alignof(StructData));
    printf("Struct member offsets: i=%zu, f=%zu, str=%zu\n",
           offsetof(StructData, i), offsetof(StructData, f),
           offsetof(StructData, str));
    printf("Struct member addresses: i=%p, f=%p, str=%p\n",
           (void *)&struct_data.i, (void *)&struct_data.f,
           (void *)&struct_data.str);
    printf("Memory for 100 records: StructData=%zu bytes, UnionData=%zu bytes\n",
           100 * sizeof(StructData), 100 * sizeof(UnionData));
    puts("Structure offsets mark each field's position. Implementations may insert padding "
         "between fields or at the end so fields and array elements meet their alignment "
         "requirements; therefore offsets and sizes vary by platform.");
    puts("All union members start at the same address. Structure fields have separate "
         "offsets and hold their values simultaneously.");
}

static int run_validation_examples(void) {
    TaggedValue test_record = {0};
    char maximum_length_text[20] = "1234567890123456789";
    const char *overlength_text = "12345678901234567890";

    puts("\nValidation and tag-switch examples:");
    if (!set_text(&test_record, maximum_length_text)) {
        puts("Could not set the maximum-length string.");
        return 0;
    }
    printf("19-character string accepted: ");
    display_tagged_value(&test_record);
    printf("20-character string rejected: %s\n",
           set_text(&test_record, overlength_text) ? "no" : "yes");

    if (!set_integer(&test_record, 10)) {
        puts("Could not switch the test record between value types.");
        return 0;
    }
    printf("Integer state: ");
    display_tagged_value(&test_record);
    if (!set_text(&test_record, "Switched to text")) {
        puts("Could not switch the test record between value types.");
        return 0;
    }
    printf("Text state: ");
    display_tagged_value(&test_record);
    if (!set_float(&test_record, 220.5f)) {
        puts("Could not switch the test record between value types.");
        return 0;
    }
    printf("Float state: ");
    display_tagged_value(&test_record);

    test_record.type = (ValueType)999;
    printf("Invalid tag is handled without reading the union: ");
    display_tagged_value(&test_record);
    return 1;
}

int main(void) {
    print_memory_layout();

    TaggedValue records[5] = {0};
    if (!set_integer(&records[0], 10) ||
        !set_float(&records[1], 220.5f) ||
        !set_text(&records[2], "Union Programming") ||
        !set_integer(&records[3], 42) ||
        !set_text(&records[4], "Short string")) {
        puts("Failed to initialize the demonstration records.");
        return 1;
    }

    puts("\nFive mixed tagged records:");
    for (size_t i = 0; i < sizeof(records) / sizeof(records[0]); i++) {
        printf("Record %zu: ", i + 1);
        display_tagged_value(&records[i]);
    }

    printf("\nsizeof(TaggedValue) = %zu, sizeof(UnionData) = %zu, "
           "sizeof(StructData) = %zu\n",
           sizeof(TaggedValue), sizeof(UnionData), sizeof(StructData));
    puts("Observation: A union can save memory when a record contains only one of "
         "several alternative values at a time, because its members share storage. "
         "Those alternatives cannot coexist independently; the tag identifies which "
         "member currently holds the valid value.");

    if (!run_validation_examples()) {
        return 1;
    }
    return 0;
}
