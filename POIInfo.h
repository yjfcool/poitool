// POIInfo.h: interface for the CPOIInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POIINFO_H__D39AE910_F106_46DD_B439_6304566465B3__INCLUDED_)
#define AFX_POIINFO_H__D39AE910_F106_46DD_B439_6304566465B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "area_tree.h"
#include "asor_tree.h"
#include "auto_ptr.h"
#include "pod_vector.h"
#include "mem_cache.h"
#include "pane_mem.h"
#include "File.h"


typedef unsigned char uchar ;

struct PoiHead 
{
	char sign[4];
	int version;
	short year;
	char month;
	char day;
	ULONG admin_pos;
	ULONG assort_pos;
	ULONG navi_table;
	ULONG entire_area_navi;
	ULONG entire_around;
	ULONG entire_tell;
	ULONG entire_sight_spell;
	ULONG entire_sight_name;
	char data_version;
	char extra_version[12];
};

typedef int pos_table[10] ;


struct across 
{
	int idx1;
	int idx2;
	int lon;
	int lat;
};

struct ArdPOI
{
	int AreaId;
	int Index;
	int distance;
	bool operator > (ArdPOI& other)
	{
		return distance > other.distance;
	}
	bool operator < (ArdPOI& other)
	{
		return distance < other.distance;
	}
};
struct sight
{
	int AreaId;
	int Index;
	int Lon;
	int Lat;
};
struct poi_specific//sizeof(poi_specific)  = ???
{
	int lon;
	int lat;
	int type;
	int area;
	b_char_mem address;
	b_char_mem telephone;
	b_char_mem describe ;
	b_char_mem pic;
};

struct other_specific
{
	int lon;
	int lat;
	int area;
	int type;
};
void windows_error(void);
void system_error(void);

BOOL assor_match(unsigned short ass1, unsigned short ass2);

class CPOIInfo 
{
public:
	CPOIInfo();
	~CPOIInfo();
	/*在以pt中中心radius为半径的范围内查找type类的poi*/
	void Find(POINT pt, int radius, short type, pod_vector<ArdPOI>& res);//<--------------------1

	/*道路名和街道详细信息*/
	BOOL place_street_speci(int city, int type, int idx, 
												int& x,
												int& y,
												unsigned short& area);
	/*交叉路口搜索*/
	void Find(int city, pod_vector<across>& cross_list, int rdx1, int rdx2 = -1);
	/*景 点 检 索*/
	BOOL Find(const char* str, pod_vector<sight>&, int city);
	BOOL Open(char* PoiFile);
	/*POI详细信息*/
	BOOL poi_speci(int city, int idx, poi_specific& res);
	/*手写搜索 拼音检索*/
	BOOL Find(char* str, int city, int type, pod_vector<int>& hit);
	/*获得名称属性*/
	BOOL str_field(int city, int type, int idx, b_char_mem& str_mem);
	//判断s_ass是否属于m_ass
	BOOL belong_to(unsigned short s_ass, unsigned short m_ass);
private:
	int LoadUnzipUnion(int city, int type, unsigned char*& pUnzipUnion);
	int LoadUnzipUnion(int city, int type);
	int load_string(int city, int seg);
	int load_sight_speci(void);
	int load_across(int city);
	int load_pl_st_speci(int city, int type);
	int load_around(idx_pane& pane, int idx1, int idx2, uchar bzip = 1);	
	int load_poi_speci(int city);
	int decode_zip_pack(CLFile& file, unsigned int pack_num, 
		pod_vector<b_uchar_mem>& unzip_mem);

	BOOL decode_unzip_pack(CLFile& File, int len, pod_vector<b_uchar_mem>& unzip_mem)throw(Base_Exception);

	BOOL str_in_union(const unsigned char* m_str,
										int m_len, 
										const unsigned char* sub_str, 
										int sub_len,
										pod_vector<int>& hit);
	BOOL sight_idx2info(int index, sight& s);
	BOOL clear(void);
public:	
	CLFile m_File;
	area_tree admin;
	asor_tree ass;
	PoiHead m_Head;

	unsigned int m_nAreaNum;
	mem_cache    m_buf_cache;
};

#endif // !defined(AFX_POIINFO_H__D39AE910_F106_46DD_B439_6304566465B3__INCLUDED_)
