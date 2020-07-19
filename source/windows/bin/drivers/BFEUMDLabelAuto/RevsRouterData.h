// RevsRouterData.h: interface for the CRevsRouterData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REVSROUTERDATA_H__E34205BE_41D3_4D42_82C1_6013D7836FE1__INCLUDED_)
#define AFX_REVSROUTERDATA_H__E34205BE_41D3_4D42_82C1_6013D7836FE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786 4996 4018)

#include "BFEUMDLabelCONSTS.h"

class CRevsRouterData  
{
private:
	int m_iRouterDeviceNumber;
	int m_iMaxSourcesUsed;
	int m_iMaxDestinationsUsed;
	int m_iRevsCounter;

	struct RouterData {
		int m_iRoutedSourceNumber; // straight revertive from grd -- true src for most cases except re-enterant
		int m_ReEnterant_Src_Number_for_Destination; // 0 if not a re-enterant dest / src - else will hold linked src number
		int m_iCalculatedSourceNumber; // calculated src number for reenterent saucy-dests, else -1.
		int m_iLabelDBRecordNumber;  // index to the ROUTER label database record data in the LabelDatabaseClass for src-- both stored in class and externally in db3
	};

	struct RouterData stRouterData[MAXDESTINATIONS];

	struct SourceTypeData { // generally only one of these will be true but in event src is multiple type
		eAliasSourceType m_iAliasType;  // looped, camera, external, or just plain local
		int m_iDestinationIfLooped; // from which dest if this is a looped src 
		char szDb2SoftName[BUFFER_CONST];
		int m_iAssociatedSNCSInfodriverSlot;  // only assigned for external source types, else 0
	};

	struct SourceTypeData stSourceData[MAXDESTINATIONS];

public:
	CRevsRouterData();
	virtual ~CRevsRouterData();

	BOOL setRouterNumber(int iCCDevice);
	BOOL setRouterSize( int iMaxSrc, int iMaxDest );  // db0 and db1 size basically

	// router revertives 

	BOOL storeGRDRevertive( int iDestination, int iSource );  // store given source from actual rev; any re-enterant src/dest calc then done  if required
	void setDestinationReEnterantValue( int iDestination, int iReEntSource ); // done at startup from reading obj settings for re-enterant vals
	BOOL setLabelDBNumber( int iDestination, int iDBNumber );

	int getRouterNumber( void );
	int getRevertivesCounter( void );
	int getMaximumDestinationsUsed( void );
	int getMaximumSourcesUsed( void );
	int getRoutedSourceForDestination( int iDestination );  // return revertive source as got from grd rev
	int getCalculatedSourceForDestination( int iDestination ); // only has a val if src/dest is re-enterant and is calculated on storing grd rev
	int getLabelDBNumber( int iDestination ); 
	int isDestinationReEnterant( int iDestination );

	bool setSourceData( int iSource, eAliasSourceType eSrcType, int iLoopedDest);
	bool setSourceSoftName( int iSource, char szSoft[BUFFER_CONST] ); 
	BOOL setSNCSInfodriverSlot( int iSource, int iSlot );
	eAliasSourceType getSourceType( int iSource );
	bool getSourceSoftName( int iSource, char szSoft[BUFFER_CONST] ); 
	int isSourceReEnterent( int iSource );  // returns linked dest if looped src else -1
	int getSNCSInfodriverSlot( int iSource );
	
};

#endif // !defined(AFX_REVSROUTERDATA_H__E34205BE_41D3_4D42_82C1_6013D7836FE1__INCLUDED_)
