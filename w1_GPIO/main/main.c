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

#include "gbl.h"
#include "mx_misc.c"
#include "mx_init.c"
//#include "mx_adc_dma.c"
//#include "mx_spi.c"
//#include "mx_can_mcp2515.c"
#include "mx_cli.c"
//#include "mx_i2c.c"
//#include "dht_11.c"
#include "w1.c"


int main()
{
	uint32_t	i, j;

unsigned int ui;

	char	s[128];		// For print

	SystemInit();
	Read_Flash();
	MX_Init();
	//Read_Flash();
	//ADC_DMA_Init();

//	Init_Spi();
//	//Init_I2C2();
//	reset_can();
//	delay_us( 100 );
//	Init_Can();

	strcpy( cli_data, CLI );
	USART_ClearITPendingBit( USART1, USART_IT_RXNE );	// Clear UART
	put_str( "+++\r\n" );
	//put_str( cli_data );								// Print message

//while( 1 ) {}

i = 0;

	// ---------------------------------------------------------------------------
	// The main program loop
	// ---------------------------------------------------------------------------
	lc1 = CntTime;
	lc2 = CntTime;
	lc3 = CntTime;

	while( TRUE )
	{

		Read_Data();
		Print_Data();
		Cli_State();

	}	// while( TRUE )
}
// ===========================================================================
