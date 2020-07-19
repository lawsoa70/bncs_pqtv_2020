// instanceInfo.h: interface for the instanceInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTANCEINFO_H__8CDF5EE9_5EAB_4C38_A048_9CD3801AFAA7__INCLUDED_)
#define AFX_INSTANCEINFO_H__8CDF5EE9_5EAB_4C38_A048_9CD3801AFAA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qstring.h>

#ifdef STYLESHEET_EXPORTS
	#define DOEXPORT __declspec(dllexport) 
#else 
	#define DOEXPORT __declspec(dllimport) 
#endif

#include <qmap.h>

#ifdef STYLESHEET_EXPORTS
template class DOEXPORT	QMap< QString, QString >;
#endif


class DOEXPORT instanceInfo  
{
public:
	instanceInfo();
	virtual ~instanceInfo();

public:	// variables
	int device;
	int offset;
	bool composite;
	QString type;
	QString complex;
	QString altId;
	QString location;
	QMap< QString, QString > compositeGroupList;
};

#endif // !defined(AFX_INSTANCEINFO_H__8CDF5EE9_5EAB_4C38_A048_9CD3801AFAA7__INCLUDED_)
