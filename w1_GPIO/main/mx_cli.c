/*
	Name:			mx_cli.c
	Purpose:		FirmWare for STM32F103C8T6, DS18B20, iButton, GPIO
	Author:			Alexander Suvorov (www.open-plc.com)
	Created:		2016/07
	Modified by:	2017/12
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/


// ===========================================================================
void Write_Flash( void )
// ===========================================================================
{
	uint32_t	i;
	uint16_t	j, k, l;

	// FLASH_Unlock
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;

	// FLASH_Erase Page
	while( FLASH->SR&FLASH_SR_BSY ) {}
	FLASH->CR |= FLASH_CR_PER;			//Page Erase Set
	FLASH->AR = Page_63;				//Page Address
	FLASH->CR |= FLASH_CR_STRT;			//Start Page Erase
	while( FLASH->SR&FLASH_SR_BSY ) {}
	FLASH->CR &= ~FLASH_CR_PER;			//Page Erase Clear
	FLASH->CR |= FLASH_CR_PG;			// FLASH_Program HalfWord

	i = DeviceType; j = i & 0xFFFF;
	while( FLASH->SR&FLASH_SR_BSY ) {}
	*( __IO uint16_t* ) Page_63 = j;
	i = i >> 16; j = i & 0xFFFF;
	while( FLASH->SR&FLASH_SR_BSY ) {}
	*( __IO uint16_t* ) ( Page_63 + 2 ) = j;

	i = Sn; j = i & 0xFFFF;
	while( FLASH->SR&FLASH_SR_BSY ) {}
	*( __IO uint16_t* ) ( Page_63 + 4 )= j;
	i = i >> 16; j = i & 0xFFFF;
	while( FLASH->SR&FLASH_SR_BSY ) {}
	*( __IO uint16_t* ) ( Page_63 + 6 ) = j;

	while( FLASH->SR&FLASH_SR_BSY ) {}
	*( __IO uint16_t* ) ( Page_63 + 8 ) = Noc;

	while( FLASH->SR&FLASH_SR_BSY ) {}
	*( __IO uint16_t* ) ( Page_63 + 10 ) = Speedc;

	while( FLASH->SR&FLASH_SR_BSY ) {}
	*( __IO uint16_t* ) ( Page_63 + 12 ) = Nn_Ch;

	while( FLASH->SR&FLASH_SR_BSY ) {}
	*( __IO uint16_t* ) ( Page_63 + 14 ) = iButton_Timeout;

	k = 16;
	for( i = 0; i < Nn_Ch; i++ )
	{
		if( GPIO_X[i].gpio_x == GPIOA )
		{
			l = 1;
		}
		else
		if( GPIO_X[i].gpio_x == GPIOB )
		{
			l = 2;
		}
		else
		{
			l = 0;
		}
		while( FLASH->SR&FLASH_SR_BSY ) {}
		*( __IO uint16_t* ) ( Page_63 + k ) = l; k += 2;

		l = GPIO_X[i].gpio_pin_nn;
		while( FLASH->SR&FLASH_SR_BSY ) {}
		*( __IO uint16_t* ) ( Page_63 + k ) = l; k += 2;

		l = GPIO_X[i].dev_type;
		while( FLASH->SR&FLASH_SR_BSY ) {}
		*( __IO uint16_t* ) ( Page_63 + k ) = l; k += 2;

	}	// for( i = 0; i < Ch_Nn; i++ )

	//j = 0;
	while( ( FLASH->SR&FLASH_SR_BSY ) ) {}
	FLASH->CR &= ~FLASH_CR_PG;
	FLASH->CR |= FLASH_CR_LOCK;

	if( Nn_Ch )
	{
		free( GPIO_X );
	}
}
// ===========================================================================


// ===========================================================================
void Read_Flash( void )
// ===========================================================================
{
	uint32_t	i;
	uint16_t	k, l;
	//uint16_t	nn_ch_init;
	static uint32_t *uid = ( uint32_t* ) 0x1FFFF7E8;


//char s[128];
//put_str( "\r\nread\r\n" );

	DeviceType		= *( uint32_t* )   Page_63;
	Sn				= *( uint32_t* ) ( Page_63 + 4 );
	Noc				= *( uint16_t* ) ( Page_63 + 8 );
	Speedc			= *( uint16_t* ) ( Page_63 + 10 );
	Nn_Ch			= *( uint16_t* ) ( Page_63 + 12 );
	iButton_Timeout = *( uint16_t* ) ( Page_63 + 14 );
	//j				= 0;

	if( Nn_Ch > MAX_NN_CH )
	{
		Nn_Ch = 0;
	}
	else
	{
		i = sizeof( GPIO_Data );
//put_str( "\r\n" );
		if( Nn_Ch )
		{
			GPIO_X = malloc( i * Nn_Ch );
			k = 16;
			for( i = 0; i < Nn_Ch; i++ )
			{
				l = *( uint16_t* ) ( Page_63 + k ); k += 2;
//sprintf( s, "ch=%d port=%d ", ( i + 1), l ); put_str( s );
				if( l == 1 )
				{
					GPIO_X[i].gpio_x = GPIOA;
				}
				else
				if( l == 2 )
				{
					GPIO_X[i].gpio_x = GPIOB;
				}
				else
				{
					GPIO_X[i].gpio_x = 0;
				}

				l = *( uint16_t* ) ( Page_63 + k ); k += 2;
//sprintf( s, "pin=%d ", l ); put_str( s );
				if( l < 16 )
				{
					GPIO_X[i].gpio_pin_nn = l;
				}
				else
				{
					GPIO_X[i].gpio_x = 0;
					GPIO_X[i].gpio_pin_nn = 0;
				}

				l = *( uint16_t* ) ( Page_63 + k ); k += 2;
//sprintf( s, "dev=%d", l ); put_str( s );
				if( l < 5 )
				{
					GPIO_X[i].dev_type = l;
				}
				else
				{
					GPIO_X[i].dev_type = 0;
				}
//put_str( "\r\n" );

				switch( GPIO_X[i].gpio_pin_nn )
				{
					case 0:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_0;	break;
					case 1:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_1;	break;
					case 2:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_2;	break;
					case 3:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_3;	break;
					case 4:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_4;	break;
					case 5:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_5;	break;
					case 6:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_6;	break;
					case 7:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_7;	break;
					case 8:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_8;	break;
					case 9:		GPIO_X[i].gpio_pin_x	= GPIO_Pin_9;	break;
					case 10:	GPIO_X[i].gpio_pin_x	= GPIO_Pin_10;	break;
					case 11:	GPIO_X[i].gpio_pin_x	= GPIO_Pin_11;	break;
					case 12:	GPIO_X[i].gpio_pin_x	= GPIO_Pin_12;	break;
					case 13:	GPIO_X[i].gpio_pin_x	= GPIO_Pin_13;	break;
					case 14:	GPIO_X[i].gpio_pin_x	= GPIO_Pin_14;	break;
					case 15:	GPIO_X[i].gpio_pin_x	= GPIO_Pin_15;	break;
					default:	GPIO_X[i].gpio_pin_x	= 0;
				}
			}	// for( i = 0; i < Nn_Ch; i++ )
		}	// if( Nn_Ch )
	}	// else - if( Nn_Ch > MAX_NN_CH )

	if( iButton_Timeout > 80 )
	{
		iButton_Timeout = 80;	// 10 sec
	}

	#ifdef DEV_TYPE
		DeviceType = DEV_TYPE;
	#endif
	#ifdef DEV_ID
		Sn = uid[2];
	#endif
}
// ===========================================================================


// ===========================================================================
void Cli( void )
// ===========================================================================
{
	bool		cmd, change_prm;
	char		s[RX_LEN];
	int			i, j, k, n, m;
	int			space_index[6];
	uint16_t	n1, n2, n3, n4;

	cli_mode	= TRUE;
	change_prm	= FALSE;
	put_str( VER );
	strcpy( cli_data, "\r\nConfig> " );		// Set prompt
	put_str( cli_data );
	m = sizeof( space_index ) / sizeof( int );

	while( cli_mode )
	{
		if( ret_button )	// Press Enter
		{
			ret_button = FALSE;
			cmd = FALSE;

			n = strlen( usart_rx_data );
			if( n )		// Length > 0
			{
				// Trim left spaces
				j = 0;
				while( usart_rx_data[0] == ' ' )
				{
					n = strlen( usart_rx_data );
					for( j = 0; j < n; j++ )
					{
						usart_rx_data[j] = usart_rx_data[j + 1];
					}
				}

				// -----------------------------------------------------------
				// Trim right spaces
				// -----------------------------------------------------------
				n = strlen( usart_rx_data ) - 1;
				if( n > 0 )
				{
					while( usart_rx_data[n] == ' ' )
					{
						if( n >= 0 )
						{
							usart_rx_data[n] = 0;
							if( n )
							{
								n--;
							}
						}
					}
				}
				// -----------------------------------------------------------

				// -----------------------------------------------------------
				// Prepare cmd string
				// -----------------------------------------------------------
				n = strlen( usart_rx_data );
				for( i = 0; i < n; i++ )
				{
					if( ( usart_rx_data[i] < 32 ) || ( usart_rx_data[i] > 126 ) )
					{
						if( usart_rx_data[i] )
						{
							usart_rx_data[i] = '$';
						}
					}
					if( usart_rx_data[i] > 96 )
					{
						usart_rx_data[i] -= 32;
					}

					while( ( usart_rx_data[i] == ' ' ) && ( usart_rx_data[i + 1] == ' ' ) )
					{
						for( j = i; j < n; j++ )
						{
							usart_rx_data[j] = usart_rx_data[j + 1];
						}
					}
				}	// for( i = 0; i < n; i++ )

				for( j = 0; j < m; j++ )
				{
					space_index[j] = 0;
				}

				j = 0;
				for( i = 0; i < n; i++ )
				{
					if( ( usart_rx_data[i] == ' ' ) && ( j < m ) )
					{
						space_index[j++] = i;
					}
				}
				// -----------------------------------------------------------

				// -----------------------------------------------------------
				// Get first param
				// -----------------------------------------------------------
				if( space_index[0] )
				{
					for( j = 0; j < space_index[0]; j++ )
					{
						s[j] = usart_rx_data[j];
					}
					s[j] = 0;
				}
				else
				{
					strcpy( s, usart_rx_data );
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Complete CLI mode
				// -----------------------------------------------------------
				if( strcmp( s, "Q" ) == 0 )
				{
					strcpy( cli_data, CLI );	// Return prompt
					put_str( cli_data );
					cli_mode = FALSE;			// Not CLI mode
					put_str( "\r\n\r\n" );
					clr_input();
					if( change_prm )
					{
						NVIC_SystemReset();
					}
					return;
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Help
				// -----------------------------------------------------------
				if( strcmp( s, "?" ) == 0 )
				{
					put_str( "\r\n" );
					put_str( "\r\n?       - This page" );
					put_str( "\r\nq       - Exit from Config" );
					put_str( "\r\nsh      - Show parameters" );
//					put_str( "\r\n" );
//					put_str( "\r\ncn <Nn> - set Nn on CAN bus (1..100)" );
//					put_str( "\r\ncs <Nn> - set CAN speed" );
//					put_str( "\r\n          Nn = 0; Speed 500000" );
//					put_str( "\r\n          Nn = 1; Speed 250000" );
//					put_str( "\r\n          Nn = 2; Speed 125000" );
//					put_str( "\r\n          Nn = 3; Speed 100000" );
//					put_str( "\r\n          Nn = 4; Speed 50000" );
//					put_str( "\r\n          Nn = 5; Speed 20000" );
//					put_str( "\r\n          Nn = 6; Speed 10000" );
					put_str( "\r\n" );
				 sprintf( s, "\r\nln <Nn>                                 - set number of channels (1-%d)", MAX_NN_CH ); put_str( s );
					put_str( "\r\nlc <channel> <port> <GPIOx> [GPIO Type] - set channel" );
					put_str( "\r\nlt <channel> <GPIO Type>                - set channel type" );
					put_str( "\r\n" );
					put_str( "\r\nwhere" );
					put_str( "\r\n       channel - Nn of channel" );
					put_str( "\r\n       port    - port A,B,(0-delete)" );
					put_str( "\r\n       GPIOx   - Nn pin of ports (0-15)" );
					put_str( "\r\n       GPIO Type:  Nn = 0 - Not connected" );
					put_str( "\r\n                   Nn = 1 - DS18B20" );
					put_str( "\r\n                   Nn = 2 - iButton" );
					put_str( "\r\n                   Nn = 3 - bidirectional GPIO pulldown" );
					put_str( "\r\n                   Nn = 4 - bidirectional GPIO pullup" );
					put_str( "\r\n" );
					put_str( "\r\ntm   <Nn>  - Set timeout for iButton, Nn=0..10" );
//					put_str( "\r\nwrsn <Nn>  - Write S/N" );
//					put_str( "\r\nwrdt <Nn>  - Write Device Type" );
					put_str( "\r\n" );
					cmd = TRUE;
					clr_input();
//change_prm = FALSE;
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Show parameters
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "SH" ) == 0 )
					{
						switch( Speedc )
						{
							case 0:		n = 500000;		break;
							case 1:		n = 250000;		break;
							case 2:		n = 125000;		break;
							case 3:		n = 100000;		break;
							case 4:		n = 50000;		break;
							case 5:		n = 20000;		break;
							case 6:		n = 10000;		break;
							default:	n = 1;
						}
						put_str( "\r\n\r\n" );
						sprintf( s, "Name         Value\r\n" );				put_str( s );
						sprintf( s, "-----------  ----------\r\n" );		put_str( s );
						sprintf( s, "Device Type  %u\r\n", DeviceType );	put_str( s );
						sprintf( s, "S/N          %u\r\n", Sn );			put_str( s );
//						sprintf( s, "Nn on CAN    %u\r\n", Noc );			put_str( s );
//						sprintf( s, "CAN Speed    %d bps\r\n", n );			put_str( s );
//						sprintf( s, "CAN Speed    %u\r\n", Speedc );		put_str( s );
						put_str( "\r\n" );
						sprintf( s, "BTN timeout  %d sec\r\n", ( iButton_Timeout / 8 ) );	put_str( s );
						put_str( "\r\n" );
						put_str( "ch port GPIO Type\r\n" );
						put_str( "-- ---- ---------\r\n" );

						for( i = 0; i < Nn_Ch; i++ )
						{
							sprintf( s, "%2u ", ( i + 1 ) ); put_str(s);
							j = 0;
							if( GPIO_X[i].gpio_x == GPIOA )
							{
								put_str( "PA" );
								j = 1;
							}
							else
							if( GPIO_X[i].gpio_x == GPIOB )
							{
								put_str( "PB" );
								j = 1;
							}
							else
							{
								put_str( "----" );
							}

							if( j )
							{
								sprintf( s, "%-2u", GPIO_X[i].gpio_pin_nn ); put_str( s );
							}

							switch( GPIO_X[i].dev_type )
							{
								case 0:		put_str( " ----" );								break;
								case 1:		put_str( " (1) DS18B20" );						break;
								case 2:		put_str( " (2) iButton" );						break;
								case 3:		put_str( " (3) bidirectional GPIO pulldown" );	break;
								case 4:		put_str( " (4) bidirectional GPIO pullup" );	break;
								default:	put_str( " ----" );
							}
						put_str( "\r\n" );
						}	// for( i = 0; i < Nn_Ch; i++ )
						if( !Nn_Ch )
						{
							put_str( "\r\n" );
						}
						cmd = TRUE;
						clr_input();
//change_prm = FALSE;
					}	// if( strcmp( s, "SH" ) == 0 )
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Input CAN number
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "CN" ) == 0 )
					{
						// Get first param
						s[0] = 0;
						if( space_index[0] )
						{
							if( space_index[1] )
							{
								i = 0;
								for( j = ( space_index[0] + 1 ); j < space_index[1]; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[0] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}

							if( s[0] )
							{
								i = atoi( s );
								if( ( i > 0 ) && ( i < 101 ) )
								{
									Noc = i;
									Write_Flash();
									Read_Flash();
									change_prm = TRUE;
								}
								else
								{
									put_str( "\r\nError CAN number. Enable 1..100\r\n" );
								}
							}
						}	// if( space_index[0] )
						else
						{
							put_str( "\r\nCommand error\r\n" );
						}
						cmd = TRUE;
						clr_input();
					}	// if( strcmp( s, "CN" ) == 0 )
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Input CAN speed
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "CS" ) == 0 )
					{
						// Get first param
						s[0] = 0;
						if( space_index[0] )
						{
							if( space_index[1] )
							{
								i = 0;
								for( j = ( space_index[0] + 1 ); j < space_index[1]; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[0] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}

							if( s[0] )
							{
								i = atoi( s );
								if( ( i >= 0 ) && ( i < 7 ) )
								{
									Speedc = i;
									Write_Flash();
									Read_Flash();
									change_prm = TRUE;

//									reset_can();
//									delay_us( 100 );
//									Init_Can();
								}
								else
								{
									put_str( "\r\nError CAN speed\r\n" );
								}
							}
						}	// if( space_index[0] )
						else
						{
							put_str( "\r\nCommand error\r\n" );
						}
						cmd = TRUE;
						clr_input();
					}	// if( strcmp( s, "CS" ) == 0 )
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Input LN ( number of channels )
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "LN" ) == 0 )
					{
						// Get first param
						s[0] = 0;
						if( space_index[0] )
						{
							if( space_index[1] )
							{
								i = 0;
								for( j = ( space_index[0] + 1 ); j < space_index[1]; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[0] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}

							n = atoi( s );
							if( ( n > 0 ) && ( n < ( MAX_NN_CH + 1 ) ) )
							{
								if( n == Nn_Ch )
								{
									put_str( "There is nothing to change.\r\n" );
								}
								else
								{
									Nn_Ch = n;
									Write_Flash();
									Read_Flash();
									change_prm = TRUE;
								}
							}
							else
							{
								put_str( "Wrong number, cancel.\r\n" );
							}
						}	// if( space_index[0] )
						else
						{
							put_str( "\r\nCommand error\r\n" );
						}
						cmd = TRUE;
						clr_input();
					}	// if( strcmp( s, "SD" ) == 0 )
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Input LC
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "LC" ) == 0 )
					{
						n1 = 0;
						n2 = 0;
						n3 = 0;
						n4 = 0;

						// Get first param
						s[0] = 0;
						if( space_index[0] )
						{
							if( space_index[1] )
							{
								i = 0;
								for( j = ( space_index[0] + 1 ); j < space_index[1]; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// if( space_index[1] )
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[0] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// else -- if( space_index[1] )

							if( s[0] )
							{
								n1 = abs( atoi( s ) );
							}
						}	// if( space_index[0] )

						// Get second param
						s[0] = 0;
						if( space_index[1] )
						{
							if( space_index[2] )
							{
								i = 0;
								for( j = ( space_index[1] + 1 ); j < space_index[2] + 1; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// if( space_index[2] )
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[1] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// else -- if( space_index[2] )

							if( s[1] == ' ' )
							{
								s[1] = 0;
							}
							if( strlen( s ) == 1 )
							{
								if( s[0] == '0' )
								{
									n2 = 0;
								}
								else
								if( ( s[0] == 'a' ) || ( s[0] == 'A' ) )
								{
									n2 = 1;
								}
								else
								if( ( s[0] == 'b' ) || ( s[0] == 'B' ) )
								{
									n2 = 2;
								}
								else
								{
									n2 = 0xFF;
								}
							}
							else
							{
								n2 = 0xFF;
							}
						}	// if( space_index[1] )

						// Get third param
						s[0] = 0;
						if( space_index[2] )
						{
							if( space_index[3] )
							{
								i = 0;
								for( j = ( space_index[2] + 1 ); j < space_index[3] + 1; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// if( space_index[2] )
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[2] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// else -- if( space_index[2] )

							if( s[0] )
							{
								n3 = abs( atoi( s ) );
							}
						}	// if( space_index[2] )

						// Get fourth param
						s[0] = 0;
						if( space_index[3] )
						{
							if( space_index[4] )
							{
								i = 0;
								for( j = ( space_index[3] + 1 ); j < space_index[4] + 1; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// if( space_index[2] )
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[3] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// else -- if( space_index[4] )

							if( s[0] )
							{
								n4 = abs( atoi( s ) );
							}
						}	// if( space_index[3] )
//sprintf( s, "n1=%d n2=%d n3=%d n4=%d\r\n", n1, n2, n3, n4 ); put_str( s );

						n = 1;
						if( ( n1 == 0 ) ||  ( n1 > Nn_Ch ) )
						{
							put_str( "\r\nWrong Nn channel\r\n" );
							n = 0;
						}

						if( n )
						{
							if( n2 > 2 )
							{
								put_str( "\r\nWrong port name\r\n" );
								n = 0;
							}
						}

						if( n )
						{
							if( n3 > 15 )
							{
								put_str( "\r\nWrong Nn pin\r\n" );
								n = 0;
							}
						}

						if( n )
						{
							if( n2 == 1 )
							{
								if( ( n3 == 13 ) || ( n3 == 14 ) )
								{
									put_str( "\r\nWrong Nn pin\r\n" );
									n = 0;
								}
							}
						}

						if( n )
						{
							if( n2 == 2 )
							{
								if( n3 == 2 )
								{
									put_str( "\r\nWrong Nn pin\r\n" );
									n = 0;
								}
							}
						}

						if( n )
						{
							if( n4 > 4 )
							{
								put_str( "\r\nWrong GPIO Type\r\n" );
								n = 0;
							}
						}

						if( n )
						{
							k = 1;
							if( n2 == 1 )
							{
								GPIO_Data.gpio_x = GPIOA;
							}
							else
							if( n2 == 2 )
							{
								GPIO_Data.gpio_x = GPIOB;
							}
							else
							{
								GPIO_Data.gpio_x = 0;
							}
							GPIO_Data.gpio_pin_nn = n3;
							GPIO_Data.dev_type = n4;

							for( i = 0; i < Nn_Ch; i++ )
							{
								if( GPIO_Data.gpio_x )
								{
									if( ( GPIO_Data.gpio_x == GPIO_X[i].gpio_x ) && ( GPIO_Data.gpio_pin_nn == GPIO_X[i].gpio_pin_nn ) )
									{
										k = 0;
										break;
									}
								}
							}	// for( i = 0; i < Nn_Ch; i++ )

							if( k )
							{
								if( n2 == 1 )
								{
									GPIO_X[n1-1].gpio_x = GPIOA;
								}
								else
									if( n2 == 2 )
								{
									GPIO_X[n1-1].gpio_x = GPIOB;
								}
								else
								{
									GPIO_X[n1-1].gpio_x = 0;
								}
								GPIO_X[n1-1].gpio_pin_nn = n3;
								GPIO_X[n1-1].dev_type = n4;
								Write_Flash();
								Read_Flash();
								change_prm = TRUE;
							}	// if( k )
							else
							{
								put_str( "\r\nPort already in use.\r\n" );
							}	// else - if( k )
						}	// if( n )
						cmd = TRUE;
						clr_input();
					}	// if( strcmp( s, "CT" ) == 0 )
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Input LT
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "LT" ) == 0 )
					{
						n1 = 0;
						n2 = 0;

						// Get first param
						s[0] = 0;
						if( space_index[0] )
						{
							if( space_index[1] )
							{
								i = 0;
								for( j = ( space_index[0] + 1 ); j < space_index[1]; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// if( space_index[1] )
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[0] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// else -- if( space_index[1] )

							if( s[0] )
							{
								n1 = abs( atoi( s ) );
							}
						}	// if( space_index[0] )

						// Get second param
						s[0] = 0;
						if( space_index[1] )
						{
							if( space_index[2] )
							{
								i = 0;
								for( j = ( space_index[1] + 1 ); j < space_index[2] + 1; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// if( space_index[2] )
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[1] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}	// else -- if( space_index[2] )

							if( s[0] )
							{
								n2 = abs( atoi( s ) );
							}
						}	// if( space_index[1] )
//sprintf( s, "\r\nn1=%d n2=%d\r\n", n1, n2 ); put_str( s );

						n = 1;
						if( ( n1 == 0 ) || ( n1 > Nn_Ch ) )
						{
							put_str( "\r\nWrong Nn channel\r\n" );
							n = 0;
						}

						if( n )
						{
							if( n2 > 4 )
							{
								put_str( "\r\nWrong GPIO Type\r\n" );
								n = 0;
							}
						}

						if( n )
						{
							GPIO_X[n1-1].dev_type = n2;
							Write_Flash();
							Read_Flash();
							change_prm = TRUE;
						}
						cmd = TRUE;
						clr_input();
					}	// if( strcmp( s, "CT" ) == 0 )
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Write SN ( Serial Number )
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "WRSN" ) == 0 )
					{
						// Get first param
						s[0] = 0;
						if( space_index[0] )
						{
							if( space_index[1] )
							{
								i = 0;
								for( j = ( space_index[0] + 1 ); j < space_index[1]; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[0] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}

							if( s[0] )
							{
								i = atoi( s );
								if( i > 0 )
								{
									Sn = i;
									Write_Flash();
									Read_Flash();
									change_prm = TRUE;
								}
								else
								{
									put_str( "\r\nError number\r\n" );
								}
							}
						}	// if( space_index[0] )
						else
						{
							put_str( "\r\nCommand error\r\n" );
						}
						cmd = TRUE;
						clr_input();
					}	// if( strcmp( s, "WRSN" ) == 0 )
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Input DT ( Device Type )
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "WRDT" ) == 0 )
					{
						// Get first param
						s[0] = 0;
						if( space_index[0] )
						{
							if( space_index[1] )
							{
								i = 0;
								for( j = ( space_index[0] + 1 ); j < space_index[1]; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[0] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}

							if( s[0] )
							{
								i = atoi( s );
								if( i > 0 )
								{
									DeviceType = i;
									Write_Flash();
									Read_Flash();
									change_prm = TRUE;
								}
								else
								{
									put_str( "\r\nError number\r\n" );
								}
							}
						}	// if( space_index[0] )
						else
						{
							put_str( "\r\nCommand error\r\n" );
						}
						cmd = TRUE;
						clr_input();
					}	// if( strcmp( s, "WRDT" ) == 0 )
				}
				// -----------------------------------------------------------


				// -----------------------------------------------------------
				// Input TM ( Ibutton timeout )
				// -----------------------------------------------------------
				if( !cmd )
				{
					if( strcmp( s, "TM" ) == 0 )
					{
						// Get first param
						s[0] = 0;
						if( space_index[0] )
						{
							if( space_index[1] )
							{
								i = 0;
								for( j = ( space_index[0] + 1 ); j < space_index[1]; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}
							else
							{
								n = strlen( usart_rx_data );
								i = 0;
								for( j = ( space_index[0] + 1 ); j < n; j++ )
								{
									s[i++] = usart_rx_data[j];
								}
								s[i] = 0;
							}

							if( s[0] )
							{
								i = atoi( s );
								if( ( i >= 0 ) && ( i < 11 ) )
								{
									iButton_Timeout = i * 8;
									Write_Flash();
									Read_Flash();
									change_prm = TRUE;
								}
								else
								{
									put_str( "\r\nError TM number\r\n" );
								}
							}
						}	// if( space_index[0] )
						else
						{
							put_str( "\r\nCommand error\r\n" );
						}
						cmd = TRUE;
						clr_input();
					}	// if( strcmp( s, "TM" ) == 0 )
				}
				// -----------------------------------------------------------


				if( !cmd )
				{
					put_str( "\r\nUnknown command" );
					put_str( "\r\n? - for help\r\n" );
					put_str( cli_data );
				}
				else
				{
					put_str( cli_data );
				}
			}	// if( n ) -- Length > 0
			else
			{
				put_str( cli_data );
			}
		}	// if( ret_button )
	}	// while( cli_mode )
	clr_input();
}
// ===========================================================================


// ===========================================================================
void Cli_State( void )
// ===========================================================================
{
	int	i, l, n, m;

	if( ret_button )	// Return button is pressed
	{
		m = 1;
		l = strlen( usart_rx_data );
		for( i = 0; i < l; i++ )
		{
			if( ( usart_rx_data[i] < '0' ) || ( usart_rx_data[i] > '9' ) )
			{
				m = 0;
			}
			if( islower( ( int ) usart_rx_data[i] ) )
			{
				usart_rx_data[i] = toupper( usart_rx_data[i] );
			}
		}	// for( i = 0; i < l; i++ )
		//ret_button = FALSE;

		if( strcmp( usart_rx_data, "CFG" ) == 0 )	// Goto to Config mode
		{
			Cli();	// CLI mode
		}
		else
		if( m )
		{
//put_str( "digit!\r\n" );
			n = atoi( usart_rx_data );
			if( ( n ) && ( n <= MAX_NN_CH ) )
			{
				n--;
//put_str( "digit-2!\r\n" );
				if( GPIO_X[n].gpio_x )
				{
					if( ( GPIO_X[n].dev_type == 3 ) || ( GPIO_X[n].dev_type == 4 ) )
					{
						l = GPIO_ReadInputDataBit( GPIO_X[n].gpio_x, GPIO_X[n].gpio_pin_x );
						if( l )
						{
							GPIO_ResetBits( GPIO_X[n].gpio_x, GPIO_X[n].gpio_pin_x );
						}
						else
						{
							GPIO_SetBits( GPIO_X[n].gpio_x, GPIO_X[n].gpio_pin_x );
						}
					}	// if( ( GPIO_X[n].dev_type == 3 ) || ( GPIO_X[n].dev_type == 4 ) )
					else
					{
						put_str( "\r\nChannel may be GPIO only.\r\n" );
					}
//put_str( "\r\n" );
				}
				else
				{
					put_str( "\r\nChannel not defined.\r\n" );
				}					
//clr_input();
			}	// if( ( n ) && ( n <= MAX_NN_CH ) )
			else
			{
				put_str( "\r\nWrong Nn of channel.\r\n" );
//clr_input();
			}
		}	// if( n )
		else
		{
			if( strlen( usart_rx_data ) )
			{
				put_str( "\r\nUnknown command\r\n" );
				put_str( "print\r\n" );
				put_str( "       <cfg> for Config\r\n" );
				put_str( "       <nn>\r\n" );
				put_str( "             nn = Nn of channel GPIO for reverse state\r\n" );
//clr_input();				
			}
			put_str( cli_data );	// Identifier
		}	// else - if( n )
	clr_input();
	}	// if( ret_button ) -- Return button is pressed
}
// ===========================================================================
