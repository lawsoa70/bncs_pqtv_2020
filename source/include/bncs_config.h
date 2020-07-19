
#if !defined(AFX_BNCS_CONFIG_H__2AE549C3_AC8E_48DB_A1D1_D786DF7F1969__INCLUDED_)
#define AFX_BNCS_CONFIG_H__2AE549C3_AC8E_48DB_A1D1_D786DF7F1969__INCLUDED_
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef BNCS_CONFIG_EXPORTS
	#define DO_CONFIG_EXPORT __declspec(dllexport) 
#else 
	#define DO_CONFIG_EXPORT __declspec(dllimport) 
#endif

#include <bncs_string.h>  
 
class QDomDocument; 
class QDomNode;

// Note: the Qxxx classes are only referenced here as pointers
//  this saves anyone using this class having to have a Qt licence

class DO_CONFIG_EXPORT bncs_config  
{
public:
	bncs_config( const bncs_string & name );
	bncs_config( const bncs_config & c );
	bncs_config();
	bncs_config & operator=( const bncs_config & c );
	virtual ~bncs_config();
	bool isValid( void );
	bool isChildValid( void );
	bool nextChild( void);
	bncs_string childAttr( const bncs_string & name );
	bncs_string childTag( void );
	bncs_string attr( const bncs_string & name );
	bncs_string tag( void );
	void drillDown( void );
	bncs_string data( void );
	static QDomDocument * cachedDoc( const bncs_string & name );
	static bool isDirty( const bncs_string & name );
	static int changeSerial( const bncs_string & name );
	static bool save( void );
	static int incSerial( const bncs_string & name );
	static int decSerial( const bncs_string & name );
	static void setEnv( const bncs_string & env );
	static void clearCache();
private:
	QDomNode *parentNode;
	QDomNode *currentNode;
	static QDomDocument * loadDoc( const bncs_string & file );
	inline bool correctNodeForComments( QDomNode & d );
};

#endif // !defined(AFX_BNCS_CONFIG_H__2AE549C3_AC8E_48DB_A1D1_D786DF7F1969__INCLUDED_)
