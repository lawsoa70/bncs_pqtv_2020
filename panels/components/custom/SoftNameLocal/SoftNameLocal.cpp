#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "SoftNameLocal.h"

#define MAIN_PANEL 1
#define CAMERA_PANEL 2


// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( SoftNameLocal )

// constructor - equivalent to ApplCore STARTUP
SoftNameLocal::SoftNameLocal( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{

	//
	iCurrentIndex=-1;
	iCurrentSource = -1;
	bPanelStarting = TRUE;
	bCameraPanel = FALSE;
	iCurrentQuickEditIndex = 0;

	// declare data storage
	allSoftData = new SoftNameData;

	// start up via parent callback

}

// destructor - equivalent to ApplCore CLOSEDOWN
SoftNameLocal::~SoftNameLocal()
{
	delete allSoftData;
}

// all button pushes and notifications come here
void SoftNameLocal::buttonCallback( buttonNotify *b )
{
	bncs_string ssprevname, ssNewName="", sstr=0, sstr2, sstr3="TEMP";

	if( b->panel() == MAIN_PANEL ) {
		if (b->id()=="ClearAll") {
			// clear all soft names for all local sources -- if temp -- which they all should be
			ClearAllLocalSoftNames();
			ResetPanel();
		}
		else if (b->id()=="ClearName") {
			// if item is selected then clear soft name for item
			ClearLocalSoftName( iCurrentSource );
			ResetPanel();
		}
		else if (b->id()=="Commit") {
			// if item is selected then clear soft name for item
			SaveLocalSoftName( iCurrentSource );
			ResetPanel();
		}
		else if (b->id()=="Abandon") {
			// clear selected items, vars etc
			ResetPanel();
		}
		else if (b->id()=="KEYBOARD") { 
			// keyboard control
			textGet( "text", MAIN_PANEL, "ChosenSoftName2", ssprevname  );
			textGet( "text", MAIN_PANEL, "KEYBOARD", ssNewName  );
			if (ssNewName==ssprevname) { // same assume it is return char
				if (iCurrentSource>0) {
					SaveLocalSoftName( iCurrentSource);
					ResetPanel();
				}
			}
			else { // different so more chars added
				char *pdest = const_cast<char *>( strchr( ssNewName, '~' ) );
				if (pdest==NULL) {
					textPut( "text", ssNewName, MAIN_PANEL, "ChosenSoftName2"  );
					textPut( "text", "", MAIN_PANEL, "LetterCount"  );
				}
				else {
					textGet( "text", MAIN_PANEL, "ChosenSoftName2", ssNewName  );
					textPut( "text", ssNewName, MAIN_PANEL, "KEYBOARD"  );
				}
			}
		}
		else if (b->id()=="ListLocal") {
			textGet( "selectedindex", MAIN_PANEL, "ListLocal", sstr );
			textPut( "text", "", MAIN_PANEL, "ChosenSoftName2" );
			textPut( "text", "", MAIN_PANEL, "KEYBOARD" );
			iCurrentIndex = atoi( sstr )+1;
			if ((iCurrentIndex>0)&&(iCurrentIndex<=ssl_LocalSources.count()))
				iCurrentSource = ssl_LocalSources[(iCurrentIndex-1)];
			sstr2 = b->value();
			textPut( "text", sstr2, MAIN_PANEL, "ChosenItem" );
			if ( getRouterLabelSoftName( iCurrentSource, &sstr2, &sstr3 )==TRUE) 	
				textPut( "text", sstr2, MAIN_PANEL, "ChosenSoftName1" );
			//textPut( "setFocus", "", MAIN_PANEL, "KEYBOARD" );
			controlEnable( MAIN_PANEL, "ChosenForm" );
			controlEnable( MAIN_PANEL, "Abandon" );
			controlEnable( MAIN_PANEL, "Commit" );
			if (sstr3=="PERM") {
				textPut( "stylesheet", "topstrap_red_on", MAIN_PANEL, "ChosenForm" );
				textPut( "text", " PERM", MAIN_PANEL, "ChosenForm" );
			}
			else {
				textPut( "stylesheet", "topstrap_reh_on", MAIN_PANEL, "ChosenForm" );
				textPut( "text", " TEMP", MAIN_PANEL, "ChosenForm" );
			}
		}
		else if (b->id()=="ChosenForm") {
			if (iCurrentIndex>=0) {
				ChangeCurrentForm(iCurrentSource);
				ResetPanel();
			}
		}
		else if (b->id()=="SOURCEOPTION") {
			if (bCameraPanel==FALSE) {
				// show  panel 2
				panelShow( CAMERA_PANEL, "cameras.bncs_ui" );
				bCameraPanel = TRUE;
				ResetPanel();
				PopulateCameraPanel();
				textPut( "text", "LIST BOX|Edit", MAIN_PANEL, "SOURCEOPTION" );
				textPut( "text", "Clear All CAMERA|Soft Names", MAIN_PANEL, "ClearAll" );
			}
			else {
				panelRemove(CAMERA_PANEL);
				bCameraPanel = FALSE;
				ResetPanel();
				textPut( "text", "CAMERAS|Quick Edit", MAIN_PANEL, "SOURCEOPTION" );
				textPut( "text", "Clear All LOCAL|Soft Names", MAIN_PANEL, "ClearAll" );
			}
		}
	}
	else if (b->panel()==CAMERA_PANEL) {
		iCurrentQuickEditIndex = 0;
		for (int i=1;i<13;i++)	textPut( "stylesheet", "source_deselected", CAMERA_PANEL, i);
		if ((b->id()>0)&&(b->id()<=ssl_CameraSources.count())) {
			textPut( "stylesheet", "enum_selected", CAMERA_PANEL, b->id());
			iCurrentQuickEditIndex = b->id(); // the index in camera list. starts at 1
			iCurrentSource = ssl_CameraSources[iCurrentQuickEditIndex-1]; // router src no. of the camera
			textGet( "text", CAMERA_PANEL, b->id(), sstr ); // sstr = the current button text
			textPut("text", "", MAIN_PANEL, "ChosenSoftName2");
			textPut( "text", "", MAIN_PANEL, "KEYBOARD" );
			iCurrentIndex = ssl_LocalSources.find(iCurrentSource) + 1; // index of the camera source in the full list. starts at 1
			textPut("selected.id", bncs_string(iCurrentIndex - 1), MAIN_PANEL, "ListLocal");
			if ( getRouterLabelSoftName( iCurrentSource, &sstr2, &sstr3 )==TRUE) 
				textPut("text", sstr2, MAIN_PANEL, "ChosenSoftName1");
			//textPut( "setFocus", "", MAIN_PANEL, "KEYBOARD" );
			controlEnable( MAIN_PANEL, "ChosenForm" );
			controlEnable( MAIN_PANEL, "Abandon" );
			controlEnable( MAIN_PANEL, "Commit" );
			if (sstr3=="PERM") {
				textPut( "stylesheet", "topstrap_red_on", MAIN_PANEL, "ChosenForm" );
				textPut( "text", " PERM", MAIN_PANEL, "ChosenForm" );
			}
			else {
				textPut( "stylesheet", "topstrap_reh_on", MAIN_PANEL, "ChosenForm" );
				textPut( "text", " TEMP", MAIN_PANEL, "ChosenForm" );
			}
		}
	}
}

void SoftNameLocal::ResetPanel( void )
{
	// main
	textPut( "text", "", MAIN_PANEL, "ChosenItem" );
	textPut( "text", "", MAIN_PANEL, "ChosenSoftName1" );
	textPut( "text", "", MAIN_PANEL, "ChosenSoftName2" );
	textPut( "text", "", MAIN_PANEL, "KEYBOARD" );
	textPut("selected=none", MAIN_PANEL, "ListLocal" ); 
	textPut( "text", "", MAIN_PANEL, "LetterCount"  );
	iCurrentIndex=-1;
	iCurrentSource=-1;
	iCurrentQuickEditIndex = 0;
	// camera
	for (int i=1;i<11;i++)
		textPut( "stylesheet", "source_deselected", CAMERA_PANEL, i);
	controlDisable(MAIN_PANEL, "ChosenForm" );
	controlDisable( MAIN_PANEL, "Abandon" );
	controlDisable( MAIN_PANEL, "Commit" );
}

void SoftNameLocal::ClearAllLocalSoftNames(void) 
{	
	if (bCameraPanel==TRUE) {
		for (int iIndex=0;iIndex<ssl_CameraSources.count();iIndex++) {
			ClearLocalSoftName(ssl_CameraSources[iIndex]);
		}
	}
	else {
		for (int iIndex=0;iIndex<ssl_LocalSources.count();iIndex++) {
			ClearLocalSoftName(ssl_LocalSources[iIndex]);
		}
	}
}


void SoftNameLocal::ClearLocalSoftName(int iSource) 
{
	bncs_string szSrcName, szNewName, szAuto, szMan, iAuto, iPerm="TEMP";
	bncs_string ssLabel;
	int iList;
	if ((iSource>0)&&(iSource<=iRouterSourceSize)) {
		if (allSoftData->getAllRouterLabelData( iSource, &szAuto, &szMan, &iAuto, &iPerm, &ssLabel, &iList )) {
			if (iPerm=="TEMP" ) {
				// onlt clear if temp
				char szNum[8] = "0000", szsz[BUFFER_CONST]= "~<xxxx>~< >~< >~<AUTO>~<TEMP>~< >";
				wsprintf( szNum, "~<%04d", iSource );
				strncpy(szsz, szNum, 6);
				infoWrite(iLabelAutoDevice, bncs_string(szsz), iSource, false );
			}
		}
	}
}


void SoftNameLocal::SaveLocalSoftName(int iSource) 
{
	bncs_string szSrcName, szNewName, szAuto, szMan, iAuto, iPerm;
	bncs_string ssLabel;
	int iList;
	char szsz[BUFFER_CONST]= "", szAdd[BUFFER_CONST]= "";
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iSource, &szAuto, &szMan, &iAuto, &iPerm, &ssLabel, &iList )) {
		//get new name
		textGet( "text", MAIN_PANEL, "ChosenSoftName2", szNewName  );
		if (szNewName.length()==0) szNewName = " ";
		strcpy( szAdd, szNewName );
		wsprintf( szsz, "~<%04d>~<%s>~<%s>~<AUTO>", iSource, szAdd, szAdd );
		if (iPerm=="PERM") strcat( szsz, "~<PERM>~< >");
		else strcat( szsz, "~<TEMP>~< >");
		infoWrite(iLabelAutoDevice, szsz, iSource, false );
	}
}



