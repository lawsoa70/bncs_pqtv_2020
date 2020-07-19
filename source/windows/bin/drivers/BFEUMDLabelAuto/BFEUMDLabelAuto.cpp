/* BFEUMDLabelAuto.cpp - custom program file for project BFEUMDLabelAuto */

#include "stdafx.h"
#include "BFEUMDLabelAuto.h"

/* TODO: place references to your functions and global variables in the BFEUMDLabelAuto.h file */

// local function prototypes
// this function is called when a valid slotchange notification is extracted from the InfoNotify
BOOL SlotChange(UINT iDriver,UINT iSlot, LPCSTR szSlot);
void CreateNewRowInListBox( HWND hList, int iRow, char szText[BUFFER_CONST] );
void UpdateTextItemtoRowInListBox( HWND hList, int iRow, int iColumn, char szText[BUFFER_CONST] );

void GetSourceDatabaseName( int iSource, int iWhichDB, char szName[BUFFER_CONST] );
bool HasSoftNameChanged( int iSource, char szNewName[BUFFER_CONST] );
BOOL HasUMDNameChanged( int iSource, char szNewName[BUFFER_CONST] );
void UpdateRouterDeviniDB2( int iSource, char szName[BUFFER_CONST] );
void ProcessAllReEnterantDestinations( void );

void ProcessRouterDB2NameChange( int iSource, char szName[BUFFER_CONST] );

void ProcessRouterSlotChange( int iSource, LPCSTR  szNewData );
void ProcessSNCSSlotChange( int iIndex, LPCSTR  szNewData );
void ProcessRouterNowOnSNCSChange( char SZSNCSLabel[BUFFER_CONST] );

void ProcessSourceList( bncs_string sSrcList, int iType );
void ProcessSNCSInfoList( bncs_string strValue, bncs_string strExtValue );


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//               DIALOG LIST BOX Routines
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void InitStatusDialog(){
	//NOTE - see Win32 API docs on Listview
	//http://www.codeproject.com/listctrl/listview.asp
	
	try {

	//init listview sncs
	hWndUMDData1 = GetDlgItem(hWndDlg, IDC_LISTSNCS);
	//init listview rtr
	hWndUMDData2 = GetDlgItem(hWndDlg, IDC_LISTRTR);
	
	//init listview umd labels output
	hWndDestData = GetDlgItem(hWndDlg, IDC_LISTOUT);
	
	if(hWndUMDData1&&hWndUMDData2&&hWndDestData){
		LVITEM lvi;
		LVCOLUMN lvc;

		// Here we put the info on the Column headers
		// this is not data, only name of each header we like
		memset(&lvc,0,sizeof(lvc));                  // Zero Members
		lvc.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;    // Type of mask

		lvc.cx=0x39;                                   // width of column
		lvc.pszText="# Index";                           // First Header Text
		SendMessage(hWndUMDData1, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc); // Insert/Show the column
		lvc.pszText="# Router";                           // First Header Text
		SendMessage(hWndUMDData2, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc); // Insert/Show the column
		
		lvc.cx=0x5B;                                   // width of 2nd column
		lvc.pszText="Label";                // Second Header Text
		SendMessage(hWndUMDData1, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc); // Insert/Show the column
		lvc.pszText="Type";                // Second Header Text
		lvc.cx=0x34;                                   // width of 2nd column
		SendMessage(hWndUMDData2, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc); // Insert/Show the column

		// dest and src columns in mixers to seats 
		lvc.cx=0x28;                                   // width of column
		lvc.pszText="Form";                           // 3rd Header Text
		SendMessage(hWndUMDData1, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc); // Insert/Show the column
		lvc.cx=0x26;                                   // width of column
		SendMessage(hWndUMDData2, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc); // Insert/Show the column

		lvc.cx=0x58;                                   // width of column
		lvc.pszText="Alias Name";                 // 5th Header Text
		SendMessage(hWndUMDData1, LVM_INSERTCOLUMN, 3, (LPARAM)&lvc); // Insert/Show the column
		lvc.cx=0x28;                                   // width of column
		lvc.pszText="Style";                           // 4th Header Text
		SendMessage(hWndUMDData2, LVM_INSERTCOLUMN, 3, (LPARAM)&lvc); // Insert/Show the column

		lvc.cx=0x68;                                   // width of column
		lvc.pszText="Linked to SRC No";        // 5th Header Text
		SendMessage(hWndUMDData1, LVM_INSERTCOLUMN, 4, (LPARAM)&lvc); // Insert/Show the column
		lvc.cx=0x56;                                   // width of column
		lvc.pszText="Auto Name";                 // 5th Header Text
		SendMessage(hWndUMDData2, LVM_INSERTCOLUMN, 4, (LPARAM)&lvc); // Insert/Show the column

		lvc.cx=0x56;                                   // width of column
		lvc.pszText="Manual Name";                 // 6th Header Text
		SendMessage(hWndUMDData2, LVM_INSERTCOLUMN, 5, (LPARAM)&lvc); // Insert/Show the column

		lvc.cx=0x2C;                                   // width of column
		lvc.pszText="Sncs#";                           // 7th Header Text
		SendMessage(hWndUMDData2, LVM_INSERTCOLUMN, 6, (LPARAM)&lvc); // Insert/Show the column

		lvc.cx=0x56;                                   // width of column
		lvc.pszText="Sncs Label";            // 8th Header Text
		SendMessage(hWndUMDData2, LVM_INSERTCOLUMN, 7, (LPARAM)&lvc); // Insert/Show the column


		/// Output list view
		lvc.cx=0x2A;                                   // width of column
		lvc.pszText="Dest";                          // 1th Header Text
		SendMessage(hWndDestData, LVM_INSERTCOLUMN, 0, (LPARAM)&lvc); // Insert/Show the column

		lvc.cx=0x2A;                                   // width of column
		lvc.pszText="Src";                           // 2th Header Text
		SendMessage(hWndDestData, LVM_INSERTCOLUMN, 1, (LPARAM)&lvc); // Insert/Show the column

		lvc.cx=0xA0;                                   // width of column
		lvc.pszText="UMD string";                           // 3th Header Text
		SendMessage(hWndDestData, LVM_INSERTCOLUMN, 2, (LPARAM)&lvc); // Insert/Show the column

		//Create the items
		memset(&lvi,0,sizeof(lvi)); // Zero struct's Members

	} // if ( hwnd.... )
	else {
		Debug("BFEUMDLabel_Auto:InitStatusDialog() -ERROR -  one of the listview handles failed ");
	}

	}
	catch(structured_exception const & e)
	{
		Log("BFEUMDLabel_Auto:InitStatusDialog() - thrown exception %x from %x", e.what(), e.where());
		exit(1);
	}
	catch(...)
	{
		Log("BFEUMDLabel_Auto:InitStatusDialog() - Unknown exception ");
		exit(1);
	}

}


//
// create a New row of data in a listview 
//
void CreateNewRowInListBox( HWND hList, int iRow, char szText[BUFFER_CONST] )
{

	LVITEM lvi;
	//  Setting properties Of members:
	lvi.mask = LVIF_TEXT;   // Text Style
	lvi.cchTextMax = 256; // Max size of text
	lvi.iSubItem = 0 ;          // Put in first column		
	lvi.pszText = szText; // Text to display (can be from a char variable) (Items)	
	lvi.iItem = iRow-1;          // choose item  

	if (hList) {
		// put in listview
		if (iRow>=0) {		
			SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&lvi); // Send info to the Listview
		}
		else
			Debug( "CreateNewRowInListBox - invalid row " );
	}
	else
		Debug( "CreateNewRowInListBox - invalid  handle" );
		

}

//
// update text data in a listview 
//
void UpdateTextItemtoRowInListBox( HWND hList, int iRow, int iColumn, char szText[BUFFER_CONST] ) 
{
	//  Setting properties Of members:
	LVITEM lvi;
	lvi.mask = LVIF_TEXT;   // Text Style
	lvi.cchTextMax = 256; // Max size of text

	if (hList) {
		// put in listview
		if ((iRow>0)&&(iColumn>0)) {
			ListView_SetItemText(hList, iRow-1, iColumn-1, szText );   // handle, row, column, text 
		}
		else
			Debug( "UpdateTextItemtoRowInListBox - invalid row or column " );
	}
	else
		Debug( "UpdateTextItemtoRowInListBox - invalid  handle" );
}


