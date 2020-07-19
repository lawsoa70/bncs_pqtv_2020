#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "lock.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( lock )

// constructor - equivalent to ApplCore STARTUP
lock::lock( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_device = 0;
	m_index.clear();
	m_states.clear();

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "p1.bncs_ui" );
	controlDisable( 1, 1 );
}

// destructor - equivalent to ApplCore CLOSEDOWN
lock::~lock()
{ 
}
 
// all button pushes and notifications come here
void lock::buttonCallback( buttonNotify *b )
{
	if( m_device && (m_index.count() > 0 ))
	{
		bool isLocked = (m_states.find("1") != -1);	// if we can't find any unlocked or unknown items they must be all locked;
		for (bncs_stringlistIterator slIt = m_index.begin(); slIt != m_index.end(); slIt++)
			infoWrite(m_device, isLocked ? "0" : "1", slIt->toInt());
	}
}

// all revertives come here
int lock::revertiveCallback( revertiveNotify * r )
{
	int intIndex = m_index.find(bncs_string(r->index()));
	if(( intIndex != -1 ) && (r->device() == m_device ))
	{	// our revertive is one of our indecies

		debug("lock::revertiveCallback: found index<%1> in indicies<%2>, states current<%3>, count<%4>, revertive<%5>", intIndex, m_index.toString(), m_states.toString(),m_states.count(),r->sInfo().firstInt());
		m_states.replaceItem(r->sInfo().firstInt(),intIndex);
		debug("lock::revertiveCallback: states updated to<%1>", m_states.toString());

		// iterate through to find overall state
		bool isLocked = true;
		bool isUnlocked = true;
		for (bncs_stringlistIterator slIt = m_states.begin(); slIt != m_states.end(); slIt++)
		{
			if (slIt->firstInt() == 1)
				isUnlocked = false;
			if (slIt->firstInt() == 0)
				isLocked = false;
		}
		if (isLocked && !isUnlocked)
		{
			m_state = 1;	// locked
			textPut("statesheet", "dest_locked", 1, 1);
		}
		else if (!isLocked && isUnlocked)
		{
			m_state = 0;	// unlocked
			textPut("statesheet", "dest_unlocked", 1, 1);
		}
		else
		{
			m_state = -1;	// unknown
			textPut("statesheet", "dest_mixlocked", 1, 1);
		}
	}
	return 0;
}
 
// all database name changes come back here
void lock::databaseCallback( revertiveNotify * r )
{
} 


// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string lock::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		m_instance = p->value();
		getDev( m_instance, &m_device );
	}
	else if( p->command() == "index" )
	{
		m_index = p->value();
		m_states.clear();	// clear our states

		if( m_device && (m_index.count() > 0 ) )
		{
			controlEnable( 1, 1 );

			// put a value to the button just to indicate this is the unknown state
		 	textPut( "statesheet", "enum_unknown", 1, 1 );

			infoUnregister(m_device);
			// go off and get the lock state
			for (bncs_stringlistIterator slIt = m_index.begin(); slIt != m_index.end(); slIt++)
			{
				infoRegister(m_device, slIt->toInt(), slIt->toInt(), true);
				infoPoll(m_device, slIt->toInt(), slIt->toInt());
				m_states << "-1";	// add an unknown to our states until we get a revertive when it will be replaced
			}

			// change button text if we have multiple locks
			if (m_index.count() > 1)
				textPut("text", "LOCK ALL", 1, 1);
			else
				textPut("text", "LOCK", 1, 1);
		}
		else
			controlDisable( 1, 1 );
	}
	else if (p->command() == "return")
	{
		bncs_stringlist sl;

		sl << "index=" + bncs_string(m_index);
		sl << "lock_instance=" + m_instance;

		return sl.toString('\n');
	}
	else if (p->command() == "_events")
	{
		bncs_stringlist sl;

		sl << "index=<lock index>";

		return sl.toString('\n');
	}
	else if (p->command() == "_commands")
	{
		bncs_stringlist sl;

		sl.clear();

		return sl.toString('\n');
	}
	return "";
}

// timer events come here
void lock::timerCallback( int id )
{
}