void SoftNameLocal::ChangeCurrentForm( int iSource  )
{
	bncs_string szssSrcName, ssAutoName, ssManName, ssAuto, ssPerm, ssLabel;
	char szsz[BUFFER_CONST]= "", szAuto[BUFFER_CONST]= "", szMan[BUFFER_CONST]= "";
	int iList;
	// get details of current index in alldata and clear both automatic and manual name -- reset basically
	if (allSoftData->getAllRouterLabelData( iSource, &ssAutoName, &ssManName, &ssAuto, &ssPerm, &ssLabel, &iList )) {
		if (ssPerm=="PERM") {
			ssPerm = "TEMP";
			textPut( "stylesheet", "topstrap_reh_on", MAIN_PANEL, "ChosenForm" );
			textPut( "text", " TEMP", MAIN_PANEL, "ChosenForm" );
		}
		else {
			ssPerm = "PERM";
			textPut( "stylesheet", "topstrap_red_on", MAIN_PANEL, "ChosenForm" );
			textPut( "text", " PERM", MAIN_PANEL, "ChosenForm" );
		}
		//now write out to slot
		strcpy( szAuto, ssAutoName );
		strcpy( szMan, ssManName );
		// need to know which name was editied
		wsprintf( szsz, "~<%04d>~<%s>~<%s>~<AUTO>", iSource, szAuto, szMan );
		if (ssPerm=="PERM") strcat( szsz, "~<PERM>~< >" );
		else strcat( szsz, "~<TEMP>~< >" );
		infoWrite(iLabelAutoDevice, szsz, iSource, false );
	}
}


