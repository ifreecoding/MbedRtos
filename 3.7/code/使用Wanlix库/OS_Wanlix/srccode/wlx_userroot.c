
#include "wlx_userroot.h"


U8 gaucRootTaskStack[ROOTTASKSTACK];    /* �������ջ */


/***********************************************************************************
��������: ������, �û���������������ʼִ��.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void WLX_RootTask(void* pvPara)
{
    /* ��ʼ����� */
    DEV_SoftwareInit();

    /* ��ʼ��Ӳ�� */
    DEV_HardwareInit();

    /* �������� */
    gpstrCheckPsgKeyTcb = WLX_TaskCreate((VFUNC)TEST_CheckPsgKeyTask, NULL,
                                        gaucCheckPsgKeyStack, TASKSTACK);
    gpstrCrossStateTcb = WLX_TaskCreate((VFUNC)TEST_CrossStateTask, NULL,
                                        gaucCrossStateStack, TASKSTACK);
    gpstrLedDisplayTcb = WLX_TaskCreate((VFUNC)TEST_LedDisplayTask, NULL,
                                        gaucLedDisplayStack, TASKSTACK);

    /* �л���������˰��������� */
    WLX_TaskSwitch(gpstrCheckPsgKeyTcb);
}

