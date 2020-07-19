// SoftNameData.cpp: implementation of the SoftNameData class.
//
//////////////////////////////////////////////////////////////////////

#include "SoftNameData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SoftNameData::SoftNameData()
{
	// init on creation
	m_iMaxSNCSLabelsUsed = 200;
	m_iMaxRouterLabelsUsed = 250;
	for (int i=0;i<MAXLABELS; i++) {
		stSNCSAliasData[i].m_iIndexNumber = 0;
		stSNCSAliasData[i].m_iAutomaticManualStatus = "AUTO";
		stSNCSAliasData[i].m_iPermTempStatus = "TEMP";
		stSNCSAliasData[i].szAliasName = "";
		stSNCSAliasData[i].szAutomaticName = "";
		stSNCSAliasData[i].szManualName = "";
		stSNCSAliasData[i].sncsLinkedtoRouterData = bncs_stringlist("");

		stRouterAliasData[i].m_iSourceNumber = i;
		stRouterAliasData[i].m_iAutomaticManualStatus = "AUTO";
		stRouterAliasData[i].m_iPermTempStatus = "TEMP";
		stRouterAliasData[i].szAutomaticName = " ";
		stRouterAliasData[i].szManualName = " ";
		stRouterAliasData[i].LinkedtoSncsData = "";
	} 
}

SoftNameData::~SoftNameData()
{
	// seek and destroy
}



int SoftNameData::getMaxSncsLabelsInUse(void)
{
	return m_iMaxSNCSLabelsUsed;
}


int SoftNameData::getMaxRouterLabelsInUse(void)
{
	return m_iMaxRouterLabelsUsed;
}


BOOL SoftNameData::storeSncsLabelData( int iIndex, bncs_string szNewData )
{
	// take in a whole string and split it up and set fields in structure for sncs data
	char szData[BUFFER_CONST], szTmp[BUFFER_CONST]="", szStr[BUFFER_CONST], *tok; 
	int i, iTokCount = 0, iVal=0;

	bncs_string szName ="$NAME", szAuto =" ", szMan =" ", szForm ="TEMP", szStyle ="AUTO";
	bncs_stringlist ssl=bncs_stringlist("", '~' );
	bool bContinue = TRUE;
	char *pdest;

	strcpy(szData, szNewData);
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
			if (i>2) {
				szName = szStr;
			}
			break;
		case 2:
			if (i>2) {
				szAuto = szStr;
			}
			break;
		case 3:
			if (i>2) {
				szMan = szStr;
			}
		    break;
		case 4:
			pdest = strstr(szStr, "MANUAL");
			if (pdest!=NULL)
				szStyle = "MANUAL";
		    break;
		case 5:
			pdest = strstr(szStr, "PERM");
			if (pdest!=NULL)
				szForm = "PERM";
		    break;
		case 6:
			ssl = bncs_stringlist(szStr, ',' );
		    break;
		default:
			bContinue=FALSE; // too many items -- process what we have extracted
		}
		tok=strtok(NULL,"~"); 				
	}// while
	if (iTokCount>5) { // min 6
		if ((iIndex>0)&&(iIndex<MAXLABELS)) {
			stSNCSAliasData[iIndex].m_iAutomaticManualStatus = szStyle;
			stSNCSAliasData[iIndex].m_iPermTempStatus = szForm;
			stSNCSAliasData[iIndex].szAliasName = szName;
			stSNCSAliasData[iIndex].szAutomaticName = szAuto;
			stSNCSAliasData[iIndex].szManualName = szMan;
			stSNCSAliasData[iIndex].sncsLinkedtoRouterData = ssl;	
			return TRUE;
		}
		else {
			OutputDebugString(" SoftNameData storeSncsLabelData out of range to store \n");
		}
	}
	else {
			OutputDebugString(" SoftNameData storeSncsLabelData tokcount too low \n");
	}

	return FALSE;
}


