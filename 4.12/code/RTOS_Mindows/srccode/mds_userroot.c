
#include "mds_userroot.h"
#include "test.h"


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

    /* ����ˢ������ */
    (void)MDS_TaskCreate("FlushScn", TEST_FlushScnTask, NULL, NULL, FLUAHSCNSTK, 2, NULL);

    /* ������������ */
    (void)MDS_TaskCreate("Key", TEST_KeyTask, NULL, NULL, KEYSTK, 4, NULL);

    /* ����������Ӧ���� */
    (void)MDS_TaskCreate("Process", TEST_ProcessTask, NULL, NULL, PROSTK, 3, NULL);

    /* �������ڴ�ӡ���� */
    gpstrSerialTaskTcb = MDS_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL, NULL, PRINTSTK, 5, NULL);
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

