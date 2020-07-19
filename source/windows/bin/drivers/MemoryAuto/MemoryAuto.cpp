/* MemoryAuto.cpp - custom program file for project MemoryAuto */

#include "stdafx.h"
#include "MemoryAuto.h"

/* TODO: place references to your functions and global variables in the MemoryAuto.h file */

// local function prototypes
// this function is called when a valid slotchange notification is extracted from the InfoNotify
BOOL SlotChange(UINT iDriver,UINT iSlot, LPCSTR szSlot);

void StripOutQuotes( LPSTR szStr ) ;

void SaveActualDataString ( int iEntry, char szMemData[BUFFER_CONST] );
void SaveRecordToMemory( LPSTR szData, int  iSlotFrom );
void RecallRecordFromMemory( LPSTR szData, int iSlotFrom, BOOL bActionSettings );  // actionsettings = FALSE for poll
void DeleteRecordFromMemory( LPSTR szData, int iSlotFrom );
void SnapshotRecordToMemory( int iSlotFrom );

void	ParseCDLString( char szData[256], int iData[MAX_DATA_ENTRIES], int *iCount );
BOOL ParseIncomingData( LPSTR szSlot, int *iCommandCode,  LPSTR szData );
BOOL EmptyMemoryRecord( int iEntry );
BOOL ValidMemoryRecord( int iEntry );
BOOL DetermineDataType( char szInStr[256], int *iVal );
BOOL ValidMemoryNamefromDatabase( char szStr[BUFFER_CONST], int iVal );
BOOL FindMemoryEntry( LPSTR szData, int  *iEntry ); 
BOOL FindNextMemoryEntry( LPSTR szData, int  *iEntry );

BOOL FindRecallMemoryEntry( char szData[BUFFER_CONST], int  *iEntry,  BOOL *bSubset ); // differs from above as this also searches for default
BOOL GetMemDataElement(  char szMemName[BUFFER_CONST], int *iEntry,  char szDataElement[ BUFFER_CONST]  ); 
BOOL DataElementToBeIncluded( int iDev, int iDest, BOOL bSubset, int iList[MAX_DATA_ENTRIES] );

bool FindSnapShot( char szStr[BUFFER_CONST] );
bool FindDefault( char szStr[BUFFER_CONST] );

BOOL ValidDevice( int iDevNum, int *iWhichDeviceEntry );
BOOL ValidRevertiveDevice( int iDevNumber, int *iDevRev );

void Write_to_CircBuffer ( UINT iEntryNumber, LPCSTR szName, LPCSTR szData  );
void Write_to_FifoBuffer ( UINT iCommandCode, LPCSTR szCommand );


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//              Circ buffer routines 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FUNCTION ProcessNextCircCommand
// extracts next contents from circ buffer and 
void ProcessNextCircCommand (void)
{
	char szStr[BUFFER_CONST]; 
	UINT  iEntry, iRead, iLentoRead;
	BYTE  abName[BUFFER_CONST];
	BYTE  abData[BUFFER_CONST];

	if (iCircBufferEntriesCount > 0) {
		iRead = cb->bGet((PBYTE)&iEntry,sizeof(UINT)); // first read the entry number
		if (iRead) {
			iRead = cb->bGet((PBYTE)&iLentoRead,sizeof(UINT)); // now read the length of name string 
			if (iRead) {
				iRead = cb->bGet(abName,iLentoRead); // now read the name string
				if (iRead) {
					iRead = cb->bGet((PBYTE)&iLentoRead,sizeof(UINT)); // now read the length of data string 
					if (iRead) {
						iRead = cb->bGet(abData,iLentoRead); // now read the name string
						if (iRead) {
							iCircBufferEntriesCount--;
							if (iCircBufferEntriesCount < 0) 
								iCircBufferEntriesCount = 0;
							// now update db name 
							strcpy( szStr, (char*)abName );
							ei.setdbname( iDevice, stDefinedData[1].iKeyNameDatabase, (iEntry+MEMORY_START-1), szStr, FALSE );
							// now update info slot
							strcpy( szStr, (char*)abData );
							ei.setslot(  iEntry+MEMORY_START-1, szStr );  // infodriver memories start at slot 81 - but 1 internally						
						}
						else {
							Debug( "ERROR in retreiving data string from buffer");
							iCircBufferEntriesCount = 0; // nothing to proces
						}
					}
					else {
						Debug( "ERROR in retreiving length of data from buffer");
						iCircBufferEntriesCount = 0; // nothing to proces
					}
				}
				else {
					Debug( "ERROR in retreiving name string from buffer");
					iCircBufferEntriesCount = 0; // nothing to proces
				}
			}
			else {
				Debug( "ERROR in retreiving length of name from buffer");
				iCircBufferEntriesCount = 0; // nothing to proces
			}
		}
		else {
			Debug( "ERROR in entry number from buffer");
			iCircBufferEntriesCount = 0; // nothing to proces
		}
	} // circbuffercount=0, no more to process

	// update circ buffer dialog
	itoa( iCircBufferEntriesCount, szStr,  10 );
	SetDlgItemText(hWndDlg, IDC_CIRC_BUFF, szStr); 

	// if circ buffer empty no need to keep timer running so end it until more entries added below
	if ((iCircBufferEntriesCount<=0 )&&(bCircBufferTimerRunning==TRUE)) {
		KillTimer(hWndMain, CIRC_BUFFER_TIMER_ID);
		bCircBufferTimerRunning=FALSE;
		// now check to see if fifo buffer timer needs to run
		if (((stCommandBuffer.iLastEntry-stCommandBuffer.iReadEntry)>0)&&(bFifoBufferTimerRunning==FALSE)) {
			// start circ buffer timer - ini file setting default 20ms 50 commands per second 
			iFifoBufferTimerId = SetTimer( hWndMain, FIFO_BUFFER_TIMER_ID, iCircBufferInterval, NULL ); // uses same timer interval - user setting
			bFifoBufferTimerRunning=TRUE;
		}
	}
}


