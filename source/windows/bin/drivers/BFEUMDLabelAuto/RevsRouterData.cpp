// RevsRouterData.cpp: implementation of the CRevsRouterData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RevsRouterData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRevsRouterData::CRevsRouterData()
{
	// initialise structure
	for (int i=0;i<MAXDESTINATIONS;i++) {
		stRouterData[i].m_iCalculatedSourceNumber=0;
		stRouterData[i].m_iRoutedSourceNumber=0;
		stRouterData[i].m_ReEnterant_Src_Number_for_Destination=0; // dest is not reeenterant
		// source data
		stSourceData[i].m_iAliasType=ePlainType;
		stSourceData[i].m_iDestinationIfLooped;
		stSourceData[i].m_iAssociatedSNCSInfodriverSlot=-1;
	}
	m_iRouterDeviceNumber=0;
	m_iMaxDestinationsUsed=0;
	m_iRevsCounter=0;


}

CRevsRouterData::~CRevsRouterData()
{
	//seek and destroy
}


BOOL CRevsRouterData::setRouterNumber(int iCCDevice)
{
	if (iCCDevice>0) {
		m_iRouterDeviceNumber=iCCDevice;
		return TRUE;
	}
	else {
		OutputDebugString( "CRevsRouterData::setRouterNumber invalid number " );
		return FALSE;
	}
}


BOOL CRevsRouterData::setRouterSize( int iMaxSrc, int iMaxDest )
{
	if ((iMaxSrc>0)&&(iMaxDest>0)) {
		m_iMaxSourcesUsed=iMaxSrc;
		m_iMaxDestinationsUsed=iMaxDest;
		return TRUE;
	}
	else {
		OutputDebugString( "CRevsRouterData::setROuterSize invalid src or dest size " );
		return FALSE;
	}
}


BOOL CRevsRouterData::storeGRDRevertive( int iDestination, int iSource )
{
  // store given source from actual rev; any re-enterant src/dest calc then done  if required
	if (iSource>0) {
		if ((iDestination>0)&&(iDestination<=m_iMaxDestinationsUsed)) {
			stRouterData[iDestination].m_iRoutedSourceNumber=iSource;
			stRouterData[iDestination].m_iCalculatedSourceNumber = iSource;
			m_iRevsCounter++;
			// now calc reeneterant src if required 
			// is new src a looped src
			if (isSourceReEnterent(iSource)>0) {
				bool bReEntCalcComplete = FALSE;
				int iBreakOutCounter=0;
				do {
					int iloopDest = isSourceReEnterent(stRouterData[iDestination].m_iCalculatedSourceNumber);
					if (iloopDest>0) {
						// find src to this looped dest 
						stRouterData[iDestination].m_iCalculatedSourceNumber = stRouterData[iloopDest].m_iRoutedSourceNumber;
						// test for infinite loop eg looped dest to same loop src
						iBreakOutCounter++;
						if (iBreakOutCounter>9) {
							bReEntCalcComplete=TRUE; // end it as  going round and  round before of stupid route made
							OutputDebugString("BFELabelAuto - storeGRDRevertive breakout of  do loop infinite route made\n ");
						}
					}
					else {
						// src no longer re-enterant so finish
						bReEntCalcComplete = TRUE;
					}
				} while (bReEntCalcComplete==FALSE);
				char szDebug[BUFFER_CONST];
				wsprintf( szDebug, "storeGRDRevertive dest %d rev src %d  calc src %d \n", iDestination, iSource, stRouterData[iDestination].m_iCalculatedSourceNumber);
				OutputDebugString( szDebug );
			}
			return TRUE;
		}
		else {
			OutputDebugString( "CRevsRouterData::storeGRDRevertive negative destination or greater than maxUsed \n" );
			return FALSE;
		}
	}
	else {
		OutputDebugString( "CRevsRouterData::storeGRDRevertive negative source \n" );
		return FALSE;
	}
}



void CRevsRouterData::setDestinationReEnterantValue( int iDestination, int iReEntSource )
{
 // done at startup from reading obj settings for re-enterant vals
	if (iReEntSource>0) {
		if ((iDestination>0)&&(iDestination<=m_iMaxDestinationsUsed)) {
			stRouterData[iDestination].m_ReEnterant_Src_Number_for_Destination=iReEntSource;
		}
		else {
			OutputDebugString( "CRevsRouterData::setDestinationReEnterantValue negative destination or greater than maxUsed \n" );
		}
	}
	else {
		OutputDebugString( "CRevsRouterData::setDestinationReEnterantValue negative source \n" );
	}
}


int CRevsRouterData::getRevertivesCounter( void )
{
	return m_iRevsCounter;
}


