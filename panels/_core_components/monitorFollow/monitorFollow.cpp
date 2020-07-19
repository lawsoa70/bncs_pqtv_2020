#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "monitorFollow.h"

const bncs_string POS_XML("ops_positions");

const int PNL_MAIN(1);
const int PNL_STATUS(2);

const int TIMER_SETUP(1);
const int TIMER_BUTTON(2);
const int TIMER_BUTTON_DUR(1000);

const bncs_string BUTTON("button");
const bncs_string LABEL("label");
const bncs_string STATUS("status");
const bncs_string CLOSE("close");

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( monitorFollow )

// constructor - equivalent to ApplCore STARTUP
monitorFollow::monitorFollow( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	m_iMonitorDest = 0;
	m_iSelectedSource = -1;
	m_iSelectedDest = -1;
	m_iMonitoredSource = -1;
	m_iDevice = 0;
	m_iLockDevice = 0;
	m_isMonFollowing = false;
	m_isMonitorDest = false;
	m_isRouter = false;
	m_isComposite = false;
	m_isLabel = true;
	m_isStatus = true;
	m_isLocked = false;
	m_isValid = false;

	panelShow( PNL_MAIN, "default.bncs_ui" );
	//controlDisable( PNL_MAIN, BUTTON );
	textPut( "statesheet", "mon_notvalid", PNL_MAIN, BUTTON );
}

// destructor - equivalent to ApplCore CLOSEDOWN
monitorFollow::~monitorFollow()
{
}	

// all button pushes and notifications come here
void monitorFollow::buttonCallback( buttonNotify *b )
{
	b->dump("monitorFollow::buttonCallback");

	if ( b->panel() == PNL_MAIN && b->id() == BUTTON )
	{
		if ( b->value() == "pressed" && m_isStatus )
		{
			timerStart( TIMER_BUTTON, TIMER_BUTTON_DUR );
		}
		else if ( b->value() == "released" )
		{
			timerStop( TIMER_BUTTON );
			if ( !m_isLocked && m_iMonitorDest )
			{
				setMonFollowing( !m_isMonFollowing );
			}
		}
	}
	else if ( b->panel() == PNL_STATUS && b->id() == CLOSE )
	{
		panelDestroy( PNL_STATUS );
	}
}

// all revertives come here
int monitorFollow::revertiveCallback( revertiveNotify * r )
{
	r->dump("monitorFollow::revertiveCallback");
	//debug("monitorFollow::revertiveCallback()::dev %1, index %2, sInfo %3, Info %4, sInfo.firstint %5", r->device(), r->index(), r->sInfo(), r->info(),r->sInfo().firstInt());

	if ( r->device() == m_iDevice)
	{
		if(m_isMonFollowing)
		{
			if( m_isMonitorDest && r->index() == m_iSelectedDest && r->index() != m_iMonitorDest && !m_isLocked )	// check monitoring dests and index is selected dest and index is not monitoring dest (anti-wolf) and not locked
			{
				if ( m_isRouter )
				{
					routerCrosspoint(m_iDevice, r->info(), m_iMonitorDest);
				}
				else
				{
					infoWrite(m_iDevice, r->sInfo(), m_iMonitorDest);
				}
			}
		}
		if (r->index() == m_iMonitorDest)
		{
			if ( m_isRouter )
			{
				m_iMonitoredSource = r->info();
			}
			else
			{
				m_iMonitoredSource = r->sInfo().firstInt();
			}

			// update the label below the button  - mainly for debug as its often hidden on panels
			bncs_string szName;
			//routerName(m_iDevice, m_isMonitorDest ? 1 : 0, m_iMonitoredSource, szName);
			routerName(m_iDevice, 0, m_iMonitoredSource, m_labelText);
			//debug("monitorFollow::revertiveCallback()::dev %1, monitor %2, index %3, name %4", m_iDevice, m_isMonitorDest ? "dest1" : "sce0", m_iMonitoredSource, szName);
			m_labelText.replace("|", " ");
			updateLabel();
		}
	}
	else if( r->device() == m_iLockDevice)
	{
		if ((r->sInfo().toInt() == 0) || (r->sInfo() == NULL))
		{
			textPut("pixmap", "", PNL_MAIN, BUTTON);
			m_isLocked = false;
		}
		else
		{
			textPut("pixmap", "/images/icon_lock.png", PNL_MAIN, BUTTON);
			m_isLocked = true;
			setMonFollowing( false );
		}
	}

	return 0;
}

