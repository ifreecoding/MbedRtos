
#ifndef __MSGPRINT_H__
#define __MSGPRINT_H__


#include "global.h"


#define PRINT_BUF_LEN               200


extern void msg_print(void);
extern void print_init(void);
extern void print_task_send_data(BUF_NODE* pbuf);


#endif

