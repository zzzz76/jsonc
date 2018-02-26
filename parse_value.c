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

static int parse_value_channel(value_object *object, value_context *context);

void value_object_set_string(value_object *object, value_context *context, size_t head);

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
void init_value_object(value_object *object) {
    assert(object != NULL);
    object->type = VALUE_NULL;
    object->u.n = 0;
    object->u.s.s = NULL;
    object->u.s.len = 0;
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
 * 上下文入栈操作
 *
 * @param context
 * @param size
 * @return
 */
static void *parse_value_context_push(value_context *context, size_t size) {
    void *ret;
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

/**
 * 解析布尔类型
 *
 * @param object
 * @param json
 * @param literal
 * @param type
 * @return
 */
static int parse_value_literal(value_object *object, value_context *context, const char *literal, value_type type) {
    /* 解析过程 */
    assert(*context->json == literal[0]);
    context->json++;
    const char *p = context->json;
    size_t i;
    for (i = 0; literal[i + 1] != '\0'; ++i) {
        if (p[i] != literal[i + 1]) {
            return PARSE_VALUE_INVALID;
        }
    }
    p += i;
    context->json = p;
    object->type = type;
    return PARSE_VALUE_OK;
}

/**
 * 解析字符串
 *
 * @param object
 * @param context
 * @return
 */
static int parse_value_string(value_object *object, value_context *context) {
    assert(*context->json == '"');
    context->json++;
    size_t head = context->top;
    const char *p = context->json;
    for (int i = 0; p[i] != '\0'; ++i) {
        if (p[i] == '"') {
            /* 复制字符串 */
            value_object_set_string(object, context, head);
            context->top = head;
            context->json = p + i + 1;
            return PARSE_VALUE_OK;
        }
        *(char *) parse_value_context_push(context, sizeof(char)) = p[i];
    }
    context->top = head;
    return PARSE_VALUE_INVALID;

}

/**
 * 解析集合
 *
 * @param object
 * @param context
 * @return
 */
static int parse_value_array(value_object *object, value_context *context) {
    assert(*context->json == '[');
    context->json++;
    size_t head = context->top;
    size_t len;
    size_t size;
    whitespace_value_context(context);
    if (*context->json == ']') {
        object->type = VALUE_ARRAY;
        object->u.v.size = 0;
        object->u.v.object = NULL;
        return PARSE_VALUE_OK;
    }

    for (size = 0;; ++size) {
        value_object v;
        init_value_object(&v);
        whitespace_value_context(context);
        int ret;
        if ((ret = parse_value_channel(&v, context)) != PARSE_VALUE_OK) {
            /* 元素解析中出现问题 */
            return ret;
        }
        *(value_object *) parse_value_context_push(context, sizeof(value_object)) = v;
        if (*context->json == ',') {
            context->json++;
        } else if (*context->json == ']') {
            context->json++;
            break;
        } else {
            /* 元素出现冗余字符 */
            return PARSE_VALUE_INVALID;
        }
    }
    len = context->top - head;
    context->top = head;

    object->type = VALUE_ARRAY;
    object->u.v.size = size;
    object->u.v.object = (value_object *) malloc(len);
    memcpy(object->u.v.object, context->stack + context->top, len);
    return PARSE_VALUE_OK;
}

/**
 * 分渠道解析
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
            return parse_value_string(object, context);
        case '[':
            return parse_value_array(object, context);
        case '\0':
            return PARSE_VALUE_EXPECT;
        default:
            break;
    }
}

/**
 * 进行上下文封装
 *
 * @param object
 * @param json
 * @return
 */
int parse_value(value_object *object, const char *json) {
    value_context c;
    init_value_context(&c, json);
    whitespace_value_context(&c);
    int ret;
    if ((ret = parse_value_channel(object, &c)) == PARSE_VALUE_OK) {
        if (*c.json != '\0') {
            /* 元素出现冗余字符 */
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    free(c.stack);
    return ret;
}

void free_value(value_object *object) {
    if (object->type == VALUE_STRING) {
        free(object->u.s.s);
    }
    if (object->type == VALUE_ARRAY) {
        for (size_t i = 0; i < object->u.v.size; ++i) {
            free_value(object->u.v.object + i);
            free(object->u.v.object + i);
        }
    }
}

value_type value_object_get_type(value_object *object) {
    assert(object != NULL);
    return object->type;
}

void value_object_set_string(value_object *object, value_context *context, size_t head) {
    size_t len = context->top - head;
    object->u.s.s = (char *) malloc(len + 1);
    memcpy(object->u.s.s, context->stack + head, len);
    object->u.s.s[len] = '\0';
    object->u.s.len = len;
    object->type = VALUE_STRING;
}

const char *value_object_get_string(value_object *object) {
    assert(object != NULL && object->type == VALUE_STRING);
    return object->u.s.s;
}

size_t value_object_get_string_len(value_object *object) {
    assert(object != NULL && object->type == VALUE_STRING);
    return object->u.s.len;
}

value_object *value_object_get_array_element(value_object *object) {
    assert(object != NULL && object->type == VALUE_ARRAY);
    return object->u.v.object;
}


