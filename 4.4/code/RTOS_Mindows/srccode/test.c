
#include "test.h"


M_QUE gstrSerialMsgQue;                 /* ���ڴ�ӡ��Ϣ����ָ�� */

M_TCB* gpstrSerialTaskTcb;              /* ���ڴ�ӡ����TCBָ�� */
M_TCB* gpstrLcdTaskTcb;                 /* LCD��ӡ����TCBָ�� */

U8 gaucTaskSrlStack[TASKSTACK];         /* ���ڴ�ӡ�����ջ */
U8 gaucTaskLcdStack[LCDTASKSTACK];      /* LCD��ӡ�����ջ */
U8 gaucTask1Stack[TASKSTACK];           /* ����1��ջ */
U8 gaucTask2Stack[TASKSTACK];           /* ����2��ջ */
U8 gaucTask3Stack[TASKSTACK];           /* ����3��ջ */


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
    while(1)
    {
        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask3 is running! Tick is: %d",
                        MDS_GetSystemTick());

        /* ��������3�� */
        TEST_TaskRun(3000);

        /* �����ӳ�7�� */
        (void)MDS_TaskDelay(700);
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
��������: LCD��ӡ����, ���ܻ�ȡ���ź����������л���Ϣ��ӡ��LCD��ʾ��.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_LcdPrintTask(void* pvPara)
{
    /* ѭ����ȡ�ź��� */
    while(1)
    {
        /* �������л���Ϣ��ӡ��LCD��Ļ */
        DEV_PrintTaskSwitchMsgToLcd();

        /* �ӳ�һ��ʱ����ٴ�ӡLCD */
        (void)MDS_TaskDelay(100);
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

/***********************************************************************************
��������: �������л�������Ϣ��ӡ���ڴ���.
��ڲ���: pstrOldTcb: �л�ǰ������TCBָ��.
          pstrNewTcb: �л��������TCBָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TaskSwitchPrint(M_TCB* pstrOldTcb, M_TCB* pstrNewTcb)
{
    /* Ϊ����ӡ���ڴ�ӡ�����LCD��ӡ������л�����, ����2��������Ϊ�ǿ������� */
    if((pstrOldTcb == gpstrSerialTaskTcb) || (pstrOldTcb == gpstrLcdTaskTcb))
    {
        pstrOldTcb = MDS_GetIdleTcb();
    }

    if((pstrNewTcb == gpstrSerialTaskTcb) || (pstrNewTcb == gpstrLcdTaskTcb))
    {
        pstrNewTcb = MDS_GetIdleTcb();
    }

    /* ͬһ������֮���л�����ӡ��Ϣ */
    if(pstrOldTcb == pstrNewTcb)
    {
        return;
    }

    /* ���ڴ��ӡ�����л���Ϣ */
    DEV_PutStrToMem((U8*)"\r\nTask %s ---> Task %s! Tick is: %d",
                    pstrOldTcb->pucTaskName, pstrNewTcb->pucTaskName,
                    MDS_GetSystemTick());

    /* �洢�����л���Ϣ */
    DEV_SaveTaskSwitchMsg(pstrNewTcb->pucTaskName);
}

