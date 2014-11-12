#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "index.h"

void CINode::_Init(CITree * lpITree)
{
#ifdef INODE_DEBUG
	printf("CINode::CINode(%p) %p\n", lpITree, this);
#endif

	m_lVal = 0;
	m_lpITree = (CITree*)NULL;
	m_lpParent = lpITree;
}

CINode::~CINode()
{
#ifdef INODE_DEBUG
	printf("CINode::~CINode() %p\n", this);
#endif

	if(m_lpITree)
		delete m_lpITree;
}

CINode *CINode::_Insert(uchar * lpszKey, int nKey)
{
#ifdef INODE_DEBUG
	printf("CINode::_Insert(%p, %d) %p\n", lpszKey, nKey, this);
#endif

	if(!m_lpITree)
		m_lpITree = new CITree(m_lpParent);

	return m_lpITree->_Insert(lpszKey, nKey);
}

bool CINode::_Delete(uchar * lpszKey)
{
#ifdef INODE_DEBUG
	printf("CINode::_Delete(%p) %p\n", lpszKey, this);
#endif

	if(m_lpITree == (CITree*)NULL)
		return true;

	return m_lpITree->_Delete(lpszKey);
}

CINode *CINode::_Find(uchar * lpszKey, int nKey)
{
#ifdef INODE_DEBUG
	printf("CINode::_Find(%p, %d) %p\n", lpszKey, nKey, this);
#endif

	if(!m_lpITree)
		return (CINode*)NULL;

	return m_lpITree->_Find(lpszKey, nKey);
}

CINode * CINode::Ascending(ulong *lplVal)
{
#ifdef INODE_DEBUG
	printf("CINode::Ascending(%p) %p\n", lplVal, this);
#endif

	if(m_lpITree)
		return m_lpITree->Ascending(lplVal);

	return this;
}

CINode * CINode::Descending(ulong *lplVal)
{
#ifdef INODE_DEBUG
	printf("CINode::Descending(%p) %p\n", lplVal, this);
#endif

	if(m_lpITree)
		return m_lpITree->Descending(lplVal);

	return this;
}

void CITree::_Init(CITree * lpITree)
{
#ifdef ITREE_DEBUG
	printf("CITree::_Init(%p) %p\n", lpITree, this);
#endif

	m_lNodes = 0;

	for(short i = 0; i < 256; i++)
		m_lpINode[i] = (CINode*)NULL;

	m_nNext = 0;
	m_nPrevious = 255;
	m_lpParent = lpITree;
}

CITree::~CITree()
{
#ifdef ITREE_DEBUG
	printf("CITree::~CITree() %p\n", this);
#endif

	for(short i = 0; i < 256; i++)
		if(m_lpINode[i])
			delete m_lpINode[i];
}

CINode * CITree::_Insert(uchar * lpszKey, int nKey)
{
#ifdef ITREE_DEBUG
	printf("CITree::_Insert(%p, %d) %p\n", lpszKey, nKey, this);
#endif

	uchar chKey = *lpszKey++;
	nKey--;

	if(m_lpINode[(int)chKey] == (CINode*)NULL)
	{
		m_lNodes++;
		m_lpINode[(int)chKey] = new CINode(this);
	}
	
	if(nKey)
		return m_lpINode[(int)chKey]->_Insert(lpszKey, nKey);

	return m_lpINode[(int)chKey];
}

bool CITree::_Delete(uchar * lpszKey)
{
#ifdef ITREE_DEBUG
	printf("CITree::_Delete(%p) %p\n", lpszKey, this);
#endif

	uchar chKey = *lpszKey++;

	if(!m_lpINode[chKey]->_Delete(lpszKey))	
		return false;

	delete m_lpINode[chKey];
	m_lpINode[chKey] = (CINode*)NULL;

	return !--m_lNodes;
}	

CINode * CITree::_Find(uchar *lpszKey, int nKey)
{
#ifdef ITREE_DEBUG
	printf("CITree::_Find(%p, %d) %p\n", lpszKey, nKey, this);
#endif

	uchar chKey = *lpszKey++;
	nKey--;

	if(!m_lpINode[(int)chKey])
		return (CINode*)NULL;

	if(nKey)
		return m_lpINode[(int)chKey]->_Find(lpszKey, nKey);

	return m_lpINode[(int)chKey];
}

CINode * CITree::Ascending(ulong *lplVal)
{
#ifdef ITREE_DEBUG
	printf("CITree::Ascending(%p) %p\n", lplVal, this);
#endif

	while(m_nNext < 256)
	{
		if(m_lpINode[m_nNext])
			return m_lpINode[m_nNext++]->Ascending(lplVal);

		m_nNext++;
	}

	m_nNext = 0;

	if(m_lpParent)
		return m_lpParent->Ascending(lplVal);

	return (CINode*)NULL;
}

CINode * CITree::Descending(ulong *lplVal)
{
#ifdef ITREE_DEBUG
	printf("CITree::Descending(%p) %p\n", lplVal, this);
#endif

	while(m_nPrevious >= 0)
	{
		if(m_lpINode[m_nPrevious])
			return m_lpINode[m_nPrevious--]->Descending(lplVal);

		m_nPrevious--;
	}

	m_nPrevious = 255;

	if(m_lpParent)
		return m_lpParent->Descending(lplVal);

	return (CINode*)NULL;
}

