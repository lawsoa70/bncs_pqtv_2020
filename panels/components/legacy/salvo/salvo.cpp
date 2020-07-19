#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "salvo.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( salvo )

#define PANEL_MAIN	1

#define TIMER_SETUP				1
#define TIMER_SETUP_DURATION	1

#define BTN_SALVO	"salvo"

// constructor - equivalent to ApplCore STARTUP
salvo::salvo( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_blnPackager = false;
	m_blnInfodriver = false;
	m_strLabel = "Salvo";
	m_strSalvoID = "";
	m_strDevice = "";
	m_intDevice = 0;
	//m_strPixmapActive = "";
	//m_strPixmapInactive = "";

	// show a panel from file p1.bncs_ui and we'll know it as our panel 1
	panelShow( PANEL_MAIN, "main.bncs_ui" );
}

// destructor - equivalent to ApplCore CLOSEDOWN
salvo::~salvo()
{
}

// all button pushes and notifications come here
void salvo::buttonCallback( buttonNotify *b )
{
	if( b->panel() == PANEL_MAIN )
	{
		if( b->id() == BTN_SALVO)
		{
			fire();
		}
	}
}

// all revertives come here
int salvo::revertiveCallback( revertiveNotify * r )
{
	debug("salvo::revertiveCallback() dev=%1 index=%2 value=%3", r->device(), r->index(), r->info());

	if( r->device() == m_intDevice)
	{
		int intSource = r->info();
		if(m_blnPackager)
		{
			bncs_stringlist sltRevertive = bncs_stringlist().fromString(r->sInfo());
			intSource = sltRevertive.getNamedParam("index").toInt();
		}

		int intItem = m_sltDests.find(bncs_string(r->index()));
		if(intItem > -1)
		{
			m_sltTally[intItem] = bncs_string(intSource);
			//debug("salvo::revertiveCallback() storing tally for dest=%1", r->index());
			checkTally();
		}
	}

	return 0;
}

// all database name changes come back here
void salvo::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string salvo::parentCallback( parentNotify *p )
{
	p->dump("salvo::parentCallback()::dump :");
	debug("salvo::PCB::command %1, subs %2, sub0 %3", p->command(), p->subs(),p->sub(0));
	if( p->command() == "return" )
	{
		bncs_stringlist sl;
		
		sl << "label=" + m_strLabel;
		sl << "salvo=" + m_strSalvoID;
		sl << "pixmap.active=" + m_strPixmapActive;
		sl << "pixmap.inactive=" + m_strPixmapInactive;

		return sl.toString( '\n' );
	}
	else if( p->command() == "label" )
	{
		m_strLabel = p->value();
		textPut("text", m_strLabel, PANEL_MAIN, BTN_SALVO);
	}
	else if (p->command() == "salvo")
	{
		m_strSalvoID = p->value();
		timerStart(TIMER_SETUP, TIMER_SETUP_DURATION);
	}
	else if (p->command() == "pixmap" && p->subs() == 1)
	{
		if (p->sub(0) == "active")
		{
			m_strPixmapActive = p->value();
			timerStart(TIMER_SETUP, TIMER_SETUP_DURATION);
		}
		else if (p->sub(0) == "inactive")
		{
			m_strPixmapInactive = p->value();
			timerStart(TIMER_SETUP, TIMER_SETUP_DURATION);
		}
	}
	else if (p->command() == "fire")
	{
		fire();
	}
	else if( p->command() == "_events" )
	{
		bncs_stringlist sl;
		
		sl << "fired";
		sl << "salvo_status=*";
		sl << "salvo_config=*";
		
		return sl.toString( '\n' );
	}
	else if( p->command() == "_commands" )
	{
		bncs_stringlist sl;
		
		sl << "fire";
		
		return sl.toString( '\n' );
	}	return "";
}

// timer events come here
void salvo::timerCallback( int id )
{
	switch (id)
	{
	case TIMER_SETUP:
		timerStop(id);
		load();
		break;

	default:	// Unhandled timer event
		timerStop(id);
		break;
	}

}

