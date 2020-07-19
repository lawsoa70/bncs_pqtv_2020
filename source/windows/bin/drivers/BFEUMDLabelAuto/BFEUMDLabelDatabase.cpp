// BFEUMDLabelDatabase.cpp: implementation of the CBFEUMDLabelDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BFEUMDLabelDatabase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBFEUMDLabelDatabase::CBFEUMDLabelDatabase()
{
	// init on creation
	m_iMaxSNCSLabelsUsed = 200;
	m_iMaxRouterLabelsUsed = 128;
	m_iRevsCounter = 0;
	for (int i=0;i<MAXDESTINATIONS; i++) {
		stSNCSAliasData[i].m_iDBIndexNumber = 0;
		stSNCSAliasData[i].m_iListViewIndexNumber = -1;
		stSNCSAliasData[i].m_iAutomaticManualStatus = eNoStyle;
		stSNCSAliasData[i].m_iPermTempStatus = eUnknownForm;
		strcpy( stSNCSAliasData[i].szAliasName, "");
		strcpy( stSNCSAliasData[i].szAutomaticName, "");
		strcpy( stSNCSAliasData[i].szManualName, "");
		stSNCSAliasData[i].sncsLinkedtoRouterData = bncs_stringlist("");

		stRouterAliasData[i].m_iDBIndexNumber = 0;
		stRouterAliasData[i].m_iListViewIndexNumber = -1;
		stRouterAliasData[i].m_iSourceNumber = 0;
		stRouterAliasData[i].m_iAutomaticManualStatus = eNoStyle;
		stRouterAliasData[i].m_iPermTempStatus = eUnknownForm;
		strcpy( stRouterAliasData[i].szAutomaticName, "");
		strcpy( stRouterAliasData[i].szManualName, "");
		strcpy( stRouterAliasData[i].szSncsLabel, "" );
	} 
}


CBFEUMDLabelDatabase::~CBFEUMDLabelDatabase()
{
	// seek and destroy
}


int CBFEUMDLabelDatabase::getRevertivesCounter( void )
{
	return m_iRevsCounter;
}


int CBFEUMDLabelDatabase::getMaxSncsLabelsInUse(void)
{
	return m_iMaxSNCSLabelsUsed;
}


int CBFEUMDLabelDatabase::getMaxRouterLabelsInUse(void)
{
	return m_iMaxRouterLabelsUsed;
}


void CBFEUMDLabelDatabase::setMaxSncsLabelsInUse(int iUsed )
{
	m_iMaxSNCSLabelsUsed=iUsed;
}


void CBFEUMDLabelDatabase::setMaxRouterLabelsInUse(int iUsed)
{
	m_iMaxRouterLabelsUsed=iUsed;
}



