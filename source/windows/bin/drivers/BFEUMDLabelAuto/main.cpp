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


/************************************
REVISION LOG ENTRY
Revision By: PaulW
Revised on 20/09/2006 12:12:12
Version: V4.2
Comments: r_p and w_p -- will look first for 4.5 CC_ROOT and SYSTEM,
then  ConfigPath defined for V3 systems
or as a last resort the default Windows/Winnt directory for dev ini files etc
************************************/

/************************************
REVISION LOG ENTRY
Revision By: SteveL
Revised on 19/04/2016 12:12:12
Version: V1.2.4.0
Comments:	Now obtains instance information from object_settings.xml
			object prefix is in device .ini file as before...
				[BFEUMDLabelAuto]
				WhichStudio=<prefix_name>
			object name is <prefix_name>_bfeumdlabel_auto

			example settings:
				<object id="studio_a_bfeumdlabel_auto">
					<setting id="router" value="rtr_sta_sdi"/>
					<setting id="sncs_interface" value="sncs_interface"/>
					<setting id="bfeksc_driver" value="studio_a_bfeksc_driver"/>
					<setting id="sources_external" value="11,12,13,14,15,16,17,18,21,22,23,24,25,26,27,28,41,42,43,44"/>
					<setting id="sncs_infodriver_slots" value="2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40"/>
					<setting id="sources_cameras" value="1,2,3,4,5,6,7,8,9,10"/>
					<setting id="sources_looped" value="61,62,63,64,65,68,69,70,71,72,95,96,97,98,99,100,101,102,103,104,110"/>
				</object>
************************************/


#include "stdafx.h"
#define EXT
#include "BFEUMDLabelAuto.h"

