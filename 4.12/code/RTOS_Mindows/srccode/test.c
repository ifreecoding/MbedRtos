
#include "test.h"


M_QUE* gpstrSerialMsgQue;               /* ���ڴ�ӡ��Ϣ����ָ�� */
M_QUE* gpstrKeyMsgQue;                  /* ������Ϣ�Ķ���ָ�� */

M_TCB* gpstrSerialTaskTcb;              /* ���ڴ�ӡ����TCBָ�� */

M_SEM* gpstrFigSem;                     /* Lcd��ʾͼ�ε��ź���ָ��, ͨ�����ź���ʵ�ֶ�
                                           Lcd��ʾͼ�εĴ��з��� */

U8 gucGameSta;                          /* ��Ϸ״̬ */

FIGSTR gstrCurFig;                      /* ��ǰͼ�νṹ */
FIGRAM gstrNextFig;                     /* �¸�ͼ�νṹ */

/* �����Lcd Ram������Lcd��Ļ�Ķ�Ӧ��ϵΪ: һά����Ԫ�ض�ӦLcd��Ļ��X��, ��ά����Ԫ
   �ض�ӦLcd��Y��, Ҳ������ԭ���ӦLcd��Ļ�����Ͻ�����ԭ��, һά����Ԫ�ص����������
   ӦLcd��Ļ���ҵ�X����������, ��ά����Ԫ�ص����������ӦLcd��Ļ���µ�Y���������� */
/* �����������ʾ��ͼ�ε�Ram */
U16 gausMainPrtRam[MAINSCN_X_LEN][MAINSCN_Y_LEN];

/* ͼ����ʾ�����ض��� */
const U8 gcaucBox_Fig[] = PIXEL_BOX_FIG;
const U8 gcaucI_Fig_0_180[] = PIXEL_I_FIG_0_180;
const U8 gcaucI_Fig_90_270[] = PIXEL_I_FIG_90_270;
const U8 gcaucMountain_Fig_0[] = PIXEL_MOUNTAIN_FIG_0;
const U8 gcaucMountain_Fig_90[] = PIXEL_MOUNTAIN_FIG_90;
const U8 gcaucMountain_Fig_180[] = PIXEL_MOUNTAIN_FIG_180;
const U8 gcaucMountain_Fig_270[] = PIXEL_MOUNTAIN_FIG_270;
const U8 gcaucZ_Fig_0_180[] = PIXEL_Z_FIG_0_180;
const U8 gcaucZ_Fig_90_270[] = PIXEL_Z_FIG_90_270;
const U8 gcaucZ_Reverse_Fig_0_180[] = PIXEL_Z_REVERSE_FIG_0_180;
const U8 gcaucZ_Reverse_Fig_90_270[] = PIXEL_Z_REVERSE_FIG_90_270;
const U8 gcaucL_Fig_0[] = PIXEL_L_FIG_0;
const U8 gcaucL_Fig_90[] = PIXEL_L_FIG_90;
const U8 gcaucL_Fig_180[] = PIXEL_L_FIG180;
const U8 gcaucL_Fig_270[] = PIXEL_L_FIG270;
const U8 gcaucL_Reverse_Fig_0[] = PIXEL_L_REVERSE_FIG_0;
const U8 gcaucL_Reverse_Fig_90[] = PIXEL_L_REVERSE_FIG_90;
const U8 gcaucL_Reverse_Fig_180[] = PIXEL_L_REVERSE_FIG_180;
const U8 gcaucL_Reverse_Fig_270[] = PIXEL_L_REVERSE_FIG_270;


/***********************************************************************************
��������: ˢ������. ��������10Hz��Ƶ��ˢ��Lcd��Ļ.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_FlushScnTask(void* pvPara)
{
    /* ��ʼ����Ļ */
    TEST_ScreenInit();

    while(1)
    {
        /* ��ȡͼ���ź���, ��ֹ��������ͬʱ��ͼ�ν��в��� */
        (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

        /* ˢ�������� */
        TEST_ImageDraw(MAINSCR_MIN_X, MAINSCR_MIN_Y, MAINSCN_X_LEN, MAINSCN_Y_LEN, (U16*)gausMainPrtRam);

        /* ˢ���¸�ͼ�δ��� */
        TEST_ImageDraw(NEXTFIG_MIN_X, NEXTFIG_MIN_Y, NEXTFIG_X_LEN, NEXTFIG_Y_LEN, (U16*)&gstrNextFig.ausFigRam);

        /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
        (void)MDS_SemGive(gpstrFigSem);

        (void)MDS_TaskDelay(10);
    }
}

