#ifndef monitorFollow_INCLUDED
	#define monitorFollow_INCLUDED

#include <bncs_script_helper.h>

#ifdef WIN32
	#ifdef DOEXPORT_SCRIPT
		#define EXPORT_SCRIPT __declspec(dllexport) 
	#else
		#define EXPORT_SCRIPT __declspec(dllimport) 
	#endif
#else 
	#define EXPORT_SCRIPT
#endif
 
class monitorFollow : public bncs_script_helper
{
public:
	monitorFollow( bncs_client_callback * parent, const char* path );
	virtual ~monitorFollow();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
private:
	void setMonFollowing( bool bFollow );
	int m_iMonitoredSource;
	bool monitorDest();
	bool monitorSource();
	bool registerDevice( );
	void setInstance( bncs_string szInstance );
	void setPosition(bncs_string strPos);
	void updateLabel(void);

	bncs_string m_scriptInstance;	// instance name of the router as set in the script
	bncs_string m_instance;			// instance name of the router as derived
	bncs_string m_instanceType;		// instance type of the router	as derived
	bncs_string m_lockInstance;		// instance name of the router lock infodriver as derived
	bool m_isMonFollowing;			// true if we are monitor following
	bool m_isMonitorDest;			// true if we are monitoring destinations
	bool m_isComposite;				// true if we are a composite instance
	bncs_string m_area;
	bncs_string m_opsPosition;
	int m_iDevice;					// device number of the monitoring router/packager
	int m_iMonitorDest;				// destination index of the monitoring position
	int m_iSelectedSource;			// Sce number selected
	int m_iSelectedDest;			// Dest number selected
	int m_iLockDevice;				// device number of the monitoring lock infodriver
	bool m_isLocked;				// true if our monitoring destination is locked
	bool m_isRouter;				// true for real routers, false for a packager
	bool m_isValid;					// true for a valid router type
	bool m_isLabel;					// true if the label is enabled
	bool m_isStatus;				// true if the status popup is enabled
	bncs_string m_labelText;			// label text
};


#endif // monitorFollow_INCLUDED