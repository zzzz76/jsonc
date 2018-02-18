#include "parse_value.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * value解析器
 *
 * Created with CLion
 * User: zzzz76
 * Date: 2018-02-15
 */

#ifndef CJSON_PARSE_STACK_INIT_SIZE
#define CJOSN_PARSE_STACK_INIT_SIZE 256
#endif


typedef struct {
    const char *json;
    char *stack;
    size_t top;
    size_t len;
} value_context;

/**
 * 初始化值上下文
 *
 * @param json
 */
static void init_value_context(value_context *context, const char *json) {
    assert(context != NULL);
    context->json = json;
    context->top = 0;
    context->len = 0;
    context->stack = NULL;
}

/**
 * 初始化值对象
 *
 * @param object
 */
static void init_value_object(value_object *object) {
    assert(object != NULL);
    object->type = VALUE_NULL;
}

/**
 * 处理值上下文空位
 *
 * @param json
 */
static void whitespace_value_context(value_context *context) {
    const char *p = context->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        p++;
    }
    context->json = p;
}

/**
 * 逐字解析值
 *
 * @param object
 * @param json
 * @param literal
 * @param type
 * @return
 */
static int parse_value_literal(value_object *object, value_context *context, const char *literal, value_type type) {
    assert(*context->json == literal[0]);
    const char *p = context->json;
    size_t i;
    for (i = 0; literal[i] != '\0'; ++i) {
        if (p[i] != literal[i]) {
            return PARSE_VALUE_INVALID;
        }
    }
    p += i;
    context->json = p;
    object->type = type;
    return PARSE_VALUE_OK;
}

/**
 * 上下文入栈操作
 *
 * @param context
 */
static char *parse_value_context_push(value_context *context, size_t size) {
    char *ret;
    if (context->top + size >= context->len) {
        if (context->stack == NULL) {
            context->len = CJOSN_PARSE_STACK_INIT_SIZE;
        }
        while (context->top + size >= context->len) {
            context->len += context->len >> 1;
        }
        context->stack = (char *) realloc(context->stack, context->len);
    }
    ret = context->stack + context->top;
    context->top += size;
    return ret;
}

static char *parse_value_context_pop(value_context *context, size_t size) {
    context->top -= size;
    return context->stack + context->top;
}

static void parse_value_set_string(value_object *object, const char *stack, size_t len) {
    object->n.s.s = (char *) malloc(len + 1);
    memcpy(object->n.s.s, stack, len);
    object->n.s.s[len] = '\0';
    object->n.s.len = len;
    object->type = VALUE_STRING;
}

/**
 * 解析值字符串
 *
 * @param object
 * @param context
 * @return
 */
static int parse_value_string(value_object *object, value_context *context) {
    assert(*context->json == '"');
    context->json++;
    size_t head = context->top, len;
    const char *p = context->json;
    for (int i = 0; p[i] != '\0'; ++i) {
        if (*p == '"') {
            /* 复制字符串 */
            len = context->top - head;
            parse_value_set_string(object, parse_value_context_pop(context, len), len);
            return PARSE_VALUE_OK;
        }
        *parse_value_context_push(context, sizeof(char)) = *p;
    }
    context->top = head;
    return PARSE_VALUE_INVALID;

}

/**
 * 分渠道解析值
 *
 * @param object
 * @param context
 * @return
 */
static int parse_value_channel(value_object *object, value_context *context) {
    switch (*context->json) {
        case 'n':
            return parse_value_literal(object, context, "null", VALUE_NULL);
        case 'f':
            return parse_value_literal(object, context, "false", VALUE_FALSE);
        case 't':
            return parse_value_literal(object, context, "true", VALUE_TRUE);
        case '"':
            break;
        case '\0':
            return PARSE_VALUE_EXPECT;
        default:
            break;
    }
}

/**
 * 解析值
 *
 * @param object
 * @param json
 * @return
 */
int parse_value(value_object *object, const char *json) {
    value_context c;
    init_value_context(&c, json);
    init_value_object(object);
    whitespace_value_context(&c);
    int ret;
    /* 若解析结束 */
    if ((ret = parse_value_channel(object, &c)) == PARSE_VALUE_OK) {
        whitespace_value_context(&c);
        if (*c.json != '\0') {
            ret = PARSE_VALUE_INVALID;
        }
    }
    return ret;
}

/**
 * 获取值对象类型
 *
 * @param object
 * @return
 */
value_type get_value_type(value_object *object) {
    assert(object != NULL);
    return object->type;
}


