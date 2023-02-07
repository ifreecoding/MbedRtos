
#include "process.h"


/***********************************************************************************
��������: ����process���񲢽��г�ʼ��.
��ڲ���: ptask: ����ṹָ��..
�� �� ֵ: RTN_SUCD: �ɹ�.
          RTN_FAIL: ʧ��.
***********************************************************************************/
U32 create_process_task(TASK_STR* ptask)
{
    /* ����process_task���� */
    ptask->ptcb = MDS_TaskCreate("process_task", msg_process_task, ptask, NULL,
                                 PROCESS_TASK_STACK, PROCESS_TASK_PRIO, NULL);
    if(NULL == ptask->ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_PROCESS,
                  "\r\ncreate process_task failed. (%s, %d)", __FILE__, __LINE__);

        return RTN_FAIL;
    }

    /* ��ʼ������Ķ��� */
    if(NULL == (ptask->pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_PROCESS,
                  "\r\nprocess_task queue init failed. (%s, %d)",
                  __FILE__, __LINE__);

        return RTN_FAIL;
    }

    print_msg(PRINT_SUGGEST, PRINT_PROCESS,
              "\r\nprocess_task init finished. (%s, %d)", __FILE__, __LINE__);

    return RTN_SUCD;
}

/***********************************************************************************
��������: ��Ϣ��������, �Ӷ��л�ȡ��Ϣ���д���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void msg_process_task(void* pvPara)
{
    while(1)
    {
        /* ������Ϣ */
        msg_process((TASK_STR*)pvPara);
    }
}

/***********************************************************************************
��������: ������յ�����Ϣ.
��ڲ���: ptask: ����ṹָ��.
�� �� ֵ: none.
***********************************************************************************/
void msg_process(TASK_STR* ptask)
{
    BUF_NODE* pbuf;

    /* process���������Ϣ */
    pbuf = process_task_receive_msg(ptask);
    if(NULL == pbuf)
    {
        return;
    }

    /* ����һ����Ϣ */
    process_one_msg(pbuf);

    /* ��Ϣ�������, ��process�����ͷŵ������ */
    process_task_free(ptask);
}

/***********************************************************************************
��������: ��process��������Ϣ.
��ڲ���: ptask: ����ṹָ��.
          pbuf: ������Ϣ��buf.
�� �� ֵ: none.
***********************************************************************************/
void send_msg_to_process_task(TASK_STR* ptask, BUF_NODE* pbuf)
{
    /* ����Ϣ������� */
    if(RTN_SUCD != MDS_QuePut(ptask->pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\n%s put message to process_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
��������: process���������Ϣ.
��ڲ���: ptask: ����ṹָ��.
�� �� ֵ: ���յ�����Ϣ��bufָ��, ���ΪNULL�������.
***********************************************************************************/
BUF_NODE* process_task_receive_msg(TASK_STR* ptask)
{
    M_DLIST* pbuf;

    /* �Ӷ����л�ȡ��Ϣ */
    if(RTN_SUCD != MDS_QueGet(ptask->pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\n%s get message from process_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
��������: ����һ����Ϣ.
��ڲ���: pbuf: ��Ҫ�������Ϣ��bufָ��.
�� �� ֵ: none.
***********************************************************************************/
void process_one_msg(BUF_NODE* pbuf)
{
    /* **********�û���Ҫ�������������޸ĸú���********** */


    /* ���û�������Ϣ�Ĵ������ */

    /* ���û�������Ҫ���͵���Ϣ */

    /* �����ɵ���Ϣ���͸�tx���� */
    //send_msg_to_tx_task();

    /* �ͷŽ��յ���Ϣ���� */
    buf_free(pbuf);
}

