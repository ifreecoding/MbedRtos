
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "msgprint.h"


U32 gprint_level;
U32 gprint_module;


TASK_STR gprint_task;           /* print����ṹ */


/***********************************************************************************
��������: ����print���񲢽��г�ʼ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void create_print_task(void)
{
    /* ��ʼ����ӡ����, �رմ�ӡ����, ��ֹprint������������Ĵ�ӡ������ѭ�� */
    print_init();

    /* ����print_task���� */
    gprint_task.ptcb = MDS_TaskCreate("print_task", msg_print_task, NULL, NULL,
                                      PRINT_TASK_STACK, PRINT_TASK_PRIO, NULL);
    if(NULL == gprint_task.ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_PRINT,
                  "\r\ncreate print_task failed. (%s, %d)", __FILE__, __LINE__);
    }

    /* ��ʼ������Ķ��� */
    if(NULL == (gprint_task.pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_PRINT,
                  "\r\nprint_task queue init failed. (%s, %d)", __FILE__, __LINE__);
    }

    print_msg(PRINT_SUGGEST, PRINT_PRINT, "\r\nprint_task init finished. (%s, %d)",
              __FILE__, __LINE__);
}

/***********************************************************************************
��������: ��ӡ��Ϣ����, ��Ҫ��ӡ�����е���Ϣ��ӡ������.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void msg_print_task(void* pvPara)
{
    while(1)
    {
        /* ��ӡ���� */
        msg_print();
    }
}

/***********************************************************************************
��������: ��ӡ��Ϣ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_print(void)
{
    BUF_NODE* pbuf;

    /* �Ӷ��л�ȡҪ�������Ϣ */
    pbuf = print_task_receive_msg();
    if(NULL == pbuf)
    {
        return;
    }

    /* ��ӡ���յ�����Ϣ */
    print_task_send_data(pbuf);
}

