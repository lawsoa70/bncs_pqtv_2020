#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "SoftNameExternal.h"

#define MAIN_PANEL 1
#define ASSIGN_PANEL 2

#define AUTONAME 1
#define MANUALNAME 2



// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( SoftNameExternal )

// constructor - equivalent to ApplCore STARTUP
SoftNameExternal::SoftNameExternal( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	bncs_string ssStr;
	//
	iCurrentIndex=-1;
	iCurrentSource = -1;
	iCurrentSNCSAliasIndex = -1;
	iCurrentEditingWhichName = 0;
	bPanelStarting = TRUE;

	// declare data storage
	allSoftData = new SoftNameData;

}

// destructor - equivalent to ApplCore CLOSEDOWN
SoftNameExternal::~SoftNameExternal()
{
}

// all button pushes and notifications come here
void SoftNameExternal::buttonCallback( buttonNotify *b )
{
	bncs_string prevsstr, sstr, sstr2, sstr3 ;

	if( b->panel() == MAIN_PANEL ) {
		if (b->id()=="ClearAll") {
			// clear all soft names for all External sources -- if temp -- which they all should be
			ClearAllExternalSoftNames();
			ResetPanel();
		}
		else if (b->id()=="ClearIt") {
			// if item is selected then clear soft name for item
			if (iCurrentEditingWhichName>0) {
				ClearExternalSoftName( iCurrentSource );
				//ResetPanel();
			}	
		}
		else if (b->id()=="ChosenAutoName") {
			if (iCurrentIndex>=0) {
				// if item is selected then clear soft name for item
				textGet( "text", MAIN_PANEL, "ChosenAutoName", sstr  );
				textPut( "stylesheet", "enum_selected", MAIN_PANEL, "ChosenAutoName" );
				textPut( "stylesheet", "source_deselected", MAIN_PANEL, "ChosenManualName" );
				textPut( "text", sstr, MAIN_PANEL, "ChosenSoftName1" );
				textPut( "text", "", MAIN_PANEL, "ChosenSoftName2" );
				textPut( "text", "", MAIN_PANEL, "KEYBOARD" );
				iCurrentEditingWhichName = AUTONAME;
			}
		}
		else if (b->id()=="ChosenManualName") {
			if (iCurrentIndex>=0) {
				// if item is selected then clear soft name for item
				textGet( "text", MAIN_PANEL, "ChosenManualName", sstr  );
				textPut( "stylesheet", "enum_selected", MAIN_PANEL, "ChosenManualName" );
				textPut( "stylesheet", "source_deselected", MAIN_PANEL, "ChosenAutoName" );
				textPut( "text", sstr, MAIN_PANEL, "ChosenSoftName1" );
				textPut( "text", "", MAIN_PANEL, "ChosenSoftName2" );
				textPut( "text", "", MAIN_PANEL, "KEYBOARD" );
				iCurrentEditingWhichName = MANUALNAME;
			}
		}
		else if (b->id()=="ChosenStyle") {
			if (iCurrentIndex>=0) {
				ChangeCurrentStyle(iCurrentSource);
			}
		}
		else if (b->id()=="ChosenForm") {
			if (iCurrentIndex>=0) {
				ChangeCurrentForm(iCurrentSource);
			}
		}
		else if (b->id()=="Commit") {
			// if item is selected then clear soft name for item
			if (iCurrentIndex>=0) {
				SaveExternalSoftName( iCurrentSource );
				//ResetPanel();
			}
		}
		else if (b->id()=="Abandon") {
			// clear selected items, vars etc
			ResetPanel();
		}
		else if (b->id()=="ASSIGNLABEL") {
			// bring up assign panel
			panelShow( ASSIGN_PANEL, "assignlabel.bncs_ui" );
			PopulateAssignPanel( iCurrentSource );
		}
		else if (b->id()=="KEYBOARD") { 
			// keyboard control
			textGet( "text", MAIN_PANEL, "ChosenSoftName2", prevsstr  );
			textGet( "text", MAIN_PANEL, "KEYBOARD", sstr  );
			if (prevsstr==sstr) {
				if (iCurrentIndex>=0) SaveExternalSoftName( iCurrentSource );
			}
			else { // different so char added etc
				//if (sstr.length()<=8) {
					char *pdest = const_cast<char*>( strchr( sstr, '~' ) );
					if (pdest==NULL) {
						textPut( "text", sstr, MAIN_PANEL, "ChosenSoftName2"  );
						textPut( "text", " ", MAIN_PANEL, "MAXCHARS"  );
					}
					else {
						textGet( "text", MAIN_PANEL, "ChosenSoftName2", sstr  );
						textPut( "text", sstr, MAIN_PANEL, "KEYBOARD"  );
					}
				//}
				/*
				if (sstr.length()>=8) {
					textPut( "text", "Note- Limit of 8|Letters reached !!", MAIN_PANEL, "MAXCHARS"  );
					textGet( "text", MAIN_PANEL, "ChosenSoftName2", sstr  );
					textPut( "text", sstr, MAIN_PANEL, "KEYBOARD"  );
				}
				*/
			}
		}
		else if ((b->id()>=1)&&(b->id()<=20)) {
			SetUpForIndex(b->id());
		}
		else if ((b->id()>=101)&&(b->id()<=120)) {
			int iVal = b->id();
			int iSrc = ssl_ExternalSources[(iVal-101)];
			ChangeCurrentStyle(iSrc);
		}
	}
	else if ( b->panel() == ASSIGN_PANEL ) {
		if (b->id()=="CLOSEPANEL") {
			// clear selections
			textPut("text", "", ASSIGN_PANEL, "SOURCENAME" );
			textPut("text", "", ASSIGN_PANEL, "LABELNAME" );
			textPut("text", "", ASSIGN_PANEL, "NEWLABEL" );
			panelRemove(ASSIGN_PANEL);
		}
		else if (b->id()=="DEASSIGN") {
			// clear any label assoc to src
			textPut("text", "", ASSIGN_PANEL, "LABELNAME" );
			DeassignSNCSLabel(iCurrentSource);
		}
		else if (b->id()=="LabelList") {
			// get new label assoc to src
			textGet( "selected", ASSIGN_PANEL, "LabelList", sstr  );
			textPut( "text", sstr, ASSIGN_PANEL, "NEWLABEL");
		}
		else if (b->id()=="ASSIGNNEW") {
			// assign new label assoc to src
			textGet( "selected", ASSIGN_PANEL, "LabelList", sstr  );
			sstr.split('-', sstr2, sstr3);
			sstr = sstr2.simplifyWhiteSpace(); // is now the new label
			textPut( "text", sstr, ASSIGN_PANEL, "LABELNAME");
			AssignSNCSLabel(iCurrentSource, sstr);
		}
	}
}

