#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "InteropAuto.h"

#define PNL_MAIN	1

#define TIMER_SETUP	1

#define MASTER_DB_NUMBER                2
#define SLAVE_DB_NUMBER                 3

#define MASTER_COUNT                   20
#define SLAVE_COUNT                    50
#define MASTER_LOCAL_STATE_TABLE      100
#define MASTER_OUTPUT_STATE_TABLE     200
#define MASTER_BLUE_TABLE             300
#define MASTER_RED_TABLE              400
#define SLAVE_BLUE_TABLE              500
#define SLAVE_RED_TABLE               600
#define MASTER_MASTER_MATRIX_TABLE   1000
#define MASTER_SLAVE_MATRIX_TABLE    2000

//define MASTER_STUDIO_A                 1
//define MASTER_STUDIO_B                 2
//define MASTER_STUDIO_C                 3

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( InteropAuto )

// constructor - equivalent to ApplCore STARTUP
InteropAuto::InteropAuto( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	//Initialise Member Vars
	m_instance = "";
	m_intDevice = 0;

	// show a panel from file p1.bncs_ui and we'll know it as our panel PNL_MAIN
	panelShow( PNL_MAIN, "p1.bncs_ui" );

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
//	setSize( PNL_MAIN );		// set the size to the same as the specified panel
	timerStart(TIMER_SETUP, 100);
}

// destructor - equivalent to ApplCore CLOSEDOWN
InteropAuto::~InteropAuto()
{
}

// all button pushes and notifications come here
void InteropAuto::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PNL_MAIN )
	{
		switch( b->id() )
		{
			case 1:			textPut( "text", m_instance,              PNL_MAIN, 4 );	   	break;
			case 2:			textPut( "text", "you pressed|control 2", PNL_MAIN, 4 );		break;
			case 3:			textPut( "text", "you pressed|control 3", PNL_MAIN, 4 );		break;
		}
	}
}

