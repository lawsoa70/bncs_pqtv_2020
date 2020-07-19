
#if !defined(AFX_TIMER_H__9A63121B_3762_4665_89A8_3674A4613859__INCLUDED_)
#define AFX_TIMER_H__9A63121B_3762_4665_89A8_3674A4613859__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <map>
using namespace std;

/* Abstract base class to receive notifications from the timer class 
\sa timer*/
class timerCallbackClass
{
public:
	virtual void timerCallback( int timerId )=0;
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
class timer;

class timerClient
{
public:
	timerClient(){};
	timerClient( timerCallbackClass *c, int id, timer * owner )
	{
		m_callback = c;
		m_id = id;
		m_owner = owner;
	};


	timerCallbackClass * m_callback;
	int m_id;
	timer * m_owner;
};
#endif // DOXYGEN_SHOULD_SKIP_THIS

class timer  
{
public:
	timer( timerCallbackClass * c );
	timer();
	virtual ~timer();
	
	int timerStart( int id, int duration );
	int timerStop( int id );

	static map< int, timerClient > timers;

	void setCallback( timerCallbackClass * c );
private:
	timerCallbackClass * callback;
};

#endif // !defined(AFX_TIMER_H__9A63121B_3762_4665_89A8_3674A4613859__INCLUDED_)
