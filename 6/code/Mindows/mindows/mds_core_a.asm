
    INCLUDE mds_core_a_inner.h


    ;ջ8�ֽڶ���
    PRESERVE8

    AREA    MDS_CORE_A, CODE, READONLY
    THUMB


    ;��������: ʵ�������л�����, ���Ĵ������ݵ���ǰ����ջ��, �ӽ�Ҫ��������ջ��ȡ��
    ;          �Ĵ�������ֵ����ת����Ҫ��������ִ��. ��Tick�жϻ�SVC�жϴ���PENDSV
    ;          �ж�, �ú���ΪPENDSV���жϷ�����.
    ;��ڲ���: none.
    ;�� �� ֵ: none.
MDS_PendSvContextSwitch

    ;����ӿڼĴ���
    PUSH   {R14}

    ;����C����������Ⱥ���
    LDR    R0, =MDS_TaskSched       ;������ַ����R0
    ADR.W  R14, {PC} + 0x7          ;���淵�ص�ַ
    BX     R0                       ;ִ��MDS_TaskSched����

    ;���浱ǰ�����ջ��Ϣ
    MOV    R14, R13                 ;��SP����LR
    LDR    R0, =gpstrCurTaskReg     ;��ȡ����gpstrCurTaskReg�ĵ�ַ
    LDR    R12, [R0]                ;����ǰ����Ĵ������ַ����R12
    MOV    R0, R12                  ;��R12����R0
    CBZ    R0, __BACKUP_REG         ;R0Ϊ0����ת��__BACKUP_REG
    ADD    R14, #0x4                ;LRָ��ջ��8���Ĵ����е�R0
    LDMIA  R14!, {R0 - R3}          ;ȡ��R0-R3��ֵ
    STMIA  R12!, {R0 - R11}         ;��R0-R11���浽�Ĵ�������
    LDMIA  R14, {R0 - R3}           ;ȡ��R12,LR,PC��XPSRֵ
    SUB    R14, #0x10               ;LRָ��ջ��8���Ĵ����е�R0
    STMIA  R12!, {R0}               ;��R12���浽�Ĵ�������
    STMIA  R12!, {R14}              ;��SP���浽�Ĵ�������
    STMIA  R12!, {R1 - R3}          ;��LR,PC��XPSR���浽�Ĵ�������
    POP    {R0}                     ;ȡ��ѹ��ջ�е�LR
    STMIA  R12, {R0}                ;��LR���浽�Ĵ������е�Exc_Rtn

    ;���л�ǰ����ļĴ������ջ��Ϣ���浽�ڴ�
    LDR    R0, =MDS_SaveTaskContext ;������ַ����R0
    ADR.W  R14, {PC} + 0x7          ;���淵�ص�ַ
    BX     R0                       ;ִ��MDS_SaveTaskContext����

__BACKUP_REG

    ;����������, �ָ���Ҫ���������ֳ�
    LDR    R0, =gpstrNextTaskReg    ;��ȡ����gpstrNextTaskReg�ĵ�ַ
    LDR    R1, [R0]                 ;����Ҫ��������ļĴ������ַ����R1
    ADD    R1, #0x10                ;R1ָ��Ĵ������е�R4
    LDMIA  R1!, {R4 - R11}          ;�ָ�R4~R11��ֵ
    ADD    R1, #0x4                 ;R1ָ��Ĵ������е�SP
    LDMIA  R1, {R13}                ;�ָ�SP��ֵ
    ADD    R1, #0x10                ;R1ָ��Ĵ������е�Exc_Rtn
    LDMIA  R1, {R0}                 ;ȡ���Ĵ������е�Exc_Rtn��ֵ
    BX     R0                       ;ִ�м������е�����


    ;��������: ʵ�ִӷǲ���ϵͳ״̬�л�������ϵͳ״̬, �ӵ�һ������ջ��ȡ���Ĵ�����
    ;          ʼֵ����ת��������ִ��. �ú����ɳ���ֱ�ӵ���.
    ;��ڲ���: none.
    ;�� �� ֵ: none.
