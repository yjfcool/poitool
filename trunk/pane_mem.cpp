// pane_mem.cpp: implementation of the pane_mem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pane_mem.h"
#include "xmemory.h"
pane_mem_cache::~pane_mem_cache()
{
	//----------------------------------------------------------------销毁旧的pane
	unsigned int i = 0;
	for(i = 0; i < m_y_dem; i++)
		pod_allocator<idx_pane>::deallocate(m_p_mem[i], m_x_dem);
	pod_allocator<idx_pane*>::deallocate(m_p_mem, m_y_dem);
	
	delete m_geo_pane;
}

BOOL pane_mem_cache::build_pane_mem(POINT pt, int radius)
{
	unsigned int lon_idx_b, lat_idx_b, pane_dem_x, pane_dem_y;

	if (!m_geo_pane->bnd_idx(pt, radius, lon_idx_b, lat_idx_b, pane_dem_x, pane_dem_y)) 
	{
		return FALSE;
	}

	unsigned int i = 0, j = 0;

	if (m_p_mem == NULL)
	{
		m_p_mem = pod_allocator<idx_pane*>::allocate(pane_dem_y);
		
		for(i = 0; i < pane_dem_y; i++)
		{
			m_p_mem[i] = pod_allocator<idx_pane>::allocate(pane_dem_x);
			memset(m_p_mem[i], 0, sizeof(idx_pane) * pane_dem_x);
			
			for(j = 0; j < pane_dem_x; j++)
			{
				m_p_mem[i][j].x_idx = lon_idx_b + j;
				m_p_mem[i][j].y_idx = lat_idx_b + i;
			}
		}
	}
	else
	{
		//--------------------------------------------------------------------判断维数是否相同
		if (pane_dem_x != m_x_dem || pane_dem_y != m_y_dem) 
		{
			//----------------------------------------------------------------建立新的pane并重用旧的pane
			idx_pane** p_mem = pod_allocator<idx_pane*>::allocate(pane_dem_y);
			
			for(i = 0; i < pane_dem_y; i++)
			{
				p_mem[i] = pod_allocator<idx_pane>::allocate(pane_dem_x);
				memset(p_mem[i], 0, sizeof(idx_pane) * pane_dem_x);

				for(j = 0; j < pane_dem_x; j++)
				{
					//-------------------------------------------------------------------------新的id号
					p_mem[i][j].x_idx = lon_idx_b + j;
					p_mem[i][j].y_idx = lat_idx_b + i;
				
					if (   lon_idx_b + j >= m_x_idx && lon_idx_b + j <= m_x_idx + m_x_dem - 1 
						&& lat_idx_b + i >= m_y_idx && lat_idx_b + i <= m_y_idx + m_y_dem - 1) 
					{
						//-------------------------------------------------------------------------新的id号对应的旧id号
						unsigned int o_idx_x = lon_idx_b + j - m_x_idx;
						unsigned int o_idx_y = lat_idx_b + i - m_y_idx;
						
						p_mem[i][j].pane_mem.m_buf = 
							m_p_mem[o_idx_y][o_idx_x].pane_mem.m_buf;
						
						p_mem[i][j].pane_mem.m_len = 
							m_p_mem[o_idx_y][o_idx_x].pane_mem.m_len;
						
						m_p_mem[o_idx_y][o_idx_x].pane_mem.m_buf = NULL;
						m_p_mem[o_idx_y][o_idx_x].pane_mem.m_len  = 0;
					}
				}
			}
			//----------------------------------------------------------------销毁旧的pane
			for(i = 0; i < m_y_dem; i++)
				pod_allocator<idx_pane>::deallocate(m_p_mem[i], m_x_dem);
			pod_allocator<idx_pane*>::deallocate(m_p_mem, m_y_dem);
			//----------------------------------------------------------------替代旧的pane
			m_p_mem = p_mem;
		}
		else
		{
			//--------------------------------------------------------------------维数是否相同但不相交
			if (m_x_idx > lon_idx_b + pane_dem_x  - 1|| m_x_idx + m_x_dem < lon_idx_b 
				|| m_y_idx > lat_idx_b + pane_dem_y  - 1|| m_y_idx + m_y_dem < lat_idx_b)
			{
				for(i = 0; i < pane_dem_y; i++)
				{
					for(j = 0; j < pane_dem_x; j++)
					{
						m_p_mem[i][j].x_idx = lon_idx_b + j;
						m_p_mem[i][j].y_idx = lat_idx_b + i;
						m_p_mem[i][j].pane_mem.free();
					}
				}
			}
			else
			{
				i = 0; j = 0;

				unsigned int k = 0, l = 0;
				for(i = 0; i < m_y_dem; i++)
				{
					for(j = 0; j < m_x_dem; j++)
					{
						//--------------------------------------------------------------------上移后y方向从下边开始遍历
						k = (lon_idx_b > m_x_idx)?i : (m_x_dem - i - 1);
						//--------------------------------------------------------------------右移后x方向从左边开始遍历
						l = (lat_idx_b > m_y_idx)?j : (m_y_dem - j - 1);

						unsigned int o_idx_x = m_p_mem[k][l].x_idx;
						unsigned int o_idx_y = m_p_mem[k][l].y_idx;

						m_p_mem[k][l].x_idx = lon_idx_b + j;
						m_p_mem[k][l].y_idx = lat_idx_b + i;

						if (   o_idx_x >= lon_idx_b && o_idx_x <= (lon_idx_b + pane_dem_x - 1)
							&& o_idx_y >= lat_idx_b && o_idx_y <= (lat_idx_b + pane_dem_y - 1)) 
						{
						//--------------------------------------------------------------------新的索引
							unsigned int n_idx_x = o_idx_x - lon_idx_b;
							unsigned int n_idx_y = o_idx_y - lat_idx_b;
						
							m_p_mem[n_idx_y][n_idx_x].pane_mem.free();
							
							unsigned char* p = m_p_mem[k][l].pane_mem.m_buf;
							unsigned int xx = m_p_mem[k][l].pane_mem.m_len;
							m_p_mem[n_idx_y][n_idx_x].pane_mem.m_buf =
							m_p_mem[k][l].pane_mem.m_buf;
							
							m_p_mem[n_idx_y][n_idx_x].pane_mem.m_len =
							m_p_mem[k][l].pane_mem.m_len;
							
							m_p_mem[k][l].pane_mem.m_buf = 0;
							m_p_mem[k][l].pane_mem.m_len = 0;
						}
						else
						{
							m_p_mem[k][l].pane_mem.free();
						}
					}
				}
			}		
		}
	}
	m_x_idx = lon_idx_b;
	m_y_idx = lat_idx_b;
	m_x_dem = pane_dem_x;
	m_y_dem = pane_dem_y;
	return TRUE;
}
