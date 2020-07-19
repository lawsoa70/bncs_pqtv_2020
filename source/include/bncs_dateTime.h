#if !defined(AFX_BNCS_DATETIME_H__4CA966ED_5AE0_439D_9938_99A6FC3EAD87__INCLUDED_)
#define AFX_BNCS_DATETIME_H__4CA966ED_5AE0_439D_9938_99A6FC3EAD87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>

struct bncs_date
{
	int day;
	int month;
	int year;
	int dayOfWeek;
};

struct bncs_time
{
	int hour;
	int minute;
	int second;
	int millisecond;
};

class bncs_dateTime  
{
public:
	bncs_dateTime();
	virtual ~bncs_dateTime();

	bncs_dateTime( const bncs_dateTime & dt );
	bncs_dateTime( const SYSTEMTIME & st );
	bncs_dateTime( const FILETIME & ft );
	bncs_dateTime( const char * isoDateTime );
	
	static bncs_dateTime nowUTC( void );
	static bncs_dateTime now( void );
	const char* toString( const char* format = "dd MMM yyyy" );
	void toString( char * out, int len, const char* format = "dd MMM yyyy" );

	bool fromString( const char* in );
	bool fromStringTime( const char * inStr );
	bool fromStringDate( const char * inStr ); 

	bool addSeconds( int seconds );
	bool addMinutes( int minutes );
	bool addHours( int hours );
	bool addDays( int days );
	bool addWeeks( int weeks );
	bool addMonths( int months );
	bool addYears( int years );

	bool setDate( int day, int month, int year );
	bool setTime( int hour, int minute, int second, int millisecond = 0 );

	bool operator<( const bncs_dateTime & in ) const;
	bool operator<=( const bncs_dateTime & in ) const;
	bool operator>( const bncs_dateTime & in ) const;
	bool operator>=( const bncs_dateTime & in ) const;
	bool operator==( const bncs_dateTime & in ) const;
	bool operator!=( const bncs_dateTime & in ) const;

	__int64 difference( const bncs_dateTime & in ) const;
	__int64 difference_seconds(const bncs_dateTime & in) const;
	__int64 difference_milliseconds( const bncs_dateTime & in ) const;

	operator __int64() const; 
	void clear( void );
	bool isNull( void ) const;
	void dump( const char* ) const;

	bncs_time time( void ) const;
	bncs_date date( void ) const;
private:	// vars
	FILETIME m_ftDateTime;
	char m_outputString[ 64 ];
	bool m_valid;
private:	// funcs
	void insertString( char * p, int delCount, const char* str );
};


#endif // !defined(AFX_BNCS_DATETIME_H__4CA966ED_5AE0_439D_9938_99A6FC3EAD87__INCLUDED_)
