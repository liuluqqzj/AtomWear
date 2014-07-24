; Copyright (c) 2009 Nordic Semiconductor. All Rights Reserved.
;
; The information contained herein is property of Nordic Semiconductor ASA.
; Terms and conditions of usage are described in detail in NORDIC
; SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
;
; Licensees are granted free, non-transferable use of the information. NO
; WARRANTY of ANY KIND is provided. This heading must NOT be removed from
; the file.
 
; NOTE: Template files (including this one) are application specific and therefore 
; expected to be copied into the application project folder prior to its use!

; Description message

; EQU: 定义一个常量：栈的大小
Stack_Size      EQU     2048	

; AREA: 定义一个代码段或数据段 ：
;                       段名，属性，仅保留内存单元 ，可读写 ， 2^3 =8 字节对齐
                AREA    STACK, NOINIT, READWRITE, ALIGN=3	

; SPACE: 分配一块内存区域，并用0初始化，此处为2048字节
Stack_Mem       SPACE   Stack_Size

; 一个标号， 代表堆的顶部地址
__initial_sp

; 定义一个堆的大小
Heap_Size       EQU     0;2048

; 定义一个代码段或数据段 ：
;                       段名，属性，仅保留内存单元 ，可读写 ， 2^3 =8 字节对齐
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base

; 分配一块内存区域，并用0初始化，此处为2048字节
Heap_Mem        SPACE   Heap_Size

; 标号，代表堆末尾地址
__heap_limit

                PRESERVE8			; 指示编译器8字节对齐
                THUMB				; 指示编译器为thumb指令

; Vector Table Mapped to Address 0 at Reset  矢量表映射到地址0处 复位

                AREA    RESET, DATA, READONLY     ; 定义RESET数据段，只读
                EXPORT  __Vectors				  ; 申明该符号可以被其它文件引用，相当于申明了一个全局变量 此处为中断地址
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

; DCD:分配一段字内存单元，要求字对齐
; 内存块起始地址标号    常数表达式或程序中的标号，分配的字节数由常数和标号的个数决定
__Vectors       DCD     __initial_sp              ; Top of Stack   //Cotex-M 要求此处为堆栈顶部地址:0x00000000， 4字节空间
                DCD     Reset_Handler             ; Reset Handler   		地址：0x00000004 , 4字节空间 
                DCD     NMI_Handler               ; NMI Handler				地址：0x00000008 , 4字节空间 
                DCD     HardFault_Handler         ; Hard Fault Handler		地址：0x0000000C , 4字节空间 
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD      POWER_CLOCK_IRQHandler ;POWER_CLOCK
                DCD      RADIO_IRQHandler ;RADIO
                DCD      UART0_IRQHandler ;UART0
                DCD      SPI0_TWI0_IRQHandler ;SPI0_TWI0
                DCD      SPI1_TWI1_IRQHandler ;SPI1_TWI1
                DCD      0 ;Reserved
                DCD      GPIOTE_IRQHandler ;GPIOTE
                DCD      ADC_IRQHandler ;ADC
                DCD      TIMER0_IRQHandler ;TIMER0
                DCD      TIMER1_IRQHandler ;TIMER1
                DCD      TIMER2_IRQHandler ;TIMER2
                DCD      RTC0_IRQHandler ;RTC0
                DCD      TEMP_IRQHandler ;TEMP
                DCD      RNG_IRQHandler ;RNG
                DCD      ECB_IRQHandler ;ECB
                DCD      CCM_AAR_IRQHandler ;CCM_AAR
                DCD      WDT_IRQHandler ;WDT
                DCD      RTC1_IRQHandler ;RTC1
                DCD      QDEC_IRQHandler ;QDEC
                DCD      LPCOMP_COMP_IRQHandler ;LPCOMP_COMP
                DCD      SWI0_IRQHandler ;SWI0
                DCD      SWI1_IRQHandler ;SWI1
                DCD      SWI2_IRQHandler ;SWI2
                DCD      SWI3_IRQHandler ;SWI3
                DCD      SWI4_IRQHandler ;SWI4
                DCD      SWI5_IRQHandler ;SWI5
                DCD      0 ;Reserved
                DCD      0 ;Reserved
                DCD      0 ;Reserved
                DCD      0 ;Reserved
                DCD      0 ;Reserved
                DCD      0 ;Reserved


