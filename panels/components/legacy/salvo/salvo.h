#ifndef salvo_INCLUDED
	#define salvo_INCLUDED

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
 
class salvo : public bncs_script_helper
{
public:
	salvo( bncs_client_callback * parent, const char* path );
	virtual ~salvo();

	void buttonCallback( buttonNotify *b );
	int revertiveCallback( revertiveNotify * r );
	void databaseCallback( revertiveNotify * r );
	bncs_string parentCallback( parentNotify *p );
	void timerCallback( int );
	
private:
	bool m_blnPackager;
	bool m_blnInfodriver;
	void checkTally();
	bncs_stringlist m_sltSources;
	bncs_stringlist m_sltDests;
	bncs_stringlist m_sltTally;
	bncs_string m_strSalvoID;
	bncs_string m_strLabel;
	bncs_string m_strPixmapActive;
	bncs_string m_strPixmapInactive;

	bncs_string m_strDevice;
	int m_intDevice;

	void load();
	void fire();

};


#endif // salvo_INCLUDED