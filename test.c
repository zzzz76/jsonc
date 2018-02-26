#include "parse_value.h"
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
    value_object v;
    v.type = VALUE_NULL;
    EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "null,"));
    EXPECT_EQ_INT(VALUE_NULL, value_object_get_type(&v));
}

static void test_parse_value_false() {
    value_object v;
    v.type = VALUE_NULL;
    EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "false,"));
    EXPECT_EQ_INT(VALUE_FALSE, value_object_get_type(&v))
}

static void test_parse_value_true() {
    value_object v;
    v.type = VALUE_NULL;
    EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "true,"));
    EXPECT_EQ_INT(VALUE_TRUE, value_object_get_type(&v))
}

static void test_parse_value_string() {
    value_object v;
    v.type = VALUE_NULL;
    EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "\"Hello\","));
    EXPECT_EQ_INT(VALUE_STRING, value_object_get_type(&v));
    EXPECT_EQ_STRING("Hello", value_object_get_string(&v), value_object_get_string_len(&v));
    free(v.u.s.s);
    EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "\"\","));
    EXPECT_EQ_INT(VALUE_STRING, value_object_get_type(&v));
    EXPECT_EQ_STRING("", value_object_get_string(&v), value_object_get_string_len(&v));
    free(v.u.s.s);
}

static void test_parse_value_array() {
    do {
        value_object v;
        init_value_object(&v);
        EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "[false,[false,true]]"));
        EXPECT_EQ_INT(VALUE_ARRAY, value_object_get_type(&v));
        EXPECT_EQ_INT(VALUE_FALSE, value_object_get_array_element(&v)[0].type);
        EXPECT_EQ_INT(VALUE_ARRAY, value_object_get_array_element(&v)[1].type);
        EXPECT_EQ_INT(VALUE_FALSE, value_object_get_array_element(&v)[1].u.v.object[0].type);
        EXPECT_EQ_INT(VALUE_TRUE, value_object_get_array_element(&v)[1].u.v.object[1].type);
        free_value(&v);
    } while(0);

    do {
        value_object v;
        init_value_object(&v);
        EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "[[[]],[[]]  ]"));
        EXPECT_EQ_INT(VALUE_ARRAY, value_object_get_type(&v));
        EXPECT_EQ_INT(VALUE_ARRAY, value_object_get_array_element(&v)[0].type);
        EXPECT_EQ_INT(VALUE_ARRAY, value_object_get_array_element(&v)[0].u.v.object[0].type);
        free_value(&v);
    } while(0);

}

static void test_base() {
    /*test_parse_value_null();
    test_parse_value_false();
    test_parse_value_true();
    test_parse_value_string();*/
    test_parse_value_array();
    
}

int main() {
    test_base();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
