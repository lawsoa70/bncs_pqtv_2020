#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "axonIdentity.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( axonIdentity )

// constructor - equivalent to ApplCore STARTUP
axonIdentity::axonIdentity( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "axonIdentify.bncs_ui" );

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
	setSize( 1 );				// set the size to the same as the specified panel
}

// destructor - equivalent to ApplCore CLOSEDOWN
axonIdentity::~axonIdentity()
{
}

// all button pushes and notifications come here
void axonIdentity::buttonCallback( buttonNotify *b )
{
/*	if( b->panel() == 1 )
	{
		switch( b->id() )
		{
			case 1:			textPut( "text", "you pressed|control 1", 1, 4 );		break;
			case 2:			textPut( "text", "you pressed|control 2", 1, 4 );		break;
			case 3:			textPut( "text", "you pressed|control 3", 1, 4 );		break;
		}
	}
*/
}

// all revertives come here
int axonIdentity::revertiveCallback( revertiveNotify * r )
{
/*	switch( r->device() )
	{
		case 123:
 			textPut( "text", r->sInfo(), 1, 3 );
			break;
	}
*/	return 0;
}

// all database name changes come back here
void axonIdentity::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string axonIdentity::parentCallback( parentNotify *p )
{
	return "";
}

// timer events come here
void axonIdentity::timerCallback( int id )
{
}
