////////////////////////////////////////////////////////////////////////////////
//! \addtogroup gfx
//! @{
//
//! \file    gfx_initialize.c
//! \brief   Contains the initialization routine for the graphics library.
//! \version 2.0
//! \date    21-July 2005
////////////////////////////////////////////////////////////////////////////////
#include "types.h"
#include "freertos_tetris.h"
#include "error.h"
#include "linkedlist.h"
//#include "handlealloc.h"
//#include <string.h>

gfx_Globals_t g_gfx_Globals;    //!< Global Structure for Graphics Library

////////////////////////////////////////////////////////////////////////////////
// To view the documentation for this function, refer to gfx.h.
////////////////////////////////////////////////////////////////////////////////
uint32_t gfx_Initialize(uint16_t u16RectNodes, uint16_t u16ClipNodes)
{
    int i;

    gfx_ClipNode_t *pClipNodes;
    gfx_RectNode_t *pRectNodes;

    pRectNodes = (gfx_RectNode_t*)malloc(sizeof(gfx_RectNode_t)*u16RectNodes);
    if(pRectNodes)
    {
        for (i=0;i<u16RectNodes;i++)
            util_linkedlist_AddHead( (void **) &g_gfx_Globals.pRectNodes,pRectNodes+i);
    }
    else
    {
        return ERROR_GENERIC;
    }

    pClipNodes = (gfx_ClipNode_t*)malloc(sizeof(gfx_ClipNode_t)*u16ClipNodes);
    if(pClipNodes)
    {
        for (i=0;i<u16ClipNodes;i++)
            util_linkedlist_AddHead((void **)&g_gfx_Globals.pClipNodes,pClipNodes+i);
    }
    else
    {
        return ERROR_GENERIC;
    }


    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}