//
//  FUNCTION: WinMain()
//
//  PURPOSE: 
//
//  COMMENTS: main entry point for BNCS Driver "BFEUMDLabelAuto"
//
//
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// exception install
	structured_exception::install();
	try
	{

	MSG msg;
	HACCEL hAccelTable;
	char acTemp[256];
	HANDLE hMut=NULL;

	lstrcpyn(acTemp,lpCmdLine,255);
	iDevice=atoi(acTemp);
		// Initialize global strings
		LoadString(hInstance, IDS_SECTION, szAppName, MAX_LOADSTRING);
		LoadString(hInstance, IDS_APP_TITLE, acTemp, MAX_LOADSTRING);
		wsprintf(szTitle,acTemp,iDevice);
		LoadString(hInstance, IDC_APPCLASS, szWindowClass, MAX_LOADSTRING);

		//check to see if an instance is already running
		hMut=CreateMutex(NULL,TRUE,szTitle);
		if (GetLastError()==ERROR_ALREADY_EXISTS)
			return -1; // terminate if it is

		MyRegisterClass(hInstance);

		// Perform application initialization:
		if (!InitInstance (hInstance, nCmdShow)) 
		{
			return FALSE;
		}

		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_BFEUMDLABELAUTO);
		
		// Main message loop:
		while (GetMessage(&msg, NULL, 0, 0)) 
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

	CloseHandle(hMut); // destroy mutex to allow subsequent instance to run
	return msg.wParam;

	}
	catch(structured_exception const & e)
	{
		Log("%s:WinMain() - thrown exception %x from %x", szAppName, e.what(), e.where());
		exit(1);
	}
	catch(...)
	{
		Log("%s:WinMain() - Unknown exception ", szAppName);
		exit(1);
	}

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
	wcex.lpszMenuName	= (LPCSTR)IDC_BFEUMDLABELAUTO;
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

	hWndMain = CreateWindow(szWindowClass, szTitle, WS_SYSMENU|WS_MINIMIZEBOX,
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
	try
	{

	RECT rctWindow;
	int wmId, wmEvent, iNewDate;
	char szFileName[256];
	char szSection [MAX_LOADSTRING];
	LoadString(hInst, IDS_SECTION, szSection, MAX_LOADSTRING);
	char *currentTime;
	time_t now;
	struct tm *ptrTime;
	
	switch (message) 
	{
	case WM_CREATE:
		return InitApp(hWnd);
		
		case WM_TIMER:
				//	Debug( "MGN timer vent %d " , wParam);

			switch ( wParam ) {

				case ONE_SECOND_TIMER_ID : // used for housekeeping and cyclic polling of hardware 
				//	Debug( "MGN - one second timer " );
					if (bAutomaticStarting == TRUE) {
						iProcessRevsCounter--;
						if (iProcessRevsCounter<1) {
							bAutomaticStarting = FALSE;
							// now do a thorough umd calc 
							for (int iD=1;iD<=routerData->getMaximumDestinationsUsed();iD++) {
								ProcessRouterRevertive( iD, routerData->getRoutedSourceForDestination(iD) ); 
							}
							ProcessAllReEnterantDestinations( ); // needs to happen...
							//
							// now sort dates out 
							//
							time( &now );
							ptrTime=localtime(&now);
							currentTime = asctime( ptrTime );
							//Debug( currentTime );
							char buf1[10], buf2[10];
							strftime( buf1, 10, "%Y%m%d", ptrTime );  buf1[8]=0;
							strftime( buf2, 10, "%H%M", ptrTime ); buf2[4]=0;
							iNewDate = atoi( buf1 );
							iCurrentDate = iNewDate;
							iCurrentTime = atoi( buf2 );
							Debug( "setting date for first time %d time %d ", iNewDate, iCurrentTime );
							SetDlgItemText(hWndDlg , IDC_AUTO_STATUS , "Running OK");

							CheckAndSetInfoResilience();
							Restart_CSI_NI_NO_Messages();
							iNextResilienceProcessing = 11;
						}
					} 
					else {
						// always call this -- if no commands in buffer then nothing to do
						ProcessNextCommand(); // to get the next command out to device
						// 
						if (bResolvingNewSNCSLabels==TRUE) {
							ResolveSNCSNewLabels( iResolvingNewSNCSIndex );
							iResolvingNewSNCSIndex ++;
							if (iResolvingNewSNCSIndex>=allLabelsDatabase->getMaxSncsLabelsInUse()) {
								iResolvingNewSNCSIndex = 0;
								bResolvingNewSNCSLabels = FALSE; 
								// stop resolving now until tomorrow
							}
						}

						// 1. check on infodriver status
						iNextResilienceProcessing--;
						if (iNextResilienceProcessing <= 0) {
							CheckAndSetInfoResilience();
							if (iOverallModeChangedOver == 0) Restart_CSI_NI_NO_Messages();
							iNextResilienceProcessing = 41; // prime number
						}

						// 2. check if recently failed over
						if (iOverallModeChangedOver>0) {
							iOverallModeChangedOver--;
							if (iOverallModeChangedOver <= 0)
								Restart_CSI_NI_NO_Messages();
						}
						//
					} // else
				break;

				case COMMAND_TIMER_ID:
					// timer gone off so end it and send next entry in fifo buffer
					KillTimer(hWnd, COMMAND_TIMER_ID);
					bNextCommandTimerRunning = FALSE;
					// process next command if any
					ProcessNextCommand();
				break;			
			
				case UMDS_TIMER_ID:
					// timer gone off so send all umds to bfe driver
					Debug( " UMDS timer executed " );
					KillTimer(hWnd, UMDS_TIMER_ID);
					SendAllUMDNamesToBFEDriverOnce();
				break;			

				case CLOCK_TIMER_ID:
						// get date and time 
						time( &now );
						ptrTime=localtime(&now);
						currentTime = asctime( ptrTime );
						//Debug( currentTime );
						char buf1[10], buf2[10];
						strftime( buf1, 10, "%Y%m%d", ptrTime );  buf1[8]=0;
						strftime( buf2, 10, "%H%M", ptrTime ); buf2[4]=0;
						iNewDate = atoi( buf1 );
						iCurrentTime = atoi( buf2 );

					   // check for NEW lables - only at midnight - when date changes 
						if (iNewDate!=iCurrentDate) {
							Debug( "date %d time %d ", iNewDate, iCurrentTime );
							Debug( "Clock timer - About to Purge SNCS Labels " );
							bResolvingNewSNCSLabels =TRUE;
							iResolvingNewSNCSIndex = 1;
							iCurrentDate=iNewDate;
						}
				break;

				case FORCETXRX_TIMER_ID:
					Debug("ForceTxrx timer");
					KillTimer(hWndMain, FORCETXRX_TIMER_ID);
					ForceAutoIntoTXRXMode();
				break;

			} // switch WParam

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
			if (fDebug==TRUE)
			{
				CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_UNCHECKED);
				sprintf(szFileName, "dev_%03d.ini", iDevice);
				w_p(szFileName, szSection, "DebugMode", "0");
				fDebug=FALSE;
			} 
			else
			{
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
			if (fDebugHide==TRUE)
			{
				MoveWindow(hWnd, rctWindow.left, rctWindow.top, APP_WIDTH, iChildHeight + 42, TRUE);
				CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_HIDE,MF_CHECKED);
				fDebugHide=FALSE;
				
				CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_UNCHECKED);
				sprintf(szFileName, "dev_%03d.ini", iDevice);
				w_p(szFileName, szSection, "DebugMode", "0");
				fDebug=FALSE;
			} 
			else
			{
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

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		} // switch (wmId)
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
	catch(structured_exception const & e)
	{
		Log("%s:WndProc() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:WndProc() - Unknown exception ", szAppName );
		exit(1);
	}

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
	try
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
	catch(structured_exception const & e)
	{
		Log("%s:DlgProcChild() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:DlgProcChild() - Unknown exception ", szAppName );
		exit(1);
	}


}

