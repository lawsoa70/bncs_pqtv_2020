
#if !defined(AFX_GLOBALSETTINGS_H__B3E931E3_0EFB_44A7_B435_F7B67E89228F__INCLUDED_)
#define AFX_GLOBALSETTINGS_H__B3E931E3_0EFB_44A7_B435_F7B67E89228F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>
#include <stdlib.h>

class global 
{
public:
	global(){};
	virtual ~global(){};

public:
	static QString path( void ) 
	{
		return QString( getenv( "CC_ROOT" )) + "\\" + getenv( "CC_SYSTEM" );
	};

	static QString tempPath( void ) 
	{
		return QString( getenv( "TMP" )) + "\\BBCTechnology\\" + getenv( "CC_SYSTEM" );
	};

	static int ws( void ) 
	{
		return QString( getenv( "CC_WORKSTATION" )).toInt();
	};

	static QString rootPath( void ) 
	{
		return QString( getenv( "CC_ROOT" ));
	};
};

#endif // !defined(AFX_GLOBALSETTINGS_H__B3E931E3_0EFB_44A7_B435_F7B67E89228F__INCLUDED_)
