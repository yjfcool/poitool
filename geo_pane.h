#ifndef GEO_PANE_H_
#define GEO_PANE_H_
/*
 *	�Զ�ά������ʽ������ÿ�����������ݰ� �������ܷ������Ӧ������
	���а��������귶Χ�ͷ��黮�ַ���

	���� �ĸ��ǵ�- build(pt1, pt2)
	��� û�б����ķ������Щ�����Ӧ������ -if [x] == NULL load_mem(x)

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
	//����һ����Χ������������
	BOOL bnd_idx(POINT pt, unsigned radius, unsigned& x_idx, unsigned& y_idx, 
												unsigned& x_dem, unsigned& y_dem);
	//���ݶ�ά��������һά����
	int  index(int x_idx, int y_idx);
	void dem(unsigned int& x_dem, unsigned int& y_dem)
	{
		x_dem = m_x_pane_num;
		y_dem = m_y_pane_num;
	}
	//�������½�ʵ������
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