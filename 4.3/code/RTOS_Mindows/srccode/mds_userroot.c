
#include "mds_userroot.h"


/***********************************************************************************
��������: ������, �û���������������ʼִ��, ���ȼ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    M_TASKOPT strOption;

    /* ��ʼ����� */
    DEV_SoftwareInit();

    /* ��ʼ��Ӳ�� */
    DEV_HardwareInit();

    /* ʹ��option��������ready״̬������1 */
    strOption.ucTaskSta = TASKREADY;
    (void)MDS_TaskCreate(TEST_TestTask1, NULL, gaucTask1Stack, TASKSTACK, 5,
                         &strOption);

    /* ��ʹ��option������������2 */
    (void)MDS_TaskCreate(TEST_TestTask2, NULL, gaucTask2Stack, TASKSTACK, 4, NULL);

    /* ʹ��option���������ӳ�20���delay״̬������3 */
    strOption.ucTaskSta = TASKDELAY;
    strOption.uiDelayTick = 2000;
    (void)MDS_TaskCreate(TEST_TestTask3, NULL, gaucTask3Stack, TASKSTACK, 3,
                         &strOption);

    /* ʹ��option�������������ӳٵ�delay״̬������4 */
    strOption.ucTaskSta = TASKDELAY;
    strOption.uiDelayTick = DELAYWAITFEV;
    gpstrTask4Tcb = MDS_TaskCreate(TEST_TestTask4, NULL, gaucTask4Stack, TASKSTACK,
                                   2, &strOption);

    /* �������ڴ�ӡ���� */
    (void)MDS_TaskCreate(TEST_SerialPrintTask, NULL, gaucTaskSrlStack, TASKSTACK, 6,
                         NULL);

    (void)MDS_TaskDelay(DELAYWAITFEV);
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

