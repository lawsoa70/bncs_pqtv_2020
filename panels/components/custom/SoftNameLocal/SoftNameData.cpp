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
	m_iMaxRouterLabelsUsed = 256;
	for (int i=0;i<MAXLABELS; i++) {

		stRouterAliasData[i].m_iListViewIndexNumber = -1;
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


int SoftNameData::getMaxRouterLabelsInUse(void)
{
	return m_iMaxRouterLabelsUsed;
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

	
BOOL SoftNameData::getAllRouterLabelData( int iIndex, int * iSrcNum, bncs_string *szAutoName, bncs_string *szManName,
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_string *ssLabel, int * iListView )
{
	// take in indexnuber and return associated data
	if ((iIndex>0)&&(iIndex<=m_iMaxRouterLabelsUsed)) {
		*iSrcNum = stRouterAliasData[iIndex].m_iSourceNumber;
		*iAutoStatus = stRouterAliasData[iIndex].m_iAutomaticManualStatus;
		*iPermStatus = stRouterAliasData[iIndex].m_iPermTempStatus;
		*iListView = stRouterAliasData[iIndex].m_iListViewIndexNumber;
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
									bncs_string *iAutoStatus, bncs_string *iPermStatus, bncs_string *ssLabel, int * iListView )
{
	// take in src number and if found then return data linked to it
	for (int iIndex=1;iIndex<=m_iMaxRouterLabelsUsed;iIndex++) {
		if (stRouterAliasData[iIndex].m_iSourceNumber == iSrcNum) {
			*iAutoStatus = stRouterAliasData[iIndex].m_iAutomaticManualStatus;
			*iPermStatus = stRouterAliasData[iIndex].m_iPermTempStatus;
			*iListView = stRouterAliasData[iIndex].m_iListViewIndexNumber;
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
