// area_tree.h: interface for the area_tree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AREA_TREE_H__4F22FC9D_95CE_41C5_B562_D574E3FEBB9A__INCLUDED_)
#define AFX_AREA_TREE_H__4F22FC9D_95CE_41C5_B562_D574E3FEBB9A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "mem_block.h"
#include "pod_vector.h"
#include "tree_mgr.h"


class zone 
{
public:
	zone(char* name, int len, char spell, char stroke)
									:zone_name(name),
									spell_head(spell),
									stroke_num(stroke),
									len_name(len)
	{
	}
	zone():zone_name(0),spell_head(0),stroke_num(0),len_name(0)
	{
	}
	~zone()
	{
	}
	zone& operator << (zone& z)
	{
		*this = z;
		return *this;
	}
	void name(b_char_mem& str)const 
	{
		str.alloc(len_name + 1);
		memmove(str.m_buf, zone_name, len_name);
		str.m_buf[len_name] = '\0';
	}
	char spell(void){return spell_head;}
	char stroke(void){return stroke_num;}
private:
	char* zone_name;
	int   len_name;
	char  spell_head;
	char  stroke_num;
};
class area_tree  :public TreeMgr<zone> 
{
private:

public:
	area_tree()
	{}
	area_tree::~area_tree()
	{}
	BOOL file2tree(char* ad_buf, int len);
private:
	/*
	 *	|1字节分区数|第一分区名称集合长度|第二分区名称集合长度|....|名称集合|
	 */
	b_char_mem m_name_series;
};

#endif // !defined(AFX_AREA_TREE_H__4F22FC9D_95CE_41C5_B562_D574E3FEBB9A__INCLUDED_)