void _Str2Pre(const char *lpszKey, char * lpszTmp)
{
	int nTmp;
	nTmp = MIN(strlen(lpszKey), 255);

	*lpszTmp++ = nTmp;

	while(nTmp--)
		*lpszTmp++ = *lpszKey++;
}

void _Pre2Str(const char *lpszKey, char *lpszTmp)
{
	int nTmp;

	nTmp = *lpszKey++;

	while(nTmp--)
		*lpszTmp++ = *lpszKey++;

	*lpszTmp = '\0';
}

CIndex::CIndex()
{
	m_lpCITree = new CITree;
	m_lpNext = (CITree*)NULL;
}

CIndex::~CIndex()
{
	delete m_lpCITree;
}

void CIndex::Insert(ulong lKey, const ulong lVal)
{
#ifdef ITREE_DEBUG
	printf("CIndex::Insert(%ld, %ld) %p\n", lKey, lVal, this);
#endif

	m_lpCITree->_Insert((uchar*)&lKey, sizeof(ulong))->m_lVal = lVal;
}

bool CIndex::Delete(ulong lKey)
{
#ifdef ITREE_DEBUG
	printf("CIndex::Delete(%ld) %p\n", lKey, this);
#endif

	ulong lTmp;

	if(!Find(lKey, &lTmp))
		return false;

	m_lpCITree->_Delete((uchar*)&lKey);
	return true;
}

bool CIndex::Find(ulong lKey, ulong * lplVal)
{
#ifdef ITREE_DEBUG
	printf("CITree::Find(%ld) %p\n", lKey, this);
#endif

	CINode * lpINode;

	lpINode =  m_lpCITree->_Find((uchar*)&lKey, sizeof(ulong));

	if(!lpINode)
		return 0;

	*lplVal = lpINode->m_lVal;
	return true;
}

void CIndex::Insert(char *lpszKey, const ulong lVal)
{
#ifdef ITREE_DEBUG
	printf("CITree::Insert(%s, %ld) %p\n", lpszKey, lVal, this);
#endif
	
	char szTmp[256];
	
	_Str2Pre(lpszKey, szTmp);
	m_lpCITree->_Insert((uchar*)szTmp,(int)*szTmp + 1)->m_lVal = lVal;
}

bool CIndex::Delete(char *lpszKey)
{
#ifdef ITREE_DEBUG
	printf("CITree::Delete(%s) %p\n", lpszKey, this);
#endif

	char szTmp[256];
	ulong lTmp;

	if(!Find(lpszKey, &lTmp))
		return false;

	_Str2Pre(lpszKey, szTmp);
	m_lpCITree->_Delete((uchar*)szTmp);
	return true;
}

bool CIndex::Find(char * lpszKey, ulong * lplVal)
{
#ifdef ITREE_DEBUG
	printf("CITree::Find(%s) %p\n", lpszKey, this);
#endif

	char szTmp[256];
	CINode *lpINode;

	_Str2Pre(lpszKey, szTmp);
	lpINode =  m_lpCITree->_Find((uchar*)szTmp, (int) *szTmp + 1);

	if(!lpINode)
		return 0;

	*lplVal = lpINode->m_lVal;
	return true;
}

void CIndex::Reset()
{
	m_lpNext = m_lpCITree;
}

bool CIndex::Ascending(ulong *lplVal)
{
	CINode *lpINode;

	lpINode = m_lpNext->Ascending(lplVal);

	if(!lpINode)
		return false;

	m_lpNext = lpINode->m_lpParent;
	*lplVal = lpINode->m_lVal;		

	return true;
}

bool CIndex::Descending(ulong *lplVal)
{
	CINode *lpINode;

	lpINode = m_lpNext->Descending(lplVal);

	if(!lpINode)
		return false;

	m_lpNext = lpINode->m_lpParent;
	*lplVal = lpINode->m_lVal;		

	return true;
}

void CIndex::_AscendingKey(uchar *lpszKey)
{
	int nSize;
	uchar szTmp[256];
	uchar *lpszTmp = szTmp;

	CITree *lpITree;

	lpITree = m_lpNext;

	nSize = 0;
	while(lpITree)
	{
		*lpszTmp++ = lpITree->m_nNext - 1;
		lpITree = lpITree->m_lpParent; 
		nSize++;
	}

	while(nSize--)
		*lpszKey++ = *--lpszTmp;
}

void CIndex::_DescendingKey(uchar *lpszKey)
{
	int nSize;
	uchar szTmp[256];
	uchar *lpszTmp = szTmp;

	CITree *lpITree;

	lpITree = m_lpNext;

	nSize = 0;
	while(lpITree)
	{
		*lpszTmp++ = lpITree->m_nPrevious + 1;
		lpITree = lpITree->m_lpParent; 
		nSize++;
	}

	while(nSize--)
		*lpszKey++ = *--lpszTmp;
}

void CIndex::AscendingKey(ulong *lplKey)
{
	_AscendingKey((uchar*)lplKey);
}

void CIndex::DescendingKey(ulong *lplKey)
{
	_DescendingKey((uchar*)lplKey);
}

void CIndex::AscendingKey(char *lpszKey)
{
	char szTmp[256];

	_AscendingKey((uchar*)szTmp);
	_Pre2Str(szTmp, lpszKey);
}

void CIndex::DescendingKey(char *lpszKey)
{
	char szTmp[256];

	_DescendingKey((uchar*)szTmp);
	_Pre2Str(szTmp, lpszKey);
}

