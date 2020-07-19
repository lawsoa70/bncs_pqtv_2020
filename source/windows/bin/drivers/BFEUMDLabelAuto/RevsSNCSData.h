// RevsSNCSData.h: interface for the CRevsSNCSData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REVSSNCSDATA_H__43C4E7A4_C69D_4357_A183_2E69802DD8EE__INCLUDED_)
#define AFX_REVSSNCSDATA_H__43C4E7A4_C69D_4357_A183_2E69802DD8EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786 4996 4018)

#include "BFEUMDLabelCONSTS.h"

class CRevsSNCSData  
{
private:
	// incoming data from SNCS infodriver, this data is added to the LabelsDatabase class.
	int m_iDeviceNumber;
	int m_iMaxSlotsUsed;
	int m_iRevsCounter;

	struct SNCSAliasData {
		char szRevertiveData[BUFFER_CONST];
		int m_iSNCSInfodriverSlot;
		int m_iAssociatedExternalSource;
	};

	struct SNCSAliasData stSNCSAliasData[MAXDESTINATIONS];

public:
	CRevsSNCSData();
	virtual ~CRevsSNCSData();

	BOOL setDeviceNumber(int iCCDevice);
	BOOL setMaximumSlotsUsed( int iMaxUsed );  
	BOOL setSNCSInfodriverSlot( int iIndex, int iSlot );
	BOOL setAssociatedExternalSource( int iIndex, int iSource );

	BOOL storeSNCSRevertive( int iSlot, char szData[BUFFER_CONST] );  // store actual revertive -- not sure how to process yet
	BOOL getSNCSRevertive( int iSlot, char szData[BUFFER_CONST] );  // retrieve actual revertive -- not sure how to process yet

	int getDeviceNumber( void );
	int getMaximumSlotsUsed( void );
	int getRevertivesCounter( void );
	int getSNCSInfodriverSlot( int iIndex );
	int getAssociatedExternalSource( int iIndex );

};

#endif // !defined(AFX_REVSSNCSDATA_H__43C4E7A4_C69D_4357_A183_2E69802DD8EE__INCLUDED_)
