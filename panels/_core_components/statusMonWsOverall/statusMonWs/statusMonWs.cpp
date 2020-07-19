#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "statusMonWs.h"

map< int, bncs_string > statusMonWs::m_workstationNames;

#define UPDATE_TIME 10000
#define UNKNOWN		-1
#define OK			0
#define ALARM		1

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( statusMonWs )

// constructor - equivalent to ApplCore STARTUP
statusMonWs::statusMonWs( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_ws = 0;
	m_device = 0;
	m_lastUpdated = 0;
	m_lastEntry = "start condition";
	m_alarmState = UNKNOWN;

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "p1.bncs_ui" );

	setSize( 1 );

	// once-off load of workstation names to static memory
	loadWorkstationNames();
}

// destructor - equivalent to ApplCore CLOSEDOWN
statusMonWs::~statusMonWs()
{
}

// all button pushes and notifications come here
void statusMonWs::buttonCallback( buttonNotify *b )
{
}

// all revertives come here
int statusMonWs::revertiveCallback( revertiveNotify * r )
{
	if( r->sInfo() == "starting" )
		textPut( "text=Wait...", 1, 1 );
	else if( r->sInfo().length() )
	{
		if( m_lastEntry != r->sInfo() )
			m_lastUpdated = GetTickCount();

		m_lastEntry = r->sInfo();

		if( m_alarmState != OK )
		{
			textPut( "stylesheet=enum_ok", 1, 1 );
			textPut( "text=Ok", 1, 1 );
	
			int time = (GetTickCount() - m_lastUpdated + UPDATE_TIME) / 1000;
			textPut( "text", bncs_string( "Last updated < %1 seconds ago" ).arg( time ), 1, "lastUpdated" );
		}
		m_alarmState = OK;

		timerStart( 1, UPDATE_TIME );
	}
	else
	{
		if( m_alarmState != ALARM )
		{
			textPut( "stylesheet=enum_alarm", 1, 1 );
			textPut( "text=Alarm", 1, 1 );
		}
		m_alarmState = ALARM;

		textPut( "text=", 1, "lastUpdated" );
		textPut( "stylesheet", "background_discrete", 1, "lastUpdated" );

		m_lastUpdated = 0;
		m_lastEntry = "";

		timerStop( 1 );
	}

	return 0;
}

// all database name changes come back here
void statusMonWs::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string statusMonWs::parentCallback( parentNotify *p )
{
	if( p->command() == "return" )
	{
		bncs_stringlist sl;

		sl << "workstation=" + bncs_string( "%1" ).arg( m_ws );

		return sl.toString( '\n' );
	}
	else if( p->command() == "workstation" )
	{
		m_ws = p->value();

		check();
	}
	else if( p->command() == "instance" )
	{
		m_instance = p->value();

		check();
	}
	return "";
}

// timer events come here
void statusMonWs::timerCallback( int id )
{
	if( m_lastUpdated )
	{
		int time = (GetTickCount() - m_lastUpdated + UPDATE_TIME) / 1000;
		textPut( "text", bncs_string( "Last updated < %1 seconds ago" ).arg( time ), 1, "lastUpdated" );

		if( time > 70 )
			textPut( "stylesheet", "enum_warning", 1, "lastUpdated" );
		else
			textPut( "stylesheet", "background_discrete", 1, "lastUpdated" );
	}
	else
		textPut( "text", "Never", 1, "lastUpdated" );

	infoPoll( m_device, m_ws, m_ws );
}

bncs_string statusMonWs::workstationName( void )
{
	map< int, bncs_string>::iterator it = m_workstationNames.find( m_ws );

	if( it != m_workstationNames.end() )
		return it->second;

	return bncs_string( "Workstation %1" ).arg( m_ws );
}	

void statusMonWs::loadWorkstationNames( void )
{
	if( !m_workstationNames.size() )
	{
		bncs_config c( "workstations" );

		while( c.isChildValid() )
		{
			bncs_string name = c.childAttr( "name" );
			int ws = c.childAttr( "id" );

			m_workstationNames.insert(  make_pair( ws, name ));

			c.nextChild();
		}
	}
}


void statusMonWs::reset( void )
{
	textPut( "text", "", 1, "lastUpdated" );
	textPut( "text", "", 1, "name" );
	textPut( "stylesheet=default", 1, 1 );
	textPut( "text", "", 1, 1 );
	textPut( "stylesheet", "background_discrete", 1, "lastUpdated" );
	m_alarmState = UNKNOWN;
}

void statusMonWs::check( void )
{
	reset();

	textPut( "text", m_ws, 1, "number" );

	if( m_instance.length() && m_ws )
	{
		textPut( "text", workstationName(), 1, "name" );


		getDev( m_instance, &m_device, 0 );

		if( m_device )
		{
			infoRegister( m_device, m_ws, m_ws );
			infoPoll( m_device, m_ws, m_ws );
		}
		else
			textPut( "text", "No workstation|status infodriver", 1, "name" );
	}
}