//
//  FUNCTION: Version Info()
//
void VersionInfo() {
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
	if (dwSize) {
		/* get country translation */
		VerQueryValue((LPVOID)abData, "\\VarFileInfo\\Translation", (LPVOID*)&lpBuffer, (UINT *) &dwSize);
		/* make country code */
		wsprintf(acBuf,"%04X%04X",*(WORD*)lpBuffer,*(WORD*)(lpBuffer+2));
		if (dwSize!=0) {
			/* get a versioninfo file version number */
			wsprintf(szName,"\\StringFileInfo\\%s\\PRODUCTVERSION",(LPSTR)acBuf);
			VerQueryValue((LPVOID)abData, (LPSTR) szName, (LPVOID*)&lpBuffer,(UINT *) &dwSize);
		}
		/* copy version number from byte buffer to a string buffer */
		lstrcpyn(szModFileName,(LPSTR)lpBuffer,(int)dwSize);
		/* copy to the dialog static text box */
		wsprintf(szBuf,"%s",szModFileName);
		// replace any commas with full stops
		for (int c=0; c<strlen(szBuf); c++)
			if (szBuf[c]==44 ) szBuf[c]=46; 
		SetDlgItemText(hWndDlg, IDC_VERSION, szBuf);
	}			
	free((PVOID)abData);
	free((PVOID)szName);
	free((PVOID)szModFileName);
	free((PVOID)szBuf);
}


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
	try
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
				LPSTR szFileVersion=(LPSTR)alloca(PATH_LENGTH);
				LPSTR szProductVersion=(LPSTR)alloca(PATH_LENGTH);

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
				lstrcpyn(szFileVersion,(LPSTR)lpBuffer,(int)dwSize);

				if (dwSize!=0)
				{
					wsprintf(szName,"\\StringFileInfo\\%s\\ProductVersion",(LPSTR)acBuf);
					VerQueryValue((LPVOID)abData, (LPSTR) szName, 
						(LPVOID*)&lpBuffer,(UINT *) &dwSize);
				}
				
				/* copy version number from byte buffer to a string buffer */
				lstrcpyn(szProductVersion,(LPSTR)lpBuffer,(int)dwSize);
				
				/* copy to the dialog static text box */
				wsprintf(szBuf,"Version %s  (%s)",szFileVersion, szProductVersion);
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
	catch(structured_exception const & e)
	{
		Log("%s:About() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:About() - Unknown exception ", szAppName );
		exit(1);
	}


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

	try
	{

	/* return 0 if OK, or -1 if an error occurs*/
		RECT rctWindow;
		char szDebug[BUFFER_CONST];
	
		OutputDebugString( " \n BFE-UMD LABEL AUTO starting up \n " );

		// initialiseCommonControls for List boxes
		InitCommonControls();

		// Init Status Dialogs
		hWndDlg = CreateDialog(hInst , (LPCTSTR)IDD_STATUS , hWnd , (DLGPROC)DlgProcChild);
		GetClientRect(hWndDlg , &rctWindow);
		iChildHeight = rctWindow.bottom - rctWindow.top + 2;
		
		GetClientRect(hWnd,&rc);
		hWndList=CreateWindow("LISTBOX","",WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_USETABSTOPS,
							  0,iChildHeight,rc.right-rc.left,rc.bottom-rc.top-iChildHeight,
							  hWnd,(HMENU)IDW_LIST,hInst,0);
		ShowWindow(hWndList,SW_SHOW);

		bAutomaticStarting=TRUE;
		bShowAllDebugMessages = FALSE;
		bValidCollediaStatetoContinue = TRUE;
		iWhichStudio = 0;
		iResolvingNewSNCSIndex = 0;
		bResolvingNewSNCSLabels = FALSE;

		iOverallTXRXModeStatus = 0;
		iNextResilienceProcessing = 11;
		iOverallStateJustChanged = 0;

		LoadIni();   // get required parameters for comms port and infodrivers
		VersionInfo();
		OutputDebugString( " \n BFE-UMD LABEL AUTO starting A \n " );


		if (fDebug)		{
			MoveWindow(hWnd, rctWindow.left, rctWindow.top, APP_WIDTH, APP_HEIGHT, TRUE);
			CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_CHECKED);
			CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_HIDE,MF_UNCHECKED);
		}
		else	{
			MoveWindow(hWnd, rctWindow.left, rctWindow.top, APP_WIDTH, iChildHeight + 42, TRUE);
			CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_ON,MF_UNCHECKED);
			CheckMenuItem(GetSubMenu(GetMenu(hWnd),1),ID_DEBUG_HIDE,MF_CHECKED);
		}
		SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Initialising");

		// connect to all the infodrivers
		ecBFEClient = new extclient;
		// connect to all the infodrivers
		eiBFEInfoId = new extinfo;

		// declare instances of client class and ext inf
		// instantiate an instance of fifi buffer  -- yeah very fluffy 
		fifoBuffer = new CFiFoBuffer();

		//instantiate an instance of all UMD data classes -- more curly and crunchy than fluffy
		sncsData = new CRevsSNCSData();
		routerData = new CRevsRouterData();
		kscUmdData = new CKSCLabelData();	

		// instantiate the main database
		allLabelsDatabase = new CBFEUMDLabelDatabase;

		OutputDebugString( " \n BFE-UMD LABEL AUTO starting B \n " );
		// connect to CSI first
		ecBFEClient->notify(CSIClientNotify);
		switch (ecBFEClient->connect()){
		case CONNECTED: 
			Debug("Connected OK to CSI");
			SetDlgItemText(hWndDlg, IDC_CSI_STATUS, "Connected to CSI");
			break;

		case CANT_FIND_CSI:
			Debug("ERROR ERROR - Connect failed to CSI ");
			Debug("ERROR ERROR -  Automatic not functioning");
			SetDlgItemText(hWndDlg, IDC_CSI_STATUS, "CSI connect FAILED");
			SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "ERROR - in FAIL state");
			bValidCollediaStatetoContinue	= FALSE;
			break;

		default:
			Debug("Other Error - code %d",ecBFEClient->getstate());
			Debug("ERROR ERROR - Connect failed to CSI,  code :%d",ecBFEClient->getstate() );
			Debug("ERROR ERROR -  Automatic not functioning");
			SetDlgItemText(hWndDlg, IDC_CSI_STATUS, "CSI connect FAILED");
			SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "ERROR - in FAIL state");
			bValidCollediaStatetoContinue	= FALSE;
		} // switch ec
		ecBFEClient->setcounters(&lTXDEV, &lRXDEV);

		if (bValidCollediaStatetoContinue==TRUE ) {
			// connect to External Infodriver
			iOverallTXRXModeStatus = 0; // unknown first time
			eiBFEInfoId->notify(InfoNotify);
			switch (eiBFEInfoId->connect(iDevice)) { // connect to infodriver
			case CONNECTED: 
				Debug("Connected OK to infodriver %d",eiBFEInfoId->iDevice);
				SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "Connected OK");
				break;	
			case CANT_FIND_INFODRIVER:
				Debug("ERROR ERROR - Connect failed to infodriver %d",eiBFEInfoId->iDevice);
				Debug("ERROR ERROR -  Automatic not functioning");
				SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "Info connect FAILED");
				SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "ERROR - in FAIL state");
				bValidCollediaStatetoContinue	= FALSE;
				break;
			default:
				Debug("ERROR ERROR - Connect failed to infodriver, code %d",eiBFEInfoId->connect(iDevice));
				Debug("ERROR ERROR -  Automatic not functioning");
				SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "Info connect FAILED");
				SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "ERROR - in FAIL state");
				bValidCollediaStatetoContinue	= FALSE;
			}
			eiBFEInfoId->setcounters(&lTXID, &lRXID);
		}
		else 
			return 0;

		if (bValidCollediaStatetoContinue==TRUE ) {
			// continue on to initialise vars, get object settings, dev registrations and poll for revs 
			InitStatusDialog();

			InitializeMemoryStructures();  // init global structures of data storage 
			OutputDebugString( " \n BFE-UMD LABEL AUTO starting C \n " );

			// get required fields from instances and object settings, database files from disc
			LoadAllInitialisationData();

			CheckAndSetInfoResilience();
			iNextResilienceProcessing = 11;
			if (iOverallTXRXModeStatus == INFO_RXMODE) {
				// try and force here once 
				eiBFEInfoId->setmode(INFO_TXRXMODE);
				eiBFEInfoId->requestmode = TO_TXRX;
			}

			// load from db files the current list of all alias data
			int iCount =0;
			iCount = LoadDB2AliasData();
			wsprintf( szDebug, "%d", iCount);
			SetDlgItemText(hWndDlg, IDC_ALIAS_SNCS, szDebug);
			allLabelsDatabase->setMaxSncsLabelsInUse(iCount);
			iCount = LoadDB3AliasData();
			wsprintf( szDebug, "%d", iCount);
			SetDlgItemText(hWndDlg, IDC_ALIAS_ROUTER, szDebug);
			allLabelsDatabase->setMaxRouterLabelsInUse(iCount);
			//OutputDebugString( " \n BFE-UMD LABEL AUTO starting Ca \n " );

			// load db2 data from router dev ini file
			LoadRouterDB2SoftNames();
			//OutputDebugString( " \n BFE-UMD LABEL AUTO starting Cb \n " );

			// set out the basics for the listviews
			DisplayAllAliasDataListview();
			//OutputDebugString( " \n BFE-UMD LABEL AUTO starting Cc \n " );

			
			SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Registering..." );
			// register with router etc
			AutomaticRegistrations();
			//OutputDebugString( " \n BFE-UMD LABEL AUTO starting Cd \n " );

			SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Polling..." );
			// poll routers etc and get revertives - 
			PollAutomaticDrivers();
			//OutputDebugString( " \n BFE-UMD LABEL AUTO starting C e\n " );

			// set timer for device polls - to check all revertives returned
			iOneSecondPollTimerId = SetTimer( hWnd, ONE_SECOND_TIMER_ID,ONE_SECOND_TIMER_INTERVAL , NULL );
			bOneSecondPollTimerRunning = TRUE;
			iProcessRevsCounter = 2;
			Debug( "One Second  timer start returned %d ", iOneSecondPollTimerId );

			iClockTimerId = SetTimer( hWnd, CLOCK_TIMER_ID, CLOCK_TIMER_INTERVAL, NULL);
			Debug( "Clock timer start returned %d ", iClockTimerId );

			iUMDTimerId = SetTimer( hWnd, UMDS_TIMER_ID, UMDS_TIMER_INTERVAL, NULL);
			Debug( "UMDs timer start returned %d ", iUMDTimerId );

			SetDlgItemText(hWndDlg, IDC_AUTO_STATUS, "Awaiting Rtr Revs");
			eiBFEInfoId->updateslot( COMM_STATUS_SLOT, "1" );	 // ok	

			// end of initialisation - driver running normally now on
			OutputDebugString( " \n BFE-UMD LABEL AUTO starting D \n " );

		}
		
		return 0;

	}
	catch(structured_exception const & e)
	{
		Log("%s:InitApp() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:InitApp() - Unknown exception ", szAppName );
		exit(1);
	}


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
	eiBFEInfoId->updateslot( COMM_STATUS_SLOT, "0" ); // currently automatic in lost fail mode
	ForceAutoIntoRXOnlyMode();
	// kill any timers
	KillTimer( hWnd , COMMAND_TIMER_ID );
	KillTimer( hWnd , ONE_SECOND_TIMER_ID );
	// delete objects
	delete ecBFEClient;
	delete fifoBuffer;
	delete sncsData;  
	delete routerData;
	delete allLabelsDatabase; 
	delete kscUmdData;
	delete eiBFEInfoId;
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
	try
	{

	char szFileName[32];
	char szSection [MAX_LOADSTRING];

	getBNCS_File_Path();   // sets various globals for system type and path that dev ini files use
	
	iWorkstation=getWS();
	
	wsprintf(szFileName,"DEV_%03d.ini",iDevice);
	LoadString(hInst, IDS_SECTION, szSection, MAX_LOADSTRING);
	
	/* extract debug and log parameters from ini file */
	fDebug=atoi(r_p(szFileName,szSection,"DebugMode","1",TRUE));
	fDebugHide = fDebug;
	fLog=atoi(r_p(szFileName,szSection,"LogMode","0",TRUE));

	bncs_string ssStr = r_p(szFileName,szSection,"WhichStudio","ST-unknown",TRUE);
	ssWhichStudio = ssStr.lower();
//	if (ssWhichStudio == "sta") iWhichStudio = STUDIO_A;
//	if (ssWhichStudio == "stb") iWhichStudio = STUDIO_B;
//	if (ssWhichStudio == "stc") iWhichStudio = STUDIO_C;
//	if (ssWhichStudio == "std") iWhichStudio = STUDIO_D;
	// changed descriptions to include full test "studio_<n>"
	// iWhichStudio is not used in any other place apart from here
	// perhaps there was bigger plans for that global !!
	if (ssWhichStudio == "studio_a") iWhichStudio = STUDIO_A;
	if (ssWhichStudio == "studio_b") iWhichStudio = STUDIO_B;
	if (ssWhichStudio == "studio_c") iWhichStudio = STUDIO_C;
	if (ssWhichStudio == "studio_d") iWhichStudio = STUDIO_D;

	}
	catch(structured_exception const & e)
	{
		Log("%s:LoadIni() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:LoadIni() - Unknown exception ", szAppName );
		exit(1);
	}

}

