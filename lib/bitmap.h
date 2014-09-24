/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012, 2013, 2014 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BITMAP_H
#define BITMAP_H 1

#include <limits.h>
#include <stdlib.h>
#include "util.h"

#define BITMAP_ULONG_BITS (sizeof(unsigned long) * CHAR_BIT)

static inline unsigned long *
bitmap_unit__(const unsigned long *bitmap, size_t offset)
{
    return CONST_CAST(unsigned long *, &bitmap[offset / BITMAP_ULONG_BITS]);
}

static inline unsigned long
bitmap_bit__(size_t offset)
{
    return 1UL << (offset % BITMAP_ULONG_BITS);
}

#define BITMAP_N_LONGS(N_BITS) DIV_ROUND_UP(N_BITS, BITMAP_ULONG_BITS)

static inline size_t
bitmap_n_longs(size_t n_bits)
{
    return BITMAP_N_LONGS(n_bits);
}

static inline size_t
bitmap_n_bytes(size_t n_bits)
{
    return bitmap_n_longs(n_bits) * sizeof(unsigned long int);
}

static inline unsigned long *
bitmap_allocate(size_t n_bits)
{
    return xzalloc(bitmap_n_bytes(n_bits));
}

unsigned long *bitmap_allocate1(size_t n_bits);

static inline unsigned long *
bitmap_clone(const unsigned long *bitmap, size_t n_bits)
{
    return xmemdup(bitmap, bitmap_n_bytes(n_bits));
}

static inline void
bitmap_free(unsigned long *bitmap)
{
    free(bitmap);
}

static inline bool
bitmap_is_set(const unsigned long *bitmap, size_t offset)
{
    return (*bitmap_unit__(bitmap, offset) & bitmap_bit__(offset)) != 0;
}

static inline void
bitmap_set1(unsigned long *bitmap, size_t offset)
{
    *bitmap_unit__(bitmap, offset) |= bitmap_bit__(offset);
}

static inline void
bitmap_set0(unsigned long *bitmap, size_t offset)
{
    *bitmap_unit__(bitmap, offset) &= ~bitmap_bit__(offset);
}

static inline void
bitmap_set(unsigned long *bitmap, size_t offset, bool value)
{
    if (value) {
        bitmap_set1(bitmap, offset);
    } else {
        bitmap_set0(bitmap, offset);
    }
}

void bitmap_set_multiple(unsigned long *, size_t start, size_t count,
                         bool value);
bool bitmap_equal(const unsigned long *, const unsigned long *, size_t n);
size_t bitmap_scan(const unsigned long int *, bool target,
                   size_t start, size_t end);
size_t bitmap_count1(const unsigned long *, size_t n);

void bitmap_and(unsigned long *dst, const unsigned long *arg, size_t n);
void bitmap_or(unsigned long *dst, const unsigned long *arg, size_t n);
void bitmap_not(unsigned long *dst, size_t n);

bool bitmap_is_all_zeros(const unsigned long *, size_t n);

#define BITMAP_FOR_EACH_1(IDX, SIZE, BITMAP) \
    for ((IDX) = bitmap_scan(BITMAP, 1, 0, SIZE); (IDX) < (SIZE);    \
         (IDX) = bitmap_scan(BITMAP, 1, (IDX) + 1, SIZE))

/* More efficient access to a map of single ulong. */
#define ULONG_FOR_EACH_1(IDX, MAP)                  \
    for (unsigned long map__ = (MAP);               \
         map__ && (((IDX) = raw_ctz(map__)), true); \
         map__ = zero_rightmost_1bit(map__))

#define ULONG_SET0(MAP, OFFSET) ((MAP) &= ~(1UL << (OFFSET)))
#define ULONG_SET1(MAP, OFFSET) ((MAP) |= 1UL << (OFFSET))

#endif /* bitmap.h */
