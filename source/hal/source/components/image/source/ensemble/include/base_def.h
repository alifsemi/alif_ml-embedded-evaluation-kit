#ifndef __BASE_DEF_H__
#define __BASE_DEF_H__

#define ENABLE_PRINTF      0
#if ENABLE_PRINTF
#define    DEBUG_PRINTF(f,...)    printf(f,##__VA_ARGS__)
#else
#define    DEBUG_PRINTF(f,...)
#endif

#define HW_REG_WORD(base,offset) *((volatile unsigned int *) (base + offset))

#define UNUSED(x)       ((void)(x))

#endif // #ifndef __BASE_DEF_H__

