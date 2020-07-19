#include <windows.h>
#include <stdio.h>
#include <bncs_string.h>
#include <bncs_config.h>
#include "router_embedded_audio.h"

// this nasty little macro to make our class visible to the outside world
EXPORT_BNCS_SCRIPT( router_embedded_audio )

// constructor - equivalent to ApplCore STARTUP
router_embedded_audio::router_embedded_audio( bncs_client_callback * parent, const char * path ) : bncs_script_helper( parent, path )
{
	//initialise member vars
	m_intVideoDestIndex = 0;
	m_strVideoRouterInstance = "";
	m_intVideoRouterDevice = 0;
	
	m_strAudioRouterInstance = "";
	m_intAudioRouterDevice = 0;
	
	m_strPanel = PANEL_DEFAULT;
	m_strStudio = getWorkstationSetting("ops_area");
	m_intCurrentEmbedder = 0;
	m_blnLabelsVisible = true;

	initEmbedders();
	panelShow( PNL_MAIN, bncs_string("%1.bncs_ui").arg(m_strPanel) );
}

// destructor - equivalent to ApplCore CLOSEDOWN
router_embedded_audio::~router_embedded_audio()
{
	//delete all CEmbedder objects
	for (MAP_INT_EMBEDDER_ITERATOR it = m_mapEmbedders.begin(); it != m_mapEmbedders.end(); it++)
	{
		CEmbedder* emb = it->second;
		delete emb;
	}
	m_mapEmbedders.clear();
}

// all button pushes and notifications come here
void router_embedded_audio::buttonCallback( buttonNotify *b )
{
	//no buttons
}

// all revertives come here
int router_embedded_audio::revertiveCallback( revertiveNotify * r )
{
	debug("router_embedded_audio::revertiveCallback dev='%1' index='%2' sInfo='%3' info=%4", 
		r->device(), r->index(), r->sInfo(), r->info());
	
	//Handler for the destination's router
	if( r->device() == m_intVideoRouterDevice ) 
	{
		if( r->index() == m_intVideoDestIndex ) 
		{
			m_intCurrentVideoSource = r->info();
			
			//check which embedder if any this source relates to
			MAP_INT_INT_ITERATOR ito = m_mapVideoSourceEmbedderOutput.find(m_intCurrentVideoSource);
			if(ito != m_mapVideoSourceEmbedderOutput.end())
			{
				setCurrentEmbedder(ito->second);
			}
			else
			{
				setCurrentEmbedder(0);
			}
		}
	}
	else if( r->device() == m_intAudioRouterDevice )
	{
		//check if this destination is in the embedder inputs map
		MAP_INT_STRING_ITERATOR it = m_mapAudioDestEmbedderInput.find(r->index());
		if(it != m_mapAudioDestEmbedderInput.end())
		{
			bncs_stringlist sltEmbedderIndexInput = it->second;
			debug("router_embedded_audio::revertiveCallback    *found on embedder='%1' input='%2'", 
				sltEmbedderIndexInput[0], sltEmbedderIndexInput[1]);

			int intEmbedder = sltEmbedderIndexInput[0].toInt();
			int intInput = sltEmbedderIndexInput[1].toInt();

			//set the source tally for this multiviewer
			CEmbedder* emb = m_mapEmbedders[intEmbedder];
			if(emb){
				emb->setInputSource(intInput, r->info());
			}

			//is this multiviewer currently displayed on this monitor
			if(intEmbedder == m_intCurrentEmbedder)
			{
				bncs_string strSourceName;
				routerName(m_intAudioRouterDevice, 2, r->info(), strSourceName);
				textPut("text", strSourceName, PNL_MAIN, bncs_string("lblTally_%1").arg(intInput));
			}
		}
	}
	return 0;
}

// all database name changes come back here
void router_embedded_audio::databaseCallback( revertiveNotify * r )
{
}

// all parent notifications come here i.e. when this script is just one 
//  component of another dialog then our host might want to tell us things
bncs_string router_embedded_audio::parentCallback( parentNotify *p )
{
	//Property set commands
	if( p->command() == "instance" )
	{
		m_strVideoRouterInstance = p->value();
		debug("destMon::parentCallback (video) instance=%1", m_strVideoRouterInstance);
	}
	if( p->command() == "audio_instance" )
	{
		m_strAudioRouterInstance = p->value();
		debug("destMon::parentCallback audio_instance=%1", m_strAudioRouterInstance);
	}
	else if( p->command() == "video_dest_index" )
	{
		m_intVideoDestIndex = p->value().toInt();
		debug("destMon::parentCallback video_dest_index=%1", m_intVideoDestIndex);

	}
	else if( p->command() == "panel" )
	{
		if(p->value().length() >0){
			m_strPanel = p->value();
		}
		else
		{
			m_strPanel = PANEL_DEFAULT;
		}
		panelDestroy(PNL_MAIN);
		panelShow( PNL_MAIN, bncs_string("%1.bncs_ui").arg(m_strPanel) );
		debug("destMon::parentCallback panel=%1", m_strPanel);

		start();
	}

	//Return Commands
	else if( p->command() == "return" )
	{
		//Called by visual editor to persist settings in bncs_ui
		if(p->value() == "all")
		{
			return  
				bncs_string( "video_dest_index=" ) + bncs_string(m_intVideoDestIndex) + "\n" +
				bncs_string( "audio_instance=" ) + m_strAudioRouterInstance + "\n" +
				bncs_string( "panel=" ) + bncs_string(m_strPanel) + "\n";
		
		}

		//Called by parent to learn the components settings
		else if( p->value() == "getSettings" )
		{
			return bncs_string( "getSettings=" )
				+ bncs_string( "instance=" ) + m_strVideoRouterInstance + ","
				+ bncs_string( "audio_instance=" ) + m_strAudioRouterInstance + ","
				+ bncs_string( "video_dest_index=" ) + bncs_string(m_intVideoDestIndex);
		}
	}
	return "";
}