/***********************************************************************************
��������: ��������. ��������10Hz��Ƶ�ʶ�ȡ������Ӧ��GPIO, �ж��Ƿ��а�������.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_KeyTask(void* pvPara)
{
    STRKEYMSGQUE* pstrKeyMsg = NULL;
    U16 usAutoDownCnt = 0;
    U16 usNotActKeyCnt = 0;
    U8 ucKey;
    U8 ucKeyNum;

    while(1)
    {
        /* ����buf, ������Ű�����Ϣ */
        if(NULL == pstrKeyMsg)
        {
            pstrKeyMsg = (STRKEYMSGQUE*)DEV_BufferAlloc(&gstrBufPool);
            if(NULL == pstrKeyMsg)
            {
                return;
            }
        }

        /* ���������ʱ�� */
        (void)MDS_TaskDelay(10);

        /* �����㰴���ȴ�ʱ��, ����Ϊ��û�а������� */
        if(0 != usNotActKeyCnt)
        {
            usNotActKeyCnt--;

            ucKey = KEYNULL;
            ucKeyNum = 0;
        }
        else /* ��Ҫ��ⰴ�� */
        {
            /* ��ȡ����ֵ������ͬʱ���°��������� */
            ucKey = DEV_ReadKey();
            ucKeyNum = TEST_KeyPressCheck(ucKey);
        }

        /* ֻ��һ������������ʱ����Ӧ���� */
        if(1 == ucKeyNum)
        {
            /* �а�����Ӧ, 200ms֮������ٴμ�ⰴ��ֵ */
            usNotActKeyCnt = NOTACTKEYTIME;

            /* �����Ϸ����GAMESTA_STOP״̬��ֻ��ӦRestart���� */
            if(GAMESTA_STOP == gucGameSta)
            {
                if(KEYRESTART == ucKey)
                {
                    /* ���ü���ֵ */
                    usNotActKeyCnt = 0;
                    usAutoDownCnt = 0;

                    /* �������¿�ʼ����ֵ */
                    pstrKeyMsg->uiKey = KEYRESTART;
                }
            }
            /* �����Ϸ����GAMESTA_DROP״̬����Ӧ���� */
            else if(GAMESTA_DROP == gucGameSta)
            {
                /* ��������°����������Զ����µİ������� */
                if(KEYDOWN == ucKey)
                {
                    usAutoDownCnt = 0;
                }

                /* ���밴��ֵ */
                pstrKeyMsg->uiKey = ucKey;
            }
        }
        /* û�а��������»��߶������ͬʱ�����µ������û�а������µ�������� */
        else
        {
            /* �����Ϸ������GAMESTA_DROP״̬��ͼ�β��Զ��� */
            if(GAMESTA_DROP != gucGameSta)
            {
                continue;
            }

            /* û�а�����Ӧ, 400ms֮������ٴμ�ⰴ��ֵ */
            if(usAutoDownCnt < AUTODOWNTIME)
            {
                usAutoDownCnt++;

                /* �����㰴��������, �˳�����ѭ�� */
                continue;
            }
            else
            {
                /* �ﵽ����������, ���¼��� */
                usAutoDownCnt = 0;
            }

            /* ��Process����һ�����°�������Ϣ */
            pstrKeyMsg->uiKey = KEYDOWN;
        }

        /* ��Process����һ�����¿�ʼ��������Ϣ */
        (void)MDS_QuePut(gpstrKeyMsgQue, &pstrKeyMsg->strQueHead);
        pstrKeyMsg = NULL;
    }
}

/***********************************************************************************
��������: ������Ӧ����. �������ɰ������񴥷�, ��������İ���������Ӧ����Ӧ.
��ڲ���: pvPara: ������ڲ���ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_ProcessTask(void* pvPara)
{
    STRKEYMSGQUE* pstrKeyMsg;
    M_DLIST* pstrList;

    while(1)
    {
        /* �д��������Ķ��� */
        if(RTN_SUCD == MDS_QueGet(gpstrKeyMsgQue, &pstrList, QUEWAITFEV))
        {
            /* ��ȡ��Ϣ�еİ�����Ϣ */
            pstrKeyMsg = (STRKEYMSGQUE*)pstrList;

            /* ���ݰ�����Ϣ����Ӧ�Ĵ��� */
            switch(pstrKeyMsg->uiKey)
            {
                case KEYUP:

                    TEST_KeyUpProcess();

                    break;

                case KEYDOWN:

                    TEST_KeyDownProcess();

                    break;

                case KEYLEFT:

                    TEST_KeyLeftProcess();

                    break;

                case KEYRIGHT:

                    TEST_KeyRightProcess();

                    break;

                case KEYRESTART:

                    TEST_KeyRestartProcess();

                    break;

                default :

                    break;
            }

            /* �ͷŶ�����Ϣ���� */
            DEV_BufferFree(&gstrBufPool, &pstrKeyMsg->strQueHead);
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
��������: ��Ļ��ʼ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_ScreenInit(void)
{
    /* ��ʼ����������ʾ����������ͼ�ε��ڴ� */
    TEST_MainScnRamInit();

    /* ��ʼ�������ڱ���ɫ */
    TEST_ImageDraw(MAINSCR_MIN_X, MAINSCR_MIN_Y, MAINSCN_X_LEN, MAINSCN_Y_LEN, (U16*)gausMainPrtRam);

    /* ��ʼ���ָ��ߺ͸����ڱ���ɫ, ���������ڵ�Ram */
    TEST_ImageDraw(LINE_MIN_X, SLAVESCR_MIN_Y, SLAVESCN_X_LEN + LINE_X_LEN, SLAVESCN_Y_LEN, (U16*)gausMainPrtRam);

    /* ������Ϊ��ɫ */
    LCDSETPENCOLOR(LCDCOLORWHITE);

    /* ��ʾ�ָ��� */
    DEV_LcdDrawBox(LINE_MIN_X * PIXELAMP, LINE_MIN_Y * PIXELAMP, LINE_X_LEN * 2, LINE_Y_LEN * PIXELAMP);
}

/***********************************************************************************
��������: ���ڴ��е����ػ���Lcd��, ÿ��ͼ�����ض�ӦPIXELAMPƽ������ʵ������.
��ڲ���: ucX: X����С����.
          ucY: Y����С����.
          ucXLen: X�᳤��.
          ucYLen: Y�᳤��.
          pusRam: �ڴ���ʼ��ַ.
�� �� ֵ: none.
***********************************************************************************/
void TEST_ImageDraw(U8 ucX, U8 ucY, U8 ucXLen, U8 ucYLen, U16* pusRam)
{
    U8 x;
    U8 y;

    /* ���ÿ��ͼ������ */
    for(x = 0; x < ucXLen; x++)
    {
        for(y = 0; y < ucYLen; y++)
        {
            /* ���û�����ɫ */
            LCDSETPENCOLOR(pusRam[x * ucYLen + y]);

            /* ��ͼ�����ػ�����Ӧ��ʵ�������� */
            DEV_LcdDrawBox(PIXELAMP * (ucX + x), PIXELAMP * (ucY + y), PIXELAMP, PIXELAMP);
        }
    }
}

/***********************************************************************************
��������: ��Ϸ��ʼ, ��ʼ����Ϸ����.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_GameStart(void)
{
    /* ��ȡͼ���ź���, ��ֹ��������ͬʱ��ͼ�ν��в��� */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* ��ʼ����Ļ */
    TEST_ScreenInit();

    /* ��ʼ���¸���ʾͼ�εĽṹ */
    TEST_NextFigStructInit();

    /* ��ʼ����ǰ��ʾͼ�εĽṹ */
    (void)TEST_CurFigStructInit();

    /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
    (void)MDS_SemGive(gpstrFigSem);

    /* ��Ϸ��ʼ, �����¸��µ�ͼ�� */
    TEST_NextFigStructInit();

    /* ��ʼ����Ϸ״̬ */
    gucGameSta = GAMESTA_DROP;
}

