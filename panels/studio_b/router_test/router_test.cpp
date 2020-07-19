#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_stringlist.h>
#include <bncs_config.h>
#include "router_test.h"

#define PNL_MAIN	1

#define TIMER_SETUP	1
#define TIMER_DISPLAY 2
#define TIMER_DISPLAY_DURATION 5
#define TIMER_STEP 3

#define EDIT_SRC_RANGE_DEFAULT "1-168, 289-408"
#define EDIT_DEST_RANGE_DEFAULT "1-168, 289-384"
#define ROUTER_DEVICE 840

#define KEYB_SRC_RANGE 1
#define KEYB_SRC_FROM 2
#define KEYB_SRC_TO 3
#define KEYB_DEST_RANGE 4
#define KEYB_DEST_FROM 5
#define KEYB_DEST_TO 6

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_test )

// constructor - equivalent to ApplCore STARTUP
router_test::router_test( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel PNL_MAIN
	m_int_router_device = ROUTER_DEVICE;
	m_int_keyboard_callback = 0;
	m_str_flags = "";
	m_bool_busy = false;
	m_bool_timed_out = false;
	m_bool_auto = false;
	panelShow( PNL_MAIN, "p1.bncs_ui" );
	controlHide(PNL_MAIN, "keyboard");
	controlDisable(PNL_MAIN, "btn_auto_stop");

	m_int_src_from = 0;
	m_int_src_to = 0;
	m_int_dest_from = 0;
	m_int_dest_to = 0;
	updateSrcRange(bncs_stringlist(EDIT_SRC_RANGE_DEFAULT));
	updateDestRange(bncs_stringlist(EDIT_DEST_RANGE_DEFAULT));
	bncs_string ret;
	textGet("width", PNL_MAIN, "lbl_progress", ret);
	m_int_prog_bar_width = int(ret);
	debug("prog bar width " + bncs_string(m_int_prog_bar_width) + "\n");
	bncs_string str_x;
	bncs_string str_y;
	controlPosition(PNL_MAIN, "lbl_progress", str_x, str_y);
	m_int_prog_bar_x = int(str_x);
	m_int_prog_bar_y = int(str_y);
	takeSrc(0);
	updateRegisterRevertives();

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
//	setSize( PNL_MAIN );		// set the size to the same as the specified panel
}

// destructor - equivalent to ApplCore CLOSEDOWN
router_test::~router_test()
{
}

// all button pushes and notifications come here
void router_test::buttonCallback( buttonNotify *b )
{
	if (b->panel() == PNL_MAIN) {
		if (b->id() == "edit_src_range") {
			openKeyboard(KEYB_SRC_RANGE, bncs_string(m_sl_src_range));
		}
		else if (b->id() == "edit_src_from") {
			openKeyboard(KEYB_SRC_FROM, bncs_string(m_int_src_from));
		}
		else if (b->id() == "edit_src_to") {
			openKeyboard(KEYB_SRC_TO, bncs_string(m_int_src_to));
		}
		else if (b->id() == "edit_dest_range") {
			openKeyboard(KEYB_DEST_RANGE, bncs_string(m_sl_dest_range));
		}
		else if (b->id() == "edit_dest_from") {
			openKeyboard(KEYB_DEST_FROM, bncs_string(m_int_dest_from));
		}
		else if (b->id() == "edit_dest_to") {
			openKeyboard(KEYB_DEST_TO, bncs_string(m_int_dest_to));
		}
		else if (b->id() == "btn_reset") {
			takeSrc(m_int_src_from);
		}
		else if (b->id() == "btn_next") {
			takeSrc(getNext(m_int_current_src, m_int_src_from, m_int_src_to, m_sl_src_range));
		}
		else if (b->id() == "btn_auto_start") {
			m_bool_auto = true;
			controlEnable(PNL_MAIN, "btn_auto_stop");
			takeSrc(getNext(m_int_current_src, m_int_src_from, m_int_src_to, m_sl_src_range));
			timerStart(TIMER_STEP, 1000);
		}
		else if (b->id() == "btn_auto_stop") {
			m_bool_auto = false;
		}
		else if (b->id() == "keyboard") {
			bncs_string t;
			textGet("text", PNL_MAIN, "keyboard", t);
			switch (m_int_keyboard_callback) {
				case KEYB_SRC_RANGE:
					//TODO validate range
					updateSrcRange(bncs_stringlist(t));
					controlHide(PNL_MAIN, "keyboard");
					break;
				case KEYB_SRC_FROM:
					if (isInRange(t.toInt(), m_int_src_from, m_int_src_to, m_sl_src_range) == 1) {
						updateSrcFrom(t.toInt());
						controlHide(PNL_MAIN, "keyboard");
					}
					break;
				case KEYB_SRC_TO:
					if (isInRange(t.toInt(), m_int_src_from, m_int_src_to, m_sl_src_range) == 1) {
						updateSrcTo(t.toInt());
						controlHide(PNL_MAIN, "keyboard");
					}
					break;
				case KEYB_DEST_RANGE:
					//TODO validate range
					updateDestRange(bncs_stringlist(t));
					controlHide(PNL_MAIN, "keyboard");
					break;
				case KEYB_DEST_FROM:
					if (isInRange(t.toInt(), m_int_dest_from, m_int_dest_to, m_sl_dest_range) == 1) {
						updateDestFrom(t.toInt());
						controlHide(PNL_MAIN, "keyboard");
					}
					break;
				case KEYB_DEST_TO:
					if (isInRange(t.toInt(), m_int_dest_from, m_int_dest_to, m_sl_dest_range) == 1) {
						updateDestTo(t.toInt());
						controlHide(PNL_MAIN, "keyboard");
					}
					break;
				default:
					controlHide(PNL_MAIN, "keyboard");
			}
		}
	}
}

