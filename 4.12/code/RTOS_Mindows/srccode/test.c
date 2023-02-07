
#include "test.h"


M_QUE* gpstrSerialMsgQue;               /* 串口打印消息队列指针 */
M_QUE* gpstrKeyMsgQue;                  /* 按键消息的队列指针 */

M_TCB* gpstrSerialTaskTcb;              /* 串口打印任务TCB指针 */

M_SEM* gpstrFigSem;                     /* Lcd显示图形的信号量指针, 通过该信号量实现对
                                           Lcd显示图形的串行访问 */

U8 gucGameSta;                          /* 游戏状态 */

FIGSTR gstrCurFig;                      /* 当前图形结构 */
FIGRAM gstrNextFig;                     /* 下个图形结构 */

/* 定义的Lcd Ram数组与Lcd屏幕的对应关系为: 一维数组元素对应Lcd屏幕的X轴, 二维数组元
   素对应Lcd的Y轴, 也即数组原点对应Lcd屏幕的左上角坐标原点, 一维数组元素的增长方向对
   应Lcd屏幕向右的X轴增长方向, 二维数组元素的增长方向对应Lcd屏幕向下的Y轴增长方向 */
/* 存放主窗口显示区图形的Ram */
U16 gausMainPrtRam[MAINSCN_X_LEN][MAINSCN_Y_LEN];

/* 图形显示的像素定义 */
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
函数功能: 刷屏任务. 该任务以10Hz的频率刷新Lcd屏幕.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_FlushScnTask(void* pvPara)
{
    /* 初始化屏幕 */
    TEST_ScreenInit();

    while(1)
    {
        /* 获取图形信号量, 防止其它任务同时对图形进行操作 */
        (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

        /* 刷新主窗口 */
        TEST_ImageDraw(MAINSCR_MIN_X, MAINSCR_MIN_Y, MAINSCN_X_LEN, MAINSCN_Y_LEN, (U16*)gausMainPrtRam);

        /* 刷新下个图形窗口 */
        TEST_ImageDraw(NEXTFIG_MIN_X, NEXTFIG_MIN_Y, NEXTFIG_X_LEN, NEXTFIG_Y_LEN, (U16*)&gstrNextFig.ausFigRam);

        /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
        (void)MDS_SemGive(gpstrFigSem);

        (void)MDS_TaskDelay(10);
    }
}

/***********************************************************************************
函数功能: 按键任务. 该任务以10Hz的频率读取按键对应的GPIO, 判断是否有按键输入.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
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
        /* 申请buf, 用来存放按键消息 */
        if(NULL == pstrKeyMsg)
        {
            pstrKeyMsg = (STRKEYMSGQUE*)DEV_BufferAlloc(&gstrBufPool);
            if(NULL == pstrKeyMsg)
            {
                return;
            }
        }

        /* 按键检测间隔时间 */
        (void)MDS_TaskDelay(10);

        /* 不满足按键等待时间, 则认为是没有按键按下 */
        if(0 != usNotActKeyCnt)
        {
            usNotActKeyCnt--;

            ucKey = KEYNULL;
            ucKeyNum = 0;
        }
        else /* 需要检测按键 */
        {
            /* 读取按键值并计算同时按下按键的数量 */
            ucKey = DEV_ReadKey();
            ucKeyNum = TEST_KeyPressCheck(ucKey);
        }

        /* 只有一个按键被按下时才响应按键 */
        if(1 == ucKeyNum)
        {
            /* 有按键响应, 200ms之后才能再次检测按键值 */
            usNotActKeyCnt = NOTACTKEYTIME;

            /* 如果游戏处于GAMESTA_STOP状态则只响应Restart按键 */
            if(GAMESTA_STOP == gucGameSta)
            {
                if(KEYRESTART == ucKey)
                {
                    /* 重置计数值 */
                    usNotActKeyCnt = 0;
                    usAutoDownCnt = 0;

                    /* 存入重新开始按键值 */
                    pstrKeyMsg->uiKey = KEYRESTART;
                }
            }
            /* 如果游戏处于GAMESTA_DROP状态则响应按键 */
            else if(GAMESTA_DROP == gucGameSta)
            {
                /* 如果有向下按键则重置自动向下的按键计数 */
                if(KEYDOWN == ucKey)
                {
                    usAutoDownCnt = 0;
                }

                /* 存入按键值 */
                pstrKeyMsg->uiKey = ucKey;
            }
        }
        /* 没有按键被按下或者多个按键同时被按下的情况则按没有按键按下的情况处理 */
        else
        {
            /* 如果游戏不处于GAMESTA_DROP状态则图形不自动走 */
            if(GAMESTA_DROP != gucGameSta)
            {
                continue;
            }

            /* 没有按键响应, 400ms之后才能再次检测按键值 */
            if(usAutoDownCnt < AUTODOWNTIME)
            {
                usAutoDownCnt++;

                /* 不满足按键检测次数, 退出本次循环 */
                continue;
            }
            else
            {
                /* 达到按键检测次数, 重新计数 */
                usAutoDownCnt = 0;
            }

            /* 给Process任务发一个向下按键的消息 */
            pstrKeyMsg->uiKey = KEYDOWN;
        }

        /* 向Process任务发一个重新开始按键的消息 */
        (void)MDS_QuePut(gpstrKeyMsgQue, &pstrKeyMsg->strQueHead);
        pstrKeyMsg = NULL;
    }
}