void salvo::fire()
{
	//Get Salvo
	bncs_config cfgSalvo = bncs_config(bncs_string("salvoes.%1").arg(m_strSalvoID));
	if (cfgSalvo.isValid())
	{
		debug("salvo::fire() Firing salvo - %1", m_strSalvoID);
		bncs_string strInstance = cfgSalvo.attr("instance");

		int intChildCount = 0;
		while( cfgSalvo.isChildValid())
		{
			bncs_string strSource = cfgSalvo.childAttr("source");
			bncs_string strDest = cfgSalvo.childAttr("dest");
			debug("salvo::fire() instance=%1 source=%2 dest=%3", strInstance, strSource, strDest);

			int intDevice = 0;
			getDev(strInstance, &intDevice);
			int intSource = strSource.toInt();
			int intDest = strDest.toInt();

			if(intDevice > 0 && intDest > 0)
			{
				if (m_blnPackager)
				{
					infoWrite(m_intDevice, bncs_string("index=%1").arg(intSource), intDest);
				}
				else if (m_blnInfodriver)
				{
					infoWrite(m_intDevice, bncs_string("%1").arg(intSource), intDest);
				}
				else
				{
					routerCrosspoint(intDevice, intSource, intDest);
				}
			}

			cfgSalvo.nextChild();
		}
	}
	else
	{
		debug("salvo::fire() Cannot find salvo - %1", m_strSalvoID);
	}

	hostNotify(bncs_string("fired=%1").arg(m_strSalvoID));
}

void salvo::load()
{
	m_sltSources.clear();
	m_sltDests.clear();
	m_sltTally.clear();

	if(m_strSalvoID.length() != 0)
	{
		//Load Salvo
		bncs_config cfgSalvo = bncs_config(bncs_string("salvoes.%1").arg(m_strSalvoID));
		if (cfgSalvo.isValid())
		{
			debug("salvo::load() Loading salvo - %1", m_strSalvoID);
			bncs_string strInstance = cfgSalvo.attr("instance");
			getDev(strInstance, &m_intDevice);
			bncs_string strType = bncs_config(bncs_string("instances.%1").arg(strInstance)).attr("type");
			debug("salvo::load() Instance is type - %1", strType);

			if (strType == "PackagerAuto")
			{
				m_blnPackager = true;
				m_blnInfodriver = false;
			}
			else if (strType == "RouterInfodriver")
			{
				m_blnPackager = false;
				m_blnInfodriver = true;
			}
			else
			{
				m_blnPackager = false;
				m_blnInfodriver = false;
			}

			int intCount = 0;
			while( cfgSalvo.isChildValid())
			{
				intCount++;
				bncs_string strSource = cfgSalvo.childAttr("source");
				bncs_string strDest = cfgSalvo.childAttr("dest");
				debug("salvo::load() instance=%1 source=%2 dest=%3", strInstance, strSource, strDest);

				m_sltSources.append(strSource);
				m_sltDests.append(strDest);
				m_sltTally.append("-1");

				int intDest = strDest.toInt();
				if (m_blnPackager)
				{
					debug("salvo::load() Packager register %1 %2", m_intDevice, intDest);
					infoRegister(m_intDevice, intDest, intDest, true);
					infoPoll(m_intDevice, intDest, intDest);
				}
				else if (m_blnInfodriver)
				{
					debug("salvo::load() Infodriver register %1 %2", m_intDevice, intDest);
					infoRegister(m_intDevice, intDest, intDest, true);
					infoPoll(m_intDevice, intDest, intDest);
				}
				else
				{
					debug("salvo::load() Router register %1 %2", m_intDevice, intDest);
					routerRegister(m_intDevice, intDest, intDest, true);
					routerPoll(m_intDevice, intDest, intDest);
				}

				cfgSalvo.nextChild();
			}
			hostNotify("salvo_config=CORRECT");
		}
		else
		{
			textPut("text", bncs_string("%1|Not Found").arg(m_strSalvoID), PANEL_MAIN, BTN_SALVO);
			hostNotify("salvo_config=INCORRECT");
		}

	}
	else
	{
		textPut("text", bncs_string("Salvo ID|Not Set").arg(m_strSalvoID), PANEL_MAIN, BTN_SALVO);
		hostNotify("salvo_config=INCORRECT");
	}
}

void salvo::checkTally()
{
	debug("salvo::checkTally()");
	bool blnActive = true;

	for(int intItem = 0; intItem < m_sltTally.count(); intItem++)
	{
		if(m_sltTally[intItem] != m_sltSources[intItem])
		{
			blnActive = false;
		}
	}

	if(blnActive)
	{
		hostNotify("salvo_status=ACTIVE");
		if (!m_strPixmapActive.length() || !m_strPixmapInactive.length())
		{
			textPut("statesheet", "salvo_active", PANEL_MAIN, BTN_SALVO);
		}
		else
		{
			textPut("pixmap", m_strPixmapActive, PANEL_MAIN, BTN_SALVO);
		}
	}
	else
	{
		hostNotify("salvo_status=INACTIVE");
		if (!m_strPixmapActive.length() || !m_strPixmapInactive.length())
		{
			textPut("statesheet", "salvo_inactive", PANEL_MAIN, BTN_SALVO);
		}
		else
		{
			textPut("pixmap", m_strPixmapInactive, PANEL_MAIN, BTN_SALVO);
		}
	}
}
