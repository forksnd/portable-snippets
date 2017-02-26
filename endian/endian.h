/* Endianness detection and swapping (v2)
 * Portable Snippets - https://gitub.com/nemequ/portable-snippets
 * Created by Evan Nemerson <evan@nemerson.com>
 *
 *   To the extent possible under law, the authors have waived all
 *   copyright and related or neighboring rights to this code.  For
 *   details, see the Creative Commons Zero 1.0 Universal license at
 *   https://creativecommons.org/publicdomain/zero/1.0/
 */

#if !defined(PSNIP_ENDIAN_H)
#define PSNIP_ENDIAN_H

#if !defined(psnip_uint16_t) || !defined(psnip_uint32_t) || !defined(psnip_uint64_t)
#  include "../exact-int/exact-int.h"
#endif

#if \
  !defined(psnip_builtin_bswap16) || \
  !defined(psnip_builtin_bswap32) || \
  !defined(psnip_builtin_bswap64)
#  include "../builtin/builtin.h"
#endif

#if defined(HEDLEY_INLINE)
#  define PSNIP_ENDIAN_INLINE HEDLEY_INLINE
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  define PSNIP_ENDIAN_INLINE inline
#elif defined(__GNUC__) || defined(_MSC_VER)
#  define PSNIP_ENDIAN_INLINE __inline
#else
#  define PSNIP_ENDIAN_INLINE
#endif

#if !defined(PSNIP_ENDIAN_LITTLE)
#  define PSNIP_ENDIAN_LITTLE 1234
#endif
#if !defined(PSNIP_ENDIAN_BIG)
#  define PSNIP_ENDIAN_BIG 4321
#endif
#if !defined(PSNIP_ENDIAN_PDP)
#  define PSNIP_ENDIAN_PDP 3412
#endif

/* Detection
 *
 * Detecting endianness can be a bit tricky.  There isn't really a
 * good standard way of determining endianness, and it's actually
 * possible to mix endianness within a single program.  This is
 * currently pretty rare, though.
 *
 * We define PSNIP_ENDIAN_ORDER to PSNIP_ENDIAN_LITTLE,
 * PSNIP_ENDIAN_BIG, or PSNIP_ENDIAN_PDP.  Additionally, you can use
 * the PSNIP_RT_BYTE_ORDER to check the runtime byte order, which is a
 * bit more reliable (though it does introduce some runtime overhead.
 *
 * In the event we are unable to determine endianness at compile-time,
 * PSNIP_ENDIAN_ORDER is left undefined and you will be forced to rely
 * on PSNIP_RT_BYTE_ORDER. */

#if !defined(PSNIP_ENDIAN_FORCE_RT)

#if !defined(PSNIP_ENDIAN_ORDER)
/* GCC (and compilers masquerading as GCC) define  __BYTE_ORDER__. */
#  if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#    define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_LITTLE
#  elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#    define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_BIG
#  elif defined(__BYTE_ORDER__) && defined(__ORDER_PDP_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_PDP_ENDIAN__)
#    define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_PDP
/* We know the endianness of some common architectures.  Common
 * architectures not listed (ARM, POWER, MIPS, etc.) here are
 * bi-endian. */
#  elif defined(__amd64) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#    define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_LITTLE
#  elif defined(__s390x__) || defined(__zarch__)
#    define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_BIG
/* Looks like we'll have to rely on the platform.  If we're missing a
 * platform, please let us know. */
#  elif defined(_WIN32)
#    define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_LITTLE
#  elif defined(sun) || defined(__sun) /* Solaris */
#    include <sys/byteorder.h>
#    if defined(_LITTLE_ENDIAN)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_LITTLE
#    elif defined(_BIG_ENDIAN)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_BIG
#    endif
#  elif defined(__APPLE__)
#    include <libkern/OSByteOrder.h>
#    if defined(__LITTLE_ENDIAN__)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_LITTLE
#    elif defined(__BIG_ENDIAN__)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_BIG
#    endif
#  elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__) || defined(BSD)
#    include <machine/endian.h>
#    if defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_LITTLE
#    elif defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_BIG
#    elif defined(__BYTE_ORDER) && (__BYTE_ORDER == __PDP_ENDIAN)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_PDP
#    endif
#  else
#    include <endian.h>
#    if defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_LITTLE
#    elif defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_BIG
#    elif defined(__BYTE_ORDER) && defined(__PDP_ENDIAN) && (__BYTE_ORDER == __PDP_ENDIAN)
#      define PSNIP_ENDIAN_ORDER PSNIP_ENDIAN_PDP
#    endif
#  endif
#endif