/***********************************************************************************
��������: ��һ��ͼ�ο�ʼ, ����������Ѿ��޷���������ͼ���򷵻�ʧ��.
��ڲ���: none.
�� �� ֵ: RTN_SUCD: ��һ��ͼ�ο�����ʾ����������.
          RTN_FAIL: ����������, �޷���ʾ��ͼ��.
***********************************************************************************/
U32 TEST_NextFigStart(void)
{
    /* ��ȡͼ���ź���, ��ֹ��������ͬʱ��ͼ�ν��в��� */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* ���¸���ʾͼ�α�Ϊ��ǰ��ʾͼ�� */
    if(RTN_FAIL == TEST_CurFigStructInit())
    {
        /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
        (void)MDS_SemGive(gpstrFigSem);

        return RTN_FAIL;
    }

    /* ����ѡ���¸���ʾͼ�� */
    TEST_NextFigStructInit();

    /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
    (void)MDS_SemGive(gpstrFigSem);

    /* ���³�ʼ����Ϸ״̬ */
    gucGameSta = GAMESTA_DROP;

    return RTN_SUCD;
}

/***********************************************************************************
��������: ��ʼ��Led��ʾ�������ڶ�Ӧ��Ram.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_MainScnRamInit(void)
{
    U8 x;
    U8 y;

    /* ���ñ���ɫΪ��ɫ */
    LCDSETBACKGROUNDCOLOR(LCDCOLORBLACK);

    /* ��ʼ����������ʾ��ͼ�ε��ڴ� */
    for(x = 0; x < MAINSCN_X_LEN; x++)
    {
        for(y = 0; y < MAINSCN_Y_LEN; y++)
        {
            /* ��ʼ��Ϊ����ɫ */
            gausMainPrtRam[x][y] = LCDGETBACKGROUNDCOLOR;
        }
    }
}

/***********************************************************************************
��������: ��ʼ��������ʾͼ�εĽṹ.
��ڲ���: none.
�� �� ֵ: RTN_SUCD: ��һ��ͼ�ο�����ʾ����������.
          RTN_FAIL: ����������, �޷���ʾ��ͼ��.
***********************************************************************************/
U32 TEST_CurFigStructInit(void)
{
    /* ��ʼ��ͼ�νṹ */
    gstrCurFig.strRam.usFigNo = gstrNextFig.usFigNo;    /* ͼ�α�� */
    gstrCurFig.strRam.usRtt = gstrNextFig.usRtt;        /* ͼ����ת�ĽǶ� */
    gstrCurFig.strRam.usClr = gstrNextFig.usClr;        /* ͼ����ɫ */

    /* ��ʼ��ͼ���������� */
    TEST_InitFigPos(&gstrCurFig);

    /* ��䵱ǰͼ�ε�Ram */
    TEST_MoveFigRamInit(&gstrCurFig.strRam);

    /* ����ǰͼ�θ��Ƶ��������� */
    if(RTN_FAIL == TEST_CopyCurFigToMainScn())
    {
        /* �޷�����, ��Ϸʧ�� */
        return RTN_FAIL;
    }

    return RTN_SUCD;
}

/***********************************************************************************
��������: ��ʼ��ͼ�ε�����.
��ڲ���: pstrFig: ͼ�νṹָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_InitFigPos(FIGSTR* pstrFig)
{
    U8* pucFig;

    /* ���ƶ�ͼ�γ�ʼֵ���ڵ�Ram��ַ */
    pucFig = TEST_GetFigPixelPointer(&pstrFig->strRam);

    /* ��ʼ��ͼ�ε�λ�� */
    pstrFig->strPos.scLTX = pucFig[0];
    pstrFig->strPos.scLTY = pucFig[1];
    pstrFig->strPos.scRBX = pucFig[0] + pucFig[2] - 1;
    pstrFig->strPos.scRBY = pucFig[1] + pucFig[3] - 1;
}

