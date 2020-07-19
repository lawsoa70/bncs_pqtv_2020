// Embedder.cpp: implementation of the CEmbedder class.
//
//////////////////////////////////////////////////////////////////////

#include "Embedder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEmbedder::CEmbedder(int intOutput)
{
	m_intOutput = intOutput;

}

CEmbedder::~CEmbedder()
{

}

void CEmbedder::setInputSource(int intInput, int intSource)
{
	m_intCurrentSource[intInput - 1] = intSource;

}

int CEmbedder::getOutput()
{
	return m_intOutput;

}

int CEmbedder::getInputSource(int intInput)
{
	return m_intCurrentSource[intInput - 1];
}
