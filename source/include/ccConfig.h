/*************************************/
/* Written by David Yates            */
/* Copyright Siemens IT Systems 2007 */
/*************************************/
#if !defined(AFX_CCCONFIG_H__0A931BB2_1970_48D9_8C01_80C1813801B9__INCLUDED_)
#define AFX_CCCONFIG_H__0A931BB2_1970_48D9_8C01_80C1813801B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )

#include <string>
#include <map>
#include <list>
using namespace std;

class ccConfig  
{
public:
	// static functions are quickies meant to get the odd entry
	static void readConfigSection( const string & file, const string & section, map< string, string > & out );
	static void readConfigSectionRoot( const string & file, const string & section, map< string, string > & out );
	static void readConfigSection( int device, const string & section, map< string, string > & out );
	static void deleteKeysFromSectionRoot(const string & file, const string & section, const string & key);

	static void writeConfigSection( const string & file, const string & section, const map< string, string > & out );
	static void writeConfigSectionRoot( const string & file, const string & section, const map< string, string > & out );
	static void writeConfigSection( int device, const string & section, const map< string, string > & out );

	static string getString( const string & file, const string & section, const string &key, const string & def, bool createOnDefault=false  );
	static string getString( int device, const string & section, const string &key, const string & def, bool createOnDefault );
	static void setString( const string & file, const string & section, const string &key, const string & value );
	static void setString( int device, const string & section, const string &key, const string & value );
	static int getInt( const string & file, const string & section, const string &key, int def, bool createOnDefault=false );
	static int getInt( int device, const string & section, const string &key, int def, bool createOnDefault=false );
	static string getDevPath( void );
	static string getSystem( void );
	static string getDevPath( int device );
	static string getDevPath( const string & filename );
	static bool isV4Based( void );
	static bool isV3Based( void );
	static int workstation( void );

	static void sectionsAbs(const string & absFilename, list< string> & out);
	static void sections(const string & file, list< string> & out);
	static void sectionsRoot(const string & file, list< string> & out);
	static void sections(int device, list< string> & out);
	static void deleteSectionRoot(const string & file, const string & section);

	ccConfig();
	// these are the class based functions where we construct a config class to retrieve and access an entire section as key value pairs
	ccConfig( const string & file, const string & section );
	ccConfig( int device, const string & section );
	virtual ~ccConfig();
	string getString( const string & key, const string & def, bool createOnDefault=false );
	void setString( const string & key, const string & value );
	int getInt( const string & key, int def, bool createOnDefault=false );

	static bool developerMode( void );
	static string getLogPath(const string & dir, const string &file);

private:
	map< string, string > m_sectionMap;
	string m_file;
	string m_section;
	// helper function to get section with an absolute filename
	static void readConfigSectionAbs( const string & absFilename, const string & section, map< string, string > & out );
	static void writeConfigSectionAbs( const string & absFilename, const string & section, const map< string, string > & in );
};

#endif // !defined(AFX_CCCONFIG_H__0A931BB2_1970_48D9_8C01_80C1813801B9__INCLUDED_)