BOOL CBFEUMDLabelDatabase::storeSncsLabelData( int iIndex, char szData[BUFFER_CONST] )
{
	// take in a whole string and split it up and set fields in structure for sncs data
	char szTmp[BUFFER_CONST]="", szStr[BUFFER_CONST]="", szName [BUFFER_CONST]="$NAME", szAuto [BUFFER_CONST]=" ", szMan [BUFFER_CONST]=" ", *tok;
	int i, iTokCount = 0, iVal=0,  iStyle=eNoStyle, iForm=eUnknownForm;
	bncs_stringlist ssl=bncs_stringlist("");
	bool bContinue = TRUE;
	int iPos=0;
	char *pdest;

	// extract data for entry
	iTokCount = 0;
	bContinue= TRUE;
	tok=strtok(szData, "~" );  
	while ((tok)&&(bContinue)) {
		iTokCount++;
		strcpy( szTmp, tok );
		i = strlen(szTmp);
		if (i>2) {
			strncpy(szStr, szTmp+1,i-2 ); // to cut out the < and > chars
			szStr[i-2]=0; // terminate correctly
		}
		else
			strcpy( szStr, szTmp );
		iVal = atoi(szStr);

		switch(iTokCount){
		case 1:	
			strcpy( szName, szStr );
			break;
		case 2:
			strcpy(szAuto, szStr );
			break;
		case 3:
			strcpy(szMan, szStr );
		    break;
		case 4:
			pdest = strstr(szStr, "MANUAL");
			if (pdest!=NULL)
				iStyle = eManualStyle;
			else
				iStyle = eAutomaticStyle;
		    break;
		case 5:
			pdest = strstr(szStr, "PERM");
			if (pdest!=NULL)
				iForm = ePermamentForm;
			else
				iForm = eTemporaryForm;
		    break;
		case 6:
			ssl = bncs_stringlist(szStr);
		    break;
		default:
			bContinue=FALSE; // too many items -- process what we have extracted
		}
		tok=strtok(NULL,"~"); 				
	}// while
	if (iTokCount>5) { // min 6
		if ((iIndex>0)&&(iIndex<MAXDESTINATIONS)) {
			stSNCSAliasData[iIndex].m_iDBIndexNumber = iIndex;
			stSNCSAliasData[iIndex].m_iListViewIndexNumber = iIndex-1;
			stSNCSAliasData[iIndex].m_iAutomaticManualStatus = iStyle;
			stSNCSAliasData[iIndex].m_iPermTempStatus = iForm;
			strcpy( stSNCSAliasData[iIndex].szAliasName, szName);
			strcpy( stSNCSAliasData[iIndex].szAutomaticName, szAuto);
			strcpy( stSNCSAliasData[iIndex].szManualName, szMan);
			stSNCSAliasData[iIndex].sncsLinkedtoRouterData = ssl;	
			m_iRevsCounter++;
			return TRUE;
		}
		else {
			OutputDebugString(" BFEUMDLabel storeSncsLabelData out of range to store \n");
		}
	}
	else {
			OutputDebugString(" BFEUMDLabel storeSncsLabelData tokcount too low \n");
	}

	return FALSE;
}



BOOL CBFEUMDLabelDatabase::storeRouterLabelData( int iIndex, char szData[BUFFER_CONST] )
{
	// take in a whole string and split it up and set fields in structure for router data
	char szStr[BUFFER_CONST]="", szTmp[BUFFER_CONST]="", szAuto [BUFFER_CONST]=" ", szMan [BUFFER_CONST]=" ", *tok;
	int i, iTokCount = 0, iVal=0,  iSourceNumber, iStyle=eNoStyle, iForm=eUnknownForm;
	char  szLabel[BUFFER_CONST]="";
	bool bContinue = TRUE;
	int iPos=0;
	char *pdest;

	// extract data for entry
	iTokCount = 0;
	bContinue= TRUE;
	tok=strtok(szData, "~" );  
	while ((tok)&&(bContinue)) {
		iTokCount++;
		strcpy( szTmp, tok );
		i = strlen(szTmp);
		if (i>2) {
			strncpy(szStr, szTmp+1,i-2 ); // to cut out the < and > chars
			szStr[i-2]=0; // terminate correctly
		}
		else
			strcpy( szStr, szTmp );
		iVal = atoi(szStr);
		//wsprintf( szTmp, "store router - str = %s, ival = %d \n", szStr, iVal );
		//OutputDebugString( szTmp );
		switch(iTokCount){
		case 1:	
			iSourceNumber = atoi(szStr);
			break;
		case 2:
			strcpy( szAuto, szStr );
			break;
		case 3:
			strcpy(szMan, szStr );
		    break;
		case 4:
			pdest = strstr(szStr, "MANUAL");
			if (pdest!=NULL)
				iStyle = eManualStyle;
			else
				iStyle = eAutomaticStyle;
		    break;
		case 5:
			pdest = strstr(szStr, "PERM");
			if (pdest!=NULL)
				iForm = ePermamentForm;
			else
				iForm = eTemporaryForm;
		    break;
		case 6:
			strcpy( szLabel, szStr );
		    break;
		default:
			bContinue=FALSE; // too many items -- process what we have extracted
		}
		tok=strtok(NULL,"~"); 				
	}// while
	if (iTokCount>5) { // 6 minimium
		if ((iIndex>0)&&(iIndex<MAXDESTINATIONS)) {
			stRouterAliasData[iIndex].m_iDBIndexNumber = iIndex;
			stRouterAliasData[iIndex].m_iListViewIndexNumber = iIndex-1;
			stRouterAliasData[iIndex].m_iAutomaticManualStatus = iStyle;
			stRouterAliasData[iIndex].m_iPermTempStatus = iForm;
			stRouterAliasData[iIndex].m_iSourceNumber = iSourceNumber;
			strcpy( stRouterAliasData[iIndex].szAutomaticName, szAuto);
			strcpy( stRouterAliasData[iIndex].szManualName, szMan);
			strcpy( stRouterAliasData[iIndex].szSncsLabel, szLabel );	
			m_iRevsCounter++;
			return TRUE;	
		}
		else {
			OutputDebugString(" BFEUMDLabel storeRouterLabelData out of range to store \n");
		}
	}
	else {
		OutputDebugString(" BFEUMDLabel storeRouterLabelData tokcount too low \n");
	}
	return FALSE;
}