/***********************************************************************************
函数功能: 按键响应任务. 该任务由按键任务触发, 根据输入的按键做出相应的响应.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_ProcessTask(void* pvPara)
{
    STRKEYMSGQUE* pstrKeyMsg;
    M_DLIST* pstrList;

    while(1)
    {
        /* 有触发按键的动作 */
        if(RTN_SUCD == MDS_QueGet(gpstrKeyMsgQue, &pstrList, QUEWAITFEV))
        {
            /* 获取消息中的按键信息 */
            pstrKeyMsg = (STRKEYMSGQUE*)pstrList;

            /* 根据按键信息做响应的处理 */
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

            /* 释放队列消息缓冲 */
            DEV_BufferFree(&gstrBufPool, &pstrKeyMsg->strQueHead);
        }
    }
}

/***********************************************************************************
函数功能: 串口打印任务, 从队列获取需要打印的消息缓冲, 将缓冲中的打印数据打印到串口.
入口参数: pvPara: 任务入口参数指针.
返 回 值: none.
***********************************************************************************/
void TEST_SerialPrintTask(void* pvPara)
{
    M_DLIST* pstrMsgQueNode;
    MSGBUF* pstrMsgBuf;

    /* 从队列循环获取消息 */
    while(1)
    {
        /* 从队列中获取到一条需要打印的消息, 向串口打印消息数据, 若获取不到队列消息
           任务则挂在队列上进入pend状态, 等待有新消息进入队列重新变为ready态 */
        if(RTN_SUCD == MDS_QueGet(gpstrSerialMsgQue, &pstrMsgQueNode, QUEWAITFEV))
        {
            pstrMsgBuf = (MSGBUF*)pstrMsgQueNode;

            /* 将缓冲中的数据打印到串口 */
            DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

            /* 缓冲消息中的数据发送完毕, 释放缓冲 */
            DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
        }
    }
}

/***********************************************************************************
函数功能: 屏幕初始化.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_ScreenInit(void)
{
    /* 初始化主窗口显示区和隐藏区图形的内存 */
    TEST_MainScnRamInit();

    /* 初始化主窗口背景色 */
    TEST_ImageDraw(MAINSCR_MIN_X, MAINSCR_MIN_Y, MAINSCN_X_LEN, MAINSCN_Y_LEN, (U16*)gausMainPrtRam);

    /* 初始化分割线和副窗口背景色, 借用主窗口的Ram */
    TEST_ImageDraw(LINE_MIN_X, SLAVESCR_MIN_Y, SLAVESCN_X_LEN + LINE_X_LEN, SLAVESCN_Y_LEN, (U16*)gausMainPrtRam);

    /* 画笔置为白色 */
    LCDSETPENCOLOR(LCDCOLORWHITE);

    /* 显示分隔线 */
    DEV_LcdDrawBox(LINE_MIN_X * PIXELAMP, LINE_MIN_Y * PIXELAMP, LINE_X_LEN * 2, LINE_Y_LEN * PIXELAMP);
}

/***********************************************************************************
函数功能: 将内存中的像素画到Lcd上, 每个图形像素对应PIXELAMP平方个的实际像素.
入口参数: ucX: X轴最小坐标.
          ucY: Y轴最小坐标.
          ucXLen: X轴长度.
          ucYLen: Y轴长度.
          pusRam: 内存起始地址.
返 回 值: none.
***********************************************************************************/
void TEST_ImageDraw(U8 ucX, U8 ucY, U8 ucXLen, U8 ucYLen, U16* pusRam)
{
    U8 x;
    U8 y;

    /* 填充每个图形像素 */
    for(x = 0; x < ucXLen; x++)
    {
        for(y = 0; y < ucYLen; y++)
        {
            /* 设置画笔颜色 */
            LCDSETPENCOLOR(pusRam[x * ucYLen + y]);

            /* 将图形像素画到对应的实际像素上 */
            DEV_LcdDrawBox(PIXELAMP * (ucX + x), PIXELAMP * (ucY + y), PIXELAMP, PIXELAMP);
        }
    }
}

/***********************************************************************************
函数功能: 游戏开始, 初始化游戏变量.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_GameStart(void)
{
    /* 获取图形信号量, 防止其它任务同时对图形进行操作 */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* 初始化屏幕 */
    TEST_ScreenInit();

    /* 初始化下个显示图形的结构 */
    TEST_NextFigStructInit();

    /* 初始化当前显示图形的结构 */
    (void)TEST_CurFigStructInit();

    /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
    (void)MDS_SemGive(gpstrFigSem);

    /* 游戏开始, 产生下个新的图形 */
    TEST_NextFigStructInit();

    /* 初始化游戏状态 */
    gucGameSta = GAMESTA_DROP;
}

