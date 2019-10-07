////////////////////////////////////////////////////////////////////////////////
//! \addtogroup gfx
//! @{
//!
//! \file    gfx_clip.c
//! \brief   Contains clip functions for the Graphics subsystem.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Includes and external references
////////////////////////////////////////////////////////////////////////////////
#include "freertos_tetris.h"
#include "types.h"
#include "linkedlist.h"
////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
gfx_Rect_t gfx_rect_Make(int16_t x, int16_t y, uint16_t width, uint16_t height)
{
    gfx_Rect_t rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    return rect;
}
////////////////////////////////////////////////////////////////////////////////
//! \brief Tests to see if a point is in a rectangle
//!
//! \param[in] pRect        Rectangle to check within.
//! \param[in] x            X position of the coordinate to test.
//! \param[in] y            Y position of the coordinate to test.
//! \return Whether the point was in the rectangle or not.
////////////////////////////////////////////////////////////////////////////////
bool gfx_rect_IsPointIn(gfx_Rect_t *pRect, int x, int y)
{
    if(x < pRect->x)
        return false;
    if (x > pRect->x + pRect->width)
        return false;
    if(y< pRect->y)
        return false;
    if(y>pRect->y + pRect->height)
        return false;
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief Releases a gfx_RectNode_t back into the pool
//!
//! \param[in] pNode Node to release back into the pool.
//!
//! \note This function is called from within the graphics system and is not meant
//!       to be called from the outside.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_Release(gfx_RectNode_t *pNode)
{
    util_linkedlist_AddHead((void **) &g_gfx_Globals.pRectNodes,pNode);
}

////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief Gets a gfx_RectNode_t from the pool
//!
//! This function gets a free gfx_RectNode_t from the pool, or mallocs memory for a new one.
//!
//! \note This function is called from within the graphics system and is not meant
//!       to be called from the outside.
//! \return Pointer to gfx_RectNode_t
////////////////////////////////////////////////////////////////////////////////
gfx_RectNode_t *gfx_clip_Get(void)
{
    gfx_RectNode_t *tmp = NULL;

    tmp = (gfx_RectNode_t*)util_linkedlist_RemoveHead((void **) &g_gfx_Globals.pRectNodes);
    return tmp;
}

////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief Releases a chain of gfx_RectNode_t back into the pool
//!
//! \param[in] pChain Chain to release back into the pool.
//! \note This function is called from within the graphics system and is not meant
//!       to be called from the outside.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_ReleaseClipChain(gfx_RectNode_t *pChain)
{
    while(pChain)
    {
        gfx_RectNode_t *pTemp = pChain;
        pChain = pChain->pNext;
        gfx_clip_Release(pTemp);
    }
}

////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief Releases a gfx_ClipNode_t back into the pool
//!
//! \param[in] pRegion Chain to release back into the pool.
//! \note This function is called from within the graphics system and is not meant
//!       to be called from the outside.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_ReleaseRegion(gfx_ClipNode_t *pRegion)
{
    util_linkedlist_AddHead((void **) &g_gfx_Globals.pClipNodes,pRegion);
}

////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief Get a gfx_ClipNode_t from the pool
//!
//! This function gets a free gfx_ClipNode_t from the pool, or mallocs memory for a new one.
//!
//! \note This function is called from within the graphics system and is not meant
//!       to be called from the outside.
////////////////////////////////////////////////////////////////////////////////
gfx_ClipNode_t *gfx_clip_GetClipNode()
{
    gfx_ClipNode_t *tmp = NULL;

    tmp = (gfx_ClipNode_t*)util_linkedlist_RemoveHead((void **) &g_gfx_Globals.pClipNodes);
    return tmp;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_PopClippingRegion(gfx_DeviceContext_t* pDC)
{
    gfx_ClipNode_t *pRegion;
    gfx_clip_ReleaseClipChain(pDC->pActiveClipRegion);
    if(pDC->pClippingStack)
    {
        pDC->pActiveClipRegion = pDC->pClippingStack->pNode;
        pRegion = pDC->pClippingStack;
        pDC->pClippingStack = pDC->pClippingStack->pNext;
        gfx_clip_ReleaseRegion(pRegion);
    }
    else
    {
        pDC->pActiveClipRegion = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_PushClippingRegion(gfx_DeviceContext_t* pDC)
{
    gfx_RectNode_t *pSourceNode;
    gfx_RectNode_t **pDestPointer;
    gfx_ClipNode_t *newRegion = gfx_clip_GetClipNode();
    configASSERT(newRegion);
    if(newRegion)
    {
        newRegion->pNode = NULL;
        pDestPointer = &newRegion->pNode;
        pSourceNode = pDC->pActiveClipRegion;
        while(pSourceNode)
        {
            *pDestPointer = gfx_clip_Get();
            configASSERT(*pDestPointer);
            if(*pDestPointer)
            {
                **pDestPointer= *pSourceNode;

                pDestPointer = &(*pDestPointer)->pNext;
                *pDestPointer = NULL;
            }
            pSourceNode = pSourceNode->pNext;
        }
        newRegion ->pNext = pDC->pClippingStack;
        pDC->pClippingStack = newRegion;
    }
}


////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_Intersect(gfx_DeviceContext_t *pDC, gfx_Rect_t *pIntersectRect)
{
    //And each clip rectangle with the one passed in.  No new elements are created.
    gfx_RectNode_t **pParentPointer = &(pDC->pActiveClipRegion);
    pIntersectRect->x += pDC->i16xOffset;
    pIntersectRect->y += pDC->i16yOffset;
    while((*pParentPointer))
    {
        gfx_RectNode_t *pTemp = (*pParentPointer);
        if(!gfx_rect_And(&pTemp->Rect, pIntersectRect))
        {
            *pParentPointer = pTemp->pNext;
            gfx_clip_Release(pTemp);
        }
        else
            pParentPointer = &(*pParentPointer)->pNext;
    }
    pIntersectRect->x -= pDC->i16xOffset;
    pIntersectRect->y -= pDC->i16yOffset;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_IntersectByPoints(gfx_DeviceContext_t *pDC, int left, int top, int right, int bottom)
{
    gfx_Rect_t rect;
    rect.x = left;
    rect.width = right - left + 1;
    rect.y = top;
    rect.height = bottom- top + 1;

    gfx_clip_Intersect(pDC, &rect);
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_Exclude(gfx_DeviceContext_t *pDC, gfx_Rect_t *pExcludeRect)
{
    //Subtract the passed in rect from each of the clipping rectangles.   This subtraction may create as many as
    //3 more clipping rectangles than existed before, depending on the geometry.
    gfx_RectNode_t **pParentPointer = &(pDC->pActiveClipRegion);
    gfx_RectNode_t *pClipList = pDC->pActiveClipRegion;
    *pParentPointer = NULL;  //clear out the list.  We'll process each one in the pClipList, and put it back into the chain connected to the DC
    pExcludeRect->x += pDC->i16xOffset;
    pExcludeRect->y += pDC->i16yOffset;
    while(pClipList)
    {
        gfx_RectNode_t *pResults;
        gfx_RectNode_t *pTemp = pClipList;
        pClipList = pClipList->pNext;
        pTemp->pNext = NULL;
        pResults = gfx_rect_Subtract(pTemp, pExcludeRect);
        if(pResults)
        {
            *pParentPointer = pResults;
            while((*pParentPointer)->pNext)
                pParentPointer = &(*pParentPointer)->pNext;
            pParentPointer = &(*pParentPointer)->pNext;
        }
    }
    pExcludeRect->x -= pDC->i16xOffset;
    pExcludeRect->y -= pDC->i16yOffset;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_ExcludeByPoints(gfx_DeviceContext_t *pDC, int left, int top, int right, int bottom)
{
    gfx_Rect_t rect;
    rect.x = left;
    rect.width = right - left + 1;
    rect.y = top;
    rect.height = bottom- top + 1;
    gfx_clip_Exclude(pDC, &rect);
}

////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief This function subtracts a gfx_Rect_t from a gfx_RectNode_t.
//!
//! This function subtracts a rectangle from another.   A pointer to a gfx_Rect_t is passed in, and up to
//! four gfx_RectNode_t's in a chain will be returned.   The gfx_RectNode_t that is passed in is to be considered
//! released back to the pool and should not be relied upon to exist after calling this function.
//!
//! \param[in] pRectNode      Single gfx_RectNode_t on which to operate.
//! \param[in] pSubtractRect  Rectangle which to subtract
//!
//! \return Pointer to a \ref gfx_RectNode_t
//!
//! \note This function is an internal helper function and is not for calling from outside the graphics library.
////////////////////////////////////////////////////////////////////////////////
gfx_RectNode_t *gfx_rect_Subtract(gfx_RectNode_t *pRectNode, gfx_Rect_t *pSubtractRect)
{
    gfx_RectNode_t *pNodeList = NULL;
    gfx_RectNode_t **pNext = &pNodeList;
    gfx_Rect_t *pSourceRect = &(pRectNode->Rect);
    if(
        (pSourceRect->x                      >= pSubtractRect->x                         )&&
        (pSourceRect->x + pSourceRect->width <= pSubtractRect->x + pSubtractRect->width  )&&
        (pSourceRect->y                      >= pSubtractRect->y                         )&&
        (pSourceRect->y + pSourceRect->height<= pSubtractRect->y + pSubtractRect->height )  )
    {//the source rect is entirely within the subtraction rect
        gfx_clip_Release(pRectNode);
        return NULL;
    }
    else
    {//there is no overlap between the two rectangles
        if((pSourceRect->x                       > pSubtractRect->x + pSubtractRect->width-1 ) ||
           (pSourceRect->x + pSourceRect->width  <= pSubtractRect->x                        ) ||
           (pSourceRect->y                       > pSubtractRect->y + pSubtractRect->height-1) ||
           (pSourceRect->y + pSourceRect->height <= pSubtractRect->y                        )   )
        {
            pRectNode->pNext = NULL;
            return pRectNode;
        }
        else
        {//its a complex subtraction, and multiple rectangles are going to be created.  The algorithm is to
            //split the source rectangle into two parts each time:  composed of the area to the top, then bottom, then
            //left, then right that is not touched by the subtract rect.   At the end, the source rect and the subtract
            //rect should be equal, and we should have up to 4 new rectangles in the chain passed back.
            //
            //
            //                  /----------------------------------/
            //                  /                                  /
            //                  /                                  /
            //                  /                  1               /
            //                  /                                  /
            //                  /-------------##########-----------/
            //                  /             ##########           /
            //                  /             ##########           /
            //                  /     3       ##########    4      /
            //                  /             ##########           /
            //                  /             ##########           /
            //                  /-------------##########-----------/
            //                  /                                  /
            //                  /                                  /
            //                  /                 2                /
            //                  /                                  /
            //                  /----------------------------------/
            //
            //
            //  # is the subtract rectagle
            //  1 is the first rectangle cast off
            //  2 is the second
            //  3 is the third
            //  4 is the remainder
            //

            //first the top
            if(pSourceRect->y < pSubtractRect->y)
            {//There is a rectangle above of pSubtractRect->y
                gfx_RectNode_t *pNode = gfx_clip_Get();
                configASSERT(pNode);
                if(pNode)
                {
                    pNode->Rect        = *pSourceRect;

                    pNode->Rect.height = pSubtractRect->y - pSourceRect->y;

                    pSourceRect->y       = pSubtractRect->y;
                    pSourceRect->height -= pNode->Rect.height;

                    if(pNode->Rect.height < 0)
                        pNode->Rect.height = 1;
                    if(pSourceRect->height < 0)
                        pSourceRect->height = 1;

                    *pNext = pNode;
                    pNext = &pNode->pNext;
                }
            }

            //then the bottom
            if(pSourceRect->y + pSourceRect->height > pSubtractRect->y + pSubtractRect->height)
            {//There is a rectangle above of pSubtractRect->y
                gfx_RectNode_t *pNode = gfx_clip_Get();
                configASSERT(pNode);
                if(pNode)
                {
                    pNode->Rect = *pSourceRect;

                    pNode->Rect.y       = pSubtractRect->y + pSubtractRect->height;
                    pNode->Rect.height  = (pSourceRect->y + pSourceRect->height) - pNode->Rect.y;

                    pSourceRect->height-= pNode->Rect.height;

                    if(pNode->Rect.height < 0)
                        pNode->Rect.height = 1;
                    if(pSourceRect->height < 0)
                       pSourceRect->height = 1;

                    *pNext = pNode;
                    pNext = &pNode->pNext;
                }
            }

            //then the left
            if(pSourceRect->x < pSubtractRect->x)
            {//There is a rectangle left of pSubtractRect->x
                gfx_RectNode_t *pNode = gfx_clip_Get();
                configASSERT(pNode);
                if(pNode)
                {
                    pNode->Rect         = *pSourceRect;

                    pNode->Rect.width   = pSubtractRect->x - pSourceRect->x;

                    pSourceRect->x      = pSubtractRect->x;
                    pSourceRect->width -= pNode->Rect.width;

                    if(pNode->Rect.width < 0)
                       pNode->Rect.width = 1;
                    if(pSourceRect->width < 0)
                       pSourceRect->width = 1;
                    *pNext = pNode;
                    pNext = &pNode->pNext;
                }
            }

            //then the right side
            if(pSourceRect->x + pSourceRect->width > pSubtractRect->x + pSubtractRect->width)
            {//there's a rectangle right of the subtract rect.
                gfx_RectNode_t *pNode = gfx_clip_Get();
                configASSERT(pNode);
                if(pNode)
                {

                    pNode->Rect         = *pSourceRect;

                    pNode->Rect.x       = pSubtractRect->x + pSubtractRect->width;
                    pNode->Rect.width   = (pSourceRect->x + pSourceRect->width) - pNode->Rect.x;

                    if(pNode->Rect.width < 0)
                       pNode->Rect.width = 1;

                    *pNext = pNode;
                    pNext = &pNode->pNext;
                }
            }
            gfx_clip_Release(pRectNode);
        }
    }
    return pNodeList;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
bool gfx_rect_And(gfx_Rect_t *pRect, gfx_Rect_t *pRectB)
{
    int iRight;
    if(pRect->x + pRect->width <= pRectB->x )
        return false;
    if(pRect->x > pRectB->x + pRectB->width)
        return false;

    iRight = pRect->x + pRect->width;

    if(pRect->x< pRectB->x)
        pRect->x = pRectB->x;
    if(iRight > pRectB->width + pRectB->x)
        pRect->width = pRectB->x + pRectB->width - pRect->x;
    else
        pRect->width = iRight - pRect->x;

    if(pRect->width > 0)
    {
        int iBottom;
        if(pRect->y + pRect->height <= pRectB->y)
            return false;
        if(pRect->y > pRectB->y + pRectB->height)
            return false;
        iBottom = pRect->y + pRect->height;
        if(pRect->y< pRectB->y)
            pRect->y= pRectB->y;
        if(iBottom > pRectB->y + pRectB->height)
            pRect->height= pRectB->y + pRectB->height - pRect->y;
        else
            pRect->height = iBottom - pRect->y;


        if(pRect->height> 0)
        {
            return true;
        }

    }
    pRect->width = 0;
    pRect->height = 0;
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_Union(gfx_DeviceContext_t *pDC, gfx_Rect_t *pRect)
{
    //gfx_RectNode_t *pWorkingNodeList = pDC->pActiveClipRegion;
    //gfx_RectNode_t *tempClipRegion = NULL;
    //gfx_RectNode_t *ClipRegionAfterSubtract = NULL;
    //pRect->x += pDC->i16xOffset;
    //pRect->y += pDC->i16yOffset;

    //ClipRegionAfterSubtract = tempClipRegion = gfx_clip_Get();    //get a new rect Node from the heap
    //if(tempClipRegion){
    //    gfx_Rect_t tempRect;
    //    while(pWorkingNodeList){
    //        tempRect = pWorkingNodeList->Rect;
    //        tempRect.x += pDC->i16xOffset;
    //        tempRect.y += pDC->i16yOffset;
    //        ClipRegionAfterSubtract = gfx_rect_Subtract(&tempClipRegion,&tempRect);
    //        pWorkingNodeList = pWorkingNodeList->pNext;
    //    }
    //    //now add the remaining rectangles to the real clipping region
    //    while(tempClipRegion){
    //        
    //    }
    //}


    //There are three cases:
    //1)    The Rect to include in the region is already completely within a single rectangle in the region, thus do not add
    //2)    The rect to include in the region is completely outside all rectangles in the region, add in its entirity.
    //3)    The rect to include in the region is partially contained by rectangles in the region, therefor add the part which is not contained.
    //The general algorithm is take the rectangle, create a new, temporary clipping region out of it.
    //For each rect in the real clipping region (while anything exists in the temporary region), subtract it from the temporary clipping region.
    //Add the contents of the remaining temporary clipping region to the actual clipping region.
    //SystemHalt();
}


////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief Helper function for sorting rectangles
//!
//! \fntype callback
//!
//! This sorting is done to put larger rectangles first in the list.   Hopefully these
//! larger rectangles will occlude the smaller ones, and reduce overdraw.  This method is used
//! rather than 'flattening' the clipping region (i.e. making sure there are no overlapping rectangles)
//! prior to drawing as an attempt to achieve the same effect.
//!
//! \param[in] pLeft  One of the rectangles
//! \param[in] pRight Other rectangle
//!
//! \retval -1 if the left is larger than the right
//! \retval  1 if it is smaller.
//!
//! \note This function is an internal helper function and is not for calling from outside the graphics library.
////////////////////////////////////////////////////////////////////////////////
int gfx_clip_SortedRectnodeBySize(gfx_RectNode_t *pLeft, gfx_RectNode_t *pRight)
{
    if(pLeft->Rect.width*pLeft->Rect.height > pRight->Rect.width*pRight->Rect.height)
        return -1;
    else
        return 1;
}

////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief Helper function for culling repetitive rectangles
//! \fntype callback
//!
//! This helper function is called by gfx_clip_AddRect (through util_linkedlist_Search()).
//! Its purpose is to return 'true' to any passed in rectangle that is wholely covered by
//! the rectangle already in the list.
//!
//! \param[in] pNode          gfx_RectNode_t* element to compare against.
//! \param[in] u32RectPointer Rectangle to determine if it is redundant
//! \retval true if the entire area of the rectangle is already accounded for.
//! \retval false if there is a portion of the rectangle that is not covered by this node.
//!
//! \note This function is an internal helper function and is not for calling from
//!       outside the graphics library.
////////////////////////////////////////////////////////////////////////////////
bool gfx_Clip_IsCoveredBy(void*pNode, uint32_t u32RectPointer)
{
    gfx_RectNode_t *pRectNode = (gfx_RectNode_t *)pNode;
    gfx_Rect_t *pRect = (gfx_Rect_t *)u32RectPointer;
    bool rtn = false;
    if(pRectNode->Rect.x <= pRect->x)
    {
        if(pRectNode->Rect.x + pRectNode->Rect.width >=pRect->x+pRect->width)
        {
            if(pRectNode->Rect.y <= pRect->y)
            {
                if(pRectNode->Rect.y + pRectNode->Rect.height >=pRect->y+pRect->height)
                {
                    rtn = true;
                }
            }
        }
    }
    return rtn;
}

////////////////////////////////////////////////////////////////////////////////
//! \internal
//! \brief Adds a rectangle to a clipping region
//!
//! \param[in] pDC   Device context
//! \param[in] rect  Pointer to the rectangle
//! \note This function is an internal helper function and is not for calling from outside the graphics library.
//!       When a rectangle is added to the ActiveClipRegion of a DC care is taken
//!       to have no overlapping rectangles.  This is necessary to prevent an
//!       alphablended draw from being called multiple times for a single bitmap.
//!       If we were to draw twice then the result would be that the top image
//!       is twice as opaque as it should be.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_AddRect(gfx_DeviceContext_t *pDC, gfx_Rect_t *rect)
{
    gfx_RectNode_t *pSearchRegion;
    gfx_RectNode_t *pRectNode = NULL;
    gfx_RectNode_t *pNewActiveClipRegion = NULL;
    bool bAlreadyInList;
    configASSERT(pDC);
    configASSERT(rect);

    if(!pDC->pActiveClipRegion){
        pRectNode = gfx_clip_Get();
        configASSERT(pRectNode);
        pRectNode->Rect = *rect;
        util_linkedlist_AddSorted((void **) &pDC->pActiveClipRegion,pRectNode, (int (*)(void *, void *)) gfx_clip_SortedRectnodeBySize);
        return;
    }
    
    //Must make a copy of the pointer, because util_linkedlist_Search updates the pointer passed in
    pSearchRegion = pDC->pActiveClipRegion;

    //See if the rectangle is already in the list.
    bAlreadyInList = util_linkedlist_Search((void **)&pSearchRegion,gfx_Clip_IsCoveredBy, (uint32_t) rect) ? true : false;

    if(!bAlreadyInList)
    {
        //The rectangle does not fit entirely within a current clipping rectangle.
        //if its not already in the list, then add it
        //pRectNode = gfx_clip_Get();

        gfx_RectNode_t *pSubtractNodes;
        //pRectNode->Rect = *rect;
        pSearchRegion = pDC->pActiveClipRegion;
        while(pSearchRegion){
            pRectNode = gfx_clip_Get(); //get a clip rect for doing the subtraction
            configASSERT(pRectNode);
            pRectNode->Rect = pSearchRegion->Rect;
            //TODO: optimize by checking for cases where we can simply change the dimensions of the input rectangle.
            //there are 4 cases where this is possible.
            pSubtractNodes = gfx_rect_Subtract(pRectNode, rect);
            if(pSubtractNodes){
                if(pNewActiveClipRegion){
                    while(pSubtractNodes){
                        gfx_RectNode_t *pNode = (gfx_RectNode_t*)util_linkedlist_RemoveHead((void**)&pSubtractNodes);
                        pNode->pNext = NULL;
                        util_linkedlist_AddSorted((void **) &pNewActiveClipRegion,pNode, (int (*)(void *, void *)) gfx_clip_SortedRectnodeBySize);
                    }
                }
                else{
                    pNewActiveClipRegion = pSubtractNodes;
                }
            }
            pSearchRegion = pSearchRegion->pNext;
        }
        //We can now replace the old ActiveClipRegion with the newActiveClipRegion
        gfx_clip_ReleaseClipChain(pDC->pActiveClipRegion);
        pDC->pActiveClipRegion = pNewActiveClipRegion;
        pRectNode = gfx_clip_Get();
        configASSERT(pRectNode);
        pRectNode->Rect = *rect;
        util_linkedlist_AddSorted((void **) &pDC->pActiveClipRegion,pRectNode, (int (*)(void *, void *)) gfx_clip_SortedRectnodeBySize);
        //pRectNode->Rect = *rect;
        //util_linkedlist_AddSorted((void **) &pDC->pActiveClipRegion,pRectNode, (int (*)(void *, void *)) gfx_clip_SortedRectnodeBySize);
    
    }
}


////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
bool gfx_clip_IsRectInClippingRegion(gfx_DeviceContext_t *pDC, gfx_Rect_t *pRect)
{
    gfx_RectNode_t *pRectNode = pDC->pActiveClipRegion;
    int left  = pRect->x + pDC->i16xOffset;
    int rightPlus1 = left + pRect->width ;
    int top   = pRect->y+ pDC->i16yOffset;
    int bottomPlus1= top + pRect->height ;
    while(pRectNode)
    {
        if(rightPlus1 < pRectNode->Rect.x)      //make sure we're not entirely to the left
        {
        }
        else if(bottomPlus1< pRectNode->Rect.y)      //make sure we're not entirely above
        {
        }
        else if(left >= pRectNode->Rect.x + pRectNode->Rect.width) //make sure we're not entirely to the right
        {
        }
        else if(top >= pRectNode->Rect.y + pRectNode->Rect.height)
        {
        }
        else
        {
            return true;
        }

        pRectNode = pRectNode->pNext;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}

