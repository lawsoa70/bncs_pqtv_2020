#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "source_grid.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( source_grid )

#define PANEL_MAIN	1

#define LAYOUT_DEFAULT	"grid_5x15x45"
#define TIMER_SET_PAGE_DELAY 1
#define TIMER_SET_PAGE_DELAY_DURATION 100

// constructor - equivalent to ApplCore STARTUP
source_grid::source_grid( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{

	m_strLayout = LAYOUT_DEFAULT;

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PANEL_MAIN, bncs_string("%1.bncs_ui").arg(m_strLayout) );

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
	//setSize( 1 );				// set the size to the same as the specified panel
}

// destructor - equivalent to ApplCore CLOSEDOWN
source_grid::~source_grid()
{
}

// all button pushes and notifications come here
void source_grid::buttonCallback( buttonNotify *b )
{
	//debug(bncs_string("source_grid::buttonCallback %1 %2 %3 Subs(%4 %5 %6)").arg(b->id()).arg(b->command()).arg(b->value()).arg(b->sub(0)).arg(b->sub(1)).arg(b->sub(2)));
	if( b->panel() == PANEL_MAIN )
	{
		if( b->id() == "sourceGrid" && b->command() == "index")
		{
			hostNotify( bncs_string("index=%1").arg(b->value()) );
		}
		else if(b->id() == "sourceGroup" && b->command() == "index")
		{
			hostNotify( bncs_string("group_index=%1").arg(b->value()) );
		}
		else if(b->id() == "sourcePage" && b->command() == "button" && b->value() == "released")
		{
			hostNotify( bncs_string("page_index=%1").arg(b->sub(0)) );
		}
	}
}

// all revertives come here
int source_grid::revertiveCallback( revertiveNotify * r )
{
/*	switch( r->device() )
	{
		case 123:
 			textPut( "text", r->sInfo(), 1, 3 );
			break;
	}
*/	return 0;
}

// all database name changes come back here
void source_grid::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string source_grid::parentCallback( parentNotify *p )
{
	debug("source_grid::parentCallback()  command=%1 value=%2", p->command(), p->value());

	if(p->command() == "instance" )
	{
		m_strInstance = p->value();
		textPut("instance", m_strInstance, PANEL_MAIN, "sourceGroup");
		textPut("instance", m_strInstance, PANEL_MAIN, "sourcePage");
		textPut("instance", m_strInstance, PANEL_MAIN, "sourceGrid");
		
		textPut("instance.map", m_strInstance, PANEL_MAIN, "sourceGroup");
		textPut("instance.map", m_strInstance, PANEL_MAIN, "sourcePage");
		textPut("instance.map", m_strInstance, PANEL_MAIN, "sourceGrid");
	}
	else if( p->command() == "return" )
	{
		bncs_stringlist sl;
		sl << "layout=" + m_strLayout;
		
		return sl.toString( '\n' );
	}
	else if( p->command() == "layout" )
	{
		m_strLayout = p->value();
		panelDestroy(PANEL_MAIN);
		panelShow( PANEL_MAIN, bncs_string("%1.bncs_ui").arg(m_strLayout));

		//Refresh instance targeting
		textPut("instance", m_strInstance, PANEL_MAIN, "sourceGroup");
		textPut("instance", m_strInstance, PANEL_MAIN, "sourcePage");
		textPut("instance", m_strInstance, PANEL_MAIN, "sourceGrid");

		textPut("instance.map", m_strInstance, PANEL_MAIN, "sourceGroup");
		textPut("instance.map", m_strInstance, PANEL_MAIN, "sourcePage");
		textPut("instance.map", m_strInstance, PANEL_MAIN, "sourceGrid");
	}
	else if( p->command() == "mapping" )
	{
		bncs_stringlist sltDatabases = bncs_stringlist().fromString(p->value());

		//Refresh mapping
		textPut("mapoffset", sltDatabases.getNamedParam("page_mapoffset"), PANEL_MAIN, "sourcePage");
		textPut("namesoffset", sltDatabases.getNamedParam("page_nameoffset"), PANEL_MAIN, "sourcePage");
		textPut("mapoffset", sltDatabases.getNamedParam("grid_mapoffset"), PANEL_MAIN, "sourceGrid");
		textPut("namesdb", sltDatabases.getNamedParam("grid_namesdb"), PANEL_MAIN, "sourceGrid");
	}
	else if(p->command() == "select_group")
	{
		textPut(bncs_string("button.%1=released").arg(p->value()),PANEL_MAIN,"sourceGroup");
	}
	else if(p->command() == "select_page")
	{
		//This is done to allow times for the group to be set before the page is set, as setting the group reset the page number
		timerStart(TIMER_SET_PAGE_DELAY,TIMER_SET_PAGE_DELAY_DURATION);
		m_currentPage = p->value();
	}
	else if(p->command() == "clear_selection")
	{
		textPut("index", "0",PANEL_MAIN, "sourceGrid");
	}
	else if(p->command() == "_events")
	{
		bncs_stringlist sl;

		sl << "index=<selected index>";
		sl << "group_index=<selected group>";
		sl << "page_index=<selected page>";
		sl << "clear_selection";

		return sl.toString('\n');
	}

	return "";
}

// timer events come here
void source_grid::timerCallback( int id )
{
	if(id == TIMER_SET_PAGE_DELAY)
	{
		timerStop(TIMER_SET_PAGE_DELAY);
		textPut(bncs_string("button.%1=released").arg(m_currentPage),PANEL_MAIN,"sourcePage");
		debug("m_currentPage:%1",m_currentPage);
	}
}