/***********************************************************************************
函数功能: 下一个图形开始, 如果主窗口已经无法再填入新图形则返回失败.
入口参数: none.
返 回 值: RTN_SUCD: 下一个图形可以显示到主窗口中.
          RTN_FAIL: 主窗口已满, 无法显示新图形.
***********************************************************************************/
U32 TEST_NextFigStart(void)
{
    /* 获取图形信号量, 防止其它任务同时对图形进行操作 */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* 将下个显示图形变为当前显示图形 */
    if(RTN_FAIL == TEST_CurFigStructInit())
    {
        /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
        (void)MDS_SemGive(gpstrFigSem);

        return RTN_FAIL;
    }

    /* 重新选择下个显示图形 */
    TEST_NextFigStructInit();

    /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
    (void)MDS_SemGive(gpstrFigSem);

    /* 重新初始化游戏状态 */
    gucGameSta = GAMESTA_DROP;

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 初始化Led显示屏主窗口对应的Ram.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_MainScnRamInit(void)
{
    U8 x;
    U8 y;

    /* 设置背景色为黑色 */
    LCDSETBACKGROUNDCOLOR(LCDCOLORBLACK);

    /* 初始化主窗口显示区图形的内存 */
    for(x = 0; x < MAINSCN_X_LEN; x++)
    {
        for(y = 0; y < MAINSCN_Y_LEN; y++)
        {
            /* 初始化为背景色 */
            gausMainPrtRam[x][y] = LCDGETBACKGROUNDCOLOR;
        }
    }
}

/***********************************************************************************
函数功能: 初始化当期显示图形的结构.
入口参数: none.
返 回 值: RTN_SUCD: 下一个图形可以显示到主窗口中.
          RTN_FAIL: 主窗口已满, 无法显示新图形.
***********************************************************************************/
U32 TEST_CurFigStructInit(void)
{
    /* 初始化图形结构 */
    gstrCurFig.strRam.usFigNo = gstrNextFig.usFigNo;    /* 图形编号 */
    gstrCurFig.strRam.usRtt = gstrNextFig.usRtt;        /* 图形旋转的角度 */
    gstrCurFig.strRam.usClr = gstrNextFig.usClr;        /* 图形颜色 */

    /* 初始化图形所在坐标 */
    TEST_InitFigPos(&gstrCurFig);

    /* 填充当前图形的Ram */
    TEST_MoveFigRamInit(&gstrCurFig.strRam);

    /* 将当前图形复制到主窗口中 */
    if(RTN_FAIL == TEST_CopyCurFigToMainScn())
    {
        /* 无法复制, 游戏失败 */
        return RTN_FAIL;
    }

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 初始化图形的坐标.
入口参数: pstrFig: 图形结构指针.
返 回 值: none.
***********************************************************************************/
void TEST_InitFigPos(FIGSTR* pstrFig)
{
    U8* pucFig;

    /* 可移动图形初始值所在的Ram地址 */
    pucFig = TEST_GetFigPixelPointer(&pstrFig->strRam);

    /* 初始化图形的位置 */
    pstrFig->strPos.scLTX = pucFig[0];
    pstrFig->strPos.scLTY = pucFig[1];
    pstrFig->strPos.scRBX = pucFig[0] + pucFig[2] - 1;
    pstrFig->strPos.scRBY = pucFig[1] + pucFig[3] - 1;
}

/***********************************************************************************
函数功能: 获取当前图形图形旋转后的位置坐标.
入口参数: pstrOldFig: 图形旋转前的结构指针.
          pstrNewFig: 图形旋转后的结构指针.
返 回 值: RTN_SUCD: 操作成功.
          RTN_FAIL: 操作失败.
***********************************************************************************/
U32 TEST_GetCurFigPosAfterRtt(FIGSTR* pstrOldFig, FIGSTR* pstrNewFig)
{
    pstrNewFig->strRam.usFigNo = pstrOldFig->strRam.usFigNo;
    pstrNewFig->strRam.usClr = pstrOldFig->strRam.usClr;

    /* 方块图形不需要旋转 */
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
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX - 2;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY + 2;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX + 2;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY - 2;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX + 2;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY - 2;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 2;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 2;
            }

            break;

        case MOUNTAIN_FIG:

            if(ROTATION_0 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX + 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 1;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_180;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX - 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY + 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else if(ROTATION_180 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_270;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY - 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else //if(ROTATION_270 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* 更新旋转后的坐标 */
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
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX + 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY - 1;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 1;
            }

            break;

        case L_FIG:

            if(ROTATION_0 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX - 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY + 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_180;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY - 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else if(ROTATION_180 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_270;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX + 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY - 1;
            }
            else //if(ROTATION_270 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX + 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 1;
            }

            break;

        case L_REVERSE_FIG:

            if(ROTATION_0 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_90;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX + 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY - 1;
            }
            else if(ROTATION_90 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_180;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX + 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY + 1;
            }
            else if(ROTATION_180 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_270;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX - 1;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY + 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }
            else //if(ROTATION_270 == pstrOldFig->strRam.usRtt)
            {
                /* 更新旋转后的度数 */
                pstrNewFig->strRam.usRtt = ROTATION_0;

                /* 更新旋转后的坐标 */
                pstrNewFig->strPos.scLTX = pstrOldFig->strPos.scLTX;
                pstrNewFig->strPos.scLTY = pstrOldFig->strPos.scLTY - 1;
                pstrNewFig->strPos.scRBX = pstrOldFig->strPos.scRBX - 1;
                pstrNewFig->strPos.scRBY = pstrOldFig->strPos.scRBY;
            }

            break;

        default:

            return RTN_FAIL;
    }

    /* 调整超出边界的坐标 */
    if(pstrNewFig->strPos.scRBY > MAINSCR_MAX_Y)        /* 超出下边界, 直接返回失败 */
    {
        return RTN_FAIL;
    }
    else if(pstrNewFig->strPos.scLTX < 0)               /* 超出左边界, 向右移动图形 */
    {
        pstrNewFig->strPos.scRBX -= pstrNewFig->strPos.scLTX;
        pstrNewFig->strPos.scLTX = 0;

        return RTN_SUCD;
    }
    else if(pstrNewFig->strPos.scRBX > MAINSCR_MAX_X)   /* 超出右边界, 向左移动图形 */
    {
        pstrNewFig->strPos.scLTX -= (pstrNewFig->strPos.scRBX - MAINSCR_MAX_X);
        pstrNewFig->strPos.scRBX = MAINSCR_MAX_X;

        return RTN_SUCD;
    }

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 初始化下个显示图形的结构.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_NextFigStructInit(void)
{
    /* 随机产生下个图形编号 */
    gstrNextFig.usFigNo = (U16)DEV_GetRandomFig();
    gstrNextFig.usRtt = ROTATION_0;

    /* 图形颜色 */
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

    /* 填充下个图形的Ram */
    TEST_MoveFigRamInit(&gstrNextFig);
}

/***********************************************************************************
函数功能: 初始化可移动图形对应的Ram.
入口参数: pstrRam: 需要初始化的图形指针.
返 回 值: none.
***********************************************************************************/
void TEST_MoveFigRamInit(FIGRAM* pstrRam)
{
    U8* pucFig;
    U8 x;
    U8 y;
    U8 i;

    /* 将显示图形的Ram清空 */
    for(x = 0; x < NEXTFIG_X_LEN; x++)
    {
        for(y = 0; y < NEXTFIG_Y_LEN; y++)
        {
            /* 初始化为背景色 */
            pstrRam->ausFigRam[x][y] = LCDGETBACKGROUNDCOLOR;
        }
    }

    /* 可移动图形的Ram地址 */
    pucFig = TEST_GetFigPixelPointer(pstrRam);

    /* 填充可移动图形的Ram, 以左上角对齐 */
    for(i = 0; i < pucFig[4] ; i++)
    {
        for(x = 0; x < pucFig[7 + i * 4]; x++)
        {
            for(y = 0; y < pucFig[8 + i * 4]; y++)
            {
                /* 初始化为图形对应的颜色 */
                pstrRam->ausFigRam[x + pucFig[5 + i * 4]][y + pucFig[6 + i * 4]] = pstrRam->usClr;
            }
        }
    }
}

/***********************************************************************************
函数功能: 获取图形像素存放的指针.
入口参数: pstrRam: 图形的指针.
返 回 值: 图形像素存放的指针.
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

            /* 图形旋转0度 */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucI_Fig_0_180;
            }
            /* 图形旋转90度 */
            else //if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucI_Fig_90_270;
            }

            break;

        case MOUNTAIN_FIG:

            /* 图形旋转0度 */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucMountain_Fig_0;
            }
            /* 图形旋转90度 */
            else if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucMountain_Fig_90;
            }
            /* 图形旋转180度 */
            else if(ROTATION_180 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucMountain_Fig_180;
            }
            /* 图形旋转270度 */
            else //if(ROTATION_270 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucMountain_Fig_270;
            }

            break;

        case Z_FIG:

            /* 图形旋转0度 */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucZ_Fig_0_180;
            }
            /* 图形旋转90度 */
            else //if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucZ_Fig_90_270;
            }

            break;

        case Z_REVERSE_FIG:

            /* 图形旋转0度 */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucZ_Reverse_Fig_0_180;
            }
            /* 图形旋转90度 */
            else //if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucZ_Reverse_Fig_90_270;
            }

            break;

        case L_FIG:

            /* 图形旋转0度 */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Fig_0;
            }
            /* 图形旋转90度 */
            else if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Fig_90;
            }
            /* 图形旋转180度 */
            else if(ROTATION_180 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Fig_180;
            }
            /* 图形旋转270度 */
            else //if(ROTATION_270 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Fig_270;
            }

            break;

        case L_REVERSE_FIG:

            /* 图形旋转0度 */
            if(ROTATION_0 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Reverse_Fig_0;
            }
            /* 图形旋转90度 */
            else if(ROTATION_90 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Reverse_Fig_90;
            }
            /* 图形旋转180度 */
            else if(ROTATION_180 == pstrRam->usRtt)
            {
                pucFig = (U8*)&gcaucL_Reverse_Fig_180;
            }
            /* 图形旋转270度 */
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
函数功能: 将当前图形复制到主窗口中.
入口参数: none.
返 回 值: RTN_SUCD: 图形可以显示到主窗口中.
          RTN_FAIL: 主窗口已满, 无法显示新图形.
