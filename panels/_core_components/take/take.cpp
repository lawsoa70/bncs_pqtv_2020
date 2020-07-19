#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "take.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( take )

// constructor - equivalent to ApplCore STARTUP
take::take( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_device = 0;
	m_lockDevice = 0;
	m_dest = -1;
	m_source = -1;
	m_undoPossible = false;
	m_state = false;


	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "p1.bncs_ui" );
	setSize( 1 );

	controlDisable( 1, "take" );
	controlDisable( 1, "undo" );
}

// destructor - equivalent to ApplCore CLOSEDOWN
take::~take()
{ 
}
 
// all button pushes and notifications come here
void take::buttonCallback( buttonNotify *b )
{
	if( b->id() == "take" )
	{
		hostNotify( bncs_string( "take.%1=%2").arg( m_dest ).arg( m_source ) );
		m_undoPossible = true;
		checkEnableState();
	}
	if( b->id() == "undo" )
	{
		hostNotify( bncs_string( "undo=%1").arg( m_dest ) );
		m_undoPossible = false;
		checkEnableState();
	}
}

// all revertives come here
int take::revertiveCallback( revertiveNotify * r )
{
//	debug( "lock device %1 dest %2 %3", m_lockDevice, m_dest, r->sInfo() );

	if(( r->index() == m_dest ) && (r->device() == m_lockDevice ))
	{
		m_state = atoi( r->sInfo() );

		checkEnableState();
	}
	return 0;
}
 
// all database name changes come back here
void take::databaseCallback( revertiveNotify * r )
{
} 


// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string take::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		m_instance = p->value();
		getDev( m_instance, &m_device );
	}
	else if( p->command() == "lock_instance" )
	{
		m_lockInstance = p->value();
		getDev( m_lockInstance, &m_lockDevice );
	}
	else if( p->command() == "dest" )
	{
		m_dest = p->value();

		if( m_lockDevice && (m_dest > 0 ) )
		{
			// go off and get the take state
			infoRegister( m_lockDevice, m_dest, m_dest );
			infoPoll( m_lockDevice, m_dest, m_dest );
		}

		bncs_string name;
		routerName( m_device, 1, p->value(), name );

		if( name == "---" )
		{
			textPut( "text=", 1, "dest" );
			textPut( "stylesheet=dest_deselected", 1, "dest" );
		}
		else
		{
			name.replace( "|", " " );
			textPut( "text", name, 1, "dest" );
			textPut( "stylesheet=dest_selected", 1, "dest" );
		}


		textPut("text", (m_dest>0)?bncs_string("Dest|%1").arg(m_dest):"Dest|-", 1, "preset_dest");

		m_undoPossible = false;
		checkEnableState();

	}
	else if( p->command() == "source" )
	{
		m_source = p->value();

		bncs_string name;
		routerName( m_device, 0, p->value(), name );

		if( name == "---" )
		{
			textPut( "text=", 1, "source" );
			textPut( "stylesheet=source_deselected", 1, "source" );
		}
		else
		{
			name.replace( "|", " " );
			textPut( "text", name, 1, "source" );
			textPut( "stylesheet=source_selected", 1, "source" );
		}
		textPut("text", (m_source>0)?bncs_string("Source|%1").arg(m_source):"Source|-", 1, "preset_source");

		checkEnableState();
	}
	else if( p->command() == "return" )
	{
		bncs_stringlist sl;

		sl << "lock_instance=" + m_lockInstance;

		return sl.toString( '\n' );
	}
	else if( p->command() == "_events" )
	{
		bncs_stringlist sl;

		sl << "take";
		sl << "undo";

		return sl.toString( '\n' );
	}
	else if( p->command() == "_commands" )
	{
		bncs_stringlist sl;

		sl << "dest=<dest index>";
		sl << "source=<source index>";

		return sl.toString( '\n' );
	}
	return "";
}

// timer events come here
void take::timerCallback( int id )
{
}


void take::checkEnableState( void )
{
	if( m_source < 1 )
	{
		controlDisable( 1, "take" );
		controlDisable( 1, "undo" );
		return;
	}
	if( m_dest < 1 )
	{
		controlDisable( 1, "take" );
		controlDisable( 1, "undo" );
		return;
	}
	if( m_state )
	{
		controlDisable( 1, "take" );
		controlDisable( 1, "undo" );
		return;
	}

	controlEnable( 1, "take" );
	
	if(	m_undoPossible )
		controlEnable( 1, "undo" );
	else
		controlDisable( 1, "undo" );
}