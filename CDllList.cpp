/*
 * CANAL interface DLL for RUSOKU technologies for TouCAN, TouCAN Marine, TouCAN Duo USB to CAN bus converter
 *
 * Copyright (C) 2000-2008 Ake Hedman, eurosource, <akhe@eurosource.se>
 * Copyright (C) 2018 Gediminas Simanskis (gediminas@rusoku.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 3.0 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.
 *
 */

#include "stdafx.h"
#include "CDllList.h"


CDllList::CDllList()
{
	
}


CDllList::~CDllList()
{
	
}



///////////////////////////////////////////////////////////////////////////////
// init_list

void CDllList::Init(struct DoubleLinkedList *pdll, unsigned char nSort)
{
	pdll->pHead = NULL;
	pdll->pTail = NULL;
	pdll->nSortOrder = nSort;
	pdll->nCount = 0;
};

///////////////////////////////////////////////////////////////////////////////
// addNodeFront  -  AKHE

BOOL CDllList::AddNodeHead(struct DoubleLinkedList *pdll, struct dllnode *pInsertNode)
{
	if (NULL == pdll) return FALSE;
	if (NULL == pInsertNode) return FALSE;

	pInsertNode->pNext = pdll->pHead;
	pInsertNode->pPrev = NULL;

	// if there already where nodes in the list let
	// previous node point at this one
	if (NULL != pdll->pHead) {
		pdll->pHead->pPrev = pInsertNode;
	}

	// If this is the first node - GS
	if (NULL == pdll->pTail) {
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
	if (NULL == pdll) return FALSE;
	if (NULL == pInsertNode) return FALSE;

	pInsertNode->pNext = NULL;
	pInsertNode->pPrev = pdll->pTail;

	// if there already where nodes in the list let
	// previous node point at this one
	if (NULL != pdll->pTail) {
		pdll->pTail->pNext = pInsertNode;
	}

	// If this is the first node
	if (NULL == pdll->pHead) {
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
	if (NULL == pdll) return FALSE;
	if (NULL == pNode) return FALSE;
	if (NULL == pInsertNode) return FALSE;

	if (NULL == pNode->pPrev) {
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
	if (NULL == pdll) return FALSE;
	if (NULL == pNode) return FALSE;
	if (NULL == pInsertNode) return FALSE;

	if (NULL == pNode->pNext) {
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
	if (NULL == pdll) return FALSE;
	if (NULL == pNode) return FALSE;
	if (NULL == pInsertNode) return FALSE;

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


	if (NULL == pdll) return FALSE;

	if (pdll->nSortOrder == SORT_NONE) {
		// No sorting just add as last element
		AddNodeTail(pdll, pInsertNode);
		return TRUE;
	}

	if ((pdll->nSortOrder == SORT_STRING) &&
		(NULL == pInsertNode->pstrKey)) {
		return FALSE;
	}

	if ((pdll->nSortOrder == SORT_NUMERIC) &&
		(NULL == pInsertNode->pKey)) {
		return FALSE;
	}


	// Get Head node
	pNode = pdll->pHead;


	if (NULL == pNode) {
		// Add to tail - last id
		return AddNodeTail(pdll, pInsertNode);
	}

	// Search for a place to insert the new node

	while (pNode != NULL) {

		if ((pdll->nSortOrder == SORT_STRING) &&
			(NULL != pInsertNode->pstrKey)) {
			nsearchResult = strcmp(pInsertNode->pstrKey, pNode->pstrKey);
		}
		else if ((pdll->nSortOrder == SORT_NUMERIC)) {

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

	if (NULL == pdll) return FALSE;
	if (NULL == pNode) return FALSE;

	if (NULL != pNode) {

		// Remove the object
		if (NULL != pNode->pObject) {
			delete pNode->pObject;
			pNode->pObject = NULL;
		}

		// Save links
		pNext = pNode->pNext;
		pPrev = pNode->pPrev;

		if (NULL != pPrev) {
			pPrev->pNext = pNext;
		}
		else {
			pdll->pHead = pNext;
		}

		if (NULL != pNext) {
			pNext->pPrev = pPrev;
		}
		else {
			pdll->pTail = pPrev;
		}

		// Remove the node
		delete pNode;
		pdll->nCount--;
	}

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

	if (NULL == pdll) return FALSE;

	pNode = pdll->pHead;

	// If there is no head there is no data
	if (NULL == pNode) return 0;

	while (pNode != NULL) {
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
	if (NULL == pdll) return FALSE;

	while (NULL != pdll->pHead) {
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
	struct dllnode *pRV = NULL;
	struct dllnode *pNode;

	if (NULL == pdll) return FALSE;

	pNode = pdll->pHead;

	while (pNode != NULL) {
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
	struct dllnode *pRV = NULL;
	struct dllnode *pNode;

	if (NULL == pdll) return FALSE;

	pNode = pdll->pHead;

	while (pNode != NULL) {
		if (0 == strcmp(strID, pNode->pstrKey)) {
			pRV = pNode;
			break;
		}

		pNode = pNode->pNext;
	}

	return pRV;
}