// all database name changes come back here
void monitorFollow::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
// component of another dialog then our host might want to tell us things
bncs_string monitorFollow::parentCallback( parentNotify *p )
{
	p->dump("monitorFollow::parentCallback");

	if (p->command() == "return")
	{
		bncs_stringlist sl;
		sl << bncs_string( "label=%1" ).arg( m_isLabel ? "enable" : "disable" );
		sl << bncs_string( "status=%1" ).arg( m_isStatus ? "enable" : "disable" );
		if (m_isLabel)
		{
			debug("monitorFollow::parentCallback: m_isLabel is TRUE");
		}
		else
		{
			debug("monitorFollow::parentCallback: m_isLabel is FALSE");
		}
		return sl.toString('\n');
	}
	else if( p->command() == "instance" )
	{
		if ( m_iDevice ) routerUnregister( m_iDevice );	// this *may* be a packager hence an infodriver but this command works just the same
		if ( m_iLockDevice ) infoUnregister( m_iLockDevice );

		m_iMonitorDest = 0;
		m_iDevice = 0;
		m_iLockDevice = 0;
		m_isValid = false;

		setInstance( p->value() );
		setPosition( getWorkstationSetting("ops_position") );
		registerDevice();
	}
	else if( p->command() == "source" )
	{
		m_isMonitorDest = false;
		m_iSelectedSource = p->value();

		if( m_isMonFollowing && m_iDevice && m_iSelectedSource > 0 && m_iMonitorDest > 0 && !m_isLocked )
		{
			monitorSource();
		}
		else
		{
			textPut("led", "false", PNL_MAIN, BUTTON);
		}
	}
	else if( p->command() == "dest" )
	{
		m_isMonitorDest = true;
		m_iSelectedDest = p->value();
		
		if( m_isMonFollowing && m_iDevice && m_iSelectedDest > 0 && m_iMonitorDest > 0 && !m_isLocked )
		{
			monitorDest();
		}
		else
		{
			textPut("led", "false", PNL_MAIN, BUTTON);
		}
	}
	else if( p->command() == "label" )
	{
		debug("monitorFollow::parentCallback: label value is %1",p->value());
		if ( p->value() == "enable" )
		{
			m_isLabel = true;
		}
		else
		{
			m_isLabel = false;
			controlHide( PNL_MAIN, LABEL );
		}
	}
	else if( p->command() == "status" )
	{
		timerStop( STATUS );
		if ( p->value() == "enable" )
		{
			m_isStatus = true;
		}
		else
		{
			m_isStatus = false;
		}
	}
	return "";
}

// timer events come here
void monitorFollow::timerCallback( int id )
{
	switch( id )
	{
	case TIMER_SETUP:	// setup routine
		timerStop(id);
		break;
		
	case TIMER_BUTTON:	// button press and hold
		timerStop(id);
		panelPopup( PNL_STATUS, "status.bncs_ui" );
		textPut( "add", bncs_string("Composite instance;%1").arg(m_isComposite?"YES":"NO"), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Script instance;%1").arg(m_scriptInstance), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Derived instance;%1").arg(m_instance), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Instance type;%1").arg(m_instanceType), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Valid;%1").arg(m_isValid?"YES":"NO"), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Instance device;%1").arg(m_isRouter?"ROUTER":"PACKAGER"), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Instance dev id;%1").arg(m_iDevice), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Instance mon dest;%1").arg(m_iMonitorDest), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Derived lock instance;%1").arg(m_lockInstance), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Lock instance dev id;%1").arg(m_iLockDevice), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Locked;%1").arg(m_isLocked?"YES":"NO"), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Following;%1").arg(m_isMonFollowing?"YES":"NO"), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Monitoring;%1").arg(m_isMonitorDest?"DEST":"SCE"), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Selected source;%1").arg(m_iSelectedSource), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Selected dest;%1").arg(m_iSelectedDest), PNL_STATUS, STATUS );
		textPut( "add", bncs_string("Monitored source;%1").arg(m_iMonitoredSource), PNL_STATUS, STATUS );
		break;
		
	default:	// Unhandled timer event
		timerStop(id);
		break;
	}
}



