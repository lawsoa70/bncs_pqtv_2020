/**************************/
/* Written by David Yates */
/* Copyright Atos 2013    */
/**************************/

#if !defined(AFX_DATABASE_H__7C26F6DD_FA16_41BB_8030_D82D3E3A4C02__INCLUDED_)
#define AFX_DATABASE_H__7C26F6DD_FA16_41BB_8030_D82D3E3A4C02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#ifdef _DEBUG
#pragma warning( disable : 4786) 
//#endif

#include <map>
#include <string>
using namespace std;

class database  
{
public:		// construction
	database(  const char * path, int device, int level );
	virtual ~database(){};

public:		// methods
	const char* getName( int index );
	bool getName( int index, char * name, int len );

	bool setName( int index, const char*, bool writeToFile = true, bool * changed = 0 );
	int getIndex( const char * name, int start = 0, int end = 0 );
	int getIndex( const char * name, bool caseInsensitive, bool substring );
	int size( void );

	bool exists( int index );

	bool dump(const char* path);
	bool loadDump(const char* path);
private:	// variables
	map< int, string > db;
	int m_device;
	int m_level;
	int m_maxSize;
	string m_filename;

private:	// member functions
	void load(  const char * path, int device, int level );
};

#endif // !defined(AFX_DATABASE_H__7C26F6DD_FA16_41BB_8030_D82D3E3A4C02__INCLUDED_)
