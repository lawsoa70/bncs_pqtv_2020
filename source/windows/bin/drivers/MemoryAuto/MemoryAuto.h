//////////////////////////////////////
// header file for MemoryAuto project //
//////////////////////////////////////


//
//
// Updating Notes, bug fixes etc
//
// Version 1.56 -- addition of able to choose the de-limiting character -- as some memories can be of comma delimited lists --- 
//                       -- so these memories need a different char -- user definable in ini file                                   PAW Apr 07
//
//
// version 1.58 -- replacement of srtstr with strcmp - stronger string comparison especially for checking memory names - Nov 07 


#if !defined(AFX_MEMORYAUTO_H__6E1F4185_1C5F_11D4_82C5_00105AC8787D__INCLUDED_)
#define AFX_MEMORYAUTO_H__6E1F4185_1C5F_11D4_82C5_00105AC8787D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786 4996)

#include "resource.h"

// TODO:	Insert additional constants, function prototypes etc. in this file

/* max size of buffer for data from string resource */
#define		MAX_LOADSTRING	100

/* max number of items in listbox */
#define		MAX_LB_ITEMS	1500

#define		APP_WIDTH			990
#define		APP_HEIGHT			750

#define		BUFFER_CONST            255
	
#define		INFO_DISCONNECTED  0	// infodriver not there or whatever
#define		INFO_RXMODE               1		// rx mode only - dual redundancy
#define		INFO_TXRXMODE          2		// connected, and in true rxtx mode
  
#define  CIRC_BUFFER_TIMER_ID             1
#define	 CIRC_BUFFER_TIMER_INTERVAL		200

#define  FIFO_BUFFER_TIMER_ID             2
#define	 FIFO_BUFFER_TIMER_INTERVAL		20

#define  LOAD_TIMER_ID             3
#define	 LOAD_TIMER_INTERVAL		3500 // alow time for all revertives to come back and be processed

#define  SNAPSHOT_TIMER_ID             4
#define	 SNAPSHOT_TIMER_INTERVAL		5500 // alow time for all revertives to come back and be processed

#define ONE_SECOND_TIMER_ID		6	 // could be used to relook for comms port / infodrivers -- for more intelligent driver

#define FORCETXRX_TIMER_ID      7					// 

#define  MAX_DATA_ENTRIES     131 //1..130..... was  101 // 1..100 entries - though never get this many as string length is limited to 255
#define  MAX_DEVICE_ENTRIES   17 // 1..16  - though may be only 2 or 3 at very most....trying to make it somewhat flexible and generic

#define   MAX_PREDEFINED_MEMORY_TYPES 2 // actually 1 defined memeory types read in from ini file that data may arrive in
#define   MAX_DEVICE_SIZE       4097  // 1..4096 for router or gpi revs or infodriver revs

#define   MAX_DEVICES_FOR_REVERTIVES  9  // 1..8 i.e. 8 device entries * 1 max_predfined memory types

#define  ROUTER_MEMORY         1		// list of sources or destinations for given router
#define  GPI_MEMORY                   2			// list of gpi states for gpis
#define  INFO_MEMORY                3       // string or slot numbers
#define  PREDEFINED_MEMORY 4       // string or numbers as defined in ini file eg RTR DEST 8, GPI OUT 4  == means 8 rtr dests followed by 4 gpis for specified devs in ini file; INF 255 == string of up to 255 chars

// editor command codes
// command types from panels
#define  SAVE_RECORD				   1  // new record or update existing record
#define  RECALL_RECORD				2  // i.e retieve record for editing at panel and set routes, gpis etc
#define  DELETE_RECORD				3  // delete entry from database
#define  SNAPSHOT_RECORD	  4  // take snapshot of current settings - for temp. period only
#define  POLL_RECORD                   5  // retrieve contents of slot and send to calling program

// defined slot usage in ext infodriver
// slots 1,11,21,31,41,51,61 for commands in from various programs
// slots 2,12,22,32,42,52,62 for answers to recall, poll commands back to various programs

#define  COMM_STATUS_SLOT 79
#define  MEMORY_COUNTER_SLOT 80 // slot holding how many records in memory
#define   MEMORY_START            81
#define   MAX_MEMORIES             4001  // 1.. 4000 can be really used for slots 81 - 4096, slot 80 fornumber of memories
#define   MEMORY_END				 4096

