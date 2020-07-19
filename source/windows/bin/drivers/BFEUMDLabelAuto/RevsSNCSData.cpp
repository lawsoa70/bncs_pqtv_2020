// RevsSNCSData.cpp: implementation of the CRevsSNCSData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RevsSNCSData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRevsSNCSData::CRevsSNCSData()
{
	m_iDeviceNumber = 0;
	m_iRevsCounter = 0;
	m_iMaxSlotsUsed = MAXDESTINATIONS; // default setting
	for (int i=0;i<MAXDESTINATIONS;i++) {
		strcpy( stSNCSAliasData[i].szRevertiveData, "" );
		stSNCSAliasData[i].m_iSNCSInfodriverSlot=0;
		stSNCSAliasData[i].m_iAssociatedExternalSource=0;
	}
}

CRevsSNCSData::~CRevsSNCSData()
{

}


BOOL CRevsSNCSData::setDeviceNumber(int iCCDevice)
{
	if ((iCCDevice>0)&&(iCCDevice<1000)) {
		 m_iDeviceNumber = iCCDevice;
		 return TRUE;
	}
	else
		return FALSE;
}


BOOL CRevsSNCSData::setMaximumSlotsUsed( int iMaxUsed )
{
	if ((iMaxUsed>0)&&(iMaxUsed<MAXDESTINATIONS)) {
		m_iMaxSlotsUsed = iMaxUsed;
		return TRUE;
	}
	else
		return FALSE;
}


BOOL CRevsSNCSData::setSNCSInfodriverSlot( int iIndex, int iSlot )
{
	if ((iIndex>0)&&(iIndex<MAXDESTINATIONS)) {
		stSNCSAliasData[iIndex].m_iSNCSInfodriverSlot = iSlot;
		return TRUE;
	}
	else
		return FALSE;
}



BOOL CRevsSNCSData::setAssociatedExternalSource( int iIndex, int iSource )
{
	if ((iIndex>0)&&(iIndex<MAXDESTINATIONS)) {
		stSNCSAliasData[iIndex].m_iAssociatedExternalSource = iSource;
		return TRUE;
	}
	else
		return FALSE;
}


BOOL CRevsSNCSData::storeSNCSRevertive( int iSlot, char szData[BUFFER_CONST] )
{
	if ((iSlot>0)&&(iSlot<m_iMaxSlotsUsed)) {
		strcpy( stSNCSAliasData[iSlot].szRevertiveData, szData );
		m_iRevsCounter++;
		return TRUE;
	}
	return FALSE;
}


BOOL CRevsSNCSData::getSNCSRevertive( int iSlot, char szData[BUFFER_CONST] )
{
	strcpy( szData, "" );
	if ((iSlot>0)&&(iSlot<m_iMaxSlotsUsed)) {
		strcpy( stSNCSAliasData[iSlot].szRevertiveData, szData );
		return TRUE;
	}
	return FALSE;
}


int CRevsSNCSData::getDeviceNumber( void )
{
	return m_iDeviceNumber;
}


int CRevsSNCSData::getMaximumSlotsUsed( void )
{
	return m_iMaxSlotsUsed;
}


int CRevsSNCSData::getRevertivesCounter( void )
{
	return m_iRevsCounter;
}


int CRevsSNCSData::getSNCSInfodriverSlot( int iIndex )
{
	if ((iIndex>0)&&(iIndex<MAXDESTINATIONS)) {
		return stSNCSAliasData[iIndex].m_iSNCSInfodriverSlot;
	}
	else
		return -1;
}



int CRevsSNCSData::getAssociatedExternalSource( int iIndex )
{
	if ((iIndex>0)&&(iIndex<MAXDESTINATIONS)) {
		return stSNCSAliasData[iIndex].m_iAssociatedExternalSource;
	}
	else
		return -1;
}

