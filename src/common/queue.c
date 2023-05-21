#include "queue.h"


#ifndef FIND_MIN
#define FIND_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif


/**
 * @brief Optionally initialise at runtime
 *
 * @param q Queue context
 * @param data Point to data buffer to use
 * @param len Length of data buffer
 */
void queue_init(queue_t *q, void *data, int len)
{
    q->h = 0;
    q->t = 0;
    q->data = data;
    q->len = len;
}

/**
 * @brief Get size of queue
 *
 * @param q Queue context
 * @return Size of queue
 */
int queue_get_size(queue_t *q)
{
    return q->len - 1;
}

/**
 * @brief Get data length currently in queue
 *
 * @param q Queue context
 * @return Lenght of data in queue
 */
int queue_get_data_len(queue_t *q)
{
    return (q->t + q->len - q->h) % q->len;
}

/**
 * @brief Get remaining space in queue
 *
 * @param q Queue context
 * @return Space available in queue
 */
int queue_get_space_len(queue_t *q)
{
    return queue_get_size(q) - queue_get_data_len(q);
}

/**
 * @brief Determine if queue is empty
 *
 * @param q Queue context
 * @return true if empty, false if otherwise
 */
int queue_is_empty(queue_t *q)
{
    return queue_get_data_len(q) == 0;
}

/**
 * @brief Determine if queue is empty
 *
 * @param q Queue context
 * @return true if empty, false if otherwise
 */
int queue_is_full(queue_t *q)
{
    return queue_get_space_len(q) == 0;
}

/**
 * @brief Push _data onto queue of len bytes
 *
 * @param q Queue context
 * @param _data Pointer of data to push to queue
 * @param len Length of data to push to queue
 * @return Length of _data pushed
 */
int queue_push(queue_t *q, const void *_data, int len)
{
    const char *data = (const char *)_data;
    if (queue_get_space_len(q) < len)
        return 0;

    for (int i = 0; i < len; i++) {
        q->data[q->t] = data[i];
        q->t = (q->t + 1) % q->len;
    }

    return len;
}

/**
 * @brief Peek queue in _buf of len bytes
 *
 * @param q Queue context
 * @param _buf Pointer to store data peeked
 * @param len Length of data to peek
 * @return Number of bytes peeked into _buf
 */
int queue_peek(queue_t *q, void *_buf, int len)
{
    char *buf = (char *)_buf;
    int data_len = queue_get_data_len(q);
    int read_len = FIND_MIN(data_len, len);

    int h = q->h;
    for (int i = 0; i < read_len; i++) {
        buf[i] = q->data[h];
        h = (h + 1) % q->len;
    }

    return read_len;
}

/**
 * @brief Pop len bytes from queue into _buf
 *
 * @param q Queue context
 * @param _buf Pointer to pop data into
 * @param len Length of _buf
 * @return Number of bytes popped from queue
 */
int queue_pop(queue_t *q, void *_buf, int len)
{
    char *buf = (char *)_buf;
    int peek_len = queue_peek(q, buf, len);

    queue_waste(q, peek_len);
    return peek_len;
}

/**
 * @brief Pop len bytes from end of queue into _buf
 *
 * @param q Queue context
 * @param _buf Pointer to pop data into
 * @param len Length of _buf
 * @return Number of bytes popped from end of queue
 */
int queue_pop_last(queue_t *q, void *_buf, int len)
{
    char *buf = (char *)_buf;
    int data_len = queue_get_data_len(q);
    if (data_len < len)
        return 0;

    queue_waste(q, data_len - len);

    return queue_pop(q, buf, len);
}

/**
 * @brief Waste (delete) len bytes from queue
 *
 * @param q Queue context
 * @param len Length to waste
 * @return Number of bytes wasted
 */
int queue_waste(queue_t *q, int len)
{
    int data_len = queue_get_data_len(q);
    int read_len = FIND_MIN(data_len, len);

    q->h = (q->h + read_len) % q->len;

    return read_len;
}

/**
 * @brief Waste (delete) entire queue
 *
 * @param q Queue context
 * @return Number of bytes wasted
 */
int queue_waste_all(queue_t *q)
{
    int data_len = queue_get_data_len(q);
    return queue_waste(q, data_len);
}

/// @brief Make sure queue is linear
/// @param q Queue context
/// @return true when the queue is linear. otherwise, False.
int queue_is_linear_buffer(queue_t *q)
{
    return (q->h == 0) && (q->t >= q->h);
}

/// @brief Get linear buffer
/// @param q Queue context
/// @param _data queue data to return
/// @param _len queue len to return
/// @return
bool queue_get_linear_buffer(queue_t *q, char **_data, int *_len)
{
    if (!queue_is_linear_buffer(q))
        return false;

    *_data = q->data;
    *_len = queue_get_data_len(q);

    return true;
}

/// @brief Reset head and tail
/// @param q q Queue context
void queue_reset(queue_t *q)
{
    q->h = 0;
    q->t = 0;
}
