#ifndef TESTING_H_
#define TESTING_H_

#include <stdio.h>
#include <string.h>

#define PRINTF_FMT(X) _Generic((X),     \
    char:               "%c",           \
    signed char:        "%hhd",         \
    unsigned char:      "%hhu",         \
    short:              "%hd",          \
    unsigned short:     "%hu",          \
    int:                "%d",           \
    unsigned int:       "%u",           \
    long:               "%ld",          \
    unsigned long:      "%lu",          \
    long long:          "%lld",         \
    unsigned long long: "%llu",         \
    float:              "%f",           \
    double:             "%f",           \
    long double:        "%Lf",          \
    _Bool:              "%d",           \
    default:            "<unsupported>" \
)

#define ASSERTION_FAILED printf("Assertion failed at %s:%d\n", __FILE__, __LINE__)

#define ASSERT_TRUE(ret, X)                       \
    do {                                          \
        if (!(X)) {                               \
            ASSERTION_FAILED;                     \
            printf("Expected true, got false\n"); \
            (ret)++;                              \
        }                                         \
    } while (0)

#define ASSERT_NONZERO(ret, X)                            \
    do {                                                  \
        if ((X) == 0) {                                   \
            ASSERTION_FAILED;                             \
            printf("Expected nonzero value, got zero\n"); \
            (ret)++;                                      \
        }                                                 \
    } while (0)

#define ASSERT_EQ(ret, ACTUAL, EXPECTED)            \
    do {                                            \
        __typeof__(ACTUAL) actual = (ACTUAL);       \
        __typeof__(EXPECTED) expected = (EXPECTED); \
        if (actual != expected) {                   \
            ASSERTION_FAILED;                       \
            printf("Expected \"");                  \
            printf(PRINTF_FMT(expected), expected); \
            printf("\", got \"");                   \
            printf(PRINTF_FMT(actual), actual);     \
            printf("\"\n");                         \
            (ret)++;                                \
        }                                           \
    } while (0)

#define ASSERT_STR_EQ(ret, ACTUAL, EXPECTED)                           \
    do {                                                               \
        const char *actual = (ACTUAL);                                 \
        const char *expected = (ACTUAL);                               \
        if (strcmp(actual, expected) != 0) {                           \
            ASSERTION_FAILED;                                          \
            printf("Expected \"%s\", got \"%s\"\n", expected, actual); \
            (ret)++;                                                   \
        }                                                              \
    } while (0)

#endif //TESTING_H_
