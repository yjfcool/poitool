// area_tree.cpp: implementation of the area_tree class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "poiview.h"
#include "area_tree.h"
#include "auto_ptr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

struct zone_info 
{
	int		begin;
	short	num;
};

BOOL area_tree::file2tree(char* ad_buf, int len)
{
	if (ad_buf[0] != 'A' || ad_buf[1] != 'D' || ad_buf[2] != 'M' || ad_buf[3] != 'I')
	{
		return FALSE;
	}
	//----------------------------------------------------------------------------------number of level
	unsigned char n_l = ad_buf[20];
	
	m_n_of_lv = n_l;

	rebuild();
	
	auto_ptr<zone_info> p_zone_info;
	zone_info* p = new zone_info[n_l];
	p_zone_info.reset(p);

	unsigned char i = 0;
	for(i = 0; i < n_l; i++)
	{
		p[i].begin	= *(int*)(ad_buf + 21 + i * 6);
		p[i].num	= *(short*)(ad_buf + 21 + i * 6 + 4);
	}
	//---------------------------------------------------------------------------------获得各层名称结合长度和总长度
	int series_len = 0;
	pod_vector<int> l_series(n_l);
	for(i = 0; i < n_l; i++)
	{
		l_series[i] = (i == (n_l - 1))?(*(int*)(ad_buf + 21 + n_l * 6)):(p[i+1].begin - p[i].begin);
		l_series[i] -= (p[i].num * (i == (n_l - 1)?13:16));
		series_len += l_series[i];
	}
	//---------------------------------------------------------------------------------拼在一起
	m_name_series.alloc(series_len + 1 + n_l * 4);
	int series_pt = 0;
	memmove(m_name_series.m_buf, &n_l, sizeof(n_l));
	series_pt += sizeof(n_l);
	for(i = 0; i < n_l; i++)
	{
		memmove(m_name_series.m_buf + series_pt, &l_series[i], 4);
		series_pt += 4;
	}
	for(i = 0; i < n_l; i++)
	{
		memmove(m_name_series.m_buf + series_pt, ad_buf + p[i].begin + p[i].num * (i == (n_l - 1)?13:16), l_series[i]);
		series_pt += l_series[i];
	}
	//---------------------------------------------------------------------------------建树
	Lemon<zone>* root = m_p_tree->get_root();
	unsigned char j = 0;
	char* p_name = (char*)(m_name_series.m_buf + 1 + n_l * 4);
	for(i = 0; i < n_l; i++)
	{
		char* p_zone = ad_buf + p[i].begin;
	
		pod_vector<Lemon<zone>*> UpLevel(1000, 100);
		
		for(int j = 0; j < p[i].num; j++)
		{
			zone z(p_name, *p_zone, *(p_zone + 1), *(p_zone + 2));
			short father = -1;
			Lemon<zone>* pNode =  NULL;
		//----------------------------------------------------------------------------得到父节点
			if (i == 0)
				pNode = root;
			else
			{
				if(UpLevel.size() == 0)
					m_p_tree->dump(i, UpLevel);
				father = *(short*)(p_zone + 11);
				pNode = UpLevel[father];
			}
		//----------------------------------------------------------------------------增加到父节点
			m_p_tree->add_child(pNode, z);
		//----------------------------------------------------------------------------指针移动
			p_name += *p_zone;
			
			if(i == n_l - 1)
				p_zone += 13;
			else
				p_zone += 16;
		}
		UpLevel.clear();
	}
	return TRUE;
}