// timer events come here
void router_embedded_audio::timerCallback( int id )
{
}

void router_embedded_audio::start()
{
	//Get the video device ID for the configured video router instance
	getDev(m_strVideoRouterInstance, &m_intVideoRouterDevice);

	//Register and poll the monitor destination
	routerRegister(m_intVideoRouterDevice, m_intVideoDestIndex, m_intVideoDestIndex);
	routerPoll(m_intVideoRouterDevice, m_intVideoDestIndex, m_intVideoDestIndex);

	//Get the audio device ID for the configured audio router instance
	getDev(m_strAudioRouterInstance, &m_intAudioRouterDevice);

	initEmbedders();

}

void router_embedded_audio::initEmbedders()
{
	//now read in embedder_list
	bncs_stringlist sltEmbedderObjects = "";
	for (int intEmbedderIndex = 1; intEmbedderIndex < MAX_EMBEDDER_COUNT; intEmbedderIndex++)
	{
		bncs_string strEmbedderObject = getObjectSetting(m_strStudio + "_embedder_list", intEmbedderIndex);
		if(strEmbedderObject.length())
		{
			sltEmbedderObjects += strEmbedderObject;
			
//			<object id="stc_embedder_list">	
//				<item id="1" value="stc_embedder_1"/>
//				<item id="2" value="stc_embedder_2"/>
//			</object>	
//			<object id="stc_embedder_1">	
//				<item id="video_router" value="stc_sdi"/>
//				<item id="video_output" value="61"/>
//				<item id="audio_router" value="stc_nexus"/>
//				<item id="audio_input_1" value="349"/>
//				<item id="audio_input_2" value="350"/>
//				<item id="audio_input_3" value="351"/>
//				<item id="audio_input_4" value="352"/>
//				<item id="audio_input_5" value="353"/>
//				<item id="audio_input_6" value="354"/>
//				<item id="audio_input_7" value="355"/>
//				<item id="audio_input_8" value="356"/>
//			</object>	

			//get the router instance for this embedder
			bncs_string strVideoRouterIntance = getObjectSetting(strEmbedderObject, "video_router");
						
			//get the router output source on which the embedder source is presented
			int intSource = getObjectSetting(strEmbedderObject, "video_output").toInt();

			//create a new embedder object
			CEmbedder* emb = new CEmbedder(intSource);
			m_mapEmbedders.insert(make_pair(intEmbedderIndex, emb));
			
			//add to static map mapSourceEmbedderOutput key=source_index value=mv_index
			//used when a revertive arrives to update the tally control
			m_mapVideoSourceEmbedderOutput.insert(make_pair(intSource, intEmbedderIndex));

			for(int intInput = 1; intInput <= MAX_INPUTS; intInput++)
			{
				int intDest = getObjectSetting(strEmbedderObject, bncs_string("audio_input_%1").arg(intInput)).toInt();

				//add to a static map mapDestEmbedderInput key=dest_index value=embedder_index,input
				//used when a revertive arrives to update the current tally
				m_mapAudioDestEmbedderInput.insert( make_pair(intDest, bncs_string("%1,%2").arg(intEmbedderIndex).arg(intInput)) );
			
				//Register and poll the embedder destinations
				routerRegister(m_intAudioRouterDevice, intDest, intDest, true);
				routerPoll(m_intAudioRouterDevice, intDest, intDest);

			}
		}
		else
		{
			//last embedder reached
			break;
		}
	}
	debug("router_embedded_audio::initEmbedders list='%1'", sltEmbedderObjects.toString());
}

void router_embedded_audio::setCurrentEmbedder(int intEmbedder)
{
	//check if labels were hidden and are now to be shown
	if(intEmbedder > 0 && !m_blnLabelsVisible)
	{
		for(int intLabel = 0; intLabel < MAX_INPUTS; intLabel++)
		{
			//make labels visible
			controlShow(PNL_MAIN, bncs_string("lblTally_%1").arg(intLabel+1));
			controlShow(PNL_MAIN, bncs_string("lblLevel_%1").arg(intLabel+1));
		}
		m_blnLabelsVisible = true;
	}

	//check if labels are shown and are to be hidden
	if(intEmbedder == 0 && m_blnLabelsVisible)
	{
		for(int intLabel = 0; intLabel < MAX_INPUTS; intLabel++)
		{
			//hide labels
			controlHide(PNL_MAIN, bncs_string("lblTally_%1").arg(intLabel+1));
			controlHide(PNL_MAIN, bncs_string("lblLevel_%1").arg(intLabel+1));
		}
		m_blnLabelsVisible = false;
	}

	if(intEmbedder > 0)
	{
		updateTallyDisplay(intEmbedder);
	}

	//store current Embedder
	m_intCurrentEmbedder = intEmbedder;
}

void router_embedded_audio::updateTallyDisplay(int intEmbedder)
{
	for(int intInput = 1; intInput <= MAX_INPUTS; intInput++)
	{
		
		CEmbedder* emb = m_mapEmbedders[intEmbedder];
		if(emb){
			bncs_string strSourceName;
			routerName(m_intAudioRouterDevice, 2, emb->getInputSource(intInput), strSourceName);
			
			textPut("text", strSourceName, PNL_MAIN, bncs_string("lblTally_%1").arg(intInput));
		}
	}
}
