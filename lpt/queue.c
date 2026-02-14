#include <stdio.h>
#include "queue.h"

typedef unsigned long ulong;
typedef unsigned int uint;


struct lpt_queue_s {
  size_t head;
  size_t tail;
  size_t count;
  size_t capacity;
  ulong *data;
};

typedef struct lpt_queue_s lpt_queue;

lpt_queue * lpt_queue_new(size_t capacity) {
  lpt_queue *q = malloc(sizeof(lpt_queue));
  q->head = 0;
  q->tail = 0;
  q->count = 0;
  q->capacity = capacity;
  q->data = malloc(capacity * sizeof(ulong));
  return q;
}

void lpt_queue_free(lpt_queue *q) {
  free(q->data);
  free(q);
}

void lpt_queue_reset(lpt_queue *q) {
  q->head = 0;
  q->tail = 0;
  q->count = 0;
}

bool lpt_queue_empty(lpt_queue *q) {
  return q->count == 0;
}

void lpt_queue_pushright(lpt_queue *q, ulong value) {
  if(q->count == q->capacity) {
    // resize
    size_t new_capacity = q->capacity * 2;
    ulong *new_data = malloc(new_capacity * sizeof(ulong));
    for(size_t i=0;i<q->count;++i) {
      new_data[i] = q->data[(q->head + i) % q->capacity];
    }
    free(q->data);
    q->data = new_data;
    q->capacity = new_capacity;
    q->head = 0;
    q->tail = q->count;
  }
  q->data[q->tail] = value;
  q->tail = (q->tail + 1) % q->capacity;
  q->count++;
}

ulong lpt_queue_popleft(lpt_queue *q) {
  ulong value = q->data[q->head];
  q->head = (q->head + 1) % q->capacity;
  q->count--;
  return value;
}

bool lpt_queue_contains(lpt_queue *q, ulong value) {
  for(size_t i=0;i<q->count;++i) {
    if(q->data[(q->head + i) % q->capacity] == value) {
      return true;
    }
  }
  return false;
}
