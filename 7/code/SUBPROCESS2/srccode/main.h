
#include "stm32f10x.h"


/* 定义变量类型 */
typedef unsigned char           U8;
typedef unsigned short          U16;
typedef unsigned int            U32;
typedef signed char             S8;
typedef short                   S16;
typedef int                     S32;


typedef void                    (*PRINT_FUNC)(U32 port, U8* pstring);


#define PRINT_FUNC_ADDR         0x08000C89


extern void time_delay(void);

