#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

#include <stdint.h>

#define RB_CFG_CHECK_PARAM 0

typedef struct
{
    uint32_t head;
    uint32_t tail;
    uint32_t total_size;
    uint32_t used_size;
    uint8_t *buf;
} rb_handle;

int32_t rb_init_sta(rb_handle *p, uint8_t *buf, uint32_t size);
int32_t rb_destroy_sta(rb_handle *p);

rb_handle *rb_init_dyn(uint32_t size);
int32_t rb_destroy_dyn(rb_handle *p);

int32_t rb_write_byte(rb_handle *p, uint8_t data);
int32_t rb_read_byte(rb_handle *p, uint8_t *data);

int32_t rb_write(rb_handle *p, const uint8_t *buf, int32_t len);
int32_t rb_read(rb_handle *p, uint8_t *buf, int32_t len);

// 无内存复制读，返回一段连续的内存地址，失败返回NULL
// 如果环形队列跨过尾部，则最多读到环形队列尾部，*len被修改为尾部之前的数据长度
uint8_t *rb_preread(rb_handle *p, int32_t *len);
// 与 rb_preread() 配合使用，入参 len 不得超过 rb_preread() 返回的 *len
int32_t rb_preread_free(rb_handle *p, int32_t len);

int32_t rb_get_used_size(const rb_handle *p);
int32_t rb_get_free_size(const rb_handle *p);
int32_t rb_is_empty(const rb_handle *p);
int32_t rb_is_full(const rb_handle *p);

void rb_print(const rb_handle *p);

#endif
