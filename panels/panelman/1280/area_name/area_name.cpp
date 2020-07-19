#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "area_name.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( area_name )

#define PANEL_MAIN 1

// constructor - equivalent to ApplCore STARTUP
area_name::area_name( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PANEL_MAIN, "main.bncs_ui" );

	bncs_string strStudio = getWorkstationSetting("ops_area");

	bncs_string strAreaName = getObjectSetting(strStudio, "area_name");

	textPut("text", strAreaName, PANEL_MAIN, "lblAreaName");



}

// destructor - equivalent to ApplCore CLOSEDOWN
area_name::~area_name()
{
}

// all button pushes and notifications come here
void area_name::buttonCallback( buttonNotify *b )
{
}

// all revertives come here
int area_name::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void area_name::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string area_name::parentCallback( parentNotify *p )
{
	return "";
}

// timer events come here
void area_name::timerCallback( int id )
{
}
