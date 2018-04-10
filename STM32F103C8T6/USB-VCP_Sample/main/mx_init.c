/*
	Name:			mx_init.c
	Purpose:		FirmWare for STM32F103C8T6: DS18B20, iButton, GPIO
	Author:			Alexander Suvorov (www.open-plc.com)
	Created:		2016/07
	Modified by:	2017/12
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/


#include "mx_init.h"

void MX_Init( void )
{
	GPIO_InitTypeDef			GPIO_InitStructure;
	USART_InitTypeDef			USART_InitStructure;
	NVIC_InitTypeDef			NVIC_InitStructure;

	// -----------------------------------------------------------------------
	// Oscilator
	// -----------------------------------------------------------------------
	RCC_DeInit();													// Resets the RCC clock configuration to the default reset state.

	#if F_APB1 == 48000000
		#ifdef OSC_INT													// === Internal ===
			RCC_HSEConfig( RCC_HSE_OFF );								// Configures the Internal oscillator (HSI).
			RCC_HSICmd( ENABLE );										// Enables the Internal High Speed oscillator (HSI).
			RCC_PLLConfig( RCC_PLLSource_HSI_Div2, RCC_PLLMul_12 );		// Configures the PLL clock source and multiplication factor.
			RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );				// Configures the system clock as PLL source (SYSCLK).
			RCC_HCLKConfig( RCC_SYSCLK_Div1 );							// Configures the AHB clock (HCLK).
			RCC_PCLK1Config( RCC_HCLK_Div2 );							// Configures the Low Speed APB clock (PCLK1).
			RCC_PCLK2Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK2).
			RCC_ADCCLKConfig( RCC_PCLK2_Div4 );							// Configures the ADC clock (ADCCLK).
			RCC_USBCLKConfig( RCC_USBCLKSource_PLLCLK_Div1 );
		#else															// === External ===
			RCC_HSICmd( DISABLE );										// Enables the External High Speed oscillator (HSE).
			RCC_HSEConfig( RCC_HSE_ON );								// Configures the External High Speed oscillator (HSE).
			RCC_PLLConfig( RCC_PLLSource_HSE_Div2, RCC_PLLMul_12 );		// Configures the PLL clock source and multiplication factor.
			RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );				// Configures the system clock as PLL source (SYSCLK).
			RCC_HCLKConfig( RCC_SYSCLK_Div1 );							// Configures the AHB clock (HCLK).
			RCC_PCLK1Config( RCC_HCLK_Div2 );							// Configures the Low Speed APB clock (PCLK1).
			RCC_PCLK2Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK2).
			RCC_ADCCLKConfig( RCC_PCLK2_Div4 );							// Configures the ADC clock (ADCCLK).
			RCC_USBCLKConfig( RCC_USBCLKSource_PLLCLK_Div1 );			// Configures the USB clock.
		#endif
	#else
		#ifdef OSC_INT													// === Internal ===
			RCC_HSEConfig( RCC_HSE_OFF );								// Configures the Internal oscillator (HSI).
			RCC_HSICmd( ENABLE );										// Enables the Internal High Speed oscillator (HSI).
			RCC_PLLConfig( RCC_PLLSource_HSI_Div2, RCC_PLLMul_6 );		// Configures the PLL clock source and multiplication factor.
			RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );				// Configures the system clock as PLL source (SYSCLK).
			RCC_HCLKConfig( RCC_SYSCLK_Div1 );							// Configures the AHB clock (HCLK).
			RCC_PCLK1Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK1).
			RCC_PCLK2Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK2).
//			RCC_ADCCLKConfig( RCC_PCLK2_Div2 );							// Configures the ADC clock (ADCCLK).
		#else															// === External ===
			RCC_HSICmd( DISABLE );										// Enables the External High Speed oscillator (HSE).
			RCC_HSEConfig( RCC_HSE_ON );								// Configures the External High Speed oscillator (HSE).
			RCC_PLLConfig( RCC_PLLSource_HSE_Div2, RCC_PLLMul_6 );		// Configures the PLL clock source and multiplication factor.
			RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );				// Configures the system clock as PLL source (SYSCLK).
			RCC_HCLKConfig( RCC_SYSCLK_Div1 );							// Configures the AHB clock (HCLK).
			RCC_PCLK1Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK1).
			RCC_PCLK2Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK2).
//			RCC_ADCCLKConfig( RCC_PCLK2_Div2 );							// Configures the ADC clock (ADCCLK).
		#endif
	#endif
	RCC_PLLCmd( ENABLE );											// Enables the PLL.
	while( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET ) {}		// Wait till PLL is ready
	while( RCC_GetSYSCLKSource() != 0x08 ) {}						// Wait till PLL is used as system clock source


	// -----------------------------------------------------------------------
	// Enable peripheral clock
	// -----------------------------------------------------------------------
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );			// Clocking GPIOA
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );			// Clocking GPIOB
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE );			// Clocking GPIOC
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE );		// Clocking USART1
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE );		// Clocking USART3
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );			// Clocking AF APB2
	#if F_APB1 == 48000000
		RCC_APB1PeriphClockCmd( RCC_APB1Periph_USB, ENABLE );		// Clocking USB
	#endif


	// -----------------------------------------------------------------------
	// Start DWT
	// -----------------------------------------------------------------------
	SCB_DEMCR		|= 0x01000000;	// enable trace
	DWT_LAR			 = 0xC5ACCE55;	// unlock access to DWT (ITM, etc.) registers
	DWT_CONTROL		|= 1;			// enable DWT cycle counter
	DWT_CYCCNT		 = 0;			// clear DWT cycle counter


	// -----------------------------------------------------------------------
	// Init GPIOS
	// -----------------------------------------------------------------------

	// Configure GPIO pin : PC13 ( LED blink )
	#ifdef LED_BLINK
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOC, &GPIO_InitStructure );
	GPIO_SetBits( GPIOC, GPIO_Pin_13 );								// Clear bit "blinking"
	#endif

	// Configure GPIO pin : PA12 - USB DP
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	GPIO_ResetBits( GPIOA, GPIO_Pin_12 );								// Clear bit "blinking"

	// Configure GPIO pin : PB2 - BOOT1
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );



	// -----------------------------------------------------------------------
	// Init UARTS
	// -----------------------------------------------------------------------
	// USART1 GPIO Configuration
	// PA9	-> USART1_Tx
	// PA10	<- USART1_Rx

	// Init pins PA9 and PA10 ( Rx / Tx USART1 )
	GPIO_StructInit( &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Mode					= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin						= GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed					= GPIO_Speed_2MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Mode					= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin						= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed					= GPIO_Speed_2MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	// Setup USART1 parameters
	USART_StructInit( &USART_InitStructure );
//	USART_InitStructure.USART_BaudRate				= 115200;
	USART_InitStructure.USART_BaudRate				= 921600;
	USART_InitStructure.USART_WordLength			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits				= USART_StopBits_1;
	USART_InitStructure.USART_Parity				= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode					= USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART1, &USART_InitStructure );
	USART_Cmd( USART1, ENABLE );

	// Start UART interrupt
	NVIC_InitStructure.NVIC_IRQChannel				= USART1_IRQn;	// Setup interrupt
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;		// Setup enable interrupt
	NVIC_Init( &NVIC_InitStructure );								// Init interrupt
	USART_Cmd( USART1, ENABLE );									// Enable USART1

	NVIC_EnableIRQ( USART1_IRQn );									// Enable interrupt UART1
	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );				// Enable interrupt for Rx
	//USART_ITConfig( USART1, USART_IT_TC, ENABLE );				// Enable interrupt for Tx


	// -----------------------------------------------------------------------
	// USART3 GPIO Configuration
	// PB10	-> USART3_Tx
	// PB11	<- USART3_Rx

	// Init pins PA10 and PA11 ( Tx / Rx USART3 )
	GPIO_StructInit( &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Mode					= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin						= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed					= GPIO_Speed_2MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Mode					= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin						= GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed					= GPIO_Speed_2MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	// Setup USART1 parameters
	USART_StructInit( &USART_InitStructure );
	USART_InitStructure.USART_BaudRate				= 19200;
//	USART_InitStructure.USART_BaudRate				= 921600;
	USART_InitStructure.USART_WordLength			= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits				= USART_StopBits_1;
	USART_InitStructure.USART_Parity				= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode					= USART_Mode_Rx | USART_Mode_Tx;
	USART_Init( USART3, &USART_InitStructure );
	USART_Cmd( USART3, ENABLE );

	// Start UART interrupt
	NVIC_InitStructure.NVIC_IRQChannel				= USART3_IRQn;	// Setup interrupt
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;		// Setup enable interrupt
	NVIC_Init( &NVIC_InitStructure );								// Init interrupt
	USART_Cmd( USART3, ENABLE );									// Enable USART3

	NVIC_EnableIRQ( USART3_IRQn );									// Enable interrupt UART3
	USART_ITConfig( USART3, USART_IT_RXNE, ENABLE );				// Enable interrupt for Rx
	//USART_ITConfig( USART1, USART_IT_TC, ENABLE );				// Enable interrupt for Tx


	// -----------------------------------------------------------------------
	// Init timers ( SysyTick )
	// -----------------------------------------------------------------------
	//SysTick_Config( F_APB1 / 5000 );								// interrupt 200 us
	SysTick_Config( F_APB1 / 100 );									// interrupt 10 ms
	// -----------------------------------------------------------------------


//	// -----------------------------------------------------------------------
//	// Start watchdog timer
//	// -----------------------------------------------------------------------
//	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
//	IWDG_SetPrescaler( IWDG_Prescaler_32 );
//	//IWDG_SetReload( 40000 / 256 );								// 150 ms
//	IWDG_SetReload( 40000 / 128 );									// 312 ms
//	IWDG_ReloadCounter();
//	IWDG_Enable();

	// -----------------------------------------------------------------------
	// Init global variables
	// -----------------------------------------------------------------------
	CntTime			= 0;		// SysTick count
	ret_button		= FALSE;	// Press "Enter"?
	cli_mode		= FALSE;	// CLI mode
	NoAnswer		= FALSE;	// No answer in CAN broadcast
	CANDownTime		= 0;




	Step_DS			= 0;
	Step_BTN		= 0;
	lc1				= 0;
	lc2				= 0;
	lc3				= 0;
}
