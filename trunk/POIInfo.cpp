// POIInfo.cpp: implementation of the CPOIInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "POIView.h"
#include "POIInfo.h"
#include "decode\decode.h"
#include "auto_ptr.h"
#include "math1.h"
#include "geo_pane.h"
#include "loger\loger.h"

#define PASSWORD _T("Jd&52Kp*dc")
const int seg_navi_sz = sizeof(pos_table);
typedef unsigned short ushort;

int _strstr(const unsigned char* sub_str, int sub_len, const unsigned char* str, int str_len)
{
	if (!sub_str || !*sub_str)
		return -1;
	
	size_t m = sub_len;
	size_t n = str_len;
/*	if(m < 3)短字符串查找没有优势
	{
		char* p = strstr(str, sub_str);
		return p?p-str:-1;
	}*/

	static short dt[256];
	for (int c = 0; c < 256; c++)
		dt[c] = m + 1;
	
	const unsigned char* p = sub_str;
	for (; *p; p++)
		dt[*(unsigned char*)p] = m - (p - sub_str);
	
	const unsigned char* mtext = str;
	const unsigned char *t = mtext;
	const unsigned char* _mtx = mtext;
	
	while (_mtx + m <= mtext + n) 
	{
		for (p = sub_str, t = _mtx; *p; ++p, ++t) 
		{
			if (*p != *t)
				break;
		}
		if (*p == 0)
			return _mtx - mtext;
		
		_mtx += dt[*((unsigned char*)_mtx+m)];
	}
	return -1;
}


CPOIInfo::CPOIInfo():m_buf_mgr(363, 10, 3)
{
}

CPOIInfo::~CPOIInfo()
{
}
bool CPOIInfo::Open(char* PoiFile)
{
	if (!m_File.attach(PoiFile, READ_ONLY)) system_error();

	char head[128];
	m_File.read(head, 128);
	memcpy(&m_Head, head, 12);
	ASSERT(m_Head.sign[0] == 'S' && m_Head.sign[1] == 'N' && m_Head.sign[2] == 'S' && m_Head.sign[3] == 'F');

	m_Head.navi_table = *(ULONG*)(head + 51);
	m_Head.admin_pos = *(ULONG*)(head + 42);
	m_Head.assort_pos = *(ULONG*)(head + 46);
	m_Head.entire_area_navi = *(ULONG*)(head + 55);
	m_Head.data_version = head[63];
	memcpy(m_Head.extra_version, head + 12, 12); 
	m_AreaNum = (m_Head.entire_area_navi - m_Head.navi_table) / 40;

	m_File.seek(m_Head.navi_table + m_AreaNum * 40, BEGIN);
	m_File.read(&m_Head.entire_around, 4);
	m_File.read(&m_Head.entire_tell, 4);
	m_File.read(&m_Head.entire_sight_spell, 4);
	m_File.read(&m_Head.entire_sight_name, 4);
	
	m_File.seek(m_Head.admin_pos, BEGIN);
	int size = m_Head.assort_pos - m_Head.admin_pos;
	char* AdminBuf = new char[size];
	m_File.read(AdminBuf, size);
	admin.file2tree(AdminBuf, size);
	delete[] AdminBuf;

	size = m_Head.navi_table - m_Head.assort_pos;

	m_File.seek(m_Head.assort_pos, BEGIN);
	char* AssBuf = new char[size];
	m_File.read(AssBuf, size);
	ass.file2tree(AssBuf, size);
	delete []AssBuf;
	return true;
}

bool CPOIInfo::Find(const unsigned char* str, int city, int type, pod_vector<int>& hit)
{
	if (type >= PLACE_OTHER || type == POI_OTHER || type == STREET_OTHER) 
	{
		return false;
	}
	unsigned char* p = NULL;
	int len = LoadUnzipUnion(type, city, p);

	return str_in_union(p, len, str, strlen((char*)str), hit);
}

