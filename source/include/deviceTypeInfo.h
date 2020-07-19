// deviceTypeInfo.h: interface for the deviceTypeInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEVICETYPEINFO_H__9D6B0FC6_7C07_42BF_9978_DA09CF6CD78F__INCLUDED_)
#define AFX_DEVICETYPEINFO_H__9D6B0FC6_7C07_42BF_9978_DA09CF6CD78F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <QString.h>
#include <qmap.h>
#include "paramInfo.h"

#ifdef STYLESHEET_EXPORTS
	#define DOEXPORT __declspec(dllexport) 
#else 
	#define DOEXPORT __declspec(dllimport) 
#endif

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class __declspec(dllexport) QMap< QString, paramInfo>;
// MOC_SKIP_END
#endif


class DOEXPORT deviceTypeInfo  
{
public:
	deviceTypeInfo();
	virtual ~deviceTypeInfo();

	
	enum driverType
	{
		none = 0,
		info=1,
		GPI,
		router
	};
public:
	enum driverType type;
	QMap< QString, paramInfo > params;
	int slotSpacing;

};

#endif // !defined(AFX_DEVICETYPEINFO_H__9D6B0FC6_7C07_42BF_9978_DA09CF6CD78F__INCLUDED_)
