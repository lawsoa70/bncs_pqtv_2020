// paramInfo.h: interface for the paramInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAMINFO_H__03ED4C84_EC49_4032_90FA_C8FCFA51576B__INCLUDED_)
#define AFX_PARAMINFO_H__03ED4C84_EC49_4032_90FA_C8FCFA51576B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef STYLESHEET_EXPORTS
	#define DOEXPORT __declspec(dllexport) 
#else 
	#define DOEXPORT __declspec(dllimport) 
#endif


class DOEXPORT paramInfo  
{
public:
	paramInfo();
	virtual ~paramInfo();
public: // variables
	int slot;
};

#endif // !defined(AFX_PARAMINFO_H__03ED4C84_EC49_4032_90FA_C8FCFA51576B__INCLUDED_)
