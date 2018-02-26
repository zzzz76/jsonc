#include <stddef.h>
/**
 * Created with CLion
 * User: zzzz76
 * Date: 2018-02-17
 */

#ifndef CJSON_PARSE_VALUE_H
#define CJSON_PARSE_VALUE_H


typedef enum {
    VALUE_NULL, VALUE_FALSE, VALUE_TRUE, VALUE_STRING, VALUE_ARRAY
} value_type;

typedef struct value_object value_object;

struct value_object {
    union {
        struct {
            char *s;
            size_t len;
        } s;
        struct {
            value_object *object;
            size_t size;
        } v;
        double n;
    } u;
    value_type type;
};

enum {
    PARSE_VALUE_OK = 0,
    PARSE_VALUE_EXPECT,
    PARSE_VALUE_INVALID,
    PARSE_VALUE_MISS_COMMA_OR_SQUARE_BRACKET,
    LEPT_PARSE_ROOT_NOT_SINGULAR
};

int parse_value(value_object *object, const char *json);

value_type value_object_get_type(value_object *object);

const char *value_object_get_string(value_object *object);

size_t value_object_get_string_len(value_object *object);

void free_value(value_object *object);

void init_value_object(value_object *object);

value_object *value_object_get_array_element(value_object *object);

#endif //CJSON_PARSE_VALUE_H
