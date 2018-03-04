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
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g");
#define EXPECT_EQ_STRING(expect, actual, alength) \
    EXPECT_EQ_BASE(sizeof(expect) - 1 == alength && memcmp(expect, actual, alength + 1) == 0, expect, actual, "%s")

#if defined(_MSC_VER)
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%Iu")
#else
#define EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%zu")
#endif
static void test_parse_value_null() {
    cjson_value value;
    init_value(&value);
    EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value, "null"));
    EXPECT_EQ_INT(VALUE_NULL, get_value_type(&value));
    free_value(&value);
}

static void test_parse_value_false() {
    cjson_value value;
    init_value(&value);
    EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value, "false"));
    EXPECT_EQ_INT(VALUE_FALSE, get_value_type(&value));
    free_value(&value);
}

static void test_parse_value_true() {
    cjson_value value;
    init_value(&value);
    EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value, "true"));
    EXPECT_EQ_INT(VALUE_TRUE, get_value_type(&value));
    free_value(&value);
}

#define TEST_NUMBER(expect, actual) \
    do {\
        cjson_value value;\
        init_value(&value);\
        EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value, actual));\
        EXPECT_EQ_INT(VALUE_NUMBER, get_value_type(&value));\
        EXPECT_EQ_DOUBLE(expect, get_value_number(&value));\
        free_value(&value);\
    } while (0);
static void test_parse_value_number() {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

#define TEST_STRING(expect, actual) \
    do {\
        cjson_value value;\
        init_value(&value);\
        EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value, actual));\
        EXPECT_EQ_INT(VALUE_STRING, get_value_type(&value));\
        EXPECT_EQ_STRING(expect, get_value_string(&value), get_value_string_len(&value));\
        free_value(&value);\
    } while(0);
static void test_parse_value_string() {
    TEST_STRING("Hello", "\"Hello\"");
    TEST_STRING("", "\"\"");
}


static void test_parse_value_array() {
    cjson_value value;
    init_value(&value);
    EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value, "[ ]"));
    EXPECT_EQ_INT(VALUE_ARRAY, get_value_type(&value));
    EXPECT_EQ_SIZE_T(0, get_value_array_size(&value));
    free_value(&value);

    init_value(&value);
    EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value, "[ null , false , true , \"abc\" ]"));
    EXPECT_EQ_INT(VALUE_ARRAY, get_value_type(&value));
    EXPECT_EQ_SIZE_T(4, get_value_array_size(&value));

    EXPECT_EQ_INT(VALUE_NULL, get_value_type(&get_value_array(&value)[0]));
    EXPECT_EQ_INT(VALUE_FALSE, get_value_type(&get_value_array(&value)[1]));
    EXPECT_EQ_INT(VALUE_TRUE, get_value_type(&get_value_array(&value)[2]));
    EXPECT_EQ_INT(VALUE_STRING, get_value_type(&get_value_array(&value)[3]));
    EXPECT_EQ_STRING("abc", get_value_string(&get_value_array(&value)[3]), get_value_string_len(&get_value_array(&value)[3]));
    free_value(&value);

    init_value(&value);
    EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value, "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
    EXPECT_EQ_INT(VALUE_ARRAY, get_value_type(&value));
    EXPECT_EQ_SIZE_T(4, get_value_array_size(&value));
    for (size_t i = 0; i < 4 ; ++i) {
        EXPECT_EQ_INT(VALUE_ARRAY, get_value_type(&get_value_array(&value)[i]));
        EXPECT_EQ_INT(VALUE_ARRAY, get_value_type(&get_value_array(&value)[i]));
        EXPECT_EQ_SIZE_T(i, get_value_array_size(&get_value_array(&value)[i]));
        for (size_t j = 0; j < i; ++j) {
            EXPECT_EQ_INT(VALUE_NUMBER, get_value_array(&get_value_array(&value)[i])[j].type);
            EXPECT_EQ_DOUBLE((double)j, get_value_array(&get_value_array(&value)[i])[j].u.n);
        }
    }
    free_value(&value);
}

