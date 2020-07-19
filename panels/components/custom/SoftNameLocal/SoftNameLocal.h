#ifndef SoftNameLocal_INCLUDED
	#define SoftNameLocal_INCLUDED

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
 
class SoftNameLocal : public bncs_script_helper
{
private:

#define MAXLABELS 500
#define STARTROUTERSLOT 0
#define STARTSNCSSLOT 2000

	bncs_string m_SoftNameInstance;

	int iLabelAutoDevice;
	int iRouterDevice;

	int iRouterSourceSize, iRouterDestinationSize;
	bncs_stringlist ssl_LocalSources, ssl_CameraSources, ssl_ExternalSources, ssl_LoopedSources;

	int iCurrentSource;
	int iCurrentIndex;
	int iCurrentQuickEditIndex;
	bool bPanelStarting;
	bool bCameraPanel;

	SoftNameData *allSoftData;

	int getDeviceNo( const bncs_string & );

	void registerForAutoRtrRevsAndPoll(void );
	void registerForAutoSncsRevsAndPoll(void );
	bool isSourceCamera( int iSource );
	bool isNOTExternalSource( int iSource );
	bool isNOTLoopedSource( int iSource );
	void updateLocalSourceData( int iSource );
	BOOL getAllRouterLabelText( int iSourceNum, bncs_string *szTextData );	
	BOOL getRouterLabelSoftName( int iSourceNum, bncs_string *szName, bncs_string *szForm );
	void ClearAllLocalSoftNames(void); 
	void ClearLocalSoftName(int iIndex); 
	void SaveLocalSoftName(int iSource); 
	void ProcessNewSoftName(int iIndex);
	void ResetPanel( void );
	void PopulateCameraPanel( void );
	void ChangeCurrentForm( int iSource  );

public:
	SoftNameLocal( bncs_client_callback * parent, const char* path );
	virtual ~SoftNameLocal();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

}; 

#endif // SoftNameLocal_INCLUDED