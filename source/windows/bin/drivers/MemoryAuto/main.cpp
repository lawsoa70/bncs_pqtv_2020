///////////////////////////////////////////////
//	main.cpp - core code for the application //
///////////////////////////////////////////////

/************************************
  REVISION LOG ENTRY
  Revision By: Chris Gil
  Revised on 23/07/2002 19:40:00
  Version: V
  Comments: Added SNMP 
 ************************************/

/************************************
  REVISION LOG ENTRY
  Revision By: TimA
  Revised on 18/07/2001 11:29:51
  Version: V
  Comments: Modified extinfo, extclient, extdriver, to have connect() function so
			all constructors take no parameters
 ************************************/


/************************************
  REVISION LOG ENTRY
  Revision By: TimA
  Revised on 28/03/2001 14:42:29
  Version: V
  Comments: new comms class specifies the serial port number in the open() function
  now, rather than the constructor
 ************************************/



#include "stdafx.h"
#define EXT
#include "MemoryAuto.h"

//
//  FUNCTION: WinMain()
//
//  PURPOSE: 
//
//  COMMENTS: main entry point for BNCS Driver "MemoryAuto"
//
//
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	
	MSG msg;
	HACCEL hAccelTable;
	char acTemp[256];

	lstrcpyn(acTemp,lpCmdLine,255);
	iDevice=atoi(acTemp);
		// Initialize global strings
		LoadString(hInstance, IDS_APP_TITLE, acTemp, MAX_LOADSTRING);
		wsprintf(szTitle,acTemp,iDevice);
		LoadString(hInstance, IDC_APPCLASS, szWindowClass, MAX_LOADSTRING);

		MyRegisterClass(hInstance);

		// Perform application initialization:
		if (!InitInstance (hInstance, nCmdShow)) 
			{
			return FALSE;
			}

		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MEMORYAUTO);

		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0)) 
			{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
				{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_REG);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_MEMORYAUTO;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

	hWndMain = CreateWindow(szWindowClass, szTitle, WS_SYSMENU|WS_MINIMIZEBOX ,
      CW_USEDEFAULT, 0, APP_WIDTH, APP_HEIGHT, NULL, NULL, hInstance, NULL);

   if (!hWndMain)
   {
      return FALSE;
   }

#ifdef _DEBUG
   ShowWindow(hWndMain,nCmdShow);
#else
   ShowWindow(hWndMain, SW_MINIMIZE);
#endif

   UpdateWindow(hWndMain);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  
//	COMMENTS:
//	
//	WM_COMMAND	- process the application menu
//  WM_DESTROY	- post a quit message and return
//	
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rctWindow;
	int wmId, wmEvent;
	char szFileName[256];
	char szSection [MAX_LOADSTRING];
	LoadString(hInst, IDS_SECTION, szSection, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_CREATE:
			return InitApp(hWnd);

		case WM_TIMER:			
			switch ( wParam ) {
				case CIRC_BUFFER_TIMER_ID:
					//Debug( " Circ timer executed " );
					ProcessNextCircCommand();
				break;

				case FIFO_BUFFER_TIMER_ID:
					ProcessNextFifoCommand();
				break;

				case LOAD_TIMER_ID:
					// load in current known memories from infodriver ?
					Debug( " Load timer executed " );
					SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Retrieving");
					KillTimer( hWndMain, LOAD_TIMER_ID );
					iNumberOfMemoryEntries = LoadCurrentSetOfMemoriesFromInfodriver();
					CalculateMemoryGlobals( );
					SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Running OK");
				break;

				case SNAPSHOT_TIMER_ID:
					Debug( " Snapshot timer executed " );
					KillTimer( hWndMain, SNAPSHOT_TIMER_ID );
					LoadSnapShotMemory(); // now make space and store current settings in memory
				break;

				case ONE_SECOND_TIMER_ID: // used for housekeeping and cyclic polling of hardware 
					//Debug("second timer");

					// 1. check on infodriver status
					iNextResilienceProcessing--;
					if (iNextResilienceProcessing <= 0) {
						CheckAndSetInfoResilience();
						iNextResilienceProcessing = 41; // prime number
					}

					// 2. check if recently failed over
					if (iOverallModeChangedOver>0) {
						iOverallModeChangedOver--;
						if (iOverallModeChangedOver <= 0)
							Restart_CSI_NI_NO_Messages();
					}

					// any other actions required during housekeeping timer -- process any pending commands if command timer not already running

				break;

				case FORCETXRX_TIMER_ID:
					Debug("ForceTxrx timer");
					KillTimer(hWndMain, FORCETXRX_TIMER_ID);
					ForceAutoIntoTXRXMode();
				break;

			} // switch wparam
		break;

		case WM_SIZE: // if the main window is resized, resize the listbox too
			//Don't resize window with child dialog
			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;

				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;

				case ID_DEBUG_CLEAR:
					SendMessage(hWndList,LB_RESETCONTENT,0,0L);
					break;

				case ID_DEBUG_ON:
					GetWindowRect(hWnd, &rctWindow);
					if (fDebug==TRUE) {
						CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_UNCHECKED);
						sprintf(szFileName, "dev_%03d.ini", iDevice);
						w_p(szFileName, szSection, "DebugMode", "0");
						fDebug=FALSE;

					} 
					else {
						MoveWindow(hWnd, rctWindow.left, rctWindow.top, APP_WIDTH, APP_HEIGHT, TRUE);
						CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_CHECKED);
						sprintf(szFileName, "dev_%03d.ini", iDevice);
						w_p(szFileName, szSection, "DebugMode", "1");
						fDebug=TRUE;

						CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_HIDE,MF_UNCHECKED);
						fDebugHide=FALSE;
					}
					break;

				case ID_DEBUG_HIDE:
					GetWindowRect(hWnd, &rctWindow);
					if (fDebugHide==TRUE) {
						MoveWindow(hWnd, rctWindow.left, rctWindow.top, APP_WIDTH, iChildHeight + 42, TRUE);
						CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_HIDE,MF_CHECKED);
						fDebugHide=FALSE;

						CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_UNCHECKED);
						sprintf(szFileName, "dev_%03d.ini", iDevice);
						w_p(szFileName, szSection, "DebugMode", "0");
						fDebug=FALSE;
					} 
					else {
						MoveWindow(hWnd, rctWindow.left, rctWindow.top, APP_WIDTH, APP_HEIGHT, TRUE);
						CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_HIDE,MF_UNCHECKED);
						fDebugHide=TRUE;

						CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_CHECKED);
						sprintf(szFileName, "dev_%03d.ini", iDevice);
						w_p(szFileName, szSection, "DebugMode", "1");
						fDebug=TRUE;
					}
					break;

				case  ID_DEBUG_SHOWALLMESSAGES:
					if (bShowAllDebugMessages == TRUE) {
						Debug("-- turning off most debug messages");
						bShowAllDebugMessages = FALSE;
						CheckMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_DEBUG_SHOWALLMESSAGES, MF_UNCHECKED);
					}
					else {
						Debug("++ Turning on all debug messages");
						bShowAllDebugMessages = TRUE;
						CheckMenuItem(GetSubMenu(GetMenu(hWnd), 1), ID_DEBUG_SHOWALLMESSAGES, MF_CHECKED);
					}
				break;

				case ID_OPTIONS_FORCETXRX:
					Debug("Forcing Automatic into TXRX Mode");
					ForceAutoIntoTXRXMode();
				break;

				case ID_OPTIONS_FORCERXONLY:
					Debug(" Trying to force driver into RXONLY mode");
					Debug(" *** Now force the other Automatic into TXRX mode to complete operation ***");
					ForceAutoIntoRXOnlyMode();
				break;

				case ID_OPTIONS_RELOAD: // read in ini and database files 
					{
						SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Resetting");
						InitializeMemoryStructures();
						ReadInIniFileSettings();
						SetDefaultMemorySlot();
						// load in current known memories from infodriver ?
						SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Retrieving");
						iNumberOfMemoryEntries = LoadCurrentSetOfMemoriesFromInfodriver();
						LoadSnapShotMemory();
						CalculateMemoryGlobals( );
						SetDlgItemText(hWndDlg, IDC_CSI_STATUS, "Registering..." );
						AutomaticRegistrations();
						PollAutomaticDrivers();
						SetDlgItemText(hWndDlg, IDC_CSI_STATUS, "Connected OK" );
						SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Running OK");				
					}
					break; 

				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;

		case WM_DESTROY:
			CloseApp(hWnd);
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