//
// Put all alias data into listview -- first time -- so setting up all 3 listviews
//
void DisplayAllAliasDataListview(void) 
{
	int iIndex, iMax, iStyle, iForm, iSrcNum, iDbEntry, iListViewIndex, iSncsIndex;
	char szAlias[BUFFER_CONST], szAuto[BUFFER_CONST], szMan[BUFFER_CONST], szTmp[BUFFER_CONST] ;
	char szSncsLabel[BUFFER_CONST];
	bncs_stringlist ssl;

	// SNCS listview
	if(hWndUMDData1){
		iMax=allLabelsDatabase->getMaxSncsLabelsInUse();
		for (iIndex=1; iIndex<=iMax;iIndex++) {
			allLabelsDatabase->getAllSncsLabelData(iIndex,szAlias,szAuto,szMan,&iStyle,&iForm,&ssl,&iDbEntry,&iListViewIndex );
			itoa( iIndex, szTmp, 10);
			char *pdest = strstr(szAlias, "$NAME");
			if (pdest==NULL ) { // valid set of data 
				CreateNewRowInListBox(hWndUMDData1, iIndex, szTmp );  // first column
				UpdateTextItemtoRowInListBox(hWndUMDData1, iIndex, 2,szAlias);
				if (iForm==ePermamentForm)
					UpdateTextItemtoRowInListBox(hWndUMDData1, iIndex, 3, "perm" );
				else 
					UpdateTextItemtoRowInListBox(hWndUMDData1, iIndex, 3, "temp" );	
				UpdateTextItemtoRowInListBox(hWndUMDData1, iIndex, 4, szAuto );
			}
			else { // blank set of data
				CreateNewRowInListBox(hWndUMDData1, iIndex, szTmp );  // first column
				UpdateTextItemtoRowInListBox(hWndUMDData1, iIndex, 2, "- - -" );
			}
		}
	}

	// RTR listview
	if(hWndUMDData2){
		iMax=allLabelsDatabase->getMaxRouterLabelsInUse();
		for (iIndex=1; iIndex<=iMax;iIndex++) {
			allLabelsDatabase->getAllRouterLabelData(iIndex,&iSrcNum,szAuto,szMan,&iStyle,&iForm,szSncsLabel,&iDbEntry,&iListViewIndex );
			wsprintf( szAlias, "%04d", iSrcNum );
			CreateNewRowInListBox(hWndUMDData2, iIndex, szAlias );  // first column
			// get type from routerclass
			eAliasSourceType eSrc = routerData->getSourceType(iSrcNum);
			switch(eSrc){
			case eLoopedType:UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 2, "Looped" );break;
			case eCameraType:UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 2, "Camera" );break;
			case eExternalType:UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 2, "External" );break;
			case eLocalType:UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 2, "Local" );break;
			default:
				UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 2, "Local" );
			}
			
			if (iForm==ePermamentForm)
				UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 3, "perm" );
			else 
				UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 3, "temp" );	

			if (iStyle==eManualStyle)
				UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 4, "manl" );
			else
				UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 4, " auto" );	

			UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 5, szAuto );
			UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 6, szMan );

			iSncsIndex=0;
			strcpy(szTmp, " ");
			allLabelsDatabase->getAllSncsLabelData(szSncsLabel, &iSncsIndex, szAuto,szMan,&iStyle,&iForm,&ssl,&iDbEntry,&iListViewIndex);
			if (iSncsIndex>0)
				wsprintf( szTmp, "%03d", iSncsIndex);
			UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 7, szTmp );

			strcpy( szTmp, " ");
			UpdateTextItemtoRowInListBox(hWndUMDData2, iIndex, 8, szSncsLabel );
			
		}
	}

	// Dest output set up listview 
	int iDests = routerData->getMaximumDestinationsUsed();
	for (iIndex=1;iIndex<=iDests;iIndex++) {
		wsprintf( szAlias, "%04d", iIndex );
		CreateNewRowInListBox(hWndDestData, iIndex, szAlias );  // first column
	}

}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  PROCESS FIFO BUFFER FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//
//  FUNCTION: UpdateFifoBufferCountDialog
//
void UpdateFifoBufferCountDialog( void ) {
	
	char szStr[BUFFER_CONST];
	int iE = fifoBuffer->Number_Entries_in_FiFoBuffer();
	itoa( iE, szStr, 10);
	SetDlgItemText( hWndDlg , IDC_ID_FIFO , szStr);

}



void ProcessNextCommand( void ) {
	
	char szCommand[BUFFER_CONST]="A1234";
	int iType, iCCDevice;

	int iE = fifoBuffer->Number_Entries_in_FiFoBuffer();
	if (iE>0) {
		// something in buffer so process it
		int iCommandLength = fifoBuffer->Retrieve_from_FifoBuffer( szCommand, &iType, &iCCDevice );
		// get command type  -- if replying to device about a route -- driver will not get a response
		// according to command type -- most will be client commands -- for a specific device ??
	}
	UpdateFifoBufferCountDialog();
}







//////////////////////////////////////////////////////////////////////////
// 
//  AUTOMATIC STARTUP FUNCTIONS
//
//////////////////////////////////////////////////////////////////////////



void InitializeMemoryStructures(void)
{
	
}


//
//  FUNCTION: ProcessSourceList()
//
//  PURPOSE: Function called on startup set up which router source is which type -- local, camera, looped, external
//
void ProcessSourceList( bncs_string sSrcList, int iType )
{
	bncs_stringlist ssl = bncs_stringlist("").fromString(sSrcList);
	if (ssl.count()==0)
		Debug("ProcessSourceList -- warning -- count of data is 0 for type %d", iType);
	for (int i=0;i<ssl.count();i++) {
		switch(iType) {
			case eExternalType: 
				routerData->setSourceData(ssl[i], eExternalType, -1);
			break;
			case eLocalType:
				routerData->setSourceData(ssl[i], eLocalType, -1);
			break;
			case eCameraType:
				routerData->setSourceData(ssl[i], eCameraType, -1);
			break;
			case eLoopedType:
				routerData->setSourceData(ssl[i], eLoopedType, ssl[i] ); // currently a 1:1 mapping of reenterant dests to src
				routerData->setDestinationReEnterantValue(ssl[i], ssl[i] );
			break;
		default:
			routerData->setSourceData(ssl[i], ePlainType, -1);
		}
	} // for
}



//
//  FUNCTION: ProcessSNCSInfoList()
//
//  PURPOSE: Function called on startup to assign infodriver slots to router external sources
//
void ProcessSNCSInfoList( bncs_string strInfo, bncs_string strExtSrc ) 
{
	bncs_stringlist sslinfo = bncs_stringlist("").fromString(strInfo);
	bncs_stringlist sslextsrc = bncs_stringlist("").fromString(strExtSrc);
	int iInfo, iSrc;

	if (sslinfo.count()==0)
		Debug("ProcessSNCSInfoList -- ERROR -- count of data is 0 for SNCS Info data");
	if (sslextsrc.count()==0)
		Debug("ProcessSNCSInfoList -- ERROR -- count of data is 0 for external src data");

	for (int i=0;i<sslinfo.count();i++) {
		iInfo = sslinfo[i];
		if (i<sslextsrc.count()) {
			iSrc = sslextsrc[i];
		}
		else {
			iSrc=-1;
			Debug("ProcessSNCSInfoList -- warning -- SNCS info slot %d has no matching ext src ", iInfo );
		}
		sncsData->setSNCSInfodriverSlot(iInfo, iInfo); // currently 1:1 mapping between index and slot
		sncsData->setAssociatedExternalSource(iInfo, iSrc);
		if (iSrc>0) {
			// store info number for slot in rtr src data
			routerData->setSNCSInfodriverSlot(iSrc, iInfo);
		}
	} // for

}

void ResolveSNCSNewLabels( int iIndex ) 
{
	char  szLabel[BUFFER_CONST]=" ", szAlias[BUFFER_CONST]=" ", szManName[BUFFER_CONST]=" ", szCmd[BUFFER_CONST];
	bncs_stringlist ssl;
	bncs_string szVal;
	int iStyle=0, iForm=0, iDb=0, iList=-1;
	// clear any NEW sncs labels to keep table from filling up -- hopefully

	// get sncs data
	if (allLabelsDatabase->getAllSncsLabelData(iIndex, szLabel, szAlias, szManName, &iStyle, &iForm, &ssl, &iDb, &iList)) {
		szVal = szManName;
		if (szVal=="NEW") {
			// build new string 
			wsprintf( szCmd, "~<%s>~<%s>~<%s>~<AUTO>~<TEMP>~< >", szLabel, szAlias, szAlias);
			ProcessSNCSSlotChange(iIndex+STARTSNCSSLOTS, szCmd );
			char szdeb[BUFFER_CONST];
			wsprintf ( szdeb, "\n Resolve sncs lables index %d \n", iIndex );
			OutputDebugString( szdeb );
		}
	}
}


//
//  FUNCTION: LoadAllInitialisationData()
//
//  PURPOSE: Function called on startup to load data from ini files / object settings etc relevant for driver
//
void LoadAllInitialisationData( void ) {

	char szDebug[BUFFER_CONST];
	char szFileName[64], szEntry[12];
	bncs_string ssObjectSettingMain = ssWhichStudio + "_bfeumdlabel_auto";
	bncs_string strValue = "", strExtValue = "";
	strcpy(szEntry, (ssWhichStudio.upper()) );
	SetDlgItemText(hWndDlg, IDC_ID_STUDIO, szEntry );	
	
	// get instances for sncs infodriver
	//strValue = ssWhichStudio + "sncs_interface"; // if there are multpile sncs devices
	//strValue = "sncs_interface";
	strValue = getObjectSetting(ssObjectSettingMain, "sncs_interface");
	if (!strValue.length()) strValue = ssWhichStudio + "sncs_interface";	// set default so we at least have a value
	sncsData->setDeviceNumber(getInstanceDevice(strValue));
	strcpy(szDebug, strValue);
	Debug("LoadAllInitialisationData - SNCS info <%s> is device %d ", szDebug, sncsData->getDeviceNumber());
	itoa( sncsData->getDeviceNumber(),szEntry, 10 ); 
	SetDlgItemText(hWndDlg, IDC_ID_SNCS, szEntry );
	sncsData->setMaximumSlotsUsed(200); // HOW to get data on this if more than this to be used ???
	
	// get instances for main router
	//strValue = ssWhichStudio + "_sdi";
	strValue = getObjectSetting(ssObjectSettingMain, "router");
	if (!strValue.length()) strValue = ssWhichStudio + "_sdi";	// set default so we at least have a value
	routerData->setRouterNumber(getInstanceDevice(strValue));
	strcpy(szDebug, strValue);
	Debug("LoadAllInitialisationData - Router <%s> is device %d ", szDebug, routerData->getRouterNumber());
	itoa( routerData->getRouterNumber(),szEntry, 10 ); 
	SetDlgItemText(hWndDlg, IDC_ID_ROUTER, szEntry );	
	
	// get router size
	sprintf(szFileName, "dev_%03d.ini", routerData->getRouterNumber());
	strcpy( szEntry, r_p(szFileName, "Database", "DatabaseSize_0", "", false) );
	SetDlgItemText(hWndDlg, IDC_ID_ROUTERSRCS, szEntry );
	int iSrc = atoi( szEntry );
	strcpy( szEntry, r_p(szFileName, "Database", "DatabaseSize_1", "", false) );
	SetDlgItemText(hWndDlg, IDC_ID_ROUTERDESTS, szEntry );
	int iDest = atoi( szEntry );
	routerData->setRouterSize(iSrc, iDest);
	Debug( "LoadAllInitialisationData - Router size is %d %d ",  iSrc, iDest );
	
	// get instance for bfe driver
	//strValue = ssWhichStudio + "_bfeksc_driver";
	strValue = getObjectSetting(ssObjectSettingMain, "bfeksc_driver");
	if (!strValue.length()) strValue = ssWhichStudio + "_bfeksc_driver";	// set default so we at least have a value
	kscUmdData->setKSCDriverNumber(getInstanceDevice(strValue));
	strcpy(szDebug, strValue);
	Debug("LoadAllInitialisationData - BFE KSC <%s> is device %d ", szDebug, kscUmdData->getKSCDriverNumber());
	itoa( kscUmdData->getKSCDriverNumber(),szEntry, 10 ); 
	SetDlgItemText(hWndDlg, IDC_ID_BFE, szEntry );	


	// object settings required -- lists of sources for external, looped, local or other
	strValue = getObjectSetting( ssObjectSettingMain, "sources_cameras" );
	strcpy(szDebug, strValue);
	Debug( "LoadAllInitialisationData - Cameras source list %s ", szDebug );
	ProcessSourceList( strValue, eCameraType );

	strValue = getObjectSetting( ssObjectSettingMain, "sources_looped" );
	strcpy(szDebug, strValue);
	Debug( "LoadAllInitialisationData - Looped source list %s ", szDebug );
	ProcessSourceList( strValue, eLoopedType );

	strExtValue = getObjectSetting( ssObjectSettingMain, "sources_external" );
	strcpy(szDebug, strExtValue);
	Debug( "LoadAllInitialisationData - External source list %s ", szDebug );
	ProcessSourceList( strExtValue, eExternalType );

	// sncs list of info slots -- one per external src at best
	strValue = getObjectSetting( ssObjectSettingMain, "sncs_infodriver_slots" );
	strcpy(szDebug, strValue);
	Debug( "LoadAllInitialisationData - SNCS infoslot list %s ", szDebug );
	ProcessSNCSInfoList( strValue, strExtValue );
}



