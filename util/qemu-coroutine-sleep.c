/*
 * QEMU coroutine sleep
 *
 * Copyright IBM, Corp. 2011
 *
 * Authors:
 *  Stefan Hajnoczi    <stefanha@linux.vnet.ibm.com>
 *
 * This work is licensed under the terms of the GNU LGPL, version 2 or later.
 * See the COPYING.LIB file in the top-level directory.
 *
 */

#include "qemu/osdep.h"
#include "qemu/coroutine_int.h"
#include "qemu/timer.h"
#include "block/aio.h"

typedef struct CoSleepCB {
    QEMUTimer *ts;
    Coroutine *co;
} CoSleepCB;

static void co_sleep_cb(void *opaque)
{
    Coroutine *co = opaque;
    aio_co_wake(co);
    timer_del(co->sleep_qt);
    timer_free(co->sleep_qt);
}

void coroutine_fn co_aio_sleep_ns(AioContext *ctx, QEMUClockType type,
                                  int64_t ns)
{
    Coroutine *co = qemu_coroutine_self();
    co->sleep_qt = aio_timer_new(ctx, type, SCALE_NS, co_sleep_cb, co);
    timer_mod(co->sleep_qt, qemu_clock_get_ns(type) + ns);
    qemu_coroutine_yield();
}
