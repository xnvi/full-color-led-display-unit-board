#include "ringbuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int32_t rb_init_sta(rb_handle *p, uint8_t *buf, uint32_t size)
{
    if (!(p && buf && size)) {
        return 1;
    }

    p->buf = buf;
    p->total_size = size;
    p->used_size = 0;
    p->head = 0;
    p->tail = 0;

    return 0;
}

int32_t rb_destroy_sta(rb_handle *p)
{
    if (p) {
        memset(p->buf, 0, p->total_size);
        p->total_size = 0;
        p->used_size = 0;
        p->head = 0;
        p->tail = 0;
    }
    else {
        return 1;
    }

    return 0;
}

rb_handle *rb_init_dyn(uint32_t size)
{
    rb_handle *p = NULL;
    uint8_t *buf = NULL;

    p = (rb_handle *)malloc(sizeof(rb_handle));
    if (p == NULL) {
        return NULL;
    }

    buf = (uint8_t *)malloc(size);
    if (buf == NULL) {
        free(p);
        return NULL;
    }

    rb_init_sta(p, buf, size);

    return p;
}

int32_t rb_destroy_dyn(rb_handle *p)
{
    if (p) {
        if (rb_destroy_sta(p)) {
            return 2;
        }
        free(p->buf);
        free(p);
    }
    else {
        return 1;
    }

    return 0;
}

inline int32_t rb_write_byte(rb_handle *p, uint8_t data)
{
#if (RB_CFG_CHECK_PARAM == 1)
    if (!p) {
        return 1;
    }
#endif
    if (rb_is_full(p)) {
        return 2;
    }

	p->buf[p->tail] = data;
	p->tail = (p->tail + 1) % p->total_size;
    p->used_size += 1;

    return 0;
}

inline int32_t rb_read_byte(rb_handle *p, uint8_t *data)
{
#if (RB_CFG_CHECK_PARAM == 1)
    if (!p) {
        return 1;
    }
#endif
    if (rb_is_empty(p)) {
        return 2;
    }

	*data = p->buf[p->head];
	p->head = (p->head + 1) % p->total_size;
    p->used_size -= 1;

    return 0;
}


int32_t rb_write(rb_handle *p, const uint8_t *buf, int32_t len)
{
    int32_t count = 0;
#if (RB_CFG_CHECK_PARAM == 1)
    if (!(p && buf && len)) {
        return 1;
    }
#endif
    if (len > rb_get_free_size(p)) {
        return 2;
    }

    if(p->tail + len < p->total_size) {
        memcpy(&p->buf[p->tail], buf, len);
        p->tail += len;
    }
    else if (p->tail + len > p->total_size) {
        count = p->total_size - p->tail;
        memcpy(&p->buf[p->tail], buf, count);
        memcpy(&p->buf[0], &buf[count], len - count);
        p->tail = len - count;
    }
    else {
        memcpy(&p->buf[p->tail], buf, len);
        p->tail = 0;
    }

    p->used_size += len;

    return 0;
}

int32_t rb_read(rb_handle *p, uint8_t *buf, int32_t len)
{
    int32_t count = 0;
#if (RB_CFG_CHECK_PARAM == 1)
    if (!(p && buf && len)) {
        return 1;
    }
#endif
    if (len > rb_get_used_size(p)) {
        return 2;
    }

    if(p->head + len < p->total_size) {
        memcpy(buf, &p->buf[p->head], len);
        p->head += len;
    }
    else if (p->head + len > p->total_size) {
        count = p->total_size - p->head;
        memcpy(buf, &p->buf[p->head], count);
        memcpy(&buf[count], &p->buf[0], len - count);
        p->head = len - count;
    }
    else
    {
        memcpy(buf, &p->buf[p->head], len);
        p->head = 0;
    }

    p->used_size -= len;

    return 0;
}

uint8_t *rb_preread(rb_handle *p, int32_t *len)
{
#if (RB_CFG_CHECK_PARAM == 1)
    if (!(p && len)) {
        return NULL;
    }
#endif
    if (*len > rb_get_used_size(p)) {
        return NULL;
    }

    if(p->head + *len <= p->total_size) {
        return &p->buf[p->head];
    }
    else {
        *len = p->total_size - p->head;
        return &p->buf[p->head];
    }
}

int32_t rb_preread_free(rb_handle *p, int32_t len)
{
#if (RB_CFG_CHECK_PARAM == 1)
    if (!(p && len)) {
        return 1;
    }
#endif
    if (len > rb_get_used_size(p)) {
        return 2;
    }
    
    if(p->head + len < p->total_size) {
        p->used_size -= len;
        p->head += len;
    }
    else if (p->head + len > p->total_size) {
        p->used_size -= p->total_size - p->head;
        p->head = 0;
    }
    else {
        p->used_size -= len;
        p->head = 0;
    }
    
    return 0;
}

inline int32_t rb_get_used_size(const rb_handle *p)
{
    return p->used_size;
}

inline int32_t rb_get_free_size(const rb_handle *p)
{
    return p->total_size - p->used_size;
}

inline int32_t rb_is_empty(const rb_handle *p)
{
    return p->used_size == 0;
}

inline int32_t rb_is_full(const rb_handle *p)
{
    return p->used_size == p->total_size;
}

void rb_print(const rb_handle *p)
{
    printf("ringbuffer info addr: %p, total: %d, used: %d, head: %d, tail: %d\n",
        p->buf, p->total_size, p->used_size, p->head, p->tail);
}