//
//  FUNCTION: DlgProcChild()
//
//  PURPOSE: Message handler for child dialog.
//
//  COMMENTS: Displays the interface status
//
// 
LRESULT CALLBACK DlgProcChild(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;
			break;
		default:
			break;
	}
    return FALSE;
}

//
//  FUNCTION: About()
//
//  PURPOSE: Message handler for about box.
//
//  COMMENTS: Displays the compile date info
//				and used to display version info from
//				a version resource
//
// 
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			{
			LPBYTE   abData;
			DWORD  handle;
			DWORD  dwSize;
			LPBYTE lpBuffer;
			LPSTR szName, szModFileName, szBuf;
			char acBuf[16];
			#define	PATH_LENGTH	256
			
			szName=(LPSTR)malloc(PATH_LENGTH);
			szModFileName=(LPSTR)malloc(PATH_LENGTH);
			szBuf=(LPSTR)malloc(PATH_LENGTH);

			/*get version info*/

			GetModuleFileName(hInst,szModFileName,PATH_LENGTH);

			dwSize = GetFileVersionInfoSize(szModFileName, &handle);

			abData=(LPBYTE)malloc(dwSize);

			GetFileVersionInfo(szModFileName, handle, dwSize, abData);
			
			if (dwSize)
				{
				/* get country translation */
				VerQueryValue((LPVOID)abData, "\\VarFileInfo\\Translation", (LPVOID*)&lpBuffer, (UINT *) &dwSize);
				/* make country code */
				wsprintf(acBuf,"%04X%04X",*(WORD*)lpBuffer,*(WORD*)(lpBuffer+2));

				if (dwSize!=0)
					{
					/* get a versioninfo file version number */
					wsprintf(szName,"\\StringFileInfo\\%s\\FileVersion",(LPSTR)acBuf);
					VerQueryValue((LPVOID)abData, (LPSTR) szName, (LPVOID*)&lpBuffer,(UINT *) &dwSize);
					}

				/* copy version number from byte buffer to a string buffer */
				lstrcpyn(szModFileName,(LPSTR)lpBuffer,(int)dwSize);

				/* copy to the dialog static text box */
				wsprintf(szBuf,"Version %s",szModFileName);
				SetDlgItemText (hDlg, IDC_VERSION, szBuf);
				}
			
			wsprintf(szName,"%s - %s",(LPSTR)__DATE__,(LPSTR)__TIME__);
			SetDlgItemText(hDlg,IDC_COMPID,szName);

			free((PVOID)abData);
			free((PVOID)szName);
			free((PVOID)szModFileName);
			free((PVOID)szBuf);
			}
			return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

