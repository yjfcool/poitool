// asor_tree.h: interface for the asor_tree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASOR_TREE_H__FF9BDA58_FF6F_40A5_B938_8E34D398B43E__INCLUDED_)
#define AFX_ASOR_TREE_H__FF9BDA58_FF6F_40A5_B938_8E34D398B43E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "mem_block.h"
#include "pod_vector.h"
#include "tree_mgr.h"

class asor 
{
public:
	asor(char* c_name, int c_len, char* e_name, int e_len, char* pic, int pic_l, int code)
																:cn_name(c_name),
																cn_len(c_len),
																en_name(e_name),
																en_len(e_len),
																ppic(pic),
																pic_len(pic_l),
																ass_code(code)
	{
	}
	asor():cn_name(0),
		cn_len(0),
		en_name(0),
		en_len(0),
		ppic(0),
		pic_len(0)
	{
	}
	~asor()
	{
	}
	asor& operator << (asor& z)
	{
		*this = z;
		return *this;
	}
	void c_name(b_char_mem& str)const 
	{
		str.alloc(cn_len + 1);
		memmove(str.m_buf, cn_name, cn_len);
		str.m_buf[cn_len] = '\0';
	}
	void e_name(b_char_mem& str)const 
	{
		str.alloc(en_len + 1);
		memmove(str.m_buf, en_name, en_len);
		str.m_buf[en_len] = '\0';
	}
	char* pic(int& pic_len)const
	{
		return ppic;
	}
	int code(void){ return ass_code;}
private:
	char*	cn_name;
	int		cn_len;
	char*	en_name;
	int		en_len;
	char*	ppic;
	int		pic_len;
	int		ass_code;
};
class asor_tree :public TreeMgr<asor>
{
public:
	asor_tree()
	{ }
	~asor_tree()
	{ }
	BOOL file2tree(char* ad_buf, int len);
private:
	/*
	 *	|1字节分区数|第一分区名称集合长度|第二分区名称集合长度|...|第N个分区集合长度|图片长度|第一层名称集合|第二层..|图片集合|
	 */
	b_char_mem m_src_series;
	pod_vector<unsigned short> m_bank;
	pod_vector<unsigned short> m_shortcut;
};

#endif // !defined(AFX_ASOR_TREE_H__FF9BDA58_FF6F_40A5_B938_8E34D398B43E__INCLUDED_)