/***********************************************************************************
��������: ��print��������Ϣ.
��ڲ���: pbuf: ������Ϣ��buf.
�� �� ֵ: none.
***********************************************************************************/
void send_msg_to_print_task(BUF_NODE* pbuf)
{
    /* ����Ϣ������� */
    if(RTN_SUCD != MDS_QuePut(gprint_task.pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_PRINT,
                  "\r\n%s put message to print_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
��������: print���������Ϣ.
��ڲ���: none.
�� �� ֵ: ���յ�����Ϣ��bufָ��, ���ΪNULL�������.
***********************************************************************************/
BUF_NODE* print_task_receive_msg(void)
{
    M_DLIST* pbuf;

    /* �Ӷ����л�ȡ��Ϣ */
    if(RTN_SUCD != MDS_QueGet(gprint_task.pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_PRINT,
                  "\r\n%s get message from print_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
��������: ��ʼ����ӡ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void print_init(void)
{
    print_level_set(PRINT_ALL);
    print_module_set(PRINT_ALL);
}

/***********************************************************************************
��������: ��ӡ��Ϣ����, ֻ�д˼���ʹ�ģ�����Ϣ���ر���ʱ���ܴ�ӡ����Ϣ.
��ڲ���: level: ��ӡ��Ϣ�ļ���.
                 PRINT_ALL: ȫ��.
                 PRINT_SUGGEST: ��ʾ.
                 PRINT_NORMAL: һ��.
                 PRINT_IMPORTANT: ��Ҫ.
          module: ��ӡ��Ϣ��ģ��.
                  PRINT_ALL: ȫ��.
                  PRINT_BUF: bufģ��.
                  PRINT_COM: ͨ��ģ��.
                  PRINT_TASK: ����ģ��.
                  PRINT_RX: ����ģ��.
                  PRINT_TX: ����ģ��.
                  PRINT_SCHED: ����ģ��.
                  PRINT_PROCESS: ����ģ��.
                  PRINT_PRINT: ��ӡģ��.
          fmt: ��һ��������ָ��, ����Ϊ�ַ���ָ��.
          ...: ��������.
�� �� ֵ: none.
***********************************************************************************/
void print_msg(U32 level, U32 module, U8* fmt, ...)
{
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    va_list args;

    /* �����ϴ�ӡ������ֱ�ӷ��� */
    if(!((0 != (level & gprint_level)) && (0 != (module & gprint_module))))
    {
        return;
    }

    /* ����buf, ���������Ҫ��ӡ���ַ� */
    pmsg_buf = (MSG_BUF*)buf_malloc_without_print(PRINT_BUF_LEN);
    if(NULL == pmsg_buf)
    {
        //print_msg(PRINT_IMPORTANT, PRINT_PRINT, "\r\ncan't malloc buf. (%s, %d)",
        //          __FILE__, __LINE__);

        return;
    }

    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    /* ���ַ�����ӡ���ڴ� */
    va_start(args, fmt);
    (void)vsprintf(pmsg_str->buf, fmt, args);
    va_end(args);

    /* ���ά������� */
    pmsg_str->head.len = strlen(pmsg_str->buf);

    /* ��buf������� */
    send_msg_to_print_task(&pmsg_buf->node);
}

/***********************************************************************************
��������: print�����ͽ��յ�����Ϣ.
��ڲ���: pbuf: �洢��ӡ��Ϣ�Ļ���.
�� �� ֵ: none.
***********************************************************************************/
void print_task_send_data(BUF_NODE* pbuf)
{
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    U32 sent_len;
    U32 send_len;

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    sent_len = 0;
    send_len = pmsg_str->head.len;

    /* ���ò�ѯ�ķ�ʽ�������� */
    while(sent_len < send_len)
    {
        /* �ȴ����ڿɷ������� */
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
        {
            ;
        }

        /* �򴮿ڷ�һ���ַ� */
        USART_SendData(USART2, pmsg_str->buf[sent_len++]);
    }

    /* �ͷŴ�ӡ��Ϣ */
    buf_free_without_print(pbuf);
}

/***********************************************************************************
��������: ���ô�ӡ��Ϣ���𿪹�, ֻ�д˼������Ϣ���ر���ʱ���ܴ�ӡ�˼������Ϣ.
��ڲ���: level: ���ô�ӡ��Ϣ�ļ���.
                 PRINT_ALL: ȫ��.
                 PRINT_SUGGEST: ��ʾ.
                 PRINT_NORMAL: һ��.
                 PRINT_IMPORTANT: ��Ҫ.
�� �� ֵ: none.
***********************************************************************************/
void print_level_set(U32 level)
{
    gprint_level |= level;
}

/***********************************************************************************
��������: �����ӡ��Ϣ���𿪹�, ֻ�д˼������Ϣ���ر���ʱ���ܴ�ӡ�˼������Ϣ.
��ڲ���: level: �����ӡ��Ϣ�ļ���.
                 PRINT_ALL: ȫ��.
                 PRINT_SUGGEST: ��ʾ.
                 PRINT_NORMAL: һ��.
                 PRINT_IMPORTANT: ��Ҫ.
�� �� ֵ: none.
***********************************************************************************/
void print_level_clr(U32 level)
{
    gprint_level &= ~level;
}

/***********************************************************************************
��������: ��ȡ��ӡ��Ϣ���𿪹�, ֻ�д˼������Ϣ���ر���ʱ���ܴ�ӡ�˼������Ϣ.
��ڲ���: none.
�� �� ֵ: �����ӡ��Ϣ�ļ���.
          PRINT_ALL: ȫ��.
          PRINT_SUGGEST: ��ʾ.
          PRINT_NORMAL: һ��.
          PRINT_IMPORTANT: ��Ҫ.
***********************************************************************************/
U32 print_level_get(void)
{
    return gprint_level;
}

/***********************************************************************************
��������: ���ô�ӡ��Ϣģ�鿪��, ֻ�д�ģ�����Ϣ���ر���ʱ���ܴ�ӡ��ģ�����Ϣ.
��ڲ���: module: ���ô�ӡ��Ϣ��ģ��.
                  PRINT_ALL: ȫ��.
                  PRINT_BUF: bufģ��.
                  PRINT_COM: ͨ��ģ��.
                  PRINT_TASK: ����ģ��.
                  PRINT_RX: ����ģ��.
                  PRINT_TX: ����ģ��.
                  PRINT_SCHED: ����ģ��.
                  PRINT_PROCESS: ����ģ��.
                  PRINT_PRINT: ��ӡģ��.
�� �� ֵ: none.
***********************************************************************************/
void print_module_set(U32 module)
{
    gprint_module |= module;
}

/***********************************************************************************
��������: �����ӡ��Ϣģ�鿪��, ֻ�д�ģ�����Ϣ���ر���ʱ���ܴ�ӡ��ģ�����Ϣ.
��ڲ���: module: �����ӡ��Ϣ��ģ��.
                  PRINT_ALL: ȫ��.
                  PRINT_BUF: bufģ��.
                  PRINT_COM: ͨ��ģ��.
                  PRINT_TASK: ����ģ��.
                  PRINT_RX: ����ģ��.
                  PRINT_TX: ����ģ��.
                  PRINT_SCHED: ����ģ��.
                  PRINT_PROCESS: ����ģ��.
                  PRINT_PRINT: ��ӡģ��.
�� �� ֵ: none.
***********************************************************************************/
void print_module_clr(U32 module)
{
    gprint_module &= ~module;
}

/***********************************************************************************
��������: ��ȡ��ӡ��Ϣģ�鿪��, ֻ�д�ģ�����Ϣ���ر���ʱ���ܴ�ӡ��ģ�����Ϣ.
��ڲ���: none.
�� �� ֵ: �����ӡ��Ϣ�ļ���.
          PRINT_ALL: ȫ��.
          PRINT_BUF: bufģ��.
          PRINT_COM: ͨ��ģ��.
          PRINT_TASK: ����ģ��.
          PRINT_RX: ����ģ��.
          PRINT_TX: ����ģ��.
          PRINT_SCHED: ����ģ��.
          PRINT_PROCESS: ����ģ��.
          PRINT_PRINT: ��ӡģ��.
***********************************************************************************/
U32 print_module_get(void)
{
    return gprint_module;
}

