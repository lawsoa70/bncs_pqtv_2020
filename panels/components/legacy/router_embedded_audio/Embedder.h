// Embedder.h: interface for the CEmbedder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMBEDDER_H__DCA7E743_4D7E_4E66_9CF9_406AE7F93C92__INCLUDED_)
#define AFX_EMBEDDER_H__DCA7E743_4D7E_4E66_9CF9_406AE7F93C92__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_INPUTS 8

class CEmbedder  
{
public:
	CEmbedder(int intOutput);
	virtual ~CEmbedder();

	int getInputSource(int intInput);
	int getOutput();
	void setInputSource(int intInput, int intSource);

private:
	int m_intOutput;
	int m_intCurrentSource[MAX_INPUTS];
};

#endif // !defined(AFX_EMBEDDER_H__DCA7E743_4D7E_4E66_9CF9_406AE7F93C92__INCLUDED_)
