#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "SoftNameSncs.h"


#define MAIN_PANEL 1

#define AUTONAME 1

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( SoftNameSncs )

// constructor - equivalent to ApplCore STARTUP
SoftNameSncs::SoftNameSncs( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	bncs_string ssStr;
	//
	iCurrentIndex=-1;
	iCurrentSource = -1;
	iCurrentListBox = 0;
	ssCurrentLabel= "";
	ssCurrentAlias = "";
	bPanelStarting = TRUE;
	bCreatingNewLabel = FALSE;

	// declare data storage
	allSoftData = new SoftNameData;

	// start up via parent callback

}

// destructor - equivalent to ApplCore CLOSEDOWN
SoftNameSncs::~SoftNameSncs()
{
}

// all button pushes and notifications come here
void SoftNameSncs::buttonCallback( buttonNotify *b )
{
	bncs_string sstr, sstr2, sstr3, prevsstr;

	if( b->panel() == MAIN_PANEL )	{
		bCreatingNewLabel = FALSE;
		if (b->id()=="CreateLabel") {
			iCurrentListBox=0;
			ssCurrentAlias = "";
			ssCurrentLabel = "";
			bCreatingNewLabel = TRUE;
			KeyboardPanelPrep();
		}
		else if (b->id()=="DeleteLabel") {
			DeleteSNCSLabel( ssCurrentLabel );
			ResetPanel();
		}
		else if (b->id()=="ClearAlias") {
			ClearSNCSLabel( ssCurrentLabel );
			ResetPanel();
		}
		else if (b->id()=="ClearDown") {
			// clear all temp aliases
			ClearAllSNCSLabels( );
			ResetPanel();
		}
		else if (b->id()=="PurgeLabel") {
			// clear any unused Labels only
			PurgeUnusedSNCSLabels( );
			ResetPanel();
		}
		else if (b->id()=="List1") {
			textPut("selected=none", MAIN_PANEL, "List2");
			textPut("selected=none", MAIN_PANEL, "List3");
			iCurrentListBox = 1;
			textGet( "selected", MAIN_PANEL, "List1", sstr  );
			ssCurrentLabel= sstr.simplifyWhiteSpace(); // is now the new label
			ssCurrentAlias = "";
			textPut( "text", ssCurrentLabel, MAIN_PANEL, "ChosenLabel" );
			textPut( "text", ssCurrentAlias, MAIN_PANEL, "ChosenAlias" );
			KeyboardPanelPrep();
		}
		else if (b->id()=="List2") {
			textPut("selected=none", MAIN_PANEL, "List1");
			textPut("selected=none", MAIN_PANEL, "List3");
			iCurrentListBox = 2;
			textGet( "selected", MAIN_PANEL, "List2", sstr  );
			sstr.split('-', sstr2, sstr3);
			ssCurrentLabel = sstr2.simplifyWhiteSpace(); // is now the new label
			ssCurrentAlias = sstr3.simplifyWhiteSpace(); // new alias name
			textPut( "text", ssCurrentLabel, MAIN_PANEL, "ChosenLabel" );
			textPut( "text", ssCurrentAlias, MAIN_PANEL, "ChosenAlias" );
			KeyboardPanelPrep();
		}
		else if (b->id()=="List3") {
			textPut("selected=none", MAIN_PANEL, "List1");
			textPut("selected=none", MAIN_PANEL, "List2");
			iCurrentListBox = 3;
			textGet( "selected", MAIN_PANEL, "List3", sstr  );
			sstr.split('-', sstr2, sstr3);
			ssCurrentLabel = sstr2.simplifyWhiteSpace(); // is now the new label
			ssCurrentAlias = sstr3.simplifyWhiteSpace(); // new alias name
			textPut( "text", ssCurrentLabel, MAIN_PANEL, "ChosenLabel" );
			textPut( "text", ssCurrentAlias, MAIN_PANEL, "ChosenAlias" );
			KeyboardPanelPrep();
		}
		if (b->id()=="SAVECLOSEPANEL") {
			if (iCurrentListBox==0) {
				//allocate new one
				AssignNewLabel( );
			}
			else {
				// modifying existing one
				AssignSNCSLabel( ssCurrentLabel );
			}
			textPut( "text", "", MAIN_PANEL, "ChosenAlias2");
			textPut( "text", "", MAIN_PANEL, "KEYBOARD");
			ResetPanel();
		}
		else if (b->id()=="CLOSEPANEL") {
			ResetPanel();
		}
		else if (b->id()=="ChosenForm") {
			if (ssCurrentLabel.length()>0) {
				ChangeCurrentForm(ssCurrentLabel);
			}
		}
		else if (b->id()=="KEYBOARD") {
			// keyboard control
			textGet( "text", MAIN_PANEL, "ChosenAlias2", prevsstr  );
			textGet( "text", MAIN_PANEL, "KEYBOARD", sstr  );
			if (prevsstr==sstr) {
				// return pressed  -- do the same as save and close
				if (iCurrentListBox==0) {
					AssignNewLabel( );
				}
				else {
					AssignSNCSLabel( ssCurrentLabel );
				}
				ResetPanel();
			}
			else { // different so char added etc
				char *pdest = const_cast<char *>( strchr( sstr, '~' ) );
				if (pdest==NULL) {
					textPut( "text", sstr, MAIN_PANEL, "ChosenAlias2"  );
					textPut( "text", " ", MAIN_PANEL, "MAXCHARS"  );
				}
				else {
					textGet( "text", MAIN_PANEL, "ChosenAlias2", sstr  );
					textPut( "text", sstr, MAIN_PANEL, "KEYBOARD"  );
				}
			}
		}	
	}
}


