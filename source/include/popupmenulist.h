#include "qpopupmenu.h"
#include "qptrlist.h"
// menulist.h: interface for the menulist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_menuLIST_H__6EB13E8B_E2BA_457C_8B83_EC790953FAEC__INCLUDED_)
#define AFX_menuLIST_H__6EB13E8B_E2BA_457C_8B83_EC790953FAEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
#if defined(Q_TEMPLATEDLL)
Q_TEMPLATE_EXTERN template class  QPtrList<QPopupMenu>;
Q_TEMPLATE_EXTERN template class  QPtrListIterator<QPopupMenu>;
#endif
*/

class QPopupMenuList : public QPtrList<QPopupMenu>
{
public:
    QPopupMenuList() : QPtrList<QPopupMenu>() {}
    QPopupMenuList( const QPopupMenuList &list ) : QPtrList<QPopupMenu>(list) {}
   ~QPopupMenuList() { clear(); }
    QPopupMenuList &operator=(const QPopupMenuList &list)
        { return (QPopupMenuList&)QPtrList<QPopupMenu>::operator=(list); }
};

class Q_EXPORT QPopupMenuListIterator : public QPtrListIterator<QPopupMenu>
{
public:
    QPopupMenuListIterator( const QPopupMenuList &l )
        : QPtrListIterator<QPopupMenu>( l ) { }
    QPopupMenuListIterator &operator=( const QPopupMenuListIterator &i )
        { return (QPopupMenuListIterator&)
                QPtrListIterator<QPopupMenu>::operator=( i ); }
};

#endif // !defined(AFX_MenuLIST_H__6EB13E8B_E2BA_457C_8B83_EC790953FAEC__INCLUDED_)