bool CPOIInfo::str_in_union(const unsigned char* m_str,
							int m_len, 
							const unsigned char* sub_str, 
							int sub_len,
							pod_vector<int>& hit)
{
	if (hit.size() > 0) hit.clear();
	
	char s_distb_sz[11] = {0};
	memmove(s_distb_sz, m_str, 10);
	int distb_sz = atoi(s_distb_sz);
	const unsigned char* _str = m_str + 10 + distb_sz;
	int* len_distb = (int*)(m_str + 10);
	int rec_num = distb_sz / sizeof(int);
	
	int i = 0;
	for(i = 0; i < rec_num; i++)
	{
		if (_strstr(sub_str, sub_len, _str, len_distb[i])) 
		{
			hit.push_back(i);
		}
		
		_str += len_distb[i];
	}
	return true;
}
bool CPOIInfo::Find(const char* str, pod_vector<sight>& lsight, int type)
{
	if (type != SIGHT_NAME && type != SIGHT_SPELL)
		return false;
	
	unsigned char* p = NULL;
	int len = LoadUnzipUnion(0, type, p);

	pod_vector<int> hit(50,50) ;
	
	if(str_in_union(p, len, (const unsigned char*)str, strlen((char*)str), hit))
	{
		sight tt;
		for(unsigned i = 0; i < hit.size(); i++)
		{
			if (sight_idx2info(hit[i], tt))
				lsight.add(tt);
			else
				return false;
		}
		return true;
	}
	else
		return false;
	
}
bool CPOIInfo::sight_idx2info(int index, sight& s)
{
	unsigned char* p = NULL;
	int len = LoadUnzipUnion(0, SIGHT_OTHER, p);

	if (len == 0 || index > (len / 14)) return false;
	
	p += index * 14;
	s.AreaId = *(ushort*)p;
	s.Index  = *(int*)(p + 2);
	s.Lon    = *(int*)(p + 6);
	s.Lat    = *(int*)(p + 10);
	return true;
}
#define LOTHER_HEAD (26)		//other头文件长度

bool CPOIInfo::poi_speci(int city, int idx, poi_specific& rec)
{
	
	unsigned char* p = NULL;
	int len = LoadUnzipUnion(0, POI_OTHER, p);
	
	if (len > 0) 
	{
		int*	coord_pt	= (int*)(p + 12);
		short*	area_pt		= (short*)(p + 12 + *(int*)(p));
		int*	other_sp_pt = (int*)(p + 12 + *(int*)(p+4) + *(int*)(p+8));
		char*	p_other    = (char*)(p + 12 + *(int*)(p+4) + *(int*)(p+8));
		
		rec.lon  = *(coord_pt + idx * 2);
		rec.lat  = *(coord_pt + idx * 2 + 1);
		
		rec.area = *(coord_pt + idx);

		int other_off = *(other_sp_pt + idx);

		p_other += other_off;
		char sign = *p_other;

		if (sign & (1 << 0)) 
		{
			rec.address.alloc(*p_other);
			memmove(rec.address.m_buf, p_other + 1, *p_other);
			p_other += *p_other + 1;
		}
		if (sign & (1 << 1) ) 
		{
			rec.telephone.alloc(*p_other);
			memmove(rec.telephone.m_buf, p_other + 1, *p_other);
			p_other += *p_other + 1;
		}
		if (sign & (1 << 2) ) 
		{
			rec.describe.alloc(*p_other);
			memmove(rec.describe.m_buf, p_other + 1, *p_other);
			p_other += *p_other + 1;
		}
		if (sign & (1 << 3) ) 
		{
			rec.pic.alloc(*p_other); 
			memmove(rec.pic.m_buf, p_other+1, *p_other);
		}
		return true;
	}
	else
		return false;

}