/***********************************************************************************
��������: ��ȡ��ǰͼ��ͼ����ת���λ������.
��ڲ���: pstrOldFig: ͼ����תǰ�Ľṹָ��.
          pstrNewFig: ͼ����ת��Ľṹָ��.
�� �� ֵ: RTN_SUCD: �����ɹ�.
          RTN_FAIL: ����ʧ��.
***********************************************************************************/
U32 TEST_GetCurFigPosAfterRtt(FIGSTR* pstrOldFig, FIGSTR* pstrNewFig)
{
    pstrNewFig->strRam.usFigNo = pstrOldFig->strRam.usFigNo;
    pstrNewFig->strRam.usClr = pstrOldFig->strRam.usClr;

    /* ����ͼ�β���Ҫ��ת */
    if(BOX_FIG == pstrOldFig->strRam.usFigNo)
    {
        pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
        pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
        pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
        pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;

        return RTN_SUCD;
    }

    switch(pstrOldFig->strRam.usFigNo)
    {
        case I_FIG:

            if(ROTATION_0 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX - 2;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY + 2;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX + 2;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY - 2;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX + 2;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY - 2;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 2;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 2;
            }

            break;

        case MOUNTAIN_FIG:

            if(ROTATION_0 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX + 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 1;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_180;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX - 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY + 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else if(ROTATION_180 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_270;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY - 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else //if(ROTATION_270 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX + 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY - 1;
            }

            break;

        case Z_FIG:
        case Z_REVERSE_FIG:

            if(ROTATION_0 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX + 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY - 1;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 1;
            }

            break;

        case L_FIG:

            if(ROTATION_0 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX - 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY + 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_180;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY - 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else if(ROTATION_180 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_270;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX + 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY - 1;
            }
            else //if(ROTATION_270 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX + 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 1;
            }

            break;

        case L_REVERSE_FIG:

            if(ROTATION_0 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX + 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY - 1;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_180;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX + 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 1;
            }
            else if(ROTATION_180 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_270;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX - 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY + 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else //if(ROTATION_270 == pstrOldFig->strRam.usRtt)
            {
                /* ������ת��Ķ��� */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* ������ת������� */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY - 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }

            break;

        default:

            return RTN_FAIL;
    }

    /* ���������߽������ */
    if(pstrNewFig->strPos.scRBY > MAINSCR_MAX_Y)        /* �����±߽�, ֱ�ӷ���ʧ�� */
    {
        return RTN_FAIL;
    }
    else if(pstrNewFig->strPos.scLTX < 0)               /* ������߽�, �����ƶ�ͼ�� */
    {
        pstrNewFig->strPos.scRBX -= pstrNewFig->strPos.scLTX;
        pstrNewFig->strPos.scLTX = 0;

        return RTN_SUCD;
    }
    else if(pstrNewFig->strPos.scRBX > MAINSCR_MAX_X)   /* �����ұ߽�, �����ƶ�ͼ�� */
    {
        pstrNewFig->strPos.scLTX -= (pstrNewFig->strPos.scRBX - MAINSCR_MAX_X);
        pstrNewFig->strPos.scRBX = MAINSCR_MAX_X;

        return RTN_SUCD;
    }

    return RTN_SUCD;
}

/***********************************************************************************
��������: ��ʼ���¸���ʾͼ�εĽṹ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_NextFigStructInit(void)
{
    /* ��������¸�ͼ�α�� */
    gstrNextFig.usFigNo = (U16)DEV_GetRandomFig();
    gstrNextFig.usRtt = ROTATION_0;

    /* ͼ����ɫ */
    switch(gstrNextFig.usFigNo)
    {
        case BOX_FIG:

            gstrNextFig.usClr = LCDCOLORGREY;

            break;

        case I_FIG:

            gstrNextFig.usClr = LCDCOLORRED;

            break;

        case MOUNTAIN_FIG:

            gstrNextFig.usClr = LCDCOLORBLUE2;

            break;

        case Z_FIG:

            gstrNextFig.usClr = LCDCOLORMAGENTA;

            break;

        case Z_REVERSE_FIG:

            gstrNextFig.usClr = LCDCOLORGREEN;

            break;

        case L_FIG:

            gstrNextFig.usClr = LCDCOLORCYAN;

            break;

        case L_REVERSE_FIG:

            gstrNextFig.usClr = LCDCOLORYELLOW;

            break;
    }

    /* ����¸�ͼ�ε�Ram */
    TEST_MoveFigRamInit(&gstrNextFig);
}

/***********************************************************************************
��������: ��ʼ�����ƶ�ͼ�ζ�Ӧ��Ram.
��ڲ���: pstrRam: ��Ҫ��ʼ����ͼ��ָ��.
�� �� ֵ: none.
***********************************************************************************/
void TEST_MoveFigRamInit(FIGRAM* pstrRam)
{
    U8* pucFig;
    U8 x;
    U8 y;
    U8 i;

    /* ����ʾͼ�ε�Ram��� */
    for(x = 0; x < NEXTFIG_X_LEN; x++)
    {
        for(y = 0; y < NEXTFIG_Y_LEN; y++)
        {
            /* ��ʼ��Ϊ����ɫ */
            pstrRam->ausFigRam[x][y] = LCDGETBACKGROUNDCOLOR;
        }
    }

    /* ���ƶ�ͼ�ε�Ram��ַ */
    pucFig = TEST_GetFigPixelPointer(pstrRam);

    /* �����ƶ�ͼ�ε�Ram, �����ϽǶ��� */
    for(i = 0; i < pucFig[4] ; i++)
    {
        for(x = 0; x < pucFig[7 + i * 4]; x++)
        {
            for(y = 0; y < pucFig[8 + i * 4]; y++)
            {
                /* ��ʼ��Ϊͼ�ζ�Ӧ����ɫ */
                pstrRam->ausFigRam[x + pucFig[5 + i * 4]][y + pucFig[6 + i * 4]] = pstrRam->usClr;
            }
        }
    }
}

/***********************************************************************************
��������: ��ȡͼ�����ش�ŵ�ָ��.
��ڲ���: pstrRam: ͼ�ε�ָ��.
�� �� ֵ: ͼ�����ش�ŵ�ָ��.
***********************************************************************************/
U8* TEST_GetFigPixelPointer(FIGRAM* pstrRam)
{
    U8* pucFig;

    switch(pstrRam->usFigNo)
    {
        case BOX_FIG:

            pucFig = (U8*)&gcaucBox_Fig;

            break;

        case I_FIG:

            /* ͼ����ת0�� */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucI_Fig_0_180;
            }
            /* ͼ����ת90�� */
            else //if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucI_Fig_90_270;
            }

            break;

        case MOUNTAIN_FIG:

            /* ͼ����ת0�� */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucMountain_Fig_0;
            }
            /* ͼ����ת90�� */
            else if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucMountain_Fig_90;
            }
            /* ͼ����ת180�� */
            else if(ROTATION_180 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucMountain_Fig_180;
            }
            /* ͼ����ת270�� */
            else //if(ROTATION_270 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucMountain_Fig_270;
            }

            break;

        case Z_FIG:

            /* ͼ����ת0�� */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucZ_Fig_0_180;
            }
            /* ͼ����ת90�� */
            else //if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucZ_Fig_90_270;
            }

            break;

        case Z_REVERSE_FIG:

            /* ͼ����ת0�� */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucZ_Reverse_Fig_0_180;
            }
            /* ͼ����ת90�� */
            else //if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucZ_Reverse_Fig_90_270;
            }

            break;

        case L_FIG:

            /* ͼ����ת0�� */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Fig_0;
            }
            /* ͼ����ת90�� */
            else if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Fig_90;
            }
            /* ͼ����ת180�� */
            else if(ROTATION_180 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Fig_180;
            }
            /* ͼ����ת270�� */
            else //if(ROTATION_270 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Fig_270;
            }

            break;

        case L_REVERSE_FIG:

            /* ͼ����ת0�� */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Reverse_Fig_0;
            }
            /* ͼ����ת90�� */
            else if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Reverse_Fig_90;
            }
            /* ͼ����ת180�� */
            else if(ROTATION_180 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Reverse_Fig_180;
            }
            /* ͼ����ת270�� */
            else //if(ROTATION_270 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Reverse_Fig_270;
            }

            break;

        default:

            return NULL;
    }

    return pucFig;
}

