#include "cjson.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

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
}

/**
 * 初始化对象成员
 *
 * @param m
 */
static void init_member(cjson_member *m) {
    assert(m != NULL);
    m->len = 0;
    m->key = NULL;
    init_value(&m->value);
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
            return CJSON_PARSE_INVALID;
        }
    }
    p += i;
    c->json = p;
    v->type = type;
    return CJSON_PARSE_OK;
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
            len = c->top - head;
            c->top = head;

            v->u.s.s = (char *) malloc(len + 1);
            memcpy(v->u.s.s, c->stack + c->top, len);
            v->u.s.s[len] = '\0';
            v->u.s.len = len;
            v->type = VALUE_STRING;
            return CJSON_PARSE_OK;
        }
        *(char *) push_context(c, sizeof(char)) = c->json[i];
    }
    c->top = head;
    return CJSON_PARSE_MISS_QUOTATION_MARK;

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
        return CJSON_PARSE_OK;
    }
    for (;;) {
        cjson_value value;
        init_value(&value);
        whitespace_context(c);
        if ((ret = parse_value(&value, c)) != CJSON_PARSE_OK) {
            /* 元素解析中出现问题 */
            break;
        }
        *(cjson_value *) push_context(c, sizeof(cjson_value)) = value;
        size++;
        whitespace_context(c);
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
            return CJSON_PARSE_OK;
        } else {
            /* 元素出现冗余字符 */
            ret = CJSON_PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
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
 * 解析键字符串
 *
 * @param m
 * @param c
 * @return
 */
static int parse_key_string(cjson_member *m, cjson_context *c) {
    assert(*c->json == '"');
    c->json++;
    size_t head = c->top;
    size_t len;
    for (int i = 0; c->json[i] != '\0'; ++i) {
        if (c->json[i] == '"') {
            c->json += i + 1;
            len = c->top - head;
            c->top = head;

            m->key = (char *) malloc(len + 1);
            memcpy(m->key, c->stack + c->top, len);
            m->key[len] = '\0';
            m->len = len;
            return CJSON_PARSE_OK;
        }
        *(char *) push_context(c, sizeof(char)) = c->json[i];
    }
    c->top = head;
    return CJSON_PARSE_MISS_QUOTATION_MARK;
}

/**
 * 解析对象
 *
 * @param v
 * @param c
 * @return
 */
static int parse_value_object(cjson_value *v, cjson_context *c) {
    assert(*c->json == '{');
    c->json++;
    int ret;
    size_t head = c->top;
    size_t len;
    size_t size = 0;
    whitespace_context(c);
    if (*c->json == '}') {
        c->json++;
        v->type = VALUE_OBJECT;
        v->u.o.size = 0;
        v->u.o.object = NULL;
        return CJSON_PARSE_OK;
    }
    for (;;) {
        cjson_member member;
        init_member(&member);
        whitespace_context(c);
        if ((ret = parse_key_string(&member, c)) != CJSON_PARSE_OK) {
            /* 元素解析发生错误 */
            break;
        }
        whitespace_context(c);
        if (*c->json != ':') {
            /* 元素解析发生错误 */
            ret = CJSON_PARSE_MISS_COLON;
            free(member.key);
            break;
        }
        c->json++;
        whitespace_context(c);
        if ((ret = parse_value(&member.value, c)) != CJSON_PARSE_OK) {
            /* 元素解析错误 */
            free(member.key);
            break;
        }
        *(cjson_member *) push_context(c, sizeof(cjson_member)) = member;
        size++;
        whitespace_context(c);
        if (*c->json == ',') {
            c->json++;
        } else if (*c->json == '}') {
            c->json++;
            len = c->top - head;
            c->top = head;

            v->type = VALUE_OBJECT;
            v->u.o.size = size;
            v->u.o.object = (cjson_member *) malloc(len);
            memcpy(v->u.o.object, c->stack + c->top, len);
            return CJSON_PARSE_OK;
        } else {
            /* 元素冗余 */
            ret = CJSON_PARSE_MISS_COMMA_OR_CURLY_BRACKET;
            break;
        }
    }
    for (int i = 0; i < size; ++i) {
        c->top -= sizeof(cjson_member);
        free_member((cjson_member *) (c->stack + c->top));
    }
    return ret;

}

/**
 * 解析数字
 *
 * @param v
 * @param c
 * @return
 */
static int parse_value_number(cjson_value *v, cjson_context *c) {
    const char *p = c->json;
    if (*p == '-') {
        p++;
    }

    if (*p == '0') {
        p++;
    } else if (*p <= '9' && *p >= '1') {
        p++;
        while (*p <= '9' && *p >= '0') {
            p++;
        }
    } else {
        return CJSON_PARSE_INVALID;
    }

    if (*p == '.') {
        p++;
        if (*p <= '9' && *p >= '0') {
            p++;
            while (*p <= '9' && *p >= '0') {
                p++;
            }
        } else {
            return CJSON_PARSE_INVALID;
        }
    }

    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') {
            p++;
        }

        if (*p <= '9' && *p >= '0') {
            p++;
            while (*p <= '9' && *p >= '0') {
                p++;
            }
        } else {
            return CJSON_PARSE_INVALID;
        }
    }
    errno = 0;
    v->u.n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
        return CJSON_PARSE_NUMBER_TOO_BIG;
    v->type = VALUE_NUMBER;
    c->json = p;
    return CJSON_PARSE_OK;
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
        case '{':
            return parse_value_object(v, c);
        case '\0':
            return CJSON_PARSE_EXPECT;
        default:
            return parse_value_number(v, c);
    }
}

