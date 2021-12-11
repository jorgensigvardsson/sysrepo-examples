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


static void print_val(const sr_val_t *value)
{
    if (NULL == value) {
        return;
    }

    printf("%s ", value->xpath);

    switch (value->type) {
    case SR_CONTAINER_T:
    case SR_CONTAINER_PRESENCE_T:
        printf("(container)");
        break;
    case SR_LIST_T:
        printf("(list instance)");
        break;
    case SR_STRING_T:
        printf("= %s", value->data.string_val);
        break;
    case SR_BOOL_T:
        printf("= %s", value->data.bool_val ? "true" : "false");
        break;
    case SR_DECIMAL64_T:
        printf("= %g", value->data.decimal64_val);
        break;
    case SR_INT8_T:
        printf("= %" PRId8, value->data.int8_val);
        break;
    case SR_INT16_T:
        printf("= %" PRId16, value->data.int16_val);
        break;
    case SR_INT32_T:
        printf("= %" PRId32, value->data.int32_val);
        break;
    case SR_INT64_T:
        printf("= %" PRId64, value->data.int64_val);
        break;
    case SR_UINT8_T:
        printf("= %" PRIu8, value->data.uint8_val);
        break;
    case SR_UINT16_T:
        printf("= %" PRIu16, value->data.uint16_val);
        break;
    case SR_UINT32_T:
        printf("= %" PRIu32, value->data.uint32_val);
        break;
    case SR_UINT64_T:
        printf("= %" PRIu64, value->data.uint64_val);
        break;
    case SR_IDENTITYREF_T:
        printf("= %s", value->data.identityref_val);
        break;
    case SR_INSTANCEID_T:
        printf("= %s", value->data.instanceid_val);
        break;
    case SR_BITS_T:
        printf("= %s", value->data.bits_val);
        break;
    case SR_BINARY_T:
        printf("= %s", value->data.binary_val);
        break;
    case SR_ENUM_T:
        printf("= %s", value->data.enum_val);
        break;
    case SR_LEAF_EMPTY_T:
        printf("(empty leaf)");
        break;
    default:
        printf("(unprintable)");
        break;
    }

    switch (value->type) {
    case SR_UNKNOWN_T:
    case SR_CONTAINER_T:
    case SR_CONTAINER_PRESENCE_T:
    case SR_LIST_T:
    case SR_LEAF_EMPTY_T:
        printf("\n");
        break;
    default:
        printf("%s\n", value->dflt ? " [default]" : "");
        break;
    }
}

#endif /* __UTILS_H__ */