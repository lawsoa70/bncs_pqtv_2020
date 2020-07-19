#ifndef router_embedded_audio_INCLUDED
	#define router_embedded_audio_INCLUDED
#pragma warning (disable:4786)
#include <map>

#include <bncs_script_helper.h>
#include "Embedder.h"

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif

typedef map<int, int> MAP_INT_INT;
typedef map<int, int>::iterator MAP_INT_INT_ITERATOR;

typedef map<int, bncs_string> MAP_INT_STRING;
typedef map<int, bncs_string>::iterator MAP_INT_STRING_ITERATOR;

typedef map<int, CEmbedder*> MAP_INT_EMBEDDER;
typedef map<int, CEmbedder*>::iterator MAP_INT_EMBEDDER_ITERATOR;

#define PNL_MAIN		1
#define PANEL_DEFAULT	"default"

#define MAX_EMBEDDER_COUNT	16

class router_embedded_audio : public bncs_script_helper
{
public:
	router_embedded_audio( bncs_client_callback * parent, const char* path );
	virtual ~router_embedded_audio();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

private:
	MAP_INT_INT m_mapVideoSourceEmbedderOutput;	//map to look up which embedder video output is on a video source
	MAP_INT_STRING m_mapAudioDestEmbedderInput;	//map to look up which embedder audio input is on an audio dest
	MAP_INT_EMBEDDER	m_mapEmbedders;		//map to hold embedder objects

	bncs_string m_strStudio;				//tv studio id
	bncs_string m_strPanel;					//the name of the panel layout to display
	bncs_string m_strVideoRouterInstance;	//the name of the video router instance
	int m_intVideoDestIndex;				//the index of the destination
	int m_intVideoRouterDevice;				//the dev id of the video router

	bncs_string m_strAudioRouterInstance;	//the name of the audio router instance
	int m_intAudioRouterDevice;				//the dev id of the audio router

	int m_intCurrentVideoSource;			//last video source routed to this dest
	int m_intCurrentEmbedder;

	bool m_blnLabelsVisible;			//flag to hold if labels are visible

	//Internal methods
	void start();				//called by the parent via a parantCallback "method"
	void setCurrentEmbedder(int intEmbedder);
	void initEmbedders();
	void updateTallyDisplay(int intEmbedder);

};


#endif // router_embedded_audio_INCLUDED