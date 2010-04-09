#ifndef _CDBFUnit_H_
#define _CDBFUnit_H_

/*
 *	T ��Ҫ��
	1.�ṩ�ȽϺ���
	2.�ṩ��ֵ����
	3.�Լ��ͷ��Լ�����Դ
 */
enum NODECOLOR
{
	BLACK	= 0,
	RED		= 1
};

template<class T>
struct RBTreeNode<T>
{
	RBTreeNode(T key)
	{
	}
	RBTreeNode<T>	*parent;
	RBTreeNode<T>	*left, *right;
	NODECOLOR		color;
	T				key;
};
typedef RBTreeNode* PRBTreeNode;


template<class T>
class RBTree
{
public:
	RBTree();
	~RBTree();
	PRBTreeNode RB_InsertNode(PRBTreeNode* root, T key);
	PRBTreeNode	RB_DeleteNode(PRBTreeNode& root, T key);
	void        RB_Mid_Visit(PRBTreeNode T);
	void        RB_Mid_DeleteTree(PRBTreeNode T);
	const int*  RB_Find_Node(PRBTreeNode root, T key);
	PRBTreeNode	RB_InsertNode_Fixup(PRBTreeNode& root, PRBTreeNode z);
	PRBTreeNode	RB_DeleteNode_Fixup(PRBTreeNode& root, PRBTreeNode z);
	void 		RB_Left_Rotate(PRBTreeNode A, PRBTreeNode& root);
	void		RB_Right_Rotate(PRBTreeNode A, PRBTreeNode& root);
};

template<class T>
PRBTreeNode RBTree<T>::RB_Find_Node(PRBTreeNode<T> root,T key)
{
	PRBTreeNode x = root;
	do {
		int compare = key - x->key;
		if (compare > 0) {
			if (NULL != x->left){
				x = x->left;
			}
			else{
				x = NULL;
				break;
			}
		}
		else if (compare < 0) {
			if (NULL != x->right){
				x = x->right;
			}
			else{
				x = NULL;
				break;
			}
		}
		else{
			break;
		}
	} while(NULL != x);

	return x;
}
template<class T>
void RBTree<T>::RB_Left_Rotate(PRBTreeNode A, PRBTreeNode& root)
{
	PRBTreeNode B;
	B = A->right;
	
	if (NULL == B)
		return;
	
	A->right  = B->left;
	if (NULL != B->left)
		B->left->parent = A;
	B->parent = A->parent;
	// ���������ж�����һ�������A->parent = NULL�����
	if (A == root)
	{
		root = B;
	}
	else if (A == A->parent->left)
	{
		A->parent->left = B;
	}
	else
	{
		A->parent->right = B;
	}
	B->left = A;
	A->parent = B;
}
template<class T>
void RBTree<T>::RB_Right_Rotate(PRBTreeNode A, PRBTreeNode& root)
{
	PRBTreeNode B;
	B = A->left;
	
	if (NULL == B)
		return;
	
	A->left   = B->right;
	if (NULL != B->right)
		B->right->parent = A;
	B->parent = A->parent;
	// ���������ж�����һ�������A->parent = NULL�����
	if (A == root)
	{
		root = B;
	}
	else if (A == A->parent->left)
	{
		A->parent->left = B;
	}
	else
	{
		A->parent->right = B;
	}
	A->parent = B;
	B->right  = A;
}
template<class T>
void RBTree<T>::RB_Mid_Visit(PRBTreeNode T)
{
	if (NULL != T)
	{
		if (NULL != T->left)
			RB_Mid_Visit(T->left);
//		Print_Node(T);
		if (NULL != T->right)
			RB_Mid_Visit(T->right);
	}
}
template<class T>
void RB_Mid_DeleteTree(PRBTreeNode T)
{
	if (NULL != T)
	{
		if (NULL != T->left)
			RB_Mid_DeleteTree(T->left);
		PRBTreeNode temp = T->right;
		delete(T);
		T = NULL;
		if (NULL != temp)
			RB_Mid_DeleteTree(temp);
	}
}

template<class T>
PRBTreeNode RBTree<T>::RB_InsertNode(PRBTreeNode root, T key)
{
	PRBTreeNode x, y;
	
	PRBTreeNode z;
	if (NULL == (z = new RBTreeNode(key)))
	{
		return NULL;
	}
	// �õ�z�ĸ��ڵ�
	x = root, y = NULL;
	while (NULL != x)
	{
		y = x;
        if (z->key < x->key)
        {
            if (NULL != x->left)
            {
                x = x->left;
            }
            else
            {
                break;
            }
        }
        else
        {
            if (NULL != x->right)
            {
                x = x->right;
            }
            else
            {
                break;
            }
        }
	}
	
	// ��z�ŵ����ʵ�λ��
	z->parent = y;
	if (NULL == y)
	{
		root = z;
	}
	else
	{
		if (z->key < y->key)
			y->left = z;
		else
			y->right = z;
	}
	// ����z����������Ϊ�ղ�����ɫ��red��ע���²���Ľڵ���ɫ����red
	z->left = z->right = NULL;
	z->color = RED;
	
	// �Ժ������������
	return  RB_InsertNode_Fixup(root, z);
}