void SoftNameExternal::SetUpForIndex( int iIndex )
{
	for (int i=1;i<=20;i++)	textPut( "stylesheet", "source_deselected", MAIN_PANEL, i );
	textPut( "stylesheet", "enum_selected", MAIN_PANEL, iIndex);
	iCurrentIndex=iIndex;
	iCurrentEditingWhichName = 0;
	if ((iCurrentIndex>0)&&(iCurrentIndex<=ssl_ExternalSources.count()))
		iCurrentSource = ssl_ExternalSources[(iCurrentIndex-1)];
	textPut( "text",iCurrentSource,  MAIN_PANEL, "ChosenNumber" );
	controlEnable(MAIN_PANEL, "ASSIGNLABEL");
	controlEnable(MAIN_PANEL, "Abandon");
	controlEnable(MAIN_PANEL, "Commit");
	DisplayChosenSourceData( true );
}


void SoftNameExternal::DisplayChosenSourceData( bool setupdefaultedit )
{
	bncs_string szSrcName, szAuto, szMan, iAuto, iPerm, sstr, ssLabel;
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iCurrentSource, &szAuto, &szMan, &iAuto, &iPerm, &ssLabel )) {
		
		routerName( iRouterDevice, 0, iCurrentSource, szSrcName );
		textPut( "text", szSrcName, MAIN_PANEL, "ChosenSource" );
		textPut( "text", szSrcName, ASSIGN_PANEL, "SOURCENAME" );

		if (ssLabel.length()==0) {
			// no associated alias so use auto name as found
			textPut( "text", "", MAIN_PANEL, "ChosenAlias" );
		}
		else {
			textPut( "text", ssLabel, MAIN_PANEL, "ChosenAlias" );
		}

		textPut( "text", szAuto, MAIN_PANEL, "ChosenAutoName" );
		textPut( "text", szMan, MAIN_PANEL, "ChosenManualName" );
		ssChosenCurrentStyle = iAuto;
		ssChosenCurrentForm = iPerm;
		if (iAuto=="MANUAL") {
			// colour button red
			textPut( "stylesheet", "topstrap_red_on", MAIN_PANEL, "ChosenStyle" );
			textPut( "text", " MANUAL", MAIN_PANEL, "ChosenStyle" );
			if (setupdefaultedit==true) {
				textPut( "stylesheet", "enum_selected", MAIN_PANEL, "ChosenManualName" );
				textPut( "stylesheet", "source_deselected", MAIN_PANEL, "ChosenAutoName" );
				textGet( "text", MAIN_PANEL, "ChosenManualName", sstr  );
				textPut( "text", sstr, MAIN_PANEL, "ChosenSoftName1" );
				iCurrentEditingWhichName = MANUALNAME;
			}
		}
		else {
			// colour blue
			textPut( "stylesheet", "topstrap_reh_on", MAIN_PANEL, "ChosenStyle" );
			textPut( "text", " AUTO", MAIN_PANEL, "ChosenStyle" );
			if (setupdefaultedit==true) {
				textPut( "stylesheet", "enum_selected", MAIN_PANEL, "ChosenAutoName" );
				textPut( "stylesheet", "source_deselected", MAIN_PANEL, "ChosenManualName" );
				textGet( "text", MAIN_PANEL, "ChosenAutoName", sstr  );
				textPut( "text", sstr, MAIN_PANEL, "ChosenSoftName1" );
				iCurrentEditingWhichName = AUTONAME;
			}
		}
		if (iPerm=="PERM") {
			// colour button red 
			textPut( "stylesheet", "topstrap_red_on", MAIN_PANEL, "ChosenForm" );
			textPut( "text", " PERM", MAIN_PANEL, "ChosenForm" );
		}
		else {
			// colour blue
			textPut( "stylesheet", "topstrap_reh_on", MAIN_PANEL, "ChosenForm" );
			textPut( "text", " TEMP", MAIN_PANEL, "ChosenForm" );
		}
	}

}


