
#include "stdafx.h"
#include <stdio.h>
#include <ctype.h>

#include "h_trie.h"
#include "alloc.h"
static char keyarray[256];

/*-------------------------------
* usage: ��ʼ����ֵ����
* comment:��charӳ���0-9����ֵ
*-------------------------------*/
void InitHTrie (trie_t **trie)
{
	int c;
	
	for (c = 0; c < 256; c )
	{
		if (isdigit (c))
			keyarray[c] = c - '0';
		else
			keyarray[c] = c;
	}
	
	*trie = NULL;
}

static trie_t * NewNode ()
{
	int i;
	trie_t *node = (trie_t *)allocate (sizeof (trie_t));
	
	if (node)
	{
		node->list = NULL;
		for (i = 0; i < TRIE_FANOUT; i )
		{
			node->subchanged[i] = 0;
			node->listchanged = 0;
			node->subtrie[i] = NULL;
		}
	}
	
	if (node == NULL)
		pr_error("errorcode:%d,msg:NewNode: allocate() return NULL\n");
	
	return node;
}

/*--------------------------------
* usage: ������в���һ���µ�����
* arguments: *trie -- ����ͷָ��
* str -- ��ֵ�ַ���
* level -- ��ֵ����
* data -- Ҫ���������
* n -- Ҫ��������ݵĴ�С
* changed - ��¼��ǰ�ڵ���ӽڵ�������Ƿ����˱仯
* 1 -- �б仯 0 -- �ޱ仯
* return: -1 -- ���� 0 -- ����
* comment��������Ҷ�ڵ���������������ʱ���ȸ��ݼ�ֵ�ҵ�Ҷ�ڵ㣬����
* Ҷ�ڵ���ָ�������в������ݡ�
*---------------------------------*/
int InsertHTrie (trie_t **trie, const char str[], const int level,
				 void *data, size_t n, int changed)
{
	int i;
	int key;
	trie_t *curnode;
	
	if (*trie == NULL)
	{
		*trie = NewNode ();
		if (*trie == NULL) {
			return -1;
		}
	}
	
	curnode = *trie;
	for (i = 0; i < level ; i )
	{
		key = (int) keyarray[(int)str[i]];
		if (curnode->subtrie[key] == NULL)
		{
			if ((curnode->subtrie[key] = NewNode ()) == NULL)
				return -1;
		}
		curnode->subchanged[key] = changed;
		
		curnode = curnode->subtrie[key];
	}
	
	curnode->listchanged = changed;
	
	return (InsertList (&(curnode->list), data, n));
}

/*--------------------------------
* usage: �ڼ����в�������
* arguments: trie -- ����ͷָ��
* str -- ��ֵ�ַ���
* level -- ��ֵ����
* data -- Ҫ���ҵ�����
* cmp -- �ȽϺ���ָ��
* return: �ҵ� -- ����ָ������ݵ�ָ�� û�ҵ� -- NULL
* comment:���ҹ�����cmp����ָ��
*---------------------------------*/
void * SearchHTrie (trie_t *trie, const char str[], const int level, void *data,
					int (*cmp) (const void *data1, const void *data2))
{
	int i;
	int key;
	trie_t *curnode;
	
	if (trie == NULL)
		return NULL;
	
	curnode = trie;
	for (i = 0; i < level ; i )
	{
		key = (int) keyarray[ (int)str[i] ];
		if (curnode->subtrie[key] == NULL)
			return NULL;
		
		curnode = curnode->subtrie[key]; 
	}
	
	return (SearchList (curnode->list, data, cmp));
}

/*--------------------------------
* usage: �ڼ����в��Ҽ�ֵָ�����ͷ�����������Ľڵ��changed�ֶ���1
* ��ʾ�ýڵ���ӽڵ�Ҫ�����仯����ڵ㲻���ڣ������ɸýڵ�
* arguments: trie -- ����ͷָ��
* str -- ��ֵ�ַ���
* level -- ��ֵ����
* list -- ����ָ����ͷlistָ���ָ�룬����Ҫ����ָ���ָ�룬
* ʹ��3��ָ��
* return: �ҵ� -- ����ָ�������ͷ��ָ�� û�ҵ� -- NULL
* comment:
*---------------------------------*/
int TouchHTrie (trie_t **trie, const char str[], const int level, list_t ***list)
{
	int i;
	int key;
	trie_t *curnode;
	
	if (*trie == NULL)
	{
		*trie = NewNode ();
		if (*trie == NULL) {
			pr_error("errorcode:%d,msg:TouchHTrie:NewNode () return NULL\n");
			
			return -1;
		}
	}
	
	curnode = *trie;
	for (i = 0; i < level ; i )
	{
		key = (int) keyarray[ (int)str[i]];
		if (curnode->subtrie[key] == NULL)
		{
			if ((curnode->subtrie[key] = NewNode ()) == NULL) {
				pr_error("errorcode:%d,msg:NewNode () error\n");
				return -1;
			}
		}
		curnode->subchanged[key] = 1;
		
		curnode = curnode->subtrie[key];
	}
	
	curnode->listchanged = 1;
	
	*list = &(curnode->list);
	
	return 0;
}

