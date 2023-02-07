
#include "test.h"


M_QUE gstrSerialMsgQue;                 /* ���ڴ�ӡ��Ϣ����ָ�� */

M_TCB* gpstrSerialTaskTcb;              /* ���ڴ�ӡ����TCBָ�� */

U8 gaucTaskSrlStack[TASKSTACK];         /* ���ڴ�ӡ�����ջ */
U8 gaucTask1Stack[TASKSTACK];           /* ����1��ջ */
U8 gaucTask2Stack[TASKSTACK];           /* ����2��ջ */
U8 gaucTask3Stack[TASKSTACK];           /* ����3��ջ */
U8 gaucTask4Stack[TASKSTACK];           /* ����4��ջ */

M_TCB* gpstrTask4Tcb;                   /* task4����TCBָ�� */


/***********************************************************************************
��������: ���������л�����1, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask1(void* pvPara)
{
    while(1)
    {
        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask1 is running! Tick is: %d",
                        MDS_GetSystemTick());

        /* ��������1�� */
        TEST_TaskRun(1000);

        /* �����ӳ�1�� */
        (void)MDS_TaskDelay(100);
    }
}

/***********************************************************************************
��������: ���������л�����2, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask2(void* pvPara)
{
    while(1)
    {
        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask2 is running! Tick is: %d",
                        MDS_GetSystemTick());

        /* ��������2�� */
        TEST_TaskRun(2000);

        /* �����ӳ�1.5�� */
        (void)MDS_TaskDelay(150);
    }
}

/***********************************************************************************
��������: ���������л�����3, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask3(void* pvPara)
{
    /* �����ӡ */
    DEV_PutStrToMem((U8*)"\r\nTask3 is running! Tick is: %d", MDS_GetSystemTick());

    /* ����task4 */
    (void)MDS_TaskWake(gpstrTask4Tcb);

    while(1)
    {
        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask3 is running! Tick is: %d",
                        MDS_GetSystemTick());

        /* ��������5�� */
        TEST_TaskRun(5000);

        /* �����ӳ�5�� */
        (void)MDS_TaskDelay(500);
    }
}

/***********************************************************************************
��������: ���������л�����4, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask4(void* pvPara)
{
    while(1)
    {
        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask4 is running! Tick is: %d",
                        MDS_GetSystemTick());

        /* ��������1�� */
        TEST_TaskRun(1000);

        /* �����ӳ�10�� */
        (void)MDS_TaskDelay(1000);
    }
}

/***********************************************************************************
��������: ���ڴ�ӡ����, �Ӷ��л�ȡ��Ҫ��ӡ����Ϣ����, �������еĴ�ӡ���ݴ�ӡ������.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_SerialPrintTask(void* pvPara)
{
    M_DLIST* pstrMsgQueNode;
    MSGBUF* pstrMsgBuf;

    /* �Ӷ���ѭ����ȡ��Ϣ */
    while(1)
    {
        /* �Ӷ����л�ȡ��һ����Ҫ��ӡ����Ϣ, �򴮿ڴ�ӡ��Ϣ���� */
        if(RTN_SUCD == MDS_QueGet(&gstrSerialMsgQue, &pstrMsgQueNode))
        {
            pstrMsgBuf = (MSGBUF*)pstrMsgQueNode;

            /* �������е����ݴ�ӡ������ */
            DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

            /* ������Ϣ�е����ݷ������, �ͷŻ��� */
            DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
        }
        else /* û�л�ȡ����Ϣ, �ӳ�һ��ʱ����ٲ�ѯ���� */
        {
            (void)MDS_TaskDelay(100);
        }
    }
}

/***********************************************************************************
��������: ģ���������к���, ��������������ʱ��.
��ڲ���: uiMs: Ҫ�ӳٵ�ʱ��, ��λms.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TaskRun(U32 uiMs)
{
    /* ���ӳ�ʱ��ģ����������ʱ�� */
    DEV_DelayMs(uiMs);
}

