/*
	Name:			mx_misc.c
	Purpose:
	Author:			Alexander Suvorov
	Created:		2016/07
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
	Site:			open-plc.com
*/


#include "mx_misc.h"

//void reset_can( void );
//void Init_Can( void );
//void put_str( char *s );


// ===========================================================================
// Delay in milliseconds
// ===========================================================================
void delay_ms( uint32_t delay )
{
	int	i;

	for( i = 0; i < delay; i++ )
	{
		delay_us( 1000 );
	}
}
// ===========================================================================


// ===========================================================================
// Delay in microseconds
// ===========================================================================
void delay_us( uint32_t delay )
{
	uint32_t	cnt;

	cnt			= F_DWT;
	cnt			*= delay;
	//cnt		+= DWT_CYCCNT;
	DWT_CYCCNT	= 0;
	while( DWT_CYCCNT < cnt ) {}
}
// ===========================================================================


// ===========================================================================
// Print str to USART1; without interrupt
// ===========================================================================
void put_str( char* str )
{
	int		i, n;

	while( !( USART1->SR & USART_SR_TC ) ) {}
	n = strlen( str );
	for( i = 0; i < n; i++ )
	{
		while( !( USART1->SR & USART_SR_TC ) ) {}
		USART1->DR = str[i];
	}
}
// ===========================================================================

void clr_input( void )
{
	int i;

	for( i = 0; i < RX_LEN; i++ )
	{
		usart_rx_data[i] = 0;
	}
	ret_button = FALSE;
	rx_idx = 0;
}

// ===========================================================================
// USART1 IRQ Handler
// ===========================================================================
void USART1_IRQHandler( void )
{
	char	c, s[2];

	// Interruption is caused by receive data
	if ( USART_GetITStatus( USART1, USART_IT_RXNE ) != RESET )
	{	
		c = USART_ReceiveData( USART1 );		// Read char
		s[0] = c;								// Put char to buffer for "echo"
		s[1] = 0;								// End of string

		if( c == 13 )
		{
			// if( rx_idx == 0 )
			usart_rx_data[rx_idx] = 0;
			ret_button = TRUE;					// Set flag "press Enter"
			rx_idx = 0;
		}
		else
		{
			if( rx_idx < ( RX_LEN - 1 ) )
			{
				usart_rx_data[rx_idx++] = c;	// Char to read buffer
				usart_rx_data[rx_idx] = 0;		// End of string
				put_str( s );					// Echo
			}
		}
		USART_ClearITPendingBit( USART1, USART_IT_RXNE );
	}
}
// ===========================================================================


// ===========================================================================
// SysTick
// Priority = 6
// Frequency = 100 Hz
// ===========================================================================
void SysTick_Handler( void )
{
//uint32_t	i;

	if( DelayTime )
		DelayTime--;
	else
	{
		if( cli_mode )
		{
			DelayTime = 25;
		}
		else
		{
			DelayTime = 75;
		}
		#ifdef LED_BLINK
			GPIOC->ODR ^= GPIO_Pin_13;
		#endif
	}

	CntTime++;

	//ADC_DMACmd( ADC1, DISABLE );
	//SysTick_Config( 0 );								// interrupt 200 us
	//SUM = 0;
	//for( i = 0; i < 1000; i++ )
	//{
	//SUM += ADC_VALUES[i];
	//}
	//SUM /= 1000;
	//ADC_DMACmd( ADC1, ENABLE );
	//SysTick_Config( F_APB1 / 5000 );								// interrupt 200 us

//	CANDownTime++;

//	if( CANDownTime > 500 )
//	{
//		//reset_can();
//		//delay_us( 100 );
//		//Init_Can();
//		CANDownTime = 0;
////put_str( "reset CAN\r\n" );
//	}

//	if( WD_Time++ > 20 )
//	{
//		WD_Time = 0;
//		IWDG_ReloadCounter();
//	}
}
// ===========================================================================


//// ===========================================================================
//void TIM4_IRQHandler( void )
//{
//	int i;
//
//	TIM_Cmd( TIM4, DISABLE );
//	SUM = 0;
//	for( i = 0; i < ADC_ARRAY_SIZE; i++ )
//	{
//		SUM += ADC_VALUES[i];
//	}
//	SUM /= ADC_ARRAY_SIZE;
//
//	if( TIM_GetITStatus( TIM4, TIM_IT_Update ) != RESET )
//	{
//		TIM_ClearITPendingBit( TIM4, TIM_IT_Update );
//		GPIOA->ODR ^= GPIO_Pin_8;
//	}
//	TIM_Cmd( TIM4, ENABLE );
//}
//// ===========================================================================
