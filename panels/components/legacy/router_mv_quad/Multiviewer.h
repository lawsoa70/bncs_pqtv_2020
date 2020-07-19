// Multiviewer.h: interface for the CMultiviewer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIVIEWER_H__E3B62C36_17C3_4803_86CB_853C9CC3637E__INCLUDED_)
#define AFX_MULTIVIEWER_H__E3B62C36_17C3_4803_86CB_853C9CC3637E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_INPUTS 4

class CMultiviewer  
{
public:
	int getInputSource(int intInput);
	CMultiviewer(int intOutput);
	virtual ~CMultiviewer();

	int getOutput();
	void setInputSource(int intInput, int intSource);

private:
	int m_intOutput;
	int m_intCurrentSource[MAX_INPUTS];
};

#endif // !defined(AFX_MULTIVIEWER_H__E3B62C36_17C3_4803_86CB_853C9CC3637E__INCLUDED_)