***********************************************************************************/
U32 TEST_CopyCurFigToMainScn(void)
{
    U8 x;
    U8 y;

    /* 检测图形冲突 */
    for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
    {
        for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
        {
            /* 如果2个图形相同的位置同不为背景色则图形有冲突, 返回失败 */
            if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y])
               && (LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
            {
                /* 返回失败 */
                return RTN_FAIL;
            }
        }
    }

    /* 将当前图形放入主窗口Ram中 */
    for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
    {
        for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
        {
            /* 当前图形复制到主窗口中 */
            if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
            {
                gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
            }
        }
    }

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 向上按键的处理过程. 按下向上按键, 当前图形需要顺时间旋转90度.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_KeyUpProcess(void)
{
    /* 图形旋转 */
    (void)TEST_RotationFigOverlapCheck();
}

/***********************************************************************************
函数功能: 向下按键的处理过程. 按下向下按键, 当前图形需要向下走一行.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_KeyDownProcess(void)
{
    U8 y;
    U8 i;
    U8 j;

    /* 将图形向下走失败, 说明已经与下面连为一体 */
    if(RTN_FAIL == TEST_MoveFigOverlapCheck(MOVE_DOWN))
    {
        /* 需要判断该行的右下角Y轴坐标 */
        y = gstrCurFig.strPos.scRBY;

        /* 检查图形所在的Y轴长度内是否可以删除一行图形 */
        for(j = 0; j < (gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1); j++)
        {
            /* 能够删除一行成功 */
            if(RTN_SUCD == TEST_DelOneLineCheck(y))
            {
                /* 改变游戏状态 */
                gucGameSta = GAMESTA_DELLINE;

                /* 被删除行闪烁2秒 */
                for(i = 0; i < 2 * DELLINFLASHFRQ; i++)
                {
                    /* 改变删除行颜色 */
                    TEST_OneLineFlash(y, i);

                    /* 任务延迟500ms */
                    (void)MDS_TaskDelay(1000 / DELLINFLASHFRQ / TICK);
                }

                /* 将该行之上的主窗口屏幕向下走一行 */
                TEST_MainScnDownOneLine(y);
            }
            else
            {
                /* 没有删除该行, 则判断上一行是否可以删除 */
                y--;
            }
        }

        /* 准备下个图形开始 */
        if(RTN_FAIL == TEST_NextFigStart())
        {
            /* 无法填充下个图形, 游戏失败, 处于停止状态 */
            gucGameSta = GAMESTA_STOP;
        }

        return;
    }
}

