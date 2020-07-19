#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "mv_select.h"

#define PNL_MAIN	1

#define TIMER_SETUP	1

#define STARTX		970
#define STARTY		40
#define SIZEX		118
#define SIZEY		83
#define SPACEX		10
#define SPACEY		14
#define LAYOUT_SLOT	401

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( mv_select )

// constructor - equivalent to ApplCore STARTUP
mv_select::mv_select( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{

	// show a panel from file p1.bncs_ui and we'll know it as our panel PNL_MAIN
	panelShow( PNL_MAIN, "p1.bncs_ui" );

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
	//	setSize( 1024,668 );		// set the size explicitly
	//	setSize( PNL_MAIN );		// set the size to the same as the specified panel

	m_strArea = getWorkstationSetting("ops_area");
	m_strMvSettings = bncs_string("%1_multiviewers").arg(m_strArea); // e.g "studio_b_multiviewers"
	m_strLayoutDriver = getObjectSetting(m_strMvSettings, "layout_driver");
	getDev(m_strLayoutDriver, &m_intLayoutDriverDev);
	m_slAreaMvList = "";
	int n = 1;
	bncs_string s = getObjectSetting(m_strMvSettings, bncs_string("mv_%1").arg(n));  // e.g. s = "studio_b_mv_1"
	while (s.length() > 0) {
		m_slAreaMvList << s;
		bncs_string strName = getObjectSetting(s, "name"); // e.g. name = "Studio B MV 1"
		textPut(bncs_string("add"), strName, PNL_MAIN, "list_mv");
		n++;
		s = getObjectSetting(m_strMvSettings, bncs_string("mv_%1").arg(n));  // e.g. s = "studio_b_mv_2"
	}
}

// destructor - equivalent to ApplCore CLOSEDOWN
mv_select::~mv_select()
{
}

// all button pushes and notifications come here
void mv_select::buttonCallback( buttonNotify *b )
{
	//debug(bncs_string("\nid: %1, c: %2, v: %3, subs: %4, sub(0): %5\n").arg(b->id()).arg(b->command()).arg(b->value()).arg(b->subs()).arg(b->sub(0)));
	if (b->panel() == PNL_MAIN) {
		if (b->id() == "list_mv") {
			if (b->command() == "selection") {
				changeMvSelection(getCurrentMv());
			}
		}
		else if (b->id().startsWith("btnLayout_")) {
			bncs_string strRequestedLayout = b->id().right(b->id().length() - 10);
			debug(bncs_string("\nRequestedLayout: <%1>\n").arg(strRequestedLayout));
			debug(bncs_string("\nDev: <%1> Text: <%2> Slot: <%3>\n")
				.arg(m_intLayoutDriverDev).arg(strRequestedLayout).arg(m_intLayoutInfoSlot));
			infoWrite(m_intLayoutDriverDev, strRequestedLayout, m_intLayoutInfoSlot);
		}
	}
}

// all revertives come here
int mv_select::revertiveCallback( revertiveNotify * r )
{
	if (r->device() == m_intLayoutDriverDev && r->index() == m_intLayoutInfoSlot) {
		debug(bncs_string("\nLayout: <%1>\n").arg(r->sInfo()));
		// tell the multiviewer component
		textPut("layout", r->sInfo(), PNL_MAIN, "multiviewer");
		// highlight the button
		for (int i = 0; i < m_slLayoutList.count(); i++) {
			bncs_string strId = m_slLayoutList[i].lower().replace(' ', '_');
			textPut("statesheet=default", PNL_MAIN, bncs_string("btnLayout_%1").arg(strId));
		}
		textPut("statesheet=dest_selected", PNL_MAIN, bncs_string("btnLayout_%1").arg(r->sInfo()));
	}
	return 0;
}

// all database name changes come back here
void mv_select::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string mv_select::parentCallback( parentNotify *p ) {
	debug(bncs_string("\nc: %1, v: %2\n").arg(p->command()).arg(p->value()));
	if (p->command() == "return") {
		if (p->value() == "all") { // Persisting values for bncs_vis_ed
			bncs_stringlist sl;
			//sl << bncs_string( "myParam=%1" ).arg( m_myParam );
			return sl.toString( '\n' );
		}
	}
	else if (p->command() == "raw_parameters") {
		//get id of current mv selection if any
		bncs_string strId;
		textGet("selectedindex", PNL_MAIN, "list_mv", strId);
		int intCurrentId = int(strId);
		int intNewId = 0;
		int intParamId = -2;
		if (p->value().length() > 0) {
			// parameter exists but need to check it's valid
			intParamId = m_slAreaMvList.find(p->value());
			if (intParamId >= 0) {
				// it's in the list
				intNewId = intParamId;
			} // else leave newId as 0
		}
		else {
			// no param given so see if there is a current value
			if (intCurrentId >= 0) {
				intNewId = intCurrentId;
			} // else leave newId as 0
		}
		//debug(bncs_string("\nintCurrentId: <%1> intParamId: <%2> intNewId: <%3>\n")
		//		.arg(intCurrentId).arg(intParamId).arg(intNewId));
		if (intCurrentId < 0 && intNewId == 0) {
			// included this line since, in some unknown circumstances, I don't seem to be able to 
			//set listbox index to 0 when there is no existing selection
			textPut(bncs_string("selected.id=1"), PNL_MAIN, "list_mv");
		}
		textPut(bncs_string("selected.id=%1").arg(intNewId), PNL_MAIN, "list_mv");
	}

	// ***** CONNECTIONS EVENTS HELPER LIST *****
	else if( p->command() == "_events" ) { // Helper-list of everything in this component generated by hostNotify's
		bncs_stringlist sl;
		sl << "notify=*";		
		return sl.toString( '\n' );
	}

	// ***** CONNECTIONS COMMANDS HELPER LIST *****
	else if( p->command() == "_commands" ) { // Helper-list of any commands/parameters you might want to set at run-time
		bncs_stringlist sl;
		//sl << "myParam=[value]";
		return sl.toString( '\n' );
	}
	return "";
}

// timer events come here
void mv_select::timerCallback ( int id ) 
{
	switch( id )
	{
	case TIMER_SETUP:
		timerStop(id);
		break;

	default:	// Unhandled timer event
		timerStop(id);
		break;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// Callbacks above - Methods below ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

bncs_string mv_select::getCurrentMv() { // returns e.g. studio_b_mv_1 or empty string if no selection
	bncs_string strId;
	textGet("selectedindex", PNL_MAIN, "list_mv", strId);
	int intId = int(strId);
	debug(bncs_string("\nid: <%1> name: <%2>\n").arg(intId).arg(m_slAreaMvList[intId]));
	if (intId >= 0) {
		return m_slAreaMvList[intId];
	} else return "";
}

void mv_select::changeMvSelection(bncs_string s) {
	// s is the multiviewer instance e.g. studio_b_mv_1
	debug(bncs_string("\nMV: <%1>\n").arg(s));
	textPut("instance", s, PNL_MAIN, "multiviewer");	// tell the mv component
	textPut("instance", s, PNL_MAIN, "memory");			// tell the memory component
	m_strRouter = getObjectSetting(m_strMvSettings, "router");
	getDev(m_strRouter, &m_intRouterDev);
	textPut("instance", m_strRouter, PNL_MAIN, "sourceGrid");      // tell the source grid
	m_slLayoutList = getObjectSetting(s, "layout_list");
	debug(bncs_string("\nNumber of layouts: %1\n").arg(m_slLayoutList.count()));
	debug(bncs_string("<%1>\n").arg(m_slLayoutList));
	// now hide all the existing layout buttons.
	// I don't know how to delete them but they can be reused.
	bncs_stringlist slExistingButtons = getIdList(PNL_MAIN, "btnLayout");
	debug(bncs_string("\nNumber of controls: %1\n").arg(slExistingButtons.count()));
	for (int i = 0; i < slExistingButtons.count(); i++) {
		debug(bncs_string("Control %1: <%2>\n").arg(i).arg(slExistingButtons[i]));
		controlHide(PNL_MAIN, slExistingButtons[i]);
	}
	// create or reuse a button for each avilable layout
	for (int intButtonCount = 0; intButtonCount < m_slLayoutList.count(); intButtonCount++) {
		bncs_string strLayoutName = m_slLayoutList[intButtonCount].simplifyWhiteSpace();
		bncs_string strControlName = bncs_string("btnLayout_%1").arg(strLayoutName.lower().replace(' ', '_'));
		bool blnReuse = (slExistingButtons.find(strControlName) >= 0);
		createLayoutButton(strLayoutName, strControlName, intButtonCount, blnReuse);
	}
	m_intLayoutInfoSlot = int(getObjectSetting(getCurrentMv(), "layout_driver_offset")) + int(LAYOUT_SLOT);
	// register for layout changes
	infoRegister(m_intLayoutDriverDev, m_intLayoutInfoSlot, m_intLayoutInfoSlot);
	infoPoll(m_intLayoutDriverDev, m_intLayoutInfoSlot, m_intLayoutInfoSlot);
}

void mv_select::createLayoutButton(bncs_string strLayoutName, bncs_string strControlName, int intButtonCount, bool blnReuse) {
	int intRow = (int) intButtonCount / 2;
	int intColumn = intButtonCount % 2;
	int intStartX = STARTX + (intColumn * (SIZEX + SPACEX));
	int intStartY = STARTY + (intRow * (SIZEY + SPACEY));
	debug(bncs_string("\nRow: <%1> Col: <%2> Layout name: <%3> Control name: <%4> Reuse: %5\n")
			.arg(intRow).arg(intColumn).arg(strLayoutName).arg(strControlName).arg(blnReuse));
	if (blnReuse) {
		controlMove(PNL_MAIN, strControlName, intStartX, intStartY);
		controlShow(PNL_MAIN, strControlName);
	}
	else {
		controlCreate(PNL_MAIN, bncs_string(strControlName), "bncs_control",
				intStartX, intStartY, SIZEX, SIZEY, "textalign=bottom");
		textPut(bncs_string("text=%1").arg(strLayoutName), PNL_MAIN, strControlName);
		textPut(bncs_string("pixmap=%1").arg("/images/mv_layouts/%1.png")
				.arg(strLayoutName.lower().replace(' ', '_')), PNL_MAIN, strControlName);
	}
}
