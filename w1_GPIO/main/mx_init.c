#include "mx_init.h"

void MX_Init( void )
{
	int				i;
	unsigned int	ui;
//	char	s[128];

	GPIO_InitTypeDef			GPIO_InitStructure;
	USART_InitTypeDef			USART_InitStructure;
	NVIC_InitTypeDef			NVIC_InitStructure;
//	ADC_InitTypeDef				ADC_InitStructure;
//	DMA_InitTypeDef				DMA_InitStructure;
//	TIM_TimeBaseInitTypeDef		TIMER_InitStructure;
//	DAC_InitTypeDef				DAC_InitStructure;


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
		//RCC_ADCCLKConfig( RCC_PCLK2_Div2 );							// Configures the ADC clock (ADCCLK).
	#else															// === External ===
		RCC_HSICmd( DISABLE );										// Enables the External High Speed oscillator (HSE).
		RCC_HSEConfig( RCC_HSE_ON );								// Configures the External High Speed oscillator (HSE).
		RCC_PLLConfig( RCC_PLLSource_HSE_Div2, RCC_PLLMul_12 );		// Configures the PLL clock source and multiplication factor.
		RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );				// Configures the system clock as PLL source (SYSCLK).
		RCC_HCLKConfig( RCC_SYSCLK_Div1 );							// Configures the AHB clock (HCLK).
		RCC_PCLK1Config( RCC_HCLK_Div2 );							// Configures the Low Speed APB clock (PCLK1).
		RCC_PCLK2Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK2).
		//RCC_ADCCLKConfig( RCC_PCLK2_Div2 );							// Configures the ADC clock (ADCCLK).
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
		//RCC_ADCCLKConfig( RCC_PCLK2_Div2 );							// Configures the ADC clock (ADCCLK).
	#else															// === External ===
		RCC_HSICmd( DISABLE );										// Enables the External High Speed oscillator (HSE).
		RCC_HSEConfig( RCC_HSE_ON );								// Configures the External High Speed oscillator (HSE).
		RCC_PLLConfig( RCC_PLLSource_HSE_Div2, RCC_PLLMul_6 );		// Configures the PLL clock source and multiplication factor.
		RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );				// Configures the system clock as PLL source (SYSCLK).
		RCC_HCLKConfig( RCC_SYSCLK_Div1 );							// Configures the AHB clock (HCLK).
		RCC_PCLK1Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK1).
		RCC_PCLK2Config( RCC_HCLK_Div1 );							// Configures the Low Speed APB clock (PCLK2).
		//RCC_ADCCLKConfig( RCC_PCLK2_Div2 );							// Configures the ADC clock (ADCCLK).
	#endif
	#endif
	RCC_PLLCmd( ENABLE );											// Enables the PLL.
	while( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET ) {}		// Wait till PLL is ready
	while( RCC_GetSYSCLKSource() != 0x08 ) {}						// Wait till PLL is used as system clock source


	// Enable peripheral clock
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );			// Clocking GPIOA
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );			// Clocking GPIOB
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE );			// Clocking GPIOC
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE );		// Clocking USART1
	//RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1, ENABLE );		// Clocking ADC1
	//RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );			// Clocking DMA1 clock
	//RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );			// Clocking TIM4

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

	// Configure GPIO pin : PB2 - BOOT1
	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	// -----------------------------------------------------------------------
	// Init PA & PB
	// -----------------------------------------------------------------------

	// Init PA
	ui = 0;
	for( i = 0; i < Nn_Ch; i++ )
	{
		if( GPIO_X[i].gpio_x == GPIOA )
		{
			ui |= GPIO_X[i].gpio_pin_x;
		}
	}
	GPIO_InitStructure.GPIO_Pin		= ui;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	// Init PB
	ui = 0;
	for( i = 0; i < Nn_Ch; i++ )
	{
		if( GPIO_X[i].gpio_x == GPIOB )
		{
			ui |= GPIO_X[i].gpio_pin_x;
		}
	}
	GPIO_InitStructure.GPIO_Pin		= ui;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init( GPIOB, &GPIO_InitStructure );

	// Set or reset pins PA & PB
	for( i = 0; i < Nn_Ch; i++ )
	{
		if( ( GPIO_X[i].gpio_x == GPIOA ) || ( GPIO_X[i].gpio_x == GPIOB ) )
		{
			if( GPIO_X[i].dev_type )
			{
				if( GPIO_X[i].dev_type == 3 )
				{
					GPIO_ResetBits( GPIO_X[i].gpio_x, GPIO_X[i].gpio_pin_x );	// Reset bit
				}
				else
				{
					GPIO_SetBits( GPIO_X[i].gpio_x, GPIO_X[i].gpio_pin_x );		// Set bit
				}
			}	// if( GPIO_X[i].dev_type )
		}	// if( GPIO_X[i].gpio_x == GPIOA )
	}	// for( i = 0; i < Nn_Ch; i++ )


//	// -----------------------------------------------------------------------
//	// PA0 - MCP2515 interrupt
//	// -----------------------------------------------------------------------
//	GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
//	GPIO_Init( GPIOA, &GPIO_InitStructure );


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
	USART_InitStructure.USART_BaudRate				= 115200;
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
	//usart_rx_data[0] = 0;											// Clear Rx buffer


	// -----------------------------------------------------------------------
	// Init timers ( SysyTick )
	// -----------------------------------------------------------------------
	//SysTick_Config( F_APB1 / 5000 );								// interrupt 200 us
	SysTick_Config( F_APB1 / 100 );								// interrupt 10 ms
	// -----------------------------------------------------------------------










































//	TIM_TimeBaseStructInit( &TIMER_InitStructure );
//	TIMER_InitStructure.TIM_CounterMode		= TIM_CounterMode_Up;
//	//TIMER_InitStructure.TIM_Prescaler		= 8000;
//	#if F_APB1 == 48000000
//		TIMER_InitStructure.TIM_Prescaler	= 2400;		// 4900 Hz for 48 MHz
//	#else
//		TIMER_InitStructure.TIM_Prescaler	= 1200;		// 4900 Hz for 24 MHz
//	#endif
//	TIMER_InitStructure.TIM_Period			= 1;		// One counter
//	TIM_TimeBaseInit( TIM4, &TIMER_InitStructure );
//	TIM_ITConfig( TIM4, TIM_IT_Update, ENABLE );
//	TIM_Cmd( TIM4, ENABLE );
//
//	/* NVIC Configuration */
//	/* Enable the TIM4_IRQn Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel						= TIM4_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd					= ENABLE;
//	NVIC_Init( &NVIC_InitStructure );


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
	//Nn_Ch			= 0;
	cli_mode		= FALSE;	// CLI mode
	NoAnswer		= FALSE;	// No answer in broadcast

	Step_DS			= 0;
	Step_BTN		= 0;

	//transmit_mode	= FALSE;
	//min_lvl			= 0;
	//max_lvl			= 0;
}
