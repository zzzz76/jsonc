#include "cjson.h"
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
} cjson_context;

static int parse_value(cjson_value *v, cjson_context *c);

/**
 * 初始化缓冲区
 *
 * @param json
 */
static void init_context(cjson_context *c, const char *json) {
    assert(c != NULL);
    c->json = json;
    c->top = 0;
    c->len = 0;
    c->stack = NULL;
}

/**
 * 初始化值
 *
 * @param v
 */
void init_value(cjson_value *v) {
    assert(v != NULL);
    v->type = VALUE_NULL;
    v->u.n = 0;
    v->u.s.s = NULL;
    v->u.s.len = 0;
}

/**
 * 处理上下文空位
 *
 * @param c
 */
static void whitespace_context(cjson_context *c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        p++;
    }
    c->json = p;
}

/**
 * 缓冲区入栈操作
 *
 * @param c
 * @param size
 * @return
 */
static void *push_context(cjson_context *c, size_t size) {
    void *ret;
    if (c->top + size >= c->len) {
        if (c->stack == NULL) {
            c->len = CJOSN_PARSE_STACK_INIT_SIZE;
        }
        while (c->top + size >= c->len) {
            c->len += c->len >> 1;
        }
        c->stack = (char *) realloc(c->stack, c->len);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

/**
 * 解析布尔类型
 *
 * @param v
 * @param c
 * @param literal
 * @param type
 * @return
 */
static int parse_value_literal(cjson_value *v, cjson_context *c, const char *literal, value_type type) {
    /* 解析过程 */
    assert(*c->json == literal[0]);
    c->json++;
    const char *p = c->json;
    size_t i;
    for (i = 0; literal[i + 1] != '\0'; ++i) {
        if (p[i] != literal[i + 1]) {
            return PARSE_VALUE_INVALID;
        }
    }
    p += i;
    c->json = p;
    v->type = type;
    return PARSE_VALUE_OK;
}

/**
 * 解析字符串
 *
 * @param v
 * @param c
 * @return
 */
static int parse_value_string(cjson_value *v, cjson_context *c) {
    assert(*c->json == '"');
    c->json++;
    size_t head = c->top;
    size_t len;
    for (int i = 0; c->json[i] != '\0'; ++i) {
        if (c->json[i] == '"') {
            /* 复制字符串 */
            c->json += i + 1;
            len = c->top -head;
            c->top = head;

            v->u.s.s = (char *) malloc(len + 1);
            memcpy(v->u.s.s, c->stack+c->top, len);
            v->u.s.s[len] = '\0';
            v->u.s.len = len;
            v->type = VALUE_STRING;
            return PARSE_VALUE_OK;
        }
        *(char *) push_context(c, sizeof(char)) = c->json[i];
    }
    c->top = head;
    return PARSE_VALUE_INVALID;

}

/**
 * 解析集合
 *
 * @param v
 * @param c
 * @return
 */
static int parse_value_array(cjson_value *v, cjson_context *c) {
    assert(*c->json == '[');
    c->json++;
    size_t head = c->top;
    size_t len;
    size_t size = 0;
    int ret;
    whitespace_context(c);
    if (*c->json == ']') {
        c->json++;
        v->type = VALUE_ARRAY;
        v->u.a.size = 0;
        v->u.a.array = NULL;
        return PARSE_VALUE_OK;
    }
    for (;;) {
        cjson_value value;
        init_value(&value);
        whitespace_context(c);
        if ((ret = parse_value(&value, c)) != PARSE_VALUE_OK) {
            /* 元素解析中出现问题 */
            break;
        }
        *(cjson_value *) push_context(c, sizeof(cjson_value)) = value;
        size++;
        if (*c->json == ',') {
            c->json++;
        } else if (*c->json == ']') {
            c->json++;
            len = c->top - head;
            c->top = head;

            v->type = VALUE_ARRAY;
            v->u.a.size = size;
            v->u.a.array = (cjson_value *) malloc(len);
            memcpy(v->u.a.array, c->stack + c->top, len);
            return PARSE_VALUE_OK;
        } else {
            /* 元素出现冗余字符 */
            ret = PARSE_VALUE_INVALID;
            break;
        }
    }
    for (int i = 0; i < size; ++i) {
        c->top -= sizeof(cjson_value);
        free_value((cjson_value *) (c->stack + c->top));
    }
    return ret;

}

/**
 * 分渠道解析
 *
 * @param v
 * @param c
 * @return
 */
static int parse_value(cjson_value *v, cjson_context *c) {
    switch (*c->json) {
        case 'n':
            return parse_value_literal(v, c, "null", VALUE_NULL);
        case 'f':
            return parse_value_literal(v, c, "false", VALUE_FALSE);
        case 't':
            return parse_value_literal(v, c, "true", VALUE_TRUE);
        case '"':
            return parse_value_string(v, c);
        case '[':
            return parse_value_array(v, c);
        case '\0':
            return PARSE_VALUE_EXPECT;
        default:
            break;
    }
}

/**
 * json解析
 *
 * @param v
 * @param json
 * @return
 */
int cjson_parse(cjson_value *v, const char *json) {
    cjson_context context;
    init_context(&context, json);
    whitespace_context(&context);
    int ret;
    if ((ret = parse_value(v, &context)) == PARSE_VALUE_OK) {
        if (*context.json != '\0') {
            /* 元素出现冗余字符 */
            v->type = VALUE_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    free(context.stack);
    return ret;
}

void free_value(cjson_value *v) {
    if (v->type == VALUE_STRING) {
        free(v->u.s.s);
    }
    if (v->type == VALUE_ARRAY) {
        for (size_t i = 0; i < v->u.a.size; ++i) {
            free_value(v->u.a.array+i);
        }
        free(v->u.a.array);
    }
}

value_type get_value_type(cjson_value *v) {
    assert(v != NULL);
    return v->type;
}

const char *get_value_string(cjson_value *object) {
    assert(object != NULL && object->type == VALUE_STRING);
    return object->u.s.s;
}

size_t get_value_string_len(cjson_value *object) {
    assert(object != NULL && object->type == VALUE_STRING);
    return object->u.s.len;
}

cjson_value *get_value_array(cjson_value *object) {
    assert(object != NULL && object->type == VALUE_ARRAY);
    return object->u.a.array;
}


