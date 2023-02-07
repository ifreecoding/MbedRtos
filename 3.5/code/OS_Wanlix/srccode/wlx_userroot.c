
#include "wlx_userroot.h"


/***********************************************************************************
��������: ������, �û���������������ʼִ��.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void WLX_RootTask(void* pvPara)
{
    U32 uiRunTime1;
    U32 uiRunTime2;
    U32 uiRunTime3;

    /* ��ʼ����� */
    DEV_SoftwareInit();

    /* ��ʼ��Ӳ�� */
    DEV_HardwareInit();

    /* �������� */
    uiRunTime1 = 1000;
    gpstrTask1Tcb = WLX_TaskCreate(TEST_TestTask1, (void*)&uiRunTime1,
                                   gaucTask1Stack, TASKSTACK);

    uiRunTime2 = 2000;
    gpstrTask2Tcb = WLX_TaskCreate(TEST_TestTask2, (void*)&uiRunTime2,
                                   gaucTask2Stack, TASKSTACK);

    uiRunTime3 = 3000;
    gpstrTask3Tcb = WLX_TaskCreate(TEST_TestTask3, (void*)&uiRunTime3,
                                   gaucTask3Stack, TASKSTACK);

    /* �����л� */
    WLX_TaskSwitch(gpstrTask1Tcb);
}

