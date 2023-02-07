
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
    while(1)
    {
        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask1 will run 2s! Tick is: %d",
                        MDS_GetSystemTick());

        /* ��������2�� */
        TEST_TaskRun(2000);

        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask1 will delay 5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* �����ӳ�5�� */
        (void)MDS_TaskDelay(500);

        /* ����70����˳������� */
        if(MDS_GetSystemTick() > 7000)
        {
            break;
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
    while(1)
    {
        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask2 will run 0.5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* ��������0.5�� */
        TEST_TaskRun(500);

        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask2 will delay 0.5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* �����ӳ�2�� */
        (void)MDS_TaskDelay(200);

        /* ����50����˳������� */
        if(MDS_GetSystemTick() > 5000)
        {
            break;
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
    while(1)
    {
        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask3 will run 0.5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* ��������0.2�� */
        TEST_TaskRun(200);

        /* �����ӡ */
        DEV_PutStrToMem((U8*)"\r\nTask3 will delay 0.5s! Tick is: %d",
                        MDS_GetSystemTick());

        /* �����ӳ�0.4�� */
        (void)MDS_TaskDelay(40);

        /* ����30����˳������� */
        if(MDS_GetSystemTick() > 3000)
        {
            break;
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
��������: LCD��ӡ����, ÿ���ӡһ��CPUռ����.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_LcdPrintTask(void* pvPara)
{
    while(1)
    {
        /* ��ȡ���ź������ӡCPUռ���� */
        if(RTN_SUCD == MDS_SemTake(gpstrLcdMsgSem, SEMWAITFEV))
        {
            /* ��CPUռ������Ϣ��ӡ��LCD��Ļ */
            DEV_PrintCpuShareMsgToLcd();
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
    static U32 suiLastTime = 0;
    U32 uiTime;

    /* ��ȡ��ǰʱ�� */
    uiTime = MDS_GetSystemTick() / 100;

    /* ����1��ʱ�� */
    if(uiTime != suiLastTime)
    {
        /* ����һ���ź���, ��ʾCPUռ���� */
        (void)MDS_SemGive(gpstrLcdMsgSem);

        /* ����ʱ�� */
        suiLastTime = uiTime;
    }
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