// all revertives come here
int SoftNameSncs::revertiveCallback( revertiveNotify * r )
{
	if ( r->device() == iLabelAutoDevice ) {
		// revertive from automatic infodriver
		if ((r->index()>STARTROUTERSLOT)&&(r->index()<=(STARTROUTERSLOT+iRouterSourceSize) )) {
			// rev for router source -- store and display
			if (isSourceExternal(r->index())>-1) {
				// store and display
				allSoftData->storeRouterLabelData(r->index(), r->sInfo() );
			}
		}
		else if ((r->index()>STARTSNCSSLOT)&&(r->index()<(FINALSNCSSLOT) )) {
			// rev for sncs label-- store and display
			allSoftData->storeSncsLabelData(r->index()-STARTSNCSSLOT, r->sInfo() );
			// update list boxes as appropriate
			if (bPanelStarting==FALSE) {
				// restart timer -- if no more revs come in then redraw listboxes etc
				timerStop( 2 );
				timerStart(2, 500);
			}
		}
		else {
			OutputDebugString("SoftNameSncs - unexpected revertive from automatic");
		}
	}
	return 0;
}

// all database name changes come back here
void SoftNameSncs::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string SoftNameSncs::parentCallback( parentNotify *p )
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
		textPut( "text", " ", MAIN_PANEL, "MAXCHARS"  );
		// now hide assign panel
		ResetPanel();
		// determine which studio it is in STA or STC -- from workstation number

		// get object settings
		ssStr = getObjectSetting( m_SoftNameInstance, "sources_external" );
		ssl_ExternalSources = bncs_stringlist(ssStr);
		iLabelCount = 0;

		// get device numbers
		ssStr = getObjectSetting( m_SoftNameInstance, "router" );
		iRouterDevice = getDeviceNo(ssStr);
		iRouterSourceSize = getRouterSize(iRouterDevice, 0);
		iRouterDestinationSize = getRouterSize(iRouterDevice, 1);
	
		iLabelAutoDevice = getDeviceNo(m_SoftNameInstance);

		// register with automatic
		// sources 
		// better to register just for External and external -- as these are really the only ones to be displayed
		registerForAutoRtrRevsAndPoll( );
	
		// sncs labels
		infoRegister( iLabelAutoDevice, STARTSNCSSLOT+1, FINALSNCSSLOT, true );
		infoPoll( iLabelAutoDevice, STARTSNCSSLOT+1, FINALSNCSSLOT );

		// colour bgs
		if (m_SoftNameInstance=="sta_bfeumdlabel_auto") {
				textPut( "stylesheet", "sta_panel_background", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "TableState1" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName3" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName4" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName7" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName8" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "LabelOrAlias" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName9" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName12" );		
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ERRORMESSAGE" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "LABELCOUNT" );	

		}
		else if (m_SoftNameInstance=="stc_bfeumdlabel_auto") {
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "TableState1" );	
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "ChosenSoftName3" );	
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "ChosenSoftName4" );	
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "ChosenSoftName7" );	
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "ChosenSoftName8" );	
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "LabelOrAlias" );	
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "ChosenSoftName9" );	
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "ChosenSoftName12" );		
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "ERRORMESSAGE" );	
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "LABELCOUNT" );	

		}
 		else  {
				textPut( "stylesheet", "", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "", MAIN_PANEL, "TableState1" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName3" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName4" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName7" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName8" );	
				textPut( "stylesheet", "", MAIN_PANEL, "LabelOrAlias" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName9" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName12" );		
				textPut( "stylesheet", "", MAIN_PANEL, "ERRORMESSAGE" );	
				textPut( "stylesheet", "", MAIN_PANEL, "LABELCOUNT" );	
		}
	}
	return "";
}