void CPOIInfo::Find(int city, pod_vector<across>& cross_list,
					int RoadIdx1, int RoadIdx2)
{
	if (cross_list.size()) cross_list.clear();

	unsigned char* p = NULL;
	int len = LoadUnzipUnion(city, STREET_ACROSS, p);
	if (len == 0) return;
	unsigned char* len_distri = p + 4;
	//--------------------------------------------------------------------------计算RoadIdx所在偏移
	int rdx1_off = 0;
	int i = 0;
	for(i = 0; i < RoadIdx1; i++)
	{
		rdx1_off += len_distri[i] * 12;
	}
	//--------------------------------------------------------------------------定位RoadIdx
	int* p_across = (int*)(p + *(int*)p + 4 + rdx1_off);
	
	across across;
	if (RoadIdx2 == -1) 
	{
		for(i = 0; i < len_distri[RoadIdx1]; i++)
		{
			across.idx1 = RoadIdx1;
			across.idx2 = *(p_across++);
			across.longitude = *(p_across++);
			across.latitude = *(p_across++);
			cross_list.add(across);
		}
	}
	else
	{
		for(i = 0; i < len_distri[RoadIdx1]; i++)
		{
			int idx2 = *(p_across++);
			if (idx2 == RoadIdx2)
			{
				across.idx1 = RoadIdx1;
				across.idx2 = idx2;
				across.longitude = *(p_across++);
				across.latitude = *(p_across++);
				cross_list.add(across);
			}
			else
				p_across += 2;
		}
	}
}
/*
 *	|4个字节交叉路口索引长度|实际索引|交叉路口|
 */
