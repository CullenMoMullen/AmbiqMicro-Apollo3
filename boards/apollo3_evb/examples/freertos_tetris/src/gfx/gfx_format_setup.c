////////////////////////////////////////////////////////////////////////////////
//! \addtogroup gfx
//! @{
//! \file    gfx_format_setup.c
//! \brief   Contains setup code to define support for different bitmap formats.
//! \version 1.0
//! \date    18-September 2005
//!
//! These tables define support for specific bitmap formats and accelleration.
//!
////////////////////////////////////////////////////////////////////////////////
#include "types.h"
#include "gfx/gfx.h"
#include "gfx/gfx_format.h"

//! This structure array defines the supported bitmaps formats and their functions for
//! handling operations.
const gfx_BitmapFormat_t g_gfx_BitmapSupportTable[]=
{
    //{
    //    BITMAP_TYPE_16BPP_565,
    //    gfx_GetDataSize_16BPP_565,
    //    gfx_GetPixel_16BPP_565,
    //    gfx_PutPixel_16BPP_565,
    //    gfx_DrawClippedLine_16BPP_565
    //},
    {
        BITMAP_TYPE_1BPP_IDEAL,
        gfx_GetDataSize_1BPP_Ideal,
        gfx_GetPixel_1BPP_Ideal,
        gfx_PutPixel_1BPP_Ideal,
        gfx_2d_DrawClippedLineGeneric
    },
    //{
    //    BITMAP_TYPE_18BPP_666,
    //    gfx_GetDataSize_18BPP_666,
    //    gfx_GetPixel_18BPP_666,
    //    gfx_PutPixel_18BPP_666,
    //    gfx_DrawClippedLine_18BPP_666
    //},
    {
        BITMAP_TYPE_1BPP_VERTICAL,
        gfx_GetDataSize_1BPP_Vertical,
        gfx_GetPixel_1BPP_Vertical,
        gfx_PutPixel_1BPP_Vertical,
        gfx_2d_DrawClippedLineGeneric
    },
    //{
    //    BITMAP_TYPE_8BPP_PALETTE,
    //    gfx_GetDataSize_8BPP_Palette,
    //    gfx_GetPixel_8BPP_Palette,
    //    gfx_PutPixel_8BPP_Palette,
    //    gfx_2d_DrawClippedLineGeneric
    //},
    //{
    //    BITMAP_TYPE_24BPP_888,
    //    gfx_GetDataSize_24BPP_888,
    //    gfx_GetPixel_24BPP_888,
    //    gfx_PutPixel_24BPP_888,
    //    gfx_2d_DrawClippedLineGeneric
    //},
//#ifndef WIN32
//    {
//        BITMAP_TYPE_16BPP_565_DD,
//        gfx_GetDataSize_16BPP_565_dd,
//        gfx_GetPixel_16BPP_565_dd,
//        gfx_PutPixel_16BPP_565_dd,
//        gfx_DrawClippedLine_16BPP_565_dd
//    },
//#endif
//    {
//        BITMAP_TYPE_32BPP_8888,
//        gfx_GetDataSize_32BPP_8888,
//        gfx_GetPixel_32BPP_8888,
//        gfx_PutPixel_32BPP_8888,
//        gfx_2d_DrawClippedLineGeneric
//    },
    //<! Insert new bitmap types here
    {
        BITMAP_TYPE_COUNT,
        NULL,
        NULL,
        NULL,
        NULL
    }
};

