// except.h: interface for the except class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NNNNN_H__E24E73BA_2655_4B24_8BB5_4B39222C9313__INCLUDED_)
#define AFX_NNNNN_H__E24E73BA_2655_4B24_8BB5_4B39222C9313__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable : 4786 4996 4018)

#include <iostream>

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

using namespace std;

//void Log(char* szFmt, ...);


#include "windows.h"

class structured_exception
   {
public:
   structured_exception(EXCEPTION_POINTERS const &) throw();
   static void install() throw();
   unsigned what() const throw();
   void const *where() const throw();
private:
   void const *address_;
   unsigned code_;
   };


#endif // !defined(AFX_NNNNN_H__E24E73BA_2655_4B24_8BB5_4B39222C9313__INCLUDED_)