// slots 81 to 4096 are used for memory storage

#define		STRINGSEQUAL		0   // compare functions return 0 as equal


/* main.cpp */
/* prototypes for main functionality */
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgProcChild(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT		InitApp(HWND);
void				CloseApp(HWND);

void				LoadIni(void);
char*				r_p(char* file, char* section, char* entry, char* defval, BOOL fWrite);
void				w_p(char* file, char* section, char* entry, char* setting);

void				Debug(LPCSTR szFmt, ...);
void				Log(LPCSTR szFmt, ...);
int					SplitString(char *string, char *delim, UINT count, char **outarray );


/* MemoryAuto.cpp */

// specific structures for automatic

struct MemoryEntry {
			BOOL bEntryUsed;
			int iNumberOfMultipleSlotsUsed; // number of slots used to store complete memory
			int iWhichDefintionEntry;
			char szMemoryName[ BUFFER_CONST ]; // name from database
			char szMemoryData[ BUFFER_CONST ];  // contents of memory slot
};

struct DefinitionEntry {
			int iDeviceType;		  // rtr, gpi, inf memory type defined above 
			int iDeviceNumber;    // Colledia device number
			int iDatabase;	            // which db data refers to : 0/1 for rtr; in / out for gpi; db num for inf; 
			BOOL bRangeDefinition;   // if TRUE then only 2 entries in iData - start and end bounds for range, iDataCount == 2;
			int iDataCount;           // count of number of  iData and iDefault entries
			int iData[ MAX_DATA_ENTRIES ];        // numeric entries eg rtr dest numbers or gpi outs
			int iDefaults[ MAX_DATA_ENTRIES ];  // numeric entries eg rtr src defaults  or gpi state settings
			char szDataEntry[ BUFFER_CONST];   // string data - also used to store string version of defaults
};

struct DefinedDataEntry {
			BOOL bDefinitionUsed;
			char szTypeName[BUFFER_CONST];     
			int iKeyNameDatabase;	// db used to store key names - usually db 0
			int iDefinitionEntries;
			struct DefinitionEntry stDefinitionData[ MAX_DEVICE_ENTRIES ];
};

struct RevertiveData {
	int iNumericData;
	char szStringData[BUFFER_CONST];
};

struct DeviceRevertives {  // to hold router or gpi revertives; infodriver revs not stored
		int iDeviceNumber;
		int iDeviceSize;
		int iDeviceType;  // rtr, gpi, inf
		struct RevertiveData stRevertive[MAX_DEVICE_SIZE]; // for data from devices or infodrivers
};

struct MemoryString {
	char szData[BUFFER_CONST];
};

struct RecallMemoryData {
	int iStartMemoryEntry;  // first location in the main memory being recalled
	int iNumberOfElements;  // number of entries in this string list here - not number of memory entries
	int iCurrentElement;  // pointer to how far the processing has got within these elements
	struct MemoryString MemData[MAX_DATA_ENTRIES];
};

struct SubSetRecallData {
		int iDeviceNumber;				// Colledia device number
		BOOL bRangeDefinition;   // if TRUE then only 2 entries in iData - start and end bounds for range, iDataCount == 2;
		int iList[ MAX_DATA_ENTRIES ];        // numeric entries eg rtr dest numbers or gpi outs or info slot numbers
};

struct FiFoBuffer {
	int iReadEntry;
	int iLastEntry;
	struct MemoryString BufferData[MAX_DEVICE_SIZE];
};


/* prototypes for the notification functions */
void UpdateCounters(void);
void InfoNotify(extinfo* pex,UINT iSlot,LPCSTR szSlot);
LRESULT CSIClientNotify(extclient* pec,LPCSTR szMsg);

// TODO: add prototypes for functions from MemoryAuto_x.cpp

void InitializeMemoryStructures(void);
void ReRegister(void);
void ReadInIniFileSettings(void);
BOOL AutomaticRegistrations(void);
void PollAutomaticDrivers(void);
void FindFreeMemorySlot( void );
void SetDefaultMemorySlot(void);
int	LoadCurrentSetOfMemoriesFromInfodriver(void);
void LoadSnapShotMemory(void);
void CalculateMemoryGlobals( void );

int  Reset_Timer( int time_int, int iOrdinal);
void ProcessNextCircCommand (void);
void ProcessNextFifoCommand (void);

void CheckAndSetInfoResilience( void );
void ForceAutoIntoRXOnlyMode(void);
void ForceAutoIntoTXRXMode(void);
void Restart_CSI_NI_NO_Messages(void);

#ifndef EXT
#define EXT extern
#endif

/////////////////////////////////////////////////////////////////////
// Global Variables for the MemoryAuto project
/////////////////////////////////////////////////////////////////////

EXT HINSTANCE hInst;								// current instance
EXT TCHAR szTitle[MAX_LOADSTRING];			// The title bar text
EXT TCHAR szWindowClass[MAX_LOADSTRING];	// The title bar text
EXT char szResult[256];							// global for profile reads
EXT BOOL fLog;										// log enable flag
EXT BOOL fDebug;									// debug enable flag
EXT BOOL fDebugHide;								// debug visible flag
EXT HWND hWndMain,hWndList;					// window handles, main and debug
EXT HWND hWndDlg;									// window handle for child dialog
EXT int iChildHeight;							// height of child dialog
EXT RECT rc;
EXT int iDevice;									// command line parameter = external driver #
EXT int iWorkstation;							// workstation number from CSI.INI
EXT ULONG lTXDEV;									// Device tx count
EXT ULONG lRXDEV;									// Device rx count
EXT ULONG lTXID;									// ID tx count
EXT ULONG lRXID;									// ID rx count
EXT extinfo ei;									// an external infodriver class
EXT extclient ec;									// a client connection class

// TODO: add additional global variables here, preceded by EXT

EXT int iCircBufferEntriesCount;
EXT circbuf *cb;

EXT int iOverallTXRXModeStatus;			      // overall mode -- based on base infodriver i.e gpi input driver
EXT int iNextResilienceProcessing;        // count down to zero -- check info tx/rx status 
EXT int iOverallStateJustChanged;
EXT int iOverallModeChangedOver;

EXT BOOL bShowAllDebugMessages;

EXT int iNumberOfMemoryEntries;
EXT int iLastMemoryEntry;
EXT int iCurrentFreeMemorySlot;
EXT int iPercentMemoryFilled;
EXT int iMemorySaveSize;  // number of slots used per mem

EXT int iCircBufferTimerId;
EXT int iCircBufferInterval;
EXT BOOL bCircBufferTimerRunning;

EXT int iFifoBufferTimerId;
EXT BOOL bFifoBufferTimerRunning;

EXT int iSnapTimerId;
EXT int iLoadTimerId;

EXT struct DefinedDataEntry stDefinedData[ MAX_PREDEFINED_MEMORY_TYPES ];
EXT BOOL bMemoriesPreDefined;

EXT struct MemoryEntry  stMemory[MAX_MEMORIES]; // internal copy of infodriver for easier processing

EXT struct RecallMemoryData stRecallMemData; // global var only used for processing memory recalls - each mem line is split into string elements for processing
EXT struct SubSetRecallData stSubsetData[MAX_DEVICE_ENTRIES]; // global var only used for processing memory recalls if a subset of data is required

EXT struct DeviceRevertives  stDeviceRevertives[MAX_DEVICES_FOR_REVERTIVES]; // storage for revertives from devices monitored

EXT struct FiFoBuffer stCommandBuffer; // in place of circ buffer - as the circ buffer is limited to 659  entries only and this app needs space for 1000s 

//EXT struct MemoryEntry stDefaults[MAX_PREDEFINED_MEMORY_TYPES]; // default memory stored in infodriver as 1st entry now
//EXT struct MemoryEntry stSnapShots[MAX_PREDEFINED_MEMORY_TYPES]; // snap shot memory stored in infodriver as 2nd entry now

// added char and int of de-limiting character -- defaults to a , (comma) -- but if saving info slots with their own delimted lists -- it screws up if same delimtiting char
EXT char szDeLimiter[2]; // string version
EXT int iDelimiterValue;  // integer value of szDelimiting

EXT bool bCurrentlyRecalling;  // set to true when recalling memory, set to false when fifo buffer empty again

#endif // !defined(AFX_MEMORYAUTO_H__6E1F4185_1C5F_11D4_82C5_00105AC8787D__INCLUDED_)