//
//  FUNCTION: InitApp()
//
//  PURPOSE: Things to do when main window is created
//
//  COMMENTS: Called from WM_CREATE message
//			 We create a list box to fill the main window to use as a
//			debugging tool, and configure the serial port and infodriver 
//			connections, as well as a circular fifo buffer
//
//

LRESULT InitApp(HWND hWnd)
{
/* return 0 if OK, or -1 if an error occurs*/
RECT rctWindow;
//char szTmp[64];
//int iI;

// Init Status Dialogs
hWndDlg = CreateDialog(hInst, (LPCTSTR)IDD_STATUS, hWnd, (DLGPROC)DlgProcChild);
GetClientRect(hWndDlg, &rctWindow);
iChildHeight = rctWindow.bottom - rctWindow.top + 2;

GetClientRect(hWnd,&rc);
hWndList=CreateWindow("LISTBOX","",WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_USETABSTOPS,
		0,iChildHeight,rc.right-rc.left,rc.bottom-rc.top-iChildHeight,
		hWnd,(HMENU)IDW_LIST,hInst,0);
ShowWindow(hWndList,SW_SHOW);

SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Initialising");

bShowAllDebugMessages = FALSE;
iOverallTXRXModeStatus = 0;
iNextResilienceProcessing = 11;
iOverallStateJustChanged = 0;
iOverallModeChangedOver = 15;

LoadIni();

if (fDebug) {
	MoveWindow(hWnd, rctWindow.left, rctWindow.top, APP_WIDTH, APP_HEIGHT, TRUE);
	CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_CHECKED);
	CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_HIDE,MF_UNCHECKED);
}
else {
	MoveWindow(hWnd, rctWindow.left, rctWindow.top, APP_WIDTH, iChildHeight + 42, TRUE);
	CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_UNCHECKED);
	CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_HIDE,MF_CHECKED);
}

