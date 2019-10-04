////////////////////////////////////////////////////////////////////////////////
//! \addtogroup util_linkedlist
//! @{
//
//! \file    linkedlist.c
//! \brief   Contains funtionality required to implement the Linked List subsystem.
//!
//! The functionality contained in this file implements a singly linked list.
//! The library can work on any linked list type as long as the first word
//! of the node or element is the "next" pointer.
////////////////////////////////////////////////////////////////////////////////
#include "types.h"
#include "freertos_tetris.h"
#include "linkedlist.h"

//! prototype of the linked list node
typedef struct _util_linkedlist_node
{
    struct _util_linkedlist_node *pNext;//!< next pointer
} util_linkedlist_node_t;

////////////////////////////////////////////////////////////////////////////////
//! \brief Removes the head element from a linked list.
//!
//! \param [in,out]  pList pointer to the head pointer of the list to remove the head element
//!                 from
//! \return pointer to the head element.
////////////////////////////////////////////////////////////////////////////////
void *util_linkedlist_RemoveHead(void **pList)
{
    util_linkedlist_node_t *pNode = (util_linkedlist_node_t*)*pList;

    if(pNode)
    {
        *pList = pNode->pNext;
        pNode->pNext = NULL;
    }
    return (void*)pNode;
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Inserts an element at the head of the list.
//!
//! \param  [in,out] pList pointer to the head pointer of the list to insert the element into
//! \param [in] pElem the element to insert at the head of the list.
////////////////////////////////////////////////////////////////////////////////
void util_linkedlist_AddHead(void **pList, void *pElem)
{
    util_linkedlist_node_t *pNode = (util_linkedlist_node_t*)pElem;
    pNode->pNext = (util_linkedlist_node_t*)*pList;
    *pList = pNode;
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Inserts an element at some point in the list.
//!
//! \note this function iterates the ->pNext pointer until
//!         uCount expires or the end of the list is reached.
//! \param [in,out] pList pointer to the head pointer of the list to insert the element into
//! \param [in] pElem the element to add to the list
//! \param [in] uCount At what point in the list it shoudl be inserted
////////////////////////////////////////////////////////////////////////////////
void util_linkedlist_AddAt(void **pList, void *pElem, uint32_t uCount)
{
    util_linkedlist_node_t **pNodeList = (util_linkedlist_node_t **)pList;
    while(*pNodeList && uCount--)
        pNodeList = &((*pNodeList)->pNext);
    *pNodeList = (util_linkedlist_node_t*)pElem;
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Counts the elements in the linked list
//!
//! \param [in,out] pList the list of which to count its elements
//! \return number of elements in the list.
////////////////////////////////////////////////////////////////////////////////
uint32_t util_linkedlist_GetCount(void **pList)
{
    uint32_t uCount = 0;
    util_linkedlist_node_t **pNodeList = (util_linkedlist_node_t **)pList;
    while(*pNodeList)
    {
        uCount++;
        pNodeList = &((*pNodeList)->pNext);
    }
    return uCount;
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Removes an item from a linked list
//!
//! \param [in,out] pList pointer to the head pointer of the list from which to remove an item
//! \param [in] pElem pointer to the element to remove from the list.
//! \return Element that is removed, or NULL if the element is not in the list.
////////////////////////////////////////////////////////////////////////////////
void *util_linkedlist_RemoveItem(void **pList, void*pElem)
{
    util_linkedlist_node_t **pNodeList = (util_linkedlist_node_t **)pList;
    while(*pNodeList)
    {
        if(*pNodeList == pElem)
        {
            *pNodeList = ((util_linkedlist_node_t*)pElem)->pNext;
            ((util_linkedlist_node_t*)pElem)->pNext = NULL;
            return pElem;
        }
        pNodeList = &((*pNodeList)->pNext);
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Adds an item to the list in sorted order
//!
//! This function is used to add an item to a list in a sorted fashion.  The function
//! pointer should return <0 if the element should be inserted after the item pointed to by
//! the first parameter.   Any other element will cause the iterator to go to the next element.
//! \param [in,out] pList pointer to the head pointer of the list to add the new element to
//! \param [in] pElem pointer to the element to add to the list
//! \param [in] pSortFunc The sort function to use
////////////////////////////////////////////////////////////////////////////////
void util_linkedlist_AddSorted  (void **pList, void *pElem, int (*pSortFunc)(void*,void*))
{
    util_linkedlist_node_t *pTemp;
    util_linkedlist_node_t **pNodeList = (util_linkedlist_node_t **)pList;
    while(*pNodeList)
    {
        int value = (pSortFunc)(*pNodeList, pElem);
        if(value <0)
            break;
        pNodeList = &((*pNodeList)->pNext);
    }
    pTemp = *pNodeList;
    *pNodeList = (util_linkedlist_node_t*)pElem;
    ((util_linkedlist_node_t*)pElem)->pNext=pTemp;
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Searches a linked list for an element
//!
//! This function is used to search a linked list for a particular element.  This function will iterate the
//! list until the test function returns true, at which point it will return the element that tested true, along
//! with updating the pList pointer to point the next element in the list (to allow for further searching)
//! \param[in,out] pList pointer to the head pointer of the list to search
//! \param[in] test the search function to test for
//! \param[in] data a data value to pass into the search function along with the current element pointer
////////////////////////////////////////////////////////////////////////////////
void    *util_linkedlist_Search    (void **pList, bool (*test)(void*, uint32_t), uint32_t data)
{
    util_linkedlist_node_t **pNodeList = (util_linkedlist_node_t **)pList;
    while(*pNodeList)
    {
        if((test)(*pNodeList,data))
        {
            util_linkedlist_node_t *pTemp=*pNodeList;
            *pList = (*pNodeList)->pNext;
            return pTemp;
        }
        pNodeList = &((*pNodeList)->pNext);
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//! \brief Performs a function on each element in the list.
//!
//! This function is used to iterate every element in a linked list and perform a set function on it.
//! The function must take the pointer to the element, plus one 32 bit value as parameters.
//! \param[in,out] pList pointer to the head pointer of the list to iterate
//! \param[in] Function the function which to call for each element
//! \param[in] data a data value to pass into the function along with the current element pointer
////////////////////////////////////////////////////////////////////////////////
void util_linkedlist_PerformOnEach(void *pList, void(*Function)(void*,uint32_t), uint32_t data)
{
    while(pList)
    {
        (Function)(pList,data);
        pList = ((util_linkedlist_node_t*)pList)->pNext;
    }
}
////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}
