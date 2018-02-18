/**
 * Created with CLion
 * User: zzzz76
 * Date: 2018-02-17
 */

#ifndef CJSON_PARSE_VALUE_H
#define CJSON_PARSE_VALUE_H

#include <glob.h>

typedef enum {
    VALUE_NULL, VALUE_FALSE, VALUE_TRUE, VALUE_STRING
} value_type;

typedef struct {
    union {
        struct {
            char *s;
            size_t len;
        } s;
        double n;
    } n;
    value_type type;
} value_object;

enum {
    PARSE_VALUE_OK = 0,
    PARSE_VALUE_EXPECT,
    PARSE_VALUE_INVALID
};

int parse_value(value_object *object, const char *json);

value_type get_value_type(value_object *object);

#endif //CJSON_PARSE_VALUE_H