//  FUNCTION: LoadDB2AliasData()
//
//  PURPOSE:load db2 data -- deemed to definitive start point for labels from SNCS
//  from these lists of all aliases will be determined and wriiten out to infodriver slots
//
//  COMMENTS: data has format "~<$alias>~< auto text >~< manual name >~<AUTO>~<TEMP> where $alias is SNCS name
//			data parsed and stored in 
int 	LoadDB2AliasData( void )
{

	try
	{

	char szFileName[64], szDbDefaultFileName[64], szActualDBFileName[64];
	char  szData[BUFFER_CONST], szEntry[12];
	int   iNumUMDs=0;

	Debug( "into LoadDB2AliasData " );
	// THIS AUTO DEVICE file names -- need to read dev ini file for exact location of db2 and db3 file 
	//sprintf(szFileName, "dev_%03d.ini", iDevice);
	//sprintf(szDbDefaultFileName, "dev_%03d.db2", iDevice);

	// GLOBAL SNCS INFO  DEVICE file names -- need to read dev ini file for exact location of db2 and db3 file 
	sprintf(szFileName, "dev_%03d.ini", sncsData->getDeviceNumber() );
	sprintf(szDbDefaultFileName, "dev_%03d.db2", sncsData->getDeviceNumber() );
	
	strcpy ( szActualDBFileName, r_p( szFileName, "[Database]",  "DatabaseFile_2", szDbDefaultFileName, FALSE ));  // get name from db first - will return szentry if not found

	for ( int i=1;i<MAXDESTINATIONS;i++) {
		// get entry from file
		wsprintf(szEntry, "%04d", i);
		strcpy( szData, r_p(szActualDBFileName, "Database_2", szEntry, "", false));
		if (strstr( szData, "~<" )) {
			// update sncs slots in ext infodriver 
			eiBFEInfoId->updateslot((STARTSNCSSLOTS+i), szData );
			// found the definitive chars for a possible alias data entry
			allLabelsDatabase->storeSncsLabelData(i, szData );
			iNumUMDs++;
		}
	}
	Debug("LoadSNCSLabelDB2AliasData -- loaded %d sncs aliases", iNumUMDs );
	return iNumUMDs;

	}
	catch(structured_exception const & e)
	{
		Log("BFEUMDLabel_Auto:LoadDB2AliasData() - thrown exception %x from %x", e.what(), e.where());
		exit(1);
	}
	catch(...)
	{
		Log("BFEUMDLabel_Auto:LoadDB2AliasData() - Unknown exception ");
		exit(1);
	}


}


//  FUNCTION: LoadDB3AliasData()
//
//  PURPOSE:load db3 data -- deemed to definitive start point for router labels
//  from these lists of all aliases will be determined and wriiten out to infodriver slots
//
//  COMMENTS: data has format "~<#xxxx>~< auto text >~< manual name >~<AUTO>~<TEMP>  where xxxx is source number
//			
int 	LoadDB3AliasData( void )
{

	try
	{

	char szFileName[64], szDbDefaultFileName[64], szActualDBFileName[64];
	char szData[BUFFER_CONST], szEntry[12];
	int iNumUMDs=0;

	Debug( "into LoadDB3AliasData " );
	// file names -- need to read dev ini file for exact location of db2 and db3 file 
	sprintf(szFileName, "dev_%03d.ini", iDevice);
	sprintf(szDbDefaultFileName, "dev_%03d.db3", iDevice);
	strcpy ( szActualDBFileName, r_p( szFileName, "[Database]",  "DatabaseFile_3", szDbDefaultFileName, FALSE ));  // get name from db first - will return szentry if not found

	for ( int i=1;i<MAXDESTINATIONS;i++) {
		// get entry from file
		wsprintf(szEntry, "%04d", i);
		strcpy( szData, r_p(szActualDBFileName, "Database_3", szEntry, "", false));
		if (strstr( szData, "~<" )) {
			// update router slots in ext infodriver 
			eiBFEInfoId->updateslot((STARTROUTERSLOTS+i), szData );
			// found the definitive chars for a possible alias data entry
			allLabelsDatabase->storeRouterLabelData(i, szData );
			iNumUMDs++;
		}
	}
	Debug("LoadDB3AliasData -- loaded %d router aliases", iNumUMDs );
	return iNumUMDs;

	}
	catch(structured_exception const & e)
	{
		Log("BFEUMDLabel_Auto:LoadDB3AliasData() - thrown exception %x from %x", e.what(), e.where());
		exit(1);
	}
	catch(...)
	{
		Log("BFEUMDLabel_Auto:LoadDB3AliasData() - Unknown exception ");
		exit(1);
	}


}




//  FUNCTION: LoadRouterDB2SoftNames()
//
//  PURPOSE:load db2 data -- softnames from db2 ini file of router dev ini
//  these are the current soft names -- loaded -- to give starting point of softnames -- only updated if different on router revs
//
void LoadRouterDB2SoftNames( void )
{

	try
	{

	char szFileName[64], szDbDefaultFileName[64], szActualDBFileName[64];
	char  szData[BUFFER_CONST], szEntry[12];
	int   iNumSources=routerData->getMaximumSourcesUsed();

	Debug( "into LoadDB2AliasData " );
	// file names -- need to read dev ini file for exact location of db2 and db3 file 
	sprintf(szFileName, "dev_%03d.ini", routerData->getRouterNumber());
	sprintf(szDbDefaultFileName, "dev_%03d.db2", routerData->getRouterNumber() );
	strcpy ( szActualDBFileName, r_p( szFileName, "[Database]",  "DatabaseFile_2", szDbDefaultFileName, FALSE ));  // get name from db first - will return szentry if not found

	for ( int i=1;i<=iNumSources;i++) {
		// get entry from file
		strcpy( szData, "");
		wsprintf(szEntry, "%04d", i);
		strcpy( szData, r_p(szActualDBFileName, "Database_2", szEntry, "", false));
		routerData->setSourceSoftName(i, szData);
	}
	Debug("LoadDB2SoftNameData -- loaded %d softnames", iNumSources );

	}
	catch(structured_exception const & e)
	{
		Log("BFEUMDLabel_Auto:LoadDB2SoftNameData() - thrown exception %x from %x", e.what(), e.where());
		exit(1);
	}
	catch(...)
	{
		Log("BFEUMDLabel_Auto:LoadDB2SoftNameData() - Unknown exception ");
		exit(1);
	}


}



//  FUNCTION: AutomaticRegistrations()
//
//  PURPOSE:register with glensound drivers for incoming CLIs, and call states
//
//  COMMENTS: 
//			
BOOL AutomaticRegistrations(void)
{

	try
	{

	// go thru defined data types and register with devices - need to find out router sizes for registration
	Debug( "into AutoRegistrations " );

	// register with router	
	ecBFEClient->regtallyrange( routerData->getRouterNumber(),1, routerData->getMaximumDestinationsUsed(), INSERT ); 
	Debug( "Autoreg - registered for router dev %d  range 1 to %d ",routerData->getRouterNumber(), routerData->getMaximumDestinationsUsed() );

	// register with sncs infodriver
	for (int i=1;i<MAXDESTINATIONS;i++) {
		int is = sncsData->getSNCSInfodriverSlot(i);
		if (is>0) {
			ecBFEClient->regtallyrange( sncsData->getDeviceNumber(),is, is, INSERT ); 
			Debug( "Autoreg - registered for sncs dev %d  slot %d ",sncsData->getDeviceNumber(), is );
		}
	}

	// register with bfe driver
	ecBFEClient->regtallyrange( kscUmdData->getKSCDriverNumber(),1, MAXDESTINATIONS, INSERT ); 
	Debug( "Autoreg - registered for bfeUMD dev %d ",kscUmdData->getKSCDriverNumber() );
	return TRUE;

	}
	catch(structured_exception const & e)
	{
		Log("BFEUMDLabel_Auto:AutomaticRegistrations() - thrown exception %x from %x", e.what(), e.where());
		exit(1);
	}
	catch(...)
	{
		Log("BFEUMDLabel_Auto:AutomaticRegistrations() - Unknown exception ");
		exit(1);
	}

}



