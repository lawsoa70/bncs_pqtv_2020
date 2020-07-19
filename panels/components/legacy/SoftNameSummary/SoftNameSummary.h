#ifndef SoftNameSummary_INCLUDED
	#define SoftNameSummary_INCLUDED

#include <bncs_script_helper.h>
#include "SoftNameData.h"

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class SoftNameSummary : public bncs_script_helper
{
private:

//#define MAXLABELS 250
#define STARTROUTERSLOT 0
#define STARTSNCSSLOT 2000

	bncs_string m_SoftNameInstance;

	int iLabelAutoDevice;
	int iRouterDevice;

	int iRouterSourceSize, iRouterDestinationSize;
	bncs_stringlist ssl_LocalSources;
	bncs_stringlist ssl_ExternalSources;
	bncs_stringlist ssl_LoopedSources;

	int iCurrentSource;
	int iCurrentIndex;
	BOOL bPanelStarting;

	SoftNameData *allSoftData;

	int getDeviceNo( const bncs_string & );
	void ResetPanel( void );

public:
	SoftNameSummary( bncs_client_callback * parent, const char* path );
	virtual ~SoftNameSummary();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

	void registerForAutoSncsRevsAndPoll(void );
	bool isSourceLocal( int iSource );
	bool isSourceExternal( int iSource );
	bool isSourceLooped( int iSource );

	void updateLocalSourceData( int iSrc );
	void fillLocalSourceData( void );

	BOOL getAllRouterLabelText( int iSourceNum, bncs_string *szTextData );	
	BOOL getAllSncsLabelText(  bncs_string szSncsName, bncs_string szTextData  );

};


#endif // SoftNameSummary_INCLUDED