/**
 * cjson解析
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
    if ((ret = parse_value(v, &context)) == CJSON_PARSE_OK) {
        whitespace_context(&context);
        if (*context.json != '\0') {
            /* 元素出现冗余字符 */
            free_value(v);
            init_value(v);
            ret = CJSON_PARSE_ROOT_NOT_SINGULAR;
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
            free_value(v->u.a.array + i);
        }
        free(v->u.a.array);
    }
    if (v->type == VALUE_OBJECT) {
        for (size_t i = 0; i < v->u.o.size; ++i) {
            free_member(v->u.o.object + i);
        }
        free(v->u.o.object);
    }
}

void free_member(cjson_member *m) {
    free_value(&m->value);
    free(m->key);
}

value_type get_value_type(cjson_value *v) {
    assert(v != NULL);
    return v->type;
}

double get_value_number(cjson_value *v) {
    assert(v != NULL && v->type == VALUE_NUMBER);
    return v->u.n;
}

const char *get_value_string(cjson_value *v) {
    assert(v != NULL && v->type == VALUE_STRING);
    return v->u.s.s;
}

size_t get_value_string_len(cjson_value *v) {
    assert(v != NULL && v->type == VALUE_STRING);
    return v->u.s.len;
}

cjson_value *get_value_array(cjson_value *v) {
    assert(v != NULL && v->type == VALUE_ARRAY);
    return v->u.a.array;
}

size_t get_value_array_size(cjson_value *v) {
    assert(v != NULL && v->type == VALUE_ARRAY);
    return v->u.a.size;
}

cjson_member *get_value_object(cjson_value *v) {
    assert(v != NULL && v->type == VALUE_OBJECT);
    return v->u.o.object;
}

size_t get_value_object_size(cjson_value *v) {
    assert(v != NULL && v->type == VALUE_OBJECT);
    return v->u.o.size;
}

char *get_member_key(cjson_member *m) {
    assert(m != NULL);
    return m->key;
}

size_t get_member_key_len(cjson_member *m) {
    assert(m != NULL);
    return m->len;
}

cjson_value *get_member_value(cjson_member *m) {
    assert(m != NULL);
    return &m->value;
}