static void test_parse_value_object() {
    cjson_value value;
    init_value(&value);
    EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value, " { } "));
    EXPECT_EQ_INT(VALUE_OBJECT, get_value_type(&value));
    EXPECT_EQ_SIZE_T(0, get_value_object_size(&value));
    free_value(&value);

    init_value(&value);
    EXPECT_EQ_INT(CJSON_PARSE_OK, cjson_parse(&value,
    " { "
            "\"n\" : null , "
            "\"f\" : false , "
            "\"t\" : true , "
            "\"i\" : 123 , "
            "\"s\" : \"abc\", "
            "\"a\" : [ 1, 2, 3 ],"
            "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
    " } "
    ));
    EXPECT_EQ_INT(VALUE_OBJECT, get_value_type(&value));
    EXPECT_EQ_SIZE_T(7, get_value_object_size(&value));
    EXPECT_EQ_STRING("n", get_member_key(&get_value_object(&value)[0]), get_member_key_len(&get_value_object(&value)[0]));
    EXPECT_EQ_INT(VALUE_NULL, get_value_type(get_member_value(&get_value_object(&value)[0])));
    EXPECT_EQ_STRING("f", get_member_key(&get_value_object(&value)[1]), get_member_key_len(&get_value_object(&value)[1]));
    EXPECT_EQ_INT(VALUE_FALSE, get_value_type(get_member_value(&get_value_object(&value)[1])));
    EXPECT_EQ_STRING("t", get_member_key(&get_value_object(&value)[2]), get_member_key_len(&get_value_object(&value)[2]));
    EXPECT_EQ_INT(VALUE_TRUE, get_value_type(get_member_value(&get_value_object(&value)[2])));

    EXPECT_EQ_STRING("i", get_member_key(&get_value_object(&value)[3]), get_member_key_len(&get_value_object(&value)[3]));
    EXPECT_EQ_INT(VALUE_NUMBER, get_value_type(get_member_value(&get_value_object(&value)[3])));
    EXPECT_EQ_DOUBLE(123.0, get_value_number(get_member_value(&get_value_object(&value)[3])));

    EXPECT_EQ_STRING("s", get_member_key(&get_value_object(&value)[4]), get_member_key_len(&get_value_object(&value)[4]));
    EXPECT_EQ_INT(VALUE_STRING, get_value_type(get_member_value(&get_value_object(&value)[4])));
    EXPECT_EQ_STRING("abc", get_value_string(get_member_value(&get_value_object(&value)[4])), get_value_string_len(get_member_value(&get_value_object(&value)[4])));

    EXPECT_EQ_STRING("a", get_member_key(&get_value_object(&value)[5]), get_member_key_len(&get_value_object(&value)[5]));
    EXPECT_EQ_INT(VALUE_ARRAY, get_value_type(get_member_value(&get_value_object(&value)[5])));
    EXPECT_EQ_SIZE_T(3, get_value_array_size(get_member_value(&get_value_object(&value)[5])));
    for (size_t i = 0; i< 3; i++) {
        EXPECT_EQ_INT(VALUE_NUMBER, get_value_type(&get_value_array(get_member_value(&get_value_object(&value)[5]))[i]));
        EXPECT_EQ_DOUBLE((double)i+1, get_value_number(&get_value_array(get_member_value(&get_value_object(&value)[5]))[i]));
    }

    EXPECT_EQ_STRING("o", get_member_key(&get_value_object(&value)[6]), get_member_key_len(&get_value_object(&value)[6]));
    EXPECT_EQ_INT(VALUE_OBJECT, get_value_type(get_member_value(&get_value_object(&value)[6])));
    cjson_value *v = get_member_value(&get_value_object(&value)[6]);
    EXPECT_EQ_SIZE_T(3, get_value_object_size(v));
    EXPECT_EQ_STRING("1", get_member_key(&get_value_object(v)[0]), get_member_key_len(&get_value_object(v)[0]));
    EXPECT_EQ_INT(VALUE_NUMBER, get_value_type(get_member_value(&get_value_object(v)[0])));
    EXPECT_EQ_DOUBLE(1.0, get_value_number(get_member_value(&get_value_object(v)[0])));
    EXPECT_EQ_STRING("2", get_member_key(&get_value_object(v)[1]), get_member_key_len(&get_value_object(v)[1]));
    EXPECT_EQ_INT(VALUE_NUMBER, get_value_type(get_member_value(&get_value_object(v)[1])));
    EXPECT_EQ_DOUBLE(2.0, get_value_number(get_member_value(&get_value_object(v)[1])));
    EXPECT_EQ_STRING("3", get_member_key(&get_value_object(v)[2]), get_member_key_len(&get_value_object(v)[2]));
    EXPECT_EQ_INT(VALUE_NUMBER, get_value_type(get_member_value(&get_value_object(v)[2])));
    EXPECT_EQ_DOUBLE(3.0, get_value_number(get_member_value(&get_value_object(v)[2])));
    free_value(&value);
}