void monitorFollow::setPosition(bncs_string strPos)	//this finds the router and destination of monitor to route to
{
	debug("monitorFollow::setPosition(): looking for settings for position <%1>",strPos);

	bncs_config c( bncs_string("%1.%2").arg(POS_XML).arg(strPos) );

	if( c.isValid() )
	{
		while( c.isChildValid() )
		{
			if( c.childAttr("id").startsWith("mon_dest")  && c.childAttr("instance") == m_instance && c.childAttr("dest_index").toInt() > 0 )
			{
				m_iMonitorDest = c.childAttr("dest_index").toInt();
				debug("monitorFollow::setPosition(): found id <%1> for instance <%2> with dest_index <%3>",c.childAttr("id"),c.childAttr("instance"),m_iMonitorDest);
				break;
			}
			c.nextChild();
		}
	}
}

void monitorFollow::setInstance(bncs_string szInstance)
{
	// NEW: set instance that will automatically work out if the router is a packager or real router
	// Uses cInstance to also gain lock infodriver

	m_scriptInstance = szInstance;

	if ( bncs_config( bncs_string("instances.%1").arg(szInstance) ).attr("composite") == "yes" )
	{	// we are a composite instance
		m_isComposite = true;
		m_instance = bncs_config( bncs_string("instances.%1.router").arg(szInstance) ).attr("instance");
		m_lockInstance = bncs_config( bncs_string("instances.%1.lock").arg(szInstance) ).attr("instance");
		debug("monitorFollow::setInstance: We are Composite Instance Router <%1> and Lock <%2>",m_instance,m_lockInstance);
	}
	else
	{	// we are a non-composite instance
		// use legacy method of finding the instance and lock
		m_isComposite = false;
		m_instance = szInstance;
		if(m_instance.startsWith("packager_") && m_instance.endsWith("_router"))
		{
			m_lockInstance = m_instance;
			m_lockInstance.replace("_router", "_lock");
		}
		else
		{
			m_lockInstance = m_instance + "_lock";
		}
		debug("monitorFollow::setInstance: We are NON-Composite Instance Router <%1> and Lock <%2>",m_instance,m_lockInstance);
	}

	m_instanceType = bncs_config( bncs_string("instances.%1").arg(m_instance) ).attr("type");
	if ( m_instanceType == "PackagerAuto" )
	{
		m_isRouter = false;
		m_isValid = true;
	}
	else if ( m_instanceType == "Router" )
	{
		m_isRouter = true;
		m_isValid = true;
	}
	else
	{
		m_instanceType = "misnomer";
		m_isValid = false;
	}
	debug("monitorFollow::setInstance: We are a <%1>",m_instanceType);

	getDev( m_instance, &m_iDevice );
	getDev( m_lockInstance, &m_iLockDevice );
}

