
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//              FiFo buffer routines 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FiFoBuffer.h"
//#include "except.h"

//////////////////////////////////////////////////////////////////////////
// Construction / destruction
//////////////////////////////////////////////////////////////////////////


CFiFoBuffer::CFiFoBuffer() {
	// new buffer 
	stCommandBuffer.iReadEntry=0;
	stCommandBuffer.iLastEntry=0;
	for (int i=0;i<MAX_BUFFER_SIZE;i++) {
		stCommandBuffer.BufferData[i].iLength=0;
		stCommandBuffer.BufferData[i].iType=0;
		stCommandBuffer.BufferData[i].iBNCSDevice=0;
		strcpy(stCommandBuffer.BufferData[i].szData, " | "); 
	}
}

CFiFoBuffer::~CFiFoBuffer() {
	// nothing to do 
}

// FUNCTION Add_to_FifoBuffer
/* add command string into  buffer
    returns the number of entries now in the  buffer  
*/
int CFiFoBuffer::Add_to_FifoBuffer( char szEntry[DATA_BUFFER_CONST], int iType, int iCCDevice, BOOL bStartOfBufferIfPoss )
{	
	int iE;
	int iLength = strlen(szEntry);
	if (bStartOfBufferIfPoss==TRUE) {
		if (stCommandBuffer.iReadEntry>0) { // space at start of buffer 
			// place command in at current readentry pointer as it has been read already
			//for (int i=0;i<iLength;i++)	stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].szData[i]=szEntry[i];
			strcpy( stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].szData, szEntry );
			stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iLength = iLength;
			stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iType = iType;
			stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iBNCSDevice = iCCDevice;
			stCommandBuffer.iReadEntry--;
			iE = (stCommandBuffer.iLastEntry-stCommandBuffer.iReadEntry);
			return iE ;
		}
		// as no space at front of buffer so try normal load at end 
	}
	
	if (stCommandBuffer.iLastEntry<(MAX_BUFFER_SIZE-1) ) {
		// space to add new entry
		stCommandBuffer.iLastEntry ++;
		//for (int i=0;i<iLength;i++)	stCommandBuffer.BufferData[stCommandBuffer.iLastEntry].szData[i]=szEntry[i];
		strcpy( stCommandBuffer.BufferData[stCommandBuffer.iLastEntry].szData, szEntry );
		stCommandBuffer.BufferData[stCommandBuffer.iLastEntry].iLength = iLength;
		stCommandBuffer.BufferData[stCommandBuffer.iLastEntry].iType = iType;
		stCommandBuffer.BufferData[stCommandBuffer.iLastEntry].iBNCSDevice = iCCDevice;
	}
	else {
		//  check if  readindex is above 0 -  i.e.  processing started - so there is space lower down in buffer
		// although now processing stored commands out of order from earlier processing 
		if (stCommandBuffer.iReadEntry>0) { // space at start of buffer 
			// place new entry in at read entry index - as that entry
			//for (int i=0;i<iLength;i++)	stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].szData[i]=szEntry[i];
			strcpy( stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].szData, szEntry );
			stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iLength = iLength;
			stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iType = iType;
			stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iBNCSDevice = iCCDevice;
			stCommandBuffer.iReadEntry--;
		}
		else { 
			OutputDebugString( "  ** ERROR FIFOBUFFER AddtoFifoBuffer- ERROR buffer is full - start processing entries **  ");
		}
	}		
	iE = (stCommandBuffer.iLastEntry-stCommandBuffer.iReadEntry);
	return iE ;
}


//FUNCTION Retrieve_from_FifoBuffer
// extracts next contents from buffer 
// returns the BYTE buffer and the type of the data, also length
int CFiFoBuffer::Retrieve_from_FifoBuffer ( char szEntry[DATA_BUFFER_CONST], int  *iType, int *iCCDevice )
{
	for (int i=0;i<DATA_BUFFER_CONST;i++) szEntry[i] = NULL;
	*iType = 0; *iCCDevice = 0;
	if ( ( stCommandBuffer.iReadEntry<stCommandBuffer.iLastEntry) && (stCommandBuffer.iLastEntry>0) ) {
		stCommandBuffer.iReadEntry++; // inc index to get entry to read -- so ireadentry has been read
		//for (i=0;i<stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iLength;i++)
		//	szEntry[i] = stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].szData[i];
		strcpy(szEntry, stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].szData );
		int iReturnLength = stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iLength;
		*iType = stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iType;
		*iCCDevice = stCommandBuffer.BufferData[stCommandBuffer.iReadEntry].iBNCSDevice;
		// now check to see if all entries read before leaving 
		if (stCommandBuffer.iReadEntry>=stCommandBuffer.iLastEntry) 
			ClearFifoBuffer();
		// now return
		return iReturnLength;
	}
	else { // processed all entries so reset counters
		ClearFifoBuffer();
		*iType = 0;
		return 0;
	}	
}

int CFiFoBuffer::Number_Entries_in_FiFoBuffer(void) 
{
	int iE = (stCommandBuffer.iLastEntry-stCommandBuffer.iReadEntry);
	return iE ;
}


void CFiFoBuffer::ClearFifoBuffer(void) {
	stCommandBuffer.iReadEntry=0;
	stCommandBuffer.iLastEntry=0;
	for (int i=0;i<MAX_BUFFER_SIZE;i++) 
		stCommandBuffer.BufferData[i].iLength=0;
}