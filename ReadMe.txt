
poi������ȫ����ΪСд


����������һ������
��ÿ��find�����ж���һ��static������¼��ǰ������poi����

ȱ�� 
�ڴ����̫Ƶ��
�ŵ�ռ���� �ڴ��ֵԼΪ7M �������Ϻ�

�ڲ�ѯ�ַ������Ե�ʱ��ȫ���������ַ���
�������Ը�������������λ���������ݰ���
������������ѯ�Ĺ�����ʼ�ղ����������롣
�����ڵ��β�ѯ��˵ռ�ڴ�ϴ�

ͬ������·�ڵĲ�ѯ��Ҳȫ�������˽���·��

POI��ѯ����

1���ַ���

�ѵ�ǰ�е�ѹ����ȫ����ѹ�������ڴ��С�
�Խ�ѹ�������ڴ���в���



2 ����ѹ���������һ���ַ������ܵò���ƥ��



2008-09-03
dlist�е�node������Դ�������ǽ���POP�������������ֶ�ɾ�����ڼ�������ָ���Ͳ���
�ֶ�ɾ��,����dlist<UnionUnzip>����ֻҪ����clear�Ϳ�������ɾ�
ͬ����LoadUnzipUnion�����в���Ҫ�ٶ������һ��ȥɾ��static�����ˡ�
�����˳�ʱ���Զ�ɾ������static ������ָ���йܵĶ���
ֵ��ע���������ָ�벻�����йܶ�������ġ������������й���char���顣

����POI����ܵĲ���
1.��λ��һ������ �ڸõ�����ѯ�ַ��� ������� ����·�� �л��������ٴ�ȫ������
2.����ȫ����������
3.�ܱ߼���

����1 POIƴ��������idx����		��ʱ������ַ���Ϊpoispell
	idx�����ѯ poi���� ��ϸ��Ϣ ��ʱ�����ַ���Ϊpoiname
									������ϸ��Ϣ
	
	����·�ڼ�����idx����	���뽻��·�����ݰ�
	idx�����ѯ ��·�� ��·��������	����poiname
										�����·��������

	��д����
	��ѯ����idx����		����poiname
	idx�����ѯpoi���� ��ϸ��Ϣ �������ƺ���ϸ��Ϣ���Ѿ�����

��ǰ����:ֻ���浱ǰ�ַ�����ѹ����;����ѹ����
�Ϻ��ߵ�POI poi other��ֵ�ﵽ6.03M ��������POI���ڴ���ܵ�8M
���ʲ���Ĳ���
1.�л���һ����������ƴ���������л�����һ���������н���·�ڼ���

ԭ����poiaroundģ�ͺ����Ǵ�ġ�
���˵�ǰ���ݿ����Χ�ĸ����ݿ顣��������ڵ�ǰ���ݿ������Ӧ�ö������Χ��4��
�����ǵ�ǰ����Χ4�����ݿ�

�Ľ�����
1.�ڴ�ӳ���ļ�
2.С�ڴ���ڴ��
3.�����ڴ�ʹ��.�ӿ��ٶ�

09-19 ����ֵdlist<int> ��Ϊvector����. ��Ϊdlist[]��������

/*
void CPOIInfo::LoadUnzipUnion(int city, int type, 
							  dlist<UnionUnzip>** str_union, char** len_table, int& str_num)
{
	static char* str_len_table = NULL;
	int* pack_pos_table = NULL;
	static dlist<UnionUnzip> unzip_str_union;
	DWORD dwRead = 0;
	
	if (str_len_table)
		delete str_len_table;
	if(pack_pos_table)
		delete pack_pos_table;
	if (unzip_str_union.size() > 0) 
	{
		int size = unzip_str_union.size();
		for(int i = 0; i < size; i++)
		{
			dnode<UnionUnzip>* p = unzip_str_union.pop();
			UnionUnzip core = p->core();
			delete core.data;
			delete p;
		}
	}
	if(str_union == NULL || len_table == NULL)
		return;
	GoTo(m_PoiHandle, type, city);
	char head[22] = {0};
	ReadFile(m_PoiHandle, head, 21, &dwRead, NULL);
	if(dwRead != 21) windows_error();
	uchar zip_pack_num = READ1BYTE(head, 8);
	str_num = READ4BYTES(head, 4);
	if (!zip_pack_num) 
	{
	}
	else
	{
		UnionUnzip str_block;
		str_len_table = new char[str_num];
		if(!str_len_table) windows_error();
		ReadFile(m_PoiHandle, str_len_table, 
			str_num * sizeof(char), &dwRead, NULL);
		if(dwRead != str_num * sizeof(char)) windows_error();
		pack_pos_table = new int[zip_pack_num+1];
		if(!pack_pos_table) windows_error();
		ReadFile(m_PoiHandle, pack_pos_table, (zip_pack_num+1) * sizeof(int)
			,&dwRead, NULL);
		if(dwRead != (zip_pack_num+1) * sizeof(int)) windows_error();
		for(int i = 0; i < zip_pack_num; i++)
		{
			char* zip_str = NULL;
			int pack_size = pack_pos_table[i+1] - pack_pos_table[i];
			if(!zip_str)
				zip_str = new char[pack_size];
			if(!zip_str) windows_error();
			
			ReadFile(m_PoiHandle, zip_str, pack_size, &dwRead, NULL);
			if(pack_size != dwRead) windows_error();
			HDECODE hz = OpenDecode(zip_str, pack_size, "Jd&52Kp*dc");
			DECODEENTRY ze;
			GetDecodeItem(hz, &ze);
			int unzip_len = ze.unc_size;
			char* unzip_str = NULL;
			unzip_str = new char[unzip_len];
			if(!unzip_str) windows_error();
			DecodeItem(hz, 0, unzip_str, unzip_len);
			CloseDecode(hz);
			str_block.data = unzip_str;
			str_block.len = unzip_len;
			unzip_str_union.add(str_block);
			delete zip_str;
		}
	}
	delete pack_pos_table;
	*str_union = &unzip_str_union;
	*len_table = str_len_table;
}*/

1 �Ѷ�д�ļ���������Ϊfopen fread fwrite������windowsƽ̨
2 ȥ�����������ȶ����صĶ�������dlist order_list������lemon treeӦ�ñ������ȶ���
3 ��ǿauto_ptr

/********bug��¼*******************************************************/

bug0	����ʱ��:�ڵ�ǰ�ַ����������һ���ַ���ʱ�ҵ�ǰָ��λ�ü��ϰ������һ���ַ������Ⱦ͵��ڰ�����
		����״̬:���赱ǰ�ַ���Ϊ"p�̾ƹ�˾",ָ��λ��Pλ��
				 �Ƚ���p���ִ���Ե�ǰ�ַ���������++������û�жԵ�ǰָ��λ���������㵼�������ָ�����
				ƥ���Ӵ