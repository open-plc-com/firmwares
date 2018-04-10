/*
	Name:			mx_misc.c
	Purpose:		FirmWare for STM32F103C8T6: DS18B20, iButton, GPIO
	Author:			Alexander Suvorov (www.open-plc.com)
	Created:		2016/07
	Modified by:	2017/12
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/


#include "mx_misc.h"

//void reset_can( void );
//void Init_Can( void );

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
	uint32_t	cnt, dwt_save;

	cnt			= F_DWT;
	cnt			*= delay;
	//cnt		+= DWT_CYCCNT;
	dwt_save	= DWT_CYCCNT;
	DWT_CYCCNT	= 0;
	while( DWT_CYCCNT < cnt ) {}
	DWT_CYCCNT	= dwt_save;
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
// USART3 IRQ Handler
// ===========================================================================
void USART3_IRQHandler( void )
{
	//char	s[2];
	char	ch;
	// Interruption is caused by receive data
	if ( USART_GetITStatus( USART3, USART_IT_RXNE ) != RESET )
	{	
		ch = USART_ReceiveData( USART3 );
		//s[1] = 0;
		if( ch != 0 )
		{
			put_usb( &ch, 1 );
		}
//strcpy( s, "qwaszx\r\n" );
//put_usb( s, 1 );
		USART_ClearITPendingBit( USART3, USART_IT_RXNE );
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

//	CANDownTime++;
//
//	if( CANDownTime > 500 )
//	{
//		reset_can();
//		delay_us( 100 );
//		Init_Can();
//		CANDownTime = 0;
//	}

//	if( WD_Time++ > 20 )
//	{
//		WD_Time = 0;
//		IWDG_ReloadCounter();
//	}
}
// ===========================================================================
