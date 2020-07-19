#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "mapping.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( mapping )

// constructor - equivalent to ApplCore STARTUP
mapping::mapping( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_panel = "default";
	m_pasteMode = false;
}

// destructor - equivalent to ApplCore CLOSEDOWN
mapping::~mapping()
{
}

// all button pushes and notifications come here
void mapping::buttonCallback( buttonNotify *b )
{
	if( b->panel() == 1 )
	{
		if( b->id() == "list" )
		{
			textPut( b->command(), b->value(), 1, "grid" );
			textPut(b->command(),b->value(),1,"database_details");
		}
		else if( b->id() == "grid" )
		{
			if( b->value() == "released"  && !m_pasteMode)
			{
				textPut( "selected=next", 1, "list" );
			}
				textPut( "dirty", 1, "page" );
				textPut( "dirty", 1, "pagepage" );
			
				controlEnable( 1, "save" );
				controlEnable( 1, "cancel" );
			
		}
		else if( b->id() == "save" )
		{
			controlDisable( 1, "save" );
			controlDisable( 1, "cancel" );
			controlEnable( 1, "renamegroups" );
			controlEnable( 1, "rename" );

			textPut( "save", 1, "grid" );
			textPut( "save", 1, "page" );
			textPut( "save", 1, "pagepage" );

			textPut( "selected=none", 1, "list" );
		}
		else if( b->id() == "cancel" )
		{
			controlDisable( 1, "save" );
			controlDisable( 1, "cancel" );
			controlEnable( 1, "renamegroups" );
			controlEnable( 1, "rename" );

			textPut( "cancel", 1, "grid" );
			textPut( "cancel", 1, "page" );
			textPut( "cancel", 1, "pagepage" );

			textPut( "selected=none", 1, "list" );
		}
		else if( b->id() == "page" )
		{
			if( b->command() == "index" )
			{
				textPut( "page", b->value(), 1, "grid" );
				textPut("index",b->value(),1,"copypaste");

			}
			else if( b->command() == "dirty" )
			{
				controlEnable( 1, "save" );
				controlEnable( 1, "cancel" );
			}
			else if( b->command() == "name" )
			{
				m_pageName = b->value();
			}
		}
		else if( b->id() == "pagepage" )
		{
			if( b->command() == "index" )
			{
				textPut( "page", b->value(), 1, "page" );
				textPut( "button.1=released", 1, "page" );
			}
			else if( b->command() == "dirty" )
			{
				controlEnable( 1, "save" );
				controlEnable( 1, "cancel" );
			}
			else if( b->command() == "name" )
			{
				m_groupName = b->value();
			}
		}
		else if( b->id() == "rename" )
		{
			panelPopup( 2, "keyboard.bncs_ui" );
			textPut( "text", m_pageName, 2, "keyboard" );
			m_renamePage = true;
			textPut( "dirty", 1, "pagepage" );
			textPut("text","Rename Page",2,"rename");
		}
		else if( b->id() == "renamegroups" )
		{
			panelPopup( 2, "keyboard.bncs_ui" );
			textPut( "text", m_groupName, 2, "keyboard" );
			m_renamePage = false;
			textPut( "dirty", 1, "page" );
			textPut("text","Rename Group",2,"rename");
		}
		else if(b->id() == "copypaste")
		{
			if(b->command() == "paste")
			{
				m_pasteMode = true;
				bncs_stringlist slMapping(b->value());
				for(int i = 0; i < slMapping.count(); ++i)
				{
					textPut(bncs_string("button.%1=released").arg(i),1,"grid");
					textPut("index",slMapping[i],1,"grid");
				}
				m_pasteMode = false;
			}
		}
	}
	else if( b->panel() == 2 )
	{
		if( b->id() == "keyboard" )
		{
			bncs_string ret;

			textGet( "text", 2, "keyboard", ret );
			if(m_renamePage)
			{
				textPut( "rename", ret, 1, "page" );
				controlDisable( 1, "renamegroups" );
			}
			else
			{
				textPut( "rename", ret, 1, "pagepage" );
				controlDisable( 1, "rename" );
			}
		}
		else
		{
			textPut( "cancel", 1, "pagepage" );
			textPut( "cancel", 1, "page" );
			controlEnable( 1, "renamegroups" );
			controlEnable( 1, "rename" );
		}
		panelDestroy( 2 );
	}
}

// all revertives come here
int mapping::revertiveCallback( revertiveNotify * r )
{
	return 0;
}

// all database name changes come back here
void mapping::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string mapping::parentCallback( parentNotify *p )
{
	if( p->command() == "instance" )
	{
		m_instance = p->value();

		timerStart( 1, 1 );
	}
	if( p->command() == "panel" )
	{
		m_panel = p->value().lower().replace( ".bncs_ui", "" );

		timerStart( 1, 1 );
	}
	else if( p->command() == "return" )
	{
		bncs_stringlist sl;

		sl += "panel=" + m_panel;

		return sl.toString( '\n' );
	}
	return "";
}

// timer events come here
void mapping::timerCallback( int id )
{
	timerStop( id );
	init();

}

void mapping::init( void )
{
	panelDestroy( 1 );

	panelShow( 1, m_panel + ".bncs_ui" );
	setSize( 1 );

	controlDisable( 1, "save" );
	controlDisable( 1, "cancel" );

	if(m_panel.left(5).lower() == "audio")
	{
		textPut( "instance", "rtr_hydra_shared", 1, "grid" );
		textPut( "instance", "rtr_hydra_shared", 1, "list" );
		textPut( "instance", "rtr_hydra_shared", 1, "database_details" );
		
		textPut( "instance.map", m_instance, 1, "grid" );
		textPut( "instance.map", m_instance, 1, "page" );
		textPut( "instance", m_instance, 1, "pagepage" );
		textPut( "instance", m_instance, 1, "page" );
		textPut( "instance.map", m_instance, 1, "pagepage" );
		textPut( "instance", m_instance, 1, "copypaste" );
	}
	else
	{
		textPut( "instance", m_instance, 1, "grid" );
		textPut( "instance.map", m_instance, 1, "grid" );
		textPut( "instance", m_instance, 1, "page" );
		textPut( "instance.map", m_instance, 1, "page" );
		textPut( "instance", m_instance, 1, "pagepage" );
		textPut( "instance.map", m_instance, 1, "pagepage" );
		textPut( "instance", m_instance, 1, "list" );
		textPut( "instance", m_instance, 1, "database_details" );
		textPut( "instance", m_instance, 1, "copypaste" );
	}
	textPut( "button.1=released", 1, "pagepage" );
	textPut( "button.1=released", 1, "page" );
	
	textPut( "statesheet=enum_selected", 1, "sources" );
	textPut( "statesheet=enum_deselected", 1, "destinations" );
}


