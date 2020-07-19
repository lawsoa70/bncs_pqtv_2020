// complexParamInfo.h: interface for the complexParamInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPLEXPARAMINFO_H__28B30F0B_E823_4B5E_86EB_8BF350A38B4D__INCLUDED_)
#define AFX_COMPLEXPARAMINFO_H__28B30F0B_E823_4B5E_86EB_8BF350A38B4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

#ifdef STYLESHEET_EXPORTS
	#define DOEXPORT __declspec(dllexport) 
#else 
	#define DOEXPORT __declspec(dllimport) 
#endif


class DOEXPORT complexParam
{
public:
	complexParam();
	virtual ~complexParam();

	QString instance;
	QString param;
};

#endif // !defined(AFX_COMPLEXPARAMINFO_H__28B30F0B_E823_4B5E_86EB_8BF350A38B4D__INCLUDED_)
