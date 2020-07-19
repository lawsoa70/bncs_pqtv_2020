#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "routerDetails.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( routerDetails )

// constructor - equivalent to ApplCore STARTUP
routerDetails::routerDetails( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_device=0;
	m_offset=0;
	m_database=0;

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "p1.bncs_ui" );
	controlDisable( 1, 1 );
}

// destructor - equivalent to ApplCore CLOSEDOWN
routerDetails::~routerDetails()
{
}

// all button pushes and notifications come here
void routerDetails::buttonCallback( buttonNotify *b )
{
	panelPopup( 2, "details.bncs_ui" );

	getDev( m_instance, &m_device, &m_offset );

	bncs_stringlist sltDatabases = bncs_stringlist().fromString(m_strDatabase);

	bncs_string strDetails = "";

	for(int intField = 0; intField < sltDatabases.count(); intField++)
	{
		bncs_string ret;
		routerName( m_device, sltDatabases[intField].toInt(), m_index, ret );

		if(intField > 0)
		{
			strDetails.append(" - ");
		}
		strDetails.append(ret);
	}

	textPut( "text", strDetails, 2, 1 );
}

// all revertives come here
int routerDetails::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void routerDetails::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string routerDetails::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		m_instance = p->value();
	}
	else if( p->command() == "index" )
	{
		m_index = p->value();

		if( m_index > 0 )
			controlEnable( 1, 1 );
		else
			controlDisable( 1, 1 );
	}
	else if( p->command() == "database" )
	{
		//m_database = p->value();
		m_strDatabase = p->value();
	}
	else if( p->command() == "text" )
	{
		m_text = p->value();

		textPut( "text", m_text, 1, 1 );
	}
	else if( p->command() == "return" )
	{
		bncs_stringlist sl;

		sl << bncs_string( "database=%1" ).arg( m_strDatabase );
		sl << "text=" + m_text;

		return sl.toString( '\n' );
	}

	return "";
}

// timer events come here
void routerDetails::timerCallback( int id )
{
}
