#include "qdockwindow.h"
#include "qptrlist.h"
// toolbarlist.h: interface for the toolbarlist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBARLIST_H__6EB13E8B_E2BA_457C_8B83_EC790953FAEC__INCLUDED_)
#define AFX_TOOLBARLIST_H__6EB13E8B_E2BA_457C_8B83_EC790953FAEC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
#if defined(Q_TEMPLATEDLL)
Q_TEMPLATE_EXTERN template class  QPtrList<QDockWindow>;
Q_TEMPLATE_EXTERN template class  QPtrListIterator<QDockWindow>;
#endif
*/

class QDockWindowList : public QPtrList<QDockWindow>
{
public:
    QDockWindowList() : QPtrList<QDockWindow>() {}
    QDockWindowList( const QDockWindowList &list ) : QPtrList<QDockWindow>(list) {}
   ~QDockWindowList() { clear(); }
    QDockWindowList &operator=(const QDockWindowList &list)
        { return (QDockWindowList&)QPtrList<QDockWindow>::operator=(list); }
};

class Q_EXPORT QDockWindowListIterator : public QPtrListIterator<QDockWindow>
{
public:
    QDockWindowListIterator( const QDockWindowList &l )
        : QPtrListIterator<QDockWindow>( l ) { }
    QDockWindowListIterator &operator=( const QDockWindowListIterator &i )
        { return (QDockWindowListIterator&)
                QPtrListIterator<QDockWindow>::operator=( i ); }
};

#endif // !defined(AFX_TOOLBARLIST_H__6EB13E8B_E2BA_457C_8B83_EC790953FAEC__INCLUDED_)