int CRevsRouterData::getRouterNumber( void )
{
	return m_iRouterDeviceNumber;
}


int CRevsRouterData::getMaximumDestinationsUsed( void )
{
	return m_iMaxDestinationsUsed;
}


int CRevsRouterData::getMaximumSourcesUsed( void )
{
	return m_iMaxSourcesUsed;
}

int CRevsRouterData::getRoutedSourceForDestination( int iDestination )
{
  // return revertive source as  got from grd rev
	if ((iDestination>0)&&(iDestination<=m_iMaxDestinationsUsed)) {
		return stRouterData[iDestination].m_iRoutedSourceNumber;
	}
	else {
		OutputDebugString( "CRevsRouterData::getRoutedSourceForDestination negative destination or greater than maxUsed \n" );
		return -1;
	}
}

int CRevsRouterData::getCalculatedSourceForDestination( int iDestination )
{
 // only has a val if src/dest is re-enterant and is calculated on storing grd rev
	if ((iDestination>0)&&(iDestination<=m_iMaxDestinationsUsed)) {
		return stRouterData[iDestination].m_iCalculatedSourceNumber;
	}
	else {
		OutputDebugString( "CRevsRouterData::getCalculatedSourceForDestination negative destination or greater than maxUsed \n" );
		return -1;
	}
}

int CRevsRouterData::getLabelDBNumber( int iDestination ) 
{
	if ((iDestination>0)&&(iDestination<=m_iMaxDestinationsUsed)) {
		return stRouterData[iDestination].m_iLabelDBRecordNumber;
	}
	else {
		OutputDebugString( "CRevsRouterData::getLabelDBNumber negative destination or greater than maxUsed \n" );
		return -1;
	}
}

int CRevsRouterData::isDestinationReEnterant( int iDestination )
{
	// if this dest loops round to a reenetant src this will return src value, else 0
	return stRouterData[iDestination].m_ReEnterant_Src_Number_for_Destination;

}


//////////////////////////////////////////////////////////////////////////
// source data routines
//////////////////////////////////////////////////////////////////////////

bool CRevsRouterData::setSourceData( int iSource, eAliasSourceType eSrcType, int iLoopedDest)
{
	if ((iSource>0)&&(iSource<MAXDESTINATIONS)) {
		stSourceData[iSource].m_iAliasType = eSrcType;
		if (eSrcType==eLoopedType) {
			stSourceData[iSource].m_iDestinationIfLooped = iLoopedDest;
			stRouterData[iLoopedDest].m_ReEnterant_Src_Number_for_Destination = iSource;
		}
		return TRUE;
	}
	else
		return FALSE;
}




BOOL CRevsRouterData::setSNCSInfodriverSlot( int iSource, int iSlot )
{
	if ((iSource>0)&&(iSource<MAXDESTINATIONS)) {
		stSourceData[iSource].m_iAssociatedSNCSInfodriverSlot = iSlot;
		return TRUE;
	}
	else {
		OutputDebugString( "CRevsRouterData::setSNCSInfodriverSlot invalid src  " );
		return FALSE;
	}
}



eAliasSourceType CRevsRouterData::getSourceType( int iSource ) 
{
	if ((iSource>0)&&(iSource<MAXDESTINATIONS)) {
		return stSourceData[iSource].m_iAliasType;
	}
	else
		return ePlainType;
}


int CRevsRouterData::getSNCSInfodriverSlot( int iSource ) 
{
	if ((iSource>0)&&(iSource<MAXDESTINATIONS)) {
		return stSourceData[iSource].m_iAssociatedSNCSInfodriverSlot;
	}
	else
		return -1;
}


int CRevsRouterData::isSourceReEnterent( int iSource )  // PS there is a 1:1 mapping for re-enterant sources and dests
{
	if ((iSource>0)&&(iSource<MAXDESTINATIONS)) {
		if (stSourceData[iSource].m_iAliasType==eLoopedType)
			return stSourceData[iSource].m_iDestinationIfLooped;
	}
	return -1;
}


bool CRevsRouterData::setSourceSoftName( int iSource, char szSoft[BUFFER_CONST] )
{
	if ((iSource>0)&&(iSource<MAXDESTINATIONS)) {
		strcpy( stSourceData[iSource].szDb2SoftName, szSoft );
		return TRUE;
	}
	return FALSE;
}


bool CRevsRouterData::getSourceSoftName( int iSource, char szSoft[BUFFER_CONST] )
{
	strcpy(szSoft, " ");
	if ((iSource>0)&&(iSource<MAXDESTINATIONS)) {
		strcpy( szSoft, stSourceData[iSource].szDb2SoftName );
		return TRUE;
	}
	return FALSE;
}

