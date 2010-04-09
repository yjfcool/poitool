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

//-----poi类别-------------------
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
//-------全境数据---------
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
	设计要求:管理POI内存块
	
	根据POI块的地区号 类别号得到一个POI 内存id
	
	  所有内存块

  地区号
	1	2	3	4...
	------------------
	|(name)		|	|
	|(spell)	|	|
	|(address)
每个地区号对应一个数组 其中包含x个指针(x == poi类别个数)
地区号中包含全境数据 全境数据指周边信息和风景
数组放在另外一个数组中 这个数组的个数即地区个数 默认为10个
则这个管理POI的结构占用(10×10×4 + 10)个字节的内存
增加POI块策略如下：
如果POI模块请求地区号为area_id类型号为type_id的poi内存块
则用area_id和type_id为索引查询是否在当前内存中
如果不在且内存限制未超过用户设定的内存上限值则直接读并加入到当前内存块中
如果发现内存过大则需要删除一些

删除顺序如下:(★★)
删除除本区域外的数据 
从地区指针的末尾删起直到删除这个地区所有数据
删除全境数据
从末尾开始删除当前地区的数据
删除过程中发现内存达到要求就停止

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
	unsigned		m_area_num;				//所有地区的个数
	unsigned		m_seg_num;				//每个地区含有的seg数目
	unsigned		m_full_area_num;		//全境数据个数
	unsigned	    m_mem_sz;				//当前总内存块
	b_uchar_mem*	m_poi_block;			//所有seg的内存指针
	pod_vector<int> m_pod_idx;				//记录已经分配的内存块索引
};




#endif // !defined(AFX_AREA_BUF_MGR_H__381464D7_5602_4FA4_85C5_52C0BBDA2C70__INCLUDED_)
