#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "lock.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( lock )

// constructor - equivalent to ApplCore STARTUP
lock::lock( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_device = 0;

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "p1.bncs_ui" );
	controlDisable( 1, 1 );
}

// destructor - equivalent to ApplCore CLOSEDOWN
lock::~lock()
{ 
}
 
// all button pushes and notifications come here
void lock::buttonCallback( buttonNotify *b )
{
	if( m_device && (m_index > 0 ))
	{
		if( m_state )
			infoWrite( m_device, "0", m_index );
		else
			infoWrite( m_device, "1", m_index );
	}
}

// all revertives come here
int lock::revertiveCallback( revertiveNotify * r )
{
	if(( r->index() == m_index ) && (r->device() == m_device ))
	{
		m_state = atoi( r->sInfo() );

		if( m_state )
		 	textPut( "statesheet", "dest_locked", 1, 1 );
		else
		 	textPut( "statesheet", "dest_unlocked", 1, 1 );
	}
	return 0;
}
 
// all database name changes come back here
void lock::databaseCallback( revertiveNotify * r )
{
} 


// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string lock::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		m_instance = p->value();
		getDev( m_instance, &m_device );
	}
	else if( p->command() == "index" )
	{
		m_index = p->value();

		if( m_device && (m_index > 0 ) )
		{
			controlEnable( 1, 1 );

			// put a value to the button just to indicate this is the unknown state
		 	textPut( "statesheet", "enum_unknown", 1, 1 );

			// go off and get the lock state
			infoRegister( m_device, m_index, m_index );
			infoPoll( m_device, m_index, m_index );
		}
		else
			controlDisable( 1, 1 );
	}
	return "";
}

// timer events come here
void lock::timerCallback( int id )
{
}
