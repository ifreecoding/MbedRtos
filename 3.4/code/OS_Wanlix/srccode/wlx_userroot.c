
#include "wlx_userroot.h"


/***********************************************************************************
��������: ������, �û���������������ʼִ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void WLX_RootTask(void)
{
    /* ��ʼ����� */
    DEV_SoftwareInit();

    /* ��ʼ��Ӳ�� */
    DEV_HardwareInit();

    /* �������� */
    gpstrTask1Tcb = WLX_TaskCreate(TEST_TestTask1, gaucTask1Stack, TASKSTACK);
    gpstrTask2Tcb = WLX_TaskCreate(TEST_TestTask2, gaucTask2Stack, TASKSTACK);
    gpstrTask3Tcb = WLX_TaskCreate(TEST_TestTask3, gaucTask3Stack, TASKSTACK);

    /* �����л� */
    WLX_TaskSwitch(gpstrTask1Tcb);
}