/***********************************************************************************
函数功能: 向左按键的处理过程. 按下向左按键, 当前图形需要向左走一列.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_KeyLeftProcess(void)
{
    /* 图形左走一列 */
    (void)TEST_MoveFigOverlapCheck(MOVE_LEFT);
}

/***********************************************************************************
函数功能: 向右按键的处理过程. 按下向右按键, 当前图形需要向右走一列.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_KeyRightProcess(void)
{
    /* 图形右走一列 */
    (void)TEST_MoveFigOverlapCheck(MOVE_RIGHT);
}

/***********************************************************************************
函数功能: 游戏重新开始按键的处理过程. 按下重新开始按键, 游戏重新开始.
入口参数: none.
返 回 值: none.
***********************************************************************************/
void TEST_KeyRestartProcess(void)
{
    /* 游戏重新开始 */
    TEST_GameStart();
}

/***********************************************************************************
函数功能: 检查同时按下的按键数量.
入口参数: none.
返 回 值: 同时按下的按键数量.
***********************************************************************************/
U8 TEST_KeyPressCheck(U32 uiKey)
{
    U8 ucKeyNum;
    U8 i;

    ucKeyNum = 0;

    /* 检查按键对应的每个bit值 */
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
函数功能: 从指定坐标开始, 主窗口屏幕图形向下走一行.
入口参数: uiy: 主窗口从这个Y轴坐标开始向下走.
返 回 值: none.
***********************************************************************************/
void TEST_MainScnDownOneLine(U32 uiy)
{
    U8 x;
    U8 y;

    /* 获取图形信号量, 防止其它任务同时对图形进行操作 */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* 从指定坐标开始, 主窗口屏幕图形向下走一行 */
    for(x = MAINSCR_MIN_X; x < MAINSCN_X_LEN; x++)
    {
        for(y = 0; y < uiy; y++)
        {
            /* 将主窗口Ram中上一行的颜色复制到下一行Ram中 */
            gausMainPrtRam[x][uiy - y] = gausMainPrtRam[x][uiy - y - 1];
        }
    }

    /* 最上面空出的一行填充背景色 */
    for(x = MAINSCR_MIN_X; x < MAINSCN_X_LEN; x++)
    {
        gausMainPrtRam[x][0] = LCDGETBACKGROUNDCOLOR;
    }

    /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
    (void)MDS_SemGive(gpstrFigSem);
}

/***********************************************************************************
函数功能: 尝试移动图形并检查与原有图形是否有重合的部分, 若有重合的部分则返回失败, 若
          没有重合的部分则将图形更新到新位置.
入口参数: ucMode: 需要检查的模式.
                  MOVE_DOWN: 图形下移后检查.
                  MOVE_LEFT: 图形左移后检查.
                  MOVE_RIGHT: 图形右移后检查.
返 回 值: RTN_SUCD: 没有重叠.
          RTN_FAIL: 有重叠.
***********************************************************************************/
U32 TEST_MoveFigOverlapCheck(U8 ucMode)
{
    U32 uiRtn;
    U8 ucMovePix;
    U8 x;
    U8 y;

    uiRtn = RTN_SUCD;       /* 将返回值默认置为RTN_SUCD */
    ucMovePix = 1;          /* 将图形移动距离默认置为一行(一列) */

    /* 向下走, 需要与主窗口下移一行的位置做比较 */
    if(MOVE_DOWN == ucMode)
    {
        /* 如果当前图形到达主窗口最底端则直接返回失败 */
        if(MAINSCR_MAX_Y == gstrCurFig.strPos.scRBY)
        {
            return RTN_FAIL;
        }

        /* 获取图形信号量, 防止其它任务同时对图形进行操作 */
        (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

        /* 将当前图形从主窗口Ram中去掉 */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* 将主窗口中当前图形的部分置为背景色 */
                if(LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y])
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = LCDGETBACKGROUNDCOLOR;
                }
            }
        }

        /* 检测图形冲突 */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            /* 需要返回 */
            if(RTN_FAIL == uiRtn)
            {
                break;
            }

            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* 如果2个图形相同的位置同不为背景色则图形有冲突, 返回失败 */
                if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[gstrCurFig.strPos.scLTX + x][(gstrCurFig.strPos.scLTY + 1) + y])
                   && (LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    /* 需要返回失败 */
                    uiRtn = RTN_FAIL;

                    /* 图形不需要移动 */
                    ucMovePix = 0;

                    break;
                }
            }
        }

        /* 更新gstrCurFig的参数 */
        gstrCurFig.strPos.scLTY += ucMovePix;
        gstrCurFig.strPos.scRBY += ucMovePix;

        /* 将当前图形放入主窗口Ram中 */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* 当前图形复制到主窗口中 */
                if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
                }
            }
        }

        /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
        (void)MDS_SemGive(gpstrFigSem);
    }
    /* 向左走, 需要与主窗口左移一行的位置做比较 */
    else if(MOVE_LEFT == ucMode)
    {
        /* 如果当前图形到达主窗口最左端则直接返回失败 */
        if(MAINSCR_MIN_X == gstrCurFig.strPos.scLTX)
        {
            return RTN_FAIL;
        }

        /* 获取图形信号量, 防止其它任务同时对图形进行操作 */
        (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

        /* 将当前图形从主窗口Ram中去掉 */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* 将主窗口中当前图形的部分置为背景色 */
                gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = LCDGETBACKGROUNDCOLOR;
            }
        }

        /* 检测图形冲突 */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            /* 需要返回 */
            if(RTN_FAIL == uiRtn)
            {
                break;
            }

            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* 图形不在显示区的部分不检查 */
                if(gstrCurFig.strPos.scLTX + x > MAINSCR_MAX_X)
                {
                    continue;
                }

                /* 如果2个图形相同的位置同不为空则图形有冲突, 返回失败 */
                if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[gstrCurFig.strPos.scLTX - 1 + x][gstrCurFig.strPos.scLTY + y])
                   && (LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    /* 需要返回失败 */
                    uiRtn = RTN_FAIL;

                    /* 图形不需要移动 */
                    ucMovePix = 0;

                    break;
                }
            }
        }

        /* 更新gstrCurFig的参数 */
        gstrCurFig.strPos.scLTX -= ucMovePix;
        gstrCurFig.strPos.scRBX -= ucMovePix;

        /* 将当前图形放入主窗口Ram中 */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* 当前图形复制到主窗口中 */
                if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
                }
            }
        }

        /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
        (void)MDS_SemGive(gpstrFigSem);
    }
    /* 向右走, 需要与主窗口右移一行的位置做比较 */
    else //if(MOVE_RIGHT == ucMode)
    {
        /* 如果当前图形到达主窗口最右端则直接返回失败 */
        if(MAINSCR_MAX_X == gstrCurFig.strPos.scRBX)
        {
            return RTN_FAIL;
        }

        /* 获取图形信号量, 防止其它任务同时对图形进行操作 */
        (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

        /* 将当前图形从主窗口Ram中去掉 */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* 将主窗口中当前图形的部分置为黑色 */
                if(LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y])
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = LCDGETBACKGROUNDCOLOR;
                }
            }
        }

        /* 检测图形冲突 */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            /* 需要返回 */
            if(RTN_FAIL == uiRtn)
            {
                break;
            }

            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* 如果2个图形相同的位置同不为空则图形有冲突, 返回失败 */
                if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[gstrCurFig.strPos.scLTX + 1 + x][gstrCurFig.strPos.scLTY + y])
                   && (LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    /* 需要返回失败 */
                    uiRtn = RTN_FAIL;

                    /* 图形不需要移动 */
                    ucMovePix = 0;

                    break;
                }
            }
        }

        /* 更新gstrCurFig的参数 */
        gstrCurFig.strPos.scLTX += ucMovePix;
        gstrCurFig.strPos.scRBX += ucMovePix;

        /* 将当前图形放入主窗口Ram中 */
        for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
        {
            for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
            {
                /* 当前图形复制到主窗口中 */
                if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
                {
                    gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
                }
            }
        }

        /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
        (void)MDS_SemGive(gpstrFigSem);
    }

    return uiRtn;
}

