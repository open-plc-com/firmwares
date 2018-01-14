#ifndef __GBL__
#define __GBL__

#ifdef __cplusplus
	extern "C" {
#endif

#define VER					"\r\r\n\n(c) Open-PLC - GPIO module v1.5 (open-plc.com)\r\n\n? - help\r\n"
#define CLI					"\r\nW1> "							// Cli prompt
#define DEV_TYPE			10000								// Device type
#define DEV_ID													// Device S/N as S/N of microcontroller
//#define OSC_INT												// Use internal oscilator
//#define LED_BLINK												// LED blink
//#define F_APB1			24000000							// Sys frequency in Hz
#define F_APB1				48000000							// Sys frequency in Hz
#define F_DWT				( F_APB1 / 1000000 )
#define FLASH_KEY1			( ( uint32_t ) 0x45670123 )
#define FLASH_KEY2			( ( uint32_t ) 0xCDEF89AB )
#define Page_63				0x0800FC00

#define RX_LEN				64									// Size of buffer UART Rx
#define SPI_LEN				24									// Size of buffer SPI

#define MAX_NN_CH			16									// Maximum number of channels
#define MAX_DEV_TYPE		5									// Maximum number of device type
#define MAX_W1_ERR			5									// Maximum continuing error counter
#define SHOW_DATA_INTERVAL	50									// 10 * SHOW_DATA_INTERVAL mSec

#define DWT_CYCCNT			*( volatile uint32_t* ) 0xE0001004
#define DWT_CONTROL			*( volatile uint32_t* ) 0xE0001000
#define SCB_DEMCR			*( volatile uint32_t* ) 0xE000EDFC
#define DWT_LAR				*( volatile uint32_t* ) 0xE0001FB0

static volatile uint32_t	DeviceType;							// Device type
static volatile uint32_t	Sn;									// Serial number (SN)
static volatile uint16_t	Noc;								// CAN number
static volatile uint16_t	Speedc;								// CAN speed
static volatile bool		NoAnswer;							// Not answer for broadcast SN
static volatile bool		CanErr;

static volatile uint8_t		rx_idx;								// Index in UART recieve buffer
static volatile bool		ret_button;							// Return is pressed
static volatile bool		cli_mode;							// CLI mode
static char					usart_rx_data[RX_LEN];				// recieve buffer data
static char					cli_data[16];						// CLI prompt

static volatile uint32_t	DelayTime;
static volatile uint32_t	CntTime;							// Counter of quants of System Timer
static volatile uint32_t	WD_Time;							// Time of watchdog
static volatile uint32_t	CANDownTime;

//static volatile uint8_t	spi1_tx_data[SPI_LEN];				// Buffer of Tx SPI1
//static volatile uint8_t	spi1_rx_data[SPI_LEN];				// Buffer of Rx SPI1
static volatile uint8_t		spi2_tx_data[SPI_LEN];				// Buffer of Tx SPI2
static volatile uint8_t		spi2_rx_data[SPI_LEN];				// Buffer of Rx SPI2

static volatile uint8_t		can_id[4];							// Header of CAN packet
static volatile uint8_t		can_data[8];						// CAN data

static volatile uint16_t	Nn_Ch;								// Number of channels
static volatile uint16_t	iButton_Timeout;
static volatile uint16_t	Show_Data;

struct GPIO_Struct
{
	GPIO_TypeDef	*gpio_x;		// GPIO_X (Port)
	uint16_t		gpio_pin_nn;	// Pin Nn
	uint16_t		gpio_pin_x;		// GPIO_Pin_x
	uint16_t		dev_type;		// Device type: 1 - DS18B20
									//				2 - iButton
									//				3 - GPIO pull Down
									//				4 - GPIO pull Up
	uint64_t		raw_data;		// Raw data
	uint16_t		err;			// 1-Wire continuous error count
	uint16_t		ibutton_time;	// The rest of the time for remember the value iButton
} GPIO_Data;

struct GPIO_Struct	GPIO_X[MAX_NN_CH];

static volatile uint32_t	lc1, lc2, lc3;
static volatile uint32_t	Step_DS;
static volatile uint32_t	Step_BTN;


#ifdef __cplusplus
	}
#endif

#endif /* __GBL__ */
