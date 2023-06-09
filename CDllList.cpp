/*
MIT License

Copyright (c) 2000-2023 Åke Hedman, Grodans Paradis AB
Copyright (c) 2005-2023 Gediminas Simanskis, Rusoku technologijos UAB (gediminas@rusoku.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "windows.h"
#include "include/CDllList.h"


///////////////////////////////////////////////////////////////////////////////
// init_list

CDllList::CDllList(){
}

CDllList::~CDllList(){
}

void CDllList::Init(struct DoubleLinkedList *pdll, unsigned char nSort)
{
	pdll->pHead = nullptr;
	pdll->pTail = nullptr;
	pdll->nSortOrder = nSort;
	pdll->nCount = 0;
};

///////////////////////////////////////////////////////////////////////////////
// addNodeFront  -  AKHE

BOOL CDllList::AddNodeHead(struct DoubleLinkedList *pdll, struct dllnode *pInsertNode)
{
	if (nullptr == pdll) return FALSE;
	if (nullptr == pInsertNode) return FALSE;

	pInsertNode->pNext = pdll->pHead;
	pInsertNode->pPrev = nullptr;

	// if there already where nodes in the list let
	// previous node point at this one
	if (nullptr != pdll->pHead) {
		pdll->pHead->pPrev = pInsertNode;
	}

	// If this is the first node - GS
	if (nullptr == pdll->pTail) {
		pdll->pTail = pInsertNode;
	}

	// The Head always point at the first node
	pdll->pHead = pInsertNode;

	// Calculate the number of elements in the list
	GetNodeCount(pdll);

	return TRUE;
};

///////////////////////////////////////////////////////////////////////////////
// addNodeTail 

BOOL CDllList::AddNodeTail(struct DoubleLinkedList *pdll, struct dllnode *pInsertNode)
{
	if (nullptr == pdll) return FALSE;
	if (nullptr == pInsertNode) return FALSE;

	pInsertNode->pNext = nullptr;
	pInsertNode->pPrev = pdll->pTail;

	// if there already where nodes in the list let
	// previous node point at this one
	if (nullptr != pdll->pTail) {
		pdll->pTail->pNext = pInsertNode;
	}

	// If this is the first node
	if (nullptr == pdll->pHead) {
		pdll->pHead = pInsertNode;
	}

	// The tail always point at the end
	pdll->pTail = pInsertNode;

	// Calculate the number of elements in the list
	GetNodeCount(pdll);

	return TRUE;
};

///////////////////////////////////////////////////////////////////////////////
// addNodeBefore  -  AKHE

BOOL CDllList::AddNodeBefore(struct DoubleLinkedList *pdll,
	struct dllnode *pNode,
	struct dllnode *pInsertNode)
{
	if (nullptr == pdll) return FALSE;
	if (nullptr == pNode) return FALSE;
	if (nullptr == pInsertNode) return FALSE;

	if (nullptr == pNode->pPrev) {
		// There is no nodes before this node
		AddNodeHead(pdll, pInsertNode);
	}
	else {
		// Add the node between the two
		InsertNode(pdll, pNode, pInsertNode);
	}

	// Calculate the number of elements in the list
	GetNodeCount(pdll);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// addNodeAfter	

BOOL CDllList::AddNodeAfter(struct DoubleLinkedList *pdll,
	struct dllnode *pNode,
	struct dllnode *pInsertNode)
{
	if (nullptr == pdll) return FALSE;
	if (nullptr == pNode) return FALSE;
	if (nullptr == pInsertNode) return FALSE;

	if (nullptr == pNode->pNext) {
		// There is no nodes after this one
		AddNodeTail(pdll, pInsertNode);
	}
	else {
		// Add the node between the two
		InsertNode(pdll, pNode, pInsertNode);
	}

	// Calculate the number of elements in the list
	GetNodeCount(pdll);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// insertNode

BOOL CDllList::InsertNode(struct DoubleLinkedList *pdll,
	struct dllnode *pNode,
	struct dllnode *pInsertNode)
{
	//struct dllnode *pNode = malloc( sizeof( struct dllnode ) );
	if (nullptr == pdll) return FALSE;
	if (nullptr == pNode) return FALSE;
	if (nullptr == pInsertNode) return FALSE;

	// First link in the new node
	pInsertNode->pPrev = pNode;
	pInsertNode->pNext = pNode->pNext;

	// Fix up the node before
	pNode->pNext = pInsertNode;

	// Fix up the node after
	pNode->pNext->pPrev = pInsertNode;

	// Calculate the number of elements in the list
	GetNodeCount(pdll);

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// addNode 
//

BOOL CDllList::AddNode(struct DoubleLinkedList *pdll, struct dllnode *pInsertNode)
{
	int nsearchResult = 0;
	struct dllnode *pNode;


	if (nullptr == pdll) return FALSE;

	if (pdll->nSortOrder == SORT_NONE) {
		// No sorting just add as last element
		AddNodeTail(pdll, pInsertNode);
		return TRUE;
	}

	if ((pdll->nSortOrder == SORT_STRING) &&
		(nullptr == pInsertNode->pstrKey)) {
		return FALSE;
	}

	if ((pdll->nSortOrder == SORT_NUMERIC) &&
		(nullptr == pInsertNode->pKey)) {
		return FALSE;
	}


	// Get Head node
	pNode = pdll->pHead;


	if (nullptr == pNode) {
		// Add to tail - last id
		return AddNodeTail(pdll, pInsertNode);
	}

	// Search for a place to insert the new node

	while (pNode != nullptr) {

		if ((pdll->nSortOrder == SORT_STRING) && (pInsertNode->pstrKey != nullptr))
        {
			nsearchResult = strcmp(pInsertNode->pstrKey, pNode->pstrKey);
		}
		else if (pdll->nSortOrder == SORT_NUMERIC)
        {

			if (*pInsertNode->pKey == *pNode->pKey) {
				nsearchResult = 0;
			}
			else if (*pInsertNode->pKey > *pNode->pKey) {
				nsearchResult = 2;
			}
			else {
				nsearchResult = -1;
			}
		}

		if (0 == nsearchResult) {
			// Add after current item
			return AddNodeAfter(pdll, pNode, pInsertNode);
		}
		else if (nsearchResult > 0) {
			// Add before current item
			return AddNodeBefore(pdll, pNode, pInsertNode);
		}

		pNode = pNode->pNext;
	}

	// If not found it should be added at the end
	return AddNodeTail(pdll, pInsertNode);
}



///////////////////////////////////////////////////////////////////////////////
// removeNode

BOOL CDllList::RemoveNode(struct DoubleLinkedList *pdll, struct dllnode *pNode)
{
	struct dllnode *pNext;
	struct dllnode *pPrev;

	if (pdll == nullptr)
        return FALSE;
	if (pNode == nullptr)
        return FALSE;

		// Remove the object
		if (nullptr != pNode->pObject) {
            //auto pobj = pNode->pObject;
            //delete pobj;
            delete  pNode->pObject
			pNode->pObject = nullptr;
		}

		// Save links
		pNext = pNode->pNext;
		pPrev = pNode->pPrev;

		if (nullptr != pPrev) {
			pPrev->pNext = pNext;
		}
		else {
			pdll->pHead = pNext;
		}

		if (nullptr != pNext) {
			pNext->pPrev = pPrev;
		}
		else {
			pdll->pTail = pPrev;
		}

		// Remove the node
		delete pNode;
		pdll->nCount--;

	// Calculate the number of elements in the list
	GetNodeCount(pdll);

	return TRUE;

};

///////////////////////////////////////////////////////////////////////////////
// getNodeCount

unsigned long CDllList::GetNodeCount(struct DoubleLinkedList *pdll)
{
	unsigned long cnt = 0; // uint32_t
	struct dllnode *pNode;

	if (nullptr == pdll) return FALSE;

	pNode = pdll->pHead;

	// If there is no head there is no data
	if (nullptr == pNode) return 0;

	while (pNode != nullptr) {
		cnt++;
		pNode = pNode->pNext;
	}

	// Store the count
	pdll->nCount = cnt;

	return cnt;
};


///////////////////////////////////////////////////////////////////////////////
// removeAllNodes

BOOL CDllList::RemoveAllNodes(struct DoubleLinkedList *pdll)
{
	if (nullptr == pdll) return FALSE;

	while (nullptr != pdll->pHead) {
		RemoveNode(pdll, pdll->pTail);
	}

	Init(pdll, 0);

	pdll->nCount = 0;

	return TRUE;
};


///////////////////////////////////////////////////////////////////////////////
// findNodeFromID

struct dllnode* CDllList::FindNodeFromID(struct DoubleLinkedList *pdll,
	ULONG nID)
{
	struct dllnode *pRV = nullptr;
	struct dllnode *pNode;

	if (nullptr == pdll) return FALSE;

	pNode = pdll->pHead;

	while (pNode != nullptr) {
		if (nID == *pNode->pKey) {
			pRV = pNode;
			break;
		}
		pNode = pNode->pNext;
	}

	return pRV;
}

///////////////////////////////////////////////////////////////////////////////
// findNodeFromString

struct dllnode* CDllList::FindNodeFromString(struct DoubleLinkedList *pdll,
	char * strID)
{
	struct dllnode *pRV = nullptr;
	struct dllnode *pNode;

	if (nullptr == pdll) return FALSE;

	pNode = pdll->pHead;

	while (pNode != nullptr) {
		if (0 == strcmp(strID, pNode->pstrKey)) {
			pRV = pNode;
			break;
		}

		pNode = pNode->pNext;
	}

	return pRV;
}