ei.notify(InfoNotify);
switch (ei.connect(iDevice)) // connect to infodriver
	{
	case CONNECTED: 
		Debug("Connected OK to infodriver %d",ei.iDevice);
		SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "Connected OK");
		break;

	case CANT_FIND_INFODRIVER:
		Debug("Connect failed to infodriver %d",ei.iDevice);
//		wsprintf( szTmp, "Failed to connect to infodriver %d",ei.iDevice ); 
//		MessageBox( hWnd, szTmp, szTitle, MB_ICONSTOP);
//		PostMessage( hWnd, WM_CLOSE, 0, 0 );
		break;

	default:
			Debug("ERROR ERROR - Connect failed to infodriver, code %d",ei.connect(iDevice));
			Debug("ERROR ERROR -  Driver not functioning");
	}

// get state of infodriver
ei.requestmode = TO_TXRX;
iOverallTXRXModeStatus = ei.getmode(false);
switch (iOverallTXRXModeStatus) {
	case INFO_DISCONNECTED: 
		SetDlgItemText(hWndDlg, IDC_INFO_STATUS, " !!! DISCONNECTED !!! ");
	break;
	case INFO_RXMODE:
		SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "  *** RX ONLY ***");
		iOverallTXRXModeStatus = INFO_RXMODE;
	break;
	case INFO_TXRXMODE:
		SetDlgItemText(hWndDlg, IDC_INFO_STATUS, " TX+RX Ok");
		iOverallTXRXModeStatus = INFO_TXRXMODE;
	break;
	default:
		SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "  ??? unknown ???");
		Debug("Other Error - code %d",ei.iStatus);
		
}

	ei.setcounters(&lTXID, &lRXID);

