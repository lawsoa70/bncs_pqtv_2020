#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "audio_dest.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( audio_dest )

// constructor - equivalent to ApplCore STARTUP
audio_dest::audio_dest( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	memset( m_tallies, 0, sizeof( m_tallies ));
	
	m_source=-1;
	m_dest=-1;
	m_globalDest=-1;
	m_deviceSource=0;
	m_deviceDest=0;
	m_deviceLock=0;
	m_deviceLockBase = 0;
	m_destDeviceOffset=0;

	m_force51Stereo = false;

	m_initiallySelected=false;
	m_selected = false;

	m_destType = NONE;
	m_sourceType = NONE;
	m_dynamicTallies = false;
	m_panelDynamic = "";
}

// destructor - equivalent to ApplCore CLOSEDOWN
audio_dest::~audio_dest()
{
}

// all button pushes and notifications come here
void audio_dest::buttonCallback( buttonNotify *b )
{
	destButtonEvent();
}


void audio_dest::destButtonEvent( void  )
{
	// we can't guarantee that we're working as part of a dest group so explicitly set the selected state
	m_selected = true;
	textPut( "stylesheet=dest_selected", 1, "dest" );

	hostNotify( bncs_string( "global_dest=%1" ).arg( m_globalDest ));
	hostNotify( bncs_string( "selected=%1" ).arg( m_globalDest ));

}

// all revertives come here
int audio_dest::revertiveCallback( revertiveNotify * r )
{
	r->dump("audio_dest::revertiveCallback");
	if( r->device() == m_deviceDest )
	{
		// get the name of this port from the source device database
		bncs_string srcName;
		routerName( m_deviceSource, 0, r->sInfo(), srcName );

		// make a single line of it
		srcName.replace( "|", " " );

		// stick on our single tally component (may or may not be shown...)
		textPut( "text", srcName, 1, "tally" );

		// work out the offset to our local tally cache
		int offset = r->index() - m_dest;

		debug("audio_dest::revertiveCallback Calc Offset:%1",offset);
		// in theory neither of the next two tests are relevant...but 
		//   these are used directly as offsets to arrays so lets be safe!
		if( offset < 0 )
			return 0;

		if( offset > 5 )
			return 0;

		// remember the current tally in an array
		m_tallies[ offset ] = r->sInfo();

		// write the tally to the button depending upon offset
		routerName( m_deviceSource, 2, r->sInfo(), srcName );
		srcName.replace( "|", " " );			// make a single line of it
		debug("audio_dest::revertiveCallback textput Name:%1 UI:%2",srcName,bncs_string( "tally_%1" ).arg( offset + 1 ));
		textPut( "text", srcName, 1, bncs_string( "tally_%1" ).arg( offset + 1 ) );

		routerName( m_deviceSource, 6, r->sInfo(), srcName );
		//Get first character of name
		char cPortType = srcName[0];
		portType sourceType = toPortType(cPortType, false);

		// stereo dest types can show either single or double tally depending upon whether spilt or not
		if( m_destType == STEREO )
		{
			if( sourceType == STEREO )
			{
				// make sure that tallies are consecutive or flag as "split"
				bool b = false;
				if( ( m_tallies[ 0 ] + 1) != (m_tallies[ 1 ] ))
					b = true;

				if( b )
				{
					controlHide( 1, "tally" );
					controlShow( 1, "tally_1" );
					controlShow( 1, "tally_2" );
					textPut( "stylesheet", "tally_audio_mixed", 1, "tally_1");
					textPut( "stylesheet", "tally_audio_mixed", 1, "tally_2");
					textPut( "stylesheet", "tally_audio_mixed", 1, "background");
				}
				else 
				{
					controlShow( 1, "tally" );
					controlHide( 1, "tally_1" );
					controlHide( 1, "tally_2" );
					textPut( "stylesheet", "tally_audio_normal", 1, "background");
				}
			}
			else
			{
				controlHide( 1, "tally" );
				controlShow( 1, "tally_1" );
				controlShow( 1, "tally_2" );
				textPut( "stylesheet", "tally_audio_mixed", 1, "tally_1");
				textPut( "stylesheet", "tally_audio_mixed", 1, "tally_2");
				textPut( "stylesheet", "tally_audio_mixed", 1, "background");
			}
		}
		else if( m_destType == SURROUND )
		{
			// make sure that tallies are consecutive or flag as "split"
			bool b = false;
			for( int x = 0 ; x < 5 ; x++ )
				if( ( m_tallies[ x ] + 1) != m_tallies[ x + 1 ] )
					b = true;

			for( int x = 1 ; x <= 6 ; x++ )
				textPut( "stylesheet", b?"tally_audio_mixed":"tally_audio_normal", 1, bncs_string( "tally_%1" ).arg( x ) );
			textPut( "stylesheet", b?"tally_audio_mixed":"tally_audio_normal", 1, "background" );
		}
		else if( m_destType == MONO )
		{
			if( sourceType == MONO )
				textPut( "stylesheet", "tally_audio_normal", 1, "tally_1");
			else
				textPut( "stylesheet", "tally_audio_mixed", 1, "tally_1");
		}
	}
	else if( r->device() == m_deviceLock )
	{
//		debug( "%1 %2 %3", r->device(), r->index(), r->sInfo() );

		map<int, int>::iterator it = m_lockStates.find( r->index());

		// atoi allows for unset (null entry) as well as explicitly set 0 or 1...dunno if this is a good thing or not
		if( it != m_lockStates.end() )
			it->second = atoi( r->sInfo() )?1:0;

		checkEnableState();
	}
	return 0;
}

