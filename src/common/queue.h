#ifndef __PMLIB_QUEUE_H__
#define __PMLIB_QUEUE_H__

#include <stdbool.h>

typedef struct {
    int h;
    int t;

    char *data;
    int len;
} queue_t;

#define queue_def_extern(name, len) \
    char __##name##buf[len]; \
    static queue_t __##name##_obj = { 0, 0, __##name##buf, sizeof(__##name##buf) }; \
    queue_t *const name = &__##name##_obj;

#define queue_def(name, len) \
    char __##name##buf[len]; \
    static queue_t __##name##_obj = { 0, 0, __##name##buf, sizeof(__##name##buf) }; \
    static queue_t *const name = &__##name##_obj;

#define queue_def_with_buf(name, _data, _len) \
    static queue_t __##name##_obj = { .h = 0, .t = 0, .data = (char*)_data, .len = _len }; \
    static queue_t *const name = &__##name##_obj;

void queue_init(queue_t *q, void *data, int len);

int queue_get_size(queue_t *q);
int queue_get_data_len(queue_t *q);
int queue_get_space_len(queue_t *q);

int queue_is_empty(queue_t *q);
int queue_is_full(queue_t *q);

int queue_push(queue_t *q, const void *_data, int len);
int queue_peek(queue_t *q, void *_buf, int len);
int queue_pop(queue_t *q, void *_buf, int len);
int queue_pop_last(queue_t *q, void *_buf, int len);

int queue_waste(queue_t *q, int len);
int queue_waste_all(queue_t *q);

bool queue_get_linear_buffer(queue_t *q, char **_data, int *_len);
void queue_reset(queue_t *q);

#endif
