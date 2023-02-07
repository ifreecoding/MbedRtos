
#include "test.h"


U8 gaucCheckPsgKeyStack[TASKSTACK]; /* ʮ��·�ڼ�����˰�������Ķ�ջ */
U8 gaucCrossStateStack[TASKSTACK];  /* ʮ��·��״̬����Ķ�ջ */
U8 gaucLedDisplayStack[TASKSTACK];  /* ʮ��·�ڵ���ʾ����Ķ�ջ */

W_TCB* gpstrCheckPsgKeyTcb;         /* ʮ��·�ڼ�����˰��������TCBָ�� */
W_TCB* gpstrCrossStateTcb;          /* ʮ��·��״̬�����TCBָ�� */
W_TCB* gpstrLedDisplayTcb;          /* ʮ��·�ڵ���ʾ�����TCBָ�� */


/* ʮ��·�ڸ���״̬ */
CROSSSTATESTR gastrCrossSta[CROSSSTATENUM] = CROSSINITVALUE;

/* ʮ��·�ڵ�ǰ״̬ */
CROSSSTATESTR gstrCurCrossSta;


U32 guiCurSta;          /* ʮ��·�ڵ�ǰ������״̬ */
U32 guiStaChange;       /*  0: ʮ��·��״̬δ�ı�, ����: ״̬�ı� */


U32 guiSystemTick;      /* ϵͳʱ�� */


/***********************************************************************************
��������: ������, C����Ӵ˺�����ʼ����. �ú�����������ϵͳ���ӷ�����״̬�л���root
          ����WLX_RootTask����.
��ڲ���: none.
�� �� ֵ: ����ֵΪ0, ��ϵͳ�������е��ú�������.
***********************************************************************************/
S32 main(void)
{
    /**************************************************************************/
    /*            �������������    www.ifreecoding.com                       */
    /*                              bbs.ifreecoding.com                       */
    /*                              blog.sina.com.cn/ifreecoding              */
    /**************************************************************************/

    /* ��ʼ������ϵͳ */
    WLX_WanlixInit(gaucRootTaskStack, ROOTTASKSTACK);
}

/***********************************************************************************
��������: ʮ��·�ڱ�����ʼ������.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_CrossVarInit(void)
{
    guiSystemTick = 0;
    guiCurSta = CROSSSTATE1;
    guiStaChange = STATECHANGE;
}

/***********************************************************************************
��������: ���ʮ��·�����˰�������.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_CheckPsgKeyTask(void* pvPara)
{
    while(1)
    {
        /* �����˰��������� */
        if(KEYNULL != DEV_ReadKey())
        {
            /* ��ʮ��·�ڸı�Ϊ���˹���·״̬ */
            if(CROSSSTATE1 == guiCurSta)
            {
                /* ״̬1�´��������ж�, ��Ϊ״̬2 */
                guiCurSta = CROSSSTATE2;

                /* ��Ϊ״̬�ı� */
                guiStaChange = STATECHANGE;
            }
            else if((CROSSSTATE3 == guiCurSta) || (CROSSSTATE4 == guiCurSta))
            {
                /* ״̬3��״̬4�´��������ж�, ��Ϊ״̬3 */
                guiCurSta = CROSSSTATE3;

                /* ��Ϊ״̬�ı� */
                guiStaChange = STATECHANGE;
            }
        }

        /* �л������Ƶ�״̬������ */
        WLX_TaskSwitch(gpstrCrossStateTcb);
    }
}

/***********************************************************************************
��������: �ı�ʮ��·�ڵ�״̬����.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_CrossStateTask(void* pvPara)
{
    U32 uiStaEndTick;
    U32 uiFlashEndTick;
    U32 i;

    uiStaEndTick = 0xFFFFFFFF;
    uiFlashEndTick = 0;

    while(1)
    {
        /* ״̬�ı� */
        if(STATEUNCHANGE != guiStaChange)
        {
            guiStaChange = STATEUNCHANGE;

            /* ��ȡ��״̬�Ĳ��� */
            gstrCurCrossSta = gastrCrossSta[guiCurSta];

            /* �����״̬����ʱ��Tick. Tick�������0���¿�ʼ */
            uiStaEndTick = guiSystemTick + gstrCurCrossSta.uiRunTime;

            /* ������˸״̬�������л���Tick. Tick�������0���¿�ʼ */
            uiFlashEndTick = guiSystemTick + FLASHTIME;
        }

        /* ��ǰ״̬ʱ�䵽, ��Ҫ�л�����һ��״̬ */
        if(uiStaEndTick == guiSystemTick)
        {
            /* ��Ϊ״̬�ı� */
            guiStaChange = STATECHANGE;

            /* Ѱ����һ��״̬ */
            guiCurSta = ++guiCurSta % CROSSSTATENUM;
        }
        else
        {
            /* ��˸״̬�������л�ʱ�䵽, ��Ҫ�л������� */
            if(uiFlashEndTick == guiSystemTick)
            {
                /* ѭ���жϸ����Ƶ���˸״̬ */
                for(i = 0; i < LEDNUM; i++)
                {
                    /* �õƲ�����˸״̬������¸��Ƶ��ж� */
                    if(LEDSTATE_FLASH != gstrCurCrossSta.astrLed[i].uiLedState)
                    {
                        continue;
                    }
                    else /* ��˸״̬ */
                    {
                        /* �ı�Ƶ�����, ����ת */
                        if(LED_DARK == gstrCurCrossSta.astrLed[i].uiBrightness)
                        {
                            gstrCurCrossSta.astrLed[i].uiBrightness = LED_LIGHT;
                        }
                        else
                        {
                            gstrCurCrossSta.astrLed[i].uiBrightness = LED_DARK;
                        }

                        /* �����¸���˸ʱ�� */
                        uiFlashEndTick = guiSystemTick + FLASHTIME;
                    }
                }
            }
        }

        /* �л������Ƶ����ȵ����� */
        WLX_TaskSwitch(gpstrLedDisplayTcb);
    }
}

/***********************************************************************************
��������: �ı�ʮ��·�ڵ���ʾ����.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_LedDisplayTask(void* pvPara)
{
    LAMPNO i;

    while(1)
    {
        /* ���ݸ����Ƶ�״̬���ƵƵ����� */
        for(i = MASTER_RED; i < LEDNUM; i++)
        {
            /* ���õƵ����� */
            DEV_LampSet(i, gstrCurCrossSta.astrLed[i].uiBrightness);
        }

        /* �л���������˰��������� */
        WLX_TaskSwitch(gpstrCheckPsgKeyTcb);
    }
}

