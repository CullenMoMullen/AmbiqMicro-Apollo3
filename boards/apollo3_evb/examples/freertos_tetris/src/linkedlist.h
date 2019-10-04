////////////////////////////////////////////////////////////////////////////////
//! \addtogroup cmp_linkedlist
//! @{
//
//! \file    linkedlist.h
//! \brief   Contains public header data for the Linked List helper functions.
////////////////////////////////////////////////////////////////////////////////
#ifndef __LINKEDLIST_H
#define __LINKEDLIST_H


#ifdef __cplusplus
extern "C" {
#endif


////////////////////////////////////////////////////////////////////////////////
//! \brief Removes the head element from a linked list.
//!
//! \param [in,out] pList Pointer to the head pointer of the list from which to remove the head element
//!
//! \internal
//! \see To view the function definition, see linkedlist.c.
////////////////////////////////////////////////////////////////////////////////
void    *util_linkedlist_RemoveHead   (void **pList);
////////////////////////////////////////////////////////////////////////////////
//! \brief Inserts an element at the head of the list.
//!
//! \param  [in,out] pList Pointer to the head pointer of the list to insert the element into
//! \param [in] pElem the element to insert at the head of the list.
//!
//! \internal
//! \see To view the function definition, see linkedlist.c.
////////////////////////////////////////////////////////////////////////////////
void     util_linkedlist_AddHead      (void **pList, void *pElem);
////////////////////////////////////////////////////////////////////////////////
//! \brief Removes an item from a linked list
//!
//! \param [in,out] pList Pointer to the head pointer of the list from which to remove an item
//! \param [in] pElem pointer to the element to remove from the list.
//! \return Element that is removed, or NULL if the element is not in the list.
//!
//! \internal
//! \see To view the function definition, see linkedlist.c.
////////////////////////////////////////////////////////////////////////////////
void    *util_linkedlist_RemoveItem   (void **pList, void *pElem);
////////////////////////////////////////////////////////////////////////////////
//! \brief Adds an item to the list in sorted order
//!
//! This function is used to add an item to a list in a sorted fashion.  The function
//! pointer should return <0 if the element should be inserted after the item pointed to by
//! the first parameter.   Any other element will cause the iterator to go to the next element.
//! \param [in,out] pList Pointer to the head pointer of the list to add the new element to
//! \param [in] pElem Pointer to the element to add to the list
//! \param [in] * Sort function to use
//!
//! \internal
//! \see To view the function definition, see linkedlist.c.
////////////////////////////////////////////////////////////////////////////////
void     util_linkedlist_AddSorted    (void **pList, void *pElem, int (*)(void*,void*));
////////////////////////////////////////////////////////////////////////////////
//! \brief Counts the elements in the linked list
//!
//! \param [in,out] pList List of which to count its elements
//! \return number of elements in the list.
//!
//! \internal
//! \see To view the function definition, see linkedlist.c.
////////////////////////////////////////////////////////////////////////////////
uint32_t util_linkedlist_GetCount     (void **pList);
////////////////////////////////////////////////////////////////////////////////
//! \brief Inserts an element at some point in the list.
//!
//! \note this function iterates the ->pNext pointer until
//!         uCount expires or the end of the list is reached.
//! \param [in,out] pList Pointer to the head pointer of the list to insert the element into
//! \param [in] pElem Element to add to the list
//! \param [in] uCount At what point in the list it should be inserted
//!
//! \internal
//! \see To view the function definition, see linkedlist.c.
////////////////////////////////////////////////////////////////////////////////
void     util_linkedlist_AddAt        (void **pList, void *pElem, uint32_t uCount);
////////////////////////////////////////////////////////////////////////////////
//! \brief Searches a linked list for an element
//!
//! This function is used to search a linked list for a particular element.  This function will iterate the
//! list until the test function returns true, at which point it will return the element that tested true, along
//! with updating the pList Pointer to point the next element in the list (to allow for further searching)
//! \param[in,out] pList Pointer to the head pointer of the list to search
//! \param[in] * Search function to test for
//! \param[in] data Data value to pass into the search function along with the current element pointer
//!
//! \internal
//! \see To view the function definition, see linkedlist.c.
////////////////////////////////////////////////////////////////////////////////
void    *util_linkedlist_Search       (void **pList, bool (*)(void*, uint32_t), uint32_t data);
////////////////////////////////////////////////////////////////////////////////
//! \brief Performs a function on each element in the list.
//!
//! This function is used to iterate every element in a linked list and perform a set function on it.
//! The function must take the pointer to the element, plus one 32 bit value as parameters.
//!
//! \param[in,out] pList Pointer to the head pointer of the list to iterate
//! \param[in] Function Function which to call for each element
//! \param[in] data Data value to pass into the function along with the current element pointer
//!
//! \internal
//! \see To view the function definition, see linkedlist.c.
////////////////////////////////////////////////////////////////////////////////
void     util_linkedlist_PerformOnEach(void *pList, void(*Function)(void*,uint32_t), uint32_t data);

//! \brief Macro used to add a node at the tail of a linked list.
//!
//! This is implemented as a macro because the functionality is a subset of the functionality
//! provided by util_linkedlist_AddAt.  For this reason, this macro resolved to a call to
//! util_linkedlist_AddAt with a postion of -1, which indicates to util_linkedlist_AddAt to 
//! add the node to the tail of the linked list.
#define  util_linkedlist_AddTail(l,e) util_linkedlist_AddAt(l,e,0xffffffff)

#ifdef __cplusplus
}
#endif

#endif //__LINKEDLIST_H
///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
//! @}


