#ifndef SoftNameSncs_INCLUDED
	#define SoftNameSncs_INCLUDED

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
 
class SoftNameSncs : public bncs_script_helper
{

#define STARTROUTERSLOT 0
#define STARTSNCSSLOT 2000
#define FINALSNCSSLOT 2201

	bncs_string m_SoftNameInstance;

	int iLabelAutoDevice;
	int iRouterDevice;

	int iRouterSourceSize, iRouterDestinationSize;
	bncs_stringlist ssl_ExternalSources;

	int iCurrentSource;
	int iCurrentIndex;
	int iCurrentListBox;
	bncs_string ssCurrentLabel;
	bncs_string ssCurrentAlias;
	bool bPanelStarting;
	bool bCreatingNewLabel;
	int iLabelCount;

	SoftNameData *allSoftData;

	int getDeviceNo( const bncs_string & );

	void registerForAutoRtrRevsAndPoll(void );
	void registerForAutoSncsRevsAndPoll(void );
	int isSourceExternal( int iSource );
	void ResetPanel( void );
	void DeleteSNCSLabel( bncs_string ssLabel  );
	void ClearSNCSLabel( bncs_string ssLabel  );
	void ClearAllSNCSLabels( void );
	void AssignSNCSLabel( bncs_string ssLabel  );
	void AssignNewLabel( void );
	void UpdateAllListBoxes(void);
	void KeyboardPanelPrep( void );
	void ResetSNCSLabel( bncs_string ssEditLabel );
	void ChangeCurrentForm( bncs_string ssLabel );
	void ClearAnySourceAssignedtoLabel( bncs_string ssLabel );
	void SoftNameSncs::PurgeUnusedSNCSLabels( void );

public:
	SoftNameSncs( bncs_client_callback * parent, const char* path );
	virtual ~SoftNameSncs();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
};


#endif // SoftNameSncs_INCLUDED