
#include "test.h"


M_QUE* gpstrSerialMsgQue;               /* ���ڴ�ӡ��Ϣ����ָ�� */
M_SEM* gpstrLcdMsgSem;                  /* LCD��ӡ�ź���ָ�� */

M_TCB* gpstrSerialTaskTcb;              /* ���ڴ�ӡ����TCBָ�� */
M_TCB* gpstrLcdTaskTcb;                 /* LCD��ӡ����TCBָ�� */

M_SEM* gpstrSemSync;                    /* ��������1������2֮��ͬ�����ź��� */
M_SEM* gpstrSemMute;                    /* ��������3������4֮�以����ź��� */


/***********************************************************************************
��������: ���������л�����1, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask1(void* pvPara)
{
    U8 i;

    i = 0;

    while(1)
    {
        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask1 is running ! Tick is: %d",
                        MDS_GetSystemTick());

        /* ��������1.5�� */
        TEST_TaskRun(1500);

        /* �����ӳ�2�� */
        (void)MDS_TaskDelay(200);

        /* ǰ10��, ÿ�������ͷ�һ��gpstrSemSync�ź��� */
        if(i < 10)
        {
            i++;

            /* �����ӡ */
            DEV_PutStrToMem((U8*)"\r\nTask1 give gpstrSemSync %d! Tick is: %d",
                            i, MDS_GetSystemTick());

            /* ͬ���������� */
            (void)MDS_SemGive(gpstrSemSync);
        }
        /* ������5��, ÿ������flushһ��gpstrSemSync�ź��� */
        else if(i < 15)
        {
            i++;

            /* �����ӡ */
            DEV_PutStrToMem((U8*)"\r\nTask1 flush gpstrSemSync %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* �ͷ����б�gpstrSemSync���������� */
            (void)MDS_SemFlush(gpstrSemSync);
        }
        /* ɾ��gpstrSemSync�ź��� */
        else if(15 == i)
        {
            i++;

            /* �����ӡ */
            DEV_PutStrToMem((U8*)"\r\nTask1 delete gpstrSemSync %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* ɾ��gpstrSemSync�ź��� */
            (void)MDS_SemDelete(gpstrSemSync);
        }
    }
}

/***********************************************************************************
��������: ���������л�����2, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask2(void* pvPara)
{
    U8 i;

    i = 0;

    while(1)
    {
        /* ǰ3�λ�ȡgpstrSemMute�ź���, ��TEST_TestTask3������ */
        if(i < 3)
        {
            i++;

            /* ��ȡ���ź��������� */
            (void)MDS_SemTake(gpstrSemMute, SEMWAITFEV);

            /* �����ӡ */
            DEV_PutStrToMem((U8*)"\r\nTask2 take gpstrSemMute %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* ��������0.5�� */
            TEST_TaskRun(500);

            /* �����ӳ�2�� */
            (void)MDS_TaskDelay(200);

            /* �����ӡ */
            DEV_PutStrToMem((U8*)"\r\nTask2 give gpstrSemMute %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* �ͷ��ź���, �Ա�����������Ի�ø��ź��� */
            (void)MDS_SemGive(gpstrSemMute);
        }
        else /* ��������ȡgpstrSemSync�ź���, ��TEST_TestTask1���񼤻� */
        {
            i++;

            /* �ź�����ɾ��, ���񷵻� */
            if(RTN_SMTKDL == MDS_SemTake(gpstrSemSync, SEMWAITFEV))
            {
                /* �����ӡ */
                DEV_PutStrToMem((U8*)"\r\nTask2 gpstrSemSync deleted! Tick is: %d",
                                MDS_GetSystemTick());

                return;
            }
            else /* ��ȡ��gpstrSemSync�ź��������� */
            {
                /* �����ӡ */
                DEV_PutStrToMem((U8*)"\r\nTask2 take gpstrSemSync %d! Tick is: %d",
                                i, MDS_GetSystemTick());

                /* ��������0.5�� */
                TEST_TaskRun(500);
            }
        }
    }
}

/***********************************************************************************
��������: ���������л�����3, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask3(void* pvPara)
{
    U8 i;

    i = 0;

    while(1)
    {
        /* ǰ3�λ�ȡgpstrSemMute�ź���, ��TEST_TestTask2������ */
        if(i < 3)
        {
            i++;

            /* ��ȡ���ź��������� */
            (void)MDS_SemTake(gpstrSemMute, SEMWAITFEV);

            /* �����ӡ */
            DEV_PutStrToMem((U8*)"\r\nTask3 take gpstrSemMute %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* ��������0.5�� */
            TEST_TaskRun(500);

            /* �����ӳ�1.5�� */
            (void)MDS_TaskDelay(150);

            /* �����ӡ */
            DEV_PutStrToMem((U8*)"\r\nTask3 give gpstrSemMute %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* �ͷ��ź���, �Ա�����������Ի�ø��ź��� */
            (void)MDS_SemGive(gpstrSemMute);
        }
        else /* ��������ȡgpstrSemSync�ź���, ��TEST_TestTask1���񼤻� */
        {
            i++;

            /* �ź�����ɾ��, ���񷵻� */
            if(RTN_SMTKDL == MDS_SemTake(gpstrSemSync, SEMWAITFEV))
            {
                /* �����ӡ */
                DEV_PutStrToMem((U8*)"\r\nTask3 gpstrSemSync deleted! Tick is: %d",
                                MDS_GetSystemTick());

                return;
            }
            else /* ��ȡ��gpstrSemSync�ź��������� */
            {
                /* �����ӡ */
                DEV_PutStrToMem((U8*)"\r\nTask3 take gpstrSemSync %d! Tick is: %d",
                                i, MDS_GetSystemTick());

                /* ��������0.5�� */
                TEST_TaskRun(500);
            }
        }
    }
}

/***********************************************************************************
��������: ���������л�����4, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask4(void* pvPara)
{
    U8 i;

    i = 0;

    while(1)
    {
        /* �ź�����ɾ��, ���񷵻� */
        if(RTN_SMTKDL == MDS_SemTake(gpstrSemSync, SEMWAITFEV))
        {
            /* �����ӡ */
            DEV_PutStrToMem((U8*)"\r\nTask4 gpstrSemSync deleted! Tick is: %d",
                            MDS_GetSystemTick());

            return;
        }
        else /* ��ȡ��gpstrSemSync�ź��������� */
        {
            i++;

            /* �����ӡ */
            DEV_PutStrToMem((U8*)"\r\nTask4 take gpstrSemSync %d! Tick is: %d", i,
                            MDS_GetSystemTick());

            /* ��������0.5�� */
            TEST_TaskRun(500);
        }
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

