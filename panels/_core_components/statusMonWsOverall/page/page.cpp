#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "page.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( page )

#define UNKNOWN		-1
#define OK			0
#define ALARM		1

// constructor - equivalent to ApplCore STARTUP
page::page( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_alarmState = UNKNOWN;

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "page.bncs_ui" );
}

// destructor - equivalent to ApplCore CLOSEDOWN
page::~page()
{
}

// all button pushes and notifications come here
void page::buttonCallback( buttonNotify *b )
{
	hostNotify( b->command() + "=" + b->value() );
}

// all revertives come here
int page::revertiveCallback( revertiveNotify * r )
{
	// find this workstation in our list
	map< int, bncs_string >::iterator it = m_workstations.find( r->index() );

	// check we got it
	if( it != m_workstations.end() )
	{
		// remember the current status string
		it->second = r->sInfo();

		// is zero length (workstation broken) and we're in NOT in the alarm state
		if( !r->sInfo().length() && (m_alarmState != ALARM ))
		{
			// just be a new alarm then!
			m_alarmState = ALARM;
			textPut( "stylesheet=enum_alarm", 1, 1 );
			textPut( "text", m_text, 1, 1 );
		}
		else
			checkAlarmState();
	}
	return 0;
}

// all database name changes come back here
void page::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string page::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		getDev( p->value(), &m_device, 0 );
	}
	else if( p->command() == "add" )
	{
		int ws = p->value();

		if( ws )
		{
			m_workstations.insert( make_pair( ws, "" ));

			infoRegister( m_device, ws, ws, true );
			infoPoll( m_device, ws, ws );
		}
		else
			debug( "page: invalid workstation number passed to statusMonWs/page script" );
	}
	else if( p->command() == "text" )
	{
		textPut( "text", p->value(), 1, 1 );
		m_text = p->value();
	}
	return "";
}

// timer events come here
void page::timerCallback( int id )
{
}


void page::checkAlarmState( void )
{
	// rattle through our list of workstations
	for( map< int, bncs_string >::iterator it = m_workstations.begin() ; it != m_workstations.end() ; ++it )
	{
		// are any of them zero length (workstation broken)
		if( !it->second.length() )
		{
			// are we NOT in the alarm state (if we, don't bother reasserting the text/colour
			if( m_alarmState != ALARM )	
			{
				m_alarmState = ALARM;
				textPut( "stylesheet=enum_alarm", 1, 1 );
			}
			return;
		}
	}
	// if we got here then we didn't find any that were in the error state....
	// so the world must be a happy place and we can mark ourselves as OK!
	if( m_alarmState != OK )
	{
		m_alarmState = OK;
		textPut( "stylesheet=enum_ok", 1, 1 );
	}
}
