
    INCLUDE mds_core_a_inner.h


    ;栈8字节对齐
    PRESERVE8

    AREA    MDS_CORE_A, CODE, READONLY
    THUMB


    ;函数功能: 实现任务切换功能, 将寄存器备份到当前任务栈中, 从将要运行任务栈中取出
    ;          寄存器保存值并跳转到将要运行任务执行. 由Tick中断或SVC中断触发PENDSV
    ;          中断, 该函数为PENDSV的中断服务函数.
    ;入口参数: none.
    ;返 回 值: none.
MDS_PendSvContextSwitch

    ;保存接口寄存器
    PUSH   {R14}

    ;调用C语言任务调度函数
    LDR    R0, =MDS_TaskSched       ;函数地址存入R0
    ADR.W  R14, {PC} + 0x7          ;保存返回地址
    BX     R0                       ;执行MDS_TaskSched函数

    ;保存当前任务的栈信息
    MOV    R14, R13                 ;将SP存入LR
    LDR    R0, =gpstrCurTaskReg     ;获取变量gpstrCurTaskReg的地址
    LDR    R12, [R0]                ;将当前任务寄存器组地址存入R12
    MOV    R0, R12                  ;将R12存入R0
    CBZ    R0, __BACKUP_REG         ;R0为0则跳转到__BACKUP_REG
    ADD    R14, #0x4                ;LR指向栈中8个寄存器中的R0
    LDMIA  R14!, {R0 - R3}          ;取出R0-R3数值
    STMIA  R12!, {R0 - R11}         ;将R0-R11保存到寄存器组中
    LDMIA  R14, {R0 - R3}           ;取出R12,LR,PC和XPSR值
    SUB    R14, #0x10               ;LR指向栈中8个寄存器中的R0
    STMIA  R12!, {R0}               ;将R12保存到寄存器组中
    STMIA  R12!, {R14}              ;将SP保存到寄存器组中
    STMIA  R12!, {R1 - R3}          ;将LR,PC和XPSR保存到寄存器组中
    POP    {R0}                     ;取出压入栈中的LR
    STMIA  R12, {R0}                ;将LR保存到寄存器组中的Exc_Rtn

    ;将切换前任务的寄存器组和栈信息保存到内存
    LDR    R0, =MDS_SaveTaskContext ;函数地址存入R0
    ADR.W  R14, {PC} + 0x7          ;保存返回地址
    BX     R0                       ;执行MDS_SaveTaskContext函数

__BACKUP_REG

    ;任务调度完毕, 恢复将要运行任务现场
    LDR    R0, =gpstrNextTaskReg    ;获取变量gpstrNextTaskReg的地址
    LDR    R1, [R0]                 ;将将要运行任务的寄存器组地址存入R1
    ADD    R1, #0x10                ;R1指向寄存器组中的R4
    LDMIA  R1!, {R4 - R11}          ;恢复R4~R11数值
    ADD    R1, #0x4                 ;R1指向寄存器组中的SP
    LDMIA  R1, {R13}                ;恢复SP数值
    ADD    R1, #0x10                ;R1指向寄存器组中的Exc_Rtn
    LDMIA  R1, {R0}                 ;取出寄存器组中的Exc_Rtn数值
    BX     R0                       ;执行即将运行的任务


    ;函数功能: 实现从非操作系统状态切换到操作系统状态, 从第一个任务栈中取出寄存器初
    ;          始值并跳转到该任务执行. 该函数由程序直接调用.
    ;入口参数: none.
    ;返 回 值: none.
MDS_SwitchToTask

    ;获取将要运行任务的指针
    LDR    R0, =gpstrNextTaskReg    ;获取变量gpstrNextTaskReg的地址
    LDR    R13, [R0]                ;将将要运行任务的寄存器组地址存入SP

    ;获取将要运行任务的栈信息并运行新任务
    ADD    R13, #0x40               ;SP指向寄存器组中的XPSR
    POP    {R0}                     ;取出寄存器组中的XPSR数值
    MSR    XPSR, R0                 ;恢复XPSR数值
    SUB    R13, #0x8                ;SP指向寄存器组中的PC
    LDMIA  R13, {R0}                ;取出寄存器组中的PC数值
    SUB    R13, #0x3C               ;SP指向寄存器组中的R0
    STMDB  R13, {R0}                ;将PC值存入寄存器组中R0下面的位置
    POP    {R0 - R12}               ;恢复R0-R12数值
    ADD    R13, #0x4                ;SP指向寄存器组中的LR
    POP    {LR}                     ;恢复LR数值
    SUB    R13, #0x40               ;SP指向寄存器组中XPSR下面的位置
    POP    {PC}                     ;执行即将运行的任务


    ;函数功能: 由程序直接调用, 触发SWI软中断.
    ;入口参数: 软中断服务号.
    ;返 回 值: none.
MDS_TaskOccurSwi

    SVC    #0           ;触发SWI软中断
    BX     R14          ;函数返回


    ;函数功能: 获取XPSR寄存器的数值.
    ;入口参数: none.
    ;返 回 值: XPSR寄存器数值.
MDS_GetXpsr

    MRS    R0, XPSR     ;获取XPSR数值
    BX     R14          ;函数返回


    ;函数功能: 将寄存器备份到当前任务栈中, 并调用硬件故障中断服务函数.
    ;入口参数: none.
    ;返 回 值: none.
MDS_FaultIsrContext

    ;保存当前任务的栈信息
    PUSH   {R14}                    ;将返回值压入栈中
    MOV    R14, R13                 ;将SP存入LR
    LDR    R0, =gpstrNextTaskReg    ;获取变量gpstrNextTaskReg的地址
    LDR    R12, [R0]                ;将当前任务寄存器组地址存入R12
    ADD    R14, #0x4                ;LR指向栈中8个寄存器中的R0
    LDMIA  R14!, {R0 - R3}          ;取出R0-R3数值
    STMIA  R12!, {R0 - R11}         ;将R0-R11保存到寄存器组中
    LDMIA  R14, {R0 - R3}           ;取出R12,LR,PC和XPSR值
    SUB    R14, #0x10               ;LR指向栈中8个寄存器中的R0
    STMIA  R12!, {R0}               ;将R12保存到寄存器组中
    STMIA  R12!, {R14}              ;将SP保存到寄存器组中
    STMIA  R12!, {R1 - R3}          ;将LR,PC和XPSR保存到寄存器组中
    POP    {R0}                     ;取出压入栈中的LR
    STMIA  R12, {R0}                ;将LR保存到寄存器组中的Exc_Rtn

    ;打印所有保存的信息
    LDR    R0, =MDS_FaultIsrPrint   ;函数地址存入R0
    ADR.W  R14, {PC} + 0x7          ;保存返回地址
    BX     R0                       ;执行MDS_FaultIsrPrint函数


    ALIGN

    END

