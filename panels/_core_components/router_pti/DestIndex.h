// DestIndex.h: interface for the DestIndex class.
//
//////////////////////////////////////////////////////////////////////
#include <bncs_script_helper.h>

#if !defined(AFX_DESTINDEX_H__3C308F82_2AF7_4577_9D93_B5287C833284__INCLUDED_)
#define AFX_DESTINDEX_H__3C308F82_2AF7_4577_9D93_B5287C833284__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class DestIndex  
{
public:
	DestIndex();
	virtual ~DestIndex();

	int Index;
	int SourceIndexRouted;
	bncs_string Name;
};

#endif // !defined(AFX_DESTINDEX_H__3C308F82_2AF7_4577_9D93_B5287C833284__INCLUDED_)
