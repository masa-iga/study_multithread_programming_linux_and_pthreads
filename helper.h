#ifndef helper_h
#define helper_h

// fflush() needed for multi thread
#define VPRINTF(...) \
    do { \
        printf("%s(L%d) ", __func__, __LINE__); \
        printf(__VA_ARGS__); \
        fflush(stdout); \
} while (false)


#endif /* helper_h */
