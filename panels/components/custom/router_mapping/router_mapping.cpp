#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_mapping.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_mapping )

// constructor - equivalent to ApplCore STARTUP
router_mapping::router_mapping( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	panelShow( 1, "p1.bncs_ui" );

	setSize( 1 );
}

// destructor - equivalent to ApplCore CLOSEDOWN
router_mapping::~router_mapping()
{
}

// all button pushes and notifications come here
void router_mapping::buttonCallback( buttonNotify *b )
{
	if( b->panel() == 1 )
	{
		if( b->id() == "sources" )
		{
			controlHide( 1, "mapping_destinations" );
			controlShow( 1, "mapping_sources" );

			textPut( "statesheet=enum_selected", b->panel(), b->id() );
			textPut( "statesheet=enum_deselected", b->panel(), "destinations");
		}
		else if( b->id() == "destinations" )
		{
			controlHide( 1, "mapping_sources" );
			controlShow( 1, "mapping_destinations" );

			textPut( "statesheet=enum_selected", b->panel(), b->id() );
			textPut( "statesheet=enum_deselected", b->panel(), "sources");
		}
	}
}

// all revertives come here
int router_mapping::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void router_mapping::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_mapping::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		m_instance = p->value();
		timerStart( 1, 1 );
	}
	if( p->command() == "sourcePanel" )
	{
		m_sourcePanel = p->value();
		timerStart( 1, 1 );
	}
	else if( p->command() == "destPanel" )
	{
		m_destPanel = p->value();
		if(m_destPanel != "")
			controlShow(1,"destinations");
		else
			controlHide(1,"destinations");
		timerStart( 1, 1 );

	}
	else if( p->command() == "return" )
	{
		bncs_stringlist sl;

		sl << "sourcePanel=" + m_sourcePanel;
		sl << "destPanel=" + m_destPanel;

		return sl.toString( '\n' );
	}
	return "";
}

// timer events come here
void router_mapping::timerCallback( int id )
{
	timerStop( id );

	textPut( "panel", m_sourcePanel, 1, "mapping_sources" );
	textPut( "panel", m_destPanel, 1, "mapping_destinations" );

	textPut( "instance", m_instance, 1, "mapping_sources" );
	textPut( "instance", m_instance, 1, "mapping_destinations" );

	controlHide( 1, "mapping_destinations" );
	controlShow( 1, "mapping_sources" );

	textPut( "statesheet=enum_selected", 1, "sources" );
}
