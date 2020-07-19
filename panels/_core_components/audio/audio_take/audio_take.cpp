#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "audio_take.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( audio_take )

// constructor - equivalent to ApplCore STARTUP
audio_take::audio_take( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_sourceType = NONE;
	m_destType = NONE;
	
	m_device = 0;
	m_deviceDest = 0;
	m_deviceLock = 0;
	m_deviceLockBase = 0;
	m_destDeviceOffset=0;
	m_dest = -1;
	m_source = -1;
	m_globalDest=-1;
	m_state = false;
	m_undoPossible = false;
	m_force51Stereo = false;

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "p1.bncs_ui" );
	setSize( 1 );

	// start with everything disabled - we can't do anything without a few more runtime parameters
	controlDisable( 1, "take" );
	controlDisable( 1, "undo" );
	controlDisable( 1, "takeSpecial" );
}

// all button pushes and notifications come here
void audio_take::buttonCallback( buttonNotify *b )
{
	switch( b->panel() )
	{
		case 1:
			if( b->id() == "take" )
			{
				hostNotify( bncs_string( "take.%1.%2=%3" ).arg( m_globalDest ).arg( m_mask ).arg( m_source ) );
				m_undoPossible = true;
				checkEnableState();
			}
			else if( b->id() == "undo" )
			{
				hostNotify( b->id() );
				m_undoPossible = false;
				checkEnableState();
			}
			else if( b->id() == "takeSpecial" )
			{
				panelDestroy( 2 );

				switch( MAKELONG( m_sourceType, m_destType ))
				{
					case MAKELONG( MONO, STEREO ):
						panelPopup( 2, "M-LR.bncs_ui" );
						break;

					case MAKELONG( MONO, SURROUND ):
						panelPopup( 2, "M-51.bncs_ui" );
						break;

					case MAKELONG( STEREO, MONO ):
						panelPopup( 2, "LR-M.bncs_ui" );
						break;

					case MAKELONG( STEREO, STEREO ):
						panelPopup( 2, "LR-LR.bncs_ui" );
						break;

					case MAKELONG( STEREO, SURROUND ):
						panelPopup( 2, "LR-51.bncs_ui" );
						break;

					case MAKELONG( SURROUND, MONO ):
						panelPopup( 2, "51-M.bncs_ui" );
						break;

					case MAKELONG( SURROUND, STEREO ):
						panelPopup( 2, "51-LR.bncs_ui" );
						break;

					case MAKELONG( SURROUND, SURROUND ):
						panelPopup( 2, "51-51.bncs_ui" );
						break;
				}
			}
			break;

		// the only thing notifying us off panel two is the audio take mode buttons
		// the id of which is the value to be passed as the mask parameter
		case 2:
		{
			bncs_string ret;
			textGet( "text", 2, b->id(), ret );

			// a very quick and dirty sanity check of the incoming id....this'll always have a dash in it
			if( b->id().contains( '-' ))
			{
				hostNotify( bncs_string( "take.%1.%2=%3" ).arg( m_globalDest ).arg( b->id() ).arg( m_source ) );
				m_undoPossible = true;
				checkEnableState();
			}

			panelDestroy( 2 );
			break;
		}
	}
}

// all revertives come here
int audio_take::revertiveCallback( revertiveNotify * r )
{
	if( r->device() == m_deviceLock )
	{
		map<int, int>::iterator it = m_lockStates.find( r->index());

		// atoi allows for unset (null entry) as well as explicitly set 0 or 1...dunno if this is a good thing or not
		if( it != m_lockStates.end() )
			it->second = atoi( r->sInfo() )?1:0;

		checkEnableState();
	}
	return 0;
}