void SoftNameExternal::PopulateAssignPanel( int iSource )
{
	bncs_string szSrcName, szAlias, szSNCS, szAuto, szMan, iAuto, iPerm, ssSncsAuto="??", sstr, ssLabel;
	bncs_stringlist ssl;
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iSource, &szAuto, &szMan, &iAuto, &iPerm, &ssLabel )) {
		routerName( iRouterDevice, 0, iSource, szSrcName );
		textPut( "text", szSrcName, ASSIGN_PANEL, "SOURCENAME" );
		textPut( "text", ssLabel, ASSIGN_PANEL, "LABELNAME" );
		int iMaxSncs=allSoftData->getMaxSncsLabelsInUse();
		// fill list box
		textPut( "clear", ASSIGN_PANEL, "LabelList" );	
		for (int iSncsIndex=1;iSncsIndex<=200;iSncsIndex++) {
			allSoftData->getAllSncsLabelData(iSncsIndex, &szAlias, &szSNCS, &iPerm, &ssl);
			if (szAlias!="$NAME") {
				textPut("add", bncs_string(szAlias + "   -   " + szSNCS), ASSIGN_PANEL, "LabelList" ); 
			}
			//if (ssLabel==szAlias) textPut( "text", szAlias, ASSIGN_PANEL, "LABELNAME" );
		}
	}
}


void SoftNameExternal::ChangeCurrentStyle( int iSource ) 
{
	bncs_string ssSrcName, ssAutoName, ssManName, ssAuto, ssPerm, ssLabel;
	char szsz[BUFFER_CONST]= "", szAuto[BUFFER_CONST]= "", szMan[BUFFER_CONST]= "";
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iSource, &ssAutoName, &ssManName, &ssAuto, &ssPerm, &ssLabel )) {
		if (ssAuto=="MANUAL") 
			ssAuto = "PERM";
		else
			ssAuto = "MANUAL";
		//now write out to slot
		strcpy( szAuto, ssAutoName );
		strcpy( szMan, ssManName );
		// need to know which name was editied
		wsprintf( szsz, "~<%04d>~<%s>~<%s>", iSource, szAuto, szMan );
		if (ssAuto=="MANUAL")  strcat( szsz, "~<MANUAL>" );
		else strcat( szsz, "~<AUTO>" );
		if (ssPerm=="PERM") 	strcat( szsz, "~<PERM>~<" );
		else	strcat( szsz, "~<TEMP>~<" );
		strcat(szsz, ssLabel );
		strcat( szsz, ">");
		infoWrite(iLabelAutoDevice, szsz, iSource, false );
	}
}