// all revertives come here
int SoftNameLocal::revertiveCallback( revertiveNotify * r )
{
	if ( r->device() == iLabelAutoDevice ) {
		// revertive from automatic infodriver
		if ((r->index()>STARTROUTERSLOT)&&(r->index()<=(STARTROUTERSLOT+iRouterSourceSize) )) {
			// rev for router source -- store and display
			if ((isNOTExternalSource(r->index())==TRUE )&&(isNOTLoopedSource(r->index())==TRUE)) {
				char szDeb[32]=" ";
				wsprintf( szDeb, "rev received %d \n",r->index());
				OutputDebugString( szDeb );
				timerStop(2);
				// store and display
				allSoftData->storeRouterLabelData(r->index(), r->sInfo() );
				if (iCurrentSource==r->index()) {
					// update now as  - it is chosen item else start timer to update soon for whole list
					updateLocalSourceData(r->index());
				}
				else
					timerStart(2, 500); // delay update of list box  for 0.5 sec to better cope with lots of revs
			}
		}
		else {
			OutputDebugString("SoftNameLocal - unexpected revertive from automatic");
		}
	}
	return 0;
}

// all database name changes come back here
void SoftNameLocal::databaseCallback( revertiveNotify * r )
{
	bncs_string sstr2, sstr3;
	if (r->device()==iRouterDevice) {
		if ((r->index()>STARTROUTERSLOT)&&(r->index()<=(STARTROUTERSLOT+iRouterSourceSize) )) {
			// valid - refresh panel
			updateLocalSourceData( -2 );
			if (iCurrentSource==r->index()) {
				// clear selected items, vars etc
				ResetPanel();
			}
		}
	}
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string SoftNameLocal::parentCallback( parentNotify *p )
{
	bncs_string ssStr;
	
	
	//Property set commands
	if( p->command() == "instance" )
	{
		m_SoftNameInstance = p->value();
		debug("SoftNameSummary::parentCallback instance=%1", m_SoftNameInstance);
	
		bPanelStarting = TRUE;
		bCameraPanel = FALSE;
		timerStart(1, 1100 );

		// get device numbers
		ssStr = getObjectSetting( m_SoftNameInstance, "router" );
		iLabelAutoDevice = getDeviceNo(m_SoftNameInstance);

		iRouterDevice = getDeviceNo(ssStr);
		iRouterSourceSize = getRouterSize(iRouterDevice, 0);
		iRouterDestinationSize = getRouterSize(iRouterDevice, 1);

		// get object settings
		ssStr = getObjectSetting( m_SoftNameInstance, "sources_cameras" );
		ssl_CameraSources = bncs_stringlist(ssStr);
		textPut("text", ssl_CameraSources.toString(), MAIN_PANEL, "debug1" );

		ssStr = getObjectSetting( m_SoftNameInstance, "sources_external" );
		ssl_ExternalSources = bncs_stringlist(ssStr);

		ssStr = getObjectSetting( m_SoftNameInstance, "sources_looped" );
		ssl_LoopedSources = bncs_stringlist(ssStr);
	
		for (int i=1;i<=iRouterSourceSize;i++) {
			if ((isNOTExternalSource(i)==TRUE)&&(isNOTLoopedSource(i)==TRUE)) {
				ssl_LocalSources.append(i);
			}
		}

		// show a panel from file main.bncs_ui and we'll know it as our panel 1
		panelShow( MAIN_PANEL, "main.bncs_ui" );
		textPut( "text", "CAMERAS|Quick Edit", MAIN_PANEL, "SOURCEOPTION" );
		textPut( "text", " ", MAIN_PANEL, "LetterCount" );
		panelShow( CAMERA_PANEL, "cameras.bncs_ui" );

		// register with automatic
		// sources 
		// better to register just for local and external -- as these are really the only ones to be displayed
		routerRegister( iRouterDevice, 1, iRouterSourceSize, true );
		registerForAutoRtrRevsAndPoll( );

		// colour bgs
		if (m_SoftNameInstance=="sta_bfeumdlabel_auto") {
				textPut( "stylesheet", "sta_panel_background", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "sta_group_background", CAMERA_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName3" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "ChosenSoftName4" );	
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "LetterCount" );	
		}
		else if (m_SoftNameInstance=="stc_bfeumdlabel_auto") {
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "stc_group_background", CAMERA_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "ChosenSoftName3" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "ChosenSoftName4" );	
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "LetterCount" );	
		}
 		else  {
				textPut( "stylesheet", "", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "", MAIN_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "", CAMERA_PANEL, "background_group_cf" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName3" );	
				textPut( "stylesheet", "", MAIN_PANEL, "ChosenSoftName4" );	
				textPut( "stylesheet", "", MAIN_PANEL, "LetterCount" );	
		}
		controlDisable( MAIN_PANEL, "Abandon" );
		controlDisable( MAIN_PANEL, "Commit" );

	}
		
	return "";
}

