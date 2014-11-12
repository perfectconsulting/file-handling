#ifndef INDEX_INCL
#define INDEX_INCL

#include "glodef.h"

//#define ITREE_DEBUG
//#define INODE_DEBUG

class CITree;
class CINode;
class CIndex;

class CINode
{
public:
	CINode()							{ _Init((CITree*)NULL); }
	CINode(CITree *lpITree)				{ _Init(lpITree); }
	~CINode();
		
	void _Init(CITree *);

	CINode * _Insert(uchar *, int);
	bool _Delete(uchar*);
	CINode * _Find(uchar *, int);

	CINode * Ascending(ulong *);
	CINode * Descending(ulong *);

	CITree *m_lpITree;
	ulong m_lVal;

	CITree *m_lpParent;
};

class CITree
{
public:
	CITree()							{ _Init((CITree*)NULL); }
	CITree(CITree *lpITree)				{ _Init(lpITree); }
	~CITree();

	void _Init(CITree* lpITree);

	CINode * _Insert(uchar *, int);
	bool _Delete(uchar *);
	CINode * _Find(uchar *, int);

	CINode * Ascending(ulong *);
	CINode * Descending(ulong *);

	long m_lNodes;
	CINode *m_lpINode[256];

	int m_nNext;
	int m_nPrevious;
	CITree *m_lpParent;
};

class CIndex
{
public:
	CIndex();
	~CIndex();

	void Insert(ulong, const ulong);
	bool Delete(ulong);
	bool Find(ulong, ulong *);

	void Insert(char *, const ulong);
	bool Delete(char *);
	bool Find(char *, ulong *);

	void Reset();
	bool Ascending(ulong *);
	bool Descending(ulong *);

	void AscendingKey(char *);
	void DescendingKey(char *);
	void AscendingKey(ulong *);
	void DescendingKey(ulong *);

private:
	void _AscendingKey(uchar *);
	void _DescendingKey(uchar *);

	CITree * m_lpCITree;
	CITree * m_lpNext;
};

#endif
