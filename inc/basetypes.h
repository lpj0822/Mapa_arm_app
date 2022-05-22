/*
 * basetypes.h
 *
 * Histroy:
 *  2012-12-12 [Louis] Created file
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _BASE_TYPES_H_
#define _BASE_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AMBA_API __attribute__((visibility("default")))

typedef unsigned char       u8; /**< UNSIGNED  8-bit data type */
typedef unsigned short     u16; /**< UNSIGNED 16-bit data type */
typedef unsigned int       u32; /**< UNSIGNED 32-bit data type */
typedef unsigned long long u64; /**< UNSIGNED 64-bit data type */
typedef signed char         s8; /**<   SIGNED  8-bit data type */
typedef signed short       s16; /**<   SIGNED 16-bit data type */
typedef signed int         s32; /**<   SIGNED 32-bit data type */
typedef signed long long   s64; /**<   SIGNED 64-bit data type */

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof((x)[0]))
#endif

/* ==========================================================================*/

struct iav_window {
	u32 width;
	u32 height;
};

struct iav_offset {
	u32 x;
	u32 y;
};

struct iav_rect {
	u32 x;
	u32 y;
	u32 width;
	u32 height;
};

#define rect_init(_x, _y, _w, _h)(	\
{				\
	struct iav_rect _r;		\
	_r.x = (_x); _r.y = (_y); _r.width = (_w); _r.height = (_h);	\
	_r;			\
})

#define win_init(w, h)		rect_init(0, 0, w, h)

#define win_invalid(w)		((w).width == 0 || (w).height == 0)
#define rect_invalid(w)		(win_invalid(w) || (w).x > 0xffff || (w).y > 0xffff)

/* return 1 if r1 contains r2 */
#define rect_contain(r1, r2) ({			\
	u32 _x1 = (r1).x, _y1 = (r1).y, _w1 = (r1).width, _h1 = (r1).height; \
	u32 _x2 = (r2).x, _y2 = (r2).y, _w2 = (r2).width, _h2 = (r2).height; \
	_x1 <= _x2 && (_x1 + _w1) >= (_x2 + _w2) &&		\
	_y1 <= _y2 && (_y1 + _h1) >= (_y2 + _h2) ? 1 : 0;	\
	 })

/* ==========================================================================*/

static inline void iav_set_bit(u32 *bit_map, int nr)
{
	*bit_map |= (1 << nr);
}

static inline int iav_test_bit(u32 *bit_map, int nr)
{
	return !!((*bit_map) & (1 << nr));
}

/* ==========================================================================*/

#ifdef __cplusplus
}
#endif

#endif
