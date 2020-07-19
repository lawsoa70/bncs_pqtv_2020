#pragma warning( disable: 4786 )

#include <map>

/** 
\class bncs_simpleMap
\brief Map (Dict) of keys against values

Example using integer key, and integer value:
\code
	bncs_simpleMap< int, int> m;

	m.insert( 1, 22 );
	m.insert( 2, 11 );

	int a = m[ 1 ];			// a contains 22
	int b = m[ 2 ];			// b contains 11
	int c = m[ 9999999 ];	// c contains 0

	int i = m.count();		// i contains 2 (the number of items actually in the list
\endcode

Example using string key and string value:
\code
bncs_simpleMap< bncs_string, bncs_string > stringMap;

stringMap.set( "stuff", "nonsense" );
stringMap.set( "biff", "boff" );

bncs_string a = stringMap[ "stuff" ];	// a contains "nonsense"
bncs_string b = stringMap[ "biff" ];	// b contains "boff"
bncs_string c = stringMap[ "something" ];	// c contains "" - nothing nasty will happen with unexpected index values
\endcode

I've shown int and string, but you can use any class, or any combination of key or value types. If you implement your own classes you need to implement operator< 

This class is just a convenience wrapper for std::map class - you can use standard std::map iterators on it.

However, these are fussy, often the complexity not required so I have provided an "internal iterator" which is convenient to use:
\code
// so set the internal iterator to the start, keep going whilst it's still valid, go to the next() entry every time round
for (stringMap.start(); stringMap.valid(); stringMap.next())
{
	debug(stringMap.currentKey());
	debug(stringMap.current());
}
\endcode
*/

template <class Key, class Value> class bncs_simpleMap: public std::map< Key, Value >
{
public:
	/** Insert a string str with unique key
	\param[in] key unique key
	\param[in] i string to store - value is replaced if it already exists
	\returns true on item inserted
	*/
	bool set( const Key & key, const Value & v )
	{
		bool ret = false;
		auto ins = insert(pair<Key, Value>(key, v));

		if (ins.second)	// we did manage to insert an item
			ret = true;
		else
		{
			map<Key, Value>::iterator it = find(key);
			if (it != end())
				it->second = v;
		}
		return ret;
	};

	/** Does item of key exist?
	\param[in] key unique key
	\returns true on exists
	*/
	bool exists(const Key & key) const
	{
		map<Key, Value>::const_iterator it = find(key);
		if (it != end())
			return true;
		return false;
	};

	/** If the item exists return it, delete it, and let me know what happened
	\param[in] key unique key
	\param[out] existed set true by this function if the value existed
	\returns the value, or a null Value object
	*/
	Value take(const Key & key, bool & existed)
	{
		map<Key, Value>::iterator it = find(key);
		if (it != end())
		{
			existed = true;
			Value ret = it->second;
			erase(it);
			return ret;
		}
		existed = false;
		return Value();
	};

	/** Remove the value with key i
	\param[in] key unique key
	\returns true on removal
	*/
	bool remove(const Key & key)
	{
		map<Key, Value>::iterator it = find(key);
		if( it != end())
		{
			erase( it );
			return true;
		}
		return false;
	};

	/** return value at index i
	\param[in] key unique key
	\returns value at index i or null/default value if none exists
	*/
	Value & operator[](const Key & key)
	{
		static Value v;

		v = Value();
		map<Key, Value>::iterator it = find(key);
		{
			if (it != end())
				return it->second;
			return v;
		}
	};

	/** Return the number of items in the map
	\returns the number of items
	*/
	int count( void ) const
	{
		return size();
	};

	/** Set the internal iterator to the start of the collection
	*/
	void start(void)
	{
		it = begin();
	};

	/** Is the internal iterator pointing at a valid entry?
	\returns true if valid
	*/
	bool valid(void) const
	{
		return !(it == end());
	};

	/** Move the internal iterator to the next item
	\returns true if iterator pointing at a valid item, false if we reached the end of the list
	*/
	bool next(void)
	{
		if (it != end())
			++it;
		return !(it == end());
	};

	/** Get the current value (as pointed to by the internal iterator)
	\returns The value for the current item
	*/
	Value & current(void)
	{
		return it->second;
	};

	/** Get the current key (as pointed to by the internal iterator)
	\returns The value for the current item
	*/
	Key currentKey(void) const
	{
		return it->first;
	};

	/** Delete the current key/value (as pointed to by the internal iterator)
	\returns True if item deleted
	*/
	bool deleteCurrent(void)
	{
		if (it != end())
		{
			it = erase(it);
			return true;
		}
		return false;
	};

	/** Delete item of the given key (there can only be one)
	\returns True if item deleted
	*/
	bool deleteKey(Key k)
	{
		map<Key, Value>::iterator it = find(k);
		if (it != end())
		{
			it = erase(it);
			return true;
		}
		return false;
	};

	/** Delete item of the given value (there may be multiple)
	\returns The number of items deleted
	*/
	int deleteValue(const Value & v)
	{
		int i = 0;
		for (map<Key, Value>::iterator it = begin(); it != end(); /*increment in loop*/)
		{
			if (it->second == v)
			{
				it = erase(it);
				i++;
				continue;
			}
			else
				++it;
		}
		return i;
	};

private:
	iterator it;
};