// timer events come here
void SoftNameSncs::timerCallback( int id )
{
	switch(id) {
	case 1: 
		timerStop(1);
		bPanelStarting=FALSE;
		// display sncs revs
		UpdateAllListBoxes(); // after all revs in
	break;
	case 2: 
		timerStop(2);
		// display sncs revs
		UpdateAllListBoxes(); // what if one chosen -- rechoose see global xy
	break;
	case 3: 
		timerStop(3);
		textPut( "text", "", MAIN_PANEL, "ERRORMESSAGE" );
	break;
	}
}


void SoftNameSncs::UpdateAllListBoxes(void) 
{
	bncs_string szAlias, szSNCS, iAuto, iPerm, sstr;
	bncs_stringlist ssl;
	char szsz[BUFFER_CONST]= "                                        ";
	iLabelCount = 0;
	textPut("clear", MAIN_PANEL, "List1");
	textPut("clear", MAIN_PANEL, "List2");
	textPut("clear", MAIN_PANEL, "List3");
	for (int iIndex=1;iIndex<MAXLABELS;iIndex++) {
		// get label data; determine which box it should go in; use - when more than 1 piece of data
		allSoftData->getAllSncsLabelData(iIndex, &szAlias, &szSNCS, &iPerm, &ssl);
		if (szAlias!="$NAME") {
			strcpy( szsz, "                                        " );
			iLabelCount++;
			if (iPerm=="PERM") {
				// all perm lables in perm listbox if assigned or not
				strncpy( szsz, szAlias, szAlias.length());
				if (szAlias.length()>8) {
					strncpy( szsz+szAlias.length(), " - ", 3 );
					strncpy( szsz+(szAlias.length()+3), szSNCS, szSNCS.length());
					// remove trailing spaces
					int iL = szAlias.length()+3+szSNCS.length();
					if (iL<(strlen(szsz)))
						szsz[iL]=0;
				}
				else {
					strncpy( szsz+8, " - ", 3 );
					strncpy( szsz+11, szSNCS, szSNCS.length());
					// remove trailing spaces
					int iL = 12 + szSNCS.length();
					if (iL<(strlen(szsz)))
						szsz[iL]=0;
				}
				textPut("add", szsz, MAIN_PANEL, "List3" );
			}
			else {
				sstr = szSNCS.stripWhiteSpace();
				if (sstr.length()==0)
					textPut("add", szAlias, MAIN_PANEL, "List1" );
				else {
					strncpy( szsz, szAlias, szAlias.length());
					if (szAlias.length()>8) {
						strncpy( szsz+szAlias.length(), " - ", 3 );
						strncpy( szsz+(szAlias.length()+3), szSNCS, szSNCS.length());
						// remove trailing spaces
						int iL = szAlias.length()+3+szSNCS.length();
						if (iL<(strlen(szsz)))
							szsz[iL]=0;
					}
					else {
						strncpy( szsz+8, " - ", 3 );
						strncpy( szsz+11, szSNCS, szSNCS.length());
						// remove trailing spaces
						int iL = 12 + szSNCS.length();
						if (iL<(strlen(szsz)))
							szsz[iL]=0;
					}
					textPut("add", szsz, MAIN_PANEL, "List2" );
				}
			}
		}
	}
	sstr = "Labels : " + bncs_string( iLabelCount ) + " / 200";
	textPut( "text", sstr, MAIN_PANEL, "LABELCOUNT" );
}


