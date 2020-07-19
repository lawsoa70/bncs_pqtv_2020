/*************************************/
/* Written by David Yates            */
/* Copyright Siemens IT Systems 2008 */
/*************************************/
#if !defined(AFX_TIMER_H__9A63121B_3762_4665_89A8_3674A4613859__INCLUDED_)
#define AFX_TIMER_H__9A63121B_3762_4665_89A8_3674A4613859__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786) 

#include <windows.h>
#include <map>
using namespace std;


class ItimerCallback
{
public:
	virtual void timerCallback( int timerId )=0;
};


class client
{
public:
	client(){};
	client( ItimerCallback *c, int id )
	{
		m_callback = c;
		m_id = id;
	};


	ItimerCallback * m_callback;
	int m_id;
};


class timer  
{
	friend VOID CALLBACK timerProc( HWND hwnd,  UINT uMsg, UINT_PTR idEvent, DWORD dwTime );
public:
	timer( ItimerCallback * c );
	virtual ~timer();
	
	int timerStart( int id, int duration );
	int timerStop( int id );

private:
	static map< int, client > m_timers;
	ItimerCallback * m_callback;
};

#endif // !defined(AFX_TIMER_H__9A63121B_3762_4665_89A8_3674A4613859__INCLUDED_)
