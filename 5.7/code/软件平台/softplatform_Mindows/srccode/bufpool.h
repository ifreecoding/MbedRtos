
#ifndef __BUFPOOL_H__
#define __BUFPOOL_H__


#include "global.h"


/* �������buf�������� */
#define BUF_TYPE_NUM        (sizeof(gabuf_len) / sizeof(U32))


/* ����س�ʼ����ֵ, ���峤����Ҫ��������ʽ���� */
#define BUF_CONFIG_VAL \
{\
    64, 128, BUF_MAX_LEN\
}


#endif

