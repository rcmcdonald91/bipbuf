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

#include "bipbuf.h"

#include <stdlib.h>
#include <string.h>

static void bipbuf_reset(bipbuf_t *, bool);

int
bipbuf_init_ex(bipbuf_t *bipbuf, size_t size, bool zerofill)
{
    if ((bipbuf->buf = (uint8_t *)malloc(size)) == NULL)
        return (-1);

    bipbuf->size = size;

    bipbuf_reset(bipbuf, zerofill);

    return (0);
}

int
bipbuf_init(bipbuf_t *bipbuf, size_t size)
{
    return (bipbuf_init_ex(bipbuf, size, false));
}

void
bipbuf_free_ex(bipbuf_t *bipbuf, bool zerofill)
{
    if (bipbuf->buf) {
        bipbuf_reset(bipbuf, zerofill);
        free(bipbuf->buf);
	}
}

void
bipbuf_free(bipbuf_t *bipbuf)
{
    return (bipbuf_free_ex(bipbuf, false));
}

uint8_t *
bipbuf_write_aquire(bipbuf_t *bipbuf, size_t *avail)
{
    size_t free;

    *avail = 0;

    if (bipbuf->bsize) {
        free = bipbuf->astart - bipbuf->bstart - bipbuf->bsize;
        if (!free)
            return (NULL);

        bipbuf->rstart = bipbuf->bstart + bipbuf->bsize;
        bipbuf->rsize = free;

        goto done;
	}

    free = bipbuf->size - bipbuf->astart - bipbuf->asize;
    if (free >= bipbuf->astart) {
        if (!free)
            return (NULL);

        bipbuf->rstart = bipbuf->astart + bipbuf->asize;
        bipbuf->rsize = free;

        goto done;
    }

    bipbuf->rstart = 0;
    bipbuf->rsize = bipbuf->astart;

done:
    *avail = free;

    return (&bipbuf->buf[bipbuf->rstart]);
}

void
bipbuf_write_release(bipbuf_t *bipbuf, size_t written)
{
    if (!written)
        goto done;

    if (!bipbuf->asize && !bipbuf->bsize) {
        bipbuf->astart = bipbuf->rstart;
        bipbuf->asize = written;
        goto done;
	}

    if (bipbuf->rstart == (bipbuf->astart + bipbuf->asize))
        bipbuf->asize += written;
    else
        bipbuf->bsize += written;

done:
    bipbuf->rstart = bipbuf->rsize = 0;
}

uint8_t *bipbuf_peek(bipbuf_t *bipbuf, size_t offset, size_t *avail)
{
    size_t pstart;

    *avail = 0;

    pstart = bipbuf->astart + offset;
    if (pstart < (bipbuf->astart + bipbuf->asize)) {
        *avail = bipbuf->asize - offset;
        goto done;
    }

    if (!bipbuf-bsize)
        return (NULL);

    offset -= bipbuf->asize;
    pstart = bipbuf->bstart + offset;

    *avail = bipbuf->bsize - offset;

done:
    return (&bipbuf->buf[pstart]);
}

uint8_t *
bipbuf_read_aquire(bipbuf_t *bipbuf, size_t *avail)
{
    *avail = 0;
    if (!bipbuf->asize)
        return (NULL);

    *avail = bipbuf->asize;

    return (&bipbuf->buf[bipbuf->astart]);
}

void
bipbuf_read_release_ex(bipbuf_t *bipbuf, size_t read, bool zerofill)
{
    if (zerofill)
        (void) memset(&bipbuf->buf[bipbuf->astart], '\0', read);

    if (read < bipbuf->asize) {
        bipbuf->asize -= read;
        bipbuf->astart += read;
    } else {
        bipbuf->astart = bipbuf->bstart;
        bipbuf->asize = bipbuf->bsize;
        bipbuf->bstart = bipbuf->bsize = 0;
    }
}

void
bipbuf_read_release(bipbuf_t *bipbuf, size_t read)
{
    return (bipbuf_read_release_ex(bipbuf, read, false));
}

extern inline size_t bipbuf_read_avail(bipbuf_t *);
extern inline size_t bipbuf_write_avail(bipbuf_t *);

static void
bipbuf_reset(bipbuf_t *bipbuf, bool zerofill)
{
    if (zerofill)
	    (void) memset(bipbuf->buf, '\0', bipbuf->size);

    bipbuf->astart = bipbuf->asize = 0;
    bipbuf->bstart = bipbuf->bsize = 0;
    bipbuf->rstart = bipbuf->rsize = 0;
}
