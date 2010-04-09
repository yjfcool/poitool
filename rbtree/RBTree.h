#ifndef _CDBFUnit_H_
#define _CDBFUnit_H_

/*
 *	T 的要求：
	1.提供比较函数
	2.提供赋值函数
	3.自己释放自己的资源
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
	// 这样三个判断连在一起避免了A->parent = NULL的情况
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
	// 这样三个判断连在一起避免了A->parent = NULL的情况
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
	// 得到z的父节点
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
	
	// 把z放到合适的位置
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
	// 设置z的左右子树为空并且颜色是red，注意新插入的节点颜色都是red
	z->left = z->right = NULL;
	z->color = RED;
	
	// 对红黑树进行修正
	return  RB_InsertNode_Fixup(root, z);
}

PRBTreeNode RBTree<T>::RB_InsertNode_Fixup(PRBTreeNode& root, PRBTreeNode z)
{
	PRBTreeNode y;
	while (root != z && RED == z->parent->color)        // 当z不是根同时父节点的颜色是red
	{
		if (z->parent == z->parent->parent->left)        // 父节点是祖父节点的左子树
		{
			y = z->parent->parent->right;                        // y为z的伯父节点
			if (NULL != y && RED == y->color)                // 伯父节点存在且颜色是red
			{
				z->parent->color = BLACK;                        // 更改z的父节点颜色是B
				y->color = BLACK;                                        // 更改z的伯父节点颜色是B
				z->parent->parent->color = RED;                // 更改z的祖父节点颜色是B
				z = z->parent->parent;                                // 更新z为它的祖父节点
			}
			else                                                                        // 无伯父节点或者伯父节点颜色是b
			{
				if (z == z->parent->right)                        // 如果新节点是父节点的右子树
				{
					z = z->parent;
					RB_Left_Rotate(z, root);
				}
				z->parent->color = BLACK;                        // 改变父节点颜色是B
				z->parent->parent->color = RED;                // 改变祖父节点颜色是R
				RB_Right_Rotate(z->parent->parent, root);
			}
		}
		else                                                                                // 父节点为祖父节点的右子树
		{
			y = z->parent->parent->left;                        // y为z的伯父节点
			if (NULL != y && RED == y->color)                // 如果y的颜色是red
			{
				z->parent->color = BLACK;                        // 更改父节点的颜色为B
				y->color = BLACK;                                        // 更改伯父节点的颜色是B
				z->parent->parent->color = RED;                // 更改祖父节点颜色是R
				z = z->parent->parent;                                // 更改z指向祖父节点
			}               
			else                                                                        // y不存在或者颜色是B
			{
				if (z == z->parent->left)                        // 如果是父节点的左子树
				{
					z = z->parent;
					RB_Right_Rotate(z, root);
				}
				z->parent->color = BLACK;                        // 改变父节点的颜色是B
				z->parent->parent->color = RED;                // 改变祖父节点的颜色是RED
				RB_Left_Rotate(z->parent->parent, root);
			}
		}
	}// while(RED == z->parent->color)
	
	// 根节点的颜色始终都是B
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

	// 当z有一个空子树的时候,y == z
	// 否则,y是大于z最小的结点
	if (NULL == z->left || NULL == z->right)
		y = z;
	else
	{
		y = z->right;
		while (NULL != y->left)
			y = y->left;
	}
	
	// x是y的子树,可能为NULL
	if (NULL != y->left)
		x = y->left;
	else
		x = y->right;
	
	// 设定x的位置取代y
	if (NULL != x)
		x->parent = y->parent;
	if (NULL == y->parent)
		root = x;
	else if (y == y->parent->left)
		y->parent->left = x;
	else
		y->parent->right = x;
	
	// 把y的key拷贝到z中,这样y就是待删除的结点了
	if (y != z)
	{
		z->key = y->key;//bug
	}
	
	// 如果y的颜色值是B,那么要对树进行修正
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
		if (x == x->parent->left)                                                                // 如果x是左子树
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