MDS_SwitchToTask

    ;��ȡ��Ҫ���������ָ��
    LDR    R0, =gpstrNextTaskReg    ;��ȡ����gpstrNextTaskReg�ĵ�ַ
    LDR    R13, [R0]                ;����Ҫ��������ļĴ������ַ����SP

    ;��ȡ��Ҫ���������ջ��Ϣ������������
    ADD    R13, #0x40               ;SPָ��Ĵ������е�XPSR
    POP    {R0}                     ;ȡ���Ĵ������е�XPSR��ֵ
    MSR    XPSR, R0                 ;�ָ�XPSR��ֵ
    SUB    R13, #0x8                ;SPָ��Ĵ������е�PC
    LDMIA  R13, {R0}                ;ȡ���Ĵ������е�PC��ֵ
    SUB    R13, #0x3C               ;SPָ��Ĵ������е�R0
    STMDB  R13, {R0}                ;��PCֵ����Ĵ�������R0�����λ��
    POP    {R0 - R12}               ;�ָ�R0-R12��ֵ
    ADD    R13, #0x4                ;SPָ��Ĵ������е�LR
    POP    {LR}                     ;�ָ�LR��ֵ
    SUB    R13, #0x40               ;SPָ��Ĵ�������XPSR�����λ��
    POP    {PC}                     ;ִ�м������е�����


    ;��������: �ɳ���ֱ�ӵ���, ����SWI���ж�.
    ;��ڲ���: ���жϷ����.
    ;�� �� ֵ: none.
MDS_TaskOccurSwi

    SVC    #0           ;����SWI���ж�
    BX     R14          ;��������


    ;��������: ��ȡXPSR�Ĵ�������ֵ.
    ;��ڲ���: none.
    ;�� �� ֵ: XPSR�Ĵ�����ֵ.
MDS_GetXpsr

    MRS    R0, XPSR     ;��ȡXPSR��ֵ
    BX     R14          ;��������


    ;��������: ���Ĵ������ݵ���ǰ����ջ��, ������Ӳ�������жϷ�����.
    ;��ڲ���: none.
    ;�� �� ֵ: none.
MDS_FaultIsrContext

    ;���浱ǰ�����ջ��Ϣ
    PUSH   {R14}                    ;������ֵѹ��ջ��
    MOV    R14, R13                 ;��SP����LR
    LDR    R0, =gpstrNextTaskReg    ;��ȡ����gpstrNextTaskReg�ĵ�ַ
    LDR    R12, [R0]                ;����ǰ����Ĵ������ַ����R12
    ADD    R14, #0x4                ;LRָ��ջ��8���Ĵ����е�R0
    LDMIA  R14!, {R0 - R3}          ;ȡ��R0-R3��ֵ
    STMIA  R12!, {R0 - R11}         ;��R0-R11���浽�Ĵ�������
    LDMIA  R14, {R0 - R3}           ;ȡ��R12,LR,PC��XPSRֵ
    SUB    R14, #0x10               ;LRָ��ջ��8���Ĵ����е�R0
    STMIA  R12!, {R0}               ;��R12���浽�Ĵ�������
    STMIA  R12!, {R14}              ;��SP���浽�Ĵ�������
    STMIA  R12!, {R1 - R3}          ;��LR,PC��XPSR���浽�Ĵ�������
    POP    {R0}                     ;ȡ��ѹ��ջ�е�LR
    STMIA  R12, {R0}                ;��LR���浽�Ĵ������е�Exc_Rtn

    ;��ӡ���б������Ϣ
    LDR    R0, =MDS_FaultIsrPrint   ;������ַ����R0
    ADR.W  R14, {PC} + 0x7          ;���淵�ص�ַ
    BX     R0                       ;ִ��MDS_FaultIsrPrint����


    ALIGN

    END