void SoftNameExternal::ChangeCurrentForm( int iSource  )
{
	bncs_string szssSrcName, ssAutoName, ssManName, ssAuto, ssPerm, ssLabel;
	char szsz[BUFFER_CONST]= "", szAuto[BUFFER_CONST]= "", szMan[BUFFER_CONST]= "";
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iSource, &ssAutoName, &ssManName, &ssAuto, &ssPerm, &ssLabel )) {
		if (ssPerm=="PERM") 
			ssPerm = "TEMP";
		else
			ssPerm = "PERM";
		//now write out to slot
		strcpy( szAuto, ssAutoName );
		strcpy( szMan, ssManName );
		// need to know which name was editied
		wsprintf( szsz, "~<%04d>~<%s>~<%s>", iSource, szAuto, szMan );
		if (ssAuto=="MANUAL")  strcat( szsz, "~<MANUAL>" );
		else strcat( szsz, "~<AUTO>" );
		if (ssPerm=="PERM") 	strcat( szsz, "~<PERM>~<" );
		else	strcat( szsz, "~<TEMP>~<" );
		strcat(szsz, ssLabel );
		strcat( szsz, ">");
		infoWrite(iLabelAutoDevice, szsz, iSource, false );
	}
}

void SoftNameExternal::DeassignSNCSLabel( int iSource  ) {
	bncs_string ssSrcName, ssAutoName, ssManName, ssAuto, ssPerm, ssLabel;
	char szsz[BUFFER_CONST]= "", szAuto[BUFFER_CONST]= "", szMan[BUFFER_CONST]= "";
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iSource, &ssAutoName, &ssManName, &ssAuto, &ssPerm, &ssLabel )) {
		allSoftData->setSourceSncsLabel(iCurrentSource, "" );
		textPut( "text", "", ASSIGN_PANEL, "SNCSLABEL" );		
		if (ssPerm=="PERM") 
			ssPerm = "PERM";
		else
			ssPerm = "TEMP";
		//now write out to slot
		strcpy( szAuto, ssAutoName );
		strcpy( szMan, ssManName );
		// need to know which name was editied
		wsprintf( szsz, "~<%04d>~< >~<%s>", iSource, szMan );
		if (ssAuto=="MANUAL")  strcat( szsz, "~<MANUAL>" );
		else strcat( szsz, "~<AUTO>" );
		if (ssPerm=="PERM") 	strcat( szsz, "~<PERM>~<" );
		else	strcat( szsz, "~<TEMP>~< >" );
		infoWrite(iLabelAutoDevice, szsz, iSource, false );
	}
}

void SoftNameExternal::AssignSNCSLabel( int iSource, bncs_string ssNewLabel  ) {
	bncs_string ssSrcName, ssAutoName, ssManName, ssAuto, ssPerm, ssLabel;
	char szsz[BUFFER_CONST]= "", szAuto[BUFFER_CONST]= "", szMan[BUFFER_CONST]= "";
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iSource, &ssAutoName, &ssManName, &ssAuto, &ssPerm, &ssLabel )) {
		//now write out to slot
		strcpy( szAuto, ssNewLabel );
		strcpy( szMan, ssManName );
		// need to know which name was editied
		wsprintf( szsz, "~<%04d>~<%s>~<%s>", iSource, szAuto, szMan );
		if (ssAuto=="MANUAL")  strcat( szsz, "~<MANUAL>" );
		else strcat( szsz, "~<AUTO>" );
		if (ssPerm=="PERM") 	strcat( szsz, "~<PERM>~<" );
		else	strcat( szsz, "~<TEMP>~<" );
		strcat(szsz, ssNewLabel );
		strcat( szsz, ">");
		infoWrite(iLabelAutoDevice, szsz, iSource, false );
	}
}


