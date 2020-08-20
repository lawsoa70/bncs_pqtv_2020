#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "interop_state.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( interop_state )

// constructor - equivalent to ApplCore STARTUP
interop_state::interop_state( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_intDevice = 0;
	m_intMasterIndex = 0;
	m_boolReadOnly = false;
	m_strLocalState = "NULL";
	m_strExternalState = "NULL";
	m_strOutputState = "NULL";
	m_intLocalSlot = 0;
	m_intExternalSlot = 0;
	m_intOutputSlot = 0;
	m_strPanel = PANEL_DEFAULT;

	// show the default panel layout on start
	panelShow(PNL_MAIN, bncs_string("layouts/%1.bncs_ui").arg(m_strPanel));

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
//	setSize( PNL_MAIN );		// set the size to the same as the specified panel

	timerStart(TIMER_SETUP, 100);
}

// destructor - equivalent to ApplCore CLOSEDOWN
interop_state::~interop_state()
{
}

// all button pushes and notifications come here
void interop_state::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PNL_MAIN ) {
		if (!m_boolReadOnly) {
			if (b->id() == "btn_reh") {
				if (m_strLocalState == "REH") {
					infoWrite(m_intDevice, "RIG", m_intLocalSlot);
				} else {
					infoWrite(m_intDevice, "REH", m_intLocalSlot);
				}
			} else if (b->id() == "btn_impend") {
				if (m_strLocalState == "IMPEND") {
					infoWrite(m_intDevice, "REH", m_intLocalSlot);
				} else {
					infoWrite(m_intDevice, "IMPEND", m_intLocalSlot);
				}
			} else if (b->id() == "btn_tx") {
				if (m_strLocalState == "TX") {
					infoWrite(m_intDevice, "REH", m_intLocalSlot);
				} else {
					infoWrite(m_intDevice, "TX", m_intLocalSlot);
				}
			}
			else if (b->id() == "btn_ext") {
				infoWrite(m_intDevice, m_boolAllowExternal ? "0" : "1", m_intAllowExternalSlot);
			}
		}
	}
}

// all revertives come here
int interop_state::revertiveCallback( revertiveNotify * r )
{
	if (r->device() == m_intDevice) {
		debug("interop_state::revertiveCallback index: %1, value: %1\n", r->index(), r->sInfo());
		if (r->index() == m_intLocalSlot) {
			m_strLocalState = r->sInfo();
			timerStop(TIMER_LOCAL_FLASH_OFF);
			timerStop(TIMER_LOCAL_FLASH_ON);
			if (m_strLocalState == "REH") {
				textPut("led", "true", PNL_MAIN, "btn_reh");
				textPut("led", "false", PNL_MAIN, "btn_impend");
				textPut("led", "false", PNL_MAIN, "btn_tx");
			}
			else if (m_strLocalState == "TX") {
				textPut("led", "false", PNL_MAIN, "btn_reh");
				textPut("led", "false", PNL_MAIN, "btn_impend");
				textPut("led", "true", PNL_MAIN, "btn_tx");
			}
			else if (m_strLocalState == "IMPEND") {
				textPut("led", "false", PNL_MAIN, "btn_reh");
				textPut("led", "true", PNL_MAIN, "btn_impend");
				textPut("led", "false", PNL_MAIN, "btn_tx");
				timerStart(TIMER_LOCAL_FLASH_OFF, 500);
			}
			else {
				textPut("led", "false", PNL_MAIN, "btn_reh");
				textPut("led", "false", PNL_MAIN, "btn_impend");
				textPut("led", "false", PNL_MAIN, "btn_tx");
			}
		}
		else if (r->index() == m_intExternalSlot) {
			m_strExternalState = r->sInfo();
			timerStop(TIMER_EXTERNAL_FLASH_OFF);
			timerStop(TIMER_EXTERNAL_FLASH_ON);
			if (m_strExternalState == "REH") {
				textPut("colour.led", "blue", PNL_MAIN, "btn_ext");
			}
			else if (m_strExternalState == "TX") {
				textPut("colour.led", "red", PNL_MAIN, "btn_ext");
			}
			else if (m_strExternalState == "IMPEND") {
				textPut("colour.led", "red", PNL_MAIN, "btn_ext");
				timerStart(TIMER_EXTERNAL_FLASH_OFF, 500);
			}
			else {
				textPut("colour.led", "black", PNL_MAIN, "btn_ext");
			}
		}
		else if (r->index() == m_intOutputSlot) {
			m_strOutputState = r->sInfo();
			timerStop(TIMER_OUTPUT_FLASH_OFF);
			timerStop(TIMER_OUTPUT_FLASH_ON);
			if (m_strOutputState == "REH") {
				textPut("stylesheet", "reh_btn_on", PNL_MAIN, "btn_reh");
				textPut("stylesheet", "tx_btn_off", PNL_MAIN, "btn_impend");
				textPut("stylesheet", "tx_btn_off", PNL_MAIN, "btn_tx");
			}
			else if (m_strOutputState == "TX") {
				textPut("stylesheet", "reh_btn_off", PNL_MAIN, "btn_reh");
				textPut("stylesheet", "tx_btn_off", PNL_MAIN, "btn_impend");
				textPut("stylesheet", "tx_btn_on", PNL_MAIN, "btn_tx");
			}
			else if (m_strOutputState == "IMPEND") {
				textPut("stylesheet", "reh_btn_off", PNL_MAIN, "btn_reh");
				textPut("stylesheet", "tx_btn_on", PNL_MAIN, "btn_impend");
				textPut("stylesheet", "tx_btn_off", PNL_MAIN, "btn_tx");
				timerStart(TIMER_OUTPUT_FLASH_OFF, 500);
			}
			else {
				textPut("stylesheet", "reh_btn_off", PNL_MAIN, "btn_reh");
				textPut("stylesheet", "tx_btn_off", PNL_MAIN, "btn_impend");
				textPut("stylesheet", "tx_btn_off", PNL_MAIN, "btn_tx");
			}
		}
		else if (r->index() == m_intAllowExternalSlot) {
			m_boolAllowExternal = (r->sInfo() == "1") ? true : false;
			textPut("colour.background", m_boolAllowExternal ? "yellow" : "", PNL_MAIN, "btn_ext");
		}
	}
	return 0;
}

