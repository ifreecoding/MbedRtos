
#include "msg_tx.h"


TASK_STR gtx_task;              /* tx����ṹ */


/***********************************************************************************
��������: ����tx���񲢽��г�ʼ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void create_tx_task(void)
{
    /* ����tx���� */
    gtx_task.ptcb = MDS_TaskCreate("tx_task", msg_tx_task, NULL, NULL,
                                   TX_TASK_STACK, TX_TASK_PRIO, NULL);
    if(NULL == gtx_task.ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX, "\r\ncreate tx_task failed. (%s, %d)",
                  __FILE__, __LINE__);
    }

    /* ��ʼ������Ķ��� */
    if(NULL == (gtx_task.pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\ntx_task queue init failed. (%s, %d)", __FILE__, __LINE__);
    }

    print_msg(PRINT_SUGGEST, PRINT_TX, "\r\ntx_task init finished. (%s, %d)",
              __FILE__, __LINE__);
}

/***********************************************************************************
��������: ��Ϣ��������, ����Ҫ�������ݲ�������͵����η��ͻ�������, ��Ӳ�������η���
          �������ڵ����ݷ��ͳ�ȥ.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void msg_tx_task(void* pvPara)
{
    while(1)
    {
        /* �������� */
        msg_tx();
    }
}

/***********************************************************************************
��������: ������Ϣ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_tx(void)
{
    BUF_NODE* pbuf;

    /* tx���������Ϣ */
    pbuf = tx_task_receive_msg();
    if(NULL == pbuf)
    {
        return;
    }

    /* **********������Ҫ�û�����ͨ��Э�鲹��ú���********** */



    /* tx���������� */
    tx_task_send_data(pbuf);

    /* **********������Ҫ�û�����ͨ��Э�鲹��ú���********** */
}

/***********************************************************************************
��������: ��tx��������Ϣ.
��ڲ���: pbuf: ������Ϣ��buf.
�� �� ֵ: none.
***********************************************************************************/
void send_msg_to_tx_task(BUF_NODE* pbuf)
{
    /* ����Ϣ������� */
    if(RTN_SUCD != MDS_QuePut(gtx_task.pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\n%s put message to tx_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
��������: tx���������Ϣ.
��ڲ���: none.
�� �� ֵ: ���յ�����Ϣ��bufָ��, ���ΪNULL�������.
***********************************************************************************/
BUF_NODE* tx_task_receive_msg(void)
{
    M_DLIST* pbuf;

    /* �Ӷ����л�ȡ��Ϣ */
    if(RTN_SUCD != MDS_QueGet(gtx_task.pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX,
                  "\r\n%s get message from tx_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
��������: tx�����ͽ��յ�����Ϣ.
��ڲ���: pbuf: ��Ҫ���͵���Ϣ��bufָ��.
�� �� ֵ: none.
***********************************************************************************/
void tx_task_send_data(BUF_NODE* pbuf)
{
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    U32 sent_len;
    U32 send_len;
    U32 len;

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    sent_len = 0;
    send_len = pmsg_str->head.len;

    /* �������һ�η���������������ѭ���������� */
    while(sent_len < send_len)
    {
        /* �������� */
        len = data_send(pmsg_str->buf + sent_len, send_len - sent_len);
        sent_len += len;
    }

    /* �ͷŴ����Ϣ�Ļ��� */
    buf_free(pbuf);
}