// all revertives come here
int router_test::revertiveCallback( revertiveNotify * r )
{
	switch( r->device() )
	{
		case ROUTER_DEVICE:
			if (r->info() == m_int_current_src) {
				m_str_flags[r->index()] = '0';
				//debug(bncs_string(r->index()) + " " + bncs_string(r->info()) + " " + m_str_flags + "\n");
			}
			break;
	}
	return 0;
}

// all database name changes come back here
void router_test::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_test::parentCallback( parentNotify *p )
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
void router_test::timerCallback( int id )
{
	switch (id) {
		case TIMER_SETUP: {
			timerStop(id);
			break;
		}

		case TIMER_DISPLAY: {
			int ones = countOnes();
			int pixelWidth = int((m_int_prog_bar_width * ones) / (m_str_flags.length() - 1));
			controlMove(PNL_MAIN, "lbl_progress_mask", m_int_prog_bar_x + pixelWidth, m_int_prog_bar_y - 10);
			if (ones == 0) {
				setBusyState(false);
			}
			break;
		}

		case TIMER_STEP: {
			timerStop (TIMER_STEP);
			while (m_bool_busy && !m_bool_timed_out) {
			}
			if (m_bool_timed_out) {
				//timed out TODO print a message
				// leave the progress bar in its current state. Step timer is already stopped.
			}
			else {
				takeSrc(getNext(m_int_current_src, m_int_src_from, m_int_src_to, m_sl_src_range));
				if (m_bool_auto) {
				}
			}
			break;
		}

	}


	//default:	// Unhandled timer event
		//(id);
		//break;
	//}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Callbacks above - Methods below ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void router_test::takeSrc(int src) {
	if (src < 0) src = 0;
	m_int_current_src = src;
	if (src > 0) {
		textPut("text", bncs_string(src), PNL_MAIN, "lbl_src_current");
		m_str_flags = "0";
		for (int i = 0; i < getLast(m_sl_dest_range); i++) {
			m_str_flags.append('0');
		}
		int d = 0;
		setBusyState(true);
		debug("about to start loop");
		while (getNext(d, m_int_dest_from, m_int_dest_to, m_sl_dest_range) > -1) {
			d = getNext(d, m_int_dest_from, m_int_dest_to, m_sl_dest_range);
			debug("make route " + bncs_string(src) + " " + bncs_string(d) + "\n");
			makeRoute(src, d);
			m_str_flags[d] = '1';
		}
	}
	else {
		textPut("text", bncs_string(""), PNL_MAIN, "lbl_src_current");
	}
}

int router_test::getFirst(bncs_stringlist list) {
	return int(bncs_stringlist (list[0], '-')[0]);
}

int router_test::getLast(bncs_stringlist list) {
	return int(bncs_stringlist(list[list.count() - 1], '-')[1]);
}

int router_test::getNext (int n, int from, int to, bncs_stringlist list) { // returns port or -1 if invalid
	int next = n + 1;
	//debug(bncs_string(next) + "\n");
	while (isInRange(next, from, to, list) == 0) { // not currently valid but keep going
		//debug("*" + bncs_string(next) + "*" + bncs_string(from) + "*" + bncs_string(to) + "*" + bncs_string(isInRange(next, from, to, list)) + "*\n");
		next++;
	} // now has to be 1 or -1
	if (isInRange(next, from, to, list) == 1) return next;
	else return -1;
}