/***********************************************************************************
��������: ����ǰͼ�θ��Ƶ���������.
��ڲ���: none.
�� �� ֵ: RTN_SUCD: ͼ�ο�����ʾ����������.
          RTN_FAIL: ����������, �޷���ʾ��ͼ��.
***********************************************************************************/
U32 TEST_CopyCurFigToMainScn(void)
{
    U8 x;
    U8 y;

    /* ���ͼ�γ�ͻ */
    for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
    {
        for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
        {
            /* ���2��ͼ����ͬ��λ��ͬ��Ϊ����ɫ��ͼ���г�ͻ, ����ʧ�� */
            if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y])
               && (LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
            {
                /* ����ʧ�� */
                return RTN_FAIL;
            }
        }
    }

    /* ����ǰͼ�η���������Ram�� */
    for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
    {
        for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
        {
            /* ��ǰͼ�θ��Ƶ��������� */
            if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
            {
                gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
            }
        }
    }

    return RTN_SUCD;
}

/***********************************************************************************
��������: ���ϰ����Ĵ������. �������ϰ���, ��ǰͼ����Ҫ˳ʱ����ת90��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_KeyUpProcess(void)
{
    /* ͼ����ת */
    (void)TEST_RotationFigOverlapCheck();
}

/***********************************************************************************
��������: ���°����Ĵ������. �������°���, ��ǰͼ����Ҫ������һ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_KeyDownProcess(void)
{
    U8 y;
    U8 i;
    U8 j;

    /* ��ͼ��������ʧ��, ˵���Ѿ���������Ϊһ�� */
    if(RTN_FAIL == TEST_MoveFigOverlapCheck(MOVE_DOWN))
    {
        /* ��Ҫ�жϸ��е����½�Y������ */
        y = gstrCurFig.strPos.scRBY;

        /* ���ͼ�����ڵ�Y�᳤�����Ƿ����ɾ��һ��ͼ�� */
        for(j = 0; j < (gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1); j++)
        {
            /* �ܹ�ɾ��һ�гɹ� */
            if(RTN_SUCD == TEST_DelOneLineCheck(y))
            {
                /* �ı���Ϸ״̬ */
                gucGameSta = GAMESTA_DELLINE;

                /* ��ɾ������˸2�� */
                for(i = 0; i < 2 * DELLINFLASHFRQ; i++)
                {
                    /* �ı�ɾ������ɫ */
                    TEST_OneLineFlash(y, i);

                    /* �����ӳ�500ms */
                    (void)MDS_TaskDelay(1000 / DELLINFLASHFRQ / TICK);
                }

                /* ������֮�ϵ���������Ļ������һ�� */
                TEST_MainScnDownOneLine(y);
            }
            else
            {
                /* û��ɾ������, ���ж���һ���Ƿ����ɾ�� */
                y--;
            }
        }

        /* ׼���¸�ͼ�ο�ʼ */
        if(RTN_FAIL == TEST_NextFigStart())
        {
            /* �޷�����¸�ͼ��, ��Ϸʧ��, ����ֹͣ״̬ */
            gucGameSta = GAMESTA_STOP;
        }

        return;
    }
}

