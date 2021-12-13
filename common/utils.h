#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysrepo.h>

#define SR_TRY(x) \
    do {\
        int rc__; \
        rc__ = (x); \
        if (rc__ != SR_ERR_OK) \
            panic("PANIC: %s returned error %d\n", #x, rc__); \
    } while(0)

static __attribute__((noreturn))
void panic(const char* msg, ...)
{
    va_list va;
    va_start(va, msg);

    vfprintf(stderr, msg, va);

    va_end(va);

    exit(-1);
}

static int sprint_val(char* buf, int buf_len, const sr_val_t* value, int print_path, int print_values_only)
{
    const char* orig_buf = buf;
    if (NULL == value)
        goto finished;

    int len = 0;
    if (print_path) {
        len = snprintf(buf, buf_len, "%s ", value->xpath);
        buf_len -= len;
        buf += len;
    }
    
    
#define EQ_VAL(x) (print_values_only ? (x) : ("= " x))
    switch (value->type) {
    case SR_CONTAINER_T:
    case SR_CONTAINER_PRESENCE_T:
        len = snprintf(buf, buf_len, "(container)");
        break;
    case SR_LIST_T:
        len = snprintf(buf, buf_len, "(list instance)");
        break;
    case SR_STRING_T:
        len = snprintf(buf, buf_len, EQ_VAL("%s"), value->data.string_val);
        break;
    case SR_BOOL_T:
        len = snprintf(buf, buf_len, EQ_VAL("%s"), value->data.bool_val ? "true" : "false");
        break;
    case SR_DECIMAL64_T:
        len = snprintf(buf, buf_len, EQ_VAL("%g"), value->data.decimal64_val);
        break;
    case SR_INT8_T:
        len = snprintf(buf, buf_len, EQ_VAL("%" PRId8), value->data.int8_val);
        break;
    case SR_INT16_T:
        len = snprintf(buf, buf_len, EQ_VAL("%" PRId16), value->data.int16_val);
        break;
    case SR_INT32_T:
        len = snprintf(buf, buf_len, EQ_VAL("%" PRId32), value->data.int32_val);
        break;
    case SR_INT64_T:
        len = snprintf(buf, buf_len, EQ_VAL("%" PRId64), value->data.int64_val);
        break;
    case SR_UINT8_T:
        len = snprintf(buf, buf_len, EQ_VAL("%" PRIu8), value->data.uint8_val);
        break;
    case SR_UINT16_T:
        len = snprintf(buf, buf_len, EQ_VAL("%" PRIu16), value->data.uint16_val);
        break;
    case SR_UINT32_T:
        len = snprintf(buf, buf_len, EQ_VAL("%" PRIu32), value->data.uint32_val);
        break;
    case SR_UINT64_T:
        len = snprintf(buf, buf_len, EQ_VAL("%" PRIu64), value->data.uint64_val);
        break;
    case SR_IDENTITYREF_T:
        len = snprintf(buf, buf_len, EQ_VAL("%s"), value->data.identityref_val);
        break;
    case SR_INSTANCEID_T:
        len = snprintf(buf, buf_len, EQ_VAL("%s"), value->data.instanceid_val);
        break;
    case SR_BITS_T:
        len = snprintf(buf, buf_len, EQ_VAL("%s"), value->data.bits_val);
        break;
    case SR_BINARY_T:
        len = snprintf(buf, buf_len, EQ_VAL("%s"), value->data.binary_val);
        break;
    case SR_ENUM_T:
        len = snprintf(buf, buf_len, EQ_VAL("%s"), value->data.enum_val);
        break;
    case SR_LEAF_EMPTY_T:
        len = snprintf(buf, buf_len, "(empty leaf)");
        break;
    default:
        len = snprintf(buf, buf_len, "(unprintable)");
        break;
    }

    buf += len;
    buf_len -= len;

    if (!print_values_only) {
        switch (value->type) {
        case SR_UNKNOWN_T:
        case SR_CONTAINER_T:
        case SR_CONTAINER_PRESENCE_T:
        case SR_LIST_T:
        case SR_LEAF_EMPTY_T:
            break;
        default:
            len = snprintf(buf, buf_len, "%s", value->dflt ? " [default]" : "");
            break;
        }

        buf += len;
        buf_len -= len;
    }

finished:    
    return buf - orig_buf;
}

static void print_val(const sr_val_t *value, int print_path, int print_values_only)
{
    char buf[100] = { 0 };

    sprint_val(buf, sizeof(buf), value, print_path, print_values_only);
    printf("%s", buf);
}

#endif /* __UTILS_H__ */