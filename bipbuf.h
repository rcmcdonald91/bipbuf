/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2024, R. Christian McDonald <rcm@rcm.sh>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BIPBUF_H
#define BIPBUF_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct bipbuf {
    uint8_t *buf;
    size_t size;
    size_t astart, asize,
           bstart, bsize;
    size_t rstart;
} bipbuf_t;

int bipbuf_init(bipbuf_t *, size_t);
int bipbuf_init_ex(bipbuf_t *, size_t, bool);

void bipbuf_free(bipbuf_t *);
void bipbuf_free_ex(bipbuf_t *, bool);

uint8_t *bipbuf_write_aquire(bipbuf_t *, size_t *);
void bipbuf_write_release(bipbuf_t *, size_t);

uint8_t *bipbuf_read_aquire(bipbuf_t *, size_t *);
void bipbuf_read_release(bipbuf_t *, size_t);
void bipbuf_read_release_ex(bipbuf_t *, size_t, bool);

uint8_t *bipbuf_peek(bipbuf_t *, size_t, size_t *);
size_t bipbuf_squash(bipbuf_t *);

inline size_t bipbuf_read_avail(bipbuf_t *bipbuf) {
    size_t avail;

    (void) bipbuf_read_aquire(bipbuf, &avail);
    bipbuf_read_release(bipbuf, 0); /* just peeking */

    return (avail);
}

inline size_t bipbuf_write_avail(bipbuf_t *bipbuf) {
    size_t avail;

    (void) bipbuf_write_aquire(bipbuf, &avail);
    bipbuf_write_release(bipbuf, 0); /* just peeking */

    return (avail);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BIPBUF_H */