// all revertives come here
int InteropAuto::revertiveCallback( revertiveNotify * r ) {
	if (r->device() == m_intDevice) {
		int relative;
		int source;
		int dest;
		if (r->index() > MASTER_SLAVE_MATRIX_TABLE) {
			relative = r->index() - MASTER_SLAVE_MATRIX_TABLE;
			source = (relative - 1) / SLAVE_COUNT;
			dest = ((relative - 1) % SLAVE_COUNT) + 1;
			if (r->sInfo() == "0") {
				debug("InteropAuto::revertiveCallback MASTER_SLAVE_MATRIX_TABLE index: %1, relative: %2, source: %3, dest: %4, value: %5\n", r->index(), relative, source, dest, r->sInfo());
			}
			else if (r->sInfo() == "1") {
				debug("InteropAuto::revertiveCallback MASTER_SLAVE_MATRIX_TABLE index: %1, relative: %2, source: %3, dest: %4, value: %5\n", r->index(), relative, source, dest, r->sInfo());
			}
			else {
				setMasterSlaveMatrixSlot(source, dest, 0);
			}
		}
		else if (r->index() > MASTER_MASTER_MATRIX_TABLE) {
			relative = r->index() - MASTER_MASTER_MATRIX_TABLE;
			source = (relative - 1) / MASTER_COUNT;
			dest = ((relative - 1) % MASTER_COUNT) + 1;
			if (r->sInfo() == "0") {
				debug("InteropAuto::revertiveCallback MASTER_MASTER_MATRIX_TABLE index: %1, relative: %2, source: %3, dest: %4, value: %5\n", r->index(), relative, source, dest, r->sInfo());
			}
			else if (r->sInfo() == "1") {
				debug("InteropAuto::revertiveCallback MASTER_MASTER_MATRIX_TABLE index: %1, relative: %2, source: %3, dest: %4, value: %5\n", r->index(), relative, source, dest, r->sInfo());
			}
			else {
				setMasterMasterMatrixSlot(source, dest, 0);
			}
		}
		else if (r->index() > SLAVE_RED_TABLE) {
			relative = r->index() - SLAVE_RED_TABLE;
			if (r->sInfo() == "0") {
				debug("InteropAuto::revertiveCallback SLAVE_RED_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else if (r->sInfo() == "1") {
				debug("InteropAuto::revertiveCallback SLAVE_RED_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else {
				setSlaveRedSlot(relative, 0);
			}
		}
		else if (r->index() > SLAVE_BLUE_TABLE) {
			relative = r->index() - SLAVE_BLUE_TABLE;
			if (r->sInfo() == "0") {
				debug("InteropAuto::revertiveCallback SLAVE_BLUE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else if (r->sInfo() == "1") {
				debug("InteropAuto::revertiveCallback SLAVE_BLUE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else {
				setSlaveBlueSlot(relative, 0);
			}
		}
		else if (r->index() > MASTER_RED_TABLE) {
			relative = r->index() - MASTER_RED_TABLE;
			if (r->sInfo() == "0") {
				debug("InteropAuto::revertiveCallback MASTER_RED_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else if (r->sInfo() == "1") {
				debug("InteropAuto::revertiveCallback MASTER_RED_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else {
				setMasterRedSlot(relative, 0);
			}
		}
		else if (r->index() > MASTER_BLUE_TABLE) {
			relative = r->index() - MASTER_BLUE_TABLE;
			if (r->sInfo() == "0") {
				debug("InteropAuto::revertiveCallback MASTER_BLUE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else if (r->sInfo() == "1") {
				debug("InteropAuto::revertiveCallback MASTER_BLUE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else {
				setMasterBlueSlot(relative, 0);
			}
		}
		else if (r->index() > MASTER_OUTPUT_STATE_TABLE) {
			relative = r->index() - MASTER_OUTPUT_STATE_TABLE;
			if (r->sInfo() == "NULL") {
				debug("InteropAuto::revertiveCallback MASTER_OUTPUT_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
				setMasterBlueSlot(relative, 0);
				setMasterRedSlot(relative, 0);
			}
			else if (r->sInfo() == "RIG") {
				debug("InteropAuto::revertiveCallback MASTER_OUTPUT_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
				setMasterBlueSlot(relative, 0);
				setMasterRedSlot(relative, 0);
			}
			else if (r->sInfo() == "REH") {
				debug("InteropAuto::revertiveCallback MASTER_OUTPUT_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
				setMasterBlueSlot(relative, 1);
				setMasterRedSlot(relative, 0);
			}
			else if (r->sInfo() == "IMPEND") {
				debug("InteropAuto::revertiveCallback MASTER_OUTPUT_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
				setMasterBlueSlot(relative, 0);
				setMasterRedSlot(relative, 1);
			}
			else if (r->sInfo() == "TX") {
				debug("InteropAuto::revertiveCallback MASTER_OUTPUT_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
				setMasterBlueSlot(relative, 0);
				setMasterRedSlot(relative, 1);
			}
			else {
				setMasterOutputState(relative, "NULL");
			}
		}
		else if (r->index() > MASTER_LOCAL_STATE_TABLE) {
			relative = r->index() - MASTER_LOCAL_STATE_TABLE;
			if (r->sInfo() == "NULL") {
				debug("InteropAuto::revertiveCallback MASTER_LOCAL_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else if (r->sInfo() == "RIG") {
				debug("InteropAuto::revertiveCallback MASTER_LOCAL_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else if (r->sInfo() == "REH") {
				debug("InteropAuto::revertiveCallback MASTER_LOCAL_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else if (r->sInfo() == "IMPEND") {
				debug("InteropAuto::revertiveCallback MASTER_LOCAL_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else if (r->sInfo() == "TX") {
				debug("InteropAuto::revertiveCallback MASTER_LOCAL_STATE_TABLE relative: %1, value: %2\n", relative, r->sInfo());
			}
			else {
				setMasterLocalState(relative, "NULL");
			}
		}
		else {
			debug("InteropAuto::revertiveCallback Error - Infodriver index too low\n");
		}
	}
	return 0;
}

// all database name changes come back here
void InteropAuto::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string InteropAuto::parentCallback( parentNotify *p )
{
	if( p->command() == "return" )
	{
		if( p->value() == "all" )
		{	// Persisting values for bncs_vis_ed
			bncs_stringlist sl;
			
			sl << bncs_string( "myParam=%1" ).arg( m_myParam );
			
			return sl.toString( '\n' );
		}

		else if( p->value() == "myParam" )
		{	// Specific value being asked for by a textGet
			return( bncs_string( "%1=%2" ).arg( p->value() ).arg( m_myParam ) );
		}

	}
	else if( p->command() == "instance" && p->value() != m_instance )
	{	// Our instance is being set/changed
		m_instance = p->value();
		//Do something instance-change related here
	}

	else if( p->command() == "myParam" )
	{	// Persisted value or 'Command' being set here
		m_myParam = p->value();
	}

	// ***** CONNECTIONS EVENTS HELPER LIST *****
	else if( p->command() == "_events" )
	{	// Helper-list of everything in this component generated by hostNotify's
		bncs_stringlist sl;

		sl << "notify=*";		
		
		return sl.toString( '\n' );
	}

	// ***** CONNECTIONS COMMANDS HELPER LIST *****
	else if( p->command() == "_commands" )
	{	// Helper-list of any commands/parameters you might want to set at run-time
		bncs_stringlist sl;
		
		sl << "myParam=[value]";
		
		return sl.toString( '\n' );
	}

	return "";
}

// timer events come here
void InteropAuto::timerCallback( int id )
{
	boolean t;
	switch( id )
	{
	case TIMER_SETUP:
		timerStop(id);
		getDev(m_instance, &m_intDevice);
		debug("InteropAuto::timerCallback m_instance=%1, m_device=%2\n", m_instance, m_intDevice);
		t = getRangeInUse(MASTER_DB_NUMBER, MASTER_COUNT, m_intMinimumMaster, m_intMaximumMaster);
		debug("InteropAuto::timerCallback m_intMinimumMaster=%1, m_intMaximumMaster=%2, return=%3\n", m_intMinimumMaster, m_intMaximumMaster, t);
		t = getRangeInUse(SLAVE_DB_NUMBER, SLAVE_COUNT, m_intMinimumSlave, m_intMaximumSlave);
		debug("InteropAuto::timerCallback m_intMinimumSlave=%1, m_intMaximumSlave=%2, return=%3\n", m_intMinimumSlave, m_intMaximumSlave, t);
		//register infodriver revertives
		infoRegister(m_intDevice, getMasterLocalStateSlot (m_intMinimumMaster), getMasterLocalStateSlot (m_intMaximumMaster), false);
		infoPoll(m_intDevice, getMasterLocalStateSlot(m_intMinimumMaster), getMasterLocalStateSlot(m_intMaximumMaster));
		infoRegister(m_intDevice, getMasterOutputStateSlot(m_intMinimumMaster), getMasterOutputStateSlot(m_intMaximumMaster), true);
		infoPoll(m_intDevice, getMasterOutputStateSlot(m_intMinimumMaster), getMasterOutputStateSlot(m_intMaximumMaster));
		infoRegister(m_intDevice, getMasterBlueSlot(m_intMinimumMaster), getMasterBlueSlot(m_intMaximumMaster), true);
		infoPoll(m_intDevice, getMasterBlueSlot(m_intMinimumMaster), getMasterBlueSlot(m_intMaximumMaster));
		infoRegister(m_intDevice, getMasterRedSlot(m_intMinimumMaster), getMasterRedSlot(m_intMaximumMaster), true);
		infoPoll(m_intDevice, getMasterRedSlot(m_intMinimumMaster), getMasterRedSlot(m_intMaximumMaster));
		infoRegister(m_intDevice, getSlaveBlueSlot(m_intMinimumSlave), getSlaveBlueSlot(m_intMaximumSlave), true);
		infoPoll(m_intDevice, getSlaveBlueSlot(m_intMinimumSlave), getSlaveBlueSlot(m_intMaximumSlave));
		infoRegister(m_intDevice, getSlaveRedSlot(m_intMinimumSlave), getSlaveRedSlot(m_intMaximumSlave), true);
		infoPoll(m_intDevice, getSlaveRedSlot(m_intMinimumSlave), getSlaveRedSlot(m_intMaximumSlave));
		infoRegister(m_intDevice, getMasterMasterMatrixSlot(m_intMinimumMaster, m_intMinimumMaster),
			getMasterMasterMatrixSlot(m_intMaximumMaster, m_intMaximumMaster), true);
		infoPoll(m_intDevice, getMasterMasterMatrixSlot(m_intMinimumMaster, m_intMinimumMaster),
			getMasterMasterMatrixSlot(m_intMaximumMaster, m_intMaximumMaster));
		infoRegister(m_intDevice, getMasterSlaveMatrixSlot(m_intMinimumMaster, m_intMinimumSlave),
			getMasterSlaveMatrixSlot(m_intMaximumMaster, m_intMaximumSlave),   true);
		infoPoll(m_intDevice, getMasterSlaveMatrixSlot(m_intMinimumMaster, m_intMinimumSlave),
			getMasterSlaveMatrixSlot(m_intMaximumMaster, m_intMaximumSlave));

		break;

	default:	// Unhandled timer event
		timerStop(id);
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Callbacks above - Methods below ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////


void InteropAuto::setMasterLocalState(int master, bncs_string state) {
	int slot = getMasterLocalStateSlot(master);
	infoWrite(m_intDevice, state, slot);
}

void InteropAuto::setMasterOutputState(int master, bncs_string state) {
	int slot = getMasterOutputStateSlot(master);
	infoWrite(m_intDevice, state, slot);
}

void InteropAuto::setMasterBlueSlot(int master, int state) {
	int slot = getMasterBlueSlot(master);
	infoWrite(m_intDevice, bncs_string(state), slot);
}

void InteropAuto::setMasterRedSlot(int master, int state) {
	int slot = getMasterRedSlot(master);
	infoWrite(m_intDevice, bncs_string(state), slot);
}

void InteropAuto::setSlaveBlueSlot(int master, int state) {
	int slot = getSlaveBlueSlot(master);
	infoWrite(m_intDevice, bncs_string(state), slot);
}

void InteropAuto::setSlaveRedSlot(int master, int state) {
	int slot = getSlaveRedSlot(master);
	infoWrite(m_intDevice, bncs_string(state), slot);
}

void InteropAuto::setMasterMasterMatrixSlot(int source, int dest, int state) {
	int slot = getMasterMasterMatrixSlot(source, dest);
	infoWrite(m_intDevice, bncs_string(state), slot);
}

void InteropAuto::setMasterSlaveMatrixSlot(int source, int dest, int state) {
	int slot = getMasterSlaveMatrixSlot(source, dest);
	infoWrite(m_intDevice, bncs_string(state), slot);
}

int InteropAuto::getMasterLocalStateSlot(int master) {
	return (MASTER_LOCAL_STATE_TABLE + master);
}

int InteropAuto::getMasterOutputStateSlot(int master) {
	return (MASTER_OUTPUT_STATE_TABLE + master);
}

int InteropAuto::getMasterBlueSlot(int master) {
	return (MASTER_BLUE_TABLE + master);
}

int InteropAuto::getMasterRedSlot(int master) {
	return (MASTER_RED_TABLE + master);
}

int InteropAuto::getSlaveBlueSlot(int slave) {
	return (SLAVE_BLUE_TABLE + slave);
}

int InteropAuto::getSlaveRedSlot(int slave) {
	return (SLAVE_RED_TABLE + slave);
}

int InteropAuto::getMasterMasterMatrixSlot(int source, int dest) {
	return (MASTER_MASTER_MATRIX_TABLE + (MASTER_COUNT * source) + dest);
}

int InteropAuto::getMasterSlaveMatrixSlot(int source, int dest) {
	return (MASTER_SLAVE_MATRIX_TABLE + (SLAVE_COUNT * source) + dest);
}

boolean InteropAuto::getRangeInUse(int db, int size, int &firstOut, int &lastOut) {
	firstOut = 0;
	lastOut = 0;
	for (int i = 1; i <= size; i++) {
		bncs_string s = "";
		routerName(m_intDevice, db, i, s);
		if (s.length() > 0) {
			if (firstOut == 0) firstOut = i; debug("InteropAuto::getRangeInUse firstOut=%1\n", firstOut);
			lastOut = i;
		}
	}
	return firstOut > 0;
}

