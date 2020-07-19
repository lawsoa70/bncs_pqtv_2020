/**************************/
/* Written by David Yates */
/* Copyright Atos 2013    */
/**************************/

#if !defined(AFX_DATABASEMGR_H__9773ADBC_FCB3_4432_AB7A_3B4F83AE186F__INCLUDED_)
#define AFX_DATABASEMGR_H__9773ADBC_FCB3_4432_AB7A_3B4F83AE186F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
//#pragma warning( disable : 4786) 
#endif

#include "database.h"
#include <windows.h>
#include <map>
using namespace std;


class databaseMgr  
{
public:	// construction
	databaseMgr( const char * path);
	databaseMgr();
	virtual ~databaseMgr();

public:	// members
	void setPath( const char * path );
	const char* getName( int device, int db, int index );
	bool getName( int device, int db, int index, char * name, int len );

	int getIndex( int device, int db, const char* name, int start = 0, int end = 0 );
	int getIndex( int device, int db, const char* name, bool caseInsensitive, bool substring );
	bool setName( int device, int db, int index, const char * name, bool writeToFile = true );

	bool exists( int device, int db, int start, int end );
	int numLoaded( void );
	const char * path( void );
	bool createDatabase(	const char* name, 
							int device, 
							int sizeDB0, 
							int sizeDB1, 
							int sizeDB2=0, 
							int sizeDB3=0, 
							int sizeDB4=0, 
							int sizeDB5=0, 
							int sizeDB6=0, 
							int sizeDB7=0, 
							int sizeDB8=0, 
							int sizeDB9=0);
	int size( int device, int db );
	int exists( int device, int db );
	void clear( int device = 0 );
private:	// variables
	map< int, database* > dbs;	
//	map< int, int > lastTimeUsed;		// use this to determine whether to drop the last used database to make room
	char m_path[ MAX_PATH ];

private:	// members
	database * doCreate( int device, int db );
	int WritePrivateProfileInt( const char* section, const char* entry, int value, const char* filename );
	database * getDatabaseObject( int device, int db, bool create = true );
};

#endif // !defined(AFX_DATABASEMGR_H__9773ADBC_FCB3_4432_AB7A_3B4F83AE186F__INCLUDED_)
