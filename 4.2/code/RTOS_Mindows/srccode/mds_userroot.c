
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

    /* �����ӡ */
    DEV_PutString((U8*)"\r\nRootTask is running!");

    /* ��������1�� */
    DEV_DelayMs(1000);

    /* �������� */
    (void)MDS_TaskCreate(TEST_TestTask1, NULL, gaucTask1Stack, TASKSTACK, 4);

    DEV_DelayMs(1000);

    (void)MDS_TaskCreate(TEST_TestTask2, NULL, gaucTask2Stack, TASKSTACK, 3);

    DEV_DelayMs(1000);

    (void)MDS_TaskCreate(TEST_TestTask3, NULL, gaucTask3Stack, TASKSTACK, 1);

    DEV_DelayMs(1000);
}

