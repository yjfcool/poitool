
poi代码风格全部改为小写


遇到这样的一个错误
在每个find函数中都有一个static变量记录当前地区跟poi类型

缺点 
内存更新太频繁
优点占的少 内存峰值约为7M 出现在上海

在查询字符串属性的时候全部载入了字符串
本来可以根据属性索引定位到具体数据包的
这样在连续查询的过程中始终不会重新载入。
但对于单次查询来说占内存较大。

同样交叉路口的查询中也全部载入了交叉路口

POI查询策略

1。字符串

把当前市的压缩包全部解压出来到内存中。
对解压出来的内存进行查找



2 最后个压缩包的最后一个字符串可能得不到匹配



2008-09-03
dlist中的node带有资源。本来是借助POP函数弹出后在手动删除。在加入智能指针后就不用
手动删除,所以dlist<UnionUnzip>类型只要调用clear就可以清除干净
同样在LoadUnzipUnion函数中不需要再额外调用一次去删除static变量了。
程序退出时会自动删除两个static 被智能指针托管的对象
值得注意的是智能指针不可以托管对象数组的。但我用它来托管了char数组。

分析POI最可能的操作
1.定位到一个地区 在该地区查询字符串 分类检索 交叉路口 切换地区后再次全部搜索
2.进行全国景点搜索
3.周边检索

对于1 POI拼音检索出idx数组		此时载入的字符串为poispell
	idx数组查询 poi名称 详细信息 此时载入字符串为poiname
									载入详细信息
	
	交叉路口检索出idx数组	载入交叉路口数据包
	idx数组查询 道路名 道路其他数据	载入poiname
										载入道路其他数据

	手写输入
	查询名称idx数组		载入poiname
	idx数组查询poi名称 详细信息 这里名称和详细信息都已经载入

当前策略:只保存当前字符串非压缩块和景点非压缩块
上海高德POI poi other峰值达到6.03M 这样整个POI堆内存可能到8M
概率不大的操作
1.切换到一个地区进行拼音检索后切换到另一个地区进行交叉路口检索

原来的poiaround模型好像是错的。
读了当前数据块和周围四个数据块。如果点落在当前数据块角上则应该读入角周围的4个
而不是当前点周围4个数据块

改进方法
1.内存映射文件
2.小内存的内存池
3.增加内存使用.加快速度

09-19 返回值dlist<int> 改为vector类型. 因为dlist[]操作很慢

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

1 把读写文件函数都该为fopen fread fwrite以脱离windows平台
2 去掉可能引起不稳定因素的东西。如dlist order_list，对于lemon tree应该保持其稳定性
3 增强auto_ptr

/********bug记录*******************************************************/

bug0	出现时刻:在当前字符串包的最后一个字符串时且当前指针位置加上包内最后一个字符串长度就等于包长度
		出现状态:假设当前字符串为"p烟酒公司",指针位于P位置
				 比较完p跟字串后对当前字符串索引做++。但并没有对当前指针位置做加运算导致用这个指针继续
				匹配子串