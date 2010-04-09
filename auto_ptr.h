/*
 * Copyright (c) 1997-1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#ifndef __SGI_STL_MEMORY
#define __SGI_STL_MEMORY


template <class _Tp> class auto_ptr {
private:
	_Tp* _M_ptr;
public:
	explicit auto_ptr(_Tp* __p = 0): _M_ptr(__p) {}
	auto_ptr(auto_ptr& __a): _M_ptr(__a.release()) {}
	auto_ptr& operator=(auto_ptr& __a){
		if (&__a != this) {
			delete _M_ptr;
			_M_ptr = __a.release();
		}
		return *this;
	}
	
	~auto_ptr() { delete _M_ptr; }
	
	_Tp& operator*() const {
		return *_M_ptr;
	}

	_Tp* get() const  {
		return _M_ptr;
	}
	_Tp* release()  {
		_Tp* __tmp = _M_ptr;
		_M_ptr = 0;
		return __tmp;
	}
	void reset(_Tp* __p = 0)  {
		if (__p != _M_ptr) {
			delete _M_ptr;
			_M_ptr = __p;
		}
	}
};

#endif /* __SGI_STL_MEMORY */

