#ifndef CC_H_
#define CC_H_

#include <stdlib.h>
#include <assert.h>
#include <stm32f103x6.h>

/* Typedefs */

typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef int8_t s8_t;
typedef int16_t s16_t;
typedef int32_t s32_t;

typedef u32_t mem_ptr_t;
typedef uint32_t sys_prot_t;

#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"

#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __packed
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/* Options */

#define LWIP_RAND()  ((uint32_t)rand())
#define LWIP_PLATFORM_DIAG(x)   ITM_Print(x, strlen(x))
//#define LWIP_PLATFORM_ASSERT(x) do {ITM_Print(x, strlen(x));\
//    abort();} while(0)
#define LWIP_NOASSERT
#define LWIP_UNUSED_ARG(x) (void)x

/* Settings */

#define LWIP_NO_STDDEF_H 1
#define LWIP_NO_STDINT_H 1
#define LWIP_NO_INTTYPES_H 1
#define LWIP_NO_STDDEF_H 1

#ifndef SYS_LIGHTWEIGHT_PROT
#define SYS_LIGHTWEIGHT_PROT 1
#endif

#endif