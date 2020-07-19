#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "tracebar.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( tracebar )

// constructor - equivalent to ApplCore STARTUP
tracebar::tracebar( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	init();
}

// destructor - equivalent to ApplCore CLOSEDOWN
tracebar::~tracebar()
{
}

// all button pushes and notifications come here
void tracebar::buttonCallback( buttonNotify *b )
{
	//Ignore button presses - read only mode
	if( !m_blnReadOnly )
	{
		if( b->id().startsWith( "button_" ))
		{
			bncs_string sAdjustName;

			int i = b->id().remove( 0, 7 ).toInt() - 1;

			sAdjustName = m_adjustnames[ i ];

			if( navigateCanAdjust( sAdjustName ))
				navigateAdjust(sAdjustName);
		}
	}
}

int tracebar::revertiveCallback( revertiveNotify * r )
{
	bool initialSetup = false;	// if we're still iterating then don't allow the dynamic update stuff to operate on the incoming data

	int iSource = 0;
	if (m_packager)
	{
		bncs_stringlist sltSourceIndex = bncs_stringlist().fromString(r->sInfo());
		iSource = sltSourceIndex.getNamedParam("index").toInt();
	}
	else
	{
		iSource = r->info();
	}

	//debug("tracebar::revertiveCallback[%1] device: %2 source: %3 destination: %4", m_packager, r->device(), iSource, r->index());

	if( (r->device() == m_iterationDevice) && ( r->index() == m_iterationDestination ))
	{
		initialSetup = true;
		if (iSource)
		{
			routerName(m_iterationDevice, 0, iSource, m_iterationRouterName);
			timerStart( 1, 1 );
		}
		else
		{
			//debug("tracebar::revertiveCallback() no routing found, iteration ends.");
			m_iterationDevice = 0;
			m_iterationDestination = 0;
			m_iterationRouterName = "";
		}
	}

	if( m_dynamicUpdates )
	{
		// find if there's a reverse map entry for this destination
		map< DWORD, DWORD >::iterator it = m_reverseMap.find( MAKELONG( r->device(), r->index() ));
		if( it != m_reverseMap.end() )
		{
			// pos and tally are stored in the value part stuffed into a DWORD
			int pos = LOWORD( it->second );
			int tally = HIWORD( it->second );

			// is this a change of state?
			if (tally != iSource)
			{
				//debug( "tracebar::+++ got new tally for position %1 of %2 (was %3)", pos, iSource, tally);

				// remember the new tally in the map
				it->second = MAKELONG(pos, iSource);

				// are we still iterating - if so, leave well alone, if this is a dynamic update then go do it!
				if( !initialSetup )
				{
					resetTraceFrom( pos );

					m_iterationDatabase = DB_SOURCE;
					m_iterationDevice = r->device();
					m_iterationDestination = 0;

					routerName(m_iterationDevice, m_iterationDatabase, iSource, m_iterationRouterName);
					
					timerStart( 1, 1 );
				}
			}
		}
	}
	return 0;
}

