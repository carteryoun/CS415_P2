#ifndef _QUEUE_H_
#define _QUEUE_H_

/*
 * interface definition for generic FIFO queue
 *
 * patterned roughly after Java 6 Queue interface
 */

typedef struct queue Queue;        /* forward reference */

/*
 * create a queue
 *
 * returns a pointer to the queue, or NULL if there are malloc() errors
 */
Queue *Queue_create(void);

/*
 * now define struct queue
 */
struct queue {
/*
 * the private data of the queue
 */
    void *self;

/*
 * destroys the queue
 */
    void (*destroy)(Queue *q);

/*
 * appends `element' to the end of the queue
 *
 * returns 1 if successful, 0 if unsuccesful
 */
    int (*enqueue)(Queue *q, void *element);

/*
 * retrieves, but does not remove, the head of the queue, returning that
 * element in `*element'
 *
 * returns 1 if successful, 0 if unsuccessful (queue is empty)
 */
    int (*front)(Queue *q, void **element);

/*
 * Retrieves, and removes, the head of the queue, returning that
 * element in `*element'
 *
 * return 1 if successful, 0 if not (queue is empty)
 */
    int (*dequeue)(Queue *q, void **element);

/*
 * returns the number of elements in the queue
 */
    long (*size)(Queue *q);
};

#endif /* _QUEUE_H_ */
