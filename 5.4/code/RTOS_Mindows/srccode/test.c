
#include "test.h"


M_QUE* gpstrSerialMsgQue;               /* ���ڴ�ӡ��Ϣ����ָ�� */
M_SEM* gpstrLcdMsgSem;                  /* LCD��ӡ�ź���ָ�� */

M_TCB* gpstrSerialTaskTcb;              /* ���ڴ�ӡ����TCBָ�� */
M_TCB* gpstrLcdTaskTcb;                 /* LCD��ӡ����TCBָ�� */


/***********************************************************************************
��������: ���������л�����1, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask1(void* pvPara)
{
    U32 uiStackRemainLen;
    U32 uiTick;

    /* ��ӡջ��Ϣ */
    uiStackRemainLen = MDS_TaskStackCheck(MDS_GetCurrentTcb());
    DEV_PutStrToMem((U8*)"\r\nTask1 stack remain %d bytes, Tick is: %d",
                    uiStackRemainLen, MDS_GetSystemTick());

    while(1)
    {
        /* ��������0.5�� */
        TEST_TaskRun(500);

        /* ��ȡϵͳʱ�� */
        uiTick = MDS_GetSystemTick();

        /* ϵͳ����ʱ����10~20��֮�� */
        if((uiTick >= 1000) && (uiTick < 2000))
        {
            /* ��ӡ������ֵ */
            DEV_PutStrToMem((U8*)"\r\nTask1 1 add to 40 value is %d, Tick is: %d",
                            TEST_Add(40), MDS_GetSystemTick());
        }
        /* ϵͳ����ʱ����20~30��֮�� */
        else if((uiTick >= 2000) && (uiTick < 3000))
        {
            /* ��ӡ������ֵ */
            DEV_PutStrToMem((U8*)"\r\nTask1 1 add to 60 value is %d, Tick is: %d",
                            TEST_Add(60), MDS_GetSystemTick());
        }
        /* ϵͳ����ʱ�䳬��30�� */
        else if(uiTick >= 3000)
        {
            /* ��ӡ������ֵ */
            DEV_PutStrToMem((U8*)"\r\nTask1 1 add to 80 value is %d, Tick is: %d",
                            TEST_Add(80), MDS_GetSystemTick());
        }

        /* �����ӳ�2�� */
        (void)MDS_TaskDelay(200);

        /* ��ӡջ��Ϣ */
        uiStackRemainLen = MDS_TaskStackCheck(MDS_GetCurrentTcb());
        DEV_PutStrToMem((U8*)"\r\nTask1 stack remain %d bytes, Tick is: %d",
                        uiStackRemainLen, MDS_GetSystemTick());
    }
}

/***********************************************************************************
��������: ���������л�����2, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask2(void* pvPara)
{
    U32 uiStackRemainLen;
    U32 uiTick;

    /* ��ӡջ��Ϣ */
    uiStackRemainLen = MDS_TaskStackCheck(MDS_GetCurrentTcb());
    DEV_PutStrToMem((U8*)"\r\nTask2 stack remain %d bytes, Tick is: %d",
                    uiStackRemainLen, MDS_GetSystemTick());

    while(1)
    {
        /* ��������0.5�� */
        TEST_TaskRun(500);

        /* ��ȡϵͳʱ�� */
        uiTick = MDS_GetSystemTick();

        /* ϵͳ����ʱ����10~20��֮�� */
        if((uiTick >= 1000) && (uiTick < 2000))
        {
            /* ��ӡ������ֵ */
            DEV_PutStrToMem((U8*)"\r\nTask2 1 add to 80 value is %d, Tick is: %d",
                            TEST_Add(80), MDS_GetSystemTick());
        }
        /* ϵͳ����ʱ����20~30��֮�� */
        else if((uiTick >= 2000) && (uiTick < 3000))
        {
            /* ��ӡ������ֵ */
            DEV_PutStrToMem((U8*)"\r\nTask2 1 add to 60 value is %d, Tick is: %d",
                            TEST_Add(60), MDS_GetSystemTick());
        }
        /* ϵͳ����ʱ�䳬��30�� */
        else  if(uiTick >= 3000)
        {
            /* ��ӡ������ֵ */
            DEV_PutStrToMem((U8*)"\r\nTask2 1 add to 40 value is %d, Tick is: %d",
                            TEST_Add(40), MDS_GetSystemTick());
        }

        /* �����ӳ�2�� */
        (void)MDS_TaskDelay(200);

        /* ��ӡջ��Ϣ */
        uiStackRemainLen = MDS_TaskStackCheck(MDS_GetCurrentTcb());
        DEV_PutStrToMem((U8*)"\r\nTask2 stack remain %d bytes, Tick is: %d",
                        uiStackRemainLen, MDS_GetSystemTick());
    }
}

/***********************************************************************************
��������: ���������л�����3, ���ڴ��ӡ�ַ���.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_TestTask3(void* pvPara)
{
    U32 uiStackRemainLen;
    U32 uiTick;

    /* ��ӡջ��Ϣ */
    uiStackRemainLen = MDS_TaskStackCheck(MDS_GetCurrentTcb());
    DEV_PutStrToMem((U8*)"\r\nTask3 stack remain %d bytes, Tick is: %d",
                    uiStackRemainLen, MDS_GetSystemTick());

    while(1)
    {
        /* ��������0.5�� */
        TEST_TaskRun(500);

        /* ��ȡϵͳʱ�� */
        uiTick = MDS_GetSystemTick();

        /* ϵͳ����ʱ�䳬��10�� */
        if(uiTick >= 1000)
        {
            /* ��ӡ������ֵ */
            DEV_PutStrToMem((U8*)"\r\nTask3 1 add to 60 value is %d, Tick is: %d",
                            TEST_Add(60), MDS_GetSystemTick());

            /* ��ӡ������ֵ */
            DEV_PutStrToMem((U8*)"\r\nTask3 1 add to 80 value is %d, Tick is: %d",
                            TEST_Add(80), MDS_GetSystemTick());

            /* ��ӡ������ֵ */
            DEV_PutStrToMem((U8*)"\r\nTask3 1 add to 40 value is %d, Tick is: %d",
                            TEST_Add(40), MDS_GetSystemTick());
        }

        /* �����ӳ�2�� */
        (void)MDS_TaskDelay(200);

        /* ��ӡջ��Ϣ */
        uiStackRemainLen = MDS_TaskStackCheck(MDS_GetCurrentTcb());
        DEV_PutStrToMem((U8*)"\r\nTask3 stack remain %d bytes, Tick is: %d",
                        uiStackRemainLen, MDS_GetSystemTick());
    }
}

/***********************************************************************************
��������: ���Ժ���, ���õݹ���ü����ۼӺ�.
��ڲ���: none.
�� �� ֵ: ������.
***********************************************************************************/
U32 TEST_Add(U32 uiNum)
{
    if(1 == uiNum)
    {
        return 1;
    }

    return uiNum + TEST_Add(uiNum - 1);
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
        /* �Ӷ����л�ȡ��һ����Ҫ��ӡ����Ϣ, �򴮿ڴ�ӡ��Ϣ����, ����ȡ����������Ϣ
           ��������ڶ����Ͻ���pend״̬, �ȴ�������Ϣ����������±�Ϊready̬ */
        if(RTN_SUCD == MDS_QueGet(gpstrSerialMsgQue, &pstrMsgQueNode, QUEWAITFEV))
        {
            pstrMsgBuf = (MSGBUF*)pstrMsgQueNode;

            /* �������е����ݴ�ӡ������ */
            DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

            /* ������Ϣ�е����ݷ������, �ͷŻ��� */
            DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
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