int CPOIInfo::load_across(int city)
{
	pos_table table;
	m_File.seek(m_Head.navi_table + city * seg_navi_sz, BEGIN);
	
	if(m_File.read(&table, seg_navi_sz)) system_error();
	int pos = 0;
	pos = m_Head.entire_area_navi + table[STREET_ACROSS];
	m_File.seek(pos, BEGIN);
	char Head[21] = {0};
	if (m_File.read(Head, 21)) system_error();
	int mem_sz = 0;
	//--------------------------------------------------------------------------------索引
	b_char_mem across_distri;
	across_distri.alloc(*(unsigned*)(Head + 4));
	m_File.read(across_distri.m_buf, across_distri.m_len);
	//--------------------------------------------------------------------------------交叉点
	ushort across_pack_num = *(Head + 8);
	int across_mem_len = 0;
	pod_vector<b_char_mem> unzip_across;
	if (across_pack_num) 
		across_mem_len = decode_zip_pack(m_File, across_pack_num, unzip_across);
	else
		return 0;
	//--------------------------------------------------------------------------------拼合
	unsigned char* p = NULL;
	mem_sz = 4 + across_distri.m_len + across_mem_len;
	if(m_buf_mgr.alloc(mem_sz, city, STREET_ACROSS) && (p = m_buf_mgr.request(city, STREET_ACROSS, NULL)))
	{
		int mem_pt = 0;
		memmove(p, &across_distri.m_len, 4);
		mem_pt += 4;
		unsigned int i = 0;
		for(i = 0; i < unzip_across.size(); i++)
		{
			memmove(p + mem_pt, unzip_across[i].m_buf, unzip_across[i].m_len);
			mem_pt += unzip_across[i].m_len;
		}
	}
	else
		return 0;
	return mem_sz;
}
void CPOIInfo::Find(POINT pt, int radius, short type, pod_vector<ArdPOI>& res)
{
	static auto_ptr<geo_pane> geo_pane_ptr;
	static auto_ptr<int> idx2_idx1;
	if (geo_pane_ptr.get() == NULL || idx2_idx1.get() == NULL) 
	{

		b_char_mem ard_head;
		DWORD dwRead = 0;
		ard_head.alloc(25);
		m_File.seek(m_Head.entire_around + 19, BEGIN);
		
		if(m_File.read(ard_head.m_buf, 25)) system_error();
		unsigned char* p = ard_head.m_buf;
		int coord_b_lon = *(int*)(p + 1);
		int coord_b_lat = *(int*)(p + 5);
		int coord_e_lon = *(int*)(p + 9);
		int coord_e_lat = *(int*)(p + 13);
		if(pt.x < coord_b_lon || pt.x > coord_e_lon || pt.y < coord_b_lat || pt.y > coord_e_lon)
			return;
		
		short pacel1_lon = *(short*)(p + 17);
		short pacel1_lat = *(short*)(p + 19);
		short pacel2_lon = *(short*)(p + 21);
		short pacel2_lat = *(short*)(p + 23);
		int pacel1_width  = (coord_e_lon - coord_b_lon) / pacel1_lon;
		int pacel1_height = (coord_e_lat - coord_b_lat) / pacel1_lat;
		int pacel2_width  = (coord_e_lon - coord_b_lon) / pacel2_lon;
		int pacel2_height = (coord_e_lat - coord_b_lat) / pacel2_lat;
		
		geo_pane* p_geo_pane = new geo_pane(coord_b_lon, coord_b_lat, 
							coord_e_lon, coord_e_lat, pacel2_lon,pacel2_lat);
		if(p_geo_pane == NULL) system_error();
		geo_pane_ptr.reset(p_geo_pane);

		int* p_idx2_idx1 = new int(1);
		if(p_idx2_idx1 == NULL) system_error();
		idx2_idx1.reset(p_idx2_idx1);
	}
	
	unsigned int lon_idx_b = 0, lat_idx_b = 0, pane_dem_x = 0, pane_dem_y = 0;
	geo_pane* p_geo_pane = geo_pane_ptr.get();
	p_geo_pane->bnd_idx(pt, radius, lon_idx_b, lat_idx_b, pane_dem_x, pane_dem_y);

	static geo_pane_mem_mgr pane;
	
	if (pane.blank()) 
	{
		pane.build(lon_idx_b, lat_idx_b, pane_dem_x, pane_dem_y);
	}
	else
	{
		geo_pane_mem_mgr pane2;
		pane2.build(lon_idx_b, lat_idx_b, pane_dem_x, pane_dem_y);
		pane2.reuse(pane);
		pane << pane2;
	}
	int i = 0, j = 0;
	for(i = 0; i < pane.y_dem(); i++)
	{
		for(j = 0; j < pane.x_dem(); j++)
		{
			idx_pane& tpane = pane[i * (pane.x_dem()) + j];
			int idx = p_geo_pane->index(tpane.x_idx, tpane.y_idx);	
			if (tpane.pane_mem.m_buf == NULL) 
			{
				load_around(tpane, idx / *idx2_idx1, idx , 1);
			}
			
			if (tpane.pane_mem.m_buf) 
			{
				int size = tpane.pane_mem.m_len;
				const char* aro_union = (const char*)tpane.pane_mem.m_buf;
				
				int ard_num = size / 12;					//poi总条数 = 块大小 / 每条记录12字节
				POINT cur_pt;
				float distance = 0;
				ArdPOI cur_poi;
				const char* p = aro_union;
				int k = 0;
				for(k = 0; k < ard_num; k++)
				{
					ushort cur_type = *(ushort*)(p + 10);
					if (cur_type != type) 
					{
						int x_off = 0, y_off = 0;
						p_geo_pane->pane_coord(idx, x_off, y_off);
						cur_pt.x =  x_off + *(ushort*)(p + 6);//这里的*(p+6)坐标貌似为相对坐标
						cur_pt.y =  y_off + *(ushort*)(p + 8);//相对于二级块的开始
						coordinate_dis(&pt, &cur_pt, &distance);
						if (distance < radius) 
						{
							cur_poi.AreaId   = *(ushort*)p;
							cur_poi.Index    = *(ushort*)(p + 2);
							cur_poi.distance = (int)distance;
							res.push_back(cur_poi);
						}
					}
					p += 12;
				}
			}
		}
	}
}