/* PDP endian not yet supported.  Patches welcome. */
#if defined(PSNIP_ENDIAN_ORDER) && (PSNIP_ENDIAN_ORDER == PSNIP_ENDIAN_PDP)
#  error PDP endian is not supported.
#endif

#endif /* !defined(PSNIP_ENDIAN_FORCE_RT) */

#if defined(PSNIP_ENDIAN_FORCE_RT) || !defined(PSNIP_ENDIAN_ORDER)

static const union {
  unsigned char bytes[4];
  psnip_uint32_t value;
} psnip_endian_rt_data = {
  { 1, 2, 3, 4 }
};

#define PSNIP_RT_BYTE_ORDER_IS_LE (psnip_endian_rt_data.value == 0x04030201)
#define PSNIP_RT_BYTE_ORDER_IS_BE (psnip_endian_rt_data.value == 0x01020304)
#define PSNIP_RT_BYTE_ORDER (PSNIP_RT_BYTE_ORDER_IS_LE ? PSNIP_ENDIAN_LITTLE : PSNIP_ENDIAN_BIG)

#define PSNIP_ENDIAN__DEFINE_LE_FUNC(siz) \
  static PSNIP_ENDIAN_INLINE \
  psnip_uint##siz##_t psnip_endian_le##siz(psnip_uint##siz##_t v) { \
    return PSNIP_RT_BYTE_ORDER_IS_LE ? v : psnip_builtin_bswap##siz(v); \
  }

#define PSNIP_ENDIAN__DEFINE_BE_FUNC(siz) \
  static PSNIP_ENDIAN_INLINE \
  psnip_uint##siz##_t psnip_endian_le##siz(psnip_uint##siz##_t v) { \
    return PSNIP_RT_BYTE_ORDER_IS_BE ? v : psnip_builtin_bswap##siz(v); \
  }

PSNIP_ENDIAN__DEFINE_LE_FUNC(16)
PSNIP_ENDIAN__DEFINE_LE_FUNC(32)
PSNIP_ENDIAN__DEFINE_LE_FUNC(64)
PSNIP_ENDIAN__DEFINE_BE_FUNC(16)
PSNIP_ENDIAN__DEFINE_BE_FUNC(32)
PSNIP_ENDIAN__DEFINE_BE_FUNC(64)

#elif PSNIP_ENDIAN_ORDER == PSNIP_ENDIAN_LITTLE
#  define psnip_endian_le16(v) ((psnip_uint16_t) (v))
#  define psnip_endian_le32(v) ((psnip_uint32_t) (v))
#  define psnip_endian_le64(v) ((psnip_uint64_t) (v))
#  define psnip_endian_be16(v) psnip_builtin_bswap16((psnip_uint16_t) (v))
#  define psnip_endian_be32(v) psnip_builtin_bswap32((psnip_uint32_t) (v))
#  define psnip_endian_be64(v) psnip_builtin_bswap64((psnip_uint64_t) (v))
#elif PSNIP_ENDIAN_ORDER == PSNIP_ENDIAN_BIG
#  define psnip_endian_le16(v) psnip_builtin_bswap16((psnip_uint16_t) (v))
#  define psnip_endian_le32(v) psnip_builtin_bswap32((psnip_uint32_t) (v))
#  define psnip_endian_le64(v) psnip_builtin_bswap64((psnip_uint64_t) (v))
#  define psnip_endian_be16(v) ((psnip_uint16_t) (v))
#  define psnip_endian_be32(v) ((psnip_uint32_t) (v))
#  define psnip_endian_be64(v) ((psnip_uint64_t) (v))
#else
#  error Unknown endianness.
#endif

#endif /* !defined(PSNIP_ENDIAN_H) */
