#ifndef bncs_string_included
	#define bncs_string_included

#ifdef BNCS_STRING_EXPORTS
	#ifndef NO_BNCS_STRING_EXPORTS
		#define DO_STRING_EXPORT __declspec(dllexport) 
//		#pragma message( "string exports" )
	#else
		#define DO_STRING_EXPORT 
//		#pragma message( "no string exports or imports" )
	#endif
#else 
	#ifndef NO_BNCS_STRING_EXPORTS
		#define DO_STRING_EXPORT __declspec(dllimport) 
//		#pragma message( "string imports" )
	#else
		#define DO_STRING_EXPORT 
//		#pragma message( "no string exports or imports 2" )
	#endif
#endif

#include <string>

class DO_STRING_EXPORT bncs_string  
{
	friend class bncs_stringlist;
private:
	char * buf;
	unsigned int len;
	bncs_string & decrementPlaceholders( void );
	bool iswhitespace( char c ) const;
public:
	// construction / destruction
	bncs_string( const char* in);
	bncs_string( void );
	bncs_string( const bncs_string & in );
	bncs_string( int in, char pad = 0, unsigned int padto =0 , int base = 10 );
	bncs_string( double in, int digits, char pad = 0, unsigned int padto = 0 );
	bncs_string( char in );
	bncs_string( const std::string & in);

// 	// Move constructor.
// 	bncs_string( bncs_string && other );

	virtual ~bncs_string();

	// operators
	const char operator[]( unsigned int pos ) const;
	char & operator[]( int pos );
	operator const char*() const;
	operator int() const;
	operator std::string() const;
	// operator double() has been removed as it causes ambiguity problems.
	// Use toDouble().
//	operator double() const;

	bncs_string & operator=( const char* in );
	bncs_string & operator=( const bncs_string &in );

// 	// Move assignment operator.
// 	bncs_string & operator=( bncs_string && other );

	friend DO_STRING_EXPORT const bncs_string operator+ ( const bncs_string & in, const bncs_string & in2 );
	friend DO_STRING_EXPORT const bncs_string operator+ ( const bncs_string & in, int in2);
	friend DO_STRING_EXPORT const bncs_string operator+ ( int in2, const bncs_string & in);
	friend DO_STRING_EXPORT const bncs_string operator+ ( const bncs_string & in, const char* in2);
	friend DO_STRING_EXPORT const bncs_string operator+ ( const char* in2, const bncs_string & in );

	bncs_string & operator+= ( const bncs_string & in );
	friend DO_STRING_EXPORT bool operator==( const bncs_string &, const bncs_string & );
	friend DO_STRING_EXPORT bool operator!=( const bncs_string &, const bncs_string & );

	friend DO_STRING_EXPORT bool operator==( const bncs_string & in1, const char* in2 );
	friend DO_STRING_EXPORT bool operator!=( const bncs_string & in1, const char* in2 );

	friend DO_STRING_EXPORT bool operator==( const char* in2, const bncs_string & in1);
	friend DO_STRING_EXPORT bool operator!=( const char* in2, const bncs_string & in1);
	
	friend DO_STRING_EXPORT bool operator==( const bncs_string & in1, int in2 );
	friend DO_STRING_EXPORT bool operator!=( const bncs_string & in1, int in2 );

	friend DO_STRING_EXPORT bool operator==( int in2, const bncs_string & in1 );
	friend DO_STRING_EXPORT bool operator!=( int in2, const bncs_string & in1 );
/*	technically correct but redundant as the above friend operators take care of all we need
	bool  operator!=( const bncs_string & in ) const;
	bool  operator!=( const char * in ) const;
	bool  operator==( const bncs_string & in ) const;
	bool  operator==( const char * in ) const;
*/
	// debug
	void dump( void );

	// misc
	bncs_string & append( const bncs_string & in );
	bncs_string & append( char in );
	bncs_string & append( int in, char pad = 0, int padto = 0, int base = 10 );

	int toInt( void ) const;
	int firstInt( void ) const;
	long toLong( void ) const;
	unsigned int toUInt( int base = 10 ) const;
	unsigned long toULong( int base = 10 ) const;
	double toDouble( void ) const;

	unsigned int length( void ) const;
	void truncate( unsigned int newlength );

	int find( char c, int index=0, bool cs=true ) const;
	int rfind( char c, int index=2147483647, bool cs=true ) const;
	int find( const bncs_string & s, int index=0, bool cs=true ) const;
	int contains( char c, bool cs=true ) const;
//	int contains( const bncs_string & substring, bool cs=true ) const;
	bool startsWith( const bncs_string & s) const;
	bool endsWith( const bncs_string & s) const;


	bncs_string left( unsigned int length ) const;
	bncs_string right( unsigned int length ) const;
	
	bncs_string mid( unsigned int start, unsigned int length ) const;

	bncs_string lower( void ) const;
	bncs_string upper( void ) const;
	bncs_string stripWhiteSpace( void ) const;
	bncs_string simplifyWhiteSpace( void ) const;

	bncs_string arg( const bncs_string & in )  const;
	bncs_string arg( const char* in  ) const;
	bncs_string arg( int in, char pad=0, unsigned int padto = 0, int base = 10 ) const;
	bncs_string arg( long in ) const;

	bncs_string & remove( unsigned int where, unsigned int howmany );
	bncs_string & insert( unsigned int where, const bncs_string & what );
	bncs_string & replace( const bncs_string & what, const bncs_string & with );
	bncs_string & replace( char what, char with );

	bncs_string & cleanup( bool BNCSi = true );
	int split( char delim, bncs_string & part1, bncs_string & part2 ) const;

	bncs_string justifyLeft( int howMany, char with ) const;
	bncs_string justifyRight( int howMany, char with ) const;

	friend DO_STRING_EXPORT bool operator<( const bncs_string & in1, const bncs_string & in2 );

	friend DO_STRING_EXPORT std::ostream & operator<<( std::ostream & os, const bncs_string & s );

// 	using std::string;
// 	friend DO_STRING_EXPORT string::string( const bncs_string & s );
};

#endif // bncs_string_included
  