int router_test::isInRange(int n, int from, int to, bncs_stringlist list) { // return 1 if yes, -1 if beyond end, else 0
	if (n > to) return -1;
	if (n < from) return 0;
	for (int blockIdx = 0; blockIdx < list.count(); blockIdx++) {
		bncs_stringlist block = bncs_stringlist(list[blockIdx], '-');
		if (n < int(block[0])) return 0; // not in range yet but keep going
		//debug(bncs_string(int(block[0])) + "£" + bncs_string(int(block[1])) + "\n");
		if (n <= int(block[1])) return 1; // in range - return now
	}
	// now must be beyond range
	return -1;
}


void router_test::makeRoute(int src, int dest) {
	routerCrosspoint(m_int_router_device, src, dest);
}

void router_test::updateRegisterRevertives() {
	routerUnregister(ROUTER_DEVICE);
	routerRegister(ROUTER_DEVICE, m_int_dest_from, m_int_dest_to);
}

int router_test::countOnes() {
	int c = 0;
	for (int i = 1;  i < m_str_flags.length(); i++) {
		if (m_str_flags[i] == '1') c++;
	}
	return c;
}

void router_test::setBusyState(bool busy) {
	if (busy) {
		m_bool_busy = true;
		controlDisable(PNL_MAIN, "btn_reset");
		controlDisable(PNL_MAIN, "btn_next");
		controlDisable(PNL_MAIN, "btn_auto_start");
		timerStart(TIMER_DISPLAY, TIMER_DISPLAY_DURATION);
	}
	else {
		timerStop(TIMER_DISPLAY);
		if (m_bool_auto) {
			timerStart(TIMER_STEP, 100);
		}
		else {
			controlEnable(PNL_MAIN, "btn_reset");
			controlEnable(PNL_MAIN, "btn_next");
			controlEnable(PNL_MAIN, "btn_auto_start");
		}
		m_bool_busy = false;
	}
}

void router_test::updateSrcRange(bncs_stringlist range) {
	// this should only be called with a valid input
	m_sl_src_range = range;
	textPut("text", bncs_string(m_sl_src_range), PNL_MAIN, "edit_src_range");
	if (isInRange(m_int_src_from, m_int_src_from, m_int_src_to, m_sl_src_range) != 1) {
		updateSrcFrom(getFirst(m_sl_src_range));
	}
	if (isInRange(m_int_src_to, m_int_src_from, m_int_src_to, m_sl_src_range) != 1) {
		updateSrcTo(getLast(m_sl_src_range));
	}
}

void router_test::updateSrcFrom(int from) {
	// this should only be called with a valid input
	m_int_src_from = from;
	textPut("text", bncs_string(m_int_src_from), PNL_MAIN, "edit_src_from");
	textPut("text", bncs_string("Reset to|Source " + bncs_string(m_int_src_from)), PNL_MAIN, "btn_reset");
}

void router_test::updateSrcTo(int to) {
	// this should only be called with a valid input
	m_int_src_to = to;
	textPut("text", bncs_string(m_int_src_to), PNL_MAIN, "edit_src_to");
}

void router_test::updateDestRange(bncs_stringlist range) {
	// this should only be called with a valid input
	m_sl_dest_range = range;
	textPut("text", bncs_string(m_sl_dest_range), PNL_MAIN, "edit_dest_range");
	if (isInRange(m_int_dest_from, m_int_dest_from, m_int_dest_to, m_sl_dest_range) != 1) {
		updateDestFrom(getFirst(m_sl_dest_range));
	}
	if (isInRange(m_int_dest_to, m_int_dest_from, m_int_dest_to, m_sl_dest_range) != 1) {
		updateDestTo(getLast(m_sl_dest_range));
	}
}

void router_test::updateDestFrom(int from) {
	// this should only be called with a valid input
	m_int_dest_from = from;
	textPut("text", bncs_string(m_int_dest_from), PNL_MAIN, "edit_dest_from");
}

void router_test::updateDestTo(int to) {
	// this should only be called with a valid input
	m_int_dest_to = to;
	textPut("text", bncs_string(m_int_dest_to), PNL_MAIN, "edit_dest_to");
}

void router_test::openKeyboard(int variable, bncs_string text) {
	m_int_keyboard_callback = variable;
	textPut("text", text, PNL_MAIN, "keyboard");
	controlShow(PNL_MAIN, "keyboard");
}