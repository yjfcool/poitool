// asor_tree.cpp: implementation of the asor_tree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "poiview.h"
#include "asor_tree.h"
#include "auto_ptr.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
struct asor_info 
{
	int		begin;
	short	num;
};


BOOL asor_tree::file2tree(char* as_buf, int len)
{
	if (as_buf[0] != 'A' || as_buf[1] != 'S' || as_buf[2] != 'S' || as_buf[3] != 'O')
	{
		return FALSE;
	}

	rebuild();
	
	unsigned char n_l = as_buf[20];
	m_n_of_lv = n_l;
	
	auto_ptr<asor_info> p_zone_info;
	asor_info* p = new asor_info[n_l];
	p_zone_info.reset(p);
	
	unsigned char i = 0;
	for(i = 0; i < n_l; i++)
	{
		p[i].begin	= *(int*)(as_buf + 21 + i * 6);
		p[i].num	= *(short*)(as_buf + 21 + i * 6 + 4);
	}
	//---------------------------------------------------------------------------------计算m_src_series长度
	int series_len = 0;
	int pic_len = *(int*)(as_buf + n_l * 6 + 37);
	series_len += pic_len;	
	pod_vector<int> l_series(n_l);
	for(i = 0; i < n_l; i++)													
	{
		l_series[i] = (i == n_l - 1)?(*(int*)(as_buf + 21 + n_l * 6)):(p[i+1].begin - p[i].begin);
		l_series[i] -= p[i].num * (i == (n_l - 1)?13:16);
		series_len += l_series[i];
	}
	//---------------------------------------------------------------------------------层数、第i层名称集合长度、图片区长度
	m_src_series.alloc(series_len + sizeof(n_l) + n_l * 4 + 4);
	//---------------------------------------------------------------------------------组合m_src_series
	int series_pt = 0;
	memmove(m_src_series.m_buf, &n_l, sizeof(n_l));
	series_pt += sizeof(n_l);
	for(i = 0; i < n_l; i++)
	{
		memmove(m_src_series.m_buf + series_pt, &l_series[i], 4);
		series_pt += 4;
	}
	memmove(m_src_series.m_buf + series_pt, &pic_len, sizeof(pic_len));
	series_pt += sizeof(pic_len);
	for(i = 0; i < n_l; i++)
	{
		memmove(m_src_series.m_buf + series_pt, as_buf + p[i].begin + p[i].num * (i == (n_l - 1)?13:16), l_series[i]);
		series_pt += l_series[i];
	}
	int pic_begin = *(int*)(as_buf + n_l * 6 + 33);	
	//---------------------------------------------------------------------------------保存图片位置
	char* ass_pic  = (char*)(m_src_series.m_buf + series_pt);
	memmove(m_src_series.m_buf + series_pt, as_buf + pic_begin, pic_len);
	//---------------------------------------------------------------------------------建立多叉树
	Lemon<asor>* root = m_p_tree->get_root();
	char* p_name = (char*)(m_src_series.m_buf + sizeof(n_l) + n_l * 4 + 4);
	for(i = 0; i < n_l; i++)
	{
		char* p_assor  = as_buf + p[i].begin;

		unsigned short j = 0;

		pod_vector<Lemon<asor>*> UpLevel(1000, 50);

		Lemon<asor>* pNode =  NULL;

		for(j = 0; j < p[i].num; j++)
		{
			asor z(p_name, *(p_assor), p_name + *(p_assor), *(p_assor + 7),
				ass_pic + *(int*)(p_assor + 1), *(short*)(p_assor + 5), j);
			
			short father = -1;
			if (i == 0)
				pNode = root;
			else
			{
				if(UpLevel.size() == 0)
					m_p_tree->dump(i, UpLevel);
				father = *(short*)(p_assor + 11);
				pNode = UpLevel[father];
			}
			m_p_tree->add_child(pNode, z);
		
			p_name += *(p_assor) + *(p_assor + 7);
			
			if(i == n_l - 1)
				p_assor += 13;
			else
				p_assor += 16;
		}
		UpLevel.clear();
	}
	return TRUE;
}