/***********************************************************************************
��������: ���󰴼��Ĵ������. �������󰴼�, ��ǰͼ����Ҫ������һ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_KeyLeftProcess(void)
{
    /* ͼ������һ�� */
    (void)TEST_MoveFigOverlapCheck(MOVE_LEFT);
}

/***********************************************************************************
��������: ���Ұ����Ĵ������. �������Ұ���, ��ǰͼ����Ҫ������һ��.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_KeyRightProcess(void)
{
    /* ͼ������һ�� */
    (void)TEST_MoveFigOverlapCheck(MOVE_RIGHT);
}

/***********************************************************************************
��������: ��Ϸ���¿�ʼ�����Ĵ������. �������¿�ʼ����, ��Ϸ���¿�ʼ.
��ڲ���: none.
�� �� ֵ: none.
***********************************************************************************/
void TEST_KeyRestartProcess(void)
{
    /* ��Ϸ���¿�ʼ */
    TEST_GameStart();
}

/***********************************************************************************
��������: ���ͬʱ���µİ�������.
��ڲ���: none.
�� �� ֵ: ͬʱ���µİ�������.
***********************************************************************************/
U8 TEST_KeyPressCheck(U32 uiKey)
{
    U8 ucKeyNum;
    U8 i;

    ucKeyNum = 0;

    /* ��鰴����Ӧ��ÿ��bitֵ */
    for(i = 0; i < KEYNUM; i++)
    {
        if(1 == ((uiKey >> i) & 1))
        {
            ucKeyNum++;
        }
    }

    return ucKeyNum;
}

/***********************************************************************************
��������: ��ָ�����꿪ʼ, ��������Ļͼ��������һ��.
��ڲ���: uiy: �����ڴ����Y�����꿪ʼ������.
�� �� ֵ: none.
***********************************************************************************/
void TEST_MainScnDownOneLine(U32 uiy)
{
    U8 x;
    U8 y;

    /* ��ȡͼ���ź���, ��ֹ��������ͬʱ��ͼ�ν��в��� */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* ��ָ�����꿪ʼ, ��������Ļͼ��������һ�� */
    for(x = MAINSCR_MIN_X; x < MAINSCN_X_LEN; x++)
    {
        for(y = 0; y < uiy; y++)
        {
            /* ��������Ram����һ�е���ɫ���Ƶ���һ��Ram�� */
            gausMainPrtRam[x][uiy - y] = gausMainPrtRam[x][uiy - y - 1];
        }
    }

    /* ������ճ���һ����䱳��ɫ */
    for(x = MAINSCR_MIN_X; x < MAINSCN_X_LEN; x++)
    {
        gausMainPrtRam[x][0] = LCDGETBACKGROUNDCOLOR;
    }

    /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
    (void)MDS_SemGive(gpstrFigSem);
}

