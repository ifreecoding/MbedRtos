
#include "msg_rx.h"


TASK_STR grx_task;              /* rx����ṹ */


/***********************************************************************************
��������: ����rx���񲢽��г�ʼ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void create_rx_task(void)
{
    /* ����rx���� */
    grx_task.ptcb = MDS_TaskCreate("rx_task", msg_rx_task, NULL, NULL,
                                   RX_TASK_STACK, RX_TASK_PRIO, NULL);
    if(NULL == grx_task.ptcb)
    {
        print_msg(PRINT_IMPORTANT, PRINT_RX, "\r\ncreate rx_task failed. (%s, %d)",
                  __FILE__, __LINE__);
    }

    /* ��ʼ������Ķ��� */
    if(NULL == (grx_task.pque = MDS_QueCreate(NULL, QUEFIFO)))
    {
        print_msg(PRINT_IMPORTANT, PRINT_RX,
                  "\r\nrx_task queue init failed. (%s, %d)", __FILE__, __LINE__);
    }

    print_msg(PRINT_SUGGEST, PRINT_RX, "\r\nrx_task init finished. (%s, %d)",
              __FILE__, __LINE__);
}

/***********************************************************************************
��������: ��Ϣ��������, �ӻ��ν��ջ������л�ȡ���ݽ������, ����ð�������ͨ�����з�
          �͸�sched������д���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void msg_rx_task(void* pvPara)
{
    while(1)
    {
        /* �������� */
        msg_rx();
    }
}

/***********************************************************************************
��������: ������Ϣ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void msg_rx(void)
{
    static MSG_BUF* psrx_buf = NULL;
    static MSG_BUF* pspacket_buf = NULL;
    static U32 msg_len = 0;
    MSG_STR* ppacket_msg;
    U32 packet_len_in_rx_msg;
    U32 recv_len;
    U32 used_len;
    U32 rtn;

    /* ��û�л����ڽ������������뻺��, �����ͷ�, ѭ������ */
    if(NULL == psrx_buf)
    {
        /* ����������ݵĻ��� */
        psrx_buf = (MSG_BUF*)buf_malloc(BUF_MAX_LEN);
        if(NULL == psrx_buf)
        {
            print_msg(PRINT_IMPORTANT, PRINT_RX, "\r\ncan't malloc buf. (%s, %d)",
                      __FILE__, __LINE__);

            return;
        }
    }

    /* ��û�л�����������������뻺�� */
    if(NULL == pspacket_buf)
    {
        /* ����������ݵĻ��� */
        pspacket_buf = (MSG_BUF*)buf_malloc(BUF_MAX_LEN);
        if(NULL == pspacket_buf)
        {
            print_msg(PRINT_IMPORTANT, PRINT_RX, "\r\ncan't malloc buf. (%s, %d)",
                      __FILE__, __LINE__);

            return;
        }
    }

    /* ��ȡ���յ������� */
    recv_len = data_receive(psrx_buf->buf, BUF_MAX_LEN);

    packet_len_in_rx_msg = 0;

    /* ���յ������ݿ��ܻ���ɶ�֡����, ��Ҫѭ����� */
    while(0 != recv_len)
    {
        /* ����Ϣ�ṹָ����Ϣ���� */
        ppacket_msg = (MSG_STR*)pspacket_buf->buf;

        /* �Խ��յ����ݽ������, �ú�����Ҫ�û�����ͨ��Э����и�д */
        rtn = rx_message_packet(psrx_buf->buf + packet_len_in_rx_msg, ppacket_msg,
                                recv_len, &used_len);
        msg_len += used_len;                /* ������ĳ��� */
        packet_len_in_rx_msg += used_len;   /* ���յ�������������ĳ��� */
        recv_len -= used_len;               /* ��ȥ�Ѿ���ȥ���ֽ��� */

        /* һ�����ݽ������ */
        if(RTN_SUCD == rtn)
        {
            /* **********������Ҫ�û�����ͨ��Э�鲹��ú���********** */





            /* **********������Ҫ�û�����ͨ��Э�鲹��ú���********** */

            /* �������Ϣ�Ļ��������з��͸�sched���� */
            send_msg_to_sched_task(&pspacket_buf->node);

            /* ����������� */
            msg_len = 0;

            /* �������������� */
            if(0 == recv_len)
            {
                /* ���������ΪNULL, �´ε��øú���ʱ���뻺�� */
                pspacket_buf = NULL;
            }
            /* ����ʣ����ֽ�û�����, ����Ҫ�����µ��ڴ濪ʼ��� */
            else
            {
                print_msg(PRINT_NORMAL, PRINT_RX,
                          "\r\nthere are still %d byte need packet. (%s, %d)",
                          recv_len, __FILE__, __LINE__);

                /* ����������ݴ�ŵĻ��� */
                pspacket_buf = (MSG_BUF*)buf_malloc(BUF_MAX_LEN);
                if(NULL == pspacket_buf)
                {
                    print_msg(PRINT_IMPORTANT, PRINT_RX,
                              "\r\ncan't malloc buf. (%s, %d)", __FILE__, __LINE__);

                    return;
                }
            }
        }
    }
}

/***********************************************************************************
��������: ��rx��������Ϣ.
��ڲ���: pbuf: ������Ϣ��buf.
�� �� ֵ: none.
***********************************************************************************/
void send_msg_to_rx_task(BUF_NODE* pbuf)
{
    /* ����Ϣ������� */
    if(RTN_SUCD != MDS_QuePut(grx_task.pque, &pbuf->list))
    {
        print_msg(PRINT_IMPORTANT, PRINT_RX,
                  "\r\n%s put message to rx_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return;
    }
}

/***********************************************************************************
��������: rx���������Ϣ.
��ڲ���: none.
�� �� ֵ: ���յ�����Ϣ��bufָ��, ���ΪNULL�������.
***********************************************************************************/
BUF_NODE* rx_task_receive_msg(void)
{
    M_DLIST* pbuf;

    /* �Ӷ����л�ȡ��Ϣ */
    if(RTN_SUCD != MDS_QueGet(grx_task.pque, &pbuf, QUEWAITFEV))
    {
        print_msg(PRINT_IMPORTANT, PRINT_RX,
                  "\r\n%s get message from rx_task queue failed. (%s, %d)",
                  MDS_GetCurrentTcb()->pucTaskName, __FILE__, __LINE__);

        return NULL;
    }

    return (BUF_NODE*)pbuf;
}

/***********************************************************************************
��������: �Խ��յ������ݽ������, ������ð�����Ϣ���͸��������. �ú������м��书��
          ���ظ�����, ÿ�μ����ϴ���������ݼ������.
��ڲ���: pbuf: ��Ҫ������������ڵĻ���.
          ppacket_buf: ��������Ϣ��ŵĻ���.
          data_len: ��Ҫ��������ݳ���.
          pused_len: ��������Ѿ��õ������ݳ���.
�� �� ֵ: RTN_SUCD: ��Ϣ������, ����һ��������Ϣ, �ɷ������������.
          RTN_FAIL: ��Ϣδ������, δ����һ��������Ϣ, ��Ҫ��������.
***********************************************************************************/
U32 rx_message_packet(U8* pbuf, MSG_STR* ppacket_buf, U32 data_len, U32* pused_len)
{
    /* **********������Ҫ�û�����ͨ��Э�鲹��ú���********** */

    *pused_len = 0;

    return RTN_SUCD;

    /* **********������Ҫ�û�����ͨ��Э�鲹��ú���********** */
}

