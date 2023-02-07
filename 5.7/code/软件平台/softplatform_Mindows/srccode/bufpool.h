
#ifndef __BUFPOOL_H__
#define __BUFPOOL_H__


#include "global.h"


/* 缓冲池中buf类型数量 */
#define BUF_TYPE_NUM        (sizeof(gabuf_len) / sizeof(U32))


/* 缓冲池初始化数值, 缓冲长度需要按递增方式排列 */
#define BUF_CONFIG_VAL \
{\
    64, 128, BUF_MAX_LEN\
}


#endif

