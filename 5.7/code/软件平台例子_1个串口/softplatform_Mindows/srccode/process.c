
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

    BUF_NODE* pbuf_rtn;
    MSG_BUF* pmsg_buf;
    MSG_BUF* pmsg_buf_rtn;
    MSG_STR* pmsg_str;
    MSG_STR* pmsg_str_rtn;
    USR_MSG* pusr_msg;

    pmsg_buf = (MSG_BUF*)pbuf;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;
    pusr_msg = (USR_MSG*)pmsg_str->buf;

    /* ����ͬ������ */
    switch(pusr_msg->cmd)
    {
        case TEST_CMD1:

            pbuf_rtn = test_cmd1(pusr_msg);

            break;

        case TEST_CMD2:

            pbuf_rtn = test_cmd2(pusr_msg);

            break;

        default:

            pbuf_rtn = NULL;
    }

    /* ����Ҫ�ظ���Ϣ */
    if(NULL == pbuf_rtn)
    {
        goto RTN;
    }

    /* �����յ�����Ϣ�ĵ�ַ */
    pmsg_buf_rtn = (MSG_BUF*)pbuf_rtn;
    pmsg_str_rtn = (MSG_STR*)pmsg_buf_rtn->buf;

    /* ���ά������� */
    pmsg_str_rtn->head.r_addr = pusr_msg->s_addr;
    pmsg_str_rtn->head.type = MSG_TYPE_COM;

    /* �����ص���Ϣ���͸�tx���� */
    send_msg_to_tx_task(pbuf_rtn);

RTN:

    /* �ͷŽ��յ���Ϣ���� */
    buf_free(pbuf);
}

/***********************************************************************************
��������: ��������1, ��LCD����ʾ�ַ���, ����10����.
��ڲ���: pbuf: ��Ҫ�������Ϣ��bufָ��.
�� �� ֵ: ���ɻظ���Ϣ�Ļ����ַ, �������Ҫ�ظ���ΪNULL.
***********************************************************************************/
BUF_NODE* test_cmd1(USR_MSG* pbuf)
{
    BUF_NODE* pbuf_node;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    USR_MSG* pusr_msg;
    U32 i;

    /* ִ������ */

    /* ����洢�ظ���Ϣ�Ļ��� */
    pbuf_node = buf_malloc(BUF_MAX_LEN);
    if(NULL == pbuf_node)
    {
        print_msg(PRINT_IMPORTANT, PRINT_PROCESS, "\r\ncan't malloc buf. (%s, %d)",
                  __FILE__, __LINE__);

        return NULL;
    }

    /* ��ʾ10���ַ��� */
    for(i = 0; i < 10; i++)
    {
        /* ��ʾ��Ϣ�е��ַ��� */
        display_string(pbuf->data, pbuf->len, i * 16, i * 8);

        MDS_TaskDelay(100);
    }

    pmsg_buf = (MSG_BUF*)pbuf_node;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;
    pusr_msg = (USR_MSG*)pmsg_str->buf;

    /* ���ظ���Ϣ */
    pusr_msg->cmd = pbuf->cmd;
    pusr_msg->len = 1;
    pusr_msg->data[0] = CMD_OK;

    return pbuf_node;
}

/***********************************************************************************
��������: ��������2, �ı�LCD����ʾ�ַ�������ɫ.
��ڲ���: pbuf: ��Ҫ�������Ϣ��bufָ��.
�� �� ֵ: ���ɻظ���Ϣ�Ļ����ַ, �������Ҫ�ظ���ΪNULL.
***********************************************************************************/
BUF_NODE* test_cmd2(USR_MSG* pbuf)
{
    BUF_NODE* pbuf_node;
    MSG_BUF* pmsg_buf;
    MSG_STR* pmsg_str;
    USR_MSG* pusr_msg;

    /* ִ������ */

    /* ����洢�ظ���Ϣ�Ļ��� */
    pbuf_node = buf_malloc(BUF_MAX_LEN);
    if(NULL == pbuf_node)
    {
        print_msg(PRINT_IMPORTANT, PRINT_PROCESS, "\r\ncan't malloc buf. (%s, %d)",
                  __FILE__, __LINE__);

        return NULL;
    }

    /* ���û�����ɫ */
    set_pen_color(pbuf->data[0] | (U16)pbuf->data[1] << 8);

    pmsg_buf = (MSG_BUF*)pbuf_node;
    pmsg_str = (MSG_STR*)pmsg_buf->buf;
    pusr_msg = (USR_MSG*)pmsg_str->buf;

    /* ���ظ���Ϣ */
    pusr_msg->cmd = pbuf->cmd;
    pusr_msg->len = 1;
    pusr_msg->data[0] = CMD_OK;

    return pbuf_node;
}

