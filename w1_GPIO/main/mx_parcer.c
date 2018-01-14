/*
	Name:			parcer.c
	Purpose:
	Author:			Alexander Suvorov (www.open-plc.com)
	Created:		2017/12
	Modified by:
	RCS-ID:
	Copyright:		(c) Alexander Suvorov
	Licence:		The MIT License (MIT)
*/


// ===========================================================================
void parce_cmd( char *str, int *space_index, int m )
// ===========================================================================
{
	int	i, j, n;

	n = strlen( usart_rx_data );
	if( n )		// Length > 0
	{
		// -----------------------------------------------------------
		// Trim left spaces
		// -----------------------------------------------------------
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
				str[j] = usart_rx_data[j];
			}
			str[j] = 0;
		}
		else
		{
			strcpy( str, usart_rx_data );
		}
		// -----------------------------------------------------------
	}
	else
	{
		str[0] = 0;
	}
	return;
}
// ===========================================================================


// ===========================================================================
void parce_prm( char *str, int nn, int *space_index, int space_index_size )
// ===========================================================================
{
	int	i, j, n;

	str[0] = 0;
	if( ( nn > 0 )  && ( nn <= space_index_size ) )
	{
		if( space_index[nn-1] )
		{
			if( space_index[nn] )
			{
				i = 0;
				for( j = ( space_index[nn-1] + 1 ); j < space_index[nn] + 1; j++ )
				{
					str[i++] = usart_rx_data[j];
				}
				str[i] = 0;
			}	// if( space_index[2] )
			else
			{
				n = strlen( usart_rx_data );
				i = 0;
				for( j = ( space_index[nn-1] + 1 ); j < n; j++ )
				{
					str[i++] = usart_rx_data[j];
				}
				str[i] = 0;
			}	// else -- if( space_index[4] )
		}
		else
		{
			str[0] = 0;
		}
	}
	else
	{
		str[0] = 0;
	}
}
// ===========================================================================
