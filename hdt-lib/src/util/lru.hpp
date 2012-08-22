/*
 * Implementation of an LRU cache with a maximum size.
 *
 * See http://code.google.com/p/lru-cache-cpp/ for usage and limitations.
 *
 * Licensed under the GNU LGPL: http://www.gnu.org/copyleft/lesser.html
 *
 * Pierre-Luc Brunelle, 2011
 * pierre-luc.brunelle@polytml.ca
 *
 */

#include <hashtable.h>
#include <sstream>
#include <cassert>

namespace lru {

//-------------------------------------------------------------
// Bucket
//-------------------------------------------------------------

template<class K, class V>
struct LRUCacheH4Value
{
	typedef std::pair<const K, LRUCacheH4Value<K, V> > Val;
	
	LRUCacheH4Value()
		: _v(), _older(NULL), _newer(NULL) { }
	
	LRUCacheH4Value(const V & v, Val * older, Val * newer)
		: _v(v), _older(older), _newer(newer) { } 
	
	V _v;
	Val * _older;
	Val * _newer;
};


//-------------------------------------------------------------
// Const Iterator
//-------------------------------------------------------------

template<class K, class V>
class LRUCacheH4ConstIterator
{
public:
	typedef std::pair<const K, LRUCacheH4Value<K, V> > Val;
	typedef LRUCacheH4ConstIterator<K, V> const_iterator;
	typedef Val & reference;
	typedef Val * pointer;
	
	enum DIRECTION {
		MRU_TO_LRU = 0,
		LRU_TO_MRU
	};
	
	LRUCacheH4ConstIterator(const Val * ptr = NULL, DIRECTION dir = MRU_TO_LRU);
	
	const_iterator & operator++();
    const_iterator operator++(int);
	
	bool operator==(const const_iterator & other);
	bool operator!=(const const_iterator & other);
	
	const K & key() const;
	const V & value() const;

private:
	const Val * _ptr;
	DIRECTION _dir;
};


template<class K, class V>
LRUCacheH4ConstIterator<K, V>::LRUCacheH4ConstIterator(
	const LRUCacheH4ConstIterator<K, V>::Val * ptr,
	LRUCacheH4ConstIterator<K, V>::DIRECTION dir)
		: _ptr(ptr), _dir(dir)
{
}


template<class K, class V>
LRUCacheH4ConstIterator<K, V> & LRUCacheH4ConstIterator<K, V>::operator++()
{
	assert(_ptr);
	_ptr = (_dir == LRUCacheH4ConstIterator<K, V>::MRU_TO_LRU ? _ptr->second._older : _ptr->second._newer);
	return *this;
}
	

template<class K, class V>
LRUCacheH4ConstIterator<K, V> LRUCacheH4ConstIterator<K, V>::operator++(int)
{
	const_iterator ret = *this;
	++*this;
	return ret;
}


template<class K, class V>
bool LRUCacheH4ConstIterator<K, V>::operator==(const const_iterator & other)
{
	return _ptr == other._ptr;
}
	

template<class K, class V>
bool LRUCacheH4ConstIterator<K, V>::operator!=(const const_iterator & other)
{
	return _ptr != other._ptr;
}


template<class K, class V>
const K & LRUCacheH4ConstIterator<K, V>::key() const
{
	assert(_ptr);
	return _ptr->first;
}


template<class K, class V>
const V & LRUCacheH4ConstIterator<K, V>::value() const
{
	assert(_ptr); 
	return _ptr->second._v;
}
	
	
//-------------------------------------------------------------
// LRU Cache
//-------------------------------------------------------------

template<class K, class V>
class LRUCacheH4
{
public:
	typedef LRUCacheH4ConstIterator<K, V> const_iterator;
	
public:
	LRUCacheH4(int maxsize);                    // Pre-condition: maxsize >= 1
	LRUCacheH4(const LRUCacheH4 & other);
	
	V & operator[](const K & key);
	void insert(const K & key, const V & value);
	
	int size() const;
	int maxsize() const;
	bool empty() const;
	
	const_iterator find(const K & key);         // updates the MRU
	const_iterator find(const K & key) const;   // does not update the MRU
	const_iterator mru_begin() const;           // from MRU to LRU
	const_iterator lru_begin() const;           // from LRU to MRU
	const_iterator end() const;
	
