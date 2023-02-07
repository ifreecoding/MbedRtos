
#ifndef  __PROCESS_H__
#define  __PROCESS_H__


#include "global.h"


#define TEST_CMD1                   0x01
#define TEST_CMD2                   0x02


#define CMD_OK                      0
#define CMD_ERR                     1


extern void msg_process(TASK_STR* ptask);
extern void process_one_msg(BUF_NODE* pbuf);
extern BUF_NODE* test_cmd1(USR_MSG* pbuf);
extern BUF_NODE* test_cmd2(USR_MSG* pbuf);


#endif

