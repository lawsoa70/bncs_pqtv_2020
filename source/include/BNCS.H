/****************************************************************************
*																									 *
*			(c) BBC 1992-2000	:	Author Simon Dowson									 *
*																									 *
*			BNCS.H   			:	General BNCS Constants							  	 *
*																									 *
****************************************************************************/
// V4.1	09/09/2004	Version for Synergy
//****************************************************************************/

// Limits
#define MAX_DEVICES	999
#define MAX_SLOTSIZE 256
#define MAX_SLOTS		4096

// Driver to CSI Status messages
#define STATUS_OK			0
#define STATUS_WARNING	1
#define STATUS_FAIL		2

// CSI Client connection mesages
#define CSI_CONNECT				   0x01
#define CSI_DISCONNECT				0x02
#define CSI_CONNECT32			   0x04

#define DRIVER_32BIT					0x8000
#define DRIVER_32BIT_MASK			0x7FFF

#define CTRL_32BIT		 			0x4000
#define CTRL_32BIT_MASK				0xBFFF

// Session result codes
#define	SUCCESS			0
#define	BUSY				1
#define	WSNOTFOUND		2
#define	FILENOTFOUND   3
#define	OPENERROR      4
#define	READERROR      5
#define	WRITEERROR     6
#define  FILEISNEWER    7
#define  DISKFULL       8
#define  TIMEOUT        9
#define  DEVICENOTFOUND	10

// Revertive modes
#define REVERTIVE			1
#define POLL				2

// Interface modes
#define IFMODE_NONE			0
#define IFMODE_RXONLY		1
#define IFMODE_TXRX			2
#define IFMODE_FORCETXRX	3
#define IFMODE_TXRXINQ     4
#define IFMODE_RXONLYBROKEN 5


// Packet header sizes
#define RR_HEADERSIZE	 24
#define IR_HEADERSIZE	 24
#define GR_HEADERSIZE	 16

// Device Driver support
#define RTRMESSAGE			0x1000
#define GPIMESSAGE			0x2000
#define INFMESSAGE			0x3000
#define UNIMESSAGE			0x4000
#define RTRMESSAGESIZE		8
#define GPIMESSAGESIZE		7
#define INFMESSAGESIZE		7
#define UNIMESSAGESIZE		7
#define SUCCESS				0
#define INVALIDDEVICE		1
#define DEVICEINUSE			2
#define DRIVERNOTENABLED	3
#define UNIHEADERSIZE		6
#define TYPEMASK				0xF000
#define DRIVERMASK			0x0FFF
#define WSNUMBER				3
#define COMMANDS				5

// Network name definations
#define STATUS_NETWORK_NAME 		"STATUS_CHANNEL"

// Applex <-> AppleCore interworking constants

#define AXAC_FILESAVE	1
#define AXAC_FILELOAD	2
#define AXAC_FILEAPPEND	3	