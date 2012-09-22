#include "Bloco.h"

template <class T>
T clamp(T val, T min, T max)
{
	if ( val < min ) { return min; }
	if ( val > max ) { return max; }

	return val;
}

// template <class T>
// T clamp(T val, T min, T max)
// {
// 	x = clamp<int>( x, 0, 1 );
// 	y = clamp<int>( y, 0, 1 );
// 	z = clamp<int>( z, 0, 1 );
// 
// 	return setChildren(4*x + 2*y + z, pData);
// }

template <class T>
class Octree;

template <class T>
class TreeNode
{

friend TreeNode;
friend Octree<T>;

protected:
	int				m_maxDepth;
	TreeNode<T>*	m_pChildren[8];
	shared_ptr<T>	m_pData;

	virtual void remove()
	{
		if(!m_pParent)
			return;

		if(!m_isLeaf)
		{
			for (int i = 0; i < 8; i++)
			{
				if(m_pChildren[i])
				{
					SAFE_DELETE(m_pChildren[i]);
				}
			}
		}

		m_pParent->removeChild(m_index);
		m_pParent = NULL;
	}
	TreeNode(int maxDepth, int index)
	{
		m_pParent = NULL;
		m_depth = 0;
		m_index = 0;
		m_isLeaf = false;
		m_maxDepth = maxDepth;
		m_pData = NULL;

		for (int i = 0; i < 8; i++)
		{
			m_pChildren[i] = NULL;
		}
	}

private:
	TreeNode<T>* m_pParent;
	bool	m_isLeaf;
	int		m_depth;
	int		m_index;
	
	inline void	setIndex(int index)					{m_index = index;}
	inline void	setDepth(int depth)					{m_depth = depth;}
	inline void	setMaxDepth(int maxDepth)			{m_maxDepth = maxDepth;}
	inline void	setIsLeaf(bool leaf)				{m_isLeaf = leaf; }
	inline void setParent(TreeNode<T>* pParent)		{m_pParent = pParent;}	

	void removeChild(int index)
	{
		m_pChildren[index] = NULL;
	}

public:
	virtual ~TreeNode() { remove(); }

	TreeNode<T>* setChildren(int x, int y, int z, shared_ptr<T> pData = nullptr)
	{
		return setChildren(4 * x + 2 * y + z, m_pData);
	}
	TreeNode<T>* setChildren(int index, shared_ptr<T> pData = nullptr)
	{
		if(m_isLeaf)
			return NULL;

		TreeNode<T>* pChild = DEBUG_CLIENTBLOCK TreeNode<T>(m_maxDepth, m_index);
		pChild->setParent(this);
		pChild->setIndex(index);
		pChild->setMaxDepth(m_maxDepth);
		pChild->setDepth(m_depth + 1);
		pChild->setData(pData);

		// if the depth is the max depth 
		// the tree node is a leaf
		if(pChild->getDepth() >= m_maxDepth)
			pChild->setIsLeaf(true);

		m_pChildren[index] = pChild;

		return pChild;
	}
	void		 setData(shared_ptr<T> pData){ m_pData = pData; }

	TreeNode<T>*		 getNextSibling()
	{
		if(!m_pParent)
			return NULL;

		TreeNode<T>** pSiblings = m_pParent->getChildren();
		
		int index = m_index + 1;
		if (index >= 8)
		{
			index -= 8;
		}

		return pSiblings[index];
	}
	TreeNode<T>*		 getPrevSibling()
	{
		if(!m_pParent)
			return NULL;

		TreeNode<T>** pSiblings = m_pParent->getChildren();
		int index = m_index - 1;

		if (index >= 8)
		{
			index += 8;
		}

		return pSiblings[index];
	}
	inline TreeNode<T>*  getChild(int x, int y, int z)	{return getChild(4 * x + 2 * y + z);}
	inline TreeNode<T>*  getChild(int index)			{return m_pChildren[index];}
	inline TreeNode<T>*  getParent()					{return m_pParent;}
	inline TreeNode<T>** getChildren()					{return m_pChildren;}
	inline int			 getDepth()						{return m_depth;}
	inline bool			 getIsLeaf()					{return m_isLeaf;}
	inline shared_ptr<T> getData()						{return m_pData;}
};

template <class T>
class Octree : public TreeNode<T>
{
public:
	Octree(int maxDepth) : TreeNode<T>(maxDepth, 0) {}
	~Octree() {remove();}
	void remove()
	{
		for (int i = 0; i < 8; i++)
		{
			if(m_pChildren[i])
			{
				SAFE_DELETE(m_pChildren[i]);
			}
		}
	}
};
