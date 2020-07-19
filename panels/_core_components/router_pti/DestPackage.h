// DestPackage.h: interface for the DestPackage class.
//
//////////////////////////////////////////////////////////////////////
#include <bncs_script_helper.h>

#if !defined(AFX_DESTPACKAGE_H__F7BFE2B8_C2C3_4E88_AA35_93BA6C8C77C7__INCLUDED_)
#define AFX_DESTPACKAGE_H__F7BFE2B8_C2C3_4E88_AA35_93BA6C8C77C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DestPackage  
{
public:
	int Index;
	bncs_string Name;
	DestPackage();
	virtual ~DestPackage();

};

#endif // !defined(AFX_DESTPACKAGE_H__F7BFE2B8_C2C3_4E88_AA35_93BA6C8C77C7__INCLUDED_)