/*!
\returns int The workstation number

\brief Function to get the workstation number

First checks to see if %CC_WORKSTATION% is defined
otherwise returns the WS stored in %WINDIR%\csi.ini

NOTE: 
In BNCSWizard apps the following change should be made to LoadINI()
- Replace:
	iWorkstation=atoi(r_p("CSI.INI","Network","Workstation","0",FALSE));
- With:
	iWorkstation=getWS();
- Also add the following to "app_name.h":
int		getWS(void);
*/
int getWS(void)
{
	try
	{

	char* szWS = getenv( "CC_WORKSTATION" );
	
	if(szWS)
	{
		return atoi(szWS);
	}
	else
	{
		return atoi(r_p("CSI.INI","Network","Workstation","0",FALSE));
	}

	}
	catch(structured_exception const & e)
	{
		Log("%s:getWS() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:getWS() - Unknown exception ", szAppName );
		exit(1);
	}

}


// new function added to version 4.2 assigns the globals    szBNCS_File_Path and szBNCS_Log_File_Path
// called from r_p and w_p functions immeadiately below
// could easily be called once on first start up of app

char *	getBNCS_File_Path( void  )  
{
	try
	{

	char* szCCRoot = getenv( "CC_ROOT" );
	char* szCCSystem = getenv( "CC_SYSTEM" );
	char szV3Path[MAX_BUFFER_STRING] = "";
	GetPrivateProfileString("Config", "ConfigPath", "", szV3Path, sizeof(szV3Path),"C:\\bncs_config.ini" );
	char szWinDir[MAX_BUFFER_STRING] = "";
	GetWindowsDirectory(szWinDir, MAX_BUFFER_STRING);

	// first check for 4.5 system settings
	if(szCCRoot && szCCSystem)	{
		sprintf(szBNCS_File_Path, "%s\\%s\\config\\system", szCCRoot, szCCSystem);
		sprintf(szBNCS_Log_File_Path, "%s\\%s\\logs", szCCRoot, szCCSystem);
	}
	else if (strlen(szV3Path)>0) {
		sprintf(szBNCS_File_Path,"%s",szV3Path);
		strcpy(szBNCS_Log_File_Path, "C:\\bncslogs");
	}
	else { 
		// all other BNCS / Colledia systems - get windows or winnt directory inc v3  that are not using bncs_config.ini
		sprintf(szBNCS_File_Path,"%s",szWinDir);
		strcpy(szBNCS_Log_File_Path, "C:\\bncslogs");
	}
	return szBNCS_File_Path;

	}
	catch(structured_exception const & e)
	{
		Log("%s:getBNCS_File_Path() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:getBNCS_File_Path() - Unknown exception ", szAppName );
		exit(1);
	}

}