bool monitorFollow::registerDevice()
{
	if ( m_iLockDevice && m_iMonitorDest )
	{
		infoRegister( m_iLockDevice, m_iMonitorDest, m_iMonitorDest );
		debug("monitorFollow::registerDevice: registering dev %1 for lock device <%2>",m_iLockDevice,m_lockInstance);
		infoPoll( m_iLockDevice, m_iMonitorDest, m_iMonitorDest );
	}
	
	if( m_iDevice && m_iMonitorDest )
	{
		//controlEnable( PNL_MAIN, BUTTON );
		textPut( "statesheet", "mon_notfollowing", PNL_MAIN, BUTTON );

		if ( m_isRouter )
		{
			debug("monitorFollow::registerDevice: registering dev %1 for router <%2>",m_iDevice,m_instance);
			routerRegister(m_iDevice, m_iMonitorDest, m_iMonitorDest);
		}
		else
		{
			debug("monitorFollow::registerDevice: registering dev %1 for packager <%2>",m_iDevice,m_instance);
			infoRegister(m_iDevice, m_iMonitorDest, m_iMonitorDest);
		}

		infoPoll( m_iDevice, m_iMonitorDest, m_iMonitorDest );

		return true;
	}
	else
	{
		//controlDisable( PNL_MAIN, BUTTON );
		textPut( "statesheet", "mon_notvalid", PNL_MAIN, BUTTON );
		return false;
	}
}

bool monitorFollow::monitorSource()
{	// this does the registration and crosspoint for when we monitor a source
	if ( m_isRouter )
	{
		routerRegister(m_iDevice, m_iMonitorDest, m_iMonitorDest);
		routerCrosspoint(m_iDevice, m_iSelectedSource, m_iMonitorDest);
	}
	else
	{
		infoRegister(m_iDevice, m_iMonitorDest, m_iMonitorDest);
		infoWrite(m_iDevice, bncs_string("index=%1").arg(m_iSelectedSource), m_iMonitorDest);
	}
	
	textPut("led.align", "bottomleft", PNL_MAIN, BUTTON);
	textPut("colour.led", "yellow", PNL_MAIN, BUTTON);
	textPut("led", (m_iSelectedSource > 0)?"true":"false", PNL_MAIN, BUTTON);

	return m_isRouter;
}

bool monitorFollow::monitorDest()
{	// this does the registrations and poll for when we monitor a destination
	if ( m_isRouter )
	{
		routerRegister(m_iDevice, m_iMonitorDest, m_iMonitorDest);
		routerRegister(m_iDevice, m_iSelectedDest, m_iSelectedDest, true);
		routerPoll(m_iDevice, m_iSelectedDest, m_iSelectedDest);
	}
	else
	{
		infoRegister(m_iDevice, m_iMonitorDest, m_iMonitorDest);
		infoRegister(m_iDevice, m_iSelectedDest, m_iSelectedDest, true);
		infoPoll(m_iDevice, m_iSelectedDest, m_iSelectedDest);
	}
	
	textPut("led.align", "bottomright", PNL_MAIN, BUTTON);
	textPut("colour.led", "green", PNL_MAIN, BUTTON);
	textPut("led", (m_iSelectedDest > 0)?"true":"false", PNL_MAIN, BUTTON);

	return m_isRouter;
}

void monitorFollow::setMonFollowing(bool bFollow)
{
	m_isMonFollowing = bFollow;
	
	if( m_isMonFollowing )
		textPut( "statesheet", "mon_following", PNL_MAIN, BUTTON );
	else
		textPut( "statesheet", "mon_notfollowing", PNL_MAIN, BUTTON );
	
	if( m_isMonFollowing && m_iDevice && m_iMonitorDest > 0 )
	{
		if( !m_isLocked )
		{
			if ( m_isMonitorDest && m_iSelectedDest > 0 )
			{
				monitorDest();
			}
			if ( !m_isMonitorDest && m_iSelectedSource > 0 )
			{
				monitorSource();
			}
		}	
	}
	else
	{
		textPut("led", "false", PNL_MAIN, BUTTON);
	}
	updateLabel();
}

void monitorFollow::updateLabel(void)
{
	if (m_isMonFollowing)
	{
		textPut("text", m_labelText, PNL_MAIN, LABEL);
	}
	else
	{
		textPut("text", "", PNL_MAIN, LABEL);
	}
}