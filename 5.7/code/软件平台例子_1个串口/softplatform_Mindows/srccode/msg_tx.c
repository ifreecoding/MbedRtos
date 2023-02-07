
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
    BUF_NODE* pbuf_fill;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;

    /* tx���������Ϣ */
    pbuf = tx_task_receive_msg();
    if(NULL == pbuf)
    {
        return;
    }

    /* **********������Ҫ�û�����ͨ��Э�鲹��ú���********** */

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    /* ͨ����Ϣ��Ҫ������䴦�� */
    if(MSG_TYPE_COM == pmsg_str->head.type)
    {
        /* �����Ҫ���͵����ݰ� */
        if(NULL == (pbuf_fill = fill_tx_msg(pbuf)))
        {
            return;
        }
    }
    else
    {
        pbuf_fill = pbuf;
    }

    /* tx���������� */
    tx_task_send_data(pbuf_fill);

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
    U32 print_level;
    U32 print_module;

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

    if(MSG_TYPE_PRT == pmsg_str->head.type)
    {
        /* ����buf_free����ǰ���δ�ӡ����, ��ֹ��buf_free�����ڵ���print_msg�����γ�
           ѭ����ӡ */
        print_level = print_level_get();
        print_module = print_module_get();
        print_level_clr(print_level);
        print_module_clr(print_module);

        /* �ͷŴ����Ϣ�Ļ��� */
        buf_free(pbuf);

        /* ����ԭ�еĴ�ӡ�����ģ�� */
        print_level_set(print_level);
        print_module_set(print_module);
    }
    else
    {
        /* �ͷŴ����Ϣ�Ļ��� */
        buf_free(pbuf);
    }
}

/***********************************************************************************
��������: ��䷢�͵���Ϣ.
��ڲ���: pbuf: ��Ҫ���͵���Ϣ��bufָ��.
�� �� ֵ: �������Ϣ����, ��ΪNULL���ʾ���ʧ��.
***********************************************************************************/
BUF_NODE* fill_tx_msg(BUF_NODE* pbuf)
{
    BUF_NODE* pbuf_node_tran;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    USR_MSG* pusr_msg;

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;
    pusr_msg = (USR_MSG*)pmsg_str->buf;

    /* ����û�����Ϣ */
    pusr_msg->head = 0x7E;                          /* ��Ϣͷ */
    pusr_msg->s_addr = LOCAL_ADDR;                  /* Դ��ַ */
    pusr_msg->d_addr = pmsg_str->head.r_addr;       /* Ŀ�ĵ�ַ */
    pusr_msg->data[pusr_msg->len] = 0x8E;           /* ��Ϣβ */

    /* ���У��� */
    pusr_msg->crc = calc_msg_crc(&pusr_msg->s_addr,
                                 USR_DATA_LEN_2_USR_CRC_LEN(pusr_msg->len));

    /* ����Ҫ���͵���Ϣ����ת�� */
    pbuf_node_tran = tx_message_transfer(pusr_msg);

    /* �ͷ���Ϣ���� */
    buf_free(pbuf);

    return pbuf_node_tran;
}

/***********************************************************************************
��������: �Է��͵����ݽ���ת��, ��Ϣ�г��ֵ������ַ������¹���ת��: 7D->7D 5D,
          7E->7D 5E, 8E->7D 6E.
��ڲ���: pbuf: ת��ǰ����Ϣ����.
�� �� ֵ: ת������Ϣ����, ��ΪNULL���ʾת��ʧ��.
***********************************************************************************/
BUF_NODE* tx_message_transfer(USR_MSG* pbuf)
{
    BUF_NODE* pbuf_node;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    U8* pin_buf;
    U8* pout_buf;
    TX_STA tx_status;
    U32 in_len;
    U32 in_cnt;
    U32 out_cnt;

    /* ����洢ת����Ϣ�Ļ��� */
    pbuf_node = buf_malloc(BUF_MAX_LEN);
    if(NULL == pbuf_node)
    {
        print_msg(PRINT_IMPORTANT, PRINT_TX, "\r\ncan't malloc buf. (%s, %d)",
                  __FILE__, __LINE__);

        return NULL;
    }

    pmsg_buf = (MSG_BUF*)pbuf_node;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;

    /* ��Ҫת�������ݵ�ָ�� */
    pin_buf = (U8*)pbuf;

    /* ת���������ݵ�ָ�� */
    pout_buf = pmsg_str->buf;

    in_cnt = 0;
    out_cnt = 0;

    /* ��Ҫת�������ݳ��� */
    in_len = USR_DATA_LEN_2_USR_TOTAL_LEN(pbuf->len);

    /* ��ʼ��ת��״̬ */
    tx_status = TX_7E;

    /* ת��������Ϣ */
    while(in_cnt < in_len - 1)
    {
        /* ת������ݳ���, ����ʧ�� */
        if(out_cnt >= GET_USR_BUF_LEN(BUF_MAX_LEN))
        {
            goto RTN;
        }

        if(TX_NORMAL == tx_status)
        {
            /* ��Ҫת�巢��7E */
            if(0x7E == pin_buf[in_cnt])
            {
                /* �ȷ���7D */
                pout_buf[out_cnt++] = 0x7D;

                tx_status = TX_TRANSFER_7E;
            }
            /* ��Ҫת�巢��7D */
            else if(0x7D == pin_buf[in_cnt])
            {
                /* �ȷ���7D */
                pout_buf[out_cnt++] = 0x7D;

                tx_status = TX_TRANSFER_7D;
            }
            /* ��Ҫת�巢��8E */
            else if(0x8E == pin_buf[in_cnt])
            {
                /* �ȷ���7D */
                pout_buf[out_cnt++] = 0x7D;

                tx_status = TX_TRANSFER_8E;
            }
            else /* ������ͨ�ַ� */
            {
                pout_buf[out_cnt++] = pin_buf[in_cnt++];
            }
        }
        else if(TX_7E == tx_status)
        {
            /* �ȷ���7Eͷ */
            pout_buf[out_cnt++] = 0x7E;
            in_cnt++;

            tx_status = TX_NORMAL;
        }
        else if(TX_TRANSFER_7E == tx_status)
        {
            /* 7Eת��, ����5Eת���ַ� */
            pout_buf[out_cnt++] = 0x5E;
            in_cnt++;

            tx_status = TX_NORMAL;
        }
        else if(TX_TRANSFER_7D == tx_status)
        {
            /* 7Dת��, ����5Dת���ַ� */
            pout_buf[out_cnt++] = 0x5D;
            in_cnt++;

            tx_status = TX_NORMAL;
        }
        else if(TX_TRANSFER_8E == tx_status)
        {
            /* 8Eת��, ����5Eת���ַ� */
            pout_buf[out_cnt++] = 0x6E;
            in_cnt++;

            tx_status = TX_NORMAL;
        }
    }

    /* ת������ݳ���, ����ʧ�� */
    if(out_cnt >= GET_USR_BUF_LEN(BUF_MAX_LEN))
    {
        goto RTN;
    }

    /* �����Ϣβ */
    pout_buf[out_cnt++] = 0x8E;
    in_cnt++;

    print_msg(PRINT_SUGGEST, PRINT_TX, "\r\n%d data transfer to %d data. (%s, %d)",
              in_cnt, out_cnt, __FILE__, __LINE__);

    /* ���ά������� */
    pmsg_str->head.len = out_cnt;
    pmsg_str->head.type = MSG_TYPE_COM;

    return pbuf_node;

RTN:

    /* �ͷ�ת����Ϣ���� */
    buf_free(pbuf_node);

    pbuf_node = NULL;

    return pbuf_node;
}

