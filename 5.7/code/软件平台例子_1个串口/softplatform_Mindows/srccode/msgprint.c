
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "msgprint.h"


U32 gprint_level;
U32 gprint_module;


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
    pmsg_str->head.type = MSG_TYPE_PRT;

    /* ��buf������� */
    send_msg_to_tx_task(&pmsg_buf->node);
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