ec.notify(CSIClientNotify);
switch (ec.connect())
	{
	case CONNECTED: 
		Debug("Connected OK to CSI");
	SetDlgItemText(hWndDlg, IDC_CSI_STATUS, "Connecting..." );
		break;

	case CANT_FIND_CSI:
		Debug("Connect failed to CSI");
//		wsprintf( szTmp, "Failed to connect to CSI %d",ec.getstate() ); 
//		MessageBox( hWnd, szTmp, szTitle, MB_ICONSTOP);
//		PostMessage( hWnd, WM_CLOSE, 0, 0 );
		break;

	default:
		Debug("Other Error - code %d",ec.getstate());
//		wsprintf( szTmp, "ERROR in connecting to CSI %d",ec.getstate() ); 
//		MessageBox( hWnd, szTmp, szTitle, MB_ICONSTOP);
//		PostMessage( hWnd, WM_CLOSE, 0, 0 );
	}
	ec.setcounters(&lTXDEV, &lRXDEV);

	ei.updateslot( COMM_STATUS_SLOT, "1" ); // set info slot to ok
	SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Initialising");

	InitializeMemoryStructures();

	ReadInIniFileSettings();
	SetDefaultMemorySlot();

	SetDlgItemText(hWndDlg, IDC_CSI_STATUS, "Registering..." );
	AutomaticRegistrations();
	bFifoBufferTimerRunning = TRUE;
	PollAutomaticDrivers();
	// set timer for device polls - loaded into fifo buffer
	iFifoBufferTimerId = SetTimer( hWnd, FIFO_BUFFER_TIMER_ID, 150, NULL );
	Debug( "Fifobuffer start timer returned %d ", iFifoBufferTimerId );

	SetDlgItemText(hWndDlg, IDC_CSI_STATUS, "Connected OK" );	
	SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Running OK");

	// set timer after which to load snapshot - which can only be done when all revs are recieved....
	iLoadTimerId = SetTimer( hWnd, LOAD_TIMER_ID, LOAD_TIMER_INTERVAL, NULL);
	Debug( "Load start timer returned %d ", iLoadTimerId );

	// set timer after which to load snapshot - which can only be done when all revs are recieved....
	iSnapTimerId = SetTimer( hWnd, SNAPSHOT_TIMER_ID, SNAPSHOT_TIMER_INTERVAL, NULL);
	Debug( "Snapshot start timer returned %d ", iSnapTimerId );
	SetTimer(hWnd, ONE_SECOND_TIMER_ID, 1000, NULL);

	bCurrentlyRecalling=FALSE;
	return 0;
}
//
//  FUNCTION: CloseApp()
//
//  PURPOSE: Things to do when main window is destroyed
//
//  COMMENTS: Called from WM_DESTROY message
//			The listbox debug window is a child of the main, so it will be 
//			destroyed automatically. 
//
void CloseApp(HWND hWnd)
{
	KillTimer(hWnd, CIRC_BUFFER_TIMER_ID);
	KillTimer(hWnd, SNAPSHOT_TIMER_ID);
}


//
//  FUNCTION: LoadIni()
//
//  PURPOSE: Assign (global) variables with values from .ini file
//
//  COMMENTS: The [section] name is gotten from a stringtable resource
//			 which defines it. It is the same name as the application, defined by the wizard
//			
//			
void LoadIni(void)
{
char szFileName[32];
char szSection [MAX_LOADSTRING];

iWorkstation=atoi(r_p("CSI.INI","Network","Workstation","0",FALSE));

wsprintf(szFileName,"DEV_%03d.ini",iDevice);
LoadString(hInst, IDS_SECTION, szSection, MAX_LOADSTRING);

/* extract debug and log parameters from ini file */
fDebug=atoi(r_p(szFileName,szSection,"DebugMode","1",TRUE));
fDebugHide = fDebug;
fLog=atoi(r_p(szFileName,szSection,"LogMode","0",TRUE));

// get buffer timer interval - default to 80ms
// get poll timer - now a ini file setting for tweaking - but also communication to device also improved w.r.t speed
iCircBufferInterval = 35;
iCircBufferInterval=atoi(r_p(szFileName,szSection,"CommandInterval","20",TRUE)); // get command dispatch interval
Debug( "Command Interval is set to %d ", iCircBufferInterval );

// ver 1.56 -- read in delimiting char -- default is a comma
strcpy( szDeLimiter, "," );
iDelimiterValue = 44;  // int of ,

strcpy( szDeLimiter, r_p(szFileName,szSection,"DeLimiterCharacter",",",TRUE) );
iDelimiterValue = atoi(szDeLimiter);
if (iDelimiterValue==0) iDelimiterValue=44; // put it back

Debug( "Delimiting Character is set to %s %d", szDeLimiter, iDelimiterValue);
if (strlen(szDeLimiter)!=1) {
	Debug( "ERROR -- Delimiter string is NOT 1 character long -- this will cause major errors - defaulting to comma");
	strcpy(szDeLimiter, "," );
}
SetDlgItemText(hWndDlg, IDC_DB_DELIMITER, szDeLimiter);	

}