// all database name changes come back here
void tracebar::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string tracebar::parentCallback( parentNotify *p )
{
	//debug("tracebar::parentCallback command=%1 value=%2", p->command(), p->value());

	if (p->command() == "instance")
	{
		m_sInstance = p->value();

		if (m_sInstance.find("package") > -1)
			m_packager = true;
		else
			m_packager = false;

		//debug("tracebar::parentCallback command=%1 value=%2 - m_packager=%3", p->command(), p->value(), m_packager);

		resetTrace();
		timerStart(1, 1);
	}
	else if( p->command() == "panel" )
	{
		panelDestroy(1);
		m_sPanel = p->value();
		panelShow(1, m_sPanel );
		setSize( 1 );
		m_idlist = getIdList(1, "button_" );
		timerStart(1, 1 );
	}
	else if (p->command() == "source")
	{
		resetTrace();

		int source = p->value();
		if( source > -1 )
		{
			m_iterationDatabase = DB_SOURCE;
			routerName( m_iterationDevice, DB_SOURCE, source, m_iterationRouterName );

			timerStart(1, 1);
		}
	}
	else if (p->command() == "dest")
	{
		resetTrace();

		int dest = p->value();
		if( dest > -1 )
		{
			m_iterationDatabase = DB_DESTINATION;
			routerName( m_iterationDevice, DB_DESTINATION, dest, m_iterationRouterName );

			timerStart(1, 1);
		}
	}
	else if (p->command() == "stylesheet" && p->subs())
	{
		if (p->sub(0) == "unused")
		{
			m_sUnusedStyle = p->value();
		}
		else if (p->sub(0) == "source")
		{
			m_sSourceStyle = p->value();
		}
		else if (p->sub(0) == "route")
		{
			m_sRouteStyle = p->value();
		}
		else if (p->sub(0) == "destination")
		{
			m_sDestinationStyle = p->value();
		}
	}
	else if (p->command() == "readonly" && p->value() == "true")
	{
		m_blnReadOnly = true;
		for (bncs_stringlist::iterator it = m_idlist.begin(); it != m_idlist.end(); it++)
		{
			textPut("style", "label", 1, *it);
		}
	}
	else if( p->command() == "dynamic" )
	{
		m_dynamicUpdates = false;

		if( p->value() == 1 )
			m_dynamicUpdates = true;
		if( p->value() == "true" )
			m_dynamicUpdates = true;
	}
	else if (p->command() == "return")
	{
		bncs_stringlist sl;

		sl << "panel=" + m_sPanel; 
		sl << "stylesheet.destination=" + m_sDestinationStyle;
		sl << "stylesheet.source=" + m_sSourceStyle;
		sl << "stylesheet.route=" + m_sRouteStyle;
		sl << "stylesheet.unused=" + m_sUnusedStyle;
		sl << bncs_string( "dynamic=" ) + (m_dynamicUpdates?"1":"0");

		return sl.toString( '\n' );
	}
	else if( p->command() == "_events" )
	{
/*		bncs_stringlist sl;

		sl << "";

		return sl.toString( '\n' );
*/	}
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
void tracebar::timerCallback( int id )
{
	timerStop( id );
	iteration();
}

void tracebar::iteration( void )
{
	//debug("tracebar::iteration(%1) device=%2 name='%3' database=%4", m_packager, m_iterationDevice, m_iterationRouterName, m_iterationDatabase);
	int iIndexPos;

	if( !m_iterationDevice || !m_iterationRouterName.length() || (m_iterationDatabase == DB_UNKNOWN) ) 
		return;

	if (!showName())
	{
		//debug("tracebar::iteration() no more button found, iteration ends.");
		m_iterationDevice = 0;
		m_iterationDestination = 0;
		m_iterationRouterName = "";
		return;
	}

	if( m_iterationDatabase == DB_DESTINATION )
	{
		//debug("tracebar::iteration() POLL");
		m_iterationDestination = routerIndex( m_iterationDevice, m_iterationDatabase, m_iterationRouterName );
		
		if (m_packager)
		{
			infoRegister(m_iterationDevice, m_iterationDestination, m_iterationDestination, true);
			infoPoll(m_iterationDevice, m_iterationDestination, m_iterationDestination);
			//debug("tracebar::iteration() dest infoPoll %1 %2 %3", m_iterationDevice, m_iterationDestination, m_iterationDestination);
		}
		else
		{
			routerRegister(m_iterationDevice, m_iterationDestination, m_iterationDestination, true);
			routerPoll(m_iterationDevice, m_iterationDestination, m_iterationDestination);
			//debug("tracebar::iteration() dest routerPoll %1 %2 %3", m_iterationDevice, m_iterationDestination, m_iterationDestination);
		}
		m_iterationDatabase = DB_SOURCE;

		if( m_dynamicUpdates )
			m_reverseMap.insert( make_pair( MAKELONG( m_iterationDevice, m_iterationDestination ), m_iterationPosition ) );
	}
	else if (m_iterationDatabase == DB_SOURCE)
	{
		//debug("tracebar::iteration() LOOKUP index");
		// lookup if device start a router list
		for (iIndexPos=0; iIndexPos<m_iNoIndexes; iIndexPos++)
		{
			if (m_routerList[iIndexPos][0].toInt() == m_iterationDevice)
			{
				//debug("tracebar::iteration() index found for device: %1 -> %2", m_iterationDevice, iIndexPos);
				break;
			}
		}

		if (iIndexPos == m_iNoIndexes)
		{
			//debug("tracebar::iteration() router not found as first of router list, iteration ends.");
			m_iterationDevice = 0;
			m_iterationDestination = 0;
			m_iterationRouterName = "";
			return;
		}

		//debug("tracebar::iteration(%1) LOOKUP connection", m_packager);
		// lookup if router name found in devices of router list as destination
		int iRouterListLength = m_routerList[iIndexPos].count();
		int iRouterPos;
		for (iRouterPos=0; iRouterPos<iRouterListLength; iRouterPos++)
		{
			m_iterationDevice = m_routerList[iIndexPos][iRouterPos].toInt();
			m_iterationDestination = routerIndex( m_iterationDevice, DB_DESTINATION, m_iterationRouterName );
			if (!m_iterationDestination)
			{
				bncs_string piped_sRouterName = m_iterationRouterName;
				piped_sRouterName.replace(' ', '|');

				//debug("tracebar::iteration() lookup piped name: %1 ", piped_sRouterName);
				
				m_iterationDestination = routerIndex(m_iterationDevice, DB_DESTINATION, piped_sRouterName);	
			}
			//debug("tracebar::iteration() lookup device: %1 name: %2 destination: %3", m_iterationDevice, m_iterationRouterName, m_iterationDestination);
			if (m_iterationDestination)
			{
				//debug("tracebar::iteration() device and destination found for name: %1 -> %2 , %3", m_iterationRouterName, m_iterationDestination, m_iterationDevice);
				if (m_packager)
				{
					infoRegister(m_iterationDevice, m_iterationDestination, m_iterationDestination, true);
					infoPoll(m_iterationDevice, m_iterationDestination, m_iterationDestination);
					//debug("tracebar::iteration() infoPoll %1 %2 %3", m_iterationDevice, m_iterationDestination, m_iterationDestination);
				}
				else
				{
					routerRegister(m_iterationDevice, m_iterationDestination, m_iterationDestination, true);
					routerPoll(m_iterationDevice, m_iterationDestination, m_iterationDestination);
					//debug("tracebar::iteration() routerPoll %1 %2 %3", m_iterationDevice, m_iterationDestination, m_iterationDestination);
				}

				if( m_dynamicUpdates )
					m_reverseMap.insert( make_pair( MAKELONG( m_iterationDevice, m_iterationDestination ), m_iterationPosition) );
				break;
			}
		}

		if (iRouterPos == iRouterListLength)
		{
			//debug("tracebar::iteration() destination name not found among the router list, iteration ends.");
			m_iterationDevice = 0;
			m_iterationDestination = 0;
			m_iterationRouterName = "";
			return;
		}
	}

}

bool tracebar::showName( void )
{
	//debug("tracebar::showName[%1] m_iterationPosition=%2 m_iterationDatabase=%3 m_iterationRouterName=%4", 
	//	m_packager, m_iterationPosition, m_iterationDatabase, m_iterationRouterName);

	bncs_string buttonId = bncs_string( "button_%1" ).arg( m_iterationPosition + 1 );

	textPut("text", m_iterationRouterName, 1, buttonId );

	textPut("stylesheet", m_iterationDatabase ? m_sDestinationStyle : m_sSourceStyle, 1, buttonId );

	// storing adjust names
	int iSlot = routerIndex(m_iterationDevice, m_iterationDatabase, m_iterationRouterName);
	bncs_string sAdjustName;
	routerName(m_iterationDevice, m_iterationDatabase ? DB_DESTINATION_ADJUST : DB_SOURCE_ADJUST, iSlot, sAdjustName);

	if( navigateCanAdjust( sAdjustName ))
	{
		m_adjustnames[ m_iterationPosition ] = sAdjustName;
		textPut("pixmap", "/images/navigateAvailable.png", 1, bncs_string( "button_%1" ).arg( m_iterationPosition + 1) );
	}
	else
		textPut("pixmap=", 1, buttonId );
	
	//debug("tracebar::Appending adjustnames: %1 %2 %3 %4", m_iterationDevice, m_iterationDatabase ? DB_DESTINATION_ADJUST : DB_SOURCE_ADJUST, iSlot, sAdjustName);

	if (m_iterationPosition > 1)
	{
		textPut("stylesheet", m_sRouteStyle, 1, bncs_string( "button_%1" ).arg( m_iterationPosition ) );
	}

	m_iterationPosition++;

	buttonId = bncs_string( "button_%1" ).arg( m_iterationPosition + 1 );

	if (m_idlist.find( buttonId ) == -1)
		return false;

	return true;
}

void tracebar::init( void )
{
	m_iterationDevice=0;
	m_iterationDestination=1;
	m_iterationPosition=0;
	m_dynamicUpdates = false;
	m_packager = false;
	
	bncs_string sValue;
	m_iNoIndexes = 0;

	m_blnReadOnly = false;
	m_sUnusedStyle = "default";
	m_sSourceStyle = "source_selected";
	m_sRouteStyle = "default";
	m_sDestinationStyle = "dest_selected";

	for( int i=0; i<MAX_INDEXES; i++ )
	{
		sValue = getObjectSetting("TraceBar", bncs_string("Index_%1").arg(i+1, '0', 3));
		if (sValue.length())
		{
			m_routerList[i] = bncs_stringlist(sValue);
			//debug("tracebar::init() : %1 = %2", bncs_string("Index_%1").arg(i+1, '0', 3), sValue);
		}
		else
		{
			m_iNoIndexes = i;
			break;
		}
	}
	m_iterationDatabase = DB_UNKNOWN;
}

void tracebar::resetTrace( void )
{
	//debug("tracebar::resetTrace() '%1'", m_sInstance );
	getDev(m_sInstance, &m_iterationDevice);

	routerUnregister( 0 );	// 0 means all!
	
	m_iterationPosition = 0;
	for( bncs_stringlist::iterator it = m_idlist.begin(); it != m_idlist.end(); it++ )
	{
		textPut( "text", "", 1, *it);
		textPut( "stylesheet", m_sUnusedStyle, 1, *it );
		textPut( "pixmapalign=topright", 1, *it );
		textPut( "pixmap=", 1, *it );
	}
	m_adjustnames.clear();

	m_reverseMap.clear();
}

void tracebar::resetTraceFrom( int position )
{
	// reset current position
	// reset adjust list
	// clear upstream buttons
	// reset reverse map
	// reset iteration device/database/dest/name
	// kick off iteration

	m_iterationPosition = position;

	for( int i = position + 2; i <= m_idlist.count() ; i++ )
	{
		bncs_string buttonName = bncs_string( "button_%1" ).arg( i );

		textPut( "text", "", 1, buttonName );
		textPut( "stylesheet", m_sUnusedStyle, 1, buttonName );
		textPut( "pixmap=", 1,  buttonName );
	}

	// erase all the upstream adjust names
	for( map< int, bncs_string>::iterator ait = m_adjustnames.begin() ; ait!= m_adjustnames.end() ; )
	{
		if( ait->first >= position )
		{
			//debug( "tracebar:+++ removing adjust string %1 %2", ait->first, ait->second );
			ait = m_adjustnames.erase( ait );
		}
		else
		{
			//debug( "tracebar:+++ NOT removing adjust string %1 %2", ait->first, ait->second );
			++ait;
		}
	}

	//debug( "tracebar::+++position %1", position );
	// reset the upstream reverse map entries
	for( map< DWORD, DWORD >::iterator rit = m_reverseMap.begin() ; rit != m_reverseMap.end() ; )
	{
		int pos = LOWORD( rit->second );
		if(  pos > position )
		{
			//debug( "tracebar::+++removing reverse map for %1 %2 %3", pos, LOWORD( rit->first ), HIWORD( rit->first ));
			rit = m_reverseMap.erase( rit );
		}
		else
		{
			//debug( "tracebar::+++NOT removing reverse map for %1 %2 %3", pos, LOWORD( rit->first ), HIWORD( rit->first ));
			++rit;
		}
	}
}
