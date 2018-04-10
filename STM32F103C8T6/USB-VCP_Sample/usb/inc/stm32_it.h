#ifndef __STM32_IT_H
#define __STM32_IT_H

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void USB_LP_CAN1_RX0_IRQHandler(void);

#endif /* __STM32_IT_H */
