#pragma once

#ifdef _3DS
# define TGT_3DS
# define OS_HORIZON
# define ENDIAN_LITTLE
# define BITS_32
#elif defined(GEKKO)
# define TGT_WII
# define OS_NONE
# define ENDIAN_BIG
# define BITS_32
#else
# define TGT_PC
# if defined(__linux__)
#  define OS_LINUX
# elif defined(__FreeBSD__)
#  define OS_FREEBSD
# elif (defined(sun) || defined(__sun)) && (defined(__SVR4) || defined(__svr4__))
#  define OS_SOLARIS
# elif defined(__APPLE__) && defined(__MACH__)
#  define OS_OSX
# elif defined(_WIN32) || defined(WIN32)
#  define OS_WIN
# else
#  warning "Unsupported OS… maybe"
# endif
# if defined(__arm__)
#  define BITS_32
# elif defined(__aarch64__)
#  define BITS_64
# elif defined(__i386__)
#  define BITS_32
# elif defined(__amd64__)
#  define BITS_64
# elif defined(__ppc64__)
#  define BITS_64
# elif defined(__ppc__)
#  define BITS_32
# else
#  error "Unsupported CPU architecture."
# endif
# if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define ENDIAN_BIG
# elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define ENDIAN_LITTLE
# else
#  error "Get a real machine."
# endif
# if __BYTE_ORDER__ != __FLOAT_WORD_ORDER__
#  error "Get a real machine."
# endif
#endif

#ifdef ENDIAN_BIG
#define BIG16(x) (x)
#define BIG32(x) (x)
#define BIG64(x) (x)
#define LITTLE16(x) (__builtin_bswap16(x))
#define LITTLE32(x) (__builtin_bswap32(x))
#define LITTLE64(x) (__builtin_bswap64(x))
#else
#define LITTLE16(x) (x)
#define LITTLE32(x) (x)
#define LITTLE64(x) (x)
#define BIG16(x) (__builtin_bswap16(x))
#define BIG32(x) (__builtin_bswap32(x))
#define BIG64(x) (__builtin_bswap64(x))
#endif

#ifdef OS_WIN
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif
