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
//#include "mx_parcer.c"
//#include "mx_cli.c"
//#include "mx_spi.c"
//#include "mx_can_mcp2515.c"
//#include "mx_adc_dma.c"
//#include "mx_pwm.c"


/* --- USB ncludes --- */
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"


int main( void )
{
	int	i;

	MX_Init();
	USART_ClearITPendingBit( USART1, USART_IT_RXNE );	// Clear UART
	put_str( "+++\r\n" );

//ADC_DMA_Init();
//PWM_Init();

	delay_ms( 2000 );
	USB_Interrupts_Config();
	USB_Init();


	//strcpy( s, "s-s\r\n" );
		i = GPIO_ReadInputDataBit( GPIOB, GPIO_Pin_2 );
	if( i )
	{
		put_str( "PB2=1\r\n" );
	}

	while (1)
	{
		delay_ms( 100 );
	}
}