//! This structure array defines pairs of bitmap formats, and the accelerated function
//! for copying data between the two formats.   If the format is not listed, a generic
//! get pixel/put pixel copy mechanism will be performed.
const gfx_format_BitBltEntry_t g_gfx_BitBltFunctions[]=
{
    //{
    //    BITMAP_TYPE_1BPP_IDEAL,
    //    BITMAP_TYPE_1BPP_VERTICAL,
    //    gfx_bitblt_1BPP_Ideal_To_Vertical,
    //},
    //{
    //    BITMAP_TYPE_16BPP_565,
    //    BITMAP_TYPE_16BPP_565,
    //    gfx_bitblit_16BPP_565,
    //},
    //{
    //    BITMAP_TYPE_1BPP_IDEAL,
    //    BITMAP_TYPE_16BPP_565,
    //    gfx_bitblit_1BPP_Ideal_To_16BPP_565,
    //},
    //{
    //    BITMAP_TYPE_18BPP_666,
    //    BITMAP_TYPE_18BPP_666,
    //    gfx_bitblit_18BPP_666,
    //},
    //{
    //    BITMAP_TYPE_8BPP_PALETTE,
    //    BITMAP_TYPE_8BPP_PALETTE,
    //    gfx_bitblit_8BPP_Palette,
    //},
    //{
    //    BITMAP_TYPE_8BPP_PALETTE,
    //    BITMAP_TYPE_16BPP_565,
    //    gfx_bitblit_8BPP_Palette_To_16BPP_565,
    //},
    //{
    //    BITMAP_TYPE_24BPP_888,
    //    BITMAP_TYPE_24BPP_888,
    //    gfx_bitblit_24BPP_888,
    //},
    //{
    //    BITMAP_TYPE_24BPP_888,
    //    BITMAP_TYPE_16BPP_565,
    //    gfx_bitblit_24BPP_888_To_16BPP_565,
    //},
//#ifndef WIN32
    //{
    //    BITMAP_TYPE_16BPP_565_DD,
    //    BITMAP_TYPE_16BPP_565_DD,
    //    gfx_bitblit_16BPP_565_dd,
    //},
    //{
    //    BITMAP_TYPE_16BPP_565,
    //    BITMAP_TYPE_16BPP_565_DD,
    //    gfx_bitblit_16BPP_565_dd,
    //},
    //{
    //    BITMAP_TYPE_1BPP_IDEAL,
    //    BITMAP_TYPE_16BPP_565_DD,
    //    gfx_bitblit_1BPP_Ideal_To_16BPP_565_dd,
    //},
//#endif
    //<! Insert new bitmap types here
    {
        BITMAP_TYPE_COUNT,
        BITMAP_TYPE_COUNT,
        NULL
    }
};

//! This structure array defines pairs of bitmap formats, and the accelerated function
//! for alphablending between the two formats.   If the format is not listed, a generic
//! get pixel/put pixel copy mechanism will be performed.
//const gfx_format_AlphaBlendEntry_t g_gfx_AlphaBlendFunctions[]=
//{
    //{
    //    BITMAP_TYPE_16BPP_565,
    //    BITMAP_TYPE_16BPP_565,
    //    gfx_alphablend_16BPP_565,
    //},
//#ifndef WIN32
    //{
    //    BITMAP_TYPE_16BPP_565,
    //    BITMAP_TYPE_16BPP_565_DD,
    //    gfx_alphablend_16BPP_565_dd,
    //},
    //{
    //    BITMAP_TYPE_16BPP_565_DD,
    //    BITMAP_TYPE_16BPP_565_DD,
    //    gfx_alphablend_16BPP_565_dd,
    //},
//#endif
    //{
    //    BITMAP_TYPE_32BPP_8888,
    //    BITMAP_TYPE_16BPP_565,
    //    gfx_alphablend_32BPP_888_To_16BPP_565,
    //},
    //<! Insert new bitmap types here
//    {
//        BITMAP_TYPE_COUNT,
//        BITMAP_TYPE_COUNT,
//        null
//    }
//};

//! This structure array defines pairs of bitmap formats, and the accelerated function
//! for copying data between the two formats.   If the format is not listed, a generic
//! get pixel/put pixel stretch mechanism will be performed.
//const gfx_format_StretchBltEntry_t g_gfx_StretchBltFunctions[]=
//{
    //<! Insert new bitmap types here
//    {
//        BITMAP_TYPE_COUNT,
//        BITMAP_TYPE_COUNT,
//        null,
//    },
//};


///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
//! @}
