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

#pragma once

/*!
Methods to handle a node for the double linked list
*/

/*!
Sorttypes for the list
*/
//enum _sorttype { SORT_NONE = 0, SORT_STRING, SORT_NUMERIC };

#define SORT_NONE				0
#define SORT_STRING				1
#define SORT_NUMERIC			2

/*!
A base class to derive from for object storage
*/

struct dllnode
{
	/*!
	String key used for alphanumeric sort
	*/
	char *pstrKey;

	/*!
	Numeric key used for numeric sort
	*/
	unsigned long *pKey;

	/*!
	Sort key (can be used if no other key is abailabe ). pKey should
	be pointing to this key if used.
	*/
	unsigned long Key;

	/*!
	A pointer to the embedded node object
	*/
	void *pObject;

	/*!
	A long that can be used in any way a user likes.
	*/
	unsigned long obid;

	/*!
	Pointer to the next node
	*/
	struct dllnode *pNext;


	/*!
	Pointer to the previous node
	*/
	struct dllnode *pPrev;

};


struct DoubleLinkedList
{
	/*!
	Sort order
	==========

	0 - not sorted.
	1 - sort on string key.
	2 - sort on numeric key.

	Note that items can be retrived in accending/decending order by
	retriving them from the head or from the tail of the list.
	*/
	UCHAR nSortOrder;

	/*!
	Pointer to the head of the linked list
	*/
	struct dllnode *pHead;

	/*!
	Pointer to the tail of the linked list
	*/
	struct dllnode *pTail;

	/*!
	Number of elements in list
	*/
	unsigned long nCount;

};


class CDllList
{

public:

	/*!
	Initialize the double linked list
	*/
	void Init(struct DoubleLinkedList *, unsigned char);

	/*!
	Add a node to the front of the linked list.
	*/
	BOOL AddNodeHead(struct DoubleLinkedList *, struct dllnode *);


	/*!
	Add a node to the end of the linked list.
	*/
	BOOL AddNodeTail(struct DoubleLinkedList *, struct dllnode *);


	/*!
	Add a node before another node
	*/
	BOOL AddNodeBefore(struct DoubleLinkedList *pdll,
		struct dllnode *pNode,
		struct dllnode *pInsertNode);

	/*!
	Add a node after another node.
	*/
	BOOL AddNodeAfter(struct DoubleLinkedList *pdll,
		struct dllnode *pNode,
		struct dllnode *pInsertNode);

	/*!
	Add a node sorted by the current sort order
	*/
	BOOL AddNode(struct DoubleLinkedList *, struct dllnode *);

	/*!
	Remove all nodes form the linked list.
	*/
	BOOL RemoveAllNodes(struct DoubleLinkedList *);

	/*!
	Get node from its numerical key

	@param Numerical key for node.
	@return Found Object
	@return NULL if no match found.
	*/
	struct dllnode *FindNodeFromID(struct DoubleLinkedList *, unsigned long);

	/*!
	Get node from its string key

	@param String key for node.
	@return Found Object
	@return NULL if no match found.
	*/
	struct dllnode *FindNodeFromString(struct DoubleLinkedList *, char *);

	/*!
	Insert a node and an object between two other nodes
	*/
	BOOL InsertNode(struct DoubleLinkedList *pdll,
		struct dllnode *pNode1,
		struct dllnode *pNode2);

	/*!
	Remove a node from the double linked list
	*/
	BOOL RemoveNode(struct DoubleLinkedList *pdll,
		struct dllnode *pNode);

	/*!
	Get the node count for the list
	*/
	ULONG GetNodeCount(struct DoubleLinkedList *pdll);

	CDllList();
	~CDllList();
};

