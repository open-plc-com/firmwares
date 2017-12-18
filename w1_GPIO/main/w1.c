/*
	Name:			w1.c
	Purpose:		SoftWare for 1-Wire sensors: DS18B20, iButton, GPIO
	Author:			Alexander Suvorov; www.open-plc.com; e-mail:info3@open-plc.com
	Created:		2016/07
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/


// ===========================================================================
uint8_t w1_crc_update( uint8_t crc, uint8_t b )
// ===========================================================================
{
	uint8_t		i;

	for ( i = 8; i; i-- )
	{
		crc = ( ( crc ^ b ) & 1 ) ? ( ( crc >> 1 ) ^ 0x8C ) : ( crc >> 1 );
		b >>= 1;
	}
	return crc;
}
// ===========================================================================


// ===========================================================================
void w1_write_byte( uint16_t nn, uint8_t byte )
// ===========================================================================
{
	uint8_t		i, k;

	for( i = 0; i < 8; i++ )
	{
		if( i )
		{
			k = byte >> i;
		}
		else
		{
			k = byte;
		}
		k &= 1;
		if( k )		// send "1"
		{
			GPIO_ResetBits( GPIO_X[nn].gpio_x, GPIO_X[nn].gpio_pin_x );
			delay_us( 15 );
			GPIO_SetBits( GPIO_X[nn].gpio_x, GPIO_X[nn].gpio_pin_x );
			delay_us( 60 );
		}
		else		// send "0"
		{
			GPIO_ResetBits( GPIO_X[nn].gpio_x, GPIO_X[nn].gpio_pin_x );
			delay_us( 60 );
			GPIO_SetBits( GPIO_X[nn].gpio_x, GPIO_X[nn].gpio_pin_x );
			delay_us( 15 );
		}
	}
}
// ===========================================================================


// ===========================================================================
void w1_read_data( uint16_t nn )
// ===========================================================================
{
	uint8_t		i, j, l, n, crc;
//int in;
	uint8_t		m[10];				// Data buffer for 1-Wire
	int16_t		data;
	uint64_t	data_64;
	float		f;
//char	s[128];

	for( i = 0; i < 9; i++ )
	{
		n = 0;
		for( j = 0; j < 8; j++ )
		{
			GPIO_ResetBits( GPIO_X[nn].gpio_x, GPIO_X[nn].gpio_pin_x );
			delay_us( 5 );
			GPIO_SetBits( GPIO_X[nn].gpio_x, GPIO_X[nn].gpio_pin_x );
			delay_us( 10 );
			l = GPIO_ReadInputDataBit( GPIO_X[nn].gpio_x, GPIO_X[nn].gpio_pin_x );
			if( l )
			{
				n |= l << j;
			}
			delay_us( 60 );
		}
		m[i] = n;
	}

	crc = 0;
	for( i = 0; i < 8; i++ )
	{
		crc = w1_crc_update( crc, m[i] );
	}

	if( GPIO_X[nn].dev_type == 1 )	// DS18B20
	{
//put_str( "!!!\r\n" );
		if( crc == m[8] )	// CRC Ok
		{
			data = m[1];
			data <<= 8;
			data |= m[0];
			f = data / 16.0 * 10.0;
			data = f;
//sprintf( s, "f.raw_data=%f %d\r\n", f, data ); put_str( s );
			GPIO_X[nn].raw_data = data;
			GPIO_X[nn].err = 0;
		}	// if( crc == m[8] ) -- CRC Ok
		else
		{
			if( GPIO_X[nn].err < MAX_W1_ERR )
			{
				GPIO_X[nn].err++;
			}
			else
			{
				//GPIO_X[nn].raw_data = ( int16_t ) 0xFC00;
				GPIO_X[nn].raw_data = 0xFC00;
			}
		}
////sprintf( s, "GPIO_X[%d].raw_data=%ld\r\n", nn, GPIO_X[nn].raw_data ); put_str( s );
//f = GPIO_X[nn].raw_data / 10.0;
//in = GPIO_X[nn].raw_data;
////sprintf( s, " / %03.1f (%d) --- in=%d", f, W1_Err[i] ); put_str( s );
//sprintf( s, " / %03.1f in=%d\r\n", f, (int)GPIO_X[nn].raw_data ); put_str( s );
	}
	else
	if( GPIO_X[nn].dev_type == 2 )	// iButton
	{
		data_64 = 0;
		for( i = 0; i < 8; i++ )
		{
			data_64 <<= 8;
			data_64 |= m[i];
		}
		if( data_64 == 0xFFFFFFFFFFFFFFFF )
		{
			if( GPIO_X[nn].ibutton_time )
			{
				GPIO_X[nn].ibutton_time--;
			}
			else
			{
				GPIO_X[nn].raw_data = data_64;
				GPIO_X[nn].ibutton_time = iButton_Timeout;
			}
		}
		else
		{
			GPIO_X[nn].raw_data = data_64;
		}
	}
}
// ===========================================================================


// ===========================================================================
void Read_Data( void )
// ===========================================================================
{
	int		i;
	//char	s[128];

	// ---------------------------------------------------------------------------
	// --- DS18B20
	// ---------------------------------------------------------------------------
	if( Step_DS == 0 )
	{
		for( i = 0; i < Nn_Ch; i++ )
		{
			if( GPIO_X[i].dev_type == 1 )
			{
				// Reset sensor
				GPIO_ResetBits( GPIO_X[i].gpio_x, GPIO_X[i].gpio_pin_x );
				delay_us( 500 );
				GPIO_SetBits( GPIO_X[i].gpio_x, GPIO_X[i].gpio_pin_x );
				delay_us( 500 );

				// Start calculate
				w1_write_byte( i, 0xCC );
				delay_us( 30 );
				w1_write_byte( i, 0x44 );
			}
			Step_DS = 1;
			lc1 = CntTime;
		}	// for( i = 0; i < Nn_Ch; i++ )
	}	// if( Step_DS == 0 )
	else
	if( Step_DS == 1 )
	{
		if( abs( CntTime - lc1 ) >= 100 )
		{
			Step_DS = 2;
			lc1 = CntTime;
		}
	}
	else
	if( Step_DS == 2 )
	{
		for( i = 0; i < Nn_Ch; i++ )
		{
			if( GPIO_X[i].dev_type == 1 )
			{
				// Reset sensor
				GPIO_ResetBits( GPIO_X[i].gpio_x, GPIO_X[i].gpio_pin_x );
				delay_us( 500 );
				GPIO_SetBits( GPIO_X[i].gpio_x, GPIO_X[i].gpio_pin_x );
				delay_us( 500 );

				// Get data
				w1_write_byte( i, 0xCC );
				delay_us( 30 );
				w1_write_byte( i, 0xBE );
				delay_us( 30 );
				w1_read_data( i );
			}
			Step_DS = 3;
			lc1 = CntTime;
		}
	}
	else
	if( Step_DS == 3 )
	{
		if( abs( CntTime - lc1 ) >= 300 )
		{
			Step_DS = 0;
			lc1 = CntTime;
		}
	}

	// ---------------------------------------------------------------------------
	// --- iButton
	// ---------------------------------------------------------------------------
	if( Step_BTN == 0 )
	{
		for( i = 0; i < Nn_Ch; i++ )
		{
			if( GPIO_X[i].dev_type == 2 )
			{
				GPIO_ResetBits( GPIO_X[i].gpio_x, GPIO_X[i].gpio_pin_x );
			}
		}
		Step_BTN = 1;
		lc2 = CntTime;
	}
	else
	if( Step_BTN == 1 )
	{
		if( CntTime - lc2 >= 25 )
		{
			for( i = 0; i < Nn_Ch; i++ )
			{
				if( GPIO_X[i].dev_type == 2 )
				{
					GPIO_SetBits( GPIO_X[i].gpio_x, GPIO_X[i].gpio_pin_x );
				}
			}

			delay_us( 500 );

			for( i = 0; i < Nn_Ch; i++ )
			{
				if( GPIO_X[i].dev_type == 2 )
				{
					// Get data
					w1_write_byte( i, 0x33 );
					delay_us( 30 );
					w1_read_data( i );
				}
			}
			Step_BTN = 0;
		}
	}

	// ---------------------------------------------------------------------------
	// --- Read GPIO
	// ---------------------------------------------------------------------------
	for( i = 0; i < Nn_Ch; i++ )
	{
		if( ( GPIO_X[i].dev_type == 3 ) || ( GPIO_X[i].dev_type == 4 ) )
		{
			GPIO_X[i].raw_data = GPIO_ReadInputDataBit( GPIO_X[i].gpio_x, GPIO_X[i].gpio_pin_x );
//sprintf( s, " gpio[%d]=%d\r\n", i, ( int ) GPIO_X[i].raw_data ); put_str( s );
		}
	}


}
// ===========================================================================


// ===========================================================================
void Print_Data( void )
// ===========================================================================
{
	#ifdef SHOW_DATA
	int			i, j;
	float		f;
	uint8_t		*b;
	char		port_ch;
	char		s[128];

	if( ( ( CntTime - lc3 ) > ( SHOW_DATA_INTERVAL ) ) && !usart_rx_data[0] )
	{
//put_str( "--->>>\r\n" );
//		//k = Dev_Type;
//		put_str( "CFG  = " );
//		for( i = 0; i < Nn_Ch; i++ )
//		{
//			j = k & 0x0F;
//			sprintf( s, "%d", j ); put_str( s );
//			k >>= 4;
//		}
//		put_str( "\r\n" );

		for( i = 0; i < Nn_Ch; i++ )
		{
			if( ( GPIO_X[i].dev_type != 0 ) && ( GPIO_X[i].gpio_x != 0 ) )
			{
				if( GPIO_X[i].gpio_x == GPIOA )
				{
					port_ch = 'A';
				}
				else
				if( GPIO_X[i].gpio_x == GPIOB )
				{
					port_ch = 'B';
				}
				else
				{
					port_ch = '-';
				}

				b = ( uint8_t* ) &GPIO_X[i].raw_data;
				sprintf( s, "ch%-2d (%c%-2d) =", ( i + 1 ), port_ch, GPIO_X[i].gpio_pin_nn ); put_str( s );
				for( j = 0; j < 8; j++ )
				{
					sprintf( s, " %02X", b[j] ); put_str( s );
				}
				if( GPIO_X[i].dev_type == 1 )	// DS18B20
				{
					f = GPIO_X[i].raw_data / 10.0;
					//sprintf( s, " / %03.1f (err=%d)", f, GPIO_X[i].err ); put_str( s );
					sprintf( s, " / %03.1f", f ); put_str( s );
				}
				else
				if( ( GPIO_X[i].dev_type == 3 ) || ( GPIO_X[i].dev_type == 4 ) )	// GPIO
				{
					sprintf( s, " / %d", ( uint16_t ) GPIO_X[i].raw_data ); put_str( s );
				}
				put_str( "\r\n" );
			}
		}
		put_str( "\r\n" );
		lc3 = CntTime;
	}
	#endif
}
// ===========================================================================
