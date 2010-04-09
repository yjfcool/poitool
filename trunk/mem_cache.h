// area_buf_mgr.h: interface for the area_buf_mgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AREA_BUF_MGR_H__381464D7_5602_4FA4_85C5_52C0BBDA2C70__INCLUDED_)
#define AFX_AREA_BUF_MGR_H__381464D7_5602_4FA4_85C5_52C0BBDA2C70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "pod_vector.h"
#include "mem_block.h"
//#include "xmemory.h"

//-----poi���-------------------
#define POI_SPELL		0
#define POI_NAME		1
#define POI_OTHER 		2
#define STREET_SPELL 	3
#define STREET_NAME 	4
#define STREET_OTHER 	5
#define STREET_ACROSS 	6
#define PLACE_SPELL 	7
#define PLACE_NAME		8
#define PLACE_OTHER 	9
//-------ȫ������---------
#define POI_AROUND 		10
#define SIGHT_NAME		11
#define SIGHT_SPELL		12
#define SIGHT_OTHER		13
#define ALL_AREA_NUM	4

#define AREA_NUM		366

#define MEM_PRINT			(true)
#define DEFAULT_PART_NUM	(PLACE_OTHER + 1)
#define DEFAULT_AREA_NUM	(366)
#define DEFAULT_BLOCK_STEP	(3)
#define MAX_MEM_SZ			(10485760)			
/*
	���Ҫ��:����POI�ڴ��
	
	����POI��ĵ����� ���ŵõ�һ��POI �ڴ�id
	
	  �����ڴ��

  ������
	1	2	3	4...
	------------------
	|(name)		|	|
	|(spell)	|	|
	|(address)
ÿ�������Ŷ�Ӧһ������ ���а���x��ָ��(x == poi������)
�������а���ȫ������ ȫ������ָ�ܱ���Ϣ�ͷ羰
�����������һ�������� �������ĸ������������� Ĭ��Ϊ10��
���������POI�Ľṹռ��(10��10��4 + 10)���ֽڵ��ڴ�
����POI��������£�
���POIģ�����������Ϊarea_id���ͺ�Ϊtype_id��poi�ڴ��
����area_id��type_idΪ������ѯ�Ƿ��ڵ�ǰ�ڴ���
����������ڴ�����δ�����û��趨���ڴ�����ֵ��ֱ�Ӷ������뵽��ǰ�ڴ����
��������ڴ��������Ҫɾ��һЩ

ɾ��˳������:(���)
ɾ����������������� 
�ӵ���ָ���ĩβɾ��ֱ��ɾ�����������������
ɾ��ȫ������
��ĩβ��ʼɾ����ǰ����������
ɾ�������з����ڴ�ﵽҪ���ֹͣ

  use like this:
 if (!area_buf(area_id, type_id, poi_block*)) {
	alloc_len();
	for()
		alloc_buf()
 */


const unsigned int max_poi_mem  = 1024*1024*15;//15M

const unsigned int max_seg_num = 1000;

class mem_cache
{
public:
	
	mem_cache():m_poi_block(0),m_area_num(0),m_seg_num(0),m_full_area_num(0),m_mem_sz(0),m_pod_idx(0)
	{
	}
	~mem_cache();
	
	BOOL prepare(unsigned area_num, unsigned seg_num, unsigned full_area_num)
	{
		clear();
		
		m_area_num = area_num;
		m_seg_num = seg_num;
		m_full_area_num = full_area_num;

		m_poi_block = pod_allocator<b_uchar_mem>::allocate(m_area_num * m_seg_num + m_full_area_num);

		m_mem_sz = 0;
		m_pod_idx.capacity(max_seg_num);
		return TRUE;
	}

	BOOL alloc(unsigned int l, unsigned area, unsigned seg);

	unsigned char* request(unsigned area, unsigned seg, unsigned* len);
private:
	BOOL clear();

	BOOL reduce(unsigned sz, unsigned area, unsigned seg);

	int poi_block_idx(unsigned area, unsigned seg = 0);
private:
	unsigned		m_area_num;				//���е����ĸ���
	unsigned		m_seg_num;				//ÿ���������е�seg��Ŀ
	unsigned		m_full_area_num;		//ȫ�����ݸ���
	unsigned	    m_mem_sz;				//��ǰ���ڴ��
	b_uchar_mem*	m_poi_block;			//����seg���ڴ�ָ��
	pod_vector<int> m_pod_idx;				//��¼�Ѿ�������ڴ������
};




#endif // !defined(AFX_AREA_BUF_MGR_H__381464D7_5602_4FA4_85C5_52C0BBDA2C70__INCLUDED_)