/***********************************************************************************
��������: �����ƶ�ͼ�β������ԭ��ͼ���Ƿ����غϵĲ���, �����غϵĲ����򷵻�ʧ��, ��
          û���غϵĲ�����ͼ�θ��µ���λ��.
��ڲ���: ucMode: ��Ҫ����ģʽ.
                  MOVE_DOWN: ͼ�����ƺ���.
                  MOVE_LEFT: ͼ�����ƺ���.
                  MOVE_RIGHT: ͼ�����ƺ���.
�� �� ֵ: RTN_SUCD: û���ص�.
          RTN_FAIL: ���ص�.
***********************************************************************************/
U32 TEST_MoveFigOverlapCheck(U8 ucMode)
{
    U32 uiRtn;
    U8 ucMovePix;
    U8 x;
    U8 y;

    uiRtn = RTN_SUCD;       /* ������ֵĬ����ΪRTN_SUCD */
    ucMovePix = 1;          /* ��ͼ���ƶ�����Ĭ����Ϊһ��(һ��) */

    /* ������, ��Ҫ������������һ�е�λ�����Ƚ� */
    if(MOVE_DOWN == ucMode)
    {
        /* �����ǰͼ�ε�����������׶���ֱ�ӷ���ʧ�� */
        if(MAINSCR_MAX_Y == gstrCurFig.strPos.scRBY)
        {
            return RTN_FAIL;
        }

        /* ��ȡͼ���ź���, ��ֹ��������ͬʱ��ͼ�ν��в��� */
        (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

        /* ����ǰͼ�δ�������Ram��ȥ�� */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* ���������е�ǰͼ�εĲ�����Ϊ����ɫ */
                if(LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y])
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = LCDGETBACKGROUNDCOLOR;
                }
            }
        }

        /* ���ͼ�γ�ͻ */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            /* ��Ҫ���� */
            if(RTN_FAIL == uiRtn)
            {
                break;
            }

            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* ���2��ͼ����ͬ��λ��ͬ��Ϊ����ɫ��ͼ���г�ͻ, ����ʧ�� */
                if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[gstrCurFig.strPos.scLTX + x][(gstrCurFig.strPos.scLTY + 1) + y])
                   && (LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    /* ��Ҫ����ʧ�� */
                    uiRtn = RTN_FAIL;

                    /* ͼ�β���Ҫ�ƶ� */
                    ucMovePix = 0;

                    break;
                }
            }
        }

        /* ����gstrCurFig�Ĳ��� */
        gstrCurFig.strPos.scLTY += ucMovePix;
        gstrCurFig.strPos.scRBY += ucMovePix;

        /* ����ǰͼ�η���������Ram�� */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* ��ǰͼ�θ��Ƶ��������� */
                if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
                }
            }
        }

        /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
        (void)MDS_SemGive(gpstrFigSem);
    }
    /* ������, ��Ҫ������������һ�е�λ�����Ƚ� */
    else if(MOVE_LEFT == ucMode)
    {
        /* �����ǰͼ�ε����������������ֱ�ӷ���ʧ�� */
        if(MAINSCR_MIN_X == gstrCurFig.strPos.scLTX)
        {
            return RTN_FAIL;
        }

        /* ��ȡͼ���ź���, ��ֹ��������ͬʱ��ͼ�ν��в��� */
        (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

        /* ����ǰͼ�δ�������Ram��ȥ�� */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* ���������е�ǰͼ�εĲ�����Ϊ����ɫ */
                gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = LCDGETBACKGROUNDCOLOR;
            }
        }

        /* ���ͼ�γ�ͻ */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            /* ��Ҫ���� */
            if(RTN_FAIL == uiRtn)
            {
                break;
            }

            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* ͼ�β�����ʾ���Ĳ��ֲ���� */
                if(gstrCurFig.strPos.scLTX + x > MAINSCR_MAX_X)
                {
                    continue;
                }

                /* ���2��ͼ����ͬ��λ��ͬ��Ϊ����ͼ���г�ͻ, ����ʧ�� */
                if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[gstrCurFig.strPos.scLTX - 1 + x][gstrCurFig.strPos.scLTY + y])
                   && (LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    /* ��Ҫ����ʧ�� */
                    uiRtn = RTN_FAIL;

                    /* ͼ�β���Ҫ�ƶ� */
                    ucMovePix = 0;

                    break;
                }
            }
        }

        /* ����gstrCurFig�Ĳ��� */
        gstrCurFig.strPos.scLTX -= ucMovePix;
        gstrCurFig.strPos.scRBX -= ucMovePix;

        /* ����ǰͼ�η���������Ram�� */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* ��ǰͼ�θ��Ƶ��������� */
                if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
                }
            }
        }

        /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
        (void)MDS_SemGive(gpstrFigSem);
    }
    /* ������, ��Ҫ������������һ�е�λ�����Ƚ� */
    else //if(MOVE_RIGHT == ucMode)
    {
        /* �����ǰͼ�ε������������Ҷ���ֱ�ӷ���ʧ�� */
        if(MAINSCR_MAX_X == gstrCurFig.strPos.scRBX)
        {
            return RTN_FAIL;
        }

        /* ��ȡͼ���ź���, ��ֹ��������ͬʱ��ͼ�ν��в��� */
        (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

        /* ����ǰͼ�δ�������Ram��ȥ�� */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* ���������е�ǰͼ�εĲ�����Ϊ��ɫ */
                if(LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y])
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = LCDGETBACKGROUNDCOLOR;
                }
            }
        }

        /* ���ͼ�γ�ͻ */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            /* ��Ҫ���� */
            if(RTN_FAIL == uiRtn)
            {
                break;
            }

            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* ���2��ͼ����ͬ��λ��ͬ��Ϊ����ͼ���г�ͻ, ����ʧ�� */
                if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[gstrCurFig.strPos.scLTX + 1 + x][gstrCurFig.strPos.scLTY + y])
                   && (LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    /* ��Ҫ����ʧ�� */
                    uiRtn = RTN_FAIL;

                    /* ͼ�β���Ҫ�ƶ� */
                    ucMovePix = 0;

                    break;
                }
            }
        }

        /* ����gstrCurFig�Ĳ��� */
        gstrCurFig.strPos.scLTX += ucMovePix;
        gstrCurFig.strPos.scRBX += ucMovePix;

        /* ����ǰͼ�η���������Ram�� */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* ��ǰͼ�θ��Ƶ��������� */
                if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
                }
            }
        }

        /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
        (void)MDS_SemGive(gpstrFigSem);
    }

    return uiRtn;
}