#define TEST_ERROR(error, json) \
    do {\
        cjson_value value;\
        init_value(&value);\
        EXPECT_EQ_INT(error, cjson_parse(&value, json));\
        EXPECT_EQ_INT(VALUE_NULL, get_value_type(&value));\
    }while(0);

static void test_parse_expect() {
    TEST_ERROR(CJSON_PARSE_EXPECT, "");
    TEST_ERROR(CJSON_PARSE_EXPECT, " ");
}

static void test_parse_invalid_value() {
    TEST_ERROR(CJSON_PARSE_INVALID, "nul");
    TEST_ERROR(CJSON_PARSE_INVALID, "?");

    /* invalid number */
    TEST_ERROR(CJSON_PARSE_INVALID, "+0");
    TEST_ERROR(CJSON_PARSE_INVALID, "+1");
    TEST_ERROR(CJSON_PARSE_INVALID, ".123"); /* at least one digit before '.' */
    TEST_ERROR(CJSON_PARSE_INVALID, "1.");   /* at least one digit after '.' */
    TEST_ERROR(CJSON_PARSE_INVALID, "INF");
    TEST_ERROR(CJSON_PARSE_INVALID, "inf");
    TEST_ERROR(CJSON_PARSE_INVALID, "NAN");
    TEST_ERROR(CJSON_PARSE_INVALID, "nan");

    /* invalid value in array */
    TEST_ERROR(CJSON_PARSE_INVALID, "[1,]");
    TEST_ERROR(CJSON_PARSE_INVALID, "[\"a\", nul]");
}

static void test_parse_root_not_singular() {
    TEST_ERROR(CJSON_PARSE_ROOT_NOT_SINGULAR, "null x");

    /* invalid number */
    TEST_ERROR(CJSON_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
    TEST_ERROR(CJSON_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(CJSON_PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse_number_too_big() {
    TEST_ERROR(CJSON_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(CJSON_PARSE_NUMBER_TOO_BIG, "-1e309");
}

static void test_parse_miss_quotation_mark() {
    TEST_ERROR(CJSON_PARSE_MISS_QUOTATION_MARK, "\"");
    TEST_ERROR(CJSON_PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_miss_comma_or_square_bracket() {
    TEST_ERROR(CJSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1");
    TEST_ERROR(CJSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
    TEST_ERROR(CJSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
    TEST_ERROR(CJSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[[]");
}

static void test_parse_miss_colon() {
    TEST_ERROR(CJSON_PARSE_MISS_COLON, "{\"a\"}");
    TEST_ERROR(CJSON_PARSE_MISS_COLON, "{\"a\",\"b\"}");
}

static void test_parse_miss_comma_or_curly_bracket() {
    TEST_ERROR(CJSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1");
    TEST_ERROR(CJSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1]");
    TEST_ERROR(CJSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1 \"b\"");
    TEST_ERROR(CJSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{}");
}

static void test_base() {
    test_parse_value_null();
    test_parse_value_false();
    test_parse_value_true();
    test_parse_value_number();
    test_parse_value_string();
    test_parse_value_array();
    test_parse_value_object();

    test_parse_expect();
    test_parse_invalid_value();
    test_parse_root_not_singular();
    test_parse_number_too_big();
    test_parse_miss_quotation_mark();
    test_parse_miss_comma_or_square_bracket();
    test_parse_miss_colon();
    test_parse_miss_comma_or_curly_bracket();
}

int main() {
    test_base();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
