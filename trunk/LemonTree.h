// LemonTree.h: interface for the LemonTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_LEMON_TREE_H_)
#define _LEMON_TREE_H_

#include "pod_vector.h"

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
author:wrr
多叉树
 -*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*-*/
template <class T> class LemonTree;


template<class T>
class Lemon
{
public:
	Lemon(T data):m_ichild(0),parent(0),child(0),m_idepth(0)
	{
		m_Kernel = data;
	}
	Lemon():m_ichild(0),parent(0),child(0),m_idepth(0)
	{
	}
	~Lemon();
	//为什么这个拷贝构造函数无效???
	Lemon( Lemon& le)
	{
		m_Kernel = le.m_Kernel;
		parent = le.parent;
		child = le.child;
		m_ichild = le.m_ichild;
	}
	void clear(void);
	void add_child(T& data);
	void edit_data(T& data);
	int child_num(void){return m_ichild;}
	int depth(void){return m_idepth;}
	Lemon<T>& operator = (T& data);
	Lemon<T>& operator = (Lemon<T>& data);
	void	  operator << (Lemon<T>& data);
	Lemon<T>* parent;
	Lemon<T>* child;
	int		m_ichild;
	int		m_idepth;
	T		m_Kernel;
};
template<class T>
Lemon<T>::~Lemon()
{
	if(child)
	{
		delete[] child;
	}
}

template<class T>
Lemon<T>& Lemon<T>::operator =(T& data)
{
	m_Kernel = data;
	return *this;
}
template<class T>
Lemon<T>& Lemon<T>::operator = (Lemon<T>& le)
{
	m_Kernel = le.m_Kernel;
	parent = le.parent;
	delete child;
	child = le.child;
	le.child = NULL;
	m_ichild = le.m_ichild;
	m_idepth = le.m_idepth;
	return *this;
}
template<class T>
void Lemon<T>::clear(void)
{
}

template<class T>
void Lemon<T>::edit_data(T& data)
{
	m_Kernel = data;
}
template<class T>
void Lemon<T>::operator << (Lemon<T>& le)
{
	m_Kernel << le.m_Kernel;
	parent = le.parent;
	delete child;
	child = le.child;
	le.child = NULL;
	m_ichild = le.m_ichild;
	m_idepth = le.m_idepth;
}

template<class T>
void Lemon<T>::add_child(T& data)
{
	if (child == NULL)
	{
		child = new Lemon[1];
		child->parent = this;
		child->m_idepth = m_idepth + 1;
		*child = data;
		m_ichild = 1;
	}
	else
	{
		Lemon<T>* p = new Lemon<T>[m_ichild + 1];
		ASSERT(p);
		for(int i = 0; i < m_ichild; i++)
			p[i] << child[i];
		p[ m_ichild ] = data;
		p[ m_ichild ].parent = this;
		(p + m_ichild)->m_idepth = m_idepth + 1;
		delete[] child;
		++m_ichild;
		child = p;
	}
}

template<class T>
class LemonTree  
{
public:
	LemonTree()
	{
		root = new Lemon<T>();
	}
	virtual ~LemonTree()
	{
		delete root ;
	}
	
	void add_child(Lemon<T>* p, T& data)
	{ 
		p->add_child(data);
	}
	
	void crush(Lemon<T>* p);
	Lemon<T>* get_root(void){return root;}
	void dump(int dep, pod_vector<Lemon<T>*>& data);
	BOOL node_num(int dep, int& num);
private:
	void collect(Lemon<T>*p, int depth, pod_vector<Lemon<T>*>& container);
	void caclu_num(Lemon<T>*p, int depth, int& num);

public://to be edit....
	Lemon<T>* root;	
};
template<class T>
void LemonTree<T>::crush(Lemon<T>* p)
{
	ASSERT(p->m_ichild >= 0 && p->m_ichild < 100);
	if (p->m_ichild > 0) 
	{
		for(int i = 0; i < p->m_ichild; i++)
			crush(p->child[i]);
		p->m_ichild = 0;
	}
	else
	{
		delete p;
	}
}

template<class T>
BOOL LemonTree<T>::node_num(int dep, int& num)
{
	num = 0; 
	caclu_num(root, dep, num);
	return TRUE;
}

template<class T>
void LemonTree<T>::caclu_num(Lemon<T>*p, int depth, int& num)
{
	if (depth == 0) 
		num += p->child_num();
	else
	{
		for(int i = 0; i < p->m_ichild; i++)
			caclu_num(p->child+i, depth-1, num);
	}
}

template<class T>
void LemonTree<T>::dump(int dep, pod_vector<Lemon<T>*>& container)
{
	collect(root, dep, container);
}
template<class T>
void LemonTree<T>::collect(Lemon<T>* p, int depth, pod_vector<Lemon<T>*>& container)
{
	if (depth == 0) 
		container.add(p);
	else
	{
		for(int i = 0; i < p->m_ichild; i++)
			collect(p->child+i, depth-1, container);
	}
}
#endif // !defined(_LEMON_TREE_H_)