// timer events come here
void SoftNameLocal::timerCallback( int id )
{
	switch(id) {
	case 1: 
		timerStop(1);
		bPanelStarting=FALSE;
		// display revs
		updateLocalSourceData( -2 );
		panelRemove(CAMERA_PANEL);
	break;
	case 2: 
		timerStop(2);
		if (bPanelStarting==FALSE)
			updateLocalSourceData( -2 );
	break;
	}
}


// simple helper to return the device number for the given instance 
int SoftNameLocal::getDeviceNo( const bncs_string & instance )
{
	bncs_config c( "instances." + instance );

	if( c.isValid())
	{
		bncs_stringlist sl = c.attr( "ref" );
		return sl.getNamedParam( "device" );
	}
	return 0;
}

void SoftNameLocal::registerForAutoRtrRevsAndPoll( void )
{
	// register for router revs and poll
	for ( int i=1;i<=iRouterSourceSize;i++) {
		if ((isNOTExternalSource( i )==TRUE)&&(isNOTLoopedSource(i)==TRUE)) {
			infoRegister(iLabelAutoDevice, i, i, true );
		}
	}
	//poll
	infoPoll(iLabelAutoDevice, 1, iRouterSourceSize );
}


bool SoftNameLocal::isSourceCamera( int iSource )
{
	int ifound = ssl_CameraSources.find( iSource );
	if (ifound>=0) 
		return TRUE;
	else
		return FALSE;
}