	void dump_mru_to_lru(std::ostream & os) const;

private:
	typedef std::pair<const K, LRUCacheH4Value<K, V> > Val;
	typedef __gnu_cxx::hashtable<Val, K, __gnu_cxx::hash<K>, std::_Select1st<Val>, std::equal_to<K> > MAP_TYPE;

private:
	Val * _update_or_insert(const K & key);
	Val * _update(typename MAP_TYPE::iterator it);
	Val * _insert(const K & key);

private:
	MAP_TYPE _map;
	Val * _mru;
	Val	* _lru;
    int _maxsize;
};


// Reserve enough space to avoid resizing later on and thus invalidate iterators
template<class K, class V>
LRUCacheH4<K, V>::LRUCacheH4(int maxsize)
	: _map(maxsize, __gnu_cxx::hash<K>(), std::equal_to<K>()),
	  _mru(NULL),
	  _lru(NULL),
	  _maxsize(maxsize)
{
	if (_maxsize <= 0)
		throw "LRUCacheH4: expecting cache size >= 1";
}


template<class K, class V>
LRUCacheH4<K, V>::LRUCacheH4(const LRUCacheH4<K, V> & other)
	: _map(other._map.bucket_count(), __gnu_cxx::hash<K>(), std::equal_to<K>()),
      _maxsize(other._maxsize),
	  _mru(NULL),
	  _lru(NULL)
{
	for (const_iterator it = other.lru_begin();  it != other.end();  ++it)
		this->insert(it.key(), it.value());
}


template<class K, class V>
V & LRUCacheH4<K, V>::operator[](const K & key)
{
	return _update_or_insert(key)->second._v;
}


template<class K, class V>
void LRUCacheH4<K, V>::insert(const K & key, const V & value)
{
	_update_or_insert(key)->second._v = value;
}


template<class K, class V>
int LRUCacheH4<K, V>::size() const
{
	return _map.size();
}
	
	
template<class K, class V>
int LRUCacheH4<K, V>::maxsize() const 
{
	return _maxsize;
}


template<class K, class V>
bool LRUCacheH4<K, V>::empty() const
{
	return size() > 0;
}


// updates MRU
template<class K, class V>
typename LRUCacheH4<K, V>::const_iterator LRUCacheH4<K, V>::find(const K & key)
{
	typename MAP_TYPE::iterator it = _map.find(key);

	if (it != _map.end())
		return const_iterator(_update(it), const_iterator::MRU_TO_LRU);
	else
		return end();
}


// does not update MRU
template<class K, class V>
typename LRUCacheH4<K, V>::const_iterator LRUCacheH4<K, V>::find(const K & key) const
{
	typename MAP_TYPE::iterator it = _map.find(key);
	
	if (it != _map.end())
		return const_iterator(&*it, const_iterator::MRU_TO_LRU);
	else
		return end();
}
	

template<class K, class V>
void LRUCacheH4<K, V>::dump_mru_to_lru(std::ostream & os) const
{
	os << "LRUCacheH4(" << size() << "/" << maxsize() << "): MRU --> LRU: " << std::endl;
	for (const_iterator it = mru_begin();  it != end();  ++it)
		os << it.key() << ": " << it.value() << std::endl;
}


template<class K, class V>
typename LRUCacheH4<K, V>::const_iterator LRUCacheH4<K, V>::mru_begin() const
{
	return const_iterator(_mru, const_iterator::MRU_TO_LRU);
}


template<class K, class V>
typename LRUCacheH4<K, V>::const_iterator LRUCacheH4<K, V>::lru_begin() const
{
	return const_iterator(_lru, const_iterator::LRU_TO_MRU);
}


template<class K, class V>
typename LRUCacheH4<K, V>::const_iterator LRUCacheH4<K, V>::end() const
{
	return const_iterator();
}


template<class K, class V>
typename LRUCacheH4<K, V>::Val * LRUCacheH4<K, V>::_update_or_insert(const K & key)
{
	typename MAP_TYPE::iterator it = _map.find(key);
	if (it != _map.end())
		return _update(it);
	else
		return _insert(key);
}


template<class K, class V>
typename LRUCacheH4<K, V>::Val * LRUCacheH4<K, V>::_update(typename MAP_TYPE::iterator it)
{
	LRUCacheH4Value<K, V> & v = it->second;
	Val * older = v._older;
	Val * newer = v._newer;
	Val * moved = &*it;
	
	// possibly update the LRU
	if (moved == _lru && _lru->second._newer)
		_lru = _lru->second._newer;
	
	if (moved != _mru) {
		// "remove" key from current position
		if (older)
			older->second._newer = newer;
		if (newer)
			newer->second._older = older;
		
		// "insert" key to MRU position
		v._older = _mru;
		v._newer = NULL;
		_mru->second._newer = moved;
		_mru = moved;
	}
	
	return moved;
}


template<class K, class V>
typename LRUCacheH4<K, V>::Val * LRUCacheH4<K, V>::_insert(const K & key)
{
	// if we have grown too large, remove LRU
	if (_map.size() >= _maxsize) {
		Val * old_lru = _lru;
		if (_lru->second._newer) {
			_lru = _lru->second._newer;
			_lru->second._older = NULL;
		}
		_map.erase(old_lru->first);
	}
	
	// insert key to MRU position
	std::pair<typename MAP_TYPE::iterator, bool> ret
		= _map.insert_unique(Val(key, LRUCacheH4Value<K, V>(V(), _mru, NULL)));
	Val * inserted = &*ret.first;
	if (_mru)
		_mru->second._newer = inserted;
	_mru = inserted;
	
	// possibly update the LRU
	if (!_lru)
		_lru = _mru;
	else if (!_lru->second._newer)
		_lru->second._newer = _mru;
	
	return inserted;
}


}  // namespace lru
