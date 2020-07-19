// Multiviewer.cpp: implementation of the CMultiviewer class.
//
//////////////////////////////////////////////////////////////////////

#include "Multiviewer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiviewer::CMultiviewer(int intOutput)
{
	m_intOutput = intOutput;
}

CMultiviewer::~CMultiviewer()
{

}

int CMultiviewer::getOutput()
{	
	return m_intOutput;
}

void CMultiviewer::setInputSource(int intInput, int intSource)
{
	m_intCurrentSource[intInput - 1] = intSource;
}

int CMultiviewer::getInputSource(int intInput)
{
	return m_intCurrentSource[intInput - 1];
}