/***********************************************************************************
��������: ������תͼ�β������ԭ��ͼ���Ƿ����غϵĲ���, �����غϵĲ����򷵻�ʧ��, ��
          û���غϵĲ�����ͼ�θ��µ���λ��.
��ڲ���: none.
�� �� ֵ: RTN_SUCD: û���ص�.
          RTN_FAIL: ���ص�.
***********************************************************************************/
U32 TEST_RotationFigOverlapCheck(void)
{
    FIGSTR strFigTemp;
    U32 uiRtn;
    U8 x;
    U8 y;

    if(RTN_FAIL == TEST_GetCurFigPosAfterRtt(&gstrCurFig, &strFigTemp))
    {
        /* ͼ���޷���ת, ����ʧ�� */
        return RTN_FAIL;
    }

    /* �����ת���ͼ��Ram */
    TEST_MoveFigRamInit(&strFigTemp.strRam);

    /* ��ȡͼ���ź���, ��ֹ��������ͬʱ��ͼ�ν��в��� */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* ����ǰͼ�δ�������Ram��ȥ�� */
    for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
    {
        for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
        {
            /* ���������е�ǰͼ�εĲ�����Ϊ����ɫ */
            if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
            {
                gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = LCDGETBACKGROUNDCOLOR;
            }
        }
    }

    uiRtn = RTN_SUCD;       /* ������ֵĬ����ΪRTN_SUCD */

    /* ���ͼ�γ�ͻ */
    for(x = 0; x < (strFigTemp.strPos.scRBX - strFigTemp.strPos.scLTX + 1); x++)
    {
        /* ��Ҫ���� */
        if(RTN_FAIL == uiRtn)
        {
            break;
        }

        for(y = 0; y < strFigTemp.strPos.scRBY - strFigTemp.strPos.scLTY + 1; y++)
        {
            /* ���2��ͼ����ͬ��λ��ͬ��Ϊ����ͼ���г�ͻ, ����ʧ�� */
            if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[strFigTemp.strPos.scLTX + x][strFigTemp.strPos.scLTY + y])
               && (LCDGETBACKGROUNDCOLOR != strFigTemp.strRam.ausFigRam[x][y]))
            {
                /* ��Ҫ����ʧ�� */
                uiRtn = RTN_FAIL;

                break;
            }
        }
    }

    /* ��ת��ͼ��û�г�ͻ */
    if(RTN_SUCD == uiRtn)
    {
        /* ����gstrCurFig�Ĳ��� */
        gstrCurFig.strRam.usRtt = strFigTemp.strRam.usRtt;
        gstrCurFig.strPos.scLTX = strFigTemp.strPos.scLTX;
        gstrCurFig.strPos.scLTY = strFigTemp.strPos.scLTY;
        gstrCurFig.strPos.scRBX = strFigTemp.strPos.scRBX;
        gstrCurFig.strPos.scRBY = strFigTemp.strPos.scRBY;

        /* ����ת���ͼ����䵽gstrCurFig�ṹ��Ram�� */
        TEST_MoveFigRamInit(&gstrCurFig.strRam);
    }

    /* ����ǰͼ�η���������Ram�� */
    for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
    {
        for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
        {
            /* ��ǰͼ�θ��Ƶ��������� */
            if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
            {
                gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
            }
        }
    }

    /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
    (void)MDS_SemGive(gpstrFigSem);

    return uiRtn;
}

/***********************************************************************************
��������: ����������Ƿ�����ɾ��һ��ͼ�ε�����.
��ڲ���: uiy: Ҫɾ���е�Y������.
�� �� ֵ: RTN_SUCD: ����ɾ��һ��ͼ��.
          RTN_FAIL: ����ɾ��һ��ͼ��.
***********************************************************************************/
U32 TEST_DelOneLineCheck(U32 uiy)
{
    U8 x;

    for(x = MAINSCR_MIN_X; x < MAINSCR_MAX_X; x++)
    {
        /* �б���ɫ������, ˵��û������һ��, ����ʧ�� */
        if(LCDGETBACKGROUNDCOLOR == gausMainPrtRam[x][uiy])
        {
            return RTN_FAIL;
        }
    }

    return RTN_SUCD;
}

/***********************************************************************************
��������: ��˸һ��.
��ڲ���: uiy: ��Ҫ��˸�е�Y������.
          uiStart: 0: �����ڲ�״̬, ���¿�ʼ.
                   ����: �����ϴ�״̬.
�� �� ֵ: none.
***********************************************************************************/
void TEST_OneLineFlash(U32 uiy, U32 uiStart)
{
    static U16 susColor = LCDCOLORGREY;
    U8 x;

    /* ��Ҫ���¿�ʼ, ʹ�ó�ʼ��ɫ */
    if(0 == uiStart)
    {
        susColor = LCDCOLORGREY;
    }

    /* ��ȡͼ���ź���, ��ֹ��������ͬʱ��ͼ�ν��в��� */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* ����ɫ���ĵ�һ��ͼ���Ram�� */
    for(x = 0; x < MAINSCN_X_LEN; x++)
    {
        gausMainPrtRam[x][uiy] = susColor;
    }

    /* �ͷ�ͼ���ź���, �Ա������������ˢ����Ļ */
    (void)MDS_SemGive(gpstrFigSem);

    /* �����´ε���ɫ */
    if(LCDCOLORGREY == susColor)
    {
        susColor = LCDCOLORBLUE;
    }
    else
    {
        susColor = LCDCOLORGREY;
    }
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
    /* Ϊ����ӡ���ڴ�ӡ������л�����, �����������Ϊ�ǿ������� */
    if(pstrOldTcb == gpstrSerialTaskTcb)
    {
        pstrOldTcb = MDS_GetIdleTcb();
    }

    if(pstrNewTcb == gpstrSerialTaskTcb)
    {
        pstrNewTcb = MDS_GetIdleTcb();
    }

    /* ͬһ������֮���л�����ӡ��Ϣ */
    if(pstrOldTcb == pstrNewTcb)
    {
        return;
    }

    /* û��ɾ���л�ǰ���� */
    if(NULL != pstrOldTcb)
    {
        /* ���ڴ��ӡ�����л���Ϣ */
        DEV_PutStrToMem((U8*)"\r\nTask %s ---> Task %s! Tick is: %d",
                        pstrOldTcb->pucTaskName, pstrNewTcb->pucTaskName,
                        MDS_GetSystemTick());
    }
    else /* �л�ǰ������ɾ�� */
    {
        /* ���ڴ��ӡ�����л���Ϣ */
        DEV_PutStrToMem((U8*)"\r\nTask NULL ---> Task %s! Tick is: %d",
                        pstrNewTcb->pucTaskName, MDS_GetSystemTick());
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

