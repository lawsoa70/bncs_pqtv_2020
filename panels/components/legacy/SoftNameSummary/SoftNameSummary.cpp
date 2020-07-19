#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "SoftNameSummary.h"

#define MAIN_PANEL 1
#define ROUTE_INHIBIT 1


// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( SoftNameSummary )

// constructor - equivalent to ApplCore STARTUP
SoftNameSummary::SoftNameSummary( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{

	bncs_string ssStr;

	// declare data storage
	allSoftData = new SoftNameData;

	// start up via parent call back
		
}

// destructor - equivalent to ApplCore CLOSEDOWN
SoftNameSummary::~SoftNameSummary()
{
}

// all button pushes and notifications come here
void SoftNameSummary::buttonCallback( buttonNotify *b )
{
	if( b->panel() == MAIN_PANEL ) {
		ResetPanel();
	}
}


void SoftNameSummary::ResetPanel( void )
{
	textPut("selected=none", MAIN_PANEL, "ListLocal" ); 
}


// all revertives come here
int SoftNameSummary::revertiveCallback( revertiveNotify * r )
{
	if ( r->device() == iLabelAutoDevice ) {
		// revertive from automatic infodriver
		if ((r->index()>STARTROUTERSLOT)&&(r->index()<=(STARTROUTERSLOT+iRouterSourceSize) )) {
			// rev for router source -- store and display
			allSoftData->storeRouterLabelData( r->index(), r->sInfo() );
			timerStop( 2 );
			if (bPanelStarting==FALSE) {
				timerStart(2, 500);
				//updateLocalSourceData( r->index() );
			}
		}
		else if ((r->index()>STARTSNCSSLOT)&&(r->index()<=(STARTSNCSSLOT+MAXLABELS) )) {
			// rev for sncs source -- store and display ??
			OutputDebugString("SoftNameSummary - SNCS revertive -- need code for processing -- to do");
		}
		else {
			OutputDebugString("SoftNameSummary - unexpected revertive from automatic");
		}
	}
	return 0;
}

// all database name changes come back here
void SoftNameSummary::databaseCallback( revertiveNotify * r )
{
	if (r->device()==iRouterDevice) {
		fillLocalSourceData();
	}
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string SoftNameSummary::parentCallback( parentNotify *p )
{
	bncs_string ssStr;

	//Property set commands
	if( p->command() == "instance" )
	{
		m_SoftNameInstance = p->value();
		debug("SoftNameSummary::parentCallback instance=%1", m_SoftNameInstance);
	
		// show a panel from file main.bncs_ui and we'll know it as our panel 1
		panelShow( MAIN_PANEL, "main.bncs_ui" );
		timerStart(1, 1500);
		bPanelStarting = TRUE;

		// get object settings
		ssStr = getObjectSetting( m_SoftNameInstance, "sources_external" );
		ssl_ExternalSources = bncs_stringlist(ssStr);
		ssStr = getObjectSetting( m_SoftNameInstance, "sources_camera" );
		ssl_LocalSources = bncs_stringlist(ssStr);
		ssStr = getObjectSetting( m_SoftNameInstance, "sources_looped" );
		ssl_LoopedSources = bncs_stringlist(ssStr);

		// get device numbers
		ssStr = getObjectSetting( m_SoftNameInstance, "router" );
		iRouterDevice = getDeviceNo(ssStr);
		iRouterSourceSize = getRouterSize(iRouterDevice, 0);
		iRouterDestinationSize = getRouterSize(iRouterDevice, 1);
	
		iLabelAutoDevice = getDeviceNo(m_SoftNameInstance);

		// register with automatic
		// sources 
		routerRegister( iRouterDevice, 1, iRouterSourceSize, true );
		infoRegister( iLabelAutoDevice, 1, iRouterSourceSize, true );
		infoPoll(iLabelAutoDevice, 1, iRouterSourceSize );
	
		// sncs labels
		infoRegister( iLabelAutoDevice, STARTSNCSSLOT+1, STARTSNCSSLOT+MAXLABELS, true );
		// poll for data from auto infodriver -- may need to slug
		//infoPoll( iLabelAutoDevice, STARTROUTERSLOT+1, STARTROUTERSLOT+iRouterSourceSize );

		// colour bgs
		if (m_SoftNameInstance=="sta_bfeumdlabel_auto") {
				textPut( "stylesheet", "sta_panel_background", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "background_group_cf" );		
				textPut( "stylesheet", "sta_group_background", MAIN_PANEL, "lbl8" );		
		}
		else if (m_SoftNameInstance == "stb_bfeumdlabel_auto") {
				textPut( "stylesheet", "stb_panel_background", MAIN_PANEL, "panel_bg");
				textPut( "stylesheet", "stb_group_background", MAIN_PANEL, "background_group_cf");
				textPut( "stylesheet", "stb_group_background", MAIN_PANEL, "lbl8");
		}
		else if (m_SoftNameInstance == "stc_bfeumdlabel_auto") {
				textPut( "stylesheet", "stc_panel_background", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "background_group_cf" );		
				textPut( "stylesheet", "stc_group_background", MAIN_PANEL, "lbl8" );		
		}
 		else  {
				textPut( "stylesheet", "", MAIN_PANEL, "panel_bg" );
				textPut( "stylesheet", "", MAIN_PANEL, "background_group_cf" );		
				textPut( "stylesheet", "", MAIN_PANEL, "lbl8" );		
		}
 


	}

	return "";
}

// timer events come here
void SoftNameSummary::timerCallback( int id )
{
	switch(id) {
	case 1: 
		timerStop(1);
		bPanelStarting=FALSE;
		// display all revs first time now
		fillLocalSourceData();
	break;
	case 2: 
		timerStop(2);
		// refresh list now no more revs arrived for a while
		fillLocalSourceData();
	break;
	}
}


// simple helper to return the device number for the given instance 
int SoftNameSummary::getDeviceNo( const bncs_string & instance )
{
	bncs_config c( "instances." + instance );

	if( c.isValid())
	{
		bncs_stringlist sl = c.attr( "ref" );
		return sl.getNamedParam( "device" );
	}
	return 0;
}


bool SoftNameSummary::isSourceLocal( int iSource )
{
	int ifound = ssl_LocalSources.find( iSource );
	if (ifound>=0) 
		return TRUE;
	else
		return FALSE;
}


bool SoftNameSummary::isSourceExternal( int iSource )
{
	int ifound = ssl_ExternalSources.find( iSource );
	if (ifound>=0) 
		return TRUE;
	else
		return FALSE;
}


bool SoftNameSummary::isSourceLooped( int iSource )
{
	int ifound = ssl_LoopedSources.find( iSource );
	if (ifound>=0) 
		return TRUE;
	else
		return FALSE;
}


void SoftNameSummary::updateLocalSourceData( int iSrc )
{
	bncs_string ssText = "doh!";
	// given src - get all data out as a nice string and write into list box for all the local srcs
	// empty list box
	getAllRouterLabelText(iSrc, &ssText );
	bncs_string sstr = "remove.id=" + bncs_string(iSrc-1);
	textPut(sstr, MAIN_PANEL, "ListLocal" ); 
	textPut("add", ssText, MAIN_PANEL, "ListLocal" ); 
}


void SoftNameSummary::fillLocalSourceData( void )
{
	int iSrc;
	bncs_string ssText = "doh!";
	// given src - get all data out as a nice string and write into list box for all the local srcs
	// empty list box
	textPut( "clear", MAIN_PANEL, "ListLocal" );	
	for ( iSrc=1;iSrc<=iRouterSourceSize;iSrc++) {
		ssText=bncs_string(iSrc);
		getAllRouterLabelText(iSrc, &ssText );
		textPut("add", ssText, MAIN_PANEL, "ListLocal" ); 
	}
}


BOOL SoftNameSummary::getAllRouterLabelText( int iSourceNum, bncs_string *szTextData )
{
	// get all data in form of text for display in list box
	bncs_string sstr, szSrcName, szAuto, szMan, iAuto, iPerm, ssLabel;
	char szTmp[32]="", szsz[BUFFER_CONST]= "                                                                                                  ";

	// get db0 name from router dev ini file
	routerName( iRouterDevice, 0, iSourceNum, sstr );
	szSrcName = sstr.upper();
	wsprintf( szTmp, " %03d ", iSourceNum );
	if (isSourceExternal(iSourceNum)==TRUE) {
		strcat(szTmp, "(E) " );
	}
	else if (isSourceLooped(iSourceNum)==TRUE) {
		strcat(szTmp, "(L) " );
	}
	else
		strcat( szTmp, "    " );
	strncpy(szsz, szTmp, 9);
	strncpy(szsz+11, szSrcName, szSrcName.length());
	*szTextData = szsz;

	// no need any more
	if (allSoftData->getAllRouterLabelData(iSourceNum, &szAuto, &szMan, &iAuto, &iPerm, &ssLabel )==TRUE) {
		// mode and softname
		if (iAuto=="MANUAL") {
			strncpy ( szsz+22, "manual", 6 );
			if (szMan.length()>0) strncpy ( szsz+31, szMan, szMan.length() );
		}
		else {
			strncpy ( szsz+21, "  auto  ", 6 );
			if (szAuto.length()>0) strncpy ( szsz+31, szAuto, szAuto.length() );
		}
		// sncs alias
		if (ssLabel.length()>0) {
			strncpy(szsz+47, ssLabel, ssLabel.length());
		}
		// remove trailing spaces
		szsz[57]=0;
		*szTextData = szsz;
		return TRUE;
	}
	return FALSE;
}

BOOL SoftNameSummary::getAllSncsLabelText(  bncs_string szSncsName, bncs_string szTextData  )
{
	return FALSE;
}

	
