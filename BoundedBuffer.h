#ifndef _BOUNDEDBUFFER_H_
#define _BOUNDEDBUFFER_H_

/*
 * BoundedBuffer.h
 *
 * Header file for a bounded buffer implemented using pthread
 * facilities to allow it to be driven by concurrent threads.
 * In particular, each bounded buffer uses mutexes and condition variables
 * to guarantee thread-safety.
 *
 * Uses standard tricks to keep it very generic.
 * Uses heap allocated data structures.
 *
 * Author: Peter Dickman
 * Revised by Joe Sventek
 *
 * Created: 7-3-2000
 * Edited:  28-2-2001
 * Edited:  4 May 2015
 * Edited:  9 November 2017 - converted to dispatch table structure
 *
 * Version: 1.2
 *
 */

/* opaque data structure representing BoundedBuffer instance */
typedef struct bounded_buffer BoundedBuffer;

/* create a bounded buffer to hold `size' items; return NULL if error */
BoundedBuffer *BoundedBuffer_create(int size);

struct bounded_buffer {
    void *self;            /* the instance-specific data structure */
    /* destroy bounded buffer, returning all heap storage */
    void (*destroy)(BoundedBuffer *bb);
    /* place item in BB, blocking if the BB is full */
    void (*blockingWrite)(BoundedBuffer *bb, void *item);
    /* place item in BB if room; return 1 if successful, 0 if not */
    int (*nonblockingWrite)(BoundedBuffer *bb, void *item);
    /* read item from BB, blocking if the BB is empty */
    void (*blockingRead)(BoundedBuffer *bb, void **item);
    /* read item from BB if non-empty; return 1 if successful, 0 if not */
    int (*nonblockingRead)(BoundedBuffer *bb, void **item);
};

#endif /* _BOUNDEDBUFFER_H_ */
