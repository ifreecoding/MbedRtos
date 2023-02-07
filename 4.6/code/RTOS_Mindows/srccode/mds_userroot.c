
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
    (void)MDS_TaskCreate("Test1", TEST_TestTask1, NULL, gaucTask1Stack, TASKSTACK,
                         4, &strOption);

    /* ��ʹ��option������������2 */
    (void)MDS_TaskCreate("Test2", TEST_TestTask2, NULL, gaucTask2Stack, TASKSTACK,
                         3, NULL);

    /* ʹ��option���������ӳ�20���delay״̬������3 */
    strOption.ucTaskSta = TASKDELAY;
    strOption.uiDelayTick = 2000;
    (void)MDS_TaskCreate("Test3", TEST_TestTask3, NULL, gaucTask3Stack, TASKSTACK,
                         2, &strOption);

    /* �������ڴ�ӡ���� */
    gpstrSerialTaskTcb = MDS_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL,
                                        gaucTaskSrlStack, TASKSTACK, 5, NULL);

    /* ����LCD��ӡ���� */
    gpstrLcdTaskTcb = MDS_TaskCreate("LcdPrt", TEST_LcdPrintTask, NULL,
                                     gaucTaskLcdStack, LCDTASKSTACK, 6, NULL);

    (void)MDS_TaskDelay(10000);

    /* ϵͳ����100���root�����Զ����� */

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

