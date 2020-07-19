#ifndef mapping_INCLUDED
	#define mapping_INCLUDED

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
 
class mapping : public bncs_script_helper
{
public:
	mapping( bncs_client_callback * parent, const char* path );
	virtual ~mapping();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );

private:
	bncs_string m_instance;
	bncs_string m_panel;
	bncs_string m_pageName;
	bncs_string m_groupName;
	bool m_renamePage;

	bool m_pasteMode;

	void init( void );
};


#endif // mapping_INCLUDED 