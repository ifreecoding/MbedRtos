
#include "mds_userroot.h"


/***********************************************************************************
��������: ������, �û���������������ʼִ��, ���ȼ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    /* ���ڳ�ʼ�� */
    uart1_init();

    /* ��ʼ�����ջ��� */
    grx_buf.len = 0;

    /* �����ź��� */
    gprx_sem = MDS_SemCreate(NULL, SEMBIN | SEMPRIO, SEMEMPTY);

    /* �������� */
    gprx_que = MDS_QueCreate(NULL, QUEPRIO);
    gptx_que = MDS_QueCreate(NULL, QUEPRIO);

    /* ����rx_thread���� */
    (void)MDS_TaskCreate(NULL, rx_thread, NULL, NULL, TASK_STASK_LEN, 2, NULL);

    /* ����handle_thread���� */
    (void)MDS_TaskCreate(NULL, handle_thread, NULL, NULL, TASK_STASK_LEN, 3, NULL);

    /* ����tx_thread���� */
    (void)MDS_TaskCreate(NULL, tx_thread, NULL, NULL, TASK_STASK_LEN, 4, NULL);
}

/***********************************************************************************
��������: ��������, CPU����ʱִ���������, ���ȼ����.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void MDS_IdleTask(void* pvPara)
{
    while(1)
    {
        ;
    }
}

