#ifndef X_MEMORY_H
#define X_MEMORY_H

template<class T> struct pod_allocator
{
	static T*   allocate(unsigned num)       { return new T [num]; }
	static void deallocate(T* ptr, unsigned num) { delete [] ptr;      }
};

template<class T> struct obj_allocator
{
	static T*   allocate()         { return new T; }
	static void deallocate(T* ptr) { delete ptr;   }
};

#endif