BOOL SoftNameData::storeRouterLabelData( int iIndex, bncs_string szNewData )
{
	// take in a whole string and split it up and set fields in structure for sncs data
	char szData[BUFFER_CONST]="", szTmp[BUFFER_CONST]="", szStr[BUFFER_CONST]="                    ", *tok; 
	int i, iTokCount = 0, iVal=0, iSourceNumber=0;

	bncs_string szName ="$NAME", szAuto ="                     ", szMan ="                    ", szForm ="TEMP";
	bncs_string szLabel= "", szStyle ="AUTO";
	bool bContinue = TRUE;
	char *pdest;

	strcpy(szData, szNewData);
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
			iSourceNumber = atoi(szStr);
			break;
		case 2:
			 szAuto=szStr;
			break;
		case 3:
			szMan=szStr;
		    break;
		case 4:
			pdest = strstr(szStr, "MANUAL");
			if (pdest!=NULL)
				szStyle = "MANUAL";
		    break;
		case 5:
			pdest = strstr(szStr, "PERM");
			if (pdest!=NULL)
				szForm = "PERM";
		    break;
		case 6:
			szLabel = szStr;
		    break;
		default:
			bContinue=FALSE; // too many items -- process what we have extracted
		}
		tok=strtok(NULL,"~"); 				
	}// while
	if (iTokCount>5) { // 6 minimium
		if ((iIndex>0)&&(iIndex<MAXLABELS)) {
			stRouterAliasData[iIndex].m_iAutomaticManualStatus = szStyle;
			stRouterAliasData[iIndex].m_iPermTempStatus = szForm;
			stRouterAliasData[iIndex].m_iSourceNumber = iSourceNumber;
			stRouterAliasData[iIndex].szAutomaticName = szAuto;
			stRouterAliasData[iIndex].szManualName = szMan;
			stRouterAliasData[iIndex].LinkedtoSncsData = szLabel;	

			return TRUE;	
		}
		else {
			OutputDebugString(" SoftNameData storeRouterLabelData out of range to store \n");
		}
	}
	else {
		OutputDebugString(" SoftNameData storeRouterLabelData tokcount too low \n");
	}
	return FALSE;
}


BOOL SoftNameData::getAllSncsLabelData( int iIndex, bncs_string *szAlias, bncs_string *szSNCSName, bncs_string *iPermStatus, bncs_stringlist *sslist )
{
	// take in index number and return all sncs data for that index
	if ((iIndex>0)&&(iIndex<=m_iMaxSNCSLabelsUsed)) {
		*iPermStatus = stSNCSAliasData[iIndex].m_iPermTempStatus;
		*szAlias = stSNCSAliasData[iIndex].szAliasName;
		*szSNCSName = stSNCSAliasData[iIndex].szAutomaticName;
		*sslist = stSNCSAliasData[iIndex].sncsLinkedtoRouterData;
		return TRUE;
	}
	else
		OutputDebugString(" SoftNameData getAllSncsLabelData 1 - index out of range to retieve \n");
	return FALSE;
}


