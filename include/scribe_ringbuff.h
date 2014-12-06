//
// Simple ring buffer implementation for message buffering.
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_RINGBUFF_H
#define SCRIBE_RINGBUFF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "scribe_conf.h"
#include "scribe_types.h"
#include "scribe_utils.h"

#define BUFFER_MASK (SCRIBE_RINGBUFFER_CAPACITY - 1)

struct scrb_ringbuff {
    char const * buffer[SCRIBE_RINGBUFFER_CAPACITY + 1];
    uint64_t write_index;
};

static inline
int scrb_ringbuff_write(struct scrb_ringbuff * const rb, char const * newelem)
{
    if (likely(rb->write_index <= SCRIBE_RINGBUFFER_CAPACITY)) {
        rb->buffer[rb->write_index & BUFFER_MASK] = newelem;
        rb->write_index += 1;
        return (SCRB_Success);
    } else {
        return (SCRB_Failure);
    }
}

static inline
char const * scrb_ringbuffer_read(struct scrb_ringbuff * const rb, uint64_t const indx)
{
    return rb->buffer[(rb->write_index + (~indx)) & BUFFER_MASK];
}

static inline
void scrb_ringbuff_reset(struct scrb_ringbuff * const rb)
{
    rb->write_index = 0;
    memset(rb->buffer, 0, sizeof(char const *) * SCRIBE_RINGBUFFER_CAPACITY); 
}

#ifdef __cplusplus
}
#endif

#endif

