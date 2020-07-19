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
	#define MAXLABELS 250
	#define BUFFER_CONST 256

	int m_iMaxSNCSLabelsUsed;
	int m_iMaxRouterLabelsUsed;

	// linked to slots 2001 + associated with auto 
	struct SncsLabelData {
		int m_iIndexNumber;                             // slot index number of revertive
		int m_iListViewIndexNumber;                  // number in list view for easy app window update
		bncs_string szAliasName;						// will start with either a $ - freeform or SNCS alias  
		bncs_string szAutomaticName;				// AUTOMATIC soft	version of name to appear on soft / right side of UMD
		bncs_string szManualName;					// MANUAL version to appear on soft UMD
		bncs_string m_iAutomaticManualStatus;
		bncs_string m_iPermTempStatus;		
		bncs_stringlist sncsLinkedtoRouterData;   // links the sncs alias data to a specific router alias data -- not used in router alias data structure
	};

	// linked to slots 1 - 128  associated with auto 
	struct RouterLabelData {
		int m_iSourceNumber;                             // actual source number == slot number of revertive
		int m_iListViewIndexNumber;                   // number in list view for easy app window update
		bncs_string szAutomaticName;				// AUTOMATIC	version of name to appear on soft / right side of UMD
		bncs_string szManualName;					 // MANUAL version to appear on soft UMD -- not really used at present
		bncs_string m_iAutomaticManualStatus;
		bncs_string m_iPermTempStatus;		
		bncs_string LinkedtoSncsData;           // links the sncs alias data to a specific router alias data 
	};

	struct SncsLabelData stSNCSAliasData[MAXLABELS]; // sncs originated labels

	struct RouterLabelData stRouterAliasData[MAXLABELS]; // router orig ones


public:
	SoftNameData();
	virtual ~SoftNameData();

	int getMaxSncsLabelsInUse(void);
	int getMaxRouterLabelsInUse(void);

	BOOL storeSncsLabelData( int iIndex, bncs_string szNewData ); // take in a whole string and split it up and set fields in structure
	BOOL storeRouterLabelData( int iIndex, bncs_string szData ); // take in a whole string and split it up and set fields in structure

	BOOL getAllSncsLabelData( int iIndex, bncs_string *szAlias, bncs_string *szAutoName, bncs_string *szManlName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_stringlist *sslist );  // locate by index number
	
	BOOL getAllSncsLabelData( bncs_string szAlias, bncs_string *szAutoName, bncs_string *szManlName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_stringlist *sslist ); // locate by alias name
	
	BOOL getAllRouterLabelData( int iIndex, int * iSourceNum, bncs_string *szAutoName, bncs_string *szManlName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_string *ssLabel  );  // locate by index number
		
	BOOL getAllRouterLabelData( int iSourceNum, bncs_string *szAutoName, bncs_string *szManlName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_string *ssLabel );  // locate by index number
		
};

#endif // !defined(AFX_SOFTNAMEDATA_H__BB5AF55F_6049_4283_8DB4_39427A857C3B__INCLUDED_)
