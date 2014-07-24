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

; EQU: ����һ��������ջ�Ĵ�С
Stack_Size      EQU     2048	

; AREA: ����һ������λ����ݶ� ��
;                       ���������ԣ��������ڴ浥Ԫ ���ɶ�д �� 2^3 =8 �ֽڶ���
                AREA    STACK, NOINIT, READWRITE, ALIGN=3	

; SPACE: ����һ���ڴ����򣬲���0��ʼ�����˴�Ϊ2048�ֽ�
Stack_Mem       SPACE   Stack_Size

; һ����ţ� ����ѵĶ�����ַ
__initial_sp

; ����һ���ѵĴ�С
Heap_Size       EQU     0;2048

; ����һ������λ����ݶ� ��
;                       ���������ԣ��������ڴ浥Ԫ ���ɶ�д �� 2^3 =8 �ֽڶ���
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base

; ����һ���ڴ����򣬲���0��ʼ�����˴�Ϊ2048�ֽ�
Heap_Mem        SPACE   Heap_Size

; ��ţ������ĩβ��ַ
__heap_limit

                PRESERVE8			; ָʾ������8�ֽڶ���
                THUMB				; ָʾ������Ϊthumbָ��

; Vector Table Mapped to Address 0 at Reset  ʸ����ӳ�䵽��ַ0�� ��λ

                AREA    RESET, DATA, READONLY     ; ����RESET���ݶΣ�ֻ��
                EXPORT  __Vectors				  ; �����÷��ſ��Ա������ļ����ã��൱��������һ��ȫ�ֱ��� �˴�Ϊ�жϵ�ַ
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

; DCD:����һ�����ڴ浥Ԫ��Ҫ���ֶ���
; �ڴ����ʼ��ַ���    �������ʽ������еı�ţ�������ֽ����ɳ����ͱ�ŵĸ�������
__Vectors       DCD     __initial_sp              ; Top of Stack   //Cotex-M Ҫ��˴�Ϊ��ջ������ַ:0x00000000�� 4�ֽڿռ�
                DCD     Reset_Handler             ; Reset Handler   		��ַ��0x00000004 , 4�ֽڿռ� 
                DCD     NMI_Handler               ; NMI Handler				��ַ��0x00000008 , 4�ֽڿռ� 
                DCD     HardFault_Handler         ; Hard Fault Handler		��ַ��0x0000000C , 4�ֽڿռ� 
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

; ������Ĵ�С
__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY     ; ����ζ���

; Reset Handler ��λ����
; RAM ���صļĴ�����ַ
NRF_POWER_RAMON_ADDRESS           EQU   0x40000524  ; NRF_POWER->RAMON address
NRF_POWER_RAMON_RAMxON_ONMODE_Msk EQU   0xF         ; All RAM blocks on in onmode bit mask

; PROC�� �ӳ���ʼ
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]  ; ����һ�����ţ�������������ͬ�����������ڱ����ű�ʹ��
                IMPORT  SystemInit						  ; ������ǰ�����������ļ�����
                IMPORT  __main
                LDR     R0, =NRF_POWER_RAMON_ADDRESS	  ; ����һ����������ָ���Ĵ���
                LDR     R2, [R0]						  ; [R0]: ��ַR0��ָ�������
                MOVS    R1, #NRF_POWER_RAMON_RAMxON_ONMODE_Msk  ; #x : ��x����ֵ��R1
                ORRS    R2, R2, R1
                STR     R2, [R0] 						   ; ��R2�е����ݱ�����[R0] ��
                LDR     R0, =SystemInit
                BLX     R0								   ; ��������ת�����л�ָ�
                LDR     R0, =__main
                BX      R0
                ENDP									  ; �ӳ������

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .						          ; B����תָ�� B . ��ѭ��
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
                ALIGN						 ; ����αָ�Ϊ���ֽڶ���

; User Initial Stack & Heap

                IF      :DEF:__MICROLIB   	 ; :DEF: X���÷�����X��������Ϊ�棬����Ϊ��
                
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
                BX      LR   				   ; LR Ϊ���ӼĴ��� �������ػ����жϷ���

                ALIGN

                ENDIF

                END

