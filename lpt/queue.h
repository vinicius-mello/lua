#ifndef LPT_QUEUE_H
#define LPT_QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

struct lpt_queue_s;
typedef struct lpt_queue_s lpt_queue;

lpt_queue * lpt_queue_new(size_t capacity);
void lpt_queue_free(lpt_queue *q);
void lpt_queue_reset(lpt_queue *q);
bool lpt_queue_empty(lpt_queue *q);
void lpt_queue_pushright(lpt_queue *q, unsigned long value);
bool lpt_queue_contains(lpt_queue *q, unsigned long value);
unsigned long lpt_queue_popleft(lpt_queue *q);

#endif