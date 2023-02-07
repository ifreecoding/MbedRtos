
;*********************************** 变量声明 **************************************
    EXTERN gpstrCurTaskReg
    EXTERN gpstrNextTaskReg


;*********************************** 函数声明 **************************************
    EXTERN MDS_TaskSched

    EXPORT MDS_PendSvContextSwitch
    EXPORT MDS_SwitchToTask
    EXPORT MDS_TaskOccurSwi
    EXPORT MDS_GetXpsr


    END

