// bncs_stringlist.h: interface for the bncs_stringlist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BNCS_STRINGLIST_H__308C68B0_E1E0_4DFE_9CCD_80DB90C813FA__INCLUDED_)
#define AFX_BNCS_STRINGLIST_H__308C68B0_E1E0_4DFE_9CCD_80DB90C813FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786)  

#include <bncs_string.h>


#ifdef BNCS_STRING_EXPORTS
	#define DO_STRINGLIST_EXPORT __declspec(dllexport) 
//	#pragma message( "string list exports" )
//	#define DO_EXTERN extern
#else 
	#define DO_STRINGLIST_EXPORT __declspec(dllimport) 
//	#pragma message( "string list imports" )
//	#define DO_EXTERN extern
#endif

using namespace std;

class DO_STRINGLIST_EXPORT bncs_string;


class bncs_stringlist;

class DO_STRINGLIST_EXPORT bncs_stringlistIterator
/// Const iterator for the bncs_stringlist
{
public:
	bncs_stringlistIterator( const bncs_stringlist * list = 0, bool end = false );
	bncs_string * operator->() const;
	bncs_string &operator*() const;
	bool operator==( const bncs_stringlistIterator & ) const;
	bool operator!=( const bncs_stringlistIterator & ) const;
	bncs_stringlistIterator operator++();	// prefix
	const bncs_stringlistIterator operator++ (int); // postfix
	bncs_stringlistIterator operator--();

private:
	int m_currentIndex;
	bool m_end;

	const bncs_stringlist *m_sl;
	static bncs_string m_safeReturn;
};

class bncs_stringlistPrivate;

class DO_STRINGLIST_EXPORT bncs_stringlist  
{
	friend class bncs_stringlistIterator;
public:
	/** Const iterator for the bncs_stringlist
	\par Example:
	\code
	for( bncs_stringlist::const_iterator it = sl.begin() ; it != sl.end() ; ++it )
	{
		printf( *it );
	}
	\endcode
	*/
	typedef bncs_stringlistIterator const_iterator;

	/** Iterator for the bncs_stringlist
	\par Example:
	\code
	for( bncs_stringlist::iterator it = sl.begin() ; it != sl.end() ; ++it )
	{
		printf( *it );
	}
	\endcode
	*/
	typedef bncs_stringlistIterator iterator;

	bncs_stringlist & operator=( const bncs_stringlist & in );
	
	bncs_stringlist();
	bncs_stringlist( const bncs_string & in, const char delim = ',' );
	bncs_stringlist( const bncs_stringlist & in );
	virtual ~bncs_stringlist();
	bncs_string & operator[]( int pos );
	operator const char*( void );
	int count( void ) const;
	bncs_string toString( const char delim = ',' );
//  REMOVED TO SAVE AMBIGUITY PROBLEMS WITH CHAR VARIANT
//	bncs_string toString( const bncs_string & delim );
	bncs_stringlist & fromString( const bncs_string & in, const char delim = ',' );
	bncs_stringlist & append( const bncs_string & in );
	bncs_stringlist & prepend( const bncs_string & in );
	bncs_stringlist & deleteItem( int pos );
	bncs_stringlist & replaceItem( const bncs_string &, int pos );
	bncs_stringlist & operator+( const bncs_string & );
	bncs_stringlist & operator+=( const bncs_string & );
	bncs_stringlist & operator+( const bncs_stringlist & in );
	bncs_stringlist & operator+=( const bncs_stringlist & in );
	bncs_stringlist & operator<<( const bncs_string & );
	bncs_stringlist & operator=( const bncs_string & );
	const iterator begin( void );
	const iterator end( void );
	const const_iterator begin( void ) const;
	const const_iterator end( void ) const;
	bncs_stringlist sort( bool asc = true ) const;
	int valueCount( const bncs_string & in, bool cs=false );
	int find( const bncs_string & in, bool cs=false );
	bncs_string getNamedParam( const bncs_string & in );
	void clear(); 
	void setCount( int, const bncs_string & def = "" );
private:
	bncs_stringlistPrivate *m_priv;
	bncs_string outputString;
};

#endif // !defined(AFX_BNCS_STRINGLIST_H__308C68B0_E1E0_4DFE_9CCD_80DB90C813FA__INCLUDED_)

