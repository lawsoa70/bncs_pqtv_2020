// KSCLabelData.cpp: implementation of the KSCLabelData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KSCLabelData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKSCLabelData::CKSCLabelData()
{
	m_allRevertivesOk = FALSE;
	m_iKSCUMD_Driver_Number = 0;
	m_iRevsCounter = 0;
	for (int i=0;i<MAXSOURCES;i++) {
		stLabelData[i].iLabelsDatabaseIndex = 0;
		stLabelData[i].LabelUsed = FALSE;
		strcpy(stLabelData[i].szFixedName, "" );
		strcpy(stLabelData[i].szSoftName, "" );
		strcpy(stLabelData[i].szRevFromDriver, "" );
	}
}


CKSCLabelData::~CKSCLabelData()
{
	//seek and destroy
}


BOOL CKSCLabelData::setFixedName( int iSource, char szName[BUFFER_CONST], BOOL bUpdateKSC )
{
	return FALSE;
}


BOOL CKSCLabelData::setSoftName( int iSource, char szName[BUFFER_CONST], BOOL bUpdateKSC )
{
	return FALSE;
}


BOOL CKSCLabelData::storeAndSendLabelToKSC( int iSource, char szLabel[BUFFER_CONST] )
{
	return FALSE;
}


BOOL CKSCLabelData::storeAndSendLabelToKSC( int iSource, char szFixed[BUFFER_CONST], char szSoft[BUFFER_CONST] )
{
	return FALSE;
}


BOOL CKSCLabelData::getFixedName( int iSource, char szName[BUFFER_CONST] )
{
	return FALSE;
}


BOOL CKSCLabelData::getSoftName( int iSource, char szName[BUFFER_CONST] )
{
	return FALSE;
}


BOOL CKSCLabelData::getRevertiveString( int iSource, char szName[BUFFER_CONST] )
{
	if ((iSource>0)&&(iSource<MAXSOURCES)) {
		strcpy(szName, stLabelData[iSource].szRevFromDriver );
		return TRUE;
	}
	else
		return FALSE;
}


BOOL CKSCLabelData::storeRevertiveFromDriver( int iSource, char szRev[BUFFER_CONST] )
{
	m_iRevsCounter++;
	if ((iSource>0)&&(iSource<MAXSOURCES)) {
		strcpy(stLabelData[iSource].szRevFromDriver, szRev );
		return TRUE;
	}
	else
		return FALSE;
}


BOOL CKSCLabelData::DetermineRevertiveStatus( void )
{
	return FALSE;
}


BOOL CKSCLabelData::setKSCDriverNumber( int iDriver )
{
	if ((iDriver>0)&&(iDriver<1000)) {
		m_iKSCUMD_Driver_Number = iDriver;
		return TRUE;
	}
	else
		return FALSE;
}


int CKSCLabelData::getKSCDriverNumber( void )
{
	return m_iKSCUMD_Driver_Number;
}


int CKSCLabelData::getRevertivesCounter( void )
{
	return m_iRevsCounter;
}

