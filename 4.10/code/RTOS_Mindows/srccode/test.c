
#include "test.h"


M_QUE* gpstrSerialMsgQue;               /* ���ڴ�ӡ��Ϣ����ָ�� */
M_SEM* gpstrLcdMsgSem;                  /* LCD��ӡ�ź���ָ�� */

M_TCB* gpstrSerialTaskTcb;              /* ���ڴ�ӡ����TCBָ�� */
M_TCB* gpstrLcdTaskTcb;                 /* LCD��ӡ����TCBָ�� */

M_SEM* gpstrSemMut;                 /* �����ź��� */


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

        /* ִ�в��Ժ��� */
        TEST_Test1();
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

        /* ִ�в��Ժ��� */
        TEST_Test3();
    }
}

/***********************************************************************************
��������: ���Ժ���1, ��ȡ/�ͷŻ����ź���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_Test1(void)
{
    /* ��ȡ���ź��������� */
    (void)MDS_SemTake(gpstrSemMut, SEMWAITFEV);

    /* �����ӡ */
    DEV_PutStrToMem((U8*)"\r\nT1 is running! Tick is: %d", MDS_GetSystemTick());

    /* ��������1.5�� */
    TEST_TaskRun(1500);

    /* ִ�в��Ժ��� */
    TEST_Test2();

    /* �����ӳ�1�� */
    (void)MDS_TaskDelay(100);

    /* �ͷ��ź��� */
    (void)MDS_SemGive(gpstrSemMut);

    /* �����ӳ�1�� */
    (void)MDS_TaskDelay(100);
}

/***********************************************************************************
��������: ���Ժ���2, ��ȡ/�ͷŻ����ź���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_Test2(void)
{
    /* ��ȡ���ź��������� */
    (void)MDS_SemTake(gpstrSemMut, SEMWAITFEV);

    /* �����ӡ */
    DEV_PutStrToMem((U8*)"\r\nT2 is running! Tick is: %d", MDS_GetSystemTick());

    /* ��������0.5�� */
    TEST_TaskRun(500);

    /* �����ӳ�2�� */
    (void)MDS_TaskDelay(200);

    /* �ͷ��ź��� */
    (void)MDS_SemGive(gpstrSemMut);

    /* �����ӳ�3�� */
    (void)MDS_TaskDelay(300);
}

/***********************************************************************************
��������: ���Ժ���3, ��ȡ/�ͷŻ����ź���.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_Test3(void)
{
    /* ��ȡ���ź��������� */
    (void)MDS_SemTake(gpstrSemMut, SEMWAITFEV);

    /* �����ӡ */
    DEV_PutStrToMem((U8*)"\r\nT3 is running! Tick is: %d", MDS_GetSystemTick());

    /* ��������0.5�� */
    TEST_TaskRun(500);

    /* �����ӳ�2�� */
    (void)MDS_TaskDelay(200);

    /* �ͷ��ź��� */
    (void)MDS_SemGive(gpstrSemMut);

    /* �����ӳ�2�� */
    (void)MDS_TaskDelay(200);
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
        if(RTN_SUCD == MDS_QueGet(gpstrSerialMsgQue, &pstrMsgQueNode))
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
        /* ��ȡ�ź���, ��LCD��ӡ�����л�ͼ������ȡ�����ź�������������ź����Ͻ���
           pend״̬, �ȴ��ź������ٴ��ͷ����±�Ϊready̬ */
        if(RTN_SUCD == MDS_SemTake(gpstrLcdMsgSem, SEMWAITFEV))
        {
            /* �������л���Ϣ��ӡ��LCD��Ļ */
            DEV_PrintTaskSwitchMsgToLcd();
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

/***********************************************************************************
��������: �����񴴽�������Ϣ��ӡ���ڴ���.
��ڲ���: pstrTcb: �´����������TCBָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TaskCreatePrint(M_TCB* pstrTcb)
{
    /* ���񴴽��ɹ� */
    if((M_TCB*)NULL != pstrTcb)
    {
        /* ��ӡ���񴴽��ɹ���Ϣ */
        DEV_PutStrToMem((U8*)"\r\nTask %s is created! Tick is: %d",
                        pstrTcb->pucTaskName, MDS_GetSystemTick());

        /* ����Ǵ�����������洢��һ����������������л���Ϣ */
        if(MDS_GetRootTcb() == pstrTcb)
        {
            /* �洢�����л���Ϣ */
            DEV_SaveTaskSwitchMsg(pstrTcb->pucTaskName);
        }
    }
    else /* ���񴴽�ʧ�� */
    {
        /* ��ӡ���񴴽�ʧ����Ϣ */
        DEV_PutStrToMem((U8*)"\r\nFail to create task! Tick is: %d",
                        MDS_GetSystemTick());
    }
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

    /* û��ɾ���л�ǰ���� */
    if(NULL != pstrOldTcb)
    {
        /* ���ڴ��ӡ�����л���Ϣ */
        DEV_PutStrToMem((U8*)"\r\nTask %s ---> Task %s! Tick is: %d",
                        pstrOldTcb->pucTaskName, pstrNewTcb->pucTaskName,
                        MDS_GetSystemTick());
    }
    else /* �л�ǰ������ɾ�� */
    {
        /* ���ڴ��ӡ�����л���Ϣ */
        DEV_PutStrToMem((U8*)"\r\nTask NULL ---> Task %s! Tick is: %d",
                        pstrNewTcb->pucTaskName, MDS_GetSystemTick());
    }

    /* �洢�����л���Ϣ */
    DEV_SaveTaskSwitchMsg(pstrNewTcb->pucTaskName);
}

/***********************************************************************************
��������: ������ɾ��������Ϣ��ӡ���ڴ���.
��ڲ���: pstrTcb: ��ɾ���������TCBָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TaskDeletePrint(M_TCB* pstrTcb)
{
    /* ��ӡ����ɾ����Ϣ */
    DEV_PutStrToMem((U8*)"\r\nTask %s is deleted! Tick is: %d",
                    pstrTcb->pucTaskName, MDS_GetSystemTick());
}