BOOL CBFEUMDLabelDatabase::getSncsLabel( int iIndex, char szAlias[BUFFER_CONST] )
{
	if ((iIndex>0)&&(iIndex<=m_iMaxSNCSLabelsUsed)) {
		strcpy( szAlias, stSNCSAliasData[iIndex].szAliasName );
		return TRUE;
	}
	else
		OutputDebugString(" BFEUMDLabel getSncsLabel - index out of range to retieve \n");
	return FALSE;
}


BOOL CBFEUMDLabelDatabase::isSncsLabelUnique( char szLabel[BUFFER_CONST] )
{
	int iResult=-1;
	for (int iIndex=1;iIndex<=m_iMaxSNCSLabelsUsed;iIndex++) {
		iResult=stricmp(stSNCSAliasData[iIndex].szAliasName, szLabel);
		if (iResult==0) { 
			return FALSE; // already exists
		}
	}
	return TRUE; // not found

}


int CBFEUMDLabelDatabase::findNextFreeSNCSIndex( void ) 
{
	// find the next unsed slot ( $NAME ) or id table full then reuse first UNASSIGNED ( ie has no alias ) slot
	// or first slot that has assigned one but not allocated  to an OS 
	// if all assigned then return ERROR and flag on panels
	int iResult=-1, iNewIndex=0;
	// first $NAME space
	for (iNewIndex=1;iNewIndex<=m_iMaxSNCSLabelsUsed;iNewIndex++) {
		iResult=stricmp(stSNCSAliasData[iNewIndex].szAliasName, "$NAME" );
		if (iResult==0) { // found  
			return iNewIndex;
		}
	}
	// got to end and no $NAME -- ie all slots have labels -- find first label with no assigned name
	for (iNewIndex=1;iNewIndex<=m_iMaxSNCSLabelsUsed;iNewIndex++) {
		if (stSNCSAliasData[iNewIndex].m_iPermTempStatus == eTemporaryForm) {
			bncs_string sstr = stSNCSAliasData[iNewIndex].szManualName;
			bncs_string sstr1 = sstr.stripWhiteSpace();
			if (sstr1.length()==0) {
				return iNewIndex;
			}
		}
	}
	// still no slot found FLAG MAJOR ERROR
	return -1;
}


BOOL CBFEUMDLabelDatabase::getAllSncsLabelData( int iIndex, char szAlias[BUFFER_CONST], char szAutoName[BUFFER_CONST], char szManName[BUFFER_CONST],
									int *iAutoStatus, int *iPermStatus, bncs_stringlist *sslist, int *iDBEntry, int *iListView )
{
	// take in index number and return all sncs data for that index
	if ((iIndex>0)&&(iIndex<=m_iMaxSNCSLabelsUsed)) {
		*iAutoStatus = stSNCSAliasData[iIndex].m_iAutomaticManualStatus;
		*iPermStatus = stSNCSAliasData[iIndex].m_iPermTempStatus;
		*iDBEntry = stSNCSAliasData[iIndex].m_iDBIndexNumber;
		*iListView = stSNCSAliasData[iIndex].m_iListViewIndexNumber;
		strcpy( szAlias, stSNCSAliasData[iIndex].szAliasName );
		strcpy( szAutoName, stSNCSAliasData[iIndex].szAutomaticName );
		strcpy( szManName, stSNCSAliasData[iIndex].szManualName );
		*sslist = stSNCSAliasData[iIndex].sncsLinkedtoRouterData;
		return TRUE;
	}
	else
		OutputDebugString(" BFEUMDLabel getAllSncsLabelData 1 - index out of range to retieve \n");
	return FALSE;
}