#define LPARO_HEAD 44
int CPOIInfo::load_around(idx_pane& pane, int idx1, int idx2, uchar bzip)
{
	m_File.seek(m_Head.entire_around 
					+ m_Head.entire_area_navi
					+ LPARO_HEAD
					+ idx1 * 4,
					BEGIN);

	char pos[8] = {0};
	int b_idx = 0, e_idx = 0;
	m_File.read(pos, 8);
	b_idx = *(int*)pos;
	e_idx = *(int*)(pos+4);
	if (b_idx == e_idx)
		/*没有数据*/
		return 0;
	m_File.seek(m_Head.entire_around 
			+ m_Head.entire_area_navi
			+ b_idx
			+ idx2 * 4,
			BEGIN);
	
	int b_zip = 0, e_zip = 0;
	m_File.read(pos, 8);
	b_zip = *(int*)pos;
	e_zip = *(int*)(pos+4);
	if (e_zip == b_zip)
		return 0;
	m_File.seek(m_Head.entire_around 
		+ m_Head.entire_area_navi
		+b_idx+b_zip,
		BEGIN);
	//----------------------------------------------------------------------------

	int mem_sz = 0;
	if (bzip) 
	{

		b_char_mem zip_mem;
		if (zip_mem.alloc(e_zip - b_zip) == TRUE) 
		{
			if(m_File.read(pane.pane_mem.m_buf, (e_zip - b_zip)* sizeof(char))) system_error();
			HDECODE hz = OpenDecode(zip_mem.m_buf, e_zip - b_zip, PASSWORD);
			DECODEENTRY ze;
			GetDecodeItem(hz, &ze);
			if(pane.pane_mem.alloc(ze.unc_size) == TRUE)
			{
				DecodeItem(hz, 0, pane.pane_mem.m_buf, ze.unc_size);
			}
			else
			{
				system_error();
			}
			mem_sz = pane.pane_mem.m_len;
			CloseDecode(hz);
		}
		else
		{
			system_error();
		}
	}
	else
	{
		if(pane.pane_mem.alloc(e_zip - b_zip) == TRUE)
		{
			if(!m_File.read(pane.pane_mem.m_buf, e_zip - b_zip)) system_error();
			mem_sz = e_zip - b_zip;
		}
		else
		{
			system_error();
		}
	}
	return mem_sz;
}
int CPOIInfo::LoadUnzipUnion(int city, int type, unsigned char*& pUnzipUnion)
{
	unsigned len = 0;
	unsigned char* p = NULL;
	if((p = m_buf_mgr.request(city, type, &len)) == NULL)
	{
		if (LoadUnzipUnion(city, type)) 
		{
			unsigned char* p = NULL;
			if(p = m_buf_mgr.request(city, type, &len))
			{
				p = pUnzipUnion;
				return len;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		pUnzipUnion = p;
		return len;
	}
}
int CPOIInfo::LoadUnzipUnion(int city, int type)
{
	switch(type) 
	{
	case POI_SPELL:
	case POI_NAME:
	case STREET_SPELL:
	case STREET_NAME:
	case PLACE_NAME:
	case PLACE_SPELL:
	case SIGHT_SPELL:
	case SIGHT_NAME:
		return load_string(city, type);
		break;
	case POI_OTHER:
		return load_poi_speci(city);
		break;
	case STREET_ACROSS:
		return load_across(city);
		break;
	case PLACE_OTHER:
	case STREET_OTHER:
		return load_pl_st_speci(city, type);
		break;
	case SIGHT_OTHER:
		return load_sight_speci();
		break;
	default: return 0;
	}
	return 0;
}
int CPOIInfo::load_sight_speci(void)
{
	if (m_File.hang()) return 0;

	char Head[9] = {0};
	DWORD dwRead = 0;
	m_File.seek(m_Head.entire_sight_spell + 4, BEGIN);
	m_File.read(Head, 9);
	int off_in_pack = 21 + *(int*)(Head + 4) + *(int*)(Head + 5);
	
	m_File.seek(m_Head.entire_sight_spell + off_in_pack, BEGIN);
		
	unsigned char* p = NULL;
	int mem_sz = *(int*)(Head + 4) * 14;
	if(m_buf_mgr.alloc(mem_sz, 0, SIGHT_OTHER) && (p = m_buf_mgr.request(0, SIGHT_OTHER, NULL)))
	{
		
		if(m_File.read(p,mem_sz)) system_error();
		return mem_sz;
	}
	else
		return 0;
}



int CPOIInfo::load_poi_speci(int city)
{
	if (m_File.hang()) return 0;
	pos_table table;
	m_File.seek(m_Head.navi_table + city * seg_navi_sz, BEGIN);

	if(m_File.read(&table, seg_navi_sz)) system_error();
	int pos = 0;
	pos = m_Head.entire_area_navi + table[POI_OTHER];
	m_File.seek(pos, BEGIN);
	char Head[26] = {0};
	if (m_File.read(Head, 26)) system_error();
	int mem_sz = 0;																	
	
	//--------------------------------------------------------------------------------坐标
	ushort pos_pack_num = *(Head + 8);
	
	int pos_mem_len = 0;
	pod_vector<b_char_mem> unzip_coord;
	
	if (pos_pack_num) 
		pos_mem_len = decode_zip_pack(m_File, pos_pack_num, unzip_coord);
	else
		return 0;
	//--------------------------------------------------------------------------------分类
	
	ushort sort_pack_num = *(Head + 13);
	
	int sort_mem_len = 0;
	pod_vector<b_char_mem> unzip_sort;
	
	if (sort_pack_num) 
		sort_mem_len = decode_zip_pack(m_File, pos_pack_num, unzip_sort);
	else
		return 0;
	//--------------------------------------------------------------------------------区划
	
	ushort area_pack_num = *(Head + 18);
	
	int area_mem_len = 0;
	pod_vector<b_char_mem> unzip_area;
	
	if (area_pack_num) 
		area_mem_len = decode_zip_pack(m_File, pos_pack_num, unzip_area);
	else
		return 0;
	//--------------------------------------------------------------------------------不规则长度分布
	ushort other_distri_pack_num = *(Head + 23);
	
	int other_distri_len = 0;
	pod_vector<b_char_mem> unzip_other_distri;
	
	if (other_distri_pack_num) 
		other_distri_len = decode_zip_pack(m_File, other_distri_pack_num,
														unzip_other_distri);
	else
		return 0;
	//--------------------------------------------------------------------------------其他不规则信息
	
	ushort other_pack_num = *(Head + 18);
	
	int other_mem_len = 0;
	pod_vector<b_char_mem> unzip_other;
	
	if (other_pack_num) 
		sort_mem_len = decode_zip_pack(m_File, pos_pack_num, unzip_area);
	else
		return 0;
	//--------------------------------------------------------------------------------拼凑到一起
	unsigned char* p = NULL;
	mem_sz = 3 + pos_mem_len + sort_pack_num + area_pack_num + other_pack_num;
	if(m_buf_mgr.alloc(mem_sz, city, POI_OTHER) && (p = m_buf_mgr.request(city, POI_OTHER, NULL)))
	{
		//|4|4|4|4|。。。|前16个字节分别指明经纬度、区划、类别、不规则分布的长度
		int mem_pt = 0;
		memmove(p + mem_pt, &pos_mem_len, 4);
		mem_pt += 4;
		memmove(p + mem_pt, &sort_mem_len, 4);
		mem_pt += 4;
		memmove(p + mem_pt, &area_mem_len, 4);
		mem_pt += 4;
		memmove(p + mem_pt, &other_distri_len, 4);
		mem_pt += 4;
		unsigned int i = 0;
		for(i = 0; i < unzip_coord.size(); i++)
		{
			memmove(p + mem_pt, unzip_coord[i].m_buf, unzip_coord[i].m_len);
			mem_pt += unzip_coord[i].m_len;
		}
		for(i = 0; i < unzip_sort.size(); i++)
		{
			memmove(p + mem_pt, unzip_sort[i].m_buf, unzip_sort[i].m_len);
			mem_pt += unzip_sort[i].m_len;
		}
		for(i = 0; i < unzip_area.size(); i++)
		{
			memmove(p + mem_pt, unzip_area[i].m_buf, unzip_area[i].m_len);
			mem_pt += unzip_area[i].m_len;
		}
		for(i = 0; i < unzip_other_distri.size(); i++)
		{
			memmove(p + mem_pt, unzip_other_distri[i].m_buf, unzip_other_distri[i].m_len);
			mem_pt += unzip_other_distri[i].m_len;
		}
		for(i = 0; i < unzip_other.size(); i++)
		{
			memmove(p + mem_pt, unzip_other[i].m_buf, unzip_other[i].m_len);
			mem_pt += unzip_other[i].m_len;
		}
		return mem_sz;
	}
	else
		return 0;
}
bool CPOIInfo::place_street_speci(int city, int type, int idx, 
								  int& x,
								  int& y,
								  unsigned short& area)
{
	if (type != STREET_OTHER && type != PLACE_OTHER)
	{
		return false;
	}
	unsigned char* p = NULL;
	int len = LoadUnzipUnion(city, STREET_ACROSS, p);
	if (len == 0) return false;
	int* p_coord = (int*)(p + 4);
	short* p_admin = (short*)(p + *(int*)p + 4);

	x = *(p_coord + idx * 2);
	y = *(p_coord + idx * 2 + 1);

	area = *(p_admin + idx);

	return true;
}
int CPOIInfo::load_pl_st_speci(int city, int type)
{
	if (m_File.hang()) return 0;
	
	if(type != PLACE_OTHER || type != STREET_OTHER) return 0;
	
	pos_table table;
	m_File.seek(m_Head.navi_table + city * seg_navi_sz, BEGIN);
	if(m_File.read(&table, seg_navi_sz)) system_error();
	int pos = 0;
	pos = m_Head.entire_area_navi + table[type];
	m_File.seek(pos, BEGIN);
	char Head[26] = {0};
	
	if (m_File.read(Head, 26)) system_error();
	int mem_sz = 0;																	
	
	//--------------------------------------------------------------------------------坐标
	ushort pos_pack_num = *(Head + 8);
	
	int pos_mem_len = 0;
	pod_vector<b_char_mem> unzip_coord;
	
	if (pos_pack_num) 
		pos_mem_len = decode_zip_pack(m_File, pos_pack_num, unzip_coord);
	else
		return 0;
	//--------------------------------------------------------------------------------区划
	
	ushort area_pack_num = *(Head + 13);
	
	int area_mem_len = 0;
	pod_vector<b_char_mem> unzip_area;
	
	if (area_pack_num) 
		area_mem_len = decode_zip_pack(m_File, pos_pack_num, unzip_area);
	else
		return 0;
	//--------------------------------------------------------------------------------拼凑到一起
	unsigned char* p = NULL;
	mem_sz = 3 + pos_mem_len + area_pack_num;
	if(m_buf_mgr.alloc(mem_sz, city, type) && (p = m_buf_mgr.request(city, type, NULL)))
	{
		int mem_pt = 0;
		memmove(p, &pos_mem_len, 4);
		mem_pt += 4;
		unsigned int i = 0;
		for(i = 0; i < unzip_coord.size(); i++)
		{
			memmove(p + mem_pt, unzip_coord[i].m_buf, unzip_coord[i].m_len);
			mem_pt += unzip_coord[i].m_len;
		}
		
		for(i = 0; i < unzip_area.size(); i++)
		{
			memmove(p + mem_pt, unzip_area[i].m_buf, unzip_area[i].m_len);
			mem_pt += unzip_area[i].m_len;
		}
	}
	else
		return 0;
	return mem_sz;
}
/*
	0~3字节为字符串长度 指明后面的x字节为长度分布
	|398000000|adfadwerewqreqererqerte|
 */
int CPOIInfo::load_string(int city, int seg)
{
	if (m_File.hang()) return 0;
	//----------------------------------------------------------------纯粹为了代码整洁
	switch(seg) {
	case POI_SPELL:
		break;
	case POI_NAME:
		break;
	case STREET_SPELL:
		break;
	case STREET_NAME:
		break;
	case PLACE_NAME:
		break;
	case PLACE_SPELL:
		break;
	case SIGHT_NAME:
		break;
	case SIGHT_SPELL:
		break;
	default:
		return 0;
	}

	if (seg == SIGHT_SPELL) 
	{
		m_File.seek(m_Head.entire_sight_spell, BEGIN);
	}
	else if (seg == SIGHT_NAME) 
	{
		m_File.seek(m_Head.entire_sight_name, BEGIN);
	}
	else
	{
		pos_table table;
		m_File.seek(m_Head.navi_table + city * seg_navi_sz, BEGIN);
		
		if(m_File.read(&table, seg_navi_sz)) system_error();
		m_File.seek(m_Head.entire_area_navi + table[seg], BEGIN);
	}
	
	char head[22] = {0};
	if(m_File.read(head, 21)) system_error();
	uchar zip_pack_num = head[8];
	int rec_num = 0;
	rec_num = *(int*)(head + 4);

	int mem_sz = 0;
	mem_block<int> len_buf;
	len_buf.alloc(rec_num);
	if(m_File.read(len_buf.m_buf, sizeof(int) * rec_num)) system_error();
	mem_sz += sizeof(int) * rec_num;
	
	if (!zip_pack_num) 
	{
		return 0;
	}
	else
	{
		mem_sz += rec_num * sizeof(char);
		
		pod_vector<b_char_mem> unzip_pack;
		int pack_len = 0;
		if (pack_len = decode_zip_pack(m_File, zip_pack_num, unzip_pack)) 
		{
			mem_sz += pack_len;
			unsigned char* p = NULL;
			if(m_buf_mgr.alloc(mem_sz, city, seg) && (p = m_buf_mgr.request(city, seg, NULL)))
			{
				int mem_p = 0;
				memmove(p + mem_p, &len_buf.m_len, 4);
				mem_p += 4;
				memmove(p + mem_p, len_buf.m_buf, len_buf.m_len);
				mem_p += len_buf.m_len;
				
				for(int i = 0; i < zip_pack_num; i++)
				{
					memmove(p + mem_p, unzip_pack[i].m_buf, unzip_pack[i].m_len);
					mem_p += unzip_pack[i].m_len;
				}
			}
			else
				return 0;
		}
		
		unzip_pack.clear();
	}/*如果是压缩格式*/
	return mem_sz;
}
int CPOIInfo::decode_zip_pack(x_file& File, int pack_num, 
							  pod_vector<b_char_mem>& unzip_mem)
{
	pod_vector<b_char_mem> zip_block;
	if (unzip_mem.size() > 0) unzip_mem.clear();

	zip_block.capacity(pack_num);
	unzip_mem.capacity(pack_num);
	
	int mem_sz = 0;
	unsigned_mem len_distr_mem;
	len_distr_mem.alloc(pack_num+1);
	DWORD dwRead = 0;
	File.read(len_distr_mem.m_buf, sizeof(int) * (pack_num + 1));
	int* len_distr = (int*)len_distr_mem.m_buf;
	unsigned int i = 0;
	for(i = 0; i < pack_num; i++)
	{
		int pack_size = len_distr[i + 1] - len_distr[i];
		zip_block[i].alloc(pack_size);
					
		unsigned char* pzip = zip_block[i].m_buf;
		if(File.read(pzip, pack_size)) system_error();
					
		HDECODE hz = OpenDecode(pzip, pack_size, PASSWORD);
		DECODEENTRY ze;
		GetDecodeItem(hz, &ze);
		unzip_mem[i].alloc(ze.unc_size);
					
		DecodeItem(hz, 0, unzip_mem[i].m_buf, ze.unc_size);
		CloseDecode(hz);
					
		mem_sz += ze.unc_size;
	}
				
	for(i = 0; i < zip_block.size(); i++)
		zip_block[i].free();
	return mem_sz;
}

BOOL CPOIInfo::str_field(int city, int type, int idx, b_char_mem str_mem)
{
	if (type != POI_SPELL		&& type != POI_NAME 
		&& type != STREET_SPELL && type != STREET_NAME
		&& type != PLACE_NAME	&& type != PLACE_SPELL
		&& type != SIGHT_NAME) {
		return FALSE;
	}
	unsigned char* p = NULL;
	int len = LoadUnzipUnion(city, type, p);
	if (len == 0) return FALSE;
	
	unsigned char* len_distri = p + 4;
	//--------------------------------------------------------------------------计算str所在偏移
	int str_off = 0;
	int i = 0;
	for(i = 0; i < idx; i++)
	{
		str_off += len_distri[i];
	}
	//--------------------------------------------------------------------------定位str
	int* p_str = (int*)(p + *(int*)p + 4 + str_off);
	
	str_mem.alloc(len_distri[idx] + 1);
	memmove(str_mem.m_buf, p_str, len_distri[idx]);

	str_mem.m_buf[len_distri[idx]] = '\0';

	return TRUE;
}

void system_error(void)
{
	windows_error();
}
void windows_error(void)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
		);
	MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Windows致命错误"), MB_OK | MB_ICONINFORMATION );
	LocalFree( lpMsgBuf );
	ASSERT(0);
}
