//////////////////////////////////////
// header file for BFEUMDLabelAuto project //
//////////////////////////////////////

#if !defined(AFX_BFEUMDLABELAUTO_H__6E1F4185_1C5F_11D4_82C5_00105AC8787D__INCLUDED_)
#define AFX_BFEUMDLABELAUTO_H__6E1F4185_1C5F_11D4_82C5_00105AC8787D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786 4996 4018)

#include "resource.h"

// TODO:	Insert additional constants, function prototypes etc. in this file

/* max size of buffer for data from string resource */
#define			MAX_LOADSTRING	100

/* max number of items in listbox */
#define		MAX_LB_ITEMS	500

/* buffer constant for strings */
#define		MAX_BUFFER_STRING 256


#define		APP_WIDTH	970
#define		APP_HEIGHT	920


// All Consts for this application can be found here for global use
#include "BFEUMDLabelCONSTS.h"


/* main.cpp */
/* prototypes for main functionality */
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgProcChild(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT				InitApp(HWND);
void				CloseApp(HWND);

void				LoadIni(void);
char *				getBNCS_File_Path( void );    // new for v4.2 of wizard - assigns the global szBNCS_File_Path
char*				r_p(char* file, char* section, char* entry, char* defval, BOOL fWrite);
void				w_p(char* file, char* section, char* entry, char* setting);
int					getWS(void);

void				Debug(LPCSTR szFmt, ...);
void				Log(LPCSTR szFmt, ...);

int				SplitString(char *string, char *delim, UINT count, char **outarray );
void          InitStatusListboxDialog( void );  // helper function to initialise listbox on app dialog


/* BFEUMDLabelAuto.cpp /*

/* prototypes for the notification functions */
void UpdateCounters(void);
void InfoNotify(extinfo* pex,UINT iSlot,LPCSTR szSlot);
LRESULT CSIClientNotify(extclient* pec,LPCSTR szMsg);

void CheckAndSetInfoResilience(void);
void ForceAutoIntoRXOnlyMode(void);
void ForceAutoIntoTXRXMode(void);
void Restart_CSI_NI_NO_Messages(void);

void InitializeMemoryStructures(void);
BOOL ConnectToInfodriver(void);
void	ProcessNextCommand( void );
void 	LoadAllInfodriverData( void );
int		Reset_Timer(int time_int, int iOrdinal);
void LoadFifoBufferWithUpdateCommands( void );
void LoadAllInitialisationData( void );
BOOL AutomaticRegistrations(void);
void PollAutomaticDrivers(void);
int 	LoadDB2AliasData( void );
int 	LoadDB3AliasData( void );
void LoadRouterDB2SoftNames(void);
void DisplayAllAliasDataListview(void);
bool findString( char szStr[BUFFER_CONST], char szFind[BUFFER_CONST]);
void SendAllUMDNamesToBFEDriverOnce();
void SendModifiedUMDNameToBFEDriver( int iSrc );

void InitStatusDialog(void);
void VersionInfo();

void UpdateFifoBufferCountDialog( void );
void  UpdateCommandTypeDialog( int  );
void  UpdateRevertiveTypeDialog( int  );
void UpdateInfodriverCounter( int iInfo, int iWhich );
void  UpdateDialogCounter( int iDC , int iVal );

BOOL ProcessRouterRevertive( int iDestination, int iSource );
void ProcessSNCSRevertive( int iRevSlot, char szData[BUFFER_CONST] );
void ProcessAllReEnterantDestinations( void );
void ResolveSNCSNewLabels( int iIndex );

#ifndef EXT
#define EXT extern
#endif

/////////////////////////////////////////////////////////////////////
// Global Variables for the BFEUMDLabelAuto project
/////////////////////////////////////////////////////////////////////

EXT HINSTANCE hInst;						// current instance
EXT TCHAR szTitle[MAX_LOADSTRING];			// The title bar text
EXT TCHAR szWindowClass[MAX_LOADSTRING];	// The title bar text
EXT char szBNCS_File_Path[MAX_BUFFER_STRING]; // global for Colledia system file path - assigned in getBNCS_File_Path, used in r_p and w_p
EXT char szBNCS_Log_File_Path[MAX_BUFFER_STRING];  // global for crash log file path - assigned in getBNCS_File_Path
EXT char szAppName[ MAX_LOADSTRING];                     // application name used in crash log messages
EXT char szResult[MAX_BUFFER_STRING];						// global for profile reads
EXT BOOL fLog;								// log enable flag
EXT BOOL fDebug;							// debug enable flag
EXT BOOL fDebugHide;						// debug visible flag
EXT HWND hWndMain,hWndList;					// window handles, main and debug
EXT HWND hWndDlg;							// window handle for child dialog
EXT int iChildHeight;						// height of child dialog
EXT RECT rc;
EXT int iDevice;							// command line parameter = external driver #
EXT int iWorkstation;						// workstation number from CSI.INI

EXT ULONG lTXDEV;							// Device tx count
EXT ULONG lRXDEV;							// Device rx count
EXT ULONG lTXID;							// InfoID tx count
EXT ULONG lRXID;							// InfoID rx count

EXT extclient *ecBFEClient;				// a client connection class
EXT extinfo *eiBFEInfoId;			// external infodriver for driver

// general vars

EXT int iTXRXModeStatus;							// overall mode -- based on base infodriver i.e gpi input driver
EXT BOOL bConnectedToInfoDriverOK;	// connected to the infodriver or not
EXT BOOL bCommsEstablished; // true if comms ok, false if not
EXT BOOL bValidCollediaStatetoContinue;
EXT BOOL bAutomaticStarting;
EXT BOOL bShowAllDebugMessages;
EXT int iWhichStudio;
EXT bncs_string ssWhichStudio;

EXT int iOverallTXRXModeStatus;			 // overall mode -- based on base infodriver i.e gpi input driver
EXT int iNextResilienceProcessing;        // count down to zero -- check info tx/rx status 
EXT int iOverallStateJustChanged;
EXT int iOverallModeChangedOver;

EXT int iOneSecondPollTimerId;
EXT BOOL bOneSecondPollTimerRunning;
EXT int iProcessRevsCounter;  // count down to zero -- hold off umds processing until all rtr revs stored first time
EXT int iClockTimerId;
EXT int iUMDTimerId;

EXT int iNextCommandTimerId;
EXT BOOL bNextCommandTimerRunning;
EXT BOOL bUpdateAllUMDLabels;
EXT BOOL bResolvingNewSNCSLabels;
EXT int iResolvingNewSNCSIndex;

EXT CFiFoBuffer* fifoBuffer;     // FIFO buffer used to buffer commands to hardware

EXT CRevsSNCSData* sncsData;   // data class to hold revs from sncs system
EXT CRevsRouterData* routerData;   // data class to hold revs from router system

EXT CKSCLabelData* kscUmdData; // data class to hold  umd labels made in this auto to be passed onto bfe driver, and revertives from it

EXT CBFEUMDLabelDatabase* allLabelsDatabase; // Central database from which all labels etc can be determined 

EXT HWND hWndUMDData1;			//handle for the listview showing umd data  sncs details 
EXT HWND hWndUMDData2;			//handle for the listview showing umd data  rtr details
EXT HWND hWndDestData;			//handle for the listview showing destination - umd output data  details

EXT int iCurrentDate;  // to be used to compare with dates stored in records - to remove new status of sncs labels
EXT int iCurrentTime;  // to be used to compare with dates stored in records - 

#endif // !defined(AFX_BFEUMDLABELAUTO_H__6E1F4185_1C5F_11D4_82C5_00105AC8787D__INCLUDED_)