__Vectors_End

; 向量表的大小
__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY     ; 代码段定义

; Reset Handler 复位处理
; RAM 开关的寄存器地址
NRF_POWER_RAMON_ADDRESS           EQU   0x40000524  ; NRF_POWER->RAMON address
NRF_POWER_RAMON_RAMxON_ONMODE_Msk EQU   0xF         ; All RAM blocks on in onmode bit mask

; PROC： 子程序开始
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]  ; 定义一个符号，并申明其它的同名符号优先于本符号被使用
                IMPORT  SystemInit						  ; 申明当前符号在其它文件定义
                IMPORT  __main
                LDR     R0, =NRF_POWER_RAMON_ADDRESS	  ; 加载一个立即数到指定寄存器
                LDR     R2, [R0]						  ; [R0]: 地址R0所指向的数据
                MOVS    R1, #NRF_POWER_RAMON_RAMxON_ONMODE_Msk  ; #x : 将x处的值给R1
                ORRS    R2, R2, R1
                STR     R2, [R0] 						   ; 将R2中的内容保存在[R0] 中
                LDR     R0, =SystemInit
                BLX     R0								   ; 带链接跳转，并切换指令集
                LDR     R0, =__main
                BX      R0
                ENDP									  ; 子程序结束

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .						          ; B是跳转指令 B . 死循环
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT   POWER_CLOCK_IRQHandler [WEAK]
                EXPORT   RADIO_IRQHandler [WEAK]
                EXPORT   UART0_IRQHandler [WEAK]
                EXPORT   SPI0_TWI0_IRQHandler [WEAK]
                EXPORT   SPI1_TWI1_IRQHandler [WEAK]
                EXPORT   GPIOTE_IRQHandler [WEAK]
                EXPORT   ADC_IRQHandler [WEAK]
                EXPORT   TIMER0_IRQHandler [WEAK]
                EXPORT   TIMER1_IRQHandler [WEAK]
                EXPORT   TIMER2_IRQHandler [WEAK]
                EXPORT   RTC0_IRQHandler [WEAK]
                EXPORT   TEMP_IRQHandler [WEAK]
                EXPORT   RNG_IRQHandler [WEAK]
                EXPORT   ECB_IRQHandler [WEAK]
                EXPORT   CCM_AAR_IRQHandler [WEAK]
                EXPORT   WDT_IRQHandler [WEAK]
                EXPORT   RTC1_IRQHandler [WEAK]
                EXPORT   QDEC_IRQHandler [WEAK]
                EXPORT   LPCOMP_COMP_IRQHandler [WEAK]
                EXPORT   SWI0_IRQHandler [WEAK]
                EXPORT   SWI1_IRQHandler [WEAK]
                EXPORT   SWI2_IRQHandler [WEAK]
                EXPORT   SWI3_IRQHandler [WEAK]
                EXPORT   SWI4_IRQHandler [WEAK]
                EXPORT   SWI5_IRQHandler [WEAK]
POWER_CLOCK_IRQHandler
RADIO_IRQHandler
UART0_IRQHandler
SPI0_TWI0_IRQHandler
SPI1_TWI1_IRQHandler
GPIOTE_IRQHandler
ADC_IRQHandler
TIMER0_IRQHandler
TIMER1_IRQHandler
TIMER2_IRQHandler
RTC0_IRQHandler
TEMP_IRQHandler
RNG_IRQHandler
ECB_IRQHandler
CCM_AAR_IRQHandler
WDT_IRQHandler
RTC1_IRQHandler
QDEC_IRQHandler
LPCOMP_COMP_IRQHandler
SWI0_IRQHandler
SWI1_IRQHandler
SWI2_IRQHandler
SWI3_IRQHandler
SWI4_IRQHandler
SWI5_IRQHandler

                B .
                ENDP
                ALIGN						 ; 杂项伪指令，为了字节对齐

; User Initial Stack & Heap

                IF      :DEF:__MICROLIB   	 ; :DEF: X的用法，若X定义了则为真，否则为假
                
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, = Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem + Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR   				   ; LR 为链接寄存器 函数返回或者中断返回

                ALIGN

                ENDIF

                END

