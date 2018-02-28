#include "cjson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Created with CLion
 * User: zzzz76
 * Date: 2018-02-17
 */

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality) {\
            test_pass++;\
        } else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0);

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect)==(actual), expect, actual, "%d");
#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength) == 0, expect, actual, "%s");

static void test_parse_value_null() {
    cjson_value value;
    init_value(&value);
    EXPECT_EQ_INT(PARSE_VALUE_OK, cjson_parse(&value, "null"));
    EXPECT_EQ_INT(VALUE_NULL, get_value_type(&value));
}

static void test_parse_value_false() {
    cjson_value value;
    init_value(&value);
    EXPECT_EQ_INT(PARSE_VALUE_OK, cjson_parse(&value, "false"));
    EXPECT_EQ_INT(VALUE_FALSE, get_value_type(&value));
}

static void test_parse_value_true() {
    cjson_value value;
    init_value(&value);
    EXPECT_EQ_INT(PARSE_VALUE_OK, cjson_parse(&value, "true"));
    EXPECT_EQ_INT(VALUE_TRUE, get_value_type(&value));
}

static void test_parse_value_string() {
    do {
        cjson_value value;
        init_value(&value);
        EXPECT_EQ_INT(PARSE_VALUE_OK, cjson_parse(&value, "\"Hello\""));
        EXPECT_EQ_INT(VALUE_STRING, get_value_type(&value));
        EXPECT_EQ_STRING("Hello", get_value_string(&value), get_value_string_len(&value));
        free_value(&value);
    } while(0);
    do {
        cjson_value value;
        init_value(&value);
        EXPECT_EQ_INT(PARSE_VALUE_OK, cjson_parse(&value, "\"\""));
        EXPECT_EQ_INT(VALUE_STRING, get_value_type(&value));
        EXPECT_EQ_STRING("", get_value_string(&value), get_value_string_len(&value));
        free_value(&value);
    } while(0);

}

static void test_parse_value_array() {
    do {
        cjson_value value;
        init_value(&value);
        EXPECT_EQ_INT(PARSE_VALUE_OK, cjson_parse(&value, "[false,[false,true]]"));
        EXPECT_EQ_INT(VALUE_ARRAY, get_value_type(&value));
        EXPECT_EQ_INT(VALUE_FALSE, get_value_array(&value)[0].type);
        EXPECT_EQ_INT(VALUE_ARRAY, get_value_array(&value)[1].type);
        EXPECT_EQ_INT(VALUE_FALSE, get_value_array(&value)[1].u.a.array[0].type);
        EXPECT_EQ_INT(VALUE_TRUE, get_value_array(&value)[1].u.a.array[1].type);
        free_value(&value);
    } while(0);
    do {
        cjson_value value;
        init_value(&value);
        EXPECT_EQ_INT(PARSE_VALUE_OK, cjson_parse(&value, "[[[]]]"));
        EXPECT_EQ_INT(VALUE_ARRAY, get_value_type(&value));
        EXPECT_EQ_INT(VALUE_ARRAY, get_value_array(&value)[0].type);
        EXPECT_EQ_INT(VALUE_ARRAY, get_value_array(&value)[0].u.a.array[0].type);
        free_value(&value);
    } while(0);
}

static void test_parse_value_object() {
    do {
        cjson_value value;
        init_value(&value);
        EXPECT_EQ_INT(PARSE_VALUE_OK, cjson_parse(&value, "{\"apple\":true,\"fruit\":{\"banna\":true}}"));
        free_value(&value);
    } while(0);
}

static void test_base() {
    test_parse_value_null();
    test_parse_value_false();
    test_parse_value_true();
    test_parse_value_string();
    test_parse_value_array();
    test_parse_value_object();
}

int main() {
    test_base();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
