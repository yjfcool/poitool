#ifndef GEO_PANE_H_
#define GEO_PANE_H_
/*
 *	以二维索引形式来区分每个地理方格数据包 给定点能返回其对应的索引
	其中包含了坐标范围和方块划分方法

	输入 四个角点- build(pt1, pt2)
	输出 没有被填充的方格和这些方格对应的索引 -if [x] == NULL load_mem(x)

 */
class geo_pane
{
public:
	geo_pane(unsigned int x_begin, unsigned int x_end, unsigned int y_begin, unsigned int y_end,
						unsigned int x_pane_num, unsigned int y_pane_num)
																		:m_x_begin(x_begin), 
																		m_x_end(x_end),
																		m_y_begin(y_begin),
																		m_y_end(y_end),
																		m_x_pane_num(x_pane_num),
																		m_y_pane_num(y_pane_num),
																		m_pane_width((m_x_end - m_x_begin)/m_x_pane_num),
																		m_pane_height((m_y_end - m_y_begin)/m_y_pane_num)
	{
	}
	~geo_pane()
	{
	}
	//给定一个范围返回索引数组
	BOOL bnd_idx(POINT pt, unsigned radius, unsigned& x_idx, unsigned& y_idx, 
												unsigned& x_dem, unsigned& y_dem);
	//根据二维索引返回一维索引
	int  index(int x_idx, int y_idx);
	void dem(unsigned int& x_dem, unsigned int& y_dem)
	{
		x_dem = m_x_pane_num;
		y_dem = m_y_pane_num;
	}
	//方格左下角实际坐标
	BOOL pane_coord(int idx, int& x, int& y);
private:
	unsigned int m_x_begin;
	unsigned int m_x_end;
	unsigned int m_y_begin;
	unsigned int m_y_end;
	unsigned int m_x_pane_num;
	unsigned int m_y_pane_num;
	unsigned int m_pane_width;
	unsigned int m_pane_height;
};

#endif