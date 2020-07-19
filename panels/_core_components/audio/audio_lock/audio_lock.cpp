#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "audio_lock.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( audio_lock )

#define B_STATE_DISABLE 0
#define B_STATE_INIT 1
#define B_STATE_UNLOCKED 2
#define B_STATE_LOCKED 3

#define PANEL_MAIN     1


// constructor - equivalent to ApplCore STARTUP
audio_lock::audio_lock( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_device = 0;
	m_deviceLock = 0;
	m_index =0;
	m_globalIndex =0;
	m_instance = "";
	m_instanceLock = "";
	m_deviceAudioBase = 0;
	m_deviceLockBase = 0;
	m_destType = NONE;
	m_lockStates.clear();
	m_destDeviceOffset = 0;
	setLockState(B_STATE_DISABLE);

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PANEL_MAIN, "main.bncs_ui" );
	controlDisable( PANEL_MAIN, "button" );
}

// destructor - equivalent to ApplCore CLOSEDOWN
audio_lock::~audio_lock()
{ 
}
 
// all button pushes and notifications come here
void audio_lock::buttonCallback( buttonNotify *b )
{
	if( m_deviceLock && (m_index > 0 ))
	{
		if( m_state )
		{
			for( map< int, int >::iterator it = m_lockStates.begin() ; it != m_lockStates.end() ; ++it )
			{
				infoWrite( m_deviceLock, "0", it->first );
			}
		}
		else
		{
			for( map< int, int >::iterator it = m_lockStates.begin() ; it != m_lockStates.end() ; ++it )
			{
				infoWrite( m_deviceLock, "1", it->first );
			}
		}
	}

}

// all revertives come here
int audio_lock::revertiveCallback( revertiveNotify * r )
{
	if(r->device() == m_deviceLock )
	{
				debug( "audio_lock::revertiveCallback %1 %2 %3", r->device(), r->index(), r->sInfo() );
		
		map<int, int>::iterator it = m_lockStates.find( r->index());
		
		// atoi allows for unset (null entry) as well as explicitly set 0 or 1...dunno if this is a good thing or not
		if( it != m_lockStates.end() )
			it->second = atoi( r->sInfo() )?1:0;
		
		checkEnableState();
	}

	return 0;
}
 
// all database name changes come back here
void audio_lock::databaseCallback( revertiveNotify * r )
{
} 


// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string audio_lock::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		//Instance name needed to find the type of audio
		m_instance = p->value();
		if( m_instance.length() )
			getDev( m_instance, &m_deviceAudioBase );
		else
			m_deviceLockBase=0;
		
	}
	else if(p->command() =="lock_instance")
	{
		m_instanceLock = p->value();
		
		if( m_instanceLock.length() )
			getDev( m_instanceLock, &m_deviceLockBase );
		else
			m_deviceLockBase=0;

	}
	else if( p->command() == "index" )
	{
		if( p->value().length() )
		{
			m_globalIndex = p->value();
			
			m_destDeviceOffset = (m_globalIndex / 4096);
			m_index = m_globalIndex % 4096;
			
			m_deviceLock = m_deviceLockBase + m_destDeviceOffset;
			m_device = m_deviceAudioBase + m_destDeviceOffset;
		}
		debug("audio_lock::parentCallback m_deviceLock(%1) m_index(%2) m_device(%3) ",m_deviceLock,m_index,m_device);
		bncs_string ret;
		routerName( m_device, 7, m_index, ret );

		m_destType = toPortType( ret[ 0 ], true, false );
		
		m_lockStates.clear();
		
		switch( m_destType )
		{
		case NONE:
			setLockState(B_STATE_DISABLE);
			break;
			
		case STEREO:
			if( m_deviceLock )
			{
				m_lockStates.insert( make_pair( m_index, -1 ));
				m_lockStates.insert( make_pair( m_index + 1, -1 ));
				
				infoRegister( m_deviceLock, m_index, m_index + 1 );
				infoPoll( m_deviceLock, m_index, m_index );
				infoPoll( m_deviceLock, m_index + 1, m_index + 1 );
			}
			setLockState(B_STATE_INIT);
			break;
			
		case MONO:
			if( m_deviceLock )
			{
				m_lockStates.insert( make_pair( m_index, -1 ));
				
				infoRegister( m_deviceLock, m_index, m_index );
				infoPoll( m_deviceLock, m_index, m_index );
			}
			setLockState(B_STATE_INIT);
			break;
			
		case SURROUND:
			if( m_deviceLock )
			{
				for( int x = m_index ; x < m_index + 6 ; x++ )
					m_lockStates.insert( make_pair( x, -1 ));
				
				infoRegister( m_deviceLock, m_index, m_index + 5 );
				infoPoll( m_deviceLock, m_index, m_index + 5 );
			}
			setLockState(B_STATE_INIT);
			break;
		}
	}
	else if(p->command() == "return")
	{
		bncs_stringlist sl;
		sl << bncs_string("lock_instance=%1").arg(m_instanceLock);
		sl << bncs_string("index=%1").arg(m_index);

		return sl.toString('\n');
	}
	return "";
}

// timer events come here
void audio_lock::timerCallback( int id )
{
}

void audio_lock::setLockState(int buttonState)
{
	switch( buttonState )
	{
	case B_STATE_DISABLE:
		controlDisable( PANEL_MAIN, "button" );
		break;
	case B_STATE_INIT:
		controlEnable( PANEL_MAIN, "button" );
		// put a value to the button just to indicate this is the unknown state
		textPut( "statesheet", "enum_unknown", PANEL_MAIN, "button" );
		break;
	case B_STATE_UNLOCKED:
		controlEnable( 1, 1 );
		textPut( "statesheet", "dest_unlocked", PANEL_MAIN, "button" );
		m_state = 0;
		break;
	case B_STATE_LOCKED:
		controlEnable( PANEL_MAIN, "button" );
		textPut( "statesheet", "dest_locked", PANEL_MAIN, "button" );
		m_state = 1;
		break;
	}
}

void audio_lock::checkEnableState( void )
{
	if( m_index < 1 )
	{
		debug( "no dest set - disabling button" );
		setLockState(B_STATE_DISABLE);
		return;
	}
	bool bLocked = false;
	for( map< int, int >::iterator it = m_lockStates.begin() ; it != m_lockStates.end() ; ++it )
	{
				debug( "checking....%1 %2", it->first, it->second );
		if( it->second )		// this catches both set states (1) and no current tally (-1)
		{
			bLocked = true;
		}
	}
	
	if(bLocked)
	{
		setLockState(B_STATE_LOCKED);
	}
	else
	{
		setLockState(B_STATE_UNLOCKED);
	}
}