// all database name changes come back here
void audio_take::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string audio_take::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		m_instanceBase = p->value();
		getDev( m_instanceBase, &m_device );
	}
	else if( p->command() == "lock_instance" )
	{
		m_instanceLockBase = p->value();
		getDev( m_instanceLockBase, &m_deviceLockBase );
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

			m_deviceDest = m_device + m_destDeviceOffset;
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

		bncs_string name;
		routerName( m_deviceDest, 1, m_dest, name );

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

		// with dest index work out possible take modes
		bncs_string ret;
		routerName( m_deviceDest, 7, m_dest, ret );

		m_destType = toPortType( ret[ 0 ], true, m_force51Stereo );

		m_undoPossible = false;
		
		setDefaultMask();

		m_lockStates.clear();

		// only do this if locking is configured.....
		if( m_deviceLock )
		{
			switch( m_destType )
			{
				case NONE:
					break;

				case STEREO:
					m_lockStates.insert( make_pair( m_dest, -1 ));
					m_lockStates.insert( make_pair( m_dest + 1, -1 ));

					infoRegister( m_deviceLock, m_dest, m_dest + 1 );
					infoPoll( m_deviceLock, m_dest, m_dest );
					infoPoll( m_deviceLock, m_dest + 1, m_dest + 1 );
					break;

				case MONO:
					m_lockStates.insert( make_pair( m_dest, -1 ));

					infoRegister( m_deviceLock, m_dest, m_dest );
					infoPoll( m_deviceLock, m_dest, m_dest );
					break;

				case SURROUND:
					infoRegister( m_deviceLock, m_dest, m_dest + 5 );

					for( int x = m_dest ; x < m_dest + 6 ; x++ )
					{
						m_lockStates.insert( make_pair( x, -1 ));
						infoPoll( m_deviceLock, x, x );
					}

					break;
			}
		}
		// do this after checking lock....this should disable whilst we wait for lock revertives
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

		bncs_string ret;
		routerName( m_device, 6, m_source, ret );
		m_sourceType = toPortType( ret[ 0 ] );

		setDefaultMask();

		checkEnableState();
	}
	else if( p->command() == "force_51_stereo" )
	{
		m_force51Stereo = (p->value() == "true" );
	}
	else if( p->command() == "return" )
	{
		bncs_stringlist sl;

		sl << "lock_instance=" + m_instanceLockBase;
		sl << bncs_string( "force_51_stereo=" ) + (m_force51Stereo?"true":"false");

		return sl.toString( '\n' );
	}
	else if( p->command() == "_events" )
	{
		bncs_stringlist sl;

		sl << "take.<global_dest_index>.<how>=<source>";
		sl << "undo";

		return sl.toString( '\n' );
	}
	else if( p->command() == "_commands" )
	{
		bncs_stringlist sl;

		sl << "global_dest=<dest_index>";
		sl << "source=<source_index>";

		return sl.toString( '\n' );
	}
	return "";
}

// timer events come here
void audio_take::timerCallback( int id )
{
}
void audio_take::checkEnableState( void )
{
	if( m_source < 1 )
	{
		controlDisable( 1, "take" );
		controlDisable( 1, "undo" );
		controlDisable( 1, "takeSpecial" );
		debug( "no valid source" );
		return;
	}
	if( m_globalDest < 1 )
	{
		controlDisable( 1, "take" );
		controlDisable( 1, "undo" );
		controlDisable( 1, "takeSpecial" );
		debug( "no valid dest" );
		return;
	}
	for( map< int, int >::iterator it = m_lockStates.begin() ; it != m_lockStates.end() ; ++it )
		if( it->second )		// this catches both set states (1) and no current tally (-1)
		{
			controlDisable( 1, "take" );
			controlDisable( 1, "undo" );
			controlDisable( 1, "takeSpecial" );
			debug( "locked %1", it->first );
			return;
		}

	if( m_mask.length() )
		controlEnable( 1, "take" );

	if(	m_undoPossible )
		controlEnable( 1, "undo" );
	else
		controlDisable( 1, "undo" );


	switch( MAKELONG( m_sourceType, m_destType ))
	{
		case MAKELONG( MONO, STEREO ):
		case MAKELONG( MONO, SURROUND ):
		case MAKELONG( STEREO, MONO ):
		case MAKELONG( STEREO, STEREO ):
		case MAKELONG( STEREO, SURROUND ):
		case MAKELONG( SURROUND, MONO ):
		case MAKELONG( SURROUND, STEREO ):
		case MAKELONG( SURROUND, SURROUND ):
			controlEnable( 1, "takeSpecial" );
			break;

		default:
			controlDisable( 1, "takeSpecial" );
			break;
	}
}

void audio_take::setDefaultMask( void )
{
	switch( MAKELONG( m_sourceType, m_destType ))
	{
		case MAKELONG( MONO, MONO ):
			m_mask = "M-M";
			textPut( "pixmap", "audio_take_mode_" + m_mask + ".png", 1, "take" );
			textPut( "text", "Take " + m_mask, 1, "take" );
			break;

		case MAKELONG( MONO, STEREO ):
			m_mask = "M-LR";
			textPut( "pixmap", "audio_take_mode_" + m_mask + ".png", 1, "take" );
			textPut( "text", "Take " + m_mask, 1, "take" );
			break;


		case MAKELONG( STEREO, STEREO ):
			m_mask = "LR-LR";
			textPut( "pixmap", "audio_take_mode_" + m_mask + ".png", 1, "take" );
			textPut( "text", "Take " + m_mask, 1, "take" );
			break;

		case MAKELONG( SURROUND, SURROUND ):
			m_mask = "51-51";
			textPut( "pixmap", "audio_take_mode_" + m_mask + ".png", 1, "take" );
			textPut( "text", "Take " + m_mask, 1, "take" );
			break;
		

/*		case MAKELONG( MONO, SURROUND ):
		case MAKELONG( STEREO, MONO ):
		case MAKELONG( STEREO, SURROUND ):
		case MAKELONG( SURROUND, MONO ):
		case MAKELONG( SURROUND, STEREO ):
*/		default:
			controlDisable( 1, "take" );
			m_mask = "";
			textPut( "pixmap=", 1, "take" );
			textPut( "text=Take", 1, "take" );
			break;
	}
}


/* 
[Database_7]
0001=L
0021=M
0025=1
*/