void SoftNameExternal::ResetPanel( void )
{
	for (int i=1;i<=20;i++)	textPut( "stylesheet", "source_deselected", MAIN_PANEL, i );
	controlDisable(MAIN_PANEL, "ASSIGNLABEL");
	controlDisable(MAIN_PANEL, "Abandon");
	controlDisable(MAIN_PANEL, "Commit");
	textPut( "text", "", MAIN_PANEL, "ChosenSource" );
	textPut( "text", "", MAIN_PANEL, "ChosenNumber" );
	textPut( "text", "", MAIN_PANEL, "ChosenAutoName" );
	textPut( "text", "", MAIN_PANEL, "ChosenManualName" );
	textPut( "text", "", MAIN_PANEL, "ChosenSoftName1" );
	textPut( "text", "", MAIN_PANEL, "ChosenSoftName2" );
	textPut( "text", "", MAIN_PANEL, "KEYBOARD" );
	textPut("text", "", MAIN_PANEL, "ChosenStyle" );
	textPut("text", "", MAIN_PANEL, "ChosenForm" );
	textPut("stylesheet", "source_deselected", MAIN_PANEL, "ChosenStyle" );
	textPut("stylesheet", "source_deselected", MAIN_PANEL, "ChosenForm" );
	textPut("stylesheet", "source_deselected", MAIN_PANEL, "ChosenAutoName" );
	textPut("stylesheet", "source_deselected", MAIN_PANEL, "ChosenManualName" );
	textPut("text", "", MAIN_PANEL, "ChosenAlias" );
	textPut("text", "", MAIN_PANEL, "AliasSourceList" );
	textPut("text", "", MAIN_PANEL, "MAXCHARS" );
	iCurrentIndex=-1;
	iCurrentSource=-1;
	iCurrentSNCSAliasIndex=-1;
	iCurrentEditingWhichName = 0;
	ssChosenCurrentStyle = "??";
	ssChosenCurrentForm = "??";
}


void SoftNameExternal::ClearAllExternalSoftNames(void) 
{
	for (int iIndex=0;iIndex<ssl_ExternalSources.count();iIndex++) {
		ClearExternalSoftName(ssl_ExternalSources[iIndex]);
	}
}


void SoftNameExternal::ClearExternalSoftName(int iSource) 
{
	bncs_string ssSrcName, ssNewName, ssAuto, ssMan, iAuto, iPerm, ssLabel;
	bncs_stringlist sslist;
	char szsz[BUFFER_CONST]= "", szAdd[BUFFER_CONST]= "", szAuto[BUFFER_CONST]= "", szMan[BUFFER_CONST]= "";
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iSource, &ssAuto, &ssMan, &iAuto, &iPerm, &ssLabel )) {
		//get new name
		strcpy( szAuto, ssAuto );
		strcpy( szMan, ssMan );
		// need to know which name was editied
		if (iCurrentEditingWhichName==AUTONAME)
			wsprintf( szsz, "~<%04d>~< >~<%s>", iSource, szMan );
		else if (iCurrentEditingWhichName==MANUALNAME)
			wsprintf( szsz, "~<%04d>~<%s>~< >", iSource, szAuto );
		if (iAuto=="MANUAL") 
			strcat( szsz, "~<MANUAL>" );
		else
			strcat( szsz, "~<AUTO>" );
		if (iAuto=="PERM") 
			strcat( szsz, "~<PERM>~< >" );
		else
			strcat( szsz, "~<TEMP>~< >" );
		infoWrite(iLabelAutoDevice, szsz, iSource, false );
	}
}


void SoftNameExternal::SaveExternalSoftName(int iSource ) 
{
	bncs_string ssSrcName, ssNewName, ssAuto, ssMan, iAuto, iPerm, ssLabel;
	char  szsz[BUFFER_CONST]= "", szAdd[BUFFER_CONST]= "", szAuto[BUFFER_CONST]= "", szMan[BUFFER_CONST]= "";
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iSource, &ssAuto, &ssMan, &iAuto, &iPerm, &ssLabel )) {
		//get new name
		textGet( "text", MAIN_PANEL, "ChosenSoftName2", ssNewName  );
		if (ssNewName.length()==0) ssNewName = " ";
		strcpy( szAdd, ssNewName );
		strcpy( szAuto, ssAuto );
		strcpy( szMan, ssMan );
		// need to know which name was editied
		if (iCurrentEditingWhichName==AUTONAME)
			wsprintf( szsz, "~<%04d>~<%s>~<%s>", iSource, szAdd, szMan );
		else if (iCurrentEditingWhichName==MANUALNAME)
			wsprintf( szsz, "~<%04d>~<%s>~<%s>", iSource, szAuto, szAdd );
		else
			wsprintf( szsz, "~<%04d>~<%s>~<%s>", iSource, szAuto, szMan );
		if (iAuto=="MANUAL") {
			strcat( szsz, "~<MANUAL>" );
			textPut( "text", ssNewName, MAIN_PANEL, "ChosenManualName" );
		}
		else {
			strcat( szsz, "~<AUTO>" );
			textPut( "text", ssNewName, MAIN_PANEL, "ChosenAutoName" );
		}
		if (iAuto=="PERM") strcat( szsz, "~<PERM>~<" );
		else	strcat( szsz, "~<TEMP>~<" );
		strcat(szsz, ssLabel );
		strcat( szsz, ">");
		infoWrite(iLabelAutoDevice, szsz, iSource, false );
		textPut( "text", "", MAIN_PANEL, "KEYBOARD" );
		textPut( "text", "", MAIN_PANEL, "ChosenSoftName2" );
		textPut( "text", "", MAIN_PANEL, "MAXCHARS" );
		textPut( "text", ssNewName, MAIN_PANEL, "ChosenSoftName1" );
	}
}


