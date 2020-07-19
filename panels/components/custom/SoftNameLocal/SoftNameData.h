// SoftNameData.h: interface for the SoftNameData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOFTNAMEDATA_H__BB5AF55F_6049_4283_8DB4_39427A857C3B__INCLUDED_)
#define AFX_SOFTNAMEDATA_H__BB5AF55F_6049_4283_8DB4_39427A857C3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <bncs_script_helper.h>

class SoftNameData  
{
private:
	#define MAXLABELS 500
	#define BUFFER_CONST 256

	int m_iMaxSNCSLabelsUsed;
	int m_iMaxRouterLabelsUsed;

	// linked to slots 1 - 128  associated with auto 
	struct RouterLabelData {
		int m_iSourceNumber;                             // actual source number == slot number of revertive
		int m_iListViewIndexNumber;                   // number in list view for easy app window update
		bncs_string szAutomaticName;				// AUTOMATIC	version of name to appear on soft / right side of UMD
		bncs_string szManualName;					 // MANUAL version to appear on soft UMD -- not really used at present
		bncs_string m_iAutomaticManualStatus;
		bncs_string m_iPermTempStatus;		
		bncs_string LinkedtoSncsData;           // sncs label that was assigned to this src
	};

	struct RouterLabelData stRouterAliasData[MAXLABELS]; // router orig ones


public:
	SoftNameData();
	virtual ~SoftNameData();

	int getMaxRouterLabelsInUse(void);

	BOOL storeRouterLabelData( int iIndex, bncs_string szData ); // take in a whole string and split it up and set fields in structure
	
	BOOL getAllRouterLabelData( int iIndex, int * iSourceNum, bncs_string *szAutoName, bncs_string *szManlName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_string *ssLabel, int *iListViewNum );  // locate by index number
		
	BOOL getAllRouterLabelData( int iSourceNum, bncs_string *szAutoName, bncs_string *szManlName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_string *ssLabel, int *iListViewNum );  // locate by index number
		
};

#endif // !defined(AFX_SOFTNAMEDATA_H__BB5AF55F_6049_4283_8DB4_39427A857C3B__INCLUDED_)