BOOL CBFEUMDLabelDatabase::getAllSncsLabelData( char szAlias[BUFFER_CONST], int *iAliasIndex, char szAutoName[BUFFER_CONST], char szManName[BUFFER_CONST],
									int *iAutoStatus, int *iPermStatus, bncs_stringlist *sslist, int *iDBEntry, int *iListView )
{
	// given an SNCS alias name  -- find it and return associated data
	int iResult=-1;
	*iAliasIndex=0;
	for (int iIndex=1;iIndex<=m_iMaxSNCSLabelsUsed;iIndex++) {
		iResult=stricmp(stSNCSAliasData[iIndex].szAliasName, szAlias);
		if (iResult==0) {
			// entry found
			*iAliasIndex = iIndex;
			*iAutoStatus = stSNCSAliasData[iIndex].m_iAutomaticManualStatus;
			*iPermStatus = stSNCSAliasData[iIndex].m_iPermTempStatus;
			*iDBEntry = stSNCSAliasData[iIndex].m_iDBIndexNumber;
			*iListView = stSNCSAliasData[iIndex].m_iListViewIndexNumber;
			strcpy( szAlias, stSNCSAliasData[iIndex].szAliasName );
			strcpy( szAutoName, stSNCSAliasData[iIndex].szAutomaticName );
			strcpy( szManName, stSNCSAliasData[iIndex].szManualName );
			*sslist = stSNCSAliasData[iIndex].sncsLinkedtoRouterData;
			return TRUE;
		}
	}
	OutputDebugString(" BFEUMDLabel getAllSncsLabelData 2 - name not found to retieve \n");
	return FALSE;
}

	
BOOL CBFEUMDLabelDatabase::getAllRouterLabelData( int iIndex, int *iSrcNum, char szAutoName[BUFFER_CONST], char szManName[BUFFER_CONST],
									int *iAutoStatus, int *iPermStatus, char szLabel[BUFFER_CONST], int *iDBEntry, int *iListView )
{
	// take in indexnuber and return associated data
	if ((iIndex>0)&&(iIndex<=m_iMaxRouterLabelsUsed)) {
		*iSrcNum = stRouterAliasData[iIndex].m_iSourceNumber;
		strcpy( szAutoName, stRouterAliasData[iIndex].szAutomaticName );
		strcpy( szManName, stRouterAliasData[iIndex].szManualName );
		*iAutoStatus = stRouterAliasData[iIndex].m_iAutomaticManualStatus;
		*iPermStatus = stRouterAliasData[iIndex].m_iPermTempStatus;
		strcpy( szLabel, stRouterAliasData[iIndex].szSncsLabel );
		*iDBEntry = stRouterAliasData[iIndex].m_iDBIndexNumber;
		*iListView = stRouterAliasData[iIndex].m_iListViewIndexNumber;
		return TRUE;
	}
	else
		OutputDebugString(" BFEUMDLabel getAllRouterLabelData out of range to retieve \n");
	return FALSE;
}


	
BOOL CBFEUMDLabelDatabase::getAllRouterLabelData( int iSrcNum, char szAutoName[BUFFER_CONST], char szManName[BUFFER_CONST],
									int *iAutoStatus, int *iPermStatus, char szLabel[BUFFER_CONST], int *iDBEntry, int *iListView )
{
	// take in src number and if found then return data linked to it
	for (int iIndex=1;iIndex<=m_iMaxRouterLabelsUsed;iIndex++) {
		if (stRouterAliasData[iIndex].m_iSourceNumber == iSrcNum) {
			strcpy( szAutoName, stRouterAliasData[iIndex].szAutomaticName );
			strcpy( szManName, stRouterAliasData[iIndex].szManualName );
			*iAutoStatus = stRouterAliasData[iIndex].m_iAutomaticManualStatus;
			*iPermStatus = stRouterAliasData[iIndex].m_iPermTempStatus;
			strcpy( szLabel, stRouterAliasData[iIndex].szSncsLabel );
			*iDBEntry = stRouterAliasData[iIndex].m_iDBIndexNumber;
			*iListView = stRouterAliasData[iIndex].m_iListViewIndexNumber;
			return TRUE;
		}
	}
	OutputDebugString(" BFEUMDLabel getAllrouterLabelData 2 - src number not found  \n");
	return FALSE;
}


	
