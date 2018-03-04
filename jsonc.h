#include <stddef.h>
/**
 * Created with CLion
 * User: zzzz76
 * Date: 2018-02-17
 */

#ifndef JSONC_PARSE_VALUE_H
#define JSONC_PARSE_VALUE_H

typedef enum {
    VALUE_NULL, VALUE_FALSE, VALUE_TRUE, VALUE_STRING, VALUE_ARRAY, VALUE_OBJECT, VALUE_NUMBER
} value_type;

typedef struct jsonc_value jsonc_value;
typedef struct jsonc_member jsonc_member;

struct jsonc_value {
    union {
        struct { jsonc_member *object;size_t size; } o;
        struct { jsonc_value *array;size_t size; } a;
        struct { char *s;size_t len; } s;
        double n;
    } u;
    value_type type;
};

struct jsonc_member {
    char *key;
    size_t len;
    jsonc_value value;
};

enum {
    JSONC_PARSE_OK = 0,
    JSONC_PARSE_EXPECT,
    JSONC_PARSE_INVALID,
    JSONC_PARSE_ROOT_NOT_SINGULAR,
    JSONC_PARSE_NUMBER_TOO_BIG,
    JSONC_PARSE_MISS_QUOTATION_MARK,
    JSONC_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
    JSONC_PARSE_MISS_COLON,
    JSONC_PARSE_MISS_COMMA_OR_CURLY_BRACKET
};

void init_value(jsonc_value *v);

int jsonc_parse(jsonc_value *v, const char *json);

void free_value(jsonc_value *v);

void free_member(jsonc_member *m);

value_type get_value_type(jsonc_value *v);

double get_value_number(jsonc_value *v);

const char *get_value_string(jsonc_value *v);

size_t get_value_string_len(jsonc_value *v);

jsonc_value *get_value_array(jsonc_value *v);

size_t get_value_array_size(jsonc_value *v);

jsonc_member *get_value_object(jsonc_value *v);

size_t get_value_object_size(jsonc_value *v);

char *get_member_key(jsonc_member *m);

size_t get_member_key_len(jsonc_member *m);

jsonc_value *get_member_value(jsonc_member *m);

#endif //JSONC_PARSE_VALUE_H
