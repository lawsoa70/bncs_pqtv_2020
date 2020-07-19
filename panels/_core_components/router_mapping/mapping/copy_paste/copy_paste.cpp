#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "copy_paste.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( copy_paste )
#define TIMER_LOAD 1
#define TIMER_LOAD_DURATION 1

// constructor - equivalent to ApplCore STARTUP
copy_paste::copy_paste( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( 1, "copy_paste.bncs_ui" );

	// you may need this call to set the size of this component 
	//  if it's used in a popup window 
//	setSize( 1024,668 );		// set the size explicitly
//	setSize( 1 );				// set the size to the same as the specified panel

	m_instance = "";
	m_mapDB = 8;
	m_mapDBOffset = 0;
	m_pageIndex = -1;
	m_copyPageIndex = -1;
	m_iDevice = 0;
	controlDisable(1,"paste");
}

// destructor - equivalent to ApplCore CLOSEDOWN
copy_paste::~copy_paste()
{
}

// all button pushes and notifications come here
void copy_paste::buttonCallback( buttonNotify *b )
{
	if( b->panel() == 1 )
	{
		if(b->id() == "copy")
		{
			pageCopy();
		}
		else if(b->id() == "paste")
		{
			pagePaste();
		}
	}
}

// all revertives come here
int copy_paste::revertiveCallback( revertiveNotify * r )
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
void copy_paste::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string copy_paste::parentCallback( parentNotify *p )
{
	if( p->command() == "return" )
	{
		bncs_stringlist sl;
		
		sl << bncs_string("Mapping_DB_Number=%1").arg(m_mapDB);
		sl << bncs_string("Mapping_DB_Offset=%1").arg(m_mapDBOffset);
		
		return sl.toString( '\n' );
	}
	else if( p->command() == "Mapping_DB_Number" )
	{
		if(p->value().toInt() != m_mapDB)
		{
			m_mapDB = p->value().toInt();
			timerStart(TIMER_LOAD,TIMER_LOAD_DURATION);
		}
	}
	else if(p->command() == "Mapping_DB_Offset" )
	{
		if(p->value().toInt() != m_mapDBOffset)
		{
			m_mapDBOffset = p->value().toInt();
			timerStart(TIMER_LOAD,TIMER_LOAD_DURATION);
		}
	}
	else if(p->command() == "index")
	{
		if(p->value().toInt() != m_pageIndex)
		{
			m_pageIndex = p->value().toInt();
		}
	}
	else if(p->command() == "instance")
	{
		if(p->value() != m_instance)
		{
			m_instance = p->value();
			getDev(m_instance,&m_iDevice);
			timerStart(TIMER_LOAD,TIMER_LOAD_DURATION);
		}
	}
	return "";
}

// timer events come here
void copy_paste::timerCallback( int id )
{
	if(id == TIMER_LOAD)
	{
		timerStop(TIMER_LOAD);
		if(m_iDevice != 0 && m_mapDBOffset != -1 && m_mapDB != -1)
			controlEnable(1,"copy");
		else
			controlDisable(1,"copy");
	}
}

void copy_paste::pageCopy()
{
	if(m_pageIndex != -1)
	{
		m_copyPageIndex = m_pageIndex;
		textPut("statesheet","enum_selected",1,"copy");
		textPut("text", bncs_string("Copied|Page %1").arg(m_copyPageIndex),1,"copy");
		controlEnable(1,"paste");
	}
	else
		controlDisable(1,"paste");
}

void copy_paste::pagePaste()
{
	if(m_copyPageIndex != -1)
	{
	
		debug(bncs_string("copy_paste::pagePaste() iDevice:%1  iMapDB:%2 iMapOffset:%3 + m_copyPageIndex:%4").arg(m_iDevice).arg(m_mapDB).arg(m_mapDBOffset).arg(m_copyPageIndex));
		
		bncs_string mapping;
		routerName(m_iDevice,m_mapDB,m_mapDBOffset + m_copyPageIndex ,mapping);
		
		debug(bncs_string("copy_paste::pagePaste() Mapping:%1").arg(mapping));
		
		textPut("text", bncs_string("Copy").arg(m_copyPageIndex),1,"copy");
		textPut("statesheet","enum_deselected",1,"copy");
		controlDisable(1,"paste");
		hostNotify(bncs_string("paste=%1").arg(mapping));
	}
}