/**************************************************
		my function to read one item of user data
		returns pointer to char data
		********************************************/
char* r_p(char* file, char* section, char* entry, char* defval, BOOL fWrite)
{
	char szPath[256];	
	char* szCCRoot = getenv( "CC_ROOT" );
	char* szCCSystem = getenv( "CC_SYSTEM" );

	if(szCCRoot && szCCSystem){
		sprintf(szPath, "%s\\%s\\config\\system\\%s", szCCRoot, szCCSystem, file);
	}
	else {
		strcpy(szPath, file);
	}

	GetPrivateProfileString(section, entry, defval, szResult, sizeof(szResult), szPath);
	if (fWrite)
		w_p(file,section,entry,szResult);

	return szResult;
}

/*!
\returns void 
\param char* file The file to read from
\param char* section The section - [section]
\param char* entry The entry - entry=
\param char* setting The value to write - =value

\brief Function to write one item of user data to an INI file
	
First checks to see if %CC_ROOT% and %CC_SYSTEM% are defined
- If so the config files in %CC_ROOT%\%CC_SYSTEM%\config\system are used
- Otherwise the config files in %WINDIR% are used
*/
void w_p(char* file, char* section, char* entry, char* setting)
{
	char szPath[256];	
	char* szCCRoot = getenv( "CC_ROOT" );
	char* szCCSystem = getenv( "CC_SYSTEM" );

	if(szCCRoot && szCCSystem){
		sprintf(szPath, "%s\\%s\\config\\system\\%s", szCCRoot, szCCSystem, file);
	}
	else {
		strcpy(szPath, file);
	}
	WritePrivateProfileString(section, entry, setting, szPath);
}

//
//  FUNCTION: UpdateCounters()
//
//  PURPOSE: Writes current counter values to child dialog
//
//  COMMENTS: 
//				
//			
//			
void UpdateCounters(void) {
	char szBuf[32];
	//Update Infodriver External Message Count
	wsprintf(szBuf, "%08d", lTXDEV);
	SetDlgItemText(hWndDlg, IDC_DEVTX, szBuf);
	wsprintf(szBuf, "%08d", lRXDEV);
	SetDlgItemText(hWndDlg, IDC_DEVRX, szBuf);
	//Update CSI Comms Message Count
	wsprintf(szBuf, "%08d", lTXID);
	SetDlgItemText(hWndDlg, IDC_IDTX, szBuf);
	wsprintf(szBuf, "%08d", lRXID);
	SetDlgItemText(hWndDlg, IDC_IDRX, szBuf);
}

// Function resets a specified timer
int Reset_Timer(int time_int, int iOrdinal) {
	int iResult;
	KillTimer(hWndMain, iOrdinal);
	iResult = SetTimer(hWndMain, iOrdinal, time_int, NULL);
	return iResult;
}

//
//  FUNCTION: Debug()
//
//  PURPOSE: Writes debug information to the listbox
//
//  COMMENTS: The function works the same way as printf
//				example: Debug("Number=%d",iVal);
//			
//			
void Debug(LPCSTR szFmt, ...)
{
if (fDebug)
	{
	LPSTR szDebug=(LPSTR)malloc(0x10000);
	va_list argptr;

	va_start(argptr,szFmt);
	vsprintf(szDebug,szFmt,argptr);
	va_end(argptr);

	if (strlen(szDebug) > 160)
		szDebug[160]=0; //Truncate long debug messages

	SendMessage(hWndList, LB_ADDSTRING,0,(LPARAM) szDebug);	//Add message
	long lCount = SendMessage(hWndList,LB_GETCOUNT,0,0);	//Get List Count
	if (lCount > MAX_LB_ITEMS)
		{
		SendMessage(hWndList, LB_DELETESTRING, 0, 0);	//Delete item
		lCount--;
		}
	SendMessage(hWndList, LB_SETTOPINDEX, (WPARAM) lCount - 1, 0);	//Set Top Index
	free((PVOID)szDebug);
	}
}

