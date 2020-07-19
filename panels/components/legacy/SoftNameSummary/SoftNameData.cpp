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
	m_iMaxSNCSLabelsUsed = MAXLABELS;
	m_iMaxRouterLabelsUsed = MAXLABELS;
	for (int i=0;i<MAXLABELS; i++) {
		stSNCSAliasData[i].m_iIndexNumber = 0;
		stSNCSAliasData[i].m_iListViewIndexNumber = -1;
		stSNCSAliasData[i].m_iAutomaticManualStatus = "NONE";
		stSNCSAliasData[i].m_iPermTempStatus = "NONE";
		stSNCSAliasData[i].szAliasName = "";
		stSNCSAliasData[i].szAutomaticName = "";
		stSNCSAliasData[i].szManualName = "";
		stSNCSAliasData[i].sncsLinkedtoRouterData = bncs_stringlist("");

		stRouterAliasData[i].m_iListViewIndexNumber = -1;
		stRouterAliasData[i].m_iSourceNumber = i;
		stRouterAliasData[i].m_iAutomaticManualStatus = "NONE";
		stRouterAliasData[i].m_iPermTempStatus = "NONE";
		stRouterAliasData[i].szAutomaticName = "";
		stRouterAliasData[i].szManualName = "";
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
			stSNCSAliasData[iIndex].m_iListViewIndexNumber = iIndex-1;
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
	char szData[BUFFER_CONST]="", szTmp[BUFFER_CONST]="", szStr[BUFFER_CONST]="                    ",  *tok; 
	int i, iTokCount = 0, iVal=0, iSourceNumber=0;

	bncs_string szName ="$NAME", szAuto =" ", szMan ="", szForm ="TEMP";
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
			stRouterAliasData[iIndex].m_iListViewIndexNumber = iIndex-1;
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

	


BOOL SoftNameData::getAllSncsLabelData( int iIndex, bncs_string *szAlias, bncs_string *szAutoName, bncs_string *szManName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_stringlist *sslist )
{
	// take in index number and return all sncs data for that index
	if ((iIndex>0)&&(iIndex<=m_iMaxSNCSLabelsUsed)) {
		*iAutoStatus = stSNCSAliasData[iIndex].m_iAutomaticManualStatus;
		*iPermStatus = stSNCSAliasData[iIndex].m_iPermTempStatus;
		*szAlias = stSNCSAliasData[iIndex].szAliasName;
		*szAutoName = stSNCSAliasData[iIndex].szAutomaticName;
		*szManName = stSNCSAliasData[iIndex].szManualName;
		*sslist = stSNCSAliasData[iIndex].sncsLinkedtoRouterData;
		return TRUE;
	}
	else
		OutputDebugString(" SoftNameData getAllSncsLabelData 1 - index out of range to retieve \n");
	return FALSE;
}

BOOL SoftNameData::getAllSncsLabelData( bncs_string szAlias, bncs_string *szAutoName, bncs_string *szManName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_stringlist *sslist )
{
	// given an SNCS alias name  -- find it and return associated data
	for (int iIndex=1;iIndex<=m_iMaxSNCSLabelsUsed;iIndex++) {
		if (stSNCSAliasData[iIndex].szAliasName==szAlias) {
			// entry found
			*iAutoStatus = stSNCSAliasData[iIndex].m_iAutomaticManualStatus;
			*iPermStatus = stSNCSAliasData[iIndex].m_iPermTempStatus;
			*szAutoName = stSNCSAliasData[iIndex].szAutomaticName;
			*szManName = stSNCSAliasData[iIndex].szManualName;
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
	char szdeb[BUFFER_CONST];
	wsprintf( szdeb, " summ get rtr %d \n", iSrcNum );
	OutputDebugString( szdeb);
	OutputDebugString(" SoftNameData getAllrouterLabelData 2 - src number not found  \n");
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
