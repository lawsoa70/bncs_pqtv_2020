//////////////////////////////////////////////////////////////////////
//
//
// BFEUMDLabelDatabase.h: interface for the CLabelDatabase class.
//
//  Main database class to hold all relevant lablel data and functions to process data
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BFEUMDLABELSDATABASE_H__3FF97143_255E_423F_A517_6037D24450CD__INCLUDED_)
#define AFX_BFEUMDLABELSDATABASE_H__3FF97143_255E_423F_A517_6037D24450CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786 4996 4018)

#include "BFEUMDLabelCONSTS.h"

class CBFEUMDLabelDatabase  
{
private:
	// the raw data as loaded and calculated on startup and during running of alias data
	// all umd labels are created from the data stored in this structure and this is the MASTER data
	
	int m_iMaxSNCSLabelsUsed;
	int m_iMaxRouterLabelsUsed;
	int m_iRevsCounter;

	// linked to db2 of dev ini file associated with auto 
	struct SncsLabelData {
		int m_iDBIndexNumber;                         // index number of entry in db2, or db3 file 
		int m_iListViewIndexNumber;                  // number in list view for easy app window update
		char szAliasName[BUFFER_CONST];         // will start with either a $ - freeform or SNCS alias  
		char szAutomaticName[BUFFER_CONST];  // AUTOMATIC soft	version of name to appear on soft / right side of UMD
		char szManualName[BUFFER_CONST];      // MANUAL version to appear on soft UMD
		int m_iAutomaticManualStatus;
		int m_iPermTempStatus;		
		bncs_stringlist sncsLinkedtoRouterData;   // links the sncs alias data to a specific router alias data -- not used in router alias data structure
	};

	// linked to db3 of dev ini file associated with auto 
	struct RouterLabelData {
		int m_iDBIndexNumber;                           // index number of entry in  db3 file 
		int m_iListViewIndexNumber;                   // number in list view for easy app window update
		int m_iSourceNumber;                             // actual source number
		char szAutomaticName[BUFFER_CONST];	  // AUTOMATIC	version of name to appear on soft / right side of UMD
		char szManualName[BUFFER_CONST];       // MANUAL version to appear on soft UMD -- not really used at present
		int m_iAutomaticManualStatus;
		int m_iPermTempStatus;		
		char szSncsLabel[BUFFER_CONST];           // one sncs alias data to a specific router alias data - index in sncs data
	};

	struct SncsLabelData stSNCSAliasData[MAXDESTINATIONS]; // sncs originated labels

	struct RouterLabelData stRouterAliasData[MAXDESTINATIONS]; // router orig ones

public:
	CBFEUMDLabelDatabase();
	virtual ~CBFEUMDLabelDatabase();

	void setMaxSncsLabelsInUse(int iUsed );
	void setMaxRouterLabelsInUse(int iUsed );
	int getMaxSncsLabelsInUse(void);
	int getMaxRouterLabelsInUse(void);
	int getRevertivesCounter( void );

	BOOL storeSncsLabelData( int iIndex, char szData[BUFFER_CONST] ); // take in a whole string and split it up and set fields in structure
	BOOL storeRouterLabelData( int iIndex, char szData[BUFFER_CONST] ); // take in a whole string and split it up and set fields in structure

	BOOL getSncsLabel( int iIndex, char szAlias[BUFFER_CONST] );  // locate by index number return label only
	
	BOOL isSncsLabelUnique( char szLabel[BUFFER_CONST] ); // check if label is new or not
	
	int findNextFreeSNCSIndex( void ); // finding index for new sncs label
 
	BOOL getAllSncsLabelData( int iIndex, char szAlias[BUFFER_CONST], char szAutoName[BUFFER_CONST], char szManlName[BUFFER_CONST],
									int *iAutoStatus, int *iPermStatus, bncs_stringlist *sslist, int *iDBENtry, int *iListViewNum );  // locate by index number
	
	BOOL getAllSncsLabelData( char szAlias[BUFFER_CONST], int *iAliasIndex, char szAutoName[BUFFER_CONST], char szManlName[BUFFER_CONST],
									int *iAutoStatus, int *iPermStatus, bncs_stringlist *sslist, int *iDBENtry, int *iListViewNum ); // locate by alias name
	
	BOOL getAllRouterLabelData( int iIndex, int * iSourceNum, char szAutoName[BUFFER_CONST], char szManlName[BUFFER_CONST],
									int *iAutoStatus, int *iPermStatus, char szLabel[BUFFER_CONST], int *iDBENtry, int *iListViewNum );  // locate by index number
	
	BOOL getAllRouterLabelData( int iSourceNum, char szAutoName[BUFFER_CONST], char szManlName[BUFFER_CONST],
									int *iAutoStatus, int *iPermStatus, char szLabel[BUFFER_CONST], int *iDBENtry, int *iListViewNum );  // locate by source number
	
};

#endif // !defined(AFX_BFEUMDLABELDATABASE_H__3FF97143_255E_423F_A517_6037D24450CD__INCLUDED_)