// all revertives come here
int SoftNameExternal::revertiveCallback( revertiveNotify * r )
{
	if ( r->device() == iLabelAutoDevice ) {
		// revertive from automatic infodriver
		if ((r->index()>STARTROUTERSLOT)&&(r->index()<=(STARTROUTERSLOT+iRouterSourceSize) )) {
			// rev for router source -- store and display
			if (isSourceExternal(r->index())>-1) {
				// store and display
				allSoftData->storeRouterLabelData(r->index(), r->sInfo() );
				updateListSourceData(r->index() );
				if (r->index()==iCurrentSource)
					DisplayChosenSourceData(false);
			}
		}
		else if ((r->index()>STARTSNCSSLOT)&&(r->index()<(FINALSNCSSLOT) )) {
			// rev for sncs label-- store and display
			allSoftData->storeSncsLabelData(r->index()-STARTSNCSSLOT, r->sInfo() );
		}
		else {
			OutputDebugString("SoftNameExternal - unexpected revertive from automatic");
		}
	}
	return 0;
}

void SoftNameExternal::databaseCallback( revertiveNotify * r )
{
	if (r->device()==iRouterDevice) {
		if ((r->index()>STARTROUTERSLOT)&&(r->index()<=(STARTROUTERSLOT+iRouterSourceSize) )) {
			if (isSourceExternal(r->index())>-1) {
				updateListSourceData( r->index() );
				if (r->index()==iCurrentSource)
					ResetPanel();
			}
		}
	}
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string SoftNameExternal::parentCallback( parentNotify *p )
{
	bncs_string ssStr;

	//Property set commands
	if( p->command() == "instance" )
	{
		m_SoftNameInstance = p->value();
		debug("SoftNameSummary::parentCallback instance=%1", m_SoftNameInstance);
	
		timerStart(1, 1000);

		// show a panel from file main.bncs_ui and we'll know it as our panel 1
		panelShow( MAIN_PANEL, "main.bncs_ui" );
		panelShow( ASSIGN_PANEL, "assignlabel.bncs_ui" );
		textPut( "text", " ", MAIN_PANEL, "MAXCHARS"  );
		// now hide assign panel
		panelRemove(ASSIGN_PANEL);
		ResetPanel();
		// determine which studio it is in STA or STC -- from workstation number

		// get object settings
		ssStr = getObjectSetting( m_SoftNameInstance, "sources_external" );
		ssl_ExternalSources = bncs_stringlist(ssStr);

		// get device numbers
		ssStr = getObjectSetting( m_SoftNameInstance, "router" );
		iRouterDevice = getDeviceNo(ssStr);
		iRouterSourceSize = getRouterSize(iRouterDevice, 0);
		iRouterDestinationSize = getRouterSize(iRouterDevice, 1);
	
		iLabelAutoDevice = getDeviceNo(m_SoftNameInstance);

		// register with automatic
		// sources 
		// better to register just for External and external -- as these are really the only ones to be displayed
		routerRegister( iRouterDevice, 1, iRouterSourceSize, true );
		registerForAutoRtrRevsAndPoll( );
	
		// sncs labels
		infoRegister( iLabelAutoDevice, STARTSNCSSLOT+1, FINALSNCSSLOT, true );
		infoPoll( iLabelAutoDevice, STARTSNCSSLOT+1, FINALSNCSSLOT );

		// colour bgs
		if (m_SoftNameInstance=="sta_bfeumdlabel_auto") {
				textPut( "stylesheet", "sta_panel_background", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName8" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName9" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName6" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName7" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName3" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName4" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "MAXCHARS" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "sta_group_background", ASSIGN_PANEL, "LabelPanel_bg" );	
		}
		else if (m_SoftNameInstance=="stc_bfeumdlabel_auto") {
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "ChosenSoftName8" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "ChosenSoftName9" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "ChosenSoftName6" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "ChosenSoftName7" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "ChosenSoftName3" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "ChosenSoftName4" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "MAXCHARS" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "stc_group_background", ASSIGN_PANEL, "LabelPanel_bg" );	
		}
 		else  {
				textPut( "stylesheet", "", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "", MAIN_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName8" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName9" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName6" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName7" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName3" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName4" );	
				textPut( "stylesheet", "", MAIN_PANEL, "MAXCHARS" );	
				textPut( "stylesheet", "", MAIN_PANEL, "background_group_cf" );	
		}
	}
	
	return "";
}

