
#include "mds_userroot.h"


/***********************************************************************************
��������: ������, �û���������������ʼִ��, ���ȼ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void MDS_RootTask(void)
{
    /* ��ʼ����� */
    DEV_SoftwareInit();

    /* ��ʼ��Ӳ�� */
    DEV_HardwareInit();

    /* ��������1 */
    (void)MDS_TaskCreate("Test1", TEST_TestTask1, NULL, NULL, TASKSTACK, 5, NULL);

    /* ��������2 */
    (void)MDS_TaskCreate("Test2", TEST_TestTask2, NULL, NULL, TASKSTACK, 4, NULL);

    /* ��������3 */
    (void)MDS_TaskCreate("Test3", TEST_TestTask3, NULL, NULL, TASKSTACK, 3, NULL);

    /* ��������4 */
    (void)MDS_TaskCreate("Test4", TEST_TestTask4, NULL, NULL, TASKSTACK, 2, NULL);

    /* �������ڴ�ӡ���� */
    gpstrSerialTaskTcb = MDS_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL, NULL,
                                        TASKSTACK, 6, NULL);

    /* ����LCD��ӡ���� */
    gpstrLcdTaskTcb = MDS_TaskCreate("LcdPrt", TEST_LcdPrintTask, NULL, NULL,
                                     LCDTASKSTACK, 7, NULL);
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

