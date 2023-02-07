
;*********************************** 变量声明 **************************************
    EXTERN gpstrCurTaskReg
    EXTERN gpstrNextTaskReg


;*********************************** 函数声明 **************************************
    EXTERN MDS_TaskSched
    EXTERN MDS_SaveTaskContext
    EXTERN MDS_FaultIsrPrint;

    EXPORT MDS_PendSvContextSwitch
    EXPORT MDS_SwitchToTask
    EXPORT MDS_TaskOccurSwi
    EXPORT MDS_GetXpsr
    EXPORT MDS_FaultIsrContext


    END