// all database name changes come back here
void audio_dest::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string audio_dest::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		m_instance = p->value();
		getDev( m_instance, &m_deviceSource );
	}
	else if( p->command() == "lock_instance" )
	{
		m_instanceLock = p->value();

		if( m_instanceLock.length() )
			getDev( m_instanceLock, &m_deviceLockBase );
		else
			m_deviceLockBase=0;

		if( m_deviceLockBase )
			m_deviceLock = m_deviceLockBase + m_destDeviceOffset;
		else
			m_deviceLock = 0;
	}
	else if( p->command() == "force_51_stereo" )
	{
		m_force51Stereo = (p->value() == "true" );
	}
	else if( p->command() == "global_dest" )
	{

		if( m_deviceDest )
			infoUnregister( m_deviceDest );
		m_deviceDest=0;
		
		if( m_deviceLock )
			infoUnregister( m_deviceLock );
		m_deviceLock=0;

		if( p->value().length() )
		{
			m_globalDest = p->value();
			
			m_destDeviceOffset = (m_globalDest / 4096);
			m_dest = m_globalDest % 4096;
			
			m_deviceDest = m_deviceSource + m_destDeviceOffset;
			m_deviceLock = m_deviceLockBase + m_destDeviceOffset;
		}
		else
		{
			m_deviceDest = 0;
			m_deviceLock = 0;
			m_globalDest = 0;
			
			m_destDeviceOffset = 0;
			m_dest = -1;
		}

		// with dest index work out possible take modes
		bncs_string ret;
		routerName( m_deviceSource, 7, m_globalDest, ret );

		//Get first character of name
		char cPortType = ret[0];
		m_destType = toPortType(cPortType, true, false);


		if(m_dynamicTallies == true)
		{
			bncs_string panel = "";
			switch( m_destType )
			{
				case NONE:
					panel = m_panel;
					break;
				case MONO:
					panel = "M_V_w135";
					break;
				case STEREO:
					panel = "LR_V_w135";
					break;
				case SURROUND:
					panel = "51_V";
					break;
			}
			if(m_panelDynamic != panel)
			{
				debug( "audio_dest 1: panel loaded source:%1  dest:%2 Panel:%3", m_source, m_dest,m_panelDynamic );
				m_panelDynamic = panel;
				panelDestroy(1);
				setSize(0,0);
				panelShow(1,m_panelDynamic + ".bncs_ui" );
				debug( "audio_dest 2: panel loaded source:%1  dest:%2 Panel:%3", m_source, m_dest,m_panelDynamic );
				setSize(1);
			}
				
		}

		textPut( "text=", 1, "tally" );
		for( int x = 1 ; x <= 6 ; x++ )
			textPut( "text=", 1, bncs_string( "tally_%1").arg( x ) );

//		debug( "base device/actual lock base/actual %1 %2 %3 %4 - global/local %5 %6", m_deviceSource, m_deviceDest, m_deviceLockBase, m_deviceLock, m_globalDest, m_dest );
		
		if( !m_alias.length() )
		{
			bncs_string name;
			routerName( m_deviceSource, 1, m_globalDest, name );

			if( name == "---" )
				textPut( "text=", 1, "dest" );
			else
				textPut( "text", name, 1, "dest" );
		}

		m_lockStates.clear();

		switch( m_destType )
		{
			case NONE:
				debug( "audio_dest: Device %1 index %2 (global %3) has unknown type", m_deviceSource, m_dest, m_globalDest );
				controlDisable( 1, "dest" );
				controlDisable( 1, "tally" );
				controlShow( 1, "tally" );
				controlHide( 1, "tally_1" );
				controlHide( 1, "tally_2" );
				controlShow( 1, "background" );
				textPut( "stylesheet", "groupbox", 1, "background");
//				panelShow( "error", "configError.bncs_ui" );
				break;

			case STEREO:
				controlEnable( 1, "dest" );
				controlEnable( 1, "tally" );
				controlHide( 1, "tally" );
				controlShow( 1, "tally_1" );
				controlShow( 1, "tally_2" );
				controlShow( 1, "background" );
				infoRegister( m_deviceDest, m_dest, m_dest + 1 );

				// this slightly curious method of polling is deliberate (it kicks in shim poll/retry/aggregation
				infoPoll( m_deviceDest, m_dest, m_dest);
				infoPoll( m_deviceDest, m_dest + 1, m_dest + 1 );

				if( m_deviceLock )
				{
					m_lockStates.insert( make_pair( m_dest, -1 ));
					m_lockStates.insert( make_pair( m_dest + 1, -1 ));

					infoRegister( m_deviceLock, m_dest, m_dest + 1 );
					infoPoll( m_deviceLock, m_dest, m_dest );
					infoPoll( m_deviceLock, m_dest + 1, m_dest + 1 );
				}
				break;

			case MONO:
				controlEnable( 1, "dest" );
				controlEnable( 1, "tally" );
				controlHide( 1, "tally" );
				controlShow( 1, "tally_1" );
				controlHide( 1, "tally_2" );
				controlShow( 1, "background" );
				textPut( "stylesheet", "groupbox", 1, "background");

				infoRegister( m_deviceDest, m_dest, m_dest );
				infoPoll( m_deviceDest, m_dest, m_dest );

				if( m_deviceLock )
				{
					m_lockStates.insert( make_pair( m_dest, -1 ));

					infoRegister( m_deviceLock, m_dest, m_dest );
					infoPoll( m_deviceLock, m_dest, m_dest );
				}
				break;

			case SURROUND:
				controlEnable( 1, "dest" );
				controlEnable( 1, "tally" );
				controlShow( 1, "tally_1" );
				controlShow( 1, "tally_2" );
				controlShow( 1, "background" );

				infoRegister( m_deviceDest, m_dest, m_dest + 5 );
				for( int x = m_dest ; x < m_dest + 6 ; x++ )
					infoPoll( m_deviceDest, x, x );

				if( m_deviceLock )
				{
					for( int x = m_dest ; x < m_dest + 6 ; x++ )
						m_lockStates.insert( make_pair( x, -1 ));

					infoRegister( m_deviceLock, m_dest, m_dest + 5 );
					infoPoll( m_deviceLock, m_dest, m_dest + 5 );
				}
				break;
		}

		if( m_selected )
			hostNotify( bncs_string( "global_dest=%1" ).arg( m_globalDest ));
	}
	else if( p->command() == "undo" )
	{
		while( m_undo.size() )
		{
			DWORD i = m_undo.front();
			m_undo.pop();

			infoWrite( m_deviceDest, HIWORD( i ), LOWORD( i ));
		}
	}
	else if( p->command() == "take" )
	{
		clearUndoQueue();

		if( !m_selected )
			return"";

		if( !strlen( p->value()))
			m_source = -1;
		else
			m_source = atoi( p->value() );

		bncs_string ret;
		routerName( m_deviceSource, 6, m_source, ret );
		
		//Get first character of name
		char cPortType = ret[0];
		m_sourceType = toPortType(cPortType);

		// if there is a dest supplied then use it and check it....otherwise gloss over!!!
		int dest;
		if( strlen( p->sub(0)))
		{
			dest= atoi( p->sub(0) );

			if( dest != m_globalDest )
				return"";
		}

		// no valid source/destination
		if( m_source < 0 )// && (dest < 0 ))
			return "";

		bncs_string mask = p->sub( 1 );

		switch( m_destType )
		{
			case MONO:
				if( !mask.length() )
					mask = "M-M";
				if( m_sourceType == MONO )
				{
					if( mask == "M-M" )
						doRoute(  m_source, m_dest );
					else
						debug( "audio_dest: can't route MONO source %1 to MONO dest %2 with mask " + mask, m_source, m_dest );
				}
				else if( m_sourceType == STEREO )
				{
					if( mask == "L-M" )
						doRoute(  m_source, m_dest );
					else if( mask == "R-M" )
						doRoute(  m_source + 1, m_dest );
					else
						debug( "audio_dest: can't route STEREO source %1 to MONO dest %2 with mask " + mask, m_source, m_dest );
				}
				else if( m_sourceType == SURROUND )
				{
					if( mask == "FL-M" )
						doRoute(  m_source, m_dest );
					else if( mask == "FR-M" )
						doRoute(  m_source + 1, m_dest );
					else if( mask == "C-M" )
						doRoute(  m_source + 2, m_dest );
					else if( mask == "E-M" )
						doRoute(  m_source + 3, m_dest );
					else if( mask == "LS-M" )
						doRoute(  m_source + 4, m_dest );
					else if( mask == "RS-M" )
						doRoute(  m_source + 5, m_dest );
					else
						debug( "audio_dest: can't route SURROUND source %1 to MONO dest %2 with mask " + mask, m_source, m_dest );
				}
				break;

			case STEREO:
				if( !mask.length() )
					mask = "LR-LR";
				if( m_sourceType == STEREO )
				{
					if( mask == "LR-LR" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source + 1, m_dest + 1 );
					}
					else if( mask == "L-LR" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source, m_dest + 1 );
					}
					else if( mask == "R-LR" )
					{
						doRoute(  m_source + 1, m_dest );
						doRoute(  m_source + 1, m_dest + 1 );
					}
					else if( mask == "L-L" )
					{
						doRoute(  m_source, m_dest );
					}
					else if( mask == "R-R" )
					{
						doRoute(  m_source + 1, m_dest + 1 );
					}
					else if( mask == "L-R" )
					{
						doRoute(  m_source, m_dest + 1 );
					}
					else if( mask == "R-L" )
					{
						doRoute(  m_source + 1, m_dest );
					}
					else
						debug( "audio_dest: can't route STEREO source %1 to STEREO dest %2 with mask " + mask, m_source, m_dest );
				}
				else if( m_sourceType == MONO )
				{
					if( mask == "M-LR" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source, m_dest + 1 );
					}
					else if( mask == "M-L" )
					{
						doRoute(  m_source, m_dest );
					}
					else if( mask == "M-R" )
					{
						doRoute(  m_source, m_dest + 1 );
					}
					else
						debug( "audio_dest: can't route MONO source %1 to STEREO dest %2 with mask " + mask, m_source, m_dest );
				}
				else if( m_sourceType == SURROUND )
				{
					if( mask == "FLFR-LR" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source + 1, m_dest + 1);
					}
					else if( mask == "FL-LR" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source, m_dest + 1);
					}
					else if( mask == "CE-LR" )
					{
						doRoute(  m_source + 2, m_dest );
						doRoute(  m_source + 3, m_dest + 1);
					}
					else if( mask == "LSRS-LR" )
					{
						doRoute(  m_source + 4, m_dest );
						doRoute(  m_source + 5, m_dest + 1);
					}
					else if( mask == "FLFR-RL" )
					{
						doRoute(  m_source, m_dest + 1 );
						doRoute(  m_source + 1, m_dest );
					}
					else if( mask == "FR-LR" )
					{
						doRoute(  m_source + 1, m_dest );
						doRoute(  m_source + 1, m_dest + 1);
					}
					else if( mask == "C-LR" )
					{
						doRoute(  m_source + 2, m_dest );
						doRoute(  m_source + 2, m_dest + 1);
					}
					else
						debug( "audio_dest: can't route SURROUND source %1 to STEREO dest %2 with mask " + mask, m_source, m_dest );
				}
				break;

			case SURROUND:
				if( !mask.length() )
					mask = "51-51";
				if( m_sourceType == MONO )
				{
					if( mask == "M-FL" )
						doRoute(  m_source, m_dest );
					else if( mask == "M-FR" )
						doRoute(  m_source, m_dest + 1 );
					else if( mask == "M-C" )
						doRoute(  m_source, m_dest + 2 );
					else if( mask == "M-E" )
						doRoute(  m_source, m_dest + 3 );
					else if( mask == "M-LS" )
						doRoute(  m_source, m_dest + 4 );
					else if( mask == "M-RS" )
						doRoute(  m_source, m_dest + 5 );
					else
						debug( "audio_dest: can't route MONO source %1 to SURROUND dest %2 with mask " + mask, m_source, m_dest );
				}
				else if( m_sourceType == SURROUND )
				{
					if( mask == "51-51" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source + 1, m_dest + 1 );
						doRoute(  m_source + 2, m_dest + 2 );
						doRoute(  m_source + 3, m_dest + 3 );
						doRoute(  m_source + 4, m_dest + 4 );
						doRoute(  m_source + 5, m_dest + 5 );
					}
					else if( mask == "FL-FLFR" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source, m_dest + 1 );
					}
					else if( mask == "FL-C" )
					{
						doRoute(  m_source, m_dest + 2 );
					}
					else if( mask == "FLFR-FLFR" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source + 1, m_dest + 1 );
					}
					else if( mask == "FLFR-CE" )
					{
						doRoute(  m_source, m_dest + 2);
						doRoute(  m_source + 1, m_dest + 3 );
					}
					else if( mask == "FLFR-LSRS" )
					{
						doRoute(  m_source, m_dest + 4 );
						doRoute(  m_source + 1, m_dest + 5 );
					}
					else if( mask == "FLFR-FRFL" )
					{
						doRoute(  m_source, m_dest + 1 );
						doRoute(  m_source + 1, m_dest );
					}
					else if( mask == "FR-FLFR" )
					{
						doRoute(  m_source + 1, m_dest );
						doRoute(  m_source + 1, m_dest + 1 );
					}
					else if( mask == "FR-C" )
					{
						doRoute(  m_source + 1, m_dest + 2 );
					}
					else if( mask == "CE-FLFR" )
					{
						doRoute(  m_source + 2, m_dest );
						doRoute(  m_source + 3, m_dest + 1 );
					}
					else if( mask == "CE-CE" )
					{
						doRoute(  m_source + 2, m_dest + 2 );
						doRoute(  m_source + 3, m_dest + 3 );
					}
					else if( mask == "CE-LSRS" )
					{
						doRoute(  m_source + 2, m_dest + 4 );
						doRoute(  m_source + 3, m_dest + 5 );
					}
					else if( mask == "C-FLFR" )
					{
						doRoute(  m_source + 2, m_dest );
						doRoute(  m_source + 2, m_dest + 1 );
					}
					else if( mask == "C-C" )
					{
						doRoute(  m_source + 2, m_dest + 2 );
					}
					else if( mask == "LSRS-FLFR" )
					{
						doRoute(  m_source + 4, m_dest );
						doRoute(  m_source + 5, m_dest + 1 );
					}
					else if( mask == "LSRS-CE" )
					{
						doRoute(  m_source + 4, m_dest + 2);
						doRoute(  m_source + 5, m_dest + 3 );
					}
					else if( mask == "LSRS-LSRS" )
					{
						doRoute(  m_source + 4, m_dest + 4 );
						doRoute(  m_source + 5, m_dest + 5 );
					}
					else if( mask == "FLFRLSRS-FLFRLSRS" )
					{
						doRoute(  m_source , m_dest );
						doRoute(  m_source + 1, m_dest + 1 );
						doRoute(  m_source + 4, m_dest + 4 );
						doRoute(  m_source + 5, m_dest + 5 );
					}
					else
						debug( "audio_dest: can't route SURROUND source %1 to SURROUND dest %2 with mask " + mask, m_source, m_dest );
				}
				else if( m_sourceType == STEREO )
				{
					if( mask == "LR-FLFR" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source + 1, m_dest + 1 );
					}
					else if( mask == "LR-CE" )
					{
						doRoute(  m_source, m_dest + 2);
						doRoute(  m_source + 1, m_dest + 3 );
					}
					else if( mask == "L-FLFR" )
					{
						doRoute(  m_source, m_dest );
						doRoute(  m_source, m_dest + 1 );
					}
					else if( mask == "L-C" )
					{
						doRoute(  m_source, m_dest + 2);
					}
					else if( mask == "LR-FRFL" )
					{
						doRoute(  m_source, m_dest + 1 );
						doRoute(  m_source + 1, m_dest );
					}
					else if( mask == "LR-LSRS" )
					{
						doRoute(  m_source, m_dest + 4 );
						doRoute(  m_source + 1, m_dest + 5 );
					}
					else if( mask == "R-FLFR" )
					{
						doRoute(  m_source + 1, m_dest );
						doRoute(  m_source + 1, m_dest + 1 );
					}
					else if( mask == "R-C" )
					{
						doRoute(  m_source + 1, m_dest + 2);
					}
					else
						debug( "audio_dest: can't route STEREO source %1 to SURROUND dest %2 with mask " + mask, m_source, m_dest );
				}
				break;
		}
	}
	else if( p->command() == "initially_selected" )
	{
		if( p->value() == "true" )
		{
			timerStart( 1, 1 );
			m_initiallySelected = true;
		}
		else
			m_initiallySelected = false;
	}
	else if ( p->command() == "alias" )
	{
		m_alias = p->value();

		textPut( "text", m_alias, 1, "dest" );
	}
	else if ( p->command() == "panel" )
	{
		m_panel = p->value();

		panelDestroy( 1 );
		panelShow( 1, m_panel + ".bncs_ui" );
		m_panelDynamic = m_panel;
		setSize( 1 );
	}
	else if( p->command() == "selected" )
	{
		clearUndoQueue();
		// is this us?
		if( m_globalDest == p->value())
		{
			// if we're not flagged as selected then it was someone other than us that did that selection
			//  we still need to show ourselves as selected
			if( !m_selected )
			{
				m_selected = true;
				textPut( "stylesheet=dest_selected", 1, "dest" );
			}
		}
		else
		{ 
			// quick check of our current state saves any unncessary flickering
			if( m_selected )
			{
				m_selected = false;
				textPut( "stylesheet=dest_deselected", 1, "dest" );
			}
		}
	}
	else if( p->command() == "defaultSourceList" )
	{
		m_defaultSourceList = bncs_stringlist( p->value() );
	}
	else if( p->command() == "default" )
	{
		switch( m_destType )
		{
			case MONO:
				if( m_defaultSourceList.count() == 1 )
					doRoute( m_defaultSourceList[ 0 ], m_dest );
				break;
			case STEREO:
				if( m_defaultSourceList.count() == 2 )
				{
					for( int x = 0 ; x < 2 ; x++ )
						doRoute( m_defaultSourceList[ x ], m_dest + x );
				}
				break;
			case SURROUND:
				if( m_defaultSourceList.count() == 6 )
				{
					for( int x = 0 ; x < 6 ; x++ )
						doRoute( m_defaultSourceList[ x ], m_dest + x );
				}
				break;
		}
	}
	else if(p->command() == "dynamic_tallies")
	{
		if(p->value().lower() == "true")
		{
			m_dynamicTallies = true;
		}
		else
		{
			m_dynamicTallies = false;
		}
	}
	else if ( p->command() == "read_only")
	{
		if (p->value().lower() == "true")
		{
			m_blnReadOnly= true;
			textPut("style", "label", 1, "dest");
			textPut("notify.released", "false", 1, "dest");
		}
		else
		{
			m_blnReadOnly= false;
			textPut("style", "button", 1, "dest");
			textPut("notify.released", "true", 1, "dest");
		}
	}
	else if( p->command() == "return" )
	{
		bncs_stringlist sl;

		sl << "panel=" + m_panel;
		sl << "alias=" + m_alias;
		sl << "lock_instance=" + m_instanceLock;
		sl << "initially_selected=" + bncs_string(m_initiallySelected?"true":"false");
		sl << "force_51_stereo=" + bncs_string( m_force51Stereo?"true":"false" );
		sl << "global_dest=" + bncs_string( "%1" ).arg( m_globalDest );
		sl << "dynamic_tallies=" + bncs_string("%1").arg(m_dynamicTallies==true?"true":"false");
		sl << "defaultSourceList=" + m_defaultSourceList.toString( ',' );
		sl << "read_only=" + bncs_string("%1").arg(m_blnReadOnly==true?"true":"false");

		return sl.toString( '\n' );
	}
	else if( p->command() == "_events" )
	{
		bncs_stringlist sl;

		sl << "global_dest=<dest index>";

		return sl.toString( '\n' );
	}
	else if( p->command() == "_commands" )
	{
		bncs_stringlist sl;

		sl << "global_dest=<dest index>";
		sl << "take.<global dest index>.<mode>=<source index>";
		sl << "selected=<dest index>";

		return sl.toString( '\n' );
	}
	return "";
}

// timer events come here
void audio_dest::timerCallback( int id )
{
	timerStop( 1 );

	destButtonEvent();
}


void audio_dest::checkEnableState( void )
{
	if( m_dest < 1 )
	{
		debug( "no dest set - disabling button" );
		controlDisable( 1, "dest" );
		return;
	}

	for( map< int, int >::iterator it = m_lockStates.begin() ; it != m_lockStates.end() ; ++it )
	{
		debug( "checking....%1 %2", it->first, it->second );
		if( it->second )		// this catches both set states (1) and no current tally (-1)
		{
			controlDisable( 1, "dest" );
			return;
		}
	}

	// got this far....must be that we're enabled then!
	controlEnable( 1, "dest" );
}

void audio_dest::doRoute( int source, int dest )
{
	int offset = dest - m_dest;

	m_undo.push( MAKELONG( dest, m_tallies[ offset ] ));

	infoWrite( m_deviceDest, source, dest );
}

void audio_dest::clearUndoQueue( void )
{
	// I didn't realise that queue doesn't have a clear function....no matter easily done really....
	while( m_undo.size() )
		m_undo.pop();
}