void SoftNameSncs::ClearSNCSLabel( bncs_string ssLabel ) {
	bncs_string  ssAlias,  iPerm;
	bncs_stringlist ssl;
	char szsz[BUFFER_CONST]= "", szLabel[BUFFER_CONST]= "";
	int iIndex;
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllSncsLabelData(ssLabel, &iIndex, &ssAlias,  &iPerm, &ssl)) {
		if (iPerm=="TEMP") {
			//now write out to slot
			strcpy( szLabel, ssLabel );
			wsprintf( szsz, "~<%s>~< >~< >~<AUTO>", szLabel );
			strcat( szsz, "~<TEMP>~< >" );
			infoWrite(iLabelAutoDevice, szsz, iIndex+STARTSNCSSLOT, false );
		}
	}
}

void SoftNameSncs::ClearAllSNCSLabels( void ) {
	bncs_string ssLabel, ssAlias,  iPerm;
	bncs_stringlist ssl;
	char szsz[BUFFER_CONST]= "", szLabel[BUFFER_CONST] = "";
	int iIndex;
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	for ( iIndex =1;iIndex<MAXLABELS;iIndex++) {
		if (allSoftData->getAllSncsLabelData(iIndex, &ssLabel,  &ssAlias, &iPerm, &ssl)) {
			bncs_string sstr = ssAlias.stripWhiteSpace();
			if ((ssLabel!="$NAME") && (iPerm=="TEMP")&& (sstr.length()>0)) {
				//now write out to slot
				strcpy(szLabel, ssLabel);
				wsprintf( szsz, "~<%s>~< >~< >~<AUTO>~<TEMP>~< >", szLabel );
				infoWrite(iLabelAutoDevice, szsz, iIndex+STARTSNCSSLOT, false );
			}
		}
	}
}


void SoftNameSncs::PurgeUnusedSNCSLabels( void ) {
	bncs_string ssLabel, ssAlias,  iPerm;
	bncs_stringlist ssl;
	char szsz[BUFFER_CONST]= "", szLabel[BUFFER_CONST] = "";
	int iIndex;
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	for ( iIndex=1;iIndex<MAXLABELS;iIndex++) {
		if (allSoftData->getAllSncsLabelData(iIndex, &ssLabel,  &ssAlias, &iPerm, &ssl)) {
			bncs_string sstr = ssAlias.stripWhiteSpace();
			if (sstr.length()==0) {
				if ((ssLabel!="$NAME") && (iPerm=="TEMP") && (sstr.length()==0)) {
					//now write out to slot
					wsprintf( szsz, "~<$NAME>~< >~< >~<AUTO>~<TEMP>~< >" );
					infoWrite(iLabelAutoDevice, szsz, iIndex+STARTSNCSSLOT, false );
				}
			}
		}
	}
}


void SoftNameSncs::DeleteSNCSLabel( bncs_string ssLabel ) {
	bncs_string  ssAlias, iPerm;
	bncs_stringlist ssl;
	char szsz[BUFFER_CONST]= "", szAuto[BUFFER_CONST]= "", szMan[BUFFER_CONST]= "";
	int iIndex;
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllSncsLabelData(ssLabel, &iIndex, &ssAlias, &iPerm, &ssl)) {
		if (iPerm=="TEMP") {
			//now write out to slot
			wsprintf( szsz, "~<$NAME>~< >~< >~<AUTO>~<TEMP>~< >" );
			infoWrite(iLabelAutoDevice, szsz, iIndex+STARTSNCSSLOT, false );
		}
	}
}


