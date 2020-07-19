// FiFoBuffer.h: interface for the CFiFoBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIFOBUFFER_H__33D126B9_8389_479A_ABC1_1136DF6DF6D3__INCLUDED_)
#define AFX_FIFOBUFFER_H__33D126B9_8389_479A_ABC1_1136DF6DF6D3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786 4996 4018)

typedef unsigned char BYTE; 

class CFiFoBuffer  
{
private:
	#define MAX_BUFFER_SIZE 1000
	#define DATA_BUFFER_CONST 256

	struct MemoryData {
		int iLength; 
		int iType;
		int iBNCSDevice;
		char szData[DATA_BUFFER_CONST];
	};

	struct FiFoBuffer {
		int iReadEntry;
		int iLastEntry;
		struct MemoryData BufferData[MAX_BUFFER_SIZE];
	};

	struct FiFoBuffer stCommandBuffer; 

public:
	CFiFoBuffer();
	virtual ~CFiFoBuffer();

	// Fifo Functions
	int Add_to_FifoBuffer( char szEntry[DATA_BUFFER_CONST], int iType, int iCCDevice, BOOL bStartOfBufferIfPoss );
	int Retrieve_from_FifoBuffer( char szEntry[DATA_BUFFER_CONST], int *iType, int *iCCDevice );
	int Number_Entries_in_FiFoBuffer(void);
	void ClearFifoBuffer(void);

};

#endif // !defined(AFX_FIFOBUFFER_H__33D126B9_8389_479A_ABC1_1136DF6DF6D3__INCLUDED_)