// FUNCTION PollAutomaticDrivers();
//
// to query all drivers to get this automatic up to speed - at startup
void PollAutomaticDrivers(void)
{

	try
	{
	char szCommand[BUFFER_CONST];
	Debug( "into PollAutomaticDrivers " );

	// set flag for starting up revs -- ie no mega processing  on each rev -- just store and then one processing pass  when all revs arrived
	// poll sncs  ---- this may be a lot of slots - and need to be slugged
	//wsprintf( szCommand, "IP %d %d %d",sncsData->getDeviceNumber(), 1, sncsData->getMaximumSlotsUsed() );
	//ecBFEClient->txrtrcmd( szCommand );

	// poll sncs infodriver for labels
	for (int i=1;i<MAXSNCSLABELS;i++) {
		int is = sncsData->getSNCSInfodriverSlot(i);
		if (is>0) {
			wsprintf( szCommand, "IP %d %d %d",sncsData->getDeviceNumber(), is, is );
			ecBFEClient->txrtrcmd( szCommand );
		}
	}
	Debug( "PollAutomaticDrivers - poll sncs " );
	
	// poll router ( slugged ?? )
	// poll reenterant dests first
	wsprintf( szCommand, "RP %d 61 69",routerData->getRouterNumber());
	ecBFEClient->txrtrcmd( szCommand );
	wsprintf( szCommand, "RP %d 95 110",routerData->getRouterNumber() );
	ecBFEClient->txrtrcmd( szCommand );
	// poll rest of router
	wsprintf( szCommand, "RP %d %d %d",routerData->getRouterNumber(), 1, routerData->getMaximumDestinationsUsed() );
	ecBFEClient->txrtrcmd( szCommand );
	Debug( "PollAutomaticDrivers - poll rtr " );
	
	// note -- do not poll for bfeUMD slots -- as this auto writes to them, and only verifies data coming back if bfe set any labels

	}
	catch(structured_exception const & e)
	{
		Log("BFEUMDLabel_Auto:PollAutomaticDrivers() - thrown exception %x from %x", e.what(), e.where());
		exit(1);
	}
	catch(...)
	{
		Log("BFEUMDLabel_Auto:PollAutomaticDrivers() - Unknown exception ");
		exit(1);
	}

}

void SendAllUMDNamesToBFEDriverOnce()
{
	// SEND ALL THE umd labels to driver at startup
	char szUMDName[BUFFER_CONST]=" ", szAutoName[BUFFER_CONST]=" ", szManName[BUFFER_CONST]=" ";
	char szSoftName[BUFFER_CONST]= " ", szCommand[BUFFER_CONST];
	int iStyle=0, iForm=0, iDb=0, iList=-1, iCalcSrc=0;
	char szSncsLabel[BUFFER_CONST];

	int imaxSrc = routerData->getMaximumSourcesUsed();
	for (int iSrc=1;iSrc<=imaxSrc;iSrc++) {

		// get db0 src name to form umd string -- no soft name to add at present
		GetSourceDatabaseName( iSrc, 0, szUMDName );  // left side of umd is always the db0 name
		// get data for source
		allLabelsDatabase->getAllRouterLabelData(iSrc,szAutoName,szManName,&iStyle,&iForm,szSncsLabel,&iDb,&iList );

		// determine source type -- will dictate what kind of processing required and build umd label -- for right side
		eAliasSourceType eSrcType =  routerData->getSourceType(iSrc);
		switch(eSrcType) {
		case eExternalType:
			// create umd label -- get any soft label for this  src
			if (iStyle==eAutomaticStyle) {
				if (strlen(szAutoName)>0) {
					strcpy( szSoftName, szAutoName);
				}
			} 
			else if (iStyle==eManualStyle) {
				if (strlen(szManName)>0) {
					strcpy( szSoftName, szManName);
				}
			}
		    break;
		case eLoopedType:
			// looped srcs still have their hard name first and hard name of calc src for soft name
			iCalcSrc = routerData->getCalculatedSourceForDestination(iSrc);
			GetSourceDatabaseName( iCalcSrc, 0, szSoftName );  // left side of umd is always the db0 name			
			break;

		default :// plain type -- LOCAL and CAMERA
			// simple form  -- at present -- just output src name to bfekscdriver dest slot
			if (iStyle==eAutomaticStyle) {
				if (strlen(szAutoName)>0) {
					strcpy( szSoftName, szAutoName);
				}
			} 
			else if (iStyle==eManualStyle) {
				if (strlen(szManName)>0) {
					strcpy( szSoftName, szManName);
				}
			}
		} // switch

		// make final umd name
		strcat( szUMDName, "|" );
		strcat( szUMDName, szSoftName );
			
		// update bfekscdriver
		wsprintf( szCommand, "IW %d '%s' %d", kscUmdData->getKSCDriverNumber(), szUMDName, iSrc );
		ecBFEClient->txinfocmd( szCommand );
	}
}

void SendModifiedUMDNameToBFEDriver( int iSrc  )
{
	// SEND ALL THE umd labels to driver at startup
	char szUMDName[BUFFER_CONST]=" ", szAutoName[BUFFER_CONST]=" ", szManName[BUFFER_CONST]=" ";
	char szSoftName[BUFFER_CONST]= " ", szCommand[BUFFER_CONST];
	int iStyle=0, iForm=0, iDb=0, iList=-1, iCalcSrc=0;
	char szSncsLabel[BUFFER_CONST];

	int imaxSrc = routerData->getMaximumSourcesUsed();
	if ( (iSrc>0)&&(iSrc<=imaxSrc)) {
		// get db0 src name to form umd string -- no soft name to add at present
		GetSourceDatabaseName( iSrc, 0, szUMDName );  // left side of umd is always the db0 name
		// get data for source
		allLabelsDatabase->getAllRouterLabelData(iSrc,szAutoName,szManName,&iStyle,&iForm,szSncsLabel,&iDb,&iList );

		// determine source type -- will dictate what kind of processing required and build umd label -- for right side
		eAliasSourceType eSrcType =  routerData->getSourceType(iSrc);
		switch(eSrcType) {
		case eExternalType:
			// create umd label -- get any soft label for this  src
			if (iStyle==eAutomaticStyle) {
				if (strlen(szAutoName)>0) {
					strcpy( szSoftName, szAutoName);
				}
			} 
			else if (iStyle==eManualStyle) {
				if (strlen(szManName)>0) {
					strcpy( szSoftName, szManName);
				}
			}
		    break;
		case eLoopedType:
			// looped srcs still have their hard name first and hard name of calc src for soft name
			iCalcSrc = routerData->getCalculatedSourceForDestination(iSrc);
			GetSourceDatabaseName( iCalcSrc, 0, szSoftName );  // left side of umd is always the db0 name			
			break;

		default :// plain type -- LOCAL and CAMERA
			// simple form  -- at present -- just output src name to bfekscdriver dest slot
			if (iStyle==eAutomaticStyle) {
				if (strlen(szAutoName)>0) {
					strcpy( szSoftName, szAutoName);
				}
			} 
			else if (iStyle==eManualStyle) {
				if (strlen(szManName)>0) {
					strcpy( szSoftName, szManName);
				}
			}
		} // switch

		// make final umd name
		strcat( szUMDName, "|" );
		strcat( szUMDName, szSoftName );
			
		// update bfekscdriver
		wsprintf( szCommand, "IW %d '%s' %d", kscUmdData->getKSCDriverNumber(), szUMDName, iSrc );
		ecBFEClient->txinfocmd( szCommand );
	}
}

/*
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//      Process Panel Commands 
//
//

bool findString( char szStr[BUFFER_CONST], char szFind[BUFFER_CONST]) 
{
	char *pdest;
	pdest = strstr( szStr, szFind );
	if( pdest != NULL ){
		// string found
		return TRUE;
	}
	return FALSE;
}

int ParseCommand( char szString[BUFFER_CONST])
{
	// split string to commandtype and string, vertical bar is delimiter
	char szCmd[BUFFER_CONST]="", szData[BUFFER_CONST]="", *tok;
	int iType=0, iTokCount = 0;
	bool bContinue=TRUE;

	// extract data for entry
	iTokCount = 0;
	tok=strtok(szData, "|" );  
	while ((tok)&&(bContinue==TRUE)) {
		iTokCount++;
		switch(iTokCount){
		case 1:	
			strcpy(szCmd, tok );
			break;
		case 2:
			strcpy(szData, tok );
			break;
		default:
			bContinue=FALSE; // too many items -- process what we have extracted
		}
		tok=strtok(NULL,"|"); 				
	}// while
	// determine command
	if (findString(szCmd, "AMEND" )==TRUE) iType= eAmendCommand;
	if (findString(szCmd, "ADD" )==TRUE) iType= eAddCommand;
	if (findString(szCmd, "REMOVE" )==TRUE) iType= eRemoveCommand;
	if (findString(szCmd, "DELETE" )==TRUE) iType= eDeleteCommand;
	if (findString(szCmd, "CLEAR" )==TRUE) iType= eClearCommand;

	// return data
	strcpy(szString, szData );
	return iType;

}

			
void ProcessPanelCommand( int iFromSlot, LPCSTR szSlot )
{
	char szCommand[BUFFER_CONST];
	strcpy(szCommand, szSlot);

	int iCommandType = ParseCommand( szCommand );
	switch (iCommandType) {
	case eAmendCommand: 
		break;
	case eAddCommand: 
		break;
	case eRemoveCommand: 
		break;
	case eDeleteCommand: 
		break;
	case eClearCommand: 
		break;
	}

}
*/



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//  ProcessRouterSlotChanges  
//

