//
//  helper.h
//  multi_thread_programming
//
//  Created by Igarashi Masanori on 2018/09/04.
//  Copyright © 2018 Igarashi Masanori. All rights reserved.
//

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