//
//  FUNCTION: Log()
//
//  PURPOSE: Writes information to the log file
//
//  COMMENTS: The function works the same way as printf
//				example: Debug("Number=%d",iVal);
//			
//			
void Log(LPCSTR szFmt, ...)
{
char szLogFile[40];

if (fLog)
	{
	char tBuffer[9];
	char szDate[16];
	FILE *fp;
	LPSTR szLog=(LPSTR)malloc(0x10000);
	va_list argptr;
   struct tm *newtime;
   time_t long_time;

	va_start(argptr,szFmt);
	vsprintf(szLog,szFmt,argptr);
	va_end(argptr);

	time( &long_time );                // Get time as long integer. 
	newtime = localtime( &long_time ); // Convert to local time. 
	_strtime( tBuffer );
	sprintf(szDate, "%04d%02d%02d", newtime->tm_year+1900, newtime->tm_mon+1, newtime->tm_mday);

	wsprintf(szLogFile, "c:\\bncslogs\\%s_child2_%d.log", szDate, iDevice);
	Debug(szLogFile);
	if (fp = fopen(szLogFile, "a"))
	{
		fprintf(fp, tBuffer);
		fprintf(fp, " - ");
		fprintf(fp, szLog);
		fprintf(fp, "\r\n");
		fclose(fp);
	}

	free((PVOID)szLog);
	}
}


//
// Function name	: SplitString
// Description	   : Split a delimited string to an array of pointers to strings
// Return type		: int number of elements returned
//
//----------------------------------------------------------------------------//
// Takes a string and splits it up creating an array of pointers to the start //
// of the sections delimted by the array of specified char delimiters         //
// the delimter characters in "string" are overwritten with NULLs             //
// Usage:                                                                     //
// char szString[] = "1,2|3'4,5,6,7,";                                        //
// char delim[] = {',', '|', '\'', ',', ',', ',', ',',};                      //
// UINT count=7;                                                              //
// char *pElements[7];                                                        //
// int iLen;                                                                  //
//	iLen = SplitString( szString, delim, 7, pElements);                        //
//                                                                            //
// NOTE: This funcion works differently to strtok in that consecutive         //
// delimiters are not overlooked and so a NULL output string is possible      //
// i.e. a string                                                              //
//    hello,,dave                                                             //
// where ,,, is the delimiter will produce 3 output strings the 2nd of which  //
// will be NULL                                                               //
//----------------------------------------------------------------------------//
int SplitString(char *string, char *delim, UINT count, char **outarray )
{
	UINT x;
	UINT y;
	UINT len;										// length of the input string
	static char *szNull = "";				// generic NULL output string
	int delimlen;
	int dp;

	len = strlen( string );
	delimlen = strlen( delim );

	if(!len)										// if the input string is a NULL then set all the output strings to NULL
	{
		for (x = 0 ; x < count; x++ )
			outarray[x] = szNull;
		return 0;
	}

	outarray[0] = string;					// set the 1st output string to the beginning of the string

	for( x = 0,y = 1 ; (x < len) && (y < count); x++ )
	{
		if( delimlen < 2 )
			dp = 1;
		else
			dp = y;
		if(string[x] == delim[dp-1])
		{
			string[ x ] = '\0';
			if((x+1) < len)					// if there is another string following this one....
			{
				outarray[ y ] = &string[x+1];
				y++;								// increment the number of assigned buffer
			}
			else
			{
				outarray[ y ] = szNull;
			}
		}
	}

	x = y;
	while( x < count )
		outarray[x++] = szNull;
	return y;									// return the number of strings allocated
}
