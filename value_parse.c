#include <assert.h>
#include <stdio.h>

/**
 * value解析器
 *
 * Created with CLion
 * User: zzzz76
 * Date: 2018-02-15
 */

typedef enum {
    VALUE_NULL, VALUE_FALSE, VALUE_TRUE
} value_type;

typedef struct {
    value_type type;
} value_object;

typedef struct {
    char *json;
} context;
/**
 * 初始化
 *
 * @param object
 */
static void init_value_object(value_object* object) {
    assert(object != NULL);
    object->type = VALUE_NULL;
}

/**
 * 处理空位
 *
 * @param json
 */
static void whitespace_value_json(const char *json) {
    const char *p = json;
    while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        p++;
    }
    json = p;
}

static int parse_value_null() {

}

static int parse_value_false() {

}

static int parse_value_true() {

}

static int parse_value_by_json(const char *json) {
    switch (*json) {
        case 'n':

            break;
        case 'f':
            break;
        case 't':
            break;
        case '"':
            break;
        case '\0':
            break;
        default:
            break;
    }
}
/**
 * 解析value字符串
 * 因为需要解耦，所以需要传进来容器和字符串来源
 *
 * @param object
 * @param json
 * @return
 */
int parse_value(value_object *object, const char *json) {
    init_value_object(object);
    whitespace_value_json(json);
    /* 若解析结束 */
    whitespace_value_json(json);
    if (json != '\0') {
        return 1;
    }
    return 0;
}



