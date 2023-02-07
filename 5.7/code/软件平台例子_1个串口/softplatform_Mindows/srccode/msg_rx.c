
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
    USR_MSG* pusr_msg;
    U32 packet_len_in_rx_msg;
    U32 recv_len;
    U32 used_len;
    U32 rtn;
    U8 crc;

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

            /* �û���Ϣ�ṹ��ָ��ָ��洢��λ�� */
            pusr_msg = (USR_MSG*)ppacket_msg->buf;

            print_msg(PRINT_SUGGEST, PRINT_RX,
                      "\r\npacket %d bytes to %d bytes successfully. (%s, %d)",
                      msg_len, ppacket_msg->head.len, __FILE__, __LINE__);

            /* �Խ��յ����ݳ��Ƚ���У�� */
            if(RTN_SUCD != check_msg_len(ppacket_msg->head.len, pusr_msg->len))
            {
                print_msg(PRINT_NORMAL, PRINT_RX,
                          "\r\nmessage length error. (%s, %d)", __FILE__, __LINE__);

                return;
            }

            /* �Խ��յ�����У��ͽ���У��, ��������Ϣͷβ��У��� */
            crc = calc_msg_crc(&pusr_msg->s_addr,
                               USR_DATA_LEN_2_USR_CRC_LEN(pusr_msg->len));
            if(pusr_msg->crc != crc)
            {
                print_msg(PRINT_NORMAL, PRINT_RX,
                          "\r\nmessage crc error, receive 0x%x, calc 0x%x."
                          " (%s, %d)", pusr_msg->crc, crc, __FILE__, __LINE__);

                return;
            }

            /* �Խ�����Ϣ�ĵ�ַ����У�� */
            if(RTN_SUCD != check_msg_addr(pusr_msg->d_addr))
            {
                print_msg(PRINT_NORMAL, PRINT_RX,
                          "\r\nreceive message address 0x%x != local 0x%x."
                          " (%s, %d)", pusr_msg->d_addr, LOCAL_ADDR,
                          __FILE__, __LINE__);

                return;
            }

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

          ��������������, �������ݰ�֡��ʽ���. �ú������Զ�ε���, �����ջ�������
          �����ݷֶ�����. �ú���ֻ����2������²ŷ���, 1. �Ѿ����һ������, 2. ��
          Ҫ����������Ѿ�����.
          �ú�������0x7EΪ��ͷ, 0x8EΪ��β������֡��ʽ, �м������������0x7E��0x8E��
          ���������ת��:
          0x7E  <--->  0x7D 0x5D
          0x8E  <--->  0x7D 0x6E
          0x7D  <--->  0x7D 0x5D
          �����������������յ������Ҳ��ַ�����Ҫ�ָ�Ϊ��������ַ�.

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

    static RX_STA srx_status = RX_IDLE;
    U32 i;
    U8 rx_char;

    /* ������Ҫ��������� */
    for(i = 0; i < data_len; i++)
    {
        /* ��ȡ��Ҫ��������� */
        rx_char = pbuf[i];

FRAME_START:

        /* ׼����������״̬ */
        if(RX_IDLE == srx_status)
        {
            /* ���ǰ�ͷ�ַ�, ֱ�ӷ��� */
            if(0x7E != rx_char)
            {
                continue;
            }

            ppacket_buf->head.len = 0;
            ppacket_buf->buf[ppacket_buf->head.len++] = 0x7E;

            /* �ı�״̬, ��ʼ�������� */
            srx_status = RX_NORMAL;

            continue;
        }
        /* ������������״̬ */
        else if(RX_NORMAL == srx_status)
        {
            /* ���յ�ת���ַ�0x7D, ���Ľ���״̬ */
            if(0x7D == rx_char)
            {
                srx_status = RX_TRANSFER;

                continue;
            }
            /* ���յ�֡������ */
            else if(0x8E == rx_char)
            {
                /* ������״̬�ָ�Ϊ����̬ */
                srx_status = RX_IDLE;

                /* ������, ��������ð������õ����ֽڳ��� */
                *pused_len = i + 1;

                ppacket_buf->buf[ppacket_buf->head.len++] = 0x8E;

                return RTN_SUCD;
            }
            /* ���յ�֡��ʼ��, ��Ϊ����һ֡���ݵĿ�ʼ, �������������, ������� */
            else if(0x7E == rx_char)
            {
                /* ������״̬�ָ�Ϊ����̬ */
                srx_status = RX_IDLE;

                /* �������¿�ʼ�������ݵ����� */
                goto FRAME_START;
            }
            else /* ���������ַ� */
            {
                /* ����������ݳ�������������, �������������, ������� */
                if(ppacket_buf->head.len >= GET_BUF_LEN(BUF_MAX_LEN))
                {
                    /* ������״̬�ָ�Ϊ����̬ */
                    srx_status = RX_IDLE;

                    /* �������¿�ʼ�������ݵ����� */
                    goto FRAME_START;
                }

                /* �洢���յ����ַ� */
                ppacket_buf->buf[ppacket_buf->head.len++] = rx_char;

                continue;
            }
        }
        /* �ַ�ת��״̬ */
        else //if(RX_TRANSFER == rx_status)
        {
            /* ����������ݳ�������������, �������������, ������� */
            if(ppacket_buf->head.len >= GET_BUF_LEN(BUF_MAX_LEN))
            {
                /* ������״̬�ָ�Ϊ����̬ */
                srx_status = RX_IDLE;

                /* �������¿�ʼ�������ݵ����� */
                goto FRAME_START;
            }

            /* �ָ�Ϊ7E */
            if(0x5E == rx_char)
            {
                ppacket_buf->buf[ppacket_buf->head.len++] = 0x7E;
            }
            /* �ָ�Ϊ7D */
            else if(0x5D == rx_char)
            {
                ppacket_buf->buf[ppacket_buf->head.len++] = 0x7D;
            }
            /* �ָ�Ϊ8E */
            else if(0x6E == rx_char)
            {
                ppacket_buf->buf[ppacket_buf->head.len++] = 0x8E;
            }
            else /* ����, �������������, ������� */
            {
                /* ������״̬�ָ�Ϊ����̬ */
                srx_status = RX_IDLE;

                /* �������¿�ʼ�������ݵ����� */
                goto FRAME_START;
            }

            srx_status = RX_NORMAL;

            continue;
        }
    }

    /* �������õ����ֽڳ��� */
    *pused_len = i;

    /* ���ν��ջ������ڵ�������ȫ�����껹û�����һ�������� */
    return RTN_FAIL;

    /* **********������Ҫ�û�����ͨ��Э�鲹��ú���********** */
}