bool SoftNameLocal::isNOTExternalSource( int iSource )
{
	int ifound = ssl_ExternalSources.find( iSource );
	if (ifound>=0) 
		return FALSE;
	else
		return TRUE; // ie source is NOT an external one
}

bool SoftNameLocal::isNOTLoopedSource( int iSource )
{
	int ifound = ssl_LoopedSources.find( iSource );
	if (ifound>=0) 
		return FALSE;
	else
		return TRUE; // ie source is NOT a looped one
}



void SoftNameLocal::updateLocalSourceData( int iSource )
{
	int i, iSrc;
	bncs_string ssText = "doh!";
	// given src - get all data out as a nice string and write into list box for all the local srcs
	textPut( "clear", MAIN_PANEL, "ListLocal" );	
	for ( i=0;i<ssl_LocalSources.count();i++) {
		iSrc = ssl_LocalSources[i].toInt();
		getAllRouterLabelText(iSrc, &ssText );
		textPut("add", ssText, MAIN_PANEL, "ListLocal" ); 
	}
	if (iCurrentSource == iSource) {
		getAllRouterLabelText(iSource, &ssText );
		bncs_string sstr = "selected.id=" + bncs_string(iSource-1);
		textPut(sstr, ssText, MAIN_PANEL, "ListLocal" ); 
	} 
	PopulateCameraPanel();

}