void SoftNameSncs::AssignSNCSLabel( bncs_string ssLabel ) {
	bncs_string  ssAlias, iPerm, ssNewName;
	bncs_stringlist ssl;
	char szsz[BUFFER_CONST]= "", szLabel[BUFFER_CONST]= "", szAlias[BUFFER_CONST]= "";
	int iIndex;
	// get details of current label and new settings from panel
	textGet( "text", MAIN_PANEL, "ChosenAlias2", ssNewName  );
	textPut( "text", ssNewName , MAIN_PANEL, "ChosenAlias" );
	if (ssNewName.length()==0) ssNewName = " ";
	if (allSoftData->getAllSncsLabelData(ssLabel, &iIndex, &ssAlias, &iPerm, &ssl)) {
		//now write out to slot
		strcpy(szLabel, ssLabel);
		strcpy(szAlias, ssNewName);
		wsprintf( szsz, "~<%s>~<%s>~<%s>~<AUTO>", szLabel, szAlias, szAlias );
		if (iPerm=="PERM") strcat( szsz, "~<PERM>~< >" );
		else strcat( szsz, "~<TEMP>~< >" );
		infoWrite(iLabelAutoDevice, szsz, iIndex+STARTSNCSSLOT, false );
	}
}

void SoftNameSncs::AssignNewLabel( ) {
	bncs_string  ssNewLabel, ssAlias, iPerm, sstr, sstr1;
	bncs_stringlist ssl;
	char szsz[BUFFER_CONST]= "", szLabel[BUFFER_CONST]= "", szAlias[BUFFER_CONST]= "";
	int iNewIndex;

	// get details of current label and new settings from panel
	textGet( "text", MAIN_PANEL, "ChosenAlias2", sstr  );
	sstr1 = sstr.stripWhiteSpace();
	sstr = sstr1.upper();
	if (sstr.length()>0) {
		ssNewLabel = sstr;
		iNewIndex = allSoftData->findNextFreeIndex( ssNewLabel );
		if (iNewIndex>0) {
			allSoftData->getAllSncsLabelData(iNewIndex, &ssNewLabel, &ssAlias, &iPerm, &ssl);
			//now write out to slot
			strcpy(szLabel, ssNewLabel);
			strcpy(szAlias, ssAlias);
			wsprintf( szsz, "~<%s>~< >~<NEW>~<AUTO>~<TEMP>~< >", szLabel );
			infoWrite(iLabelAutoDevice, szsz, iNewIndex+STARTSNCSSLOT, false );
			iCurrentListBox = 1;
			ssCurrentLabel= ssNewLabel; // is now the new label
			ssCurrentAlias = "";
			bCreatingNewLabel = FALSE;
			KeyboardPanelPrep();
		} // newindex found
		else {
			// table is full 
			textPut( "text", "SNCS LABEL TABLE IS FULL -- Please delete unwanted labels !!!", MAIN_PANEL, "ERRORMESSAGE" );
			timerStart( 3, 10000 );
		}
	} // len = 0
}


void SoftNameSncs::ChangeCurrentForm( bncs_string ssLabel )
{
	bncs_string  ssAlias, iPerm;
	bncs_stringlist ssl;
	char szsz[BUFFER_CONST]= "", szLabel[BUFFER_CONST]= "", szAlias[BUFFER_CONST]= "";
	int iIndex;
	// get details of current label and new settings from panel
	if (allSoftData->getAllSncsLabelData(ssLabel, &iIndex, &ssAlias, &iPerm, &ssl)) {
		if (iPerm=="PERM") 
			iPerm = "TEMP";
		else
			iPerm = "PERM";
		//now write out to slot
		strcpy(szLabel, ssLabel);
		strcpy(szAlias, ssAlias);
		wsprintf( szsz, "~<%s>~<%s>~<%s>~<AUTO>", szLabel, szAlias, szAlias );
		if (iPerm=="PERM") {
			strcat( szsz, "~<PERM>~< >" );
			textPut( "stylesheet", "topstrap_red_on", MAIN_PANEL, "ChosenForm" );
			textPut( "text", " PERM", MAIN_PANEL, "ChosenForm" );
			controlDisable(MAIN_PANEL, "DeleteLabel" );
		}
		else {
			strcat( szsz, "~<TEMP>~< >" );
			textPut( "stylesheet", "topstrap_reh_on", MAIN_PANEL, "ChosenForm" );
			textPut( "text", " TEMP", MAIN_PANEL, "ChosenForm" );
			controlEnable(MAIN_PANEL, "DeleteLabel" );
		}
		infoWrite(iLabelAutoDevice, szsz, iIndex+STARTSNCSSLOT, false );
	}
}


