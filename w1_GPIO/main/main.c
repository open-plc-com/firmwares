#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_iwdg.h>
#include <stm32f10x_exti.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_dac.h>

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>

#include "gbl.h"
#include "mx_misc.c"
#include "mx_init.c"
#include "mx_parcer.c"
#include "mx_cli.c"
//#include "mx_spi.c"
//#include "mx_can_mcp2515.c"
#include "w1.c"


int main()
{
	//uint32_t	i, j;
	//unsigned int ui;
	//char	s[128];		// For print

	SystemInit();
	Read_Flash();
	MX_Init();

	//Init_Spi();
	//reset_can();
	//delay_us( 100 );
	//Init_Can();

	strcpy( cli_data, CLI );
	USART_ClearITPendingBit( USART1, USART_IT_RXNE );	// Clear UART
	put_str( "+++\r\n" );
	//put_str( cli_data );								// Print message


	// ---------------------------------------------------------------------------
	// The main program loop
	// ---------------------------------------------------------------------------
	lc1 = CntTime;
	lc2 = CntTime;
	lc3 = CntTime;
	while( TRUE )
	{

/*		Here you can place a custom script

		ch - Nn of channel; ( 0 - 15 )

		if( GPIO_X[ch].gpio_x && GPIO_X[ch].dev_type )	// Channel and device defined?
		{
			x = GPIO_X[ch].raw_data;	// Get value of channel ( 64 bits )

			// You can Set/Reset bit only for GPIO
			if( ( GPIO_X[ch].dev_type == 3 ) || ( GPIO_X[ch].dev_type == 4 ) )	// Channel is GPIO?
			{
				GPIO_SetBits( GPIO_X[ch].gpio_x, GPIO_X[ch].gpio_pin_x );		// Set bit
				GPIO_ResetBits( GPIO_X[ch].gpio_x, GPIO_X[ch].gpio_pin_x );		// Reset bit
			}
		}
*/

		Read_Data();
		if( Show_Data ) { Print_Data(); }
		Cli_State();

	}	// while( TRUE )
}
// ===========================================================================
