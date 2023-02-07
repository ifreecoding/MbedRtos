
    INCLUDE wlx_core_a.h


    ;栈8字节对齐
    PRESERVE8

    AREA    WLX_CORE_A, CODE, READONLY
    THUMB


    ;函数功能: 实现任务切换功能, 将寄存器备份到当前任务栈中, 从将要运行任务栈中取出
    ;          寄存器保存值并跳转到将要运行任务执行.
    ;入口参数: none.
    ;返 回 值: none.
WLX_ContextSwitch

    ;保存当前任务的栈信息
    MOV    R3, R0               ;将寄存器组地址存入R3寄存器
    STMIA  R3!, {R0, R4 - R12}  ;保存R0,R4-R12寄存器
    STMIA  R3!, {R13}           ;保存SP寄存器
    STMIA  R3!, {R14}           ;保存LR寄存器
    MRS    R2, XPSR             ;获取XPSR寄存器数值
    STMIA  R3, {R2}             ;保存到寄存器组中的XPSR

    ;恢复将要运行任务的栈信息并运行新任务
    LDMIA  R1!, {R0, R4 - R12}  ;恢复R0,R4-R12寄存器
    LDMIA  R1, {R13}            ;恢复SP寄存器
    ADD    R1, #8               ;R1指向寄存器组中的XPSR
    LDMIA  R1, {R2}             ;获取寄存器组中的XPSR数值
    MSR    XPSR, R2             ;恢复XPSR寄存器
    SUB    R1, #4               ;R1指向寄存器组中的LR
    LDMIA  R1, {PC}             ;切换任务


    ;函数功能: 实现从非操作系统状态切换到操作系统状态, 从第一个任务栈中取出寄存器初
    ;          始值并跳转到该任务执行.
    ;入口参数: none.
    ;返 回 值: none.
WLX_SwitchToTask

    ;恢复将要运行任务的栈信息并运行新任务
    MOV    R2, R0               ;将寄存器组地址存入R2寄存器
    LDMIA  R2!, {R0, R4 - R12}  ;恢复R0,R4-R12寄存器
    LDMIA  R2, {R13}            ;恢复SP寄存器
    ADD    R2, #8               ;R2指向寄存器组中XPSR
    LDMIA  R2, {R1}             ;获取寄存器组中的XPSR数值
    MSR    XPSR, R1             ;恢复XPSR寄存器
    SUB    R2, #4               ;R2指向寄存器组中的LR
    LDMIA  R2, {PC}             ;运行首个任务


    ALIGN

    END

