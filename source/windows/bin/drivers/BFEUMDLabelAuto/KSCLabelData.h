// KSCLabelData.h: interface for the KSCLabelData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KSCLABELDATA_H__C46AACD3_E4B5_4E88_B533_AC1A44BF7133__INCLUDED_)
#define AFX_KSCLABELDATA_H__C46AACD3_E4B5_4E88_B533_AC1A44BF7133__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786 4996 4018)

#include "BFEUMDLabelCONSTS.h"

class CKSCLabelData  
{
private:

	#define MAXLABELSIZE 12
	#define MAXREVERTIVESIZE 32

	// data store of all labels for all Sources as calculated by this auto -- these are the strings passed to the BFEUMD driver
	int m_iKSCUMD_Driver_Number; // CC device number of the kscumd driver -- not the ext info linked to auto 
	BOOL m_allRevertivesOk;
	int m_iRevsCounter;

	struct LabelData {
		BOOL LabelUsed;
		char szFixedName[MAXLABELSIZE]; // max size should be 8 chars
		char szSoftName[MAXLABELSIZE];   // fill the remaining spaces up to a total length of 16
		char szRevFromDriver[MAXREVERTIVESIZE]; // for  those dests under bfe control ?? or consistency check ??
		int iLabelsDatabaseIndex; // index into raw database from  which this label is drawn
	};

	struct LabelData stLabelData[MAXSOURCES];

	// private functions
	void UpdateKSCDriver( int iSource );

public:
	CKSCLabelData();
	virtual ~CKSCLabelData();

	BOOL setFixedName( int iSource, char szName[BUFFER_CONST], BOOL bUpdateKSC );
	BOOL setSoftName( int iSource, char szName[BUFFER_CONST], BOOL bUpdateKSC );

	BOOL storeAndSendLabelToKSC( int iSource, char szLabel[BUFFER_CONST] );
	BOOL storeAndSendLabelToKSC( int iSource, char szFixed[BUFFER_CONST], char szSoft[BUFFER_CONST] );

	BOOL getFixedName( int iSource, char szName[BUFFER_CONST] );
	BOOL getSoftName( int iSource, char szName[BUFFER_CONST] );
	BOOL getRevertiveString( int iSource, char szName[BUFFER_CONST] );

	BOOL storeRevertiveFromDriver( int iSource, char szRev[BUFFER_CONST] );
	BOOL DetermineRevertiveStatus( void );  // have all revs returned yet, and return status

	BOOL setKSCDriverNumber( int iDriver );
	int getKSCDriverNumber( void );
	int getRevertivesCounter( void );


};

#endif // !defined(AFX_KSCLABELDATA_H__C46AACD3_E4B5_4E88_B533_AC1A44BF7133__INCLUDED_)
