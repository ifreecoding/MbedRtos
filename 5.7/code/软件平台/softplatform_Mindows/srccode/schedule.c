
#include "schedule.h"


TASK_STR gsched_task;           /* sched����ṹ */


/***********************************************************************************
��������: ����sched���񲢽��г�ʼ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void create_sched_task(void)
{
    /* ����sched_task���� */
    gsched_task.ptcb = MDS_TaskCreate("sched_task", msg_sched_task, NULL, NULL,
                                      SCHED_TASK_STACK, SCHED_TASK_PRIO, NULL);
    if(NULL == gsched_task.ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\ncreate sched_task failed. (%s, %d)", __FILE__, __LINE__);
    }

    /* ��ʼ������Ķ��� */
    if(NULL == (gsched_task.pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\nsched_task queue init failed. (%s, %d)", __FILE__, __LINE__);
    }

    print_msg(PRINT_SUGGEST, PRINT_SCHED, "\r\nsched_task init finished. (%s, %d)",
              __FILE__, __LINE__);
}

/***********************************************************************************
��������: ��Ϣ��������, ��������ȳ�������һ�����е���Ϣ������������Ϣ.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void msg_sched_task(void* pvPara)
{
    while(1)
    {
        /* ������Ϣ */
        msg_dispatch();
    }
}

/***********************************************************************************
��������: �����յ�����Ϣ�ַ�����ͬ��������д���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_dispatch(void)
{
    BUF_NODE* pbuf;

    /* �Ӷ��л�ȡҪ�������Ϣ */
    pbuf = sched_task_receive_msg();
    if(NULL == pbuf)
    {
        return;
    }

    /* sched������е���, ��������ִ�н��յ�����Ϣ */
    sched_task_schedule(pbuf);
}

/***********************************************************************************
��������: ��sched��������Ϣ.
��ڲ���: pbuf: ������Ϣ��buf.
�� �� ֵ: none.
***********************************************************************************/
void send_msg_to_sched_task(BUF_NODE* pbuf)
{
    /* ����Ϣ������� */
    if(RTN_SUCD != MDS_QuePut(gsched_task.pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\n%s put message to sched_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
��������: sched���������Ϣ.
��ڲ���: none.
�� �� ֵ: ���յ�����Ϣ��bufָ��, ���ΪNULL�������.
***********************************************************************************/
BUF_NODE* sched_task_receive_msg(void)
{
    M_DLIST* pbuf;

    /* �Ӷ����л�ȡ��Ϣ */
    if(RTN_SUCD != MDS_QueGet(gsched_task.pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\n%s get message from sched_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
��������: sched�������, ��������.
��ڲ���: pbuf: ��Ҫ���ȵ���Ϣ��bufָ��.
�� �� ֵ: none.
***********************************************************************************/
void sched_task_schedule(BUF_NODE* pbuf)
{
    /* **********�û����Ը���������Ҫ�޸ĸú���********** */

    TASK_STR* ptask;

    /* ���������������õ����� */
    ptask = process_task_malloc();
    if(NULL == ptask)
    {
        print_msg(PRINT_IMPORTANT, PRINT_SCHED,
                  "\r\ncan't malloc process_task, free message buf 0x%x. (%s, %d)",
                  pbuf, __FILE__, __LINE__);

        goto RTN;
    }

    /* ����Ϣת����process���� */
    send_msg_to_process_task(ptask, pbuf);

    return;

    /* ������, �ͷŴ�Ž�����Ϣ�Ļ��� */
RTN:

    buf_free(pbuf);
}