PRBTreeNode RBTree<T>::RB_InsertNode_Fixup(PRBTreeNode& root, PRBTreeNode z)
{
	PRBTreeNode y;
	while (root != z && RED == z->parent->color)        // ��z���Ǹ�ͬʱ���ڵ����ɫ��red
	{
		if (z->parent == z->parent->parent->left)        // ���ڵ����游�ڵ��������
		{
			y = z->parent->parent->right;                        // yΪz�Ĳ����ڵ�
			if (NULL != y && RED == y->color)                // �����ڵ��������ɫ��red
			{
				z->parent->color = BLACK;                        // ����z�ĸ��ڵ���ɫ��B
				y->color = BLACK;                                        // ����z�Ĳ����ڵ���ɫ��B
				z->parent->parent->color = RED;                // ����z���游�ڵ���ɫ��B
				z = z->parent->parent;                                // ����zΪ�����游�ڵ�
			}
			else                                                                        // �޲����ڵ���߲����ڵ���ɫ��b
			{
				if (z == z->parent->right)                        // ����½ڵ��Ǹ��ڵ��������
				{
					z = z->parent;
					RB_Left_Rotate(z, root);
				}
				z->parent->color = BLACK;                        // �ı丸�ڵ���ɫ��B
				z->parent->parent->color = RED;                // �ı��游�ڵ���ɫ��R
				RB_Right_Rotate(z->parent->parent, root);
			}
		}
		else                                                                                // ���ڵ�Ϊ�游�ڵ��������
		{
			y = z->parent->parent->left;                        // yΪz�Ĳ����ڵ�
			if (NULL != y && RED == y->color)                // ���y����ɫ��red
			{
				z->parent->color = BLACK;                        // ���ĸ��ڵ����ɫΪB
				y->color = BLACK;                                        // ���Ĳ����ڵ����ɫ��B
				z->parent->parent->color = RED;                // �����游�ڵ���ɫ��R
				z = z->parent->parent;                                // ����zָ���游�ڵ�
			}               
			else                                                                        // y�����ڻ�����ɫ��B
			{
				if (z == z->parent->left)                        // ����Ǹ��ڵ��������
				{
					z = z->parent;
					RB_Right_Rotate(z, root);
				}
				z->parent->color = BLACK;                        // �ı丸�ڵ����ɫ��B
				z->parent->parent->color = RED;                // �ı��游�ڵ����ɫ��RED
				RB_Left_Rotate(z->parent->parent, root);
			}
		}
	}// while(RED == z->parent->color)
	
	// ���ڵ����ɫʼ�ն���B
	root->color = BLACK;
	
	return root;
}

template<class T>
PRBTreeNode RBTree<T>::RB_DeleteNode(PRBTreeNode& root, T key)
{
	PRBTreeNode x, y, z;
	int index = 0;

	z = RB_Find_Node(root, key);
	if (NULL == z)
		return NULL;

	// ��z��һ����������ʱ��,y == z
	// ����,y�Ǵ���z��С�Ľ��
	if (NULL == z->left || NULL == z->right)
		y = z;
	else
	{
		y = z->right;
		while (NULL != y->left)
			y = y->left;
	}
	
	// x��y������,����ΪNULL
	if (NULL != y->left)
		x = y->left;
	else
		x = y->right;
	
	// �趨x��λ��ȡ��y
	if (NULL != x)
		x->parent = y->parent;
	if (NULL == y->parent)
		root = x;
	else if (y == y->parent->left)
		y->parent->left = x;
	else
		y->parent->right = x;
	
	// ��y��key������z��,����y���Ǵ�ɾ���Ľ����
	if (y != z)
	{
		z->key = y->key;//bug
	}
	
	// ���y����ɫֵ��B,��ôҪ������������
	if (BLACK == y->color && NULL != x)
		RB_DeleteNode_Fixup(root, x);
	
	delete y;
	
	return root;
}
template<class T>
PRBTreeNode RBTree<T>::RB_DeleteNode_Fixup(PRBTreeNode& root, PRBTreeNode x)
{
	PRBTreeNode w;
	while (x != root && BLACK == x->color)
	{
		if (x == x->parent->left)                                                                // ���x��������
		{
			w = x->parent->right; 

			if (NULL == w)
				continue;
			
			if (RED == w->color)  
			{
				w->color = BLACK;
				x->parent->color = RED;
				RB_Left_Rotate(x->parent, root);
				w = x->parent->right;
			}
			if (NULL != w->left         && BLACK == w->left->color &&
				NULL != w->right && BLACK == w->right->color)
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{
				if (NULL != w->right && BLACK == w->right->color)
				{
					w->left->color = BLACK;
					w->color = RED;
					RB_Right_Rotate(w, root);
					w = x->parent->right;
				}
				
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color  = BLACK;
				RB_Left_Rotate(x->parent, root);
				x = root;
			}
		}
		else
		{
			w = x->parent->left;
			if (NULL == w)
				continue;
			if (RED == w->color)
			{
				w->color = BLACK;
				x->parent->color = RED;
				RB_Left_Rotate(x->parent, root);
				w = x->parent->left;
			}
			if (NULL != w->left         && BLACK == w->left->color &&
				NULL != w->right && BLACK == w->right->color)
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{
				if (NULL != w->left && BLACK == w->left->color)
				{
					w->right->color = BLACK;
					w->color = RED;
					RB_Left_Rotate(w, root);
					w = x->parent->left;
				}
				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color  = BLACK;
				RB_Right_Rotate(x->parent, root);
				x = root;
			}
		}
	}
	
	x->color = BLACK;
	return root;
}

#endif//#ifndef _CDBFUnit_H_

