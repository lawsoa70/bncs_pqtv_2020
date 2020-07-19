// asyncPost.h: interface for the asyncPost class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASYNCPOST_H__3E0CE20F_7EE1_4483_A954_243842194797__INCLUDED_)
#define AFX_ASYNCPOST_H__3E0CE20F_7EE1_4483_A954_243842194797__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class asyncPost  
{
public:
	asyncPost( WPARAM wParam, LPARAM lParam, int len );
	virtual ~asyncPost();

	LPARAM lParam(void);
	WPARAM wParam(void);

private:
	WPARAM m_wp;
	LPARAM m_lp;
	char * m_mem;
};

#endif // !defined(AFX_ASYNCPOST_H__3E0CE20F_7EE1_4483_A954_243842194797__INCLUDED_)
