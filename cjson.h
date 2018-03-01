#include <stddef.h>
/**
 * Created with CLion
 * User: zzzz76
 * Date: 2018-02-17
 */

#ifndef CJSON_PARSE_VALUE_H
#define CJSON_PARSE_VALUE_H

typedef enum {
    VALUE_NULL, VALUE_FALSE, VALUE_TRUE, VALUE_STRING, VALUE_ARRAY, VALUE_OBJECT, VALUE_NUMBER
} value_type;

typedef struct cjson_value cjson_value;
typedef struct cjson_member cjson_member;

struct cjson_value {
    union {
        struct { cjson_member *object;size_t size; } o;
        struct { cjson_value *array;size_t size; } a;
        struct { char *s;size_t len; } s;
        double n;
    } u;
    value_type type;
};

struct cjson_member {
    char *key;
    size_t len;
    cjson_value value;
};

enum {
    PARSE_VALUE_OK = 0,
    PARSE_VALUE_EXPECT,
    PARSE_VALUE_INVALID,
    PARSE_VALUE_MISS_COMMA_OR_SQUARE_BRACKET,
    LEPT_PARSE_ROOT_NOT_SINGULAR
};
int cjson_parse(cjson_value *v, const char *json);

value_type get_value_type(cjson_value *v);

const char *get_value_string(cjson_value *v);

size_t get_value_string_len(cjson_value *v);

void free_value(cjson_value *v);

void free_member(cjson_member *m);

double get_value_number(cjson_value *v);

void init_value(cjson_value *v);

cjson_value *get_value_array(cjson_value *v);

size_t get_value_array_size(cjson_value *v);

#endif //CJSON_PARSE_VALUE_H
