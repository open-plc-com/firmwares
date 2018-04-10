#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"
#include "mx_misc.h"

#include <stdio.h>

#define VCOMPORT_IN_FRAME_INTERVAL	5	// Interval between sending IN packets in frame number (1 frame = 1ms)

uint8_t				USB_Rx_Buffer[VIRTUAL_COM_PORT_DATA_SIZE];
extern uint8_t		USB_Tx_State;

// ===========================================================================
void put_usb( char *s, int len )
// ===========================================================================
{
	UserToPMABufferCopy( ( uint8_t* ) &s[0], ENDP1_TXADDR, len );
	SetEPTxCount( ENDP1, len );
	SetEPTxValid( ENDP1 ); 
}
// ===========================================================================


// ===========================================================================
void EP1_IN_Callback (void)
// ===========================================================================
{
	Handle_USBAsynchXfer();
}
// ===========================================================================


// ===========================================================================
void EP3_OUT_Callback(void)
// ===========================================================================
{
	uint16_t	USB_Rx_Cnt;

//char s[128];


	USB_Rx_Cnt = USB_SIL_Read( EP3_OUT, USB_Rx_Buffer );	// Get the received data buffer and update the counter

//put_usb( &s[0], 6 );

put_usb( (char*) &USB_Rx_Buffer[0], USB_Rx_Cnt );

	/* Enable the receive of data on EP3 */
	SetEPRxValid( ENDP3 );
}
// ===========================================================================


// ===========================================================================
void SOF_Callback(void)
// ===========================================================================
{
	static uint32_t		FrameCount = 0;

	if( bDeviceState == CONFIGURED )
	{
		if( FrameCount++ == VCOMPORT_IN_FRAME_INTERVAL )
		{
			FrameCount = 0;				// Reset the frame counter
			Handle_USBAsynchXfer();		// Check the data to be sent through IN pipe
		}
	}
}
// ===========================================================================