void ProcessRouterSlotChange( int iSource, LPCSTR  szData )
{
	char szNewData[BUFFER_CONST], szCommand[BUFFER_CONST], szdbName[BUFFER_CONST];
	int  iStyle, iForm, iSrcNum, iDbEntry, iListViewIndex, iSncsIndex=0;
	char szAuto[BUFFER_CONST], szMan[BUFFER_CONST], szTmp[BUFFER_CONST], *pdest;
	char szSNCSAlias[BUFFER_CONST], szSNCSmanl[BUFFER_CONST], szSncsLabel[BUFFER_CONST];
	int  iSNCSStyle, iSNCSForm, iDb, iLV, iAdex=0, iMdex=0;
	bncs_string  sstr1, sstr2;
	bncs_stringlist ssl;

	strcpy(szNewData, szData );
	// store new data
	allLabelsDatabase->storeRouterLabelData(iSource, szNewData );
	// get stored data 
	allLabelsDatabase->getAllRouterLabelData(iSource,&iSrcNum,szAuto,szMan,&iStyle,&iForm,szSncsLabel,&iDbEntry,&iListViewIndex );

	if (strlen(szSncsLabel)>1) {
		// sncs label being used or parameter has been set with something !!!
		// sncs labels start with $ so will always be more than 1 char - sorts space too
		if (allLabelsDatabase->getAllSncsLabelData(szSncsLabel, &iSncsIndex, szSNCSAlias, szSNCSmanl, &iSNCSStyle,&iSNCSForm, &ssl, &iDb, &iLV )) {
			sstr1 = szSNCSAlias;
			sstr2 = sstr1.stripWhiteSpace();
			if (sstr2.length()==0) strcpy( szSNCSAlias, szSncsLabel);
		}
		else {
			// for some reason the  label is not found 
			Debug("ProcessRouterSlotChange sncs label %s  not found -- defaulting to label name", szSncsLabel );
			strcpy( szSNCSAlias, szSncsLabel );
		}
		strcpy( szAuto, szSNCSAlias);
	}
	else {
		// check to see if szAuto has $ as first char -- may be sncs alias  -- this is only if they are playing silly beggars with panels 
		// if SNCS label involved -- get sncs data and update stored router data 
		// Auto name first :
		pdest = strchr( szAuto, '$');
		if (pdest!=NULL) { // $ found
			if (allLabelsDatabase->getAllSncsLabelData(szAuto, &iAdex, szSNCSAlias,szSNCSmanl, &iSNCSStyle,&iSNCSForm, &ssl, &iDb, &iLV )) {
				sstr1 = szSNCSAlias;
				sstr2 = sstr1.stripWhiteSpace();
				if (sstr2.length()>0) 	strcpy( szAuto, szSNCSAlias);
			}
			else {
				// for some reason the  label is not found 
				Debug("ProcessRouterSlotChange $$$$ auto %s  not found -- keeping auto  name", szAuto );
			}
		}
	} 
	// Manual name next : --- really playing silly beggars now !!
	if (strlen(szMan)>0) {
		pdest = strchr( szMan, '$');
		if (pdest!=NULL) {
			if (allLabelsDatabase->getAllSncsLabelData(szMan, &iMdex, szSNCSAlias,szSNCSmanl, &iSNCSStyle,&iSNCSForm, &ssl, &iDb, &iLV )) {
				sstr1 = szSNCSAlias;
				sstr2 = sstr1.stripWhiteSpace();
				if (sstr2.length()>0) strcpy( szMan, szSNCSAlias);
			}
		}
	} 
	
	// to bounce back out to infodriver for that src -- build string  
	wsprintf( szCommand, "~<%04d>~<%s>~<%s>~<", iSource, szAuto, szMan );
	if (iStyle==eManualStyle) {
		strcat( szCommand, "MANUAL" );
		strcpy (szdbName, szMan );
	}
	else {
		strcat( szCommand, "AUTO" );
		strcpy (szdbName, szAuto );
	}
	strcat( szCommand, ">~<" );
	if (iForm==ePermamentForm) {
		strcat( szCommand, "PERM" );
	}
	else {
		strcat( szCommand, "TEMP" );
	}
	wsprintf( szTmp, ">~<%s>", szSncsLabel);
	strcat( szCommand, szTmp);

	// now bounce back out to infodriver for panels update
	eiBFEInfoId->updateslot(iSource, szCommand );

	// update db3 of ext inf dev ini file
	ecBFEClient->setdbname(iDevice, 3, iSource, szCommand, FALSE );

	// re-update router label record with modified values
	allLabelsDatabase->storeRouterLabelData(iSource, szCommand );
	
	// force reevaluation of any dests 
	for (int iD=1;iD<=routerData->getMaximumDestinationsUsed();iD++) {
		if (routerData->getCalculatedSourceForDestination(iD)==iSource)
			ProcessRouterRevertive( iD, routerData->getRoutedSourceForDestination(iD) ); 
	}
	ProcessAllReEnterantDestinations( ); // needs to happen...

	// update  db2 in rtr dev for src -- 
	if (HasSoftNameChanged( iSource, szdbName )==TRUE) {
		UpdateRouterDeviniDB2( iSource, szdbName );
		// send to bfe driver at this point too if different as it may have changed
		SendModifiedUMDNameToBFEDriver( iSource );
	}
	
	// update app dialog
	if (hWndUMDData2) {
		if (iForm==ePermamentForm)
			UpdateTextItemtoRowInListBox(hWndUMDData2, iListViewIndex+1, 3, "perm" );
		else 
			UpdateTextItemtoRowInListBox(hWndUMDData2, iListViewIndex+1, 3, "temp" );		
		if (iStyle==eManualStyle)
			UpdateTextItemtoRowInListBox(hWndUMDData2, iListViewIndex+1, 4, "manl" );
		else
			UpdateTextItemtoRowInListBox(hWndUMDData2, iListViewIndex+1, 4, " auto" );	
		UpdateTextItemtoRowInListBox(hWndUMDData2, iListViewIndex+1, 5, szAuto );
		UpdateTextItemtoRowInListBox(hWndUMDData2, iListViewIndex+1, 6, szMan );
		strcpy( szTmp, " ");
		if (iSncsIndex>0) wsprintf( szTmp, "%03d", iSncsIndex);
		UpdateTextItemtoRowInListBox(hWndUMDData2, iListViewIndex+1, 7, szTmp );
		strcpy( szTmp, " ");
		UpdateTextItemtoRowInListBox(hWndUMDData2, iListViewIndex+1, 8, szSncsLabel );
	}
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//
//  ProcessSNCSSlotChanges  
//
//  ***** This is for slotchange messages only ***** not info revs from sncs land via fabian
//
void ProcessSNCSSlotChange( int iIndex, LPCSTR szData  )
{
	char szNewData[BUFFER_CONST];
	char szSNCSAlias[BUFFER_CONST], szSNCSLabel[BUFFER_CONST], szSNCSOldLabel[BUFFER_CONST]="", szSNCSmanl[BUFFER_CONST];
	int  iSNCSStyle, iSNCSForm, iDb, iLV, iAdex=0, iMdex=0;
	bncs_stringlist ssl;
	int iDataIndex = iIndex-STARTSNCSSLOTS;

	if ( (iDataIndex>0)&&(iDataIndex<=allLabelsDatabase->getMaxSncsLabelsInUse()) ) {

		// first get current sncs label data  -- mainly to check if label being cleared
		allLabelsDatabase->getSncsLabel(iDataIndex, szSNCSOldLabel  );

		strcpy(szNewData, szData );
		// do some checks for validity first just in case ??
		// store new data
		allLabelsDatabase->storeSncsLabelData(iDataIndex, szNewData );

		// write out to slot 2000+ for label for panels
		eiBFEInfoId->updateslot(iIndex, szData );

		// update db2 file 
		eiBFEInfoId->setdbname( sncsData->getDeviceNumber() , 2, iDataIndex, szData, FALSE );

		allLabelsDatabase->getAllSncsLabelData(iDataIndex, szSNCSLabel, szSNCSAlias, szSNCSmanl, &iSNCSStyle, &iSNCSForm, &ssl, &iDb, &iLV  );

		// update app dialog
		if (hWndUMDData1) {
			UpdateTextItemtoRowInListBox(hWndUMDData1, iDataIndex, 2, szSNCSLabel );
			if (iSNCSForm==ePermamentForm)
				UpdateTextItemtoRowInListBox(hWndUMDData1, iDataIndex, 3, "perm" );
			else 
				UpdateTextItemtoRowInListBox(hWndUMDData1, iDataIndex, 3, "temp" );	
			UpdateTextItemtoRowInListBox(hWndUMDData1, iDataIndex, 4, szSNCSAlias );
			//bncs_string sstr = ssl.toString(',');
			//strcpy(szTmp, sstr );
			//UpdateTextItemtoRowInListBox(hWndUMDData1, iDataIndex, 5, szTmp );
		}

		// changes in labels etc can mean changes to softnames for sources
		char szAuto[BUFFER_CONST]=" ", szMan[BUFFER_CONST]=" ", szCommand[BUFFER_CONST]=" ", szSncsLabel[BUFFER_CONST]=" ";
		char szTmp[ BUFFER_CONST ];
		int  iStyle, iForm, iDbEntry, iListViewIndex;
		char *pdest, *pdest1; 
		
		// only do this outside of clearing of NEW SNCS lables
		if (iDataIndex!=iResolvingNewSNCSIndex) {
			// need to do this once only per rev
			for ( int iSource=1;iSource<routerData->getMaximumSourcesUsed();iSource++) {
				if (allLabelsDatabase->getAllRouterLabelData(iSource, szAuto, szMan, &iStyle, &iForm, szSncsLabel, &iDbEntry, &iListViewIndex)) {
					
					pdest = strstr( szSNCSLabel, "$NAME" ); // new label is it $NAME
					pdest1 = strstr( szSNCSOldLabel, szSncsLabel ); // is old label same as current label assigned to src
					
					if ((pdest!=NULL)&&(pdest1!=NULL)) {
						// clearing current label 
						Debug(" Clearing src %d for label %s, old auto %s, old man %s ", iSource, szSNCSOldLabel, szAuto, szMan );
						wsprintf( szCommand, "~<%04d>~< >~< >", iSource );
						//strcat( szCommand, szMan);
						if (iStyle==eManualStyle) strcat( szCommand, "~<MANUAL>~<" );
						else strcat( szCommand, "~<AUTO>~<" );
						if (iForm==ePermamentForm) strcat( szCommand, "PERM>~< >" );
						else strcat( szCommand, "TEMP>~< >" );
						ProcessRouterSlotChange(iSource, szCommand );
					}
								
					pdest = strstr( szSncsLabel,szSNCSLabel ); // new label same as current label assigned to src
					if (pdest!=NULL) {
						// this source is using this alias so therefore update if any change 
						// create a cmd string to be processed as if a router slot change  
						Debug(" Ammending src %d for label %s, old auto %s, old man %s ", iSource, szSncsLabel, szAuto, szMan );
						wsprintf( szCommand, "~<%04d>~<%s>~<%s>~<", iSource, szAuto, szMan );
						if (iStyle==eManualStyle) strcat( szCommand, "MANUAL" );
						else strcat( szCommand, "AUTO" );
						strcat( szCommand, ">~<" );
						if (iForm==ePermamentForm) strcat( szCommand, "PERM" );
						else strcat( szCommand, "TEMP" );
						wsprintf( szTmp, ">~<%s>", szSncsLabel);
						strcat( szCommand, szTmp);
						ProcessRouterSlotChange(iSource, szCommand );
					}

				}
			} // for
		} // if not resolving

	}
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


int CalculateRandomOffset()
{
	// used to create a time offset before calling TXRX function - to stop multiInfodriver tx clash
	int iRand1 = 0;
	srand(time(0)); // generate random seed point
	iRand1 = (rand() % 2007);
	return (iRand1);
}

/////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION: InfoNotify()
//
//  PURPOSE: Callback function from an infodriver, notifying event
//
//  COMMENTS: the pointer pex references the infodriver which is notifying
//			  iSlot and szSlot are provided for convenience
//			
void InfoNotify(extinfo* pex, UINT iSlot, LPCSTR szSlot)
{

	switch (pex->iStatus) {
	case CONNECTED: // this is the "normal" situation
		UpdateCounters();
		if (!SlotChange(pex->iDevice, iSlot, szSlot))
			pex->setslot(iSlot, szSlot);
	break;

	case DISCONNECTED:
		Debug("Infodriver %d has disconnected", pex->iDevice);
		SetDlgItemText(hWndDlg, IDC_INFO_STATUS, " ERROR: Disconnected");
		eiBFEInfoId->updateslot(COMM_STATUS_SLOT, "0");	 // error	
		PostMessage(hWndMain, WM_CLOSE, 0, 0);
		return;
	break;

	case TO_RXONLY:
		if (bShowAllDebugMessages) Debug("Infodriver %d received request to go RX Only", pex->iDevice);
		if (iOverallTXRXModeStatus == INFO_TXRXMODE) {
			// need to bounce this request by reasserting this txrx
			eiBFEInfoId->setmode(INFO_TXRXMODE);
			eiBFEInfoId->requestmode = TO_TXRX;
			iOverallModeChangedOver = 2;
		}
		break;

	case TO_TXRX:
		if (bShowAllDebugMessages) Debug("Infodriver %d received request to go TXRX", pex->iDevice);
		if ((pex->iDevice == iDevice) && (iOverallTXRXModeStatus == INFO_RXMODE) && (iOverallStateJustChanged == 0)) {
			// current driver rxonly - therefore try and change mode
			iOverallStateJustChanged = 1;
			int iTimerOffset = CalculateRandomOffset();
			if (bShowAllDebugMessages) Debug("InfoNotify - force go TXRX - in %d ms", iTimerOffset);
			SetTimer(hWndMain, FORCETXRX_TIMER_ID, iTimerOffset, NULL);
			SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "Req Tx/Rx :- MAIN ");
			eiBFEInfoId->updateslot(COMM_STATUS_SLOT, "1");	 // ok
		}
		break;

	case QUERY_TXRX:
	case 2190:
		// ignore this state -- means all left as it was 
		break;

	default:
		Debug("iStatus=%d", pex->iStatus);

	}

}


//
//  FUNCTION: SlotChange()
//
//  PURPOSE: Function called when slot change is notified
//
//  COMMENTS: All we need here are the driver number, the slot, and the new contents
//		
//	RETURNS: TRUE for processed (don't update the slot),
//			 FALSE for ignored (so calling function can just update the slot 
//								like a normal infodriver)
//			

BOOL SlotChange(UINT iDevice,UINT iSlot, LPCSTR szSlot)
{

	try
	{

	if ( (iSlot>0) && (iSlot<4097) ) { // all slots
		// slots 1..2000 are linked to router dests and db3 
		// slots 2001..4000 are linked to sncs labels and db2 
		if ((iSlot>STARTROUTERSLOTS)&&(iSlot<=STARTROUTERSLOTS+routerData->getMaximumSourcesUsed())) {
			// change in router slots -- process 
			SetDlgItemText( hWndDlg, IDC_DATA_PANELS, szSlot);
			ProcessRouterSlotChange( iSlot, szSlot );
		}
		else if ((iSlot>STARTSNCSSLOTS)&&(iSlot<=STARTSNCSSLOTS+MAXSNCSLABELS)) {
			// change in sncs slots -- process 
			SetDlgItemText( hWndDlg, IDC_DATA_PANELS, szSlot);
			ProcessSNCSSlotChange( iSlot, szSlot );
		}
		else {
			Debug("Slot change outside expected slots, ignored for %d string %s",iSlot,szSlot);
		}
		UpdateCounters();
		return TRUE;
	}
	else {
		Debug("Slot change ignored - slot %d has changed to %s",iSlot,szSlot);
		return FALSE;
	}

	}
	catch(structured_exception const & e)
	{
		Log("BFEUMDLabel_Auto:SlotChange() - thrown exception %x from %x", e.what(), e.where());
		exit(1);
	}
	catch(...)
	{
		Log("BFEUMDLabel_Auto:SlotChange() - Unknown exception ");
		exit(1);
	}

}






/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CSI Client functions - process reveritves from registered drivers
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

//
// get router source name from db file
//
void GetSourceDatabaseName( int iSource, int iWhichDB, char szName[BUFFER_CONST] )
{
	char szFileName[64], szDbA[64], szDbb[64], szEntry[64], szActualDBFileName[64];

	strcpy( szName, " ");
	// file names -- need to read dev ini file for exact location of db2 and db3 file 
	sprintf(szFileName, "dev_%03d.ini", routerData->getRouterNumber() );
	sprintf(szDbA, "DatabaseFile_%d", iWhichDB );
	strcpy ( szActualDBFileName, r_p( szFileName, "Database",  szDbA, "", FALSE ));  // get name from db first - will return szentry if not found
	sprintf(szDbb, "Database_%d", iWhichDB );
	sprintf(szEntry, "%04d", iSource);
	strcpy( szName, r_p(szActualDBFileName, szDbb, szEntry, " ", false));
		
}

void UpdateRouterDeviniDB2( int iSource, char szName[BUFFER_CONST] )
{
	int iRtrDevice = routerData->getRouterNumber();
	routerData->setSourceSoftName(iSource, szName );
	eiBFEInfoId->setdbname( iRtrDevice, 2, iSource, szName, FALSE );

	/// XXX PAW --- add here cmds to update bfe driver with new name -- slot number is src with new text ??
	// IS THIS really enough ?? --- is this just the soft name --- but need the hard bit too ?? -- do bfe need the FULL name 

}


void ProcessRouterDB2NameChange( int iSource, char szName[BUFFER_CONST] )
{
	// XXX TO DO ??? -- this app should be the only one changing db2 anyway ...
	Debug(" ProcessRouterDB2NameChange XXX code to do ??? %d, %s", iSource, szName);
}


//
// determine if umd has changed -- no neeed to RM etc if not changed
//
bool HasSoftNameChanged( int iSource, char szNewName[BUFFER_CONST] )
{
	char szCurrName[BUFFER_CONST] = "";
	if (routerData->getSourceSoftName(iSource,szCurrName )==TRUE) {
		int result=0;
		result=strcmp(szCurrName, szNewName );
		if (result==0) 
			return FALSE; // no difference in case, or chars
		else
			return TRUE; // there is some difference 
	}
	return FALSE;
}



//
// determine if umd has changed -- no neeed to RM etc if not changed
//
BOOL HasUMDNameChanged( int iSource, char szNewName[BUFFER_CONST] )
{
	char szCurrName[BUFFER_CONST] = "";
	if (kscUmdData->getRevertiveString(iSource, szCurrName)==TRUE) {
		int result=0;
		result=strcmp(szCurrName, szNewName );
		if (result==0) 
			return FALSE; // no difference in case, or chars
		else
			return TRUE; // there is some difference 
	}
	return FALSE;
}


//
//  calculate the new umd label and update db files, infodriver slots and app dialog 
//
BOOL ProcessRouterRevertive( int iDestination, int iSource ) 
{
	char szUMDName[BUFFER_CONST]=" ", szAutoName[BUFFER_CONST]=" ", szManName[BUFFER_CONST]=" ", szEntry[64];
	char szSoftName[BUFFER_CONST]= " ", szCommand[BUFFER_CONST];
	int iStyle=0, iForm=0, iDb=0, iList=-1, iCalcSrc=0;
	char szSncsLabel[BUFFER_CONST];

	if ( (iDestination>0)&&(iDestination<=routerData->getMaximumDestinationsUsed())
	&&(iSource>0)&&(iSource<=routerData->getMaximumSourcesUsed()) ) {

		// get db0 src name to form umd string -- no soft name to add at present
		GetSourceDatabaseName( routerData->getRoutedSourceForDestination(iDestination), 0, szUMDName );  // left side of umd is always the db0 name
		// get data for source
		allLabelsDatabase->getAllRouterLabelData(iSource,szAutoName,szManName,&iStyle,&iForm,szSncsLabel,&iDb,&iList );

		// determine source type -- will dictate what kind of processing required and build umd label -- for right side
		eAliasSourceType eSrcType =  routerData->getSourceType(iSource);
		switch(eSrcType) {
		case eExternalType:
			// create umd label -- get any soft label for this  src
			if (iStyle==eAutomaticStyle) {
				if (strlen(szAutoName)>0) {
					strcpy( szSoftName, szAutoName);
				}
			} 
			else if (iStyle==eManualStyle) {
				if (strlen(szManName)>0) {
					strcpy( szSoftName, szManName);
				}
			}
		    break;
		case eLoopedType:
			// need to get calc src number and then get data linked to that which may be any of the other types...
			// looped srcs still have their hard name first and hard name of calc src for soft name
			iCalcSrc = routerData->getCalculatedSourceForDestination(iDestination);
			//Debug( " calc src for dest %d is %d ", iDestination, iCalcSrc );
			GetSourceDatabaseName( iCalcSrc, 0, szSoftName );  // left side of umd is always the db0 name			
			break;

		default :// plain type -- LOCAL and CAMERA
			// simple form  -- at present -- just output src name to bfekscdriver dest slot
			if (iStyle==eAutomaticStyle) {
				if (strlen(szAutoName)>0) {
					strcpy( szSoftName, szAutoName);
				}
			} 
			else if (iStyle==eManualStyle) {
				if (strlen(szManName)>0) {
					strcpy( szSoftName, szManName);
				}
			}
		} // switch

		/*
		///////////////////////////////////////////////////  is this really necessary ????  i dont think soooooo
		// update automatics infodriver
		wsprintf( szCommand, "~<%04d>~<%s>~<%s>~<", iSource, szAutoName, szManName );
		if (iStyle==eManualStyle) {
			strcat( szCommand, "MANUAL" );
		}
		else {
			strcat( szCommand, "AUTO" );
		}
		strcat( szCommand, ">~<" );
		if (iForm==ePermamentForm) {
			strcat( szCommand, "PERM" );
		}
		else {
			strcat( szCommand, "TEMP" );
		}
		wsprintf( szEntry, ">~<%s>", szSncsLabel );
		strcat( szCommand, szEntry );
		eiBFEInfoId->updateslot(iSource, szCommand );
		///////////////////////////////////////////////////////
		*/

		// make final umd name
		strcat( szUMDName, "|" );
		strcat( szUMDName, szSoftName );
			
		// update db2 of  rtr db See sf24  -- with softname
		if (HasSoftNameChanged( iSource, szSoftName )==TRUE) {
			// rm db2  of rtr dev ini 
			UpdateRouterDeviniDB2( iSource, szSoftName );

		}

		// update bfekscdriver
		// check for change to umdname
		if (HasUMDNameChanged( iSource, szUMDName)==TRUE) {
			wsprintf( szCommand, "IW %d '%s' %d", kscUmdData->getKSCDriverNumber(), szUMDName, iSource );
			ecBFEClient->txinfocmd( szCommand );
		}

		//update rtr list box
		wsprintf(szEntry, "%04d", iSource );
		UpdateTextItemtoRowInListBox(hWndDestData, iDestination, 2, szEntry );
		// replace | with space
		char * pdest = strchr( szUMDName, '|' );
		int result = pdest - szUMDName;
		if( pdest != NULL ) {
			if ((result>=0)&&(result<strlen(szUMDName)) )
				szUMDName[result]=' ';
		}
		UpdateTextItemtoRowInListBox(hWndDestData, iDestination, 3, szUMDName );

	}
	return FALSE;
}



//
// special case for looped dests -- if src to looped dest changes then all the labels for reenterant src needto change too.
//

void ProcessAllReEnterantDestinations( void )
{
	int iSrc;
	for (int iDest=1;iDest<=routerData->getMaximumDestinationsUsed();iDest++) {
		iSrc = routerData->getRoutedSourceForDestination( iDest );
		if ((routerData->isDestinationReEnterant(iDest)>0)||(routerData->isSourceReEnterent(iSrc)>0)) {
			// reenterant dest or if src feeding dest is reenterant, routing may have changed and labels may need updating so force recalc
			routerData->storeGRDRevertive(iDest, iSrc); // force a recalc of looped src/dest
			ProcessRouterRevertive(iDest, iSrc );
		}
	}
}



//////////////////////////////////////////////////////////////////////////
//
// SNCS revertives from sncs land via fabian
//

BOOL IsLabelValidSncsString( bncs_string ssInstr ) 
{
	bncs_string sstr1 = ssInstr.stripWhiteSpace();
	if (sstr1.length()>0) {
		return TRUE;
		// not all labels will have $ sign at start
		//int iPos = sstr1.find('$');
		//if (iPos>=0) 
		//	return TRUE;
	}
	return FALSE;		
}

void ProcessSNCSRevertive( int iRevSlot, char szData[BUFFER_CONST] )
{
	// find sncs data record for this slot
	// store data
	// determine if a new label -- add to next free slot or overwrite old unassigned label

	char szNewData[BUFFER_CONST];
	char  szAutoName[BUFFER_CONST]=" ", szManName[BUFFER_CONST]=" ";
	int iStyle=0, iForm=0, iDb=0, iList=-1;
	char szSncsLabel[BUFFER_CONST];
	BOOL bContinue = FALSE;

	// process data -- should have $ as first char and then  update 2000+ etc as appropriate
	// check if label is already used -- if new add to list and set to new
	// store new, update existing ??? etc
	// get assoc rtr src -- reevaluate any changes in umd strings

	if (IsLabelValidSncsString(szData)==TRUE) {
		bContinue = TRUE;
		if (allLabelsDatabase->isSncsLabelUnique(szData)==TRUE) {
			int iNewIndex = allLabelsDatabase->findNextFreeSNCSIndex( );
			if (iNewIndex>0) {
				// update infoslot for new label for panels to pick up
				wsprintf( szNewData, "~<%s>~< >~<NEW>~<AUTO>~<TEMP>~< >", szData);
				eiBFEInfoId->updateslot(iNewIndex+STARTSNCSSLOTS, szNewData );
				// store data in sncs db
				allLabelsDatabase->storeSncsLabelData(iNewIndex, szNewData );
				// update listview of sncs data
				UpdateTextItemtoRowInListBox(hWndUMDData1, iNewIndex, 2,szData);
				UpdateTextItemtoRowInListBox(hWndUMDData1, iNewIndex, 3, "temp" );	
				UpdateTextItemtoRowInListBox(hWndUMDData1, iNewIndex, 4, " " );
			}
			else {
				Debug( "ProcessSNCSRevertive -- **** unable to add label ,%s, to table -- MAJOR ERROR ****", szData );
				bContinue = FALSE;
			}
		}
		else {
			// label seen before  -- just assign below 
			bContinue = TRUE;
		}

		if (bContinue==TRUE) {
			// determine slot to which external source -- for auto assignment 
			int iAssocSource = sncsData->getAssociatedExternalSource(iRevSlot);
			if ((iAssocSource>0)&&(iAssocSource<=routerData->getMaximumSourcesUsed())) {
				// prep newdata with source data
				allLabelsDatabase->getAllRouterLabelData(iAssocSource,szAutoName,szManName,&iStyle,&iForm,szSncsLabel,&iDb,&iList );
				wsprintf( szNewData, "~<%04d>~<%s>~<%s>~<", iAssocSource, szData, szManName );
				if (iStyle==eManualStyle) strcat( szNewData, "MANUAL" );
				else strcat( szNewData, "AUTO" );
				strcat( szNewData, ">~<" );
				if (iForm==ePermamentForm) strcat( szNewData, "PERM>~<" );
				else strcat( szNewData, "TEMP>~<" );
				strcat( szNewData, szData );
				strcat( szNewData, ">" );
				// update database , and router rev, etc and db2 if required -- check to see if diff label in use 
				ProcessRouterSlotChange	( iAssocSource, szNewData );
			}	
		} // if continue

	}
	else {
		if (strlen(szData)>0)
			Debug( "ProcessSNCSRevertive - label %s from slot %d is invalid ", szData, iRevSlot );
		else
			Debug( "ProcessSNCSRevertive - label slot %d is empty", iRevSlot );
	}

}



void StripOutQuotes( LPSTR szStr ) 
{
	int iI, iLen = strlen( szStr );
	char szTmp[256]="";

	wsprintf(szTmp,"",NULL);
	if (iLen > 2) {
		for ( iI=0; iI < (iLen-2); iI++)
			szTmp[iI] = szStr[iI+1];
		szTmp[iI] = NULL; // terminate string
		iLen = strlen( szTmp );
	}
	strcpy( szStr, szTmp );	
}


LRESULT CSIClientNotify(extclient* pec,LPCSTR szMsg)
{
	try
	{

	UINT iParamCount = pec->getparamcount();
	LPSTR szParamRev = pec->getparam(0);
	LPSTR szDevNum = pec->getparam(1);
	LPSTR szSlot = pec->getparam(2);
	LPSTR szIndex = pec->getparam(3);
	LPSTR szContent = pec->getparam(4);

	char szTmp[BUFFER_CONST] = "";
	//char szCommand[256];

	int iDevNum = atoi( szDevNum );
	int iDBNum = atoi( szSlot );

	// infodriver revs 
	int iSlotNum = atoi( szSlot );
	int iIndexNum = atoi( szIndex );

	// router revs
	int iRtrDest = atoi( szSlot );
	int iRtrSource = atoi( szIndex );
	

	switch (pec->getstate())
	{
	case REVTYPE_R:		
		if (iParamCount==5) {
			if (routerData->getRouterNumber() == iDevNum) {
				if ( (iRtrDest>0)&&(iRtrDest<=routerData->getMaximumDestinationsUsed()) ) {
					//Debug( "Rtr rev from %d, new src %d to dest %d", iDevNum, iRtrSource, iRtrDest ); 
					// store rev and do any calc required within class
					if (routerData->getRoutedSourceForDestination(iRtrDest)!=iRtrSource) {
						routerData->storeGRDRevertive(iRtrDest, iRtrSource );
						// update revertive dialog 
						SetDlgItemText( hWndDlg, IDC_DATA_ROUTER, szMsg);
						// process revertive -- if not in startup mode
						if (bAutomaticStarting==FALSE) {
							ProcessRouterRevertive( iRtrDest, iRtrSource ); 
							// resolve new umds if this dest is a looped dest
							int iReEntSrc1 = routerData->isDestinationReEnterant(iRtrDest); // cecking to see if dest is reenterant
							int iReEntSrc2 = routerData->isSourceReEnterent(iRtrSource); // checking to see if src is a reenterant one
							if ((iReEntSrc1>0)||(iReEntSrc2>0)) {
								Debug( " rtr rev -- reent src/dest 1 is %d src is %d", iReEntSrc1, iReEntSrc2); 
								ProcessAllReEnterantDestinations( ); // needs to happen...
							}
						}
					}
				}
				else
					Debug( "ERROR in RTR revertives - unexpected dest for device %d ", iRtrDest, iDevNum );			
			}
			else 
				Debug( "ERROR in RTR revertives - unexpected device %d ", iDevNum );			
		}
		else
			Debug ( "ERROR in Rtr revertives parameter count, %d, %s", iParamCount, szMsg );

		// if not starting up then process -- recalc new umd label for dest and send  to bfe driver
		// update dialog

	break;

	case REVTYPE_I:
		// test for valid infodriver rev  -- two sources --- SNCS or BFE driver
		if (iParamCount==5) {
			StripOutQuotes( szContent );
			char szData[BUFFER_CONST] = " ";
			if (strlen(szContent)>0) strcpy(szData,szContent);
			if (sncsData->getDeviceNumber()==iDevNum) {
				// update dialog
				SetDlgItemText( hWndDlg, IDC_DATA_SNCS, szMsg);
				// store rev
				sncsData->storeSNCSRevertive(iSlotNum, szData);
				// process rev
				ProcessSNCSRevertive(iSlotNum, szData );
			}
			else if (kscUmdData->getKSCDriverNumber()==iDevNum) {
				// update dialog
				SetDlgItemText( hWndDlg, IDC_DATA_BFE, szMsg);
				// store rev
				kscUmdData->storeRevertiveFromDriver(iSlotNum, szData);
				// process rev -- nothing to do at present as KSC will not write back to BNCS at present -- sulky thing !
			}
				else 
					Debug( "ERROR in INFO revertives - unexpected device %d ", iDevNum );			
		}
		else
			Debug ( "ERROR in INFO revertives parameter count, %d, %s", iParamCount, szMsg );

	break;

	case DATABASECHANGE:
		/*
		Debug ("CSIClient - Database change message - %s",szMsg);
		//TODO: Add parsing function for database changes
		// get device number , database affected 
		// if rtr device and db2 -- check for difference to name known here -- ammend if necc and update any umds
		if ( iDevNum==routerData->getRouterNumber()) {
			if (iDBNum==2) {
				if ((iIndexNum>0)&&(iIndexNum<=routerData->getMaximumSourcesUsed())) {
					//wsprintf(szTmp, "Router db change db2 index %d new name %s\n", iIndexNum, szContent );
					//OutputDebugString( szTmp );
					ProcessRouterDB2NameChange( iIndexNum, szContent );
				}
			}
		}
		*/
	break;
		
	case REVTYPE_G:
		Debug ("CSIClient - GPI Revertive message unexpected and ignored %s",szMsg);
	break;
		
	case STATUS:
		Debug("CSIClient - Status message is %s",szMsg);
	break;
		
	case DISCONNECTED:
		Debug("CSIClient - CSI has closed down - ERROR -- driver in FAIL  or  ERROR  state");
		SetDlgItemText( hWndDlg, IDC_CSI_STATUS, "CSI DISCONNECTED");
		// close auto down
	break;
		
	}

	UpdateCounters();

	/* always return TRUE so CSI doesn't delete your client registration */
	return TRUE;
	

	}
	catch(structured_exception const & e)
	{
		Log("%s:CSIClientNotify() - thrown exception %x from %x", szAppName, e.what(), e.where());	
		exit(1);
	}
	catch(...)
	{
		Log("%s:CSIClientNotify() - Unknown exception ", szAppName );
		exit(1);
	}


}


///////////////////////////////////////////////////////////////////////////////////////////
//
//  Infodriver Resilience
//
///////////////////////////////////////////////////////////////////////////////////////////
// 
//  Check infodriver status for resilience
//
void CheckAndSetInfoResilience(void)
{
	// having connected to driver now then getmode for redundancy
	// get state of infodriver
	if (eiBFEInfoId) {
		char szNM[32] = "", szCM[32] = "", szStr[64] = " ??unknown??";
		int iMode = eiBFEInfoId->getmode(); // main info determines the status of all others -- to prevent mix occuring
		int iReq = eiBFEInfoId->requestmode; // main info determines the status of all others -- to prevent mix occuring
		if (bShowAllDebugMessages) Debug("CheckInfoMode- mode %d req %d ", iMode, iReq);
		iOverallStateJustChanged = 0;

		// determine string for driver dialog window
		if ((iMode == INFO_RXMODE) || (iMode == IFMODE_TXRXINQ)) { // if still in rx
			strcpy(szStr, "RXonly - RESERVE");
			strcpy(szNM, "RXONLY");
			Restart_CSI_NI_NO_Messages();
		}
		if (iMode == INFO_TXRXMODE){
			strcpy(szStr, "Tx/Rx - MAIN OK");
			strcpy(szNM, "TX-RX");
		}

		if ((iMode != iOverallTXRXModeStatus) && (eiBFEInfoId)) {
			if (iOverallTXRXModeStatus == INFO_RXMODE) strcpy(szCM, "RXONLY");
			if (iOverallTXRXModeStatus == INFO_TXRXMODE) strcpy(szCM, "TX-RX");
			Debug("CheckInfoMode- new mode %s(%d) different to current %s(%d) -- changing status", szNM, iMode, szCM, iOverallTXRXModeStatus);
			switch (iMode) {
			case INFO_RXMODE: case IFMODE_TXRXINQ:
				strcpy(szStr, "RXonly - RESERVE");
				iOverallModeChangedOver = 5;
				iOverallTXRXModeStatus = INFO_RXMODE;
				if (eiBFEInfoId->getmode() != INFO_RXMODE) {
					eiBFEInfoId->setmode(INFO_RXMODE);
					eiBFEInfoId->requestmode = TO_RXONLY;
				}
				Debug("Automatic running in  RXONLY  mode ");
				break;
			case INFO_TXRXMODE:
				strcpy(szStr, "Tx/Rx - MAIN OK");
				iOverallModeChangedOver = 5;
				iOverallTXRXModeStatus = INFO_TXRXMODE;
				if (eiBFEInfoId->getmode() != INFO_TXRXMODE) {
					eiBFEInfoId->setmode(INFO_TXRXMODE);
					eiBFEInfoId->requestmode = TO_TXRX;
				}
				eiBFEInfoId->updateslot(COMM_STATUS_SLOT, "1"); // automatic in OK mode
				Debug("Automatic running in  TX/RX  mode ");

				break;
			default:
				strcpy(szStr, " ??unknown??");
				iOverallTXRXModeStatus = IFMODE_NONE;
				Debug("Automatic in an  UNKNOWN  infodriver mode ");
			}
		}
		SetDlgItemText(hWndDlg, IDC_INFO_STATUS, szStr);
	}
}


void Restart_CSI_NI_NO_Messages(void)
{
	if (eiBFEInfoId) eiBFEInfoId->requestmode = INFO_TXRXMODE;
	iOverallModeChangedOver = 0;
}


void ForceAutoIntoRXOnlyMode(void)
{
	//	iOverallTXRXModeStatus = UNKNOWNVAL;
	if (eiBFEInfoId) {
		eiBFEInfoId->setmode(INFO_RXMODE);					// force to rxonly -- so that resilient pair can take over
		eiBFEInfoId->requestmode = TO_RXONLY; // force other driver into tx immeadiately if running ??
		SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "FORCE RX");
	}
	iNextResilienceProcessing = 1; // reset counter 
	iOverallModeChangedOver = 5;
}


void ForceAutoIntoTXRXMode(void)
{
	//	iOverallTXRXModeStatus = UNKNOWNVAL;
	if (eiBFEInfoId) {
		eiBFEInfoId->setmode(INFO_TXRXMODE);
		eiBFEInfoId->requestmode = TO_TXRX;
		SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "Req go Tx/Rx");
		eiBFEInfoId->setslot(COMM_STATUS_SLOT, "1"); // currently automatic up
	}
	iNextResilienceProcessing = 1; // reset counter 
	iOverallModeChangedOver = 5;
}