void SoftNameLocal::PopulateCameraPanel(void )
{
	// get all data in form of text for display in list box
	bncs_string ssSrcName, szAuto, szMan, iAuto, iPerm, sstr1;
	bncs_string ssLabel;
	int i, iList;
	for (i=1;i<13;i++) controlDisable(CAMERA_PANEL, i);
	for (i=0;i<ssl_CameraSources.count();i++) {
		// get src num, name and soft name
		if (allSoftData->getAllRouterLabelData(ssl_CameraSources[i], &szAuto, &szMan, &iAuto, &iPerm, &ssLabel, &iList )==TRUE) {
			sstr1="Index" + bncs_string((i+1));
			textPut( "text", ssl_CameraSources[i], CAMERA_PANEL, sstr1 );
			sstr1="Source" + bncs_string((i+1));
			// get db0 name from router dev ini file
			routerName( iRouterDevice, 0, ssl_CameraSources[i], ssSrcName );
			textPut( "text", ssSrcName, CAMERA_PANEL, sstr1 );
			textPut( "text", szAuto, CAMERA_PANEL, bncs_string(i+1) );
			controlEnable(CAMERA_PANEL, bncs_string(i+1));
		}
	}
}


BOOL SoftNameLocal::getAllRouterLabelText( int iSourceNum, bncs_string *szTextData )
{
	// get all data in form of text for display in list box
	bncs_string sstr, szSrcName, szAuto, szMan, iAuto, iPerm;
	bncs_string ssLabel;
	int iList, iIndex;
	char szTmp[32]="", szsz[BUFFER_CONST]= "                                    ";
	*szTextData = szsz;

	// get db0 name from router dev ini file
	routerName( iRouterDevice, 0, iSourceNum, sstr );
	szSrcName = sstr.upper();
	wsprintf( szTmp, "  %03d ", iSourceNum );
	strncpy(szsz, szTmp, 5);
	strncpy(szsz+9, szSrcName, szSrcName.length());
	*szTextData = szsz;
	if (allSoftData->getAllRouterLabelData(iSourceNum, &iIndex, &szAuto, &szMan, &iAuto, &iPerm, &ssLabel, &iList )==TRUE) {
		if (szAuto.length()>0) strncpy ( szsz+19, szAuto, szAuto.length() );
		szsz[19+szAuto.length()]=NULL; // terminate at end of softname == removes trailing spaces
		if (iPerm=="PERM") strcat( szsz, " (p)");
		*szTextData = szsz;
		return TRUE;
	}
	return FALSE;
}

BOOL SoftNameLocal::getRouterLabelSoftName( int iSourceNum, bncs_string *szName, bncs_string *szForm )
{
	// get all data in form of text for display in list box
	bncs_string szSrcName, szAuto, szMan, iAuto, iPerm;
	bncs_string ssLabel;
	int iList, iIndex;

	if (allSoftData->getAllRouterLabelData(iSourceNum, &iIndex, &szAuto, &szMan, &iAuto, &iPerm, &ssLabel, &iList )==TRUE) {
		*szName = szAuto;
		*szForm = iPerm;
		return TRUE;
	}
	return FALSE;
}

