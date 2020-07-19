#if !defined(AFX_DECODECAPTIONSTRING_H__E3CC3545_2B29_4323_9D26_7AF0057EB3D5__INCLUDED_)
#define AFX_DECODECAPTIONSTRING_H__E3CC3545_2B29_4323_9D26_7AF0057EB3D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifdef _WIN32
	#include <windows.h>
#endif
#include <stdio.h>

#define SUBS 32

class decodeCaptionString
{
public:
	decodeCaptionString( const char * );
	~decodeCaptionString();
	decodeCaptionString(const decodeCaptionString & in);
/** Pointer to the "command" part of the string */
	const char *command;
/** Pointer list to the sub elements of the string */
	const char *sub[ SUBS ];
/** Pointer to the value part of the string*/
	const char *value;
/** The number of sub elements from the input string */
	int subs;
/** true if the data in this class is valid */
	bool valid; 
#ifdef _WIN32
	void dump( void );
	char * toString( char * );
#endif
private:
	char *copy;
	void split(const char * in);
};

#endif // !defined(AFX_DECODECAPTIONSTRING_H__E3CC3545_2B29_4323_9D26_7AF0057EB3D5__INCLUDED_)
