#include "parse_value.h"
#include <stdio.h>

/**
 * Created with CLion
 * User: zzzz76
 * Date: 2018-02-17
 */

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(expect, actual, format) \
    do {\
        test_count++;\
        if ((expect) == (actual)) {\
            test_pass++;\
        } else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0);

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE(expect, actual, "%d");

static void test_parse_value_null() {
    value_object v;
    v.type = VALUE_NULL;
    EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "null"));
    EXPECT_EQ_INT(VALUE_NULL, get_value_type(&v));
}

static void test_parse_value_false() {
    value_object v;
    v.type = VALUE_NULL;
    EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "false"));
    EXPECT_EQ_INT(VALUE_FALSE, get_value_type(&v))
}

static void test_parse_value_true() {
    value_object v;
    v.type = VALUE_NULL;
    EXPECT_EQ_INT(PARSE_VALUE_OK, parse_value(&v, "true"));
    EXPECT_EQ_INT(VALUE_TRUE, get_value_type(&v))
}

/**
 * 缺少对异常结果的测试
 * 缺少另外两个渠道的测试
 *
 * 先解决string渠道的解析
 */
/*#define TEST_BASE_SUB(expect, a, b) \
    EXPECT_EQ_INT(expect, base_sub(a, b));\
    EXPECT_EQ_INT(expect, base_sub_re(a, b));\
    EXPECT_EQ_INT(expect, base_sub_re_re(a, b));
static void test_base_sub() {
    TEST_BASE_SUB(0, 1, 1);
    TEST_BASE_SUB(-2, -1, 1);
    TEST_BASE_SUB(2, 1, -1);
    TEST_BASE_SUB(0, -1, -1);
    TEST_BASE_SUB(2147483647, -2147483648, 1);
}

#define TEST_BASE_MUL(expect, a, b) \
    EXPECT_EQ_INT(expect, base_mul(a, b));\
    EXPECT_EQ_INT(expect, base_mul_re(a, b));
static void test_base_mul() {
    TEST_BASE_MUL(9, 3, 3);
    TEST_BASE_MUL(-9, -3, 3);
    TEST_BASE_MUL(-9, 3, -3);
    TEST_BASE_MUL(9, -3, -3);
    TEST_BASE_MUL(0, -2147483648, 2);
    TEST_BASE_MUL(-2, 2147483647, 2);
}

#define TEST_BASE_DIV(expect, a, b) \
    EXPECT_EQ_INT(expect, base_div(a, b));\
    EXPECT_EQ_INT(expect, base_div_re(a, b));
static void test_base_div() {
    TEST_BASE_DIV(2, 2, 1);
    TEST_BASE_DIV(-2, -2, 1);
    TEST_BASE_DIV(0, 2, -1);
    TEST_BASE_DIV(0, -2, -1);
    TEST_BASE_DIV(0, 1, 2);
    TEST_BASE_DIV(0, 1, -2);
    TEST_BASE_DIV(2147483647, -1, 2);
    TEST_BASE_DIV(1, -1, -2);
}*/

static void test_base() {
    test_parse_value_null();
    test_parse_value_false();
    test_parse_value_true();
}

int main() {
    test_base();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
