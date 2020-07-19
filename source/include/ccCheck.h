/*************************************/
/* Written by David Yates            */
/* Copyright Atos 2007 */
/*************************************/
#if !defined(AFX_CCCHECK_H__D895F3E1_2518_4A19_84C5_E1FF7230D416__INCLUDED_)
#define AFX_CCCHECK_H__D895F3E1_2518_4A19_84C5_E1FF7230D416__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <string>
using namespace std;

class ccCheck  
{
public:
	ccCheck( const string & header="" );
	virtual ~ccCheck();
public:
	void setHeader( const string & header );
	bool checkDevice(unsigned int device);
	bool checkIndex( unsigned int index );
	bool checkSource( int index );
	bool checkRange(unsigned int start, unsigned int end);
	bool checkIndexRange(unsigned int start, unsigned int end);
	bool checkDbIndexRange(unsigned int start, unsigned int end);
	bool checkDatabase(unsigned int db, unsigned int max);
	bool checkDatabase(unsigned int db);
	bool checkV1Database(unsigned int db);
	bool checkString( const string & );
	bool checkString(const string &, unsigned int);
	bool checkWorkstation(unsigned int ws);
	void log( const string & log );
	void setMaxIndex(unsigned int index);
	void setMaxSource(int source);
	void setMaxDatabase(unsigned int db);
	void setMaxDevice(unsigned int dev);
	void setMaxString(unsigned int size);
private:
	string m_header;
	unsigned int m_maxDevice;
	unsigned int m_maxIndex;
	unsigned int m_maxDatabase;
	int m_maxSource;
	unsigned int m_maxString;
};


#endif // !defined(AFX_CCCHECK_H__D895F3E1_2518_4A19_84C5_E1FF7230D416__INCLUDED_)