/*!
\returns char* Pointer to data obtained
\param char* file The file name only to read from - path is added in this function according to Colledia system
\param char* section The section - [section]
\param char* entry The entry - entry=
\param char* defval The default value to return if not found
\param BOOL fWrite Flag - write the default value to the file

\brief Function to read one item of user data from an INI file
Calls 	function getBNCS_File_Path  which 
checks to see if %CC_ROOT% and %CC_SYSTEM% are defined
- If so the config files in %CC_ROOT%\%CC_SYSTEM%\config\system are used
else checks for V3 bncs_config.ini used 
- Otherwise the config files in %WINDIR% are used
*/
char* r_p(char* file, char* section, char* entry, char* defval, BOOL fWrite)
{
	try
	{

	char szFilePath[MAX_BUFFER_STRING];	
	char szPathOnly[MAX_BUFFER_STRING];

	strcpy( szPathOnly, getBNCS_File_Path() );
	if (strlen(szPathOnly)>0)
		wsprintf( szFilePath,  "%s\\%s", szPathOnly, file );
	else
		wsprintf( szFilePath,  "%s", file );


	GetPrivateProfileString(section, entry, defval, szResult, sizeof(szResult), szFilePath);
	if (fWrite)
		w_p(file,section,entry,szResult);
	
	return szResult;

	}
	catch(structured_exception const & e)
	{
		Log("%s:r_p() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:r_p() - Unknown exception ", szAppName );
		exit(1);
	}

}

