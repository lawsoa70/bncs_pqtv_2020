#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "statusmonwsoverall.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( statusmonwsoverall )

// constructor - equivalent to ApplCore STARTUP
statusmonwsoverall::statusmonwsoverall( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "statusmonwsoverall.bncs_ui" );
	controlHide( 1, "ignore" );
	controlHide( 1, "ignore1" );

	// get the number of workstation buttons - we'll use this to work out how big the pages are (for targeting)
	bncs_stringlist buttons = getIdList( 1, "ws_" );
	m_noWorkstationButtons = buttons.count();

	// get the number of page buttons
	buttons = getIdList( 1, "page_" );
	m_noPageButtons = buttons.count();

	loadIgnoreList();
	loadWorkstations();

	// start with the first page
	loadPage( 1 );
}

// destructor - equivalent to ApplCore CLOSEDOWN
statusmonwsoverall::~statusmonwsoverall()
{
}

// all button pushes and notifications come here
void statusmonwsoverall::buttonCallback( buttonNotify *b )
{
	bncs_string p, n;

	b->id().split( '_', p, n );

	if( p == "page" )
	{
		int page = n;

		if( page )
		{
			loadPage( page );

			// this ensures that we move the tab by it's height / distance between page buttons
			controlMove( 1, "tab", 0, 100 * (page - 1) );
		}
	}
}

// all revertives come here
int statusmonwsoverall::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void statusmonwsoverall::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string statusmonwsoverall::parentCallback( parentNotify *p )
{
	if( p->command() == "return" )
	{
		bncs_stringlist sl;
		
//		sl << "myParam=" + m_myParam;
		
		return sl.toString( '\n' );
	}
/*	else if( p->command() == "myParam" )
	{
		m_myParam = p->value();
	}
*/	return "";
}

// timer events come here
void statusmonwsoverall::timerCallback( int id )
{
}


void statusmonwsoverall::loadWorkstations( void )
{
	// rattle through standard V4.5 workstations xml file
	bncs_config c( "workstations" );

	int y = 1;
	int page=0;

	// keep going until we run out of entries
	while( c.isChildValid() )
	{
		int ws = c.childAttr( "id" );

		// sanity check to ensure that this is a likely looking entry
		if( ws )
		{
			if( m_ignoreList.find( ws ) == m_ignoreList.end() )
			{
				// remember in our list
				m_workstations.push_back( ws );

				// from our running total (y) work out what page we're on
				page = ((y-1) / m_noWorkstationButtons ) + 1;

				// tell the page button that it should look at this workstation
				textPut( "add", ws, 1, bncs_string( "page_%1" ).arg( page ));

				y++;
			}
		}
		c.nextChild();
	}

	// name the page buttons
	for( int p = 1 ; p <= m_noPageButtons ; p++ )
	{
		// is valid page
		if( p <= page )
			textPut( "text", bncs_string( "Page %1" ).arg( p ), 1, bncs_string( "page_%1" ).arg( p ));
		else		// not a valid page - so just be tidy and hide the button
			controlHide( 1, bncs_string( "page_%1" ).arg( p ));
	}
}

void statusmonwsoverall::loadPage( int page )
{
	int offset = (page-1) * m_noWorkstationButtons;
	int range = offset + m_noWorkstationButtons;

	int y = 1;	// current button number
	for( int x = offset ; x < range ; x++ )	// x is current offset in our global ws list
	{
		if( x < m_workstations.size() )
		{
			int ws = m_workstations[ x ];

			if( ws )
			{
				textPut( "workstation", ws, 1, bncs_string( "ws_%1" ).arg( y ));
				controlShow( 1, bncs_string( "ws_%1" ).arg( y ));
				y++;
				continue;
			}
		}
		// we'll only hit this code with invalid workstation number ('cause it's zero, or our index is 
		//  more than the number of workstations that we've got listed
		textPut( "workstation=0", 1, bncs_string( "ws_%1" ).arg( y ));
		controlHide( 1, bncs_string( "ws_%1" ).arg( y ));
		y++;
	}
}


void statusmonwsoverall::loadIgnoreList( void )
{
	bncs_string ret;

	textGet( "text", 1, "ignore", ret );

	bncs_stringlist sl( ret );

	m_ignoreList.clear();

	for( bncs_stringlist::const_iterator it = sl.begin() ; it != sl.end() ; ++it )
	{
		if( (*it).contains( '-' ))
		{
			bncs_stringlist bits( *it, '-' );

			if( (int) bits[ 0 ] <= (int) bits[ 1 ] )
				for( int x = bits[ 0 ] ; x <= bits[ 1 ] ; x++ )
					m_ignoreList.insert( x );
		}
		else
			m_ignoreList.insert( *it );
	}
}
