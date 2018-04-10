#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_pwr.h"
#include "hw_config.h"

__IO uint32_t bDeviceState	= UNCONNECTED;	// USB device status
__IO bool fSuspendEnabled	= TRUE;			// true when suspend is possible
__IO uint32_t EP[8];

struct
{
	__IO RESUME_STATE	eState;
	__IO uint8_t		bESOFcnt;
} ResumeS;

__IO uint32_t remotewakeupon=0;

RESULT PowerOn( void )
{
	uint16_t wRegVal;

	/*** cable plugged-in ? ***/
	USB_Cable_Config( ENABLE );

	/*** CNTR_PWDN = 0 ***/
	wRegVal = CNTR_FRES;
	_SetCNTR( wRegVal );

	/*** CNTR_FRES = 0 ***/
	wInterrupt_Mask = 0;
	_SetCNTR( wInterrupt_Mask );
	/*** Clear pending interrupts ***/
	_SetISTR(0);
	/*** Set interrupt mask ***/
	wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
	_SetCNTR( wInterrupt_Mask );
  
	return USB_SUCCESS;
}


RESULT PowerOff()
{
	return USB_SUCCESS;
}

void Suspend( void ) {}
void Resume_Init( void ) {}
void Resume( RESUME_STATE eResumeSetVal ) {}