// all database name changes come back here
void interop_state::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string interop_state::parentCallback( parentNotify *p )
{
	if( p->command() == "return" )
	{
		if( p->value() == "all" )
		{	// Persisting values for bncs_vis_ed
			bncs_stringlist sl;
			//sl << bncs_string("myParam=%1").arg(m_myParam);
			sl << bncs_string("MasterIndex=%1").arg(bncs_string(m_intMasterIndex));
			sl << bncs_string("Panel=%1").arg(bncs_string(m_strPanel));
			sl << bncs_string("ReadOnly=%1").arg(m_boolReadOnly ? "true" : "false");
			return sl.toString('\n');
		}

	}

	else if( p->command() == "instance" && p->value() != m_instance )
	{	// Our instance is being set/changed
		m_instance = p->value();
		//Do something instance-change related here
		getDev(m_instance, &m_intDevice);
	}

	else if (p->command() == "MasterIndex") {
		m_intMasterIndex = int(p->value());
	}

	else if (p->command() == "ReadOnly") {
		m_boolReadOnly = (p->value().lower() == "true" || p->value() == "1");
	}

	else if (p->command() == "Panel")
	{
		if (p->value().length() >0){
			m_strPanel = p->value();
		}
		else
		{
			m_strPanel = PANEL_DEFAULT;
		}
		panelDestroy(PNL_MAIN);
		panelShow(PNL_MAIN, bncs_string("layouts/%1.bncs_ui").arg(m_strPanel));
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
void interop_state::timerCallback( int id )
{
	switch( id )
	{
	case TIMER_SETUP:
		timerStop(id);
		m_intLocalSlot = getMasterLocalStateSlot(m_intMasterIndex);
		if (m_intLocalSlot > 0) {
			infoRegister(m_intDevice, m_intLocalSlot, m_intLocalSlot);
			infoPoll(m_intDevice, m_intLocalSlot, m_intLocalSlot);
		}
		m_intExternalSlot = getMasterExternalStateSlot(m_intMasterIndex);
		if (m_intExternalSlot > 0) {
			infoRegister(m_intDevice, m_intExternalSlot, m_intExternalSlot, true);
			infoPoll(m_intDevice, m_intExternalSlot, m_intExternalSlot);
		}
		m_intOutputSlot = getMasterOutputStateSlot(m_intMasterIndex);
		if (m_intOutputSlot > 0) {
			infoRegister(m_intDevice, m_intOutputSlot, m_intOutputSlot, true);
			infoPoll(m_intDevice, m_intOutputSlot, m_intOutputSlot);
		}
		m_intAllowExternalSlot = getMasterAllowExternalSlot(m_intMasterIndex);
		if (m_intAllowExternalSlot > 0) {
			infoRegister(m_intDevice, m_intAllowExternalSlot, m_intAllowExternalSlot, true);
			infoPoll(m_intDevice, m_intAllowExternalSlot, m_intAllowExternalSlot);
		}
		break;

	case TIMER_LOCAL_FLASH_OFF:
		timerStop(id);
		timerStart(TIMER_LOCAL_FLASH_ON, 500);
		textPut("led", "false", PNL_MAIN, "btn_impend");
		break;

	case TIMER_LOCAL_FLASH_ON:
		timerStop(id);
		timerStart(TIMER_LOCAL_FLASH_OFF, 500);
		textPut("led", "true", PNL_MAIN, "btn_impend");
		break;

	case TIMER_EXTERNAL_FLASH_OFF:
		timerStop(id);
		timerStart(TIMER_EXTERNAL_FLASH_ON, 500);
		textPut("colour.led", "black", PNL_MAIN, "btn_ext");
		break;

	case TIMER_EXTERNAL_FLASH_ON:
		timerStop(id);
		timerStart(TIMER_EXTERNAL_FLASH_OFF, 500);
		textPut("colour.led", "red", PNL_MAIN, "btn_ext");
		break;

	case TIMER_OUTPUT_FLASH_OFF:
		timerStop(id);
		timerStart(TIMER_OUTPUT_FLASH_ON, 500);
		textPut("colour.background", "", PNL_MAIN, "btn_impend");
		break;

	case TIMER_OUTPUT_FLASH_ON:
		timerStop(id);
		timerStart(TIMER_OUTPUT_FLASH_OFF, 500);
		textPut("colour.background", "red", PNL_MAIN, "btn_impend");
		break;

	default:	// Unhandled timer event
		timerStop(id);
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Callbacks above - Methods below ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int interop_state::getMasterLocalStateSlot(int master) {
	return (MASTER_LOCAL_STATE_TABLE + master);
}

int interop_state::getMasterExternalStateSlot(int master) {
	return (MASTER_EXTERNAL_STATE_TABLE + master);
}

int interop_state::getMasterOutputStateSlot(int master) {
	return (MASTER_OUTPUT_STATE_TABLE + master);
}

int interop_state::getMasterAllowExternalSlot(int master) {
	return (ALLOW_EXTERNAL_TABLE + master);
}


