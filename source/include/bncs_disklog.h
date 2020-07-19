// log.h: interface for the log class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__FEDBCE6D_E085_4C7B_BC86_943250FD9939__INCLUDED_)
#define AFX_LOG_H__FEDBCE6D_E085_4C7B_BC86_943250FD9939__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class bncs_disklog  
{
public:
	//! Format of log-file name.
	enum filetype
	{
		SINGLE /*! File-name as given. */ = 1, 
		TIMESTAMP_DAY /*! File-name is generated using date. */,
		TIMESTAMP_HOUR //! File-name is generated using date+hour.
	};
	bncs_disklog( const char * root, enum filetype mode, const char *msg1, const char *msg2=0, const char *msg3=0, const char *msg4=0, const char *msg5=0, const char *msg6=0 );
	bncs_disklog( const char * root, enum filetype mode = SINGLE );
	void write( const char *msg1, const char *msg2=0, const char *msg3=0, const char *msg4=0, const char *msg5=0, const char *msg6=0 );
	virtual ~bncs_disklog();
	const char * ref( void );
	void clear( void );
private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	void openLogFile( const char * root, enum filetype mode );
	void initVars( void );
	void createDirectory( const char* inpath );
	void cleanup( void );

	char m_ref[ 64 ];
	char m_root[ 32 ];
	HANDLE hFile;
	enum filetype m_mode;
	char filename[MAX_PATH];
	int lastTimePeriod;
#endif
};

#endif // !defined(AFX_LOG_H__FEDBCE6D_E085_4C7B_BC86_943250FD9939__INCLUDED_)
