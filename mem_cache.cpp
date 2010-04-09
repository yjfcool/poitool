// area_buf_mgr.cpp: implementation of the area_buf_mgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "poiview.h"
#include "mem_cache.h"

mem_cache::~mem_cache()
{
	clear();
}

/*
	缩小内存占用量 
	sz		- 缩小后的内存
	area	- 当前区域
	seg		- 当前部分
 */
BOOL mem_cache::reduce(unsigned sz, unsigned area, unsigned seg)
{
	unsigned int idx_idx = 0;
	//--------------------------------------------------------------------------当前区域部分记录
	int c_b_idx = poi_block_idx(area, 0);
	int c_e_idx = poi_block_idx(area, m_seg_num);
	pod_vector<unsigned int> c_pod_idx(m_seg_num);										//记录本地其他部分
	pod_vector<unsigned int> o_pod_idx(m_pod_idx.size());								//记录外地部分
	//--------------------------------------------------------------------------遍历获得内存分配情况
	while (idx_idx < m_pod_idx.size()) 
	{
		int idx = 0;
		idx = m_pod_idx[idx_idx];
		if (idx >= c_b_idx && idx <= c_e_idx)									//本地其它部分
		{
			c_pod_idx.push_back(idx_idx);
		}
		else
		{
			o_pod_idx.push_back(idx_idx);										//外地部分
		}
		++idx_idx;
	}
	//--------------------------------------------------------------------------②先删除不在当前区域的部分
	idx_idx = 0;
	while(idx_idx < o_pod_idx.size() &&  m_mem_sz > sz)
	{
		int idx = 0;
		idx = m_pod_idx[o_pod_idx[idx_idx]];

		m_mem_sz -=  m_poi_block[idx].m_len;
		
		m_poi_block[idx].free();
		
		m_pod_idx[o_pod_idx[idx_idx]] = -1;
		
		++idx_idx;
	};
	if(m_mem_sz < sz) goto clear_pod;
	//--------------------------------------------------------------------------③删除当前区域的部分
	idx_idx = 0;
	while(idx_idx < c_pod_idx.size() &&  m_mem_sz > sz)
	{
		int idx = 0;
		idx = m_pod_idx[c_pod_idx[idx_idx]];
		
		m_mem_sz -=  m_poi_block[idx].m_len;
		
		m_poi_block[idx].free();
		
		m_pod_idx[c_pod_idx[idx_idx]] = -1;
		
		++idx_idx;
	};
clear_pod:
	idx_idx = 0;
	pod_vector<int> pod_idx(m_pod_idx.size());
	while (idx_idx < m_pod_idx.size()) 
	{
		if (m_pod_idx[idx_idx] != -1) 
		{
			pod_idx.push_back(m_pod_idx[idx_idx]);
		}
		++idx_idx;
	}
	m_pod_idx = pod_idx;
	return (m_mem_sz < sz)?TRUE:FALSE;
}
BOOL mem_cache::clear()
{
	if (m_poi_block) 
	{
		unsigned int i = 0;
		unsigned int block_num = m_area_num * m_seg_num + m_full_area_num;
		for(i = 0; i < block_num; i++)
		{
			m_poi_block[i].free();
		}
		pod_allocator<b_uchar_mem>::deallocate(m_poi_block, m_area_num * m_seg_num + m_full_area_num);
		m_poi_block = NULL;
	}
	return TRUE;
}

BOOL mem_cache::alloc(unsigned int l, unsigned area, unsigned seg)
{
	if (area > m_area_num|| seg > m_seg_num + m_full_area_num) 
	{
		return FALSE;
	}
	int idx = poi_block_idx(area, seg);
	if ((l + m_mem_sz) > max_poi_mem) 
	{
		int idx = poi_block_idx(area, seg);
		if(reduce(max_poi_mem - l, area, seg))
		{
			if(m_poi_block[idx].alloc(l))
			{
				m_pod_idx.add(idx);
				m_mem_sz += l;
				return TRUE;
			}
		}
		return FALSE;
	}
	else
	{
		if(m_poi_block[idx].alloc(l))
		{
			m_pod_idx.add(idx);
			m_mem_sz += l;
			return TRUE;
		}
		return FALSE;
	}
}
unsigned char* mem_cache::request(unsigned area, unsigned seg, unsigned* len)
{
	if (area > m_area_num|| seg > m_seg_num + m_full_area_num) 
	{
		return NULL;
	}
	
	int idx = poi_block_idx(area, seg);
		
	if (len) *len = m_poi_block[idx].m_len;
	
	return m_poi_block[idx].m_buf;
}
int mem_cache::poi_block_idx(unsigned area, unsigned seg )
{
	if (seg > m_seg_num) 
	{
		return seg - m_seg_num + m_area_num * m_seg_num;
	}
	else
		return seg + area * m_seg_num;
}