BOOL SoftNameData::getAllSncsLabelData( bncs_string szAlias, int *iSNCSIndex, bncs_string *szSNCSName,  bncs_string *iPermStatus, bncs_stringlist *sslist )
{
	// given an SNCS alias name  -- find it and return associated data
	for (int iIndex=1;iIndex<=m_iMaxSNCSLabelsUsed;iIndex++) {
		if (stSNCSAliasData[iIndex].szAliasName==szAlias) {
			// entry found
			*iSNCSIndex = iIndex;
			*iPermStatus = stSNCSAliasData[iIndex].m_iPermTempStatus;
			*szSNCSName = stSNCSAliasData[iIndex].szAutomaticName;
			*sslist = stSNCSAliasData[iIndex].sncsLinkedtoRouterData;
			return TRUE;
		}
	}
	OutputDebugString(" SoftNameData getAllSncsLabelData 2 - name not found to retieve \n");
	return FALSE;
}

	
BOOL SoftNameData::getAllRouterLabelData( int iIndex, int * iSrcNum, bncs_string *szAutoName, bncs_string *szManName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_string *ssLabel )
{
	// take in indexnuber and return associated data
	if ((iIndex>0)&&(iIndex<=m_iMaxRouterLabelsUsed)) {
		*iSrcNum = stRouterAliasData[iIndex].m_iSourceNumber;
		*iAutoStatus = stRouterAliasData[iIndex].m_iAutomaticManualStatus;
		*iPermStatus = stRouterAliasData[iIndex].m_iPermTempStatus;
		*szAutoName = stRouterAliasData[iIndex].szAutomaticName;
		*szManName = stRouterAliasData[iIndex].szManualName;
		*ssLabel = stRouterAliasData[iIndex].LinkedtoSncsData;
		return TRUE;
	}
	else
		OutputDebugString(" SoftNameData getAllRouterLabelData out of range to retieve \n");
	return FALSE;
}


	
BOOL SoftNameData::getAllRouterLabelData( int iSrcNum, bncs_string *szAutoName, bncs_string *szManName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_string *ssLabel )
{
	// take in src number and if found then return data linked to it
	for (int iIndex=1;iIndex<=m_iMaxRouterLabelsUsed;iIndex++) {
		if (stRouterAliasData[iIndex].m_iSourceNumber == iSrcNum) {
			*iAutoStatus = stRouterAliasData[iIndex].m_iAutomaticManualStatus;
			*iPermStatus = stRouterAliasData[iIndex].m_iPermTempStatus;
			*szAutoName = stRouterAliasData[iIndex].szAutomaticName;
			*szManName = stRouterAliasData[iIndex].szManualName;
			*ssLabel = stRouterAliasData[iIndex].LinkedtoSncsData;
			return TRUE;
		}
	}
	OutputDebugString(" SoftNameData getAllrouterLabelData 2 - src number not found  \n");
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////

BOOL SoftNameData::setSourceSncsLabel( int iSourceNumber, bncs_string ssSncsLabel )
{
	// take in src number and if found then return data linked to it
	for (int iIndex=1;iIndex<=m_iMaxRouterLabelsUsed;iIndex++) {
		if (stRouterAliasData[iIndex].m_iSourceNumber == iSourceNumber) {
			stRouterAliasData[iIndex].LinkedtoSncsData = ssSncsLabel;
			return TRUE;
		}
	}
	return FALSE;
}
	

bncs_string SoftNameData::getSourceSncsLabel( int iSourceNumber )
{
	// take in src number and if found then return data linked to it
	for (int iIndex=1;iIndex<=m_iMaxRouterLabelsUsed;iIndex++) {
		if (stRouterAliasData[iIndex].m_iSourceNumber == iSourceNumber) 
			return stRouterAliasData[iIndex].LinkedtoSncsData;
	}
	return 0;
}


int SoftNameData::findNextFreeIndex( bncs_string ssNewLabel )
{
	// find the next unsed slot ( $NAME ) or id table full then reuse first UNASSIGNED ( ie has no alias ) slot
	// or first slot that has assigned one but not allocated  to an OS 
	// if all assigned then return ERROR and flag on panels
	int iNewIndex=0;
	bool bkeepLooking = TRUE;
	// first $NAME space
	for (iNewIndex=1;iNewIndex<MAXLABELS;iNewIndex++) {
		if (stSNCSAliasData[iNewIndex].szAliasName=="$NAME") {
			bkeepLooking=FALSE;
			stSNCSAliasData[iNewIndex].szAliasName = ssNewLabel;
			stSNCSAliasData[iNewIndex].szAutomaticName = " ";
			stSNCSAliasData[iNewIndex].szManualName = " ";
			stSNCSAliasData[iNewIndex].m_iPermTempStatus = "TEMP";
			stSNCSAliasData[iNewIndex].m_iAutomaticManualStatus = "AUTO";
			return iNewIndex;
		}
	}
	// got to end and no $NAME -- ie all slots have labels -- find first label with no assigned name
	for (iNewIndex=1;iNewIndex<MAXLABELS;iNewIndex++) {
		if (stSNCSAliasData[iNewIndex].m_iPermTempStatus == "TEMP") {
			bncs_string sstr = stSNCSAliasData[iNewIndex].szManualName.stripWhiteSpace();
			if (sstr.length()==0) {
				bkeepLooking=FALSE;
				stSNCSAliasData[iNewIndex].szAliasName = ssNewLabel;
				stSNCSAliasData[iNewIndex].szAutomaticName = " ";
				stSNCSAliasData[iNewIndex].szManualName = " ";
				stSNCSAliasData[iNewIndex].m_iPermTempStatus = "TEMP";
				stSNCSAliasData[iNewIndex].m_iAutomaticManualStatus = "AUTO";
				return iNewIndex;
			}
		}
	}
	// still no slot found FLAG MAJOR ERROR
	return -1;
}



//////////////////////////////////////////////////////////////////////////