/***********************************************************************************
函数功能: 尝试旋转图形并检查与原有图形是否有重合的部分, 若有重合的部分则返回失败, 若
          没有重合的部分则将图形更新到新位置.
入口参数: none.
返 回 值: RTN_SUCD: 没有重叠.
          RTN_FAIL: 有重叠.
***********************************************************************************/
U32 TEST_RotationFigOverlapCheck(void)
{
    FIGSTR strFigTemp;
    U32 uiRtn;
    U8 x;
    U8 y;

    if(RTN_FAIL == TEST_GetCurFigPosAfterRtt(&gstrCurFig, &strFigTemp))
    {
        /* 图形无法旋转, 返回失败 */
        return RTN_FAIL;
    }

    /* 填充旋转后的图形Ram */
    TEST_MoveFigRamInit(&strFigTemp.strRam);

    /* 获取图形信号量, 防止其它任务同时对图形进行操作 */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* 将当前图形从主窗口Ram中去掉 */
    for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
    {
        for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
        {
            /* 将主窗口中当前图形的部分置为背景色 */
            if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
            {
                gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = LCDGETBACKGROUNDCOLOR;
            }
        }
    }

    uiRtn = RTN_SUCD;       /* 将返回值默认置为RTN_SUCD */

    /* 检测图形冲突 */
    for(x = 0; x < (strFigTemp.strPos.scRBX - strFigTemp.strPos.scLTX + 1); x++)
    {
        /* 需要返回 */
        if(RTN_FAIL == uiRtn)
        {
            break;
        }

        for(y = 0; y < strFigTemp.strPos.scRBY - strFigTemp.strPos.scLTY + 1; y++)
        {
            /* 如果2个图形相同的位置同不为空则图形有冲突, 返回失败 */
            if((LCDGETBACKGROUNDCOLOR != gausMainPrtRam[strFigTemp.strPos.scLTX + x][strFigTemp.strPos.scLTY + y])
               && (LCDGETBACKGROUNDCOLOR != strFigTemp.strRam.ausFigRam[x][y]))
            {
                /* 需要返回失败 */
                uiRtn = RTN_FAIL;

                break;
            }
        }
    }

    /* 旋转后图形没有冲突 */
    if(RTN_SUCD == uiRtn)
    {
        /* 更新gstrCurFig的参数 */
        gstrCurFig.strRam.usRtt = strFigTemp.strRam.usRtt;
        gstrCurFig.strPos.scLTX = strFigTemp.strPos.scLTX;
        gstrCurFig.strPos.scLTY = strFigTemp.strPos.scLTY;
        gstrCurFig.strPos.scRBX = strFigTemp.strPos.scRBX;
        gstrCurFig.strPos.scRBY = strFigTemp.strPos.scRBY;

        /* 将旋转后的图形填充到gstrCurFig结构的Ram中 */
        TEST_MoveFigRamInit(&gstrCurFig.strRam);
    }

    /* 将当前图形放入主窗口Ram中 */
    for(x = 0; x < (gstrCurFig.strPos.scRBX - gstrCurFig.strPos.scLTX + 1); x++)
    {
        for(y = 0; y < gstrCurFig.strPos.scRBY - gstrCurFig.strPos.scLTY + 1; y++)
        {
            /* 当前图形复制到主窗口中 */
            if((LCDGETBACKGROUNDCOLOR != gstrCurFig.strRam.ausFigRam[x][y]))
            {
                gausMainPrtRam[gstrCurFig.strPos.scLTX + x][gstrCurFig.strPos.scLTY + y] = gstrCurFig.strRam.usClr;
            }
        }
    }

    /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
    (void)MDS_SemGive(gpstrFigSem);

    return uiRtn;
}