/*!
\returns void 
\param char* file The file name only to read from - path is added in this function according to Colledia system
\param char* section The section - [section]
\param char* entry The entry - entry=
\param char* setting The value to write - =value

\brief Function to write one item of user data to an INI file
Calls 	function getBNCS_File_Path  which 
checks to see if %CC_ROOT% and %CC_SYSTEM% are defined
- If so the config files in %CC_ROOT%\%CC_SYSTEM%\config\system are used
else checks for V3 bncs_config.ini used 
- Otherwise the config files in %WINDIR% are used
*/
void w_p(char* file, char* section, char* entry, char* setting)
{
	try
	{

	char szFilePath[MAX_BUFFER_STRING];	
	char szPathOnly[MAX_BUFFER_STRING];

	strcpy( szPathOnly, getBNCS_File_Path() );
	if (strlen(szPathOnly)>0)
		wsprintf( szFilePath,  "%s\\%s", szPathOnly, file );
	else
		wsprintf( szFilePath,  "%s", file );

	WritePrivateProfileString(section, entry, setting, szFilePath);

	}
	catch(structured_exception const & e)
	{
		Log("%s:w_p() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:w_p() - Unknown exception ", szAppName );
		exit(1);
	}

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
void UpdateCounters(void)
{
	try
	{
	char szBuf[32];
	//Update CSI Comms Message Count
	wsprintf(szBuf, "%08d", lTXDEV);
	SetDlgItemText(hWndDlg, IDC_DEVTX1, szBuf);
	wsprintf(szBuf, "%08d", lRXDEV);
	SetDlgItemText(hWndDlg, IDC_DEVRX1, szBuf);
	//Update Infodriver External Message Count
	wsprintf(szBuf, "%08d", lTXID);
	SetDlgItemText(hWndDlg, IDC_IDTX1, szBuf);
	wsprintf(szBuf, "%08d", lRXID);
	SetDlgItemText(hWndDlg, IDC_IDRX1, szBuf);

	// counters from devices registered for
	wsprintf(szBuf, "%08d", routerData->getRevertivesCounter());
	SetDlgItemText(hWndDlg, IDC_ROUTERREVS, szBuf);
	wsprintf(szBuf, "%08d", sncsData->getRevertivesCounter());
	SetDlgItemText(hWndDlg, IDC_SNCSREVS, szBuf);
	wsprintf(szBuf, "%08d", kscUmdData->getRevertivesCounter());
	SetDlgItemText(hWndDlg, IDC_BFEDRIVERREVS, szBuf);
	wsprintf(szBuf, "%08d", allLabelsDatabase->getRevertivesCounter());
	SetDlgItemText(hWndDlg, IDC_CCPANELREVS, szBuf);


	}
	catch(structured_exception const & e)
	{
		Log("%s:UpdateCounters() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:UpdateCounters() - Unknown exception ", szAppName );
		exit(1);
	}

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
	try
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

		// version 4.2 of wizard added this - log debug message if log enabled
		if (fLog) {
			Log( szDebug );
		} 
		
		// now free memory used
		free((PVOID)szDebug);
	}

	}
	catch(structured_exception const & e)
	{
		Log("%s:Debug() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:Debug() - Unknown exception ", szAppName );
		exit(1);
	}

}