/*-------------------------------------------
*
*-------------------------------------------*/
list_t *GetListofHTrie (trie_t *trie, const char str[], const int level)
{
	int i;
	int key;
	trie_t *curnode;
	
	if (trie == NULL)
		return NULL;
	
	curnode = trie;
	for (i = 0; i < level ; i )
	{
		key = (int) keyarray[(int)str[i]];
		if (curnode->subtrie[key] == NULL)
			return NULL;
		
		curnode = curnode->subtrie[key];
	}
	
	return curnode->list;
}


list_t *MatchHTrie (trie_t *trie, const char str[], const int level)
{
	int i;
	int key;
	trie_t *curnode;
	
	if (trie == NULL)
		return NULL;
	
	curnode = trie;
	
	for (i = 0; i < level ; i )
	{
		key = (int) keyarray[(int)str[i]];
		if (curnode->subtrie[key] == NULL)
			return curnode->list;
		
		curnode = curnode->subtrie[key];
	}
	
	return curnode->list;
}

/*-------------------------------
* usage: �ͷż���
* arguments: trie -- the head of trie
*-------------------------------*/
void FreeHTrie (trie_t *trie)
{
	int i;
	
	if (trie)
	{
		for (i = 0; i < TRIE_FANOUT; i )
			FreeHTrie (trie->subtrie[i]);
		FreeList (trie->list);
		free (trie);
	}
}

/*----------------------------------
* usage: print the data of the trie
*----------------------------------*/
void PrintHTrie (trie_t *trie, const int level, const int key, void (*print) (const void *data))
{
	int i;
	
	if (trie)
	{
		fprintf (stderr, "enter subtrie -- level:%d,key:%d\n", level, key);
		for (i = 0; i < TRIE_FANOUT; i )
		{
			PrintHTrie (trie->subtrie[i], level 1, i, print);
		}
		PrintList (trie->list, print);
	}
}
/*
void OperateHTrie (trie_t *trie, void (* op_list) (void *data))
{

}
*/

/*------------------------------------------
* usage: ˢ��TRIE����changedΪ1�Ľڵ���ӽڵ��������op_listָ���Ĳ���
* parameters: trie -- trie head pointer
* op_list-- ��list�Ĳ���
*------------------------------------------*/
void RefreshHTrie (trie_t *trie, void (* op_data) (void *data))
{
	int i;
	
	if (trie)
	{
		for (i = 0; i < TRIE_FANOUT; i )
		{
			if (trie->subchanged[i]) /* �ӽڵ㷢�����仯 */
			{
				RefreshHTrie (trie->subtrie[i], op_data);
				trie->subchanged[i] = 0;
			}
		}
		if (trie->listchanged)
		{
			OperateList (trie->list, op_data);
			trie->listchanged = 0;
		}
	}
}

int NeedRefresh (trie_t *trie)
{
	int i;
	
	if (trie)
	{
		for (i = 0; i < TRIE_FANOUT; i )
		{
			if (trie->subchanged[i]) /* �ӽڵ㷢�����仯 */
			{
				return 1;
			}
		}
		if (trie->listchanged)
		{
			return 1;
		}
	}
	
	return 0;
}

/*
* ���ܣ� TRIE��������������
*
* ע�� �ڵ�������ж�
*
* ���� 0 δִ����� 1 ִ�����
*/
int DealHTrie (trie_t *trie, int (* op_data) (void *data))
{
int i;

if (trie)
{
for (i = 0; i < TRIE_FANOUT; i )
{
if (trie->subchanged[i]) /* �ӽڵ㷢�����仯 */
{
/* �ֽڵ�����жϣ� ���� 0 */
if (DealHTrie (trie->subtrie[i], op_data) == 0)
return 0;

trie->subchanged[i] = 0;
}
}
if (trie->listchanged)
{
if (DealList (trie->list, op_data) == 0)
return 0;
trie->listchanged = 0;
}
}

return 1;
}