// timer events come here
void SoftNameExternal::timerCallback( int id )
{
	switch(id) {
	case 1: 
		timerStop(1);
		bPanelStarting=FALSE;
		// display revs
		for (int iS=0;iS<ssl_ExternalSources.count();iS++) {
			updateListSourceData( ssl_ExternalSources[iS] );
		}
	}
}



// simple helper to return the device number for the given instance 
int SoftNameExternal::getDeviceNo( const bncs_string & instance )
{
	bncs_config c( "instances." + instance );

	if( c.isValid())
	{
		bncs_stringlist sl = c.attr( "ref" );
		return sl.getNamedParam( "device" );
	}
	return 0;
}

void SoftNameExternal::registerForAutoRtrRevsAndPoll( void )
{
	// register for router revs and poll
	int i, iSrc;
	// register External
	for ( i=0;i<ssl_ExternalSources.count();i++) {
		iSrc = ssl_ExternalSources[i].toInt();
		infoRegister(iLabelAutoDevice, iSrc, iSrc, true );
	}
	//poll
	for ( i=0;i<ssl_ExternalSources.count();i++) {
		iSrc = ssl_ExternalSources[i].toInt();
		infoPoll(iLabelAutoDevice, iSrc, iSrc);
	}
}


int SoftNameExternal::isSourceExternal( int iSource )
{
	int ifound = ssl_ExternalSources.find( iSource );
	if (ifound>=0) 
		return (ifound+1); // data index starts  at 1
	else
		return -1;
}


void SoftNameExternal::updateListSourceData( int iSource )
{
	// given src - get all data out as a nice string and write into list box for all the External srcs
	// get all data in form of text for display in list box
	bncs_string szSrcName, szAuto, szMan, iAuto, iPerm, sstr, ssLabel;
	bncs_stringlist sslist;
	int  iIndex;
	
	iIndex = (ssl_ExternalSources.find(iSource))+1;

	if (allSoftData->getAllRouterLabelData(iSource, &szAuto, &szMan, &iAuto, &iPerm, &ssLabel )==TRUE) {
		routerName(iRouterDevice, 0, iSource, szSrcName );
		textPut( "text", szSrcName, MAIN_PANEL, iIndex);
		// put auto name in alias name label
		sstr = "SNCS" + bncs_string(iIndex);
		textPut( "text", ssLabel, MAIN_PANEL, sstr);
		sstr = "ALIAS" + bncs_string(iIndex);
		textPut( "text", szAuto, MAIN_PANEL, sstr);
		sstr = bncs_string(iIndex+100);
		if (iAuto=="MANUAL") {
			// colour button red
			textPut( "stylesheet", "topstrap_red_on", MAIN_PANEL, sstr );
			textPut( "text", " MANUAL", MAIN_PANEL, sstr );
		}
		else {
			// colour blue
			textPut( "stylesheet", "topstrap_reh_on", MAIN_PANEL, sstr);
			textPut( "text", " AUTO", MAIN_PANEL, sstr );
		}
		sstr = "SOFTNAME" + bncs_string(iIndex);
		if (iAuto=="MANUAL") {
			textPut( "text", szMan, MAIN_PANEL, sstr );
		}
		else {
			textPut( "text", szAuto, MAIN_PANEL, sstr );
		}

	}

}