//
// FUNCTION CreateLogDirectory
//
// new function Version 4.2 of the Wizard - to create log file directory according to Colledia system type
//
BOOL CreateLogFileDirectory ( void )
{
	try
	{

	static bool bFirst = true;
	
	//create the log directory the first time
	if (bFirst) {
		_mkdir(szBNCS_Log_File_Path);	
		bFirst = false;
		return TRUE;
	}
		return FALSE;

	}
	catch(structured_exception const & e)
	{
		Log("%s:CreateLogFileDirectory() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:CreateLogFileDirectory() - Unknown exception ", szAppName );
		exit(1);
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
//		rewritten in wizard version 4.2 to log crashes too - 
//
void Log(LPCSTR szFmt, ...)
{
	char szLogFile[MAX_BUFFER_STRING];
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
		
		wsprintf(szLogFile, "%s\\%s_child2_%d.log", szBNCS_Log_File_Path, szDate, iDevice);
		
		// create log file directory if it doesn't exist
		CreateLogFileDirectory();

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
//	iLen = SplitString( szString, delim, 7, pElements);                       //
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
	try
	{

	UINT x;
	UINT y;
	UINT len;					// length of the input string
	static char *szNull = "";	// generic NULL output string
	int delimlen;
	int dp;

	len = strlen( string );
	delimlen = strlen( delim );

	if(!len)					// if the input string is a NULL then set all the output strings to NULL
	{
		for (x = 0 ; x < count; x++ )
			outarray[x] = szNull;
		return 0;
	}

	outarray[0] = string;		// set the 1st output string to the beginning of the string

	for( x = 0,y = 1 ; (x < len) && (y < count); x++ )
	{
		if( delimlen < 2 )
			dp = 1;
		else
			dp = y;
		if(string[x] == delim[dp-1])
		{
			string[ x ] = '\0';
			if((x+1) < len)		// if there is another string following this one....
			{
				outarray[ y ] = &string[x+1];
				y++;			// increment the number of assigned buffer
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
	return y;					// return the number of strings allocated


	}
	catch(structured_exception const & e)
	{
		Log("%s:SplitString() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:SplitString() - Unknown exception ", szAppName );
		exit(1);
	}

}
