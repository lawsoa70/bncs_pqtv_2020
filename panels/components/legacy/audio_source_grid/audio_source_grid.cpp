#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "audio_source_grid.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( audio_source_grid )

#define	PNL_MAIN		1

#define TIMER_DEFAULT	1
#define TIMER_SETUP		2

#define	SOURCE_PAGE_MAP_BASE	2000
#define	SOURCE_PAGE_NAME_BASE	2500

// constructor - equivalent to ApplCore STARTUP
audio_source_grid::audio_source_grid( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	//Initialise Variables
	m_intPageOffset = 0;
	m_strInstance = "";
	m_strLayout = "";
	m_intLastPage = -1;
	m_blnTellHost = true;

	//Set a timer to load a default UI if no configuration has been defined yet
	timerStart( TIMER_DEFAULT, 100 );
}

// destructor - equivalent to ApplCore CLOSEDOWN
audio_source_grid::~audio_source_grid()
{
}

// all button pushes and notifications come here
void audio_source_grid::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PNL_MAIN )
	{
		if( b->id() == "source_pages")
		{
			m_blnTellHost=true;
			m_intLastPage = b->value().toInt();
			loadPage(m_intLastPage, true);
		
//			textPut( "page", m_intLastPage, PNL_MAIN, "source_grid");
		}
		else if( (b->id() == "source_grid") && (b->command() == "index") && m_blnTellHost )
		{
			hostNotify( bncs_string("index=%1").arg(b->value().toInt()) );
				

			//Note the name of the source will also be needed for the tracebar
		}
		else
		{

			m_blnTellHost=true;
		}
	}
}

// all revertives come here
int audio_source_grid::revertiveCallback( revertiveNotify * r )
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
void audio_source_grid::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string audio_source_grid::parentCallback( parentNotify *p )
{
	// these are our settings
	if( p->command() == "instance" )
	{
		m_strInstance = p->value();
		timerStart( TIMER_SETUP, 1 );
	}	
	else if( p->command() == "page_offset" )
	{
		m_intPageOffset = p->value().toInt();
		timerStart( TIMER_SETUP, 1 );
	}
	else if( p->command() == "layout" )
	{
		m_strLayout = p->value();
		timerStart( TIMER_SETUP, 1 );
	}
	//Temp handler for previous parameter name and content
	else if( p->command() == "panel" )
	{
		if( p->value().lower().endsWith(".bncs_ui"))
		{
			m_strLayout = p->value().left( p->value().length() - 8 );
		}
		else
		{
			m_strLayout = p->value();
		}
		timerStart( TIMER_SETUP, 1 );

	}
	else if (p->command() == "index")
	{
		m_blnTellHost=false;
		loadPage(m_intLastPage, false);
	}
	// this is us being asked to return the set of values we want persisted
	else if( p->command() == "return" )
	{
		//Called by visual editor to persist settings in bncs_ui
		if(p->value() == "all")
		{
			bncs_stringlist sltParams;		// a means of returning the string
			sltParams += "layout=" + m_strLayout;
			sltParams += bncs_string( "page_offset=" ) + m_intPageOffset;
			return sltParams.toString( '\n' );	// return our stringlist as newline delimited single string
		}
	}
	else if(p->command() == "_events")
	{
		bncs_stringlist sl;

		sl << "index=<selected index>";

		return sl.toString('\n');
	}

	return "";
}

// timer events come here
void audio_source_grid::timerCallback( int id )
{
	switch (id)
	{
		case TIMER_DEFAULT:
			timerStop(TIMER_DEFAULT);
			panelShow( PNL_MAIN, "default.bncs_ui" );
			break;
		
		case TIMER_SETUP:
			timerStop(TIMER_SETUP);
			setupGrid();
			break;
	}
}

void audio_source_grid::setupGrid()
{
	//Load the layout
	bncs_string strLayoutFile = bncs_string("layouts/%1.bncs_ui").arg(m_strLayout);
	debug("router_source_grid::setupGrid() loading layout='%1'", strLayoutFile);
	textPut("text", strLayoutFile, PNL_MAIN, "1");
	panelShow(PNL_MAIN, strLayoutFile);

	//Set the instances
	textPut("instance", m_strInstance, PNL_MAIN, "source_pages");
	textPut("instance.map", m_strInstance, PNL_MAIN, "source_pages");

	textPut("instance", m_strInstance, PNL_MAIN, "source_pagepage");
	textPut("instance.map", m_strInstance, PNL_MAIN, "source_pagepage");
	
	textPut("instance", m_strInstance, PNL_MAIN, "source_grid");
	textPut("instance.map", m_strInstance, PNL_MAIN, "source_grid");

	//Set the mapping offsets
	textPut("namesoffset", SOURCE_PAGE_NAME_BASE + m_intPageOffset, PNL_MAIN, "source_pages");
	textPut("mapoffset", SOURCE_PAGE_MAP_BASE + m_intPageOffset, PNL_MAIN, "source_pages");
	textPut("mapoffset", m_intPageOffset, PNL_MAIN, "source_grid");

	//Assume the following non-default settings are configured on the source_pages control
	//notify.index=true
	//namesdb=8
	
	//Switch to page 1
	textPut("page", 1, PNL_MAIN, "source_pages");
}

void audio_source_grid::loadPage(int pageNumber, bool tellHost)
{
	textPut("page", pageNumber, PNL_MAIN, "source_grid");

}