/***********************************************************************************
函数功能: 检查主窗口是否满足删除一行图形的条件.
入口参数: uiy: 要删除行的Y轴坐标.
返 回 值: RTN_SUCD: 可以删除一行图形.
          RTN_FAIL: 不能删除一行图形.
***********************************************************************************/
U32 TEST_DelOneLineCheck(U32 uiy)
{
    U8 x;

    for(x = MAINSCR_MIN_X; x < MAINSCR_MAX_X; x++)
    {
        /* 有背景色的像素, 说明没有填满一行, 返回失败 */
        if(LCDGETBACKGROUNDCOLOR == gausMainPrtRam[x][uiy])
        {
            return RTN_FAIL;
        }
    }

    return RTN_SUCD;
}

/***********************************************************************************
函数功能: 闪烁一行.
入口参数: uiy: 需要闪烁行的Y轴坐标.
          uiStart: 0: 更新内部状态, 重新开始.
                   其它: 继续上次状态.
返 回 值: none.
***********************************************************************************/
void TEST_OneLineFlash(U32 uiy, U32 uiStart)
{
    static U16 susColor = LCDCOLORGREY;
    U8 x;

    /* 需要重新开始, 使用初始颜色 */
    if(0 == uiStart)
    {
        susColor = LCDCOLORGREY;
    }

    /* 获取图形信号量, 防止其它任务同时对图形进行操作 */
    (void)MDS_SemTake(gpstrFigSem, SEMWAITFEV);

    /* 将颜色更改到一行图像的Ram中 */
    for(x = 0; x < MAINSCN_X_LEN; x++)
    {
        gausMainPrtRam[x][uiy] = susColor;
    }

    /* 释放图形信号量, 以便其它任务可以刷新屏幕 */
    (void)MDS_SemGive(gpstrFigSem);

    /* 更新下次的颜色 */
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
函数功能: 将任务创建过程信息打印到内存中.
入口参数: pstrTcb: 新创建的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void TEST_TaskCreatePrint(M_TCB* pstrTcb)
{
    /* 任务创建成功 */
    if((M_TCB*)NULL != pstrTcb)
    {
        /* 打印任务创建成功信息 */
        DEV_PutStrToMem((U8*)"\r\nTask %s is created! Tick is: %d",
                        pstrTcb->pucTaskName, MDS_GetSystemTick());
    }
    else /* 任务创建失败 */
    {
        /* 打印任务创建失败信息 */
        DEV_PutStrToMem((U8*)"\r\nFail to create task! Tick is: %d",
                        MDS_GetSystemTick());
    }
}

/***********************************************************************************
函数功能: 将任务切换过程信息打印到内存中.
入口参数: pstrOldTcb: 切换前的任务TCB指针.
          pstrNewTcb: 切换后的任务TCB指针.
返 回 值: none.
***********************************************************************************/
void TEST_TaskSwitchPrint(M_TCB* pstrOldTcb, M_TCB* pstrNewTcb)
{
    /* 为不打印串口打印任务的切换过程, 将这个任务认为是空闲任务 */
    if(pstrOldTcb == gpstrSerialTaskTcb)
    {
        pstrOldTcb = MDS_GetIdleTcb();
    }

    if(pstrNewTcb == gpstrSerialTaskTcb)
    {
        pstrNewTcb = MDS_GetIdleTcb();
    }

    /* 同一个任务之间切换不打印信息 */
    if(pstrOldTcb == pstrNewTcb)
    {
        return;
    }

    /* 没有删除切换前任务 */
    if(NULL != pstrOldTcb)
    {
        /* 向内存打印任务切换信息 */
        DEV_PutStrToMem((U8*)"\r\nTask %s ---> Task %s! Tick is: %d",
                        pstrOldTcb->pucTaskName, pstrNewTcb->pucTaskName,
                        MDS_GetSystemTick());
    }
    else /* 切换前的任务被删除 */
    {
        /* 向内存打印任务切换信息 */
        DEV_PutStrToMem((U8*)"\r\nTask NULL ---> Task %s! Tick is: %d",
                        pstrNewTcb->pucTaskName, MDS_GetSystemTick());
    }
}

/***********************************************************************************
函数功能: 将任务删除过程信息打印到内存中.
入口参数: pstrTcb: 被删除的任务的TCB指针.
返 回 值: none.
***********************************************************************************/
void TEST_TaskDeletePrint(M_TCB* pstrTcb)
{
    /* 打印任务删除信息 */
    DEV_PutStrToMem((U8*)"\r\nTask %s is deleted! Tick is: %d",
                    pstrTcb->pucTaskName, MDS_GetSystemTick());
}

