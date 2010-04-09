#ifndef AGG_POD_VECTOR
#define AGG_POD_VECTOR
#include "xmemory.h"
typedef unsigned char  int8u;  
//--------------------------------------------------------------���༭����pod_vector
//resize���������� m_capacity = new_size;

template<class T> class pod_vector
{
    public:
        typedef T value_type;
		
        ~pod_vector() { pod_allocator<T>::deallocate(m_array, m_capacity); }
        pod_vector() : m_size(0), m_capacity(0), m_array(0),m_granularity(10) {}
        pod_vector(unsigned cap, unsigned granularity=10);
		
        // Copying
        pod_vector(const pod_vector<T>&);
        const pod_vector<T>& operator = (const pod_vector<T>&);
		
        // Set new capacity. All data is lost, size is set to zero.
        void capacity(unsigned cap);
        unsigned capacity() const { return m_capacity; }
		
        // Allocate n elements. All data is lost, 
        // but elements can be accessed in range 0...size-1. 
        void allocate(unsigned size);
		
        // Resize keeping the content.
        void resize(unsigned new_size);
		
        void zero()
        {
            memset(m_array, 0, sizeof(T) * m_size);
        }
		
        void add(const T& v);
        void push_back( T& v)   { m_array[m_size++] = v; }
        void insert_at(unsigned pos, const T& val);
        void inc_size(unsigned size) { m_size += size; }
        unsigned size()      const   { return m_size; }
        unsigned byte_size() const   { return m_size * sizeof(T); }
        void serialize(int8u* ptr) const;
        void deserialize(const int8u* data, unsigned byte_size);
        const T& operator [] (unsigned i) const { return m_array[i]; }
		T& operator [] (unsigned i)       { return m_array[i]; }
        const T& at(unsigned i) const           { return m_array[i]; }
		T& at(unsigned i)                 { return m_array[i]; }
        T  value_at(unsigned i) const           { return m_array[i]; }
		
        const T* data() const { return m_array; }
		T* data()       { return m_array; }
		
        void remove_all()         { m_size = 0; }
        void clear()              { m_size = 0; }
        void cut_at(unsigned num) { if(num < m_size) m_size = num; }
		
		void usable(unsigned num);
    private:
        unsigned m_size;											//���г�Ա����
        unsigned m_capacity;										//�����������
		unsigned m_granularity;										//��������
        T*       m_array;
};
//------------------------------------------------------------------------���õ�ǰ���õĳ�Ա����
template<class T>
void pod_vector<T>::usable(unsigned num)
{
	m_size = num > m_capacity?m_capacity:num;
}
//------------------------------------------------------------------------������ʽ���ӳ�Ա
template<class T>
void pod_vector<T>::add(const T& v)
{
	if (m_size + 1 >= m_capacity) 
	{
		resize(m_capacity + m_granularity);
	}
	
	m_array[m_size++] = v;
}
//------------------------------------------------------------------------�������õ�ǰ�ڴ�����
template<class T> 
void pod_vector<T>::capacity(unsigned cap)
{
	m_size = 0;
	if(cap > m_capacity)
	{
		pod_allocator<T>::deallocate(m_array, m_capacity);
		m_capacity = cap;
		m_array = m_capacity ? pod_allocator<T>::allocate(m_capacity) : 0;
	}
}

//------------------------------------------------------------------------
template<class T> 
void pod_vector<T>::allocate(unsigned size)
{
	capacity(size);
	m_size = size;
}

//------------------------------------------------------------------------
template<class T> 
void pod_vector<T>::resize(unsigned new_size)
{
	if(new_size > m_size)
	{
		if(new_size > m_capacity)
		{
			T* data = pod_allocator<T>::allocate(new_size);
			memcpy(data, m_array, m_size * sizeof(T));
			pod_allocator<T>::deallocate(m_array, m_capacity);
			m_array = data;
			m_capacity = new_size;
		}
	}
	else
	{
		m_size = new_size;
	}
}

//------------------------------------------------------------------------
template<class T> pod_vector<T>::pod_vector(unsigned cap, unsigned granularity) :
m_size(0), 
m_capacity(cap), 
m_granularity(granularity),
m_array(pod_allocator<T>::allocate(m_capacity)) {}

//------------------------------------------------------------------------
template<class T> pod_vector<T>::pod_vector(const pod_vector<T>& v) :
m_size(v.m_size),
m_capacity(v.m_capacity),
m_granularity(v.m_granularity)
m_array(v.m_capacity ? pod_allocator<T>::allocate(v.m_capacity) : 0)
{
	memcpy(m_array, v.m_array, sizeof(T) * v.m_size);
}

//------------------------------------------------------------------------
template<class T> const pod_vector<T>& 
pod_vector<T>::operator = (const pod_vector<T>&v)
{
	allocate(v.m_size);
	if(v.m_size) memcpy(m_array, v.m_array, sizeof(T) * v.m_size);
	return *this;
}

//------------------------------------------------------------------------
template<class T> void pod_vector<T>::serialize(int8u* ptr) const
{ 
	if(m_size) memcpy(ptr, m_array, m_size * sizeof(T)); 
}

//------------------------------------------------------------------------
template<class T> 
void pod_vector<T>::deserialize(const int8u* data, unsigned byte_size)
{
	byte_size /= sizeof(T);
	allocate(byte_size);
	if(byte_size) memcpy(m_array, data, byte_size * sizeof(T));
}

//------------------------------------------------------------------------
template<class T> 
void pod_vector<T>::insert_at(unsigned pos, const T& val)
{
	if(pos >= m_size) 
	{
		m_array[m_size] = val;
	}
	else
	{
		memmove(m_array + pos + 1, m_array + pos, (m_size - pos) * sizeof(T));
		m_array[pos] = val;
	}
	++m_size;
}
#endif