void SoftNameSncs::KeyboardPanelPrep( void )
{
	bncs_string  ssAlias, iPerm;
	bncs_stringlist ssl;
	char szsz[BUFFER_CONST]= "", szLabel[BUFFER_CONST]= "", szAlias[BUFFER_CONST]= "";
	int iIndex;
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllSncsLabelData(ssCurrentLabel, &iIndex, &ssAlias, &iPerm, &ssl)) {
		if (iCurrentListBox==0) {
			textPut("text", " - -", MAIN_PANEL, "ChosenLabel1" );
			textPut("text", " - -", MAIN_PANEL, "ChosenAlias1" );
			textPut("text", " - -", MAIN_PANEL, "ChosenSoftName1" );
			textPut("text", " New LABEL:", MAIN_PANEL, "LabelOrAlias" );
			controlEnable(MAIN_PANEL, "DeleteLabel" );
		} 
		else {
			textPut("text", ssCurrentLabel, MAIN_PANEL, "ChosenLabel1" );
			textPut("text", ssAlias, MAIN_PANEL, "ChosenAlias1" );
			textPut("text", ssAlias, MAIN_PANEL, "ChosenSoftName1" );
			textPut("text", " New Alias :", MAIN_PANEL, "LabelOrAlias" );
			controlEnable(MAIN_PANEL, "DeleteLabel" );
			controlEnable(MAIN_PANEL, "ClearAlias" );
		}
		if (iPerm=="PERM") {
			// colour button red 
			textPut( "stylesheet", "topstrap_red_on", MAIN_PANEL, "ChosenForm" );
			textPut( "text", " PERM", MAIN_PANEL, "ChosenForm" );
			controlDisable(MAIN_PANEL, "DeleteLabel" );
		}
		else {
			// colour blue
			textPut( "stylesheet", "topstrap_reh_on", MAIN_PANEL, "ChosenForm" );
			textPut( "text", " TEMP", MAIN_PANEL, "ChosenForm" );
		}
	}
	textPut("text", "", MAIN_PANEL, "ChosenAlias2" );
	textPut("text", "", MAIN_PANEL, "KEYBOARD" );
	textPut("text", "", MAIN_PANEL, "ERRORMESSAGE" );
	controlEnable(MAIN_PANEL, "KEYBOARD" );
	controlEnable(MAIN_PANEL, "CLOSEPANEL" );
	controlEnable(MAIN_PANEL, "SAVECLOSEPANEL" );
	controlEnable(MAIN_PANEL, "ChosenForm" );
}


void SoftNameSncs::ResetPanel( void )
{
	iCurrentIndex=-1;
	iCurrentSource=-1;
	iCurrentListBox=0;
	ssCurrentAlias = "";
	ssCurrentLabel = "";
	controlDisable(MAIN_PANEL, "DeleteLabel" );
	controlDisable(MAIN_PANEL, "EditLabel" );
	controlDisable(MAIN_PANEL, "ClearAlias" );
	controlDisable(MAIN_PANEL, "CLOSEPANEL" );
	controlDisable(MAIN_PANEL, "SAVECLOSEPANEL" );
	controlDisable(MAIN_PANEL, "ChosenForm" );
	textPut("text", "", MAIN_PANEL, "ChosenAlias1" );
	textPut("text", "", MAIN_PANEL, "ChosenLabel1" );
	textPut("text", "", MAIN_PANEL, "ChosenAlias2" );
	textPut("text", "", MAIN_PANEL, "ChosenSoftName1" );
	textPut("text", "", MAIN_PANEL, "KEYBOARD" );
	textPut("selected=none", MAIN_PANEL, "List1");
	textPut("selected=none", MAIN_PANEL, "List2");
	textPut("selected=none", MAIN_PANEL, "List3");
}



// simple helper to return the device number for the given instance 
int SoftNameSncs::getDeviceNo( const bncs_string & instance )
{
	bncs_config c( "instances." + instance );

	if( c.isValid())
	{
		bncs_stringlist sl = c.attr( "ref" );
		return sl.getNamedParam( "device" );
	}
	return 0;
}

void SoftNameSncs::registerForAutoRtrRevsAndPoll( void )
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


int SoftNameSncs::isSourceExternal( int iSource )
{
	int ifound = ssl_ExternalSources.find( iSource );
	if (ifound>=0) 
		return (ifound+1); // data index starts  at 1
	else
		return -1;
}

