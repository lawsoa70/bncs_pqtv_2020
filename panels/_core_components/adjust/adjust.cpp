#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "adjust.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( adjust )

// constructor - equivalent to ApplCore STARTUP
adjust::adjust( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_database=-1;
	m_device=0;
	m_offset=0;

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "p1.bncs_ui" );
	controlDisable( 1, 1 );
}

// destructor - equivalent to ApplCore CLOSEDOWN
adjust::~adjust()
{
}

// all button pushes and notifications come here
void adjust::buttonCallback( buttonNotify *b )
{
	if( m_adjustName.length() )
		navigateAdjust( m_adjustName );
}

// all revertives come here
int adjust::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void adjust::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string adjust::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		m_instance = p->value();
	}
	else if( p->command() == "index" )
	{
		getDev( m_instance, &m_device, &m_offset );

		bncs_string strAdjustInstance, strConnector;
		bncs_string ret;
		routerName( m_device, m_database, p->value(), ret );

		if( ret == "---" || ret == "!!!" )
			debug("core: adjust: can't adjust %1 instance %2, device %3 supplied index in db%4 doesn't resolve to a name", p->value(), m_instance, m_device, m_database);

		if(ret.contains('#'))
		{
			ret.split('#', strAdjustInstance, strConnector);
		}
		else
		{
			strAdjustInstance = ret;
		}

		if( navigateCanAdjust( strAdjustInstance ))
		{
			controlEnable( 1, 1 );
			m_adjustName = strAdjustInstance;
		}
		else
		{
			debug( "core: adjust: can't adjust %1", strAdjustInstance );
			m_adjustName = "";
			controlDisable( 1, 1 );
		}
	}
	else if( p->command() == "name" )
	{
		if( navigateCanAdjust( p->value() ))
		{
			controlEnable( 1, 1 );
			m_adjustName = p->value();
		}
		else
		{
			m_adjustName = "";
			controlDisable( 1, 1 );
		}
	}
	else if( p->command() == "database" )
	{
		m_database = p->value();
	}
	else if( p->command() == "text" )
	{
		m_text = p->value();

		textPut( "text", m_text, 1, 1 );
	}
	else if( p->command() == "return" )
	{
		bncs_stringlist sl;

		sl << bncs_string( "database=%1" ).arg( m_database );
		sl << "text=" + m_text;

		return sl.toString( '\n' );
	}
	else if( p->command() == "_events" )
	{
//		bncs_stringlist sl;

//		sl << "dest=<dest index>";

//		return sl.toString( '\n' );
	}
	else if( p->command() == "_commands" )
	{
		bncs_stringlist sl;

		sl << "name=<name>";
		sl << "instance=<instance name>";
		sl << "database=<router database no>";
		sl << "index=<router index>";

		return sl.toString( '\n' );
	}
	return "";
}

// timer events come here
void adjust::timerCallback( int id )
{
}