// FUNCTION Write_into_CircBuffer
/* add command string into circular buffer
   test for awaiting reply, 
   if not then process next command from buffer 
		and call write to device
   else awaiting reply, end, and the command just added will be processed in time
*/
void Write_to_CircBuffer ( UINT iEntryNumber, LPCSTR szName, LPCSTR szData  )
{
	char szStr[BUFFER_CONST]; 
	UINT  iLen;

    strcpy( szStr, szName );
	iLen = strlen( szStr ) + 1;	// to include 0 terminator
	if (iLen < BUFFER_CONST) {
		if (cb->bAdd( (PBYTE)&iEntryNumber, sizeof(UINT) )) { // first add the entry number
			if (cb->bAdd( (PBYTE)&iLen, sizeof(UINT) )) {   // now add length of name string
				if (cb->bAdd( (PBYTE)szStr, iLen )) { // now add the name string
					strcpy( szStr, szData );
					iLen = strlen( szData ) + 1;	// to include 0 terminator
					if (cb->bAdd( (PBYTE)&iLen, sizeof(UINT) )) {   // now add length of data string
						if (cb->bAdd( (PBYTE)szStr, iLen )) { // now add the data string
							iCircBufferEntriesCount++;
							Debug( "Adding name %s to buffer, len:%d, buffer entries now:%d", szName, iLen, iCircBufferEntriesCount );
						}
						else {
							Debug( "FAILED TO ADD TO CIRC BUFFER (5) %s ", szStr );
							// kill and reset buffer at this point and global flags too??
						}
					}
					else {
						Debug( "FAILED TO ADD TO CIRC BUFFER (4) " );
						// kill and reset buffer at this point and global flags too??
					}
				}
				else {
					Debug( "FAILED TO ADD TO CIRC BUFFER (3) %s ", szStr );
					// kill and reset buffer at this point and global flags too??
				}
			}
			else {
				Debug( "FAILED TO ADD TO CIRC BUFFER (2) " );
				// kill and reset buffer at this point and global flags too??
			}
		}
		else {
			Debug( "FAILED TO ADD TO CIRC BUFFER (1) " );
			// kill and reset buffer at this point and global flags too??
		}
	}
	else 
		Debug("WriteToCircBuffer, command too long: %s ", szStr );

	// update circ buffer dialog
	itoa(  iCircBufferEntriesCount,szStr,  10 );
	SetDlgItemText(hWndDlg, IDC_CIRC_BUFF, szStr); 

	if ((iCircBufferEntriesCount>0)&&(bCircBufferTimerRunning==FALSE)&&(bFifoBufferTimerRunning==FALSE)) {
		// start circ buffer timer - ini file setting default 50ms 20 commands per second 
		// 1.33 only start circ buffer if fifo buffer timer not running too 
		iCircBufferTimerId = SetTimer( hWndMain, CIRC_BUFFER_TIMER_ID, CIRC_BUFFER_TIMER_INTERVAL, NULL );
		bCircBufferTimerRunning=TRUE;
		Debug( "Circ Buffer timer running %d", iCircBufferTimerId );
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//              FiFo buffer routines 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//FUNCTION ProcessNextFifoCommand
// extracts next contents from buffer and sends out command
void ProcessNextFifoCommand (void)
{
	char szStr[BUFFER_CONST], szTmp[10]; 

	if ( ( stCommandBuffer.iReadEntry<stCommandBuffer.iLastEntry) && (stCommandBuffer.iLastEntry>0) ) {
		stCommandBuffer.iReadEntry++;
		strcpy( szStr, stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].szData );
		// now send command out 
		ec.txrtrcmd( szStr );
	}
	else { // processed all entries so reset counters
		stCommandBuffer.iReadEntry=0;
		stCommandBuffer.iLastEntry=0;
	}

	// if buffer empty no need to keep timer running so end it until more entries added below
	if ((stCommandBuffer.iLastEntry==0 )&&(bFifoBufferTimerRunning==TRUE)) {
		KillTimer(hWndMain, FIFO_BUFFER_TIMER_ID);
		bFifoBufferTimerRunning=FALSE;
		// ver 1.33 check circ buffer now
		if ((iCircBufferEntriesCount>0)&&(bCircBufferTimerRunning==FALSE)) {
			// start circ buffer timer - ini file setting default 50ms 20 commands per second 
			iCircBufferTimerId = SetTimer( hWndMain, CIRC_BUFFER_TIMER_ID, CIRC_BUFFER_TIMER_INTERVAL, NULL );
			bCircBufferTimerRunning=TRUE;
			Debug( "Circ Buffer timer running %d", iCircBufferTimerId );
		}
	}

	// update circ buffer dialog
	int iCount = stCommandBuffer.iLastEntry-stCommandBuffer.iReadEntry;
	itoa( iCount, szTmp,  10 );
	SetDlgItemText(hWndDlg, IDC_FIFO_BUFF, szTmp); 

	// added feature
	if (bCurrentlyRecalling==TRUE) {
		if (iCount<10) {
			bCurrentlyRecalling=FALSE;
			// write to info slots 3, 13 etc
			strcpy(szTmp, "0" );
			ei.updateslot(  3, szTmp ); // slot 3 etc
			ei.updateslot( 13, szTmp ); // slot 3 etc
			ei.updateslot( 23, szTmp ); // slot 3 etc
			ei.updateslot( 33, szTmp ); // slot 3 etc
			ei.updateslot( 43, szTmp ); // slot 3 etc
			ei.updateslot( 53, szTmp ); // slot 3 etc
			ei.updateslot( 63, szTmp ); // slot 3 etc
		}
	}

}

// FUNCTION Write_to_FifoBuffer
/* add command string into circular buffer
   test for awaiting reply, 
   if not then process next command from buffer 
		and call write to device
   else awaiting reply, end, and the command just added will be processed in time

*/
void Write_to_FifoBuffer( UINT iCommandCode, LPCSTR szCommand )
{
	char szStr[BUFFER_CONST]; 

	if (stCommandBuffer.iLastEntry<(MAX_DEVICE_SIZE-1) ) {
		// space to add new entry
		stCommandBuffer.iLastEntry ++;
		strcpy( stCommandBuffer.BufferData[stCommandBuffer.iLastEntry].szData, szCommand );	
	}
	else {
		//  check if  readindex is above 0 -  i.e.  processing started - so there is space lower down in buffer
		// although now processing stored commands out of order from earlier processing 
		if (stCommandBuffer.iReadEntry>1) { // space at start of buffer 
			stCommandBuffer.iReadEntry--;
			strcpy( stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].szData, szCommand );	
		}
		else
		Debug( "Write to FIFO buffer -ERROR buffer is full - start processing entries " );
	}

	if (((stCommandBuffer.iLastEntry-stCommandBuffer.iReadEntry)>0)&&(bFifoBufferTimerRunning==FALSE)&&(bCircBufferTimerRunning==FALSE)) {
		// start circ buffer timer - ini file setting default 20ms 50 commands per second 
		// ver 1.33 - only start fifo timer if circ buffer timer not running
		iFifoBufferTimerId = SetTimer( hWndMain, FIFO_BUFFER_TIMER_ID, iCircBufferInterval, NULL ); // uses same timer interval - user setting
		bFifoBufferTimerRunning=TRUE;
	}

	// update fifo buffer dialog
	itoa(  (stCommandBuffer.iLastEntry-stCommandBuffer.iReadEntry), szStr,  10 );
	SetDlgItemText(hWndDlg, IDC_FIFO_BUFF, szStr); 
		
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//  FUNCTION: InitializeMemoryStructures()
//
//  PURPOSE: initialise arrays to 0;
//
//  COMMENTS: 
//			
void InitializeMemoryStructures(void)
{
	int i, j, k;
	Debug( "into Init Mem Structures" );

	for ( i=0;i<MAX_MEMORIES;i++ ) {
		stMemory[i].bEntryUsed = FALSE;
		stMemory[i].iNumberOfMultipleSlotsUsed = 0;
		stMemory[i].iWhichDefintionEntry = 1; // default to 1 - as onlt 1 defined type per memory app at present
		strcpy( stMemory[i].szMemoryName, "-" );
		strcpy( stMemory[i].szMemoryData, "-" );
	}

	for ( i=0;i<MAX_PREDEFINED_MEMORY_TYPES;i++ ) {
		stDefinedData[i].bDefinitionUsed = FALSE;
		strcpy( stDefinedData[i].szTypeName, "" );
		stDefinedData[i].iKeyNameDatabase = 0;
		stDefinedData[i].iDefinitionEntries = 0;
		for ( j =0;j<MAX_DEVICE_ENTRIES; j++ ) {
			stDefinedData[i].stDefinitionData[j].iDeviceType=0;
			stDefinedData[i].stDefinitionData[j].iDeviceNumber=0;
			stDefinedData[i].stDefinitionData[j].iDatabase=0;
			stDefinedData[i].stDefinitionData[j].bRangeDefinition=FALSE;
			stDefinedData[i].stDefinitionData[j].iDataCount=0;
			for (k =0;k<MAX_DATA_ENTRIES; k++ ) {
				stDefinedData[i].stDefinitionData[j].iData[k]=0;
			}
			for (k =0;k<MAX_DATA_ENTRIES; k++ ) {
				stDefinedData[i].stDefinitionData[j].iDefaults[k]=0;
			}
			strcpy( stDefinedData[i].stDefinitionData[j].szDataEntry, "" );
		}
	}

	for (  i=0;i<MAX_DEVICES_FOR_REVERTIVES;i++ ) {
		stDeviceRevertives[i].iDeviceNumber=0;
		stDeviceRevertives[i].iDeviceSize=0;
		stDeviceRevertives[i].iDeviceType=0;
		for ( j =0;j<MAX_DEVICE_SIZE; j++ ) {
			stDeviceRevertives[i].stRevertive[j].iNumericData=0;
			strcpy( stDeviceRevertives[i].stRevertive[j].szStringData, " " );
		}
	}

	// buffer
	stCommandBuffer.iReadEntry = 0;
	stCommandBuffer.iLastEntry = 0;
	for (i=0;i<MAX_DEVICE_SIZE;i++) 
		strcpy( stCommandBuffer.BufferData[i].szData, "" );

	stRecallMemData.iStartMemoryEntry = 0;
	stRecallMemData.iNumberOfElements = 0;
	stRecallMemData.iCurrentElement = 1;
	for (i=0;i<MAX_DATA_ENTRIES;i++ )
		strcpy ( stRecallMemData.MemData[i].szData, "" );

	for (i=0;i<MAX_DEVICE_ENTRIES;i++ )	{
		stSubsetData[i].iDeviceNumber = 0;
		stSubsetData[i].bRangeDefinition = FALSE;
		for (j=0;j<MAX_DATA_ENTRIES;j++)
			stSubsetData[i].iList[j]=0;
	}

	iNumberOfMemoryEntries = 0;
	iLastMemoryEntry = 0;
	iCurrentFreeMemorySlot = 1;
	iPercentMemoryFilled = 0;
	iMemorySaveSize = 1; // min number of slots to save a memory to

	// circ buffer
	cb = new circbuf ();
	iCircBufferEntriesCount = 0;
	bCircBufferTimerRunning = FALSE;
}


BOOL RangeDefined( char *tok )
{
	if (strcmp(tok, "RANGE")==STRINGSEQUAL)
		return TRUE;
	else 
		return FALSE;
}


//  FUNCTION: ParseCDLString()
//
//  PURPOSE:parse a comma delimited list into an array to pass back for storage
//
void	ParseCDLString( char szCDLData[256], int iData[MAX_DATA_ENTRIES], int *iCount, bool *bRange )
{
	char szData [256], *tok;
	int  iVal=0, iInCount=0, iTokCount=0 ;

	*iCount=0;
	*bRange = FALSE;

	strcpy( szData, "" );
	strcpy( szData, szCDLData);
	if (strlen(szCDLData) > 1) {
		Debug( " ParseCDLString > %s ", szData );
		tok=strtok(szCDLData, ",");
		//0001=1,2,3,4,5,6,7,8.....
		while(tok) {
			iVal = 0; // reset each time round the loop
			iTokCount++;
			if (iTokCount==1) {
				// check for keyword - RANGE used to denote range of dests etc - elements 2 and 3  of cdl are bounds of range
				if (RangeDefined( tok )==TRUE) {
					*bRange = TRUE;
					iInCount--; // deliberate so that entries 1 and 2 will be lower and upper bound ranges
				}
			} 
			iVal = atoi(tok); // note RANGE will return a zero
			if (iInCount<MAX_DATA_ENTRIES) {
				iInCount++;
				iData[iInCount]=iVal;
			}
			else
				Debug( "ParseCDLString - too many entries in data string %d", iInCount );
			tok=strtok(NULL, ",");
		}	// while tok
	} // if strlen
	*iCount=iInCount;
}


//  FUNCTION: DetermineDataType()
//
//  PURPOSE:determin whether it is router or gpi data type defined
//
BOOL DetermineDataType( char szInStr[256], int *iVal )
{
	*iVal = 0;
	if (strcmp(szInStr, "RTR")==STRINGSEQUAL) {
		*iVal = ROUTER_MEMORY;
		return TRUE;
	}
	else {
		if (strcmp(szInStr, "GPI")==STRINGSEQUAL) {
			*iVal = GPI_MEMORY;
			return TRUE;
		}
		else {
			if (strcmp(szInStr, "INF")==STRINGSEQUAL) {
				*iVal = INFO_MEMORY;
				return TRUE;
			}
			else {
				return FALSE;
				Debug( "DetermineDAtaType - returned FALSE %s ", szInStr );
			}
		}
	}
	return FALSE;
	Debug( "DetermineDAtaType - returned FALSE %s ", szInStr );
}


BOOL ValidMemoryNamefromDatabase( char szStr[BUFFER_CONST], int iVal  )
{
	if ( atoi( szStr) != iVal ) {
		// must be a name - no further test at this time
		return TRUE;
	}
	else
		return FALSE; // i.e if slot slot contents is same as slot number then slot is empty
}


//  FUNCTION: ReadInIniFileSettings()
//
//  PURPOSE:readin all ini file settings from Auto ini file - funnily enough
//
//  COMMENTS: 
//			
void ReadInIniFileSettings(void)
{
	char szStr[256], szFileName[256], szData[256],szData2[256], szType[120] ;
	int iTokCount = 0, iTypeCount=0, iDefCount=0, iVal=0,  iNum=1, iDCount=0, i;
	bool bContinue = TRUE, bMore = TRUE, bRange=FALSE;
	int iDataRead[ MAX_DATA_ENTRIES ];

	Debug( "into ReadInIniFileSettings" );
	sprintf(szFileName, "dev_%03d.ini", iDevice);
	// read in predefined data types- including defaults
	while ( (bContinue==TRUE) && (iTypeCount<MAX_PREDEFINED_MEMORY_TYPES) ) {
		iTypeCount++;
		wsprintf(szStr,"DEFINED_%d",iTypeCount);
		strcpy( szType, "" );
		strcpy( szType, r_p( szFileName,"DEFINED_DATA",szStr,"0",FALSE) );
		Debug( "ReadInIniFileSettings - read 1: %s %s %s", szFileName, szStr, szType );
		if ((strlen(szType) > 1)&&(iTypeCount<MAX_PREDEFINED_MEMORY_TYPES)) {
			strcpy( stDefinedData[iTypeCount].szTypeName, szType );
			Debug( "DATA TYPE FOUND - %s ", szType );
			// read in all entries for defined data type			
			stDefinedData[iTypeCount].iKeyNameDatabase= atoi( r_p( szFileName,szType,"DATA_KEYNAMES_DATABASE","0",FALSE) );
			iDefCount=0; bMore=TRUE;
			do {
				strcpy( szData, "" );
				iDefCount++;
				wsprintf(szStr,"DATA_DEFINITION_%d", iDefCount);
				strcpy( szData, r_p( szFileName,szType,szStr,"0",FALSE) );
				if (DetermineDataType( szData, &iVal)) {
					Debug( "DEFINITION FOUND - %s", szData );
					stDefinedData[iTypeCount].bDefinitionUsed = TRUE;
					stDefinedData[iTypeCount].iDefinitionEntries = iDefCount;
					stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDeviceType = iVal;
					wsprintf(szStr,"%s_%d_DEVICE", szData, iDefCount);					
					stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDeviceNumber=
						atoi( r_p( szFileName,szType,szStr,"0",FALSE) );
					stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDatabase=1; // dests or gpi out
					switch (iVal) {
						case 1:  // rtr 
							wsprintf(szStr,"RTR_%d_DESTS", iDefCount);					
							strcpy( szData2, r_p( szFileName,szType,szStr,"0",FALSE) );
							// now parse cdl string of dests
							ParseCDLString( szData2, iDataRead, &iDCount, &bRange );
							stDefinedData[iTypeCount].stDefinitionData[iDefCount].bRangeDefinition = bRange;
							for (i=0;i<=iDCount;i++)
								stDefinedData[iTypeCount].stDefinitionData[iDefCount].iData[i] = iDataRead[i];
							stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDataCount = iDCount;
							// now defaults
							wsprintf(szStr,"RTR_%d_DEFAULT_SRC", iDefCount);					
							strcpy( szData2, r_p( szFileName,szType,szStr,"0",FALSE) );
							strcpy( stDefinedData[iTypeCount].stDefinitionData[iDefCount].szDataEntry, szData2 );
							Debug( "ReadInIniFileSettings - read rtr data: %s ", szData2 );
							// now parse cdl string of dests for default states
							ParseCDLString( szData2, iDataRead, &iDCount, &bRange );
							for (i=0;i<=iDCount;i++)
								stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDefaults[i] = iDataRead[i];
							if (iDCount!=stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDataCount) {
								Debug( "ReadInIniFileSettings -ERROR- difference between destinations and default src %d, %d ", 
									iDCount, stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDataCount );
							}
						break;
						case 2:  // gpi
							wsprintf(szStr,"GPI_%d_OUT", iDefCount);					
							strcpy( szData2, r_p( szFileName,szType,szStr,"0",FALSE) );
							// now parse cdl string of dests
							ParseCDLString( szData2, iDataRead, &iDCount, &bRange );
							stDefinedData[iTypeCount].stDefinitionData[iDefCount].bRangeDefinition = bRange;
							for (i=0;i<=iDCount;i++)
								stDefinedData[iTypeCount].stDefinitionData[iDefCount].iData[i] = iDataRead[i];
							stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDataCount = iDCount;
							// now defaults
							wsprintf(szStr,"GPI_%d_DEFAULT_STATE", iDefCount);					
							strcpy( szData2, r_p( szFileName,szType,szStr,"0",FALSE) );
							strcpy( stDefinedData[iTypeCount].stDefinitionData[iDefCount].szDataEntry, szData2 );
							// now parse cdl string of dests for default states
							ParseCDLString( szData2, iDataRead, &iDCount, &bRange );
							for (i=0;i<=iDCount;i++)
								stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDefaults[i] = iDataRead[i];
							if (iDCount!=stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDataCount) {
								Debug( "ReadInIniFileSettings -ERROR- difference between gpis and default states %d, %d ", 
									iDCount, stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDataCount );
							}
						break;
						case 3:  // inf - list of slots interested in							
							wsprintf(szStr,"INF_%d_SLOTS", iDefCount);					
							strcpy( szData2, r_p( szFileName,szType,szStr,"0",FALSE) );
							// now parse cdl string of dests
							ParseCDLString( szData2, iDataRead, &iDCount, &bRange );
							stDefinedData[iTypeCount].stDefinitionData[iDefCount].bRangeDefinition = bRange;
							stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDataCount = iDCount;
							for (i=0;i<=iDCount;i++)
								stDefinedData[iTypeCount].stDefinitionData[iDefCount].iData[i] = iDataRead[i];
							// now defaults
							wsprintf(szStr,"INF_%d_DEFAULT_DATA", iDefCount);					
							strcpy( szData2, r_p( szFileName,szType,szStr,"0",FALSE) );
							strcpy( stDefinedData[iTypeCount].stDefinitionData[iDefCount].szDataEntry, szData2 );

							 // now parse cdl string of dests for default states 
							ParseCDLString( szData2, iDataRead, &iDCount, &bRange );
							for (i=0;i<=iDCount;i++)
								stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDefaults[i] = iDataRead[i];
							if (iDCount!=stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDataCount) {
								Debug( "ReadInIniFileSettings -ERROR- difference betweeninfs and default states %d, %d ", 
									iDCount, stDefinedData[iTypeCount].stDefinitionData[iDefCount].iDataCount );
							}						
						break;

					} // switch
				}
				else
					bMore=FALSE;
			}
			while ((bMore==TRUE)&&(iDefCount< MAX_DEVICE_ENTRIES));

		} // if strlen
		else
			bContinue = FALSE; // no more types to be found
	} // while 
	if (iTypeCount==0) 
		Debug( "ReadInIniFileSettings - WARNING - no definitions found in dev ini file " );

	// update debug window on findings
	
}


//  FUNCTION: LoadCurrentSetOfMemoriesFromInfodriver()
//
//  PURPOSE:get from infodriver slots 81 to 4096 all data currently stored and load
// returns count of entries readin 
//
int LoadCurrentSetOfMemoriesFromInfodriver(void)
{
	// first read slot 80 and obtain the number of memories last used
	// read in from slot 81 until all memories read in or 4096 is reached - if short, log error and readjust contents of 80

	int iCounter=0, iC=0, iSlot =MEMORY_COUNTER_SLOT, iMemSlot, iStartMemSlot=1;
	char szStr[BUFFER_CONST], szData[BUFFER_CONST], szActualDBFileName[32],  szDbDefaultFileName[32],  szDbFileName[32];
	char szFileName[256], szEntry[32] , szTmp[16], szDbName[32];

	// read in counter from slot 80 - counter for number of mem locs used ( saving reading all 3995 info slots )
	ei.getslot( MEMORY_COUNTER_SLOT, szStr );
	Debug( "LoadCurrentSetOfMemoriesFromInfodriver  -  Number of lines stored is %s ", szStr );
	iCounter = atoi( szStr );
	if (iCounter > 0 ) {
		// need to get the exact location of database file -- may not be inside dev ini file -- but as its own .dbx file


		sprintf(szFileName, "dev_%03d.ini", iDevice);
		sprintf(szDbName, "Database_%d", stDefinedData[1].iKeyNameDatabase );
		sprintf(szDbFileName, "DatabaseFile_%d", stDefinedData[1].iKeyNameDatabase );
		sprintf(szDbDefaultFileName, "DEV_%03d.DB%d", iDevice, stDefinedData[1].iKeyNameDatabase );
		strcpy ( szActualDBFileName, r_p( szFileName, "[Database]",  szDbFileName, szDbDefaultFileName, FALSE ));  // get name from db first - will return szentry if not found

		do {
			iSlot++;
			sprintf( szEntry, "%04d", iSlot );
			strcpy ( szStr, r_p( szActualDBFileName,  szDbName,  szEntry, szEntry, FALSE ));  // get name from db first - will return szentry if not found

			// check contents of returned string - if equal to slot number eg 0001=0001 - assume unused memory slot
			if ( ValidMemoryNamefromDatabase( szStr, iSlot )) {
				ei.getslot( iSlot, szData ); // get slot
				iMemSlot=iSlot-MEMORY_START+1;
				stMemory[iMemSlot].bEntryUsed=TRUE;
				stMemory[iMemSlot].iWhichDefintionEntry = 1;  // default at present
				strcpy( stMemory[iMemSlot].szMemoryName, szStr );
				strcpy( stMemory[iMemSlot].szMemoryData, szData ); // data will be parsed as required on RECALL command
				Debug( "LoadCurrentSet - slot %d name %s, %s ",  iSlot, szStr, szData );
				// check to see if continuation from prev mem slot - ie very big memory
				if (iMemSlot > 1)   {
					// first look to see if current mem name is same as prev one
					if (stricmp(stMemory[iMemSlot].szMemoryName, stMemory[iStartMemSlot].szMemoryName )==STRINGSEQUAL) {
						// same so must be a multiple memory
						stMemory[iStartMemSlot].iNumberOfMultipleSlotsUsed++;
					}
					else { // different name so reset pointer var
						iStartMemSlot = iMemSlot; // set to current mem, 
					}
				}

				iC++; // counter of valid records read in
				wsprintf( szTmp, "%d", iC );
				// update debug window to show records being read in
				SetDlgItemText(hWndDlg, IDC_DB_RECORDS, szTmp );
 			}
		}
		while ( (iC<iCounter ) && (iSlot<4096) );
		Debug( "LoadCurrentSetOfMemoriesFromInfodriver  -  Number of lines found  is %d, got to slot %d ", iC, iSlot );
	} // if icounter>0

	// set global vars
	iCounter = iC;
	iNumberOfMemoryEntries = iC;
	wsprintf( szTmp, "%d", iC );
	SetDlgItemText(hWndDlg, IDC_DB_RECORDS, szTmp );
    itoa( ( iC/39), szTmp, 10 );
	SetDlgItemText(hWndDlg, IDC_DB_PERCENTUSED, szTmp );
 
	return iCounter;
}

//  FUNCTION: SetDefaultMemorySlot()
//
//  PURPOSE:set slot 81 from data read in from ini file
// - special save to store default memory
//
void SetDefaultMemorySlot( void) 
{
	// save defaults according to order defined in data definition
	char  szData[BUFFER_CONST], szTmp[12], szTmp2[12]; //szOutData[BUFFER_CONST],;
	int i,j, iEntry=1, iCurrMemCount=0;

	// Default should be first memory stored in database - store in info now - and read in below
    iEntry = 1;

	for (i=1;i<MAX_PREDEFINED_MEMORY_TYPES;i++)	{ // at present there is only 1 defined mem type per instance of mem app
		if (stDefinedData[i].bDefinitionUsed==TRUE) {
			strcpy( szData, "" );
			for (j=1;j<=stDefinedData[i].iDefinitionEntries;j++)	{
				wsprintf( szTmp, "%s,", stDefinedData[i].stDefinitionData[j].szDataEntry );
				if ( (strlen(szData)+strlen(szTmp))>BUFFER_CONST-2) { // must allow for NULL terminator
					// now store and inc counters
					ei.setdbname( iDevice, stDefinedData[i].iKeyNameDatabase, (iEntry+MEMORY_START-1), "DEFAULT", FALSE );
					ei.setslot(  iEntry+MEMORY_START-1, szData );  // infodriver memories start at infodriver slot 81 - but 1 internally
					Debug("SetDefaultMemorySlot > slot %d (%d) - %s ",  iEntry, iEntry+MEMORY_START-1, szData );
					// reset and inc counter
					strcpy( szData, "" );
					iEntry++;
				}
				strcat( szData, szTmp ); 
			} // for j
			// save last fragment 
			ei.setdbname( iDevice, stDefinedData[i].iKeyNameDatabase, (iEntry+MEMORY_START-1), "DEFAULT", FALSE );
			ei.setslot(  iEntry+MEMORY_START-1, szData );  // infodriver memories start at infodriver slot 81 - but 1 internally
			Debug("SetDefaultMemorySlot > slot %d (%d) - %s ",  iEntry, iEntry+MEMORY_START-1, szData );

			// update infodriver counter slot
			ei.getslot( MEMORY_COUNTER_SLOT, szTmp );
			iCurrMemCount = atoi( szTmp );
			if (iCurrMemCount<iEntry) {
				itoa( iEntry, szTmp2, 10 );
				ei.setslot( MEMORY_COUNTER_SLOT, szTmp2 );
			}

			// update dialog window
			SetDlgItemText(hWndDlg, IDC_DB_DEF_DATA1, szData); 
			SetDlgItemText(hWndDlg, IDC_DB_DEF_KEY1, stDefinedData[i].szTypeName); 
		} // if
	} // for i
}

//
//  FUNCTION: LoadSnapShotMemory()
//
void LoadSnapShotMemory(void)
{
	int iEntry;
	char szTmp[10];

	//  snapshot memory  should be first memory by default - if not there then save now !! and calc memory save size
	if (FindMemoryEntry( "SNAPSHOT", &iEntry ) ) {  // previously saved - how many slots used 
		iMemorySaveSize = stMemory[iEntry].iNumberOfMultipleSlotsUsed+1;
		itoa( iMemorySaveSize, szTmp, 10 );
		SetDlgItemText(hWndDlg, IDC_DB_SAVESIZE, szTmp );
	}
    else { // may be first time system run up so save a snapshot memory 
		SaveRecordToMemory( "SNAPSHOT" , 0 );
		if (FindMemoryEntry( "SNAPSHOT", &iEntry ) ) {  // previously saved - how many slots used 
			iMemorySaveSize = stMemory[iEntry].iNumberOfMultipleSlotsUsed+1;
			itoa( iMemorySaveSize, szTmp, 10 );
			SetDlgItemText(hWndDlg, IDC_DB_SAVESIZE, szTmp );
		}
		else
			Debug( "LoadSnapShotMemory - MAJOR ERROR - SAVE of SNAPSHOT must have failed in some way " );
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// Memory Record Processing Routines
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


void CalculateMemoryGlobals( void )
{  // one pass thru will recalc the memory system pointers and globals and update debug window
	int iE=0, iCount=0, iLast=0;
	char szTmp[10];
	BOOL bFreeSlotFound = FALSE;

	for ( iE=1;iE<MAX_MEMORIES;iE++) {
		if (bFreeSlotFound==FALSE) {
			if (stMemory[iE].bEntryUsed==FALSE) {
				bFreeSlotFound=TRUE; 
				iCurrentFreeMemorySlot =iE;
			} // find first free slot
		}
		if ( stMemory[iE].bEntryUsed==TRUE) {
			iCount++; 
			iLast = iE;
		}
	}// for

    iNumberOfMemoryEntries = iCount;
	iLastMemoryEntry=iLast;
	 iPercentMemoryFilled = iCount/39; //(iCount*iMemorySaveSize)/39;

	 // update counter in infodriver
	 itoa( iCount, szTmp, 10 );
	ei.updateslot( MEMORY_COUNTER_SLOT, szTmp );  

	// update debug window	
	wsprintf( szTmp, "%d", iCurrentFreeMemorySlot );
	SetDlgItemText(hWndDlg, IDC_DB_FREESLOT, szTmp );
	wsprintf( szTmp, "%d", (iNumberOfMemoryEntries) );
	SetDlgItemText(hWndDlg, IDC_DB_RECORDS, szTmp );
	wsprintf( szTmp, "%d percent", iPercentMemoryFilled );
	SetDlgItemText(hWndDlg, IDC_DB_PERCENTUSED, szTmp );

	if (iNumberOfMemoryEntries >= 4001 ) {
		SetDlgItemText(hWndDlg, IDC_DB_PERCENTUSED, "* 100% *" );
		Debug( "WARNING *** Percentage loaded is  100% Memory IS FULL *** " );
	}
}


BOOL ValidMemoryRecord( int iEntry )
{  
	if (stMemory[iEntry].bEntryUsed==TRUE)
		return TRUE;
	else
		return FALSE;
}


void FindFreeMemorySlot( void )
{
	int iE=0;
	BOOL bFound = FALSE;
	char szTmp[10];

	do {
		iE++;
		if (stMemory[iE].bEntryUsed==FALSE) {
			bFound=TRUE;
		}
	}
	while ((bFound==FALSE)&&(iE<MAX_MEMORIES));
	iCurrentFreeMemorySlot=iE;
	if (iE<MAX_MEMORIES) {
		wsprintf( szTmp, "%d", iCurrentFreeMemorySlot );
		SetDlgItemText(hWndDlg, IDC_DB_FREESLOT, szTmp );
	}
	else {
		Debug( "FindFreeDbSlot > ERROR no free slot found " );
		SetDlgItemText(hWndDlg, IDC_DB_FREESLOT, "No FREE" );
	}
}


BOOL FindMemoryEntry( LPSTR szData, int  *iEntry )
{
	int iE=0, iResult=-1;
	BOOL bFound = FALSE;

	*iEntry =0;
	do {
		iE++;
		if (stMemory[iE].bEntryUsed==TRUE) { // is slot used
			iResult = stricmp( stMemory[iE].szMemoryName, szData );
			if( iResult==0 ) {  // string identical irrespective of case
				*iEntry = iE;
				bFound= TRUE;
			}
		} 
	}
	while ((bFound==FALSE)&&(iE<MAX_MEMORIES));
	return bFound;
}

BOOL FindNextMemoryEntry( LPSTR szData, int  *iEntry )
{ // searches from iEntry as a start point rather tahn  zero
	int iE, iResult=-1;

	iE = *iEntry;
	do {
		iE++;
		if (stMemory[iE].bEntryUsed==TRUE) { // is slot used
			iResult = stricmp( stMemory[iE].szMemoryName, szData );
			if( iResult==0 ) {  // string identical irrespective of case
				*iEntry = iE;
				return TRUE;
			}
		} 
	}
	while (iE<MAX_MEMORIES);
	*iEntry = 0;
	return FALSE;
}

BOOL FindRecallMemoryEntry( char szData[BUFFER_CONST], int  *iEntry,  BOOL *bSubset ,  BOOL *bDEFAULT ) // differs from above as this also searches for default
{
	int i,j, iE=0;
	int  ch = '|';
	char  *pdest, szList[BUFFER_CONST], szName[BUFFER_CONST], szTmp1[16], szTmp2[16];
	int result;

	*bSubset = FALSE;
	*bDEFAULT = FALSE;
	*iEntry = 0;

	// these other commands require pamareters - which should be delimited by a |
	pdest = strchr( szData, ch );
	result = pdest - szData;
	if( pdest != NULL ) {
		// | char found so there is a subset list to parse
		*bSubset = TRUE;
		// initialise global structure
		for (i=0;i<MAX_DEVICE_ENTRIES;i++ )	{
			stSubsetData[i].iDeviceNumber = 0;
			stSubsetData[i].bRangeDefinition = FALSE;
			for (j=0;j<MAX_DATA_ENTRIES;j++)
				stSubsetData[i].iList[j]=0;
		}
		// split string to get data list, use szData to return just the memory name
		strncpy( szName, szData, result);
		szName[result]=0; // terminate string
		strcpy( szList, szData+result+1 );
		strcpy( szData, szName );

		// now parse cdl for list of dests, gpis or infoslots to recall
		Debug( "FindRecallMem - %s - Subset list found %s", szData, szList );
		char  *tok;
		int  iTokCount=0 ;
		int  iDevCount = 0;
		int iListCount =0;
		BOOL bContinue = TRUE;
		tok=strtok(szList, ",");  //DEV=xxx,RANGE,dest,dest.... for each dev list the dests - range is optional
		while ( (tok)&&(bContinue) ) {
			iTokCount++;
			strcpy( szTmp1, tok );
			// check for dev
			//Debug(szTmp1);
			pdest = strstr( szTmp1, "DEV=" );
			result = pdest - szTmp1 + 4;
			if (pdest!=NULL) {
				iDevCount++;
				if (iDevCount<MAX_DEVICE_ENTRIES) {
					strcpy( szTmp2, szTmp1+result );
					iListCount =0;
					stSubsetData[iDevCount].iDeviceNumber = atoi(szTmp2);
					//Debug(szTmp2);
				}
				else
					bContinue=FALSE;
			}
			else {
				// check for range
				if (strcmp(szTmp1, "RANGE")==STRINGSEQUAL) {
					if (iDevCount<MAX_DEVICE_ENTRIES )
						stSubsetData[iDevCount].bRangeDefinition = TRUE;
					//Debug("Range true" );
				}
				else {
					// else must be dest etc
					if ((iDevCount<MAX_DEVICE_ENTRIES )&&(iListCount<MAX_DATA_ENTRIES)) {
						iListCount++;
						stSubsetData[iDevCount].iList[iListCount] = atoi(szTmp1);
						//Debug( "%d", stSubsetData[iDevCount].iList[iListCount] );
					}
					else
						Debug( "FindRecallMem - too many entries in data string %d", iTokCount );
				}
			}
			tok=strtok(NULL, ",");
		}	// while tok		

	} // if pdest

	// first look for default - if so return true and 0
	int iResult=-1;
	if (strcmp(szData, "DEFAULT")==STRINGSEQUAL) {
		*bDEFAULT = TRUE;
		return TRUE;
	}
	// not default so try and find it
	do {
		iE++;
		if (stMemory[iE].bEntryUsed==TRUE) { // is slot used
			iResult = stricmp( stMemory[iE].szMemoryName, szData );
			if( iResult==0 ) {  // string identical irrespective of case
				*iEntry = iE;
				return TRUE;
			}
		} 
	}
	while (iE<MAX_MEMORIES);
	return FALSE;
}



BOOL ValidRevertiveDevice( int iDevNumber, int *iDevRev )
{
	int iD=0;
	BOOL bFound = FALSE;

	*iDevRev=0;
	do {
		iD++;
		if (stDeviceRevertives[iD].iDeviceNumber==iDevNumber) {
			*iDevRev=iD;
			return TRUE;
		}
	}
	while ((bFound==FALSE)&&(iD<MAX_DEVICES_FOR_REVERTIVES));

	Debug( "ValidRevertiveDevice - ERROR no revertive device found for %d", iDevNumber );
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
/// Memory Commands Processing Routines
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GetNextMemoryEntry( int *iNextEntry ) 
{
	// get next entry set aside for memory save - if this save is longer than expected then findnext free slot
	BOOL bFound = FALSE;
	char szStr[BUFFER_CONST];
	int iPos = *iNextEntry, iResult=-1;

	strcpy ( szStr, stMemory[iPos].szMemoryName );
	do {
		iPos++;
		if (stMemory[iPos].bEntryUsed==TRUE) { // is slot used
			iResult = stricmp( stMemory[iPos].szMemoryName, szStr );
			if( iResult==0 ) {  // string identical irrespective of case
				*iNextEntry = iPos;
				return TRUE;
			}
		} 
	}
	while (iPos<MAX_MEMORIES);

	// got to end of memories and no further set up entries found - so find initial start point and find next free slot after this
	// reevaluate number of entries for this memory save
	int iE=0;
	char szTmp[10]; 
	if (FindMemoryEntry( szStr, &iE ) ) {
		// then find next available slot after initial entry - but needs to be after other mem slots allocated to this mem name else retieve will fail because of out of order
		do {
			iE++;
			if (stMemory[iE].bEntryUsed==FALSE) {
				stMemory[iE].bEntryUsed = TRUE;
				strcpy( stMemory[iE].szMemoryName, szStr );
				*iNextEntry = iE;
				stMemory[iE].iNumberOfMultipleSlotsUsed++; // inc multiple slot counter in start memory loc
				if ((stMemory[iE].iNumberOfMultipleSlotsUsed+1)>iMemorySaveSize) {
					iMemorySaveSize = stMemory[iE].iNumberOfMultipleSlotsUsed+1;
					itoa( iMemorySaveSize, szTmp, 10 );
					SetDlgItemText(hWndDlg, IDC_DB_SAVESIZE, szTmp );
				}
				return TRUE;
			}
		}
		while (iE<MAX_MEMORIES);
	}

	Debug( "GetNextMemoryEntry - part 2 fail - database find error - aborting save " );
	return FALSE;
}


void SaveActualDataString ( int iEntry, char szMemData[BUFFER_CONST] )
{
	int iDefMemType =1;
	// save string to infodriver and memory
	// new v1.3 - write into circ buffer
	Write_to_CircBuffer( iEntry, stMemory[iEntry].szMemoryName, szMemData ); // for database name and associated infoslot string

	// update appropriate database in device file
	///ei.setdbname( iDevice, stDefinedData[iDefMemType].iKeyNameDatabase, (iEntry+MEMORY_START-1), stMemory[iEntry].szMemoryName, FALSE );
	// update appropriate infodriver slot 
	///ei.setslot(  iEntry+MEMORY_START-1, szMemData );  // infodriver memories start at slot 81 - but 1 internally

	Debug("SaveRecordtoMemory > slot %d (%d) - %s %s ",  iEntry, iEntry+MEMORY_START-1, stMemory[iEntry].szMemoryName, szMemData );
	// now add to to internal memory - quicker for polls and recalls
	strcpy( stMemory[iEntry].szMemoryData, szMemData );

}

void SaveRecordToMemory( LPSTR szData, int  iSlotFrom )
{ 
// save record - extract memory name, 
	// test for memory name already - if so 
	//		then update 
	//		else save record to current free slot, then find next free slot

	// update existing record or add new one if not found - from editor panel will have a full set of data most likely -confimation string to slot +1
	// parse data into bits - like reading read routine and store overwriting current data

    char szTmp [BUFFER_CONST], szMemData [BUFFER_CONST], *tok;
	int iEntry=0, iDefMemType=1, iDevRev, iDst;
	int i,j, iTokCount = 0, iVal=0,  iNum=1, iC, iRangeStart, iRangeEnd;
	bool bContinue = TRUE, bNewRecordAdded=FALSE;

	// extract data for entry
	iTokCount = 0;
	iDefMemType=1;

	bContinue= TRUE;
	tok=strtok(szData, szDeLimiter );  // 1.56
	while ((tok)&&(bContinue)) {
		iTokCount++;
		iVal = atoi(tok);
		strcpy( szTmp, tok );
		switch(iTokCount) {
		case 1: // Memory Name
			if (FindMemoryEntry( szTmp, &iEntry ) ) {
				stMemory[iEntry].bEntryUsed = TRUE;
				strcpy( stMemory[iEntry].szMemoryName, szTmp );
				Debug( " saveParser1 > entry %swill start at or be %d ", szData, iEntry );
			}
			else {
				Debug ( "SaveRecordToMemory > As Record does not exisit for %s - finding free slot(s)", szTmp );
				// Find new slot and add record and continue 
				bNewRecordAdded = TRUE;
				// FindFreeMemorySlot();
				iEntry =  iCurrentFreeMemorySlot;
				if ((iEntry > 0)&&(iEntry < (MAX_MEMORIES-iMemorySaveSize))) { // but do not know if fragmentation will occur 
					// set up block of memories to save to - block size based on prev memory saves
					Debug( " saveParser2 > entry %s will commence at %d for %d slots",szData,  iEntry, iMemorySaveSize );
					i = 0;
					iC= 0;
					do {
						if (stMemory[iEntry+i].bEntryUsed == FALSE) {
							stMemory[iEntry+i].bEntryUsed = TRUE;
							strcpy( stMemory[iEntry+i].szMemoryName, szTmp );
							iC ++;
							i++;
							if (iMemorySaveSize>1) 
								stMemory[iEntry].iNumberOfMultipleSlotsUsed++; // inc multiple slot counter in start memory loc
						}
						else {
							Debug ( "SaveRecordToMemory > Finding FreeSlot was in use !!! trying next available record");
							i++;
						} // if
						if ((iEntry+i)>MAX_MEMORIES) { // test to prevent overflow of memory - if max reached abort save
							Debug ( "SaveRecordToMemory > NO FURTHER FREE SLOTS FOUND - DATABASE FULL !!! Save Failed");
							bContinue = FALSE;
							if (iSlotFrom>0) 
								ei.setslot(  iSlotFrom +1, "Memory saved  Failed - Memory Database FULL " );
							return;
						}
					} 
					while ( iC < iMemorySaveSize );
				}
				else {
					Debug ( "SaveRecordToMemory > INSUFFICENT SPACE so DATABASE FULL !!! Not enough room to save memory ");
					bContinue = FALSE;
					if (iSlotFrom>0) 
						ei.setslot(  iSlotFrom +1, "Memory saved  Failed - Memory Database FULL " );
					return;
				}
			}
		break;
		case 2: // memory type  or default to mem type 1  - for later version 
			// find type - if no type def to 1  set flag to say not found
			iDefMemType=1;
		break;
		} // switch
		tok=strtok(NULL, szDeLimiter); // 1.56				
	}	// while tok && bContinue

	// now work out current settings for  memory type and build string and write to slot
	// RM to update files
	if (bContinue) {

		strcpy( szMemData, "" );
		iC = 0;
		// go thru defined data structure and get dev number and dests etc and get current revs to build string to save
		for (i=1; i<=stDefinedData[iDefMemType].iDefinitionEntries;i++) {
			iDevRev=0;
			if ( ValidRevertiveDevice( stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, &iDevRev )) {

				// need to recode for RANGE  and multiple data saves
				if (stDefinedData[iDefMemType].stDefinitionData[i].bRangeDefinition==TRUE ) { // RANGE definition
					iRangeStart = stDefinedData[iDefMemType].stDefinitionData[i].iData[1];
					iRangeEnd = stDefinedData[iDefMemType].stDefinitionData[i].iData[2];
					for ( j=iRangeStart; j<=iRangeEnd; j++ ) {
						iDst = j;					
						if ((iDst>0)&&(iDst<MAX_DEVICE_SIZE)) {
							// now get revertive data string
							if (stDefinedData[iDefMemType].stDefinitionData[i].iDeviceType==INFO_MEMORY) {
								strcpy( szTmp, stDeviceRevertives[iDevRev].stRevertive[ iDst ].szStringData );
								//strcat( szTmp, "," ); // pre 1.39
							}
							else {
								//wsprintf( szTmp, "%d,",stDeviceRevertives[iDevRev].stRevertive[ iDst ].iNumericData ); pre 1.39
								wsprintf( szTmp, "%d",stDeviceRevertives[iDevRev].stRevertive[ iDst ].iNumericData );
							}
							// test for strlen - if too long then save and use next slot allocated for mem
							if ( ( (strlen(szMemData)+strlen(szTmp) )<=237) && (iC<(MAX_DATA_ENTRIES-2)) ) { // was buffer_const - but i think this may cause csi to crash
								//if (strlen(szMemData)>0) strcat( szMemData, "," ); // ie add comma delimiter - but not for the first entry
								if (strlen(szMemData)>0) strcat( szMemData, szDeLimiter ); // ver 1.56 add delimiter - but not for the first entry
								strcat( szMemData, szTmp );
								iC++;
							}
							else {
								SaveActualDataString( iEntry, szMemData );
								strcpy( szMemData, "" );
								strcat( szMemData, szTmp );
								iC=1;
								// get next iEntry if it exists 
								if (GetNextMemoryEntry( &iEntry ) == FALSE) { // should always really be true - major error if false
									Debug( "SaveRecordtoMemory - 1SAVE ABORTED - no more free entries - database full " );
									// clean up fragment ???
									if (iSlotFrom>0) 
										ei.setslot(  iSlotFrom +1, "Memory saved  Failed - Memory Database FULL " );
									return;
								}
							} // if strlen
						} // if iDst
						else {
							Debug( " SaveRecordtoMemory - 1SAVE ABORTED - unknown revertive dest %d for device %d ", iDst,stDeviceRevertives[iDevRev].iDeviceNumber );
							// clean up fragment ???
							if (iSlotFrom>0) 
								ei.setslot(  iSlotFrom +1, "Memory saved  Failed (error code 1)" );
								return;
						}

					} // for j
				}
				else {  // explicit list of dests etc
					iC = 0;
					for ( j=1;j<=stDefinedData[iDefMemType].stDefinitionData[i].iDataCount;j++) { 
						iDst = stDefinedData[iDefMemType].stDefinitionData[i].iData[j];				
						if ((iDst>0)&&(iDst<MAX_DEVICE_SIZE)) {
							// now get revertive data string
							if (stDefinedData[iDefMemType].stDefinitionData[i].iDeviceType==INFO_MEMORY) {
								strcpy( szTmp, stDeviceRevertives[iDevRev].stRevertive[ iDst ].szStringData );
								//strcat( szTmp, "," ); pre 1.39
							}
							else {
								wsprintf( szTmp, "%d",stDeviceRevertives[iDevRev].stRevertive[ iDst ].iNumericData );
								//wsprintf( szTmp, "%d,",stDeviceRevertives[iDevRev].stRevertive[ iDst ].iNumericData ); // pre 1.39
							}
							// test for strlen - if too long then save and use next slot allocated for mem
							if ( ( (strlen(szMemData)+strlen(szTmp) )<=237) && (iC<(MAX_DATA_ENTRIES-2)) ) { // was buffer_const - but i think this may cause csi to crash
								//if (strlen(szMemData)>0) strcat( szMemData, "," ); // ie add comma delimiter - but not for the first entry
								if (strlen(szMemData)>0) strcat( szMemData, szDeLimiter ); // ver 1.56 add delimiter - but not for the first entry
								strcat( szMemData, szTmp );
								iC++;
							}
							else {
								SaveActualDataString( iEntry, szMemData );
								strcpy( szMemData, "" );
								strcat( szMemData, szTmp );
								iC=1;
								// get next iEntry if it exists 
								if (GetNextMemoryEntry( &iEntry ) == FALSE) { // should always really be true - major error if false
									Debug( "SaveRecordtoMemory - 2SAVE ABORTED - no more free entries - database full " );
									// clean up fragment ???
									if (iSlotFrom>0) 
										ei.setslot(  iSlotFrom +1, "Memory saved  Failed - Memory Database FULL " );
									return;
								}
							} // if strlen
						} // if iDst
						else {
							Debug( " SaveRecordtoMemory - 2SAVE ABORTED - unknown revertive dest %d for device %d ", iDst,stDeviceRevertives[iDevRev].iDeviceNumber );
							if (iSlotFrom>0) 
								ei.setslot(  iSlotFrom +1, "Memory saved  Failed (error code 2)" );
							// clean up fragment ???
							return;
						}					
					
					} // for j
				}// if rangeDefintion
			}
			else {
				Debug( "SaveRecordtoMemory - SAVE ABORTED - unknown device %d ", stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber );
				// clean up fragment ???
				return;
			}
		} // for i

		// save last fragment - or single line if that is all memory takes up
		if (strlen( szMemData ) >= 1)
			SaveActualDataString( iEntry, szMemData );
		if (iSlotFrom>0) 
			ei.setslot(  iSlotFrom +1, "Memory saved  OK" );

		// update debug memory
		wsprintf( szTmp, "%d", iEntry );
		SetDlgItemText(hWndDlg, IDC_DB_REC_ENTRY2, szTmp );
		SetDlgItemText(hWndDlg, IDC_DB_REC_KEY2, stMemory[iEntry].szMemoryName );
		SetDlgItemText(hWndDlg, IDC_DB_STORED, stMemory[iEntry].szMemoryData );
		
	}

	// sort out globals and update debug window
	CalculateMemoryGlobals( );

}

//
// Function to get data from memory arrays and split into a string array in which each element can be processed in turn
// uses the global var stRecallMemData to store current line being processed
//
// returns the data element extracted and if it was sucessful or not
//
BOOL GetMemDataElement(  char szMemName[BUFFER_CONST], int *iEntry,  char szDataElement[ BUFFER_CONST]  ) 
{
	char szNewMem[BUFFER_CONST]= "", *tok;
	int iNewE=0, iTokCount = 0, i=0;
	BOOL bContinue = TRUE;


	strcpy( szDataElement, " " ); // reset var
	if ( stRecallMemData.iCurrentElement > stRecallMemData.iNumberOfElements ) {
		// initialise string data to blanks to clear out old data
		for (i=0;i<MAX_DATA_ENTRIES;i++ )
			strcpy ( stRecallMemData.MemData[i].szData, "" );

		// get next line of data from memory and split into element strings 
		iNewE = *iEntry;
		if (FindNextMemoryEntry( szMemName, &iNewE )==TRUE) {
			*iEntry = iNewE;
			strcpy( szNewMem, stMemory[iNewE].szMemoryData );
			//Debug( "GMDE - new mem %s ", szNewMem );
			// parse current data string 
			iTokCount = 0;
			bContinue= TRUE;
			tok=strtok(szNewMem, szDeLimiter);  // ver 1.56
			while ((tok)&&(bContinue)) {
				iTokCount++;
				if (iTokCount<MAX_DATA_ENTRIES)
					strcpy( stRecallMemData.MemData[iTokCount].szData, tok);
				else {
					bContinue=FALSE;
					Debug( "GetMemDataElement - got to MAX_DATA_ENTRIES before finishing parse - data may be lost " );
				}
				tok=strtok(NULL, szDeLimiter); // ver 1.56		
			} // while tok
		}
		else { // no further memory entries for this memory name
			return FALSE;
			Debug( "GetMemDataElement - no further memory entries for %s ", szMemName  );
		}
		// reset vars
		stRecallMemData.iCurrentElement = 1;
		stRecallMemData.iNumberOfElements = iTokCount; 
	}

	// get next piece of data
	if ( stRecallMemData.iCurrentElement <= stRecallMemData.iNumberOfElements ) {
		// pass back data element
		strcpy( szDataElement, stRecallMemData.MemData[stRecallMemData.iCurrentElement].szData );
		//Debug( "GMDE - 2 - %d %s ", stRecallMemData.iCurrentElement, szDataElement );
		stRecallMemData.iCurrentElement++;
		return TRUE;
	}
	else {
		return FALSE;
		Debug( "GetMemDataElement - no further data elements" );
	}
}

//
// Function to check if memory data to be used or not
// data always used if bSubset is FALSE, else checked that it is included in list - true if so
//
// returns true or false
//
BOOL DataElementToBeIncluded( int iDev, int iDest, BOOL bSubset )
{
	if (bSubset==TRUE) {
		for (int i=1; i < MAX_DEVICE_ENTRIES; i++) {
			if ( stSubsetData[i].iDeviceNumber==iDev) {
				if (stSubsetData[i].bRangeDefinition == TRUE) {
					if ((iDest>=stSubsetData[i].iList[1])&&(iDest<=stSubsetData[i].iList[2]))
						return TRUE; // in range so TRUE
				}
				else {
					for (int j = 1; j<MAX_DATA_ENTRIES; j++) {
						if (iDest==stSubsetData[i].iList[j]) 
							return TRUE; // entry found in list so TRUE
						else if (stSubsetData[i].iList[j]==0) 
							j=MAX_DATA_ENTRIES; // shortcircuit loop for speed - 0 means entry will not be found
					} // for j
				}
			}
		} // for i
		// fall out of loop of subset devices - so return FALSE here
		return FALSE;
	} // if bSubset
	return TRUE; // data element to be output
}


void 	RecallRecordFromMemory( LPSTR szData, int iSlotFrom, BOOL bActionSettings  )
{
	// recall memory and send to panel, also if action == TRUE - execute crosspoints and gpi settings
	// i.e. POLL memory will only send revertive back to panel - action is false for POLL
	// if the memory as multiple entries it will not be returned as revertive to a POLL - so dont do a poll from panels

	// ver 1.38 modified to allow subsets to be recalled or whole memory

	int i=0, j=0,  iEntry = 0, iDefMemType=1;
    char  szElement [BUFFER_CONST],  szTmp[30];
	char szS1[BUFFER_CONST], szS2[BUFFER_CONST];
	BOOL bSubsetMemory = FALSE;
	BOOL bDefaultMemory = FALSE;

	if (FindRecallMemoryEntry( szData, &iEntry, &bSubsetMemory, &bDefaultMemory) ) {
		// recall a previously stored memory 
		OutputDebugString("RecallRecordFromMemory 1");
		if (bActionSettings==TRUE) {
		//  send rc , gs, iw  as appropriate
			iDefMemType = stMemory[iEntry].iWhichDefintionEntry; // which will be 1 - as only 1 type per mem app at present
			// set up global var to be used thru data processing in the for loops below
			stRecallMemData.iNumberOfElements = 0;
			stRecallMemData.iCurrentElement = 1; // this will force the getMemDataElement to get data from memory
			stRecallMemData.iStartMemoryEntry = iEntry;
			iEntry--; // as iEntry ++ is done first in getMemDataElement inside loops - just the way it is done
			strcpy( szElement, ""  );

			for (i=1; i<=stDefinedData[iDefMemType].iDefinitionEntries; i++) {
				// test for RANGE data style or Specific defined data
				OutputDebugString("RecallRecordFromMemory 2");
				if (stDefinedData[iDefMemType].stDefinitionData[i].bRangeDefinition==TRUE) { 
					OutputDebugString("RecallRecordFromMemory 2A");

					if (bDefaultMemory == TRUE ) { // get only DEFAULT memory data here
						if (GetMemDataElement(  szData, &iEntry, szElement )==FALSE ) { /// XXXX MAJOR ERROR in recalling data for ranges other than default
							Debug( "RecallRecordFromMemory - def MAJOR ERROR - insufficent data stored for Range Memory for DEFAULT ");
							wsprintf( szTmp, "%d", iEntry );
							SetDlgItemText(hWndDlg, IDC_DB_REC_ENTRY, szTmp );
							SetDlgItemText(hWndDlg, IDC_DB_REC_KEY, stMemory[iEntry].szMemoryName );
							SetDlgItemText(hWndDlg, IDC_DB_RETRIEVED, "**** insufficent data *****" );
							return; // exit early from recall
						}
						//Debug( "0-getmemdataelement %d %s ", iEntry, szElement );
					}

					for ( j=stDefinedData[iDefMemType].stDefinitionData[i].iData[1]; j<=stDefinedData[iDefMemType].stDefinitionData[i].iData[2]; j++ ) {     /// XXXXX MAJOR ERROR HERE in the szElement

						if (bDefaultMemory == FALSE ) { // get data here for memories other than DEFAULT
							if (GetMemDataElement(  szData, &iEntry, szElement )==FALSE ) {
								Debug( "RecallRecordFromMemory - oth MAJOR ERROR - insufficent data for Range Memory for %s ", szData);
								wsprintf( szTmp, "%d", iEntry );
								SetDlgItemText(hWndDlg, IDC_DB_REC_ENTRY, szTmp );
								SetDlgItemText(hWndDlg, IDC_DB_REC_KEY, stMemory[iEntry].szMemoryName );
								SetDlgItemText(hWndDlg, IDC_DB_RETRIEVED, "**** insufficent data *****" );
								return; // exit early from recall
							}
						}
						// test for subset here
						//Debug( "1-getmemdataelement %d %s ", iEntry, szElement );
						if (DataElementToBeIncluded( stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, j, bSubsetMemory )) {

							// process data - kept as code here so easy exit from loops if data error
							switch( stDefinedData[iDefMemType].stDefinitionData[i].iDeviceType ) {
							case ROUTER_MEMORY: // rtr
								if (atoi( szElement )>0 ) {
									wsprintf( szTmp, "RC %d %s %d", stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, szElement, j );
									//Debug( " 1- rc command %d %s %d", stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, szElement, j );
									// now send to  Fifo  buffer 
									Write_to_FifoBuffer ( ROUTER_MEMORY, szTmp );
								}
								//else 
								//	Debug( "Recall of router src is 0" );
							break;	
							case GPI_MEMORY : // gpi
								wsprintf( szTmp, "GS %d %s %d", stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, szElement,j );
								// now send to  Fifo  buffer 
								Write_to_FifoBuffer ( GPI_MEMORY, szTmp );
							break;
							case INFO_MEMORY : // info slots
								wsprintf( szTmp, "IW %d '%s' %d", stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, szElement, j );
								// now send to  Fifo  buffer 
								Write_to_FifoBuffer ( INFO_MEMORY, szTmp );
							break;
							} // switch

						} // dataElement2BIncluded

					} // for j
					// move pointer on 1 in global stRecall structure for range - just to keep in proper step - as range has 2 parameters both the same i know
					if (bDefaultMemory == TRUE ) stRecallMemData.iCurrentElement++;  // if statement part added in 1.39

				} // if range
				else { // set for specific entries stored
					OutputDebugString("RecallRecordFromMemory 2B");
					for (j=1;j<=stDefinedData[iDefMemType].stDefinitionData[i].iDataCount;j++ ) {

						if (GetMemDataElement(  szData, &iEntry, szElement ) ) {
							//Debug( "2-getmemdataelement %d %s ", iEntry, szElement );

							// test for subset here
							if (DataElementToBeIncluded( stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, 
								stDefinedData[iDefMemType].stDefinitionData[i].iData[j], bSubsetMemory )) {
								OutputDebugString("RecallRecordFromMemory 3B");

								// process data - kept as code here so easy exit from loops if data error
								switch( stDefinedData[iDefMemType].stDefinitionData[i].iDeviceType ) {
									case ROUTER_MEMORY: // rtr
										if (atoi( szElement )>0 ) {
											wsprintf( szTmp, "RC %d %s %d", stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, 
												szElement,	stDefinedData[iDefMemType].stDefinitionData[i].iData[j] );
											//Debug( " 2- rc command %d %s %d", stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, 
												//szElement,	stDefinedData[iDefMemType].stDefinitionData[i].iData[j] );
											// now send to  Fifo  buffer 
											Write_to_FifoBuffer ( ROUTER_MEMORY, szTmp );
										}
										//else 
										//	Debug( "Recall of router src is 0" );
									break;	
									case GPI_MEMORY : // gpi
										wsprintf( szTmp, "GS %d %s %d", stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, 
											szElement,	stDefinedData[iDefMemType].stDefinitionData[i].iData[j] );
										// now send to  Fifo  buffer 
										Write_to_FifoBuffer ( GPI_MEMORY, szTmp );
									break;
									case INFO_MEMORY : // info slots
										wsprintf( szTmp, "IW %d ' %s' %d", stDefinedData[iDefMemType].stDefinitionData[i].iDeviceNumber, 
											szElement, stDefinedData[iDefMemType].stDefinitionData[i].iData[j] );
										// now send to  Fifo  buffer 
										Write_to_FifoBuffer ( INFO_MEMORY, szTmp );
									break;
								} // switch

							} // dataElement2BIncluded

						} // if GetMemDataElement
						else {
							Debug( "RecallRecordFromMemory - spfic MAJOR ERROR - insufficent data stored for specific Memory for %s ", szData);
							wsprintf( szTmp, "%d", iEntry );
							SetDlgItemText(hWndDlg, IDC_DB_REC_ENTRY, szTmp );
							SetDlgItemText(hWndDlg, IDC_DB_REC_KEY, stMemory[iEntry].szMemoryName );
							SetDlgItemText(hWndDlg, IDC_DB_RETRIEVED, " !!!! insufficent data !!!! " );
							if (iSlotFrom>0 )
								ei.setslot( iSlotFrom+1, "Memory only partially recalled  - insufficent data"  );
							return; // exit early from recall
						}

					} // for j
				} // if range or specific data defined
			} // for i
		} // if bActionSettings
		strcpy( szS1, stMemory[(stRecallMemData.iStartMemoryEntry)].szMemoryData );
		if ( (strlen(szS1)+strlen(szData)+2) > BUFFER_CONST ) {
			szS1[BUFFER_CONST-strlen(szData)-2] = NULL; // ie terminate string early 
		}

		if (iSlotFrom>0 )  { // slot = 0 if initiated from within auto - else a panel initiated - send single line of data only - multiple mem locs cannot be returned 
			wsprintf( szS2, "Memory %s recalled .... Note - it WILL take several minutes for this action to complete", szData );
			ei.setslot( iSlotFrom+1, szS2 );
			ei.setslot( iSlotFrom+2, "1" ); // added feature that sets flag whilst recalling etc -- so panel can disable buttons etc
			bCurrentlyRecalling = TRUE;
		}
		// update debug window
		wsprintf( szTmp, "%d", iEntry );
		SetDlgItemText(hWndDlg, IDC_DB_REC_ENTRY, szTmp );
		SetDlgItemText(hWndDlg, IDC_DB_REC_KEY, szData );
		SetDlgItemText(hWndDlg, IDC_DB_RETRIEVED, szS1 );

	} // if FindRecallMemoryEntry
	else {
		Debug( "RecallRecordFromMemory - No data for memory called %s ", szData );
		if (iSlotFrom>0 ) {
			ei.setslot( iSlotFrom+1, "Memory failed to be recalled  - - no entry found"  );
			ei.setslot( iSlotFrom+2, "0" ); // slot 3 etc
			bCurrentlyRecalling = FALSE;
		}
	}
}
	

void 	DeleteRecordFromMemory( LPSTR szData, int iSlotFrom )
{
	// find entry - and delete it - recalc nextfree slot, update debug window
	int iEntry, iDefMemType=1, iCount=0;
    char  szTmp[128];

	iEntry = 0;
	if ( (FindSnapShot(szData)==FALSE)&&(FindDefault(szData)==FALSE) ) {
		Debug("DeleteRecordFromMemory - starting to delete %s", szData );
		while (FindNextMemoryEntry( szData, &iEntry )==TRUE ) {
			iCount++;
			strcpy( stMemory[iEntry].szMemoryName, " " );
			strcpy( stMemory[iEntry].szMemoryData, " " );
			stMemory[iEntry].bEntryUsed = FALSE;

			// write to device database removing the name
			wsprintf( szTmp, "%04d", (iEntry+MEMORY_START-1) );
			Write_to_CircBuffer( iEntry, szTmp, " " ); // for database name and associated infoslot string
			
			//ei.setdbname( iDevice, stDefinedData[iDefMemType].iKeyNameDatabase, (iEntry+MEMORY_START-1), szTmp, FALSE );
			//ei.setslot(  (iEntry+MEMORY_START-1), " " );  // note infodriver memories start at 101 - but 1 internally
			Debug( "DeleteRecordFromMemory - Record deleted for %s, was at %d ", szData, iEntry );

		}
		// check thru memories and update debug window
		CalculateMemoryGlobals( );

		// now return ans  via iSlotfrom + 1
		wsprintf( szTmp, " Memory %s deleted", szData );
		ei.setslot( iSlotFrom+1, szTmp );

		// debug on iCount
		Debug("DeleteRecordFromMemory - ending - %d records deleted ", iCount );
	}
	else
		Debug( "DeleteRecord - cannot delete SNAPSHOT or DEFAULT memory ");

}

void SnapShotRecordToMemory( int iSlotFrom )
{
	// use save function to save to SNAPSHOT memory already stored
	SaveRecordToMemory(  "SNAPSHOT",  iSlotFrom );

}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// FUNCTION FindRTR ()
bool FindRTR( char szStr[BUFFER_CONST] )
{
	if (strcmp(szStr, "RTR")==STRINGSEQUAL)
		return TRUE;
	else
		return FALSE;
}

// FUNCTION FindGPI ()
bool FindGPI( char szStr[BUFFER_CONST] )
{
	if (strcmp(szStr, "GPI")==STRINGSEQUAL)
		return TRUE;
	else
		return FALSE;
}

// FUNCTION FindSave ()
bool FindSave( char szStr[BUFFER_CONST] )
{
	if (strcmp(szStr, "SAVE")==STRINGSEQUAL)
		return TRUE;
	else
		return FALSE;
}

// FUNCTION FindRecall ()
bool FindRecall( char szStr[BUFFER_CONST] )
{
	if (strcmp(szStr, "RECALL")==STRINGSEQUAL)
		return TRUE;
	else
		return FALSE;
}

// FUNCTION FindPOLL ()
bool FindPoll( char szStr[BUFFER_CONST] )
{
	if (strcmp(szStr, "POLL")==STRINGSEQUAL)
		return TRUE;
	else
		return FALSE;
}

// FUNCTION FindSnapShot ()
bool FindSnapShot( char szStr[BUFFER_CONST] )
{
	if (strcmp(szStr, "SNAPSHOT")==STRINGSEQUAL)
		return TRUE;
	else
		return FALSE;
}

// FUNCTION FindDefault ()
bool FindDefault( char szStr[BUFFER_CONST] )
{
	if (strcmp(szStr, "DEFAULT")==STRINGSEQUAL)
		return TRUE;
	else
		return FALSE;
}

// FUNCTION FindDelete ()
bool FindDelete( char szStr[BUFFER_CONST] )
{
	if (strcmp(szStr, "DELETE")==STRINGSEQUAL)
		return TRUE;
	else
		return FALSE;
}


	
BOOL ParseIncomingData( LPCSTR szSlot, int *iCommandCode,  LPSTR szData ) 
{
	char szStr[BUFFER_CONST], szCommand[BUFFER_CONST];
	int  ch = '|';
	char *pdest;
	int result;

	*iCommandCode = 0;
	strcpy ( szData, " " );
	strcpy ( szCommand, " " );

	strcpy( szStr, szSlot );

	// these other commands require pamareters - which should be delimited by a |
	pdest = strchr( szStr, ch );
	result = pdest - szStr;
	if( pdest != NULL ) {
		// extract command string
		strncpy( szCommand, szStr, result);
		// extract remaining data minus command
		strcpy( szData, szStr+result+1 );
		szCommand[result]=0;
		if (FindSave( szCommand )) {
			*iCommandCode = SAVE_RECORD;
			return TRUE;
		}
		else if (FindRecall( szCommand )) {
			*iCommandCode = RECALL_RECORD;
			return TRUE;
		}
		else if (FindDelete( szCommand )) {
			*iCommandCode = DELETE_RECORD;
			return TRUE;
		}
		else if (FindPoll( szCommand )) {
			*iCommandCode = POLL_RECORD;
			return TRUE;
		}
		else
			Debug( "ParseIncomingData - unknown command %s from slotcontents %s ", szCommand, szSlot );
	}
	else { // these commands do not require a vertical | - namely SNAPSHOT command
		if (FindSnapShot( szStr )) {
			*iCommandCode = SNAPSHOT_RECORD;
			return TRUE;
		}
		else {
			Debug( "ParseIncomingData > Invalid commandStr - no vertical bar%s", szStr );
			return FALSE; // no divider found
		}
	}
	return FALSE;
}

int CalculateRandomOffset()
{
	// used to create a time offset before calling TXRX function - to stop multiInfodriver tx clash
	int iRand1 = 0;
	srand(time(0)); // generate random seed point
	iRand1 = (rand() % 2007);
	return (iRand1);
}


//
//  FUNCTION: InfoNotify()
//
//  PURPOSE: Callback function from an infodriver, notifying event
//
//  COMMENTS: the pointer pex references the infodriver which is notifying
//			  iSlot and szSlot are provided for convenience
//			
void InfoNotify(extinfo* pex,UINT iSlot,LPCSTR szSlot)
{
	//char szTmp[64];

	switch (pex->iStatus) {

		case CONNECTED: // this is the "normal" situation
			UpdateCounters();
			if (!SlotChange(pex->iDevice,iSlot,szSlot)) 
				pex->setslot(iSlot,szSlot);
		break;

		case DISCONNECTED:
			Debug("Infodriver %d has disconnected", pex->iDevice);
			PostMessage( hWndMain, WM_CLOSE, 0, 0 );
			return;
		break;
				
		case TO_RXONLY:
			if (bShowAllDebugMessages) Debug("Infodriver %d received request to go RX Only", pex->iDevice);
			if (iOverallTXRXModeStatus == INFO_TXRXMODE) {
				// need to bounce this request by reasserting this txrx
				ei.setmode(INFO_TXRXMODE);
				ei.requestmode = INFO_TXRXMODE;
				iOverallModeChangedOver = 3;
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
				ei.updateslot(COMM_STATUS_SLOT, "1");	 // ok
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
		// Slots 1 -72 are reserved for outgoing data for panels displaying caller info
		//     where 1 is command in, 2 is answer back, 3 is assigned for recall / action status
	    // slot 79 is Comms slot 
		// Slots 80 reserved for number of memories 
		// Slots 81 - 85 or so - reserved for default and snapshot memories 
		// Slots up to  4096 - memory data

	int iCommandCode;
	char szData[BUFFER_CONST];

	Debug("Slot change function from device %d - slot %d has changed to '%s'",iDevice,iSlot,szSlot);
	switch (iSlot) {

	case 1: 	case 11: 	case 21: 	case 31:	case 41: 
	case 51:  case 61:
		// parse incoming string - a CDL list with command as first parameter 
		if (ParseIncomingData( szSlot, &iCommandCode, szData ) ==TRUE) {
			switch ( iCommandCode ) {
			case  SAVE_RECORD:
				SaveRecordToMemory( szData, iSlot ); // add or update existing memory
				break;

			case  RECALL_RECORD:
				RecallRecordFromMemory( szData, iSlot, TRUE ); // send data ro panel and set crosspoints and gpis
				break;

			case  POLL_RECORD:
				RecallRecordFromMemory( szData, iSlot, FALSE ); // send data to panel only - no crospoints or gpis set
				break;

			case  DELETE_RECORD:
				DeleteRecordFromMemory( szData, iSlot );
				break;

			case  SNAPSHOT_RECORD:
				SnapShotRecordToMemory( iSlot );
				break;
			}
		}
		else 
			Debug( "SlotChange > Parse Data Failed %s", szSlot );
		break;

	default:
		Debug("Slot change IGNORED  from device %d - slot %d has changed to '%s'",iDevice,iSlot,szSlot);
		return FALSE;
		}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CSI Client functions - process reveritves from phone drivers
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////


BOOL ValidDevice( int iDevNum, int *iWhichDeviceEntry ) 
{
	int i;

	*iWhichDeviceEntry = 0;
	for (i=1;i<MAX_DEVICES_FOR_REVERTIVES;i++) {
		if (stDeviceRevertives[i].iDeviceNumber==iDevNum) {
			*iWhichDeviceEntry = i;
			return TRUE;
		}
	}	
	return FALSE;
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

//
//  FUNCTION: CSIClientNotify()
//
//  PURPOSE: Callback function for incoming CSI messages for client class
//
//  COMMENTS: the message is supplied in szMsg
LRESULT CSIClientNotify(extclient* pec,LPCSTR szMsg)
{
UINT iParamCount = pec->getparamcount();
LPSTR szParamRev = pec->getparam(0);
LPSTR szDevNum = pec->getparam(1);
LPSTR szSlot = pec->getparam(2);
LPSTR szIndex = pec->getparam(3);
LPSTR szContent = pec->getparam(4);

char szTmp[256] = "";
//char szCommand[256];

int iDevNum = atoi( szDevNum );

// infodriver revs 
int iSlotNum = atoi( szSlot );
int iIndexNum = atoi( szIndex );

// router revs
int iRtrDest = atoi( szSlot );
int iRtrSource = atoi( szIndex );

// gpi revs
int iWhichGpi = atoi( szSlot );
int iGPIState = atoi( szIndex );

int  iI=0, iWhichDeviceEntry;

switch (pec->getstate())
	{
	case REVTYPE_R:
		//Debug ("R Revertive message %s",szMsg);
		UpdateCounters();
		// comment out these debug lines in release version 
		//Debug ("RTR Revertive message string %s",szMsg);
		//for (i=0;i<pec->getparamcount();Debug("Param %d is %s",++i,pec->getparam(i)));

		if (iParamCount==5) {
			if (ValidDevice( iDevNum, &iWhichDeviceEntry )==TRUE) {
				stDeviceRevertives[iWhichDeviceEntry].stRevertive[iRtrDest].iNumericData = iRtrSource;
			}
			else 
				Debug( "ERROR in RTR revertives - unexpected device %d ", iDevNum );			
		}
		else
			Debug ( "ERROR in Rtr revertives parameter count, %d, %s", iParamCount, szMsg );
	break;

	case REVTYPE_I:
		//Debug ("I Revertive message %s",szMsg);
		UpdateCounters();
		// comment out these debug lines in release version 
		//Debug ("RTR Revertive message string %s",szMsg);
		//for (i=0;i<pec->getparamcount();Debug("Param %d is %s",++i,pec->getparam(i)));

		if (iParamCount==5) {
			if (ValidDevice( iDevNum, &iWhichDeviceEntry )==TRUE) {
				StripOutQuotes( szContent );
				// store astring of infodriver and parse / use it as needed
				if (strlen( szContent ) == 0) {
					strcpy( stDeviceRevertives[iWhichDeviceEntry].stRevertive[iSlotNum].szStringData, " " );				
				}
				else
					strcpy( stDeviceRevertives[iWhichDeviceEntry].stRevertive[iSlotNum].szStringData, szContent );				
				stDeviceRevertives[iWhichDeviceEntry].stRevertive[iSlotNum].iNumericData = atoi( szContent );
			}
			else 
				Debug( "ERROR in INF revertives - unexpected device %d ", iDevNum );			
		}
		else
			Debug ( "ERROR in INF revertives parameter count, %d, %s", iParamCount, szMsg );
	break;

	case REVTYPE_G:
		//Debug ("G Revertive message %s",szMsg);
		if (iParamCount==5) {
			if (ValidDevice( iDevNum, &iWhichDeviceEntry )==TRUE) {
				stDeviceRevertives[iWhichDeviceEntry].stRevertive[iWhichGpi].iNumericData = iGPIState;
			}
			else 
				Debug( "ERROR in GPI  revertives - unexpected device %d ", iDevNum );			
		}
		else
			Debug ( "ERROR in GPI revertives parameter count, %d, %s", iParamCount, szMsg );
		break;

	case DATABASECHANGE:
		//Debug ("Memory change message - %s",szMsg);
		break;

	case STATUS:
		Debug("Status message is %s",szMsg);
		break;

	case DISCONNECTED:
		Debug("CSI has closed down");
		DestroyWindow(hWndMain);
		break;

	}

/* always return TRUE so CSI doesn't delete your client registration */
return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// FUNCTION PollAutomaticDrivers();
//
// to query all drivers to get this automatic up to speed - at startup
// XXX to do - to break up polls into smaller chunks - using the fifo buffer
void PollAutomaticDrivers(void)
{
	char  szCommand[256];
	int i, iC=0, iS=1, iE=256;

	Debug( "into PollAutomaticDrivers " );
	for (i=1;i<MAX_DEVICES_FOR_REVERTIVES;i++) {
		if ((stDeviceRevertives[i].iDeviceNumber>0)&&(stDeviceRevertives[i].iDeviceSize>0)) {
			iC=0; iS=1; iE=1; // reset vars
			switch( stDeviceRevertives[i].iDeviceType) {
			case 1 : // rtr
				do {
					iC ++;
					iS = iE; // previous end pont
					iE = iC*256;
					if (iE <  stDeviceRevertives[i].iDeviceSize) {
						wsprintf( szCommand, "RP %d %d %d",stDeviceRevertives[i].iDeviceNumber, iS, iE );
						Write_to_FifoBuffer( ROUTER_MEMORY, szCommand );
					}
					else {
						wsprintf( szCommand, "RP %d %d %d",stDeviceRevertives[i].iDeviceNumber, iS, stDeviceRevertives[i].iDeviceSize );
						Write_to_FifoBuffer( ROUTER_MEMORY, szCommand );
					}
				} // do
				while (iE <=  stDeviceRevertives[i].iDeviceSize );
				// version 1.3 - add commands to FIFO buffer for phased release
				Debug( "PollAutomaticDrivers - Router poll for device %d, slots %d added to circ buffer", stDeviceRevertives[i].iDeviceNumber, stDeviceRevertives[i].iDeviceSize );
			break;
			case 2: // gpi
				do {
					iC ++;
					iS = iE; // previous end pont
					iE = iC*256;
					if (iE <  stDeviceRevertives[i].iDeviceSize) {
						wsprintf( szCommand, "GP %d %d %d",stDeviceRevertives[i].iDeviceNumber, iS, iE );
						Write_to_FifoBuffer( GPI_MEMORY, szCommand );
					}
					else {
						wsprintf( szCommand, "GP %d %d %d",stDeviceRevertives[i].iDeviceNumber, iS, stDeviceRevertives[i].iDeviceSize );
						Write_to_FifoBuffer( GPI_MEMORY, szCommand );
					}
				} // do
				while (iE <=  stDeviceRevertives[i].iDeviceSize );
				// version 1.3 - add commands to FIFO buffer for phased release
				Debug( "PollAutomaticDrivers - GPI poll for device %d, slots %d  added to circ buffer", stDeviceRevertives[i].iDeviceNumber, stDeviceRevertives[i].iDeviceSize );
				break;
			case 3: // inf
				do {
					iC ++;
					iS = iE; // previous end pont
					iE = iC*256;
					if (iE <  stDeviceRevertives[i].iDeviceSize) {
						wsprintf( szCommand, "IP %d %d %d",stDeviceRevertives[i].iDeviceNumber, iS, iE );
						Write_to_FifoBuffer( INFO_MEMORY, szCommand );
					}
					else {
						wsprintf( szCommand, "IP %d %d %d",stDeviceRevertives[i].iDeviceNumber, iS, stDeviceRevertives[i].iDeviceSize );
						Write_to_FifoBuffer( INFO_MEMORY, szCommand );
					}
				} // do
				while (iE <=  stDeviceRevertives[i].iDeviceSize );
				// version 1.3 - add commands to FIFO buffer for phased release
				Debug( "PollAutomaticDrivers - INFO poll for device %d, slots %d  added to circ buffer", stDeviceRevertives[i].iDeviceNumber, stDeviceRevertives[i].iDeviceSize );

				break;
			} // switch
		} // if
	}	// for
	// now set off circ buffer timer ?
}
	
//  FUNCTION: AutomaticRegistrations()
//
//  PURPOSE:register with glensound drivers for incoming CLIs, and call states
//
//  COMMENTS: 
//			
BOOL AutomaticRegistrations(void)
{
	int i,j,iRev;
	char szFilename[32];

	// go thru defined data types and register with devices - need to find out router sizes for registration
	Debug( "into AutoRegistrations " );
	iRev=0;
	for ( i=1; i<MAX_PREDEFINED_MEMORY_TYPES; i++ ) {
		if (stDefinedData[i].bDefinitionUsed==TRUE) {
			for ( j =1;j<=stDefinedData[i].iDefinitionEntries; j++ ) {
				iRev++;
				if (iRev<MAX_DEVICES_FOR_REVERTIVES) {
					stDeviceRevertives[iRev].iDeviceType = stDefinedData[i].stDefinitionData[j].iDeviceType;
					stDeviceRevertives[iRev].iDeviceNumber = stDefinedData[i].stDefinitionData[j].iDeviceNumber;
					// get device size and register
					sprintf( szFilename, "dev_%03d.ini", stDeviceRevertives[iRev].iDeviceNumber);
					stDeviceRevertives[iRev].iDeviceSize = atoi( r_p( szFilename, "Database", "DatabaseSize_1", "0", FALSE ) );
					if (stDeviceRevertives[iRev].iDeviceSize>0) {
						switch ( stDeviceRevertives[iRev].iDeviceType ) {
						case ROUTER_MEMORY : case GPI_MEMORY :
							Debug( "AutoReg: %s %d ", szFilename, stDeviceRevertives[iRev].iDeviceSize );
							ec.regtallyrange( stDeviceRevertives[iRev].iDeviceNumber,1, stDeviceRevertives[iRev].iDeviceSize, INSERT ); 
							Debug( "AutomaticRegistrations - register for dev %d  max dest %d", stDeviceRevertives[iRev].iDeviceNumber, stDeviceRevertives[iRev].iDeviceSize );
						break;
						case INFO_MEMORY:
							int iR = stDefinedData[i].stDefinitionData[j].iData[stDefinedData[i].stDefinitionData[j].iDataCount];
							Debug( "AutoReg: %s %d ", szFilename, iR );
							ec.regtallyrange( stDeviceRevertives[iRev].iDeviceNumber,1, iR , INSERT );
							stDeviceRevertives[iRev].iDeviceSize = iR;
							Debug( "AutomaticRegistrations - register for dev %d  max dest %d", stDeviceRevertives[iRev].iDeviceNumber,iR );
						break;
						} // switch
					}
					else
						Debug( "AutomaticRegistrations - ERROR maxdest 0 for dev %d ", stDeviceRevertives[iRev].iDeviceNumber );
				}
				else 
					Debug( "AutomaticRegistrations - ERROR- too many registration devices" );
			} // for j
		}  // if true
		else 
			Debug( "AutoReg : defined is false for %s %d ", stDefinedData[i].szTypeName, i );
	} // for i
	Debug( "auto reg %d ", iRev );

	return TRUE;
}



///////////////////////////////////////////////////////////////////////////////////////////
//
//  Infodriver Resilience
//
///////////////////////////////////////////////////////////////////////////////////////////
// 
//  Check infodriver status for resilience
//
// 
void CheckAndSetInfoResilience(void)
{

	// having connected to driver now then getmode for redundancy
	// get state of infodriver
	char szNM[32] = "", szCM[32] = "", szStr[64] = " ??unknown??";
	int iMode = ei.getmode(); // main info determines the status of all others -- to prevent mix occuring
	int iReq = ei.requestmode; // main info determines the status of all others -- to prevent mix occuring
	if (bShowAllDebugMessages) Debug("CheckInfoMode- mode %d req %d ", iMode, iReq);
	iOverallStateJustChanged = 0;

	// determine string for driver dialog window
	if ((iMode == INFO_RXMODE) || (iMode == IFMODE_TXRXINQ)) { // if still in rx
		strcpy(szStr, "RXonly - RESERVE");
		strcpy(szNM, "RXONLY");
	}
	if (iMode == INFO_TXRXMODE){
		strcpy(szStr, "Tx/Rx - MAIN OK");
		strcpy(szNM, "TX-RX");
	}

	if (iMode != iOverallTXRXModeStatus) {
		if (iOverallTXRXModeStatus == INFO_RXMODE) strcpy(szCM, "RXONLY");
		if (iOverallTXRXModeStatus == INFO_TXRXMODE) strcpy(szCM, "TX-RX");
		Debug("CheckInfoMode- new mode %s(%d) different to current %s(%d) -- changing status", szNM, iMode, szCM, iOverallTXRXModeStatus);
		switch (iMode) {
		case INFO_RXMODE: case IFMODE_TXRXINQ:
			strcpy(szStr, "RXonly - RESERVE");
			iOverallModeChangedOver = 5;
			iOverallTXRXModeStatus = INFO_RXMODE;
			if (ei.getmode() != INFO_RXMODE) {
				ei.setmode(INFO_RXMODE);
				ei.requestmode = TO_RXONLY;
			}
			Debug("Automatic running in  RXONLY  mode ");
			break;
		case INFO_TXRXMODE:
			strcpy(szStr, "Tx/Rx - MAIN OK");
			iOverallModeChangedOver = 5;
			iOverallTXRXModeStatus = INFO_TXRXMODE;
			if (ei.getmode() != INFO_TXRXMODE) {
				ei.setmode(INFO_TXRXMODE);
				ei.requestmode = TO_TXRX;
			}
			ei.updateslot(COMM_STATUS_SLOT, "1"); // automatic in OK mode
			Debug("Automatic running in  TX/RX  mode ");

			break;
		default:
			strcpy(szStr, " ??unknown??");
			iOverallTXRXModeStatus = IFMODE_NONE;
			Debug("Automatic in an  UNKNOWN  infodriver mode ");
		}
	}
	iNextResilienceProcessing = 43; // reset counter 
	SetDlgItemText(hWndDlg, IDC_INFO_STATUS, szStr);
}


void Restart_CSI_NI_NO_Messages(void)
{
	ei.requestmode = INFO_TXRXMODE;
	iOverallModeChangedOver = 0;
}


void ForceAutoIntoRXOnlyMode(void)
{
	//	iOverallTXRXModeStatus = UNKNOWNVAL;
	ei.setmode(INFO_RXMODE);					// force to rxonly -- so that resilient pair can take over
	ei.requestmode = TO_RXONLY; // force other driver into tx immeadiately if running ??
	SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "FORCE RX");
	iNextResilienceProcessing = 1; // reset counter 
	iOverallModeChangedOver = 5;
}


void ForceAutoIntoTXRXMode(void)
{
	//	iOverallTXRXModeStatus = UNKNOWNVAL;
	ei.setmode(INFO_TXRXMODE);
	ei.requestmode = TO_TXRX;
	SetDlgItemText(hWndDlg, IDC_INFO_STATUS, "Req go Tx/Rx");
	ei.setslot(COMM_STATUS_SLOT, "1"); // currently automatic up
	iNextResilienceProcessing = 1; // reset counter 
	iOverallModeChangedOver = 5;
}



