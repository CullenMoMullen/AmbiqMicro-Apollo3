////////////////////////////////////////////////////////////////////////////////
//! \addtogroup cmp_gfx
//! @{
//
//! \file    gfx.h
//! \brief   Contains public header data for the graphics library.
//! \version 1.0
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFX_H__
#define __GFX_H__

#include "types.h"

#ifdef RGB
#undef RGB
#endif

//! \brief Macro that allows RGB (red, green, blues) values to be constructed.
//!The RGB macro and RGBA macro can be used to create a native color.  The format for a native color
//! is the following:
//!
//!    31      24|23     16|15      8|7       0
//!     ---------------------------------------
//!    |    A    |    B    |    G    |    R    |
//!     ---------------------------------------
//!
//! RGB is used to define the color of a single pixel of image data.
//! Combined, the intensities of Red, Green, and Blue components define a color.
#define RGB(R,G,B)      ((((B) & 0xFF) << 16) | (((G) & 0xFF) << 8) | ((R) & 0xFF) | 0xff000000)
//! \brief Macro used to define a color.
//!
//! This macro is similar to RGB except it allows for an alpha value to be specified for the color.
#define RGBA(R,G,B,A)   ((((B) & 0xFF) << 16) | (((G) & 0xFF) << 8) | ((R) & 0xFF) | ((A)<<24))

//! \brief Macro used to obtain the Red intensity of a color value.
#define GET_RED(X)   (((X)    )&0xff)
//! \brief Macro used to obtain the Green intensity of a color value.
#define GET_GREEN(X) (((X)>>8 )&0xff)
//! \brief Macro used to obtain the Blue intensity of a color value.
#define GET_BLUE(X)  (((X)>>16)&0xff)
//! \brief Macro used to obtain the Alpha channel of a color value.
#define GET_ALPHA(X) (((X)>>24)&0xff)

//! \brief Macro used to specify a no transparency value for an alpha channel.
#define ALPHA_OPAQUE 0xFF
//! \brief Macro used to specify a complete transparency value for an alpha channel.
#define ALPHA_TRANSPARENT 0

//! \brief Macro used to specify a 0 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_0_PERCENT 255
//! \brief Macro used to specify a 10 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_10_PERCENT 230
//! \brief Macro used to specify a 20 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_20_PERCENT 204
//! \brief Macro used to specify a 30 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_30_PERCENT 179
//! \brief Macro used to specify a 40 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_40_PERCENT 153
//! \brief Macro used to specify a 50 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_50_PERCENT 128
//! \brief Macro used to specify a 60 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_60_PERCENT 102
//! \brief Macro used to specify a 70 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_70_PERCENT 77
//! \brief Macro used to specify a 80 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_80_PERCENT 51
//! \brief Macro used to specify a 90 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_90_PERCENT 26
//! \brief Macro used to specify a 100 percent transparency value for an alpha channel.
#define ALPHA_TRANSPARENT_100_PERCENT 0


//! \brief Type definition for a color value.
//!
//! Color data is packed into a 32-bit WORD.  This data includes Red,
//! Green, and Blue intensity information as well as information about
//! the alpha channel.  To define a color the RGB or RGBA macros should be used.
//! Similarly GET_RED, GET_GREEN, GET_BLUE, and GET_ALPHA can be used to obtain
//! the various components of a gfx_Color_t value.
typedef uint32_t gfx_Color_t;


//! \brief Standard definition for the color white.
#define COLOR_WHITE         RGB(255,255,255)
//! \brief Standard definition for the color teal.
#define COLOR_TEAL          RGB(0,255,255)
//! \brief Standard definition for the color purple.
#define COLOR_PURPLE        RGB(255,0,255)
//! \brief Standard definition for the color blue.
#define COLOR_BLUE          RGB(0,0,255)
//! \brief Standard definition for the color light gray.
#define COLOR_LIGHT_GRAY    RGB(192,192,192)
//! \brief Standard definition for the color dark gray.
#define COLOR_DARK_GRAY     RGB(128,128,128)
//! \brief Standard definition for the color dark teal.
#define COLOR_DARK_TEAL     RGB(0,128,128)
//! \brief Standard definition for the color dark purple.
#define COLOR_DARK_PURPLE   RGB(128,0,128)
//! \brief Standard definition for the color dark blue.
#define COLOR_DARK_BLUE     RGB(0,0,128)
//! \brief Standard definition for the color yellow.
#define COLOR_YELLOW        RGB(255,255,0)
//! \brief Standard definition for the color green.
#define COLOR_GREEN         RGB(0,255,0)
//! \brief Standard definition for the color dark yellow.
#define COLOR_DARK_YELLOW   RGB(128,128,0)
//! \brief Standard definition for the color dark green.
#define COLOR_DARK_GREEN    RGB(0,128,0)
//! \brief Standard definition for the color red.
#define COLOR_RED           RGB(255,0,0)
//! \brief Standard definition for the color dark red.
#define COLOR_DARK_RED      RGB(128,0,0)
//! \brief Standard definition for the color black.
#define COLOR_BLACK         RGB(0,0,0)


typedef enum _gfx_ColorPaletteIdx_t_ {
    PALETTE_FG_COLOR_IDX = 0,
    PALETTE_BG_COLOR_IDX = 1,
    PALETTE_NUM_COLORS
} gfx_ColorPaletteIdx_t;

//! Enum defining the encoding types for text strings
typedef enum _txt_StringTypesEnum{
    //! \brief Standard DBCS
    TXT_STRING_TYPE_DBCS = 0,
    //! \brief UTF16
    TXT_STRING_TYPE_UTF16,
    //! \brief UTF8/ASCII (It is recommended to treat ASCII strings as UTF-8 to avoid accessing the DBCS code page resources during conversion).
    TXT_STRING_TYPE_UTF8
} txt_StringTypesEnum_t;

//! Enum defining the orientation for text strings
typedef enum _txt_StringOrientationEnum{
    //! \brief Zero degree
    GFX_ROTATE_0 = 0,
    //! \brief 90 degree
    GFX_ROTATE_90,
    //! \brief 180 degree
    GFX_ROTATE_180,
    //! \brief 270 degree    
    GFX_ROTATE_270    
} gfx_OrientationEnum_t;

//! Enum defining the different bitmap types.
typedef enum _gfx_BitmapTypeEnum
{
    //! \brief RGB Color with 5 bits red, 6 bits green, and 5 bits blue.
    //BITMAP_TYPE_16BPP_565 = 0,
    //! \brief Monochrome Color (1 bpp) with pixels arrange in row major ordering.
    BITMAP_TYPE_1BPP_IDEAL,
    //! \brief RGB Color with 6 bits red, 6 bits green, and 6 bits blue.
    //BITMAP_TYPE_18BPP_666,
    //! \brief Monochrome Color (1 bpp) with pixels arrange in 8 bit column ordering.
    BITMAP_TYPE_1BPP_VERTICAL,
    //! \brief RGB Color with 8 bits red, 8 bits green, and 8 bits blue.
    //BITMAP_TYPE_24BPP_888,
    //BITMAP_TYPE_32BPP_8888,
    BITMAP_TYPE_INVALID,
    //! \brief Total number of currently supported color encodings.
    BITMAP_TYPE_COUNT = BITMAP_TYPE_INVALID
} gfx_BitmapTypeEnum_t;


//!  Definition of a bitmap for the GFX library
typedef struct _gfx_Bitmap
{
    //! \brief Type of bitmap this header is
    unsigned uType      : 8;
    //! \brief Width of bitmap (in pixels)
    unsigned uWidth     : 12;
    //! \brief Height of bitmap (in pixels)
    unsigned uHeight    : 12;
    //! \brief Data of the bitmap
    unsigned char     pData[];
} gfx_Bitmap_t;


//! Definition of a rectangle for the GFX library
typedef struct _gfx_Rect
{
    //! \brief X position of upper left corner
    int16_t x;
    //! \brief Y position of upper left corner
    int16_t y;
    //! \brief Width of rectangle
    int16_t width;
    //! \brief Height of rectangle
    int16_t height;
} gfx_Rect_t;

//! \brief Used in defining active clipping regions
typedef struct _gfx_RectNode
{
    //! \brief Points to the next rectangle node in the clipping region linked list.
    struct _gfx_RectNode *pNext;
    //! \brief The rectangle data for this rectangle node in the clipping region.
    gfx_Rect_t Rect;
} gfx_RectNode_t;

//! \brief Used in defining active clipping regions
typedef struct _gfx_ClipNode
{
    //! \brief Pointer to the next node in the clipping region linked list.
    struct _gfx_ClipNode *pNext;
    //! \brief Pointer to the rectangle data for this node.
    gfx_RectNode_t *pNode;
} gfx_ClipNode_t;


//! \brief Defines values needed for translating a drawing command to a target bitmap.
typedef struct _gfx_DeviceContext
{
    //! \brief Used to offset the origin of the DC so that a Widget drawing to the DC
    //!  can assume (0,0) is the upper left corner of their client rectangle.
    short           i16xOffset;
    //! \brief Used to offset the origin of the DC so that a Widget drawing to the DC                                    
    //!  can assume (0,0) is the upper left corner of their client rectangle.
    short           i16yOffset;         
                                        
    //! \brief Active palette.
    gfx_Color_t    *pPalette;
    //! \brief Pointer to a chain of gfx_RectNode_t that represent the active clipping region.
    gfx_RectNode_t *pActiveClipRegion;  
    //! \brief Stack of clipping regions saved to be restored at a later time                                    
    gfx_ClipNode_t *pClippingStack;     
    //! \brief Destination of all the graphics operations on the device context. Can be smaller than DC size (if so frame slicing will be used).
    gfx_Bitmap_t   *pBitmap;            
    bool           bDirty;              //!< Indicates the DC has been drawn to
    uint32_t       u32MaxFrameSizeInBytes;  //!< Maximum Frame Size in bytes
    uint16_t       u16Width;            //!< Actual width in pixels of the DC
    uint16_t       u16Height;           //!< Actual height in pixels of the DC
    gfx_Rect_t     m_CurrentSliceRect;  //!< Rectangle representing the current Slice being drawn
} gfx_DeviceContext_t;

//! \brief Used as an input to stretch blit to determine the resampling filter to use for the stretch operation.
typedef enum _gfx_StretchFilter_t{
    //! \brief Simplest filter requiring the least processing, but provides the least sharpness in the output (not good for < 50% reduction or > 200% magnification).
    NEAREST_NEIGHBOR_STRETCH = 0,
    //! \brief Resampling filter that uses a weighted average of four surrounding pixels in the source image to generate the destination pixel.
    BILINEAR_INTERPOLATION_STRETH,
    //! \brief Resampling filter that uses an average of 16 pixel in the source image to generate a pixel in the destination.
    BICUBIC_INTERPOLATION_STRETCH
} gfx_StretchFilter_t;

//! Global structure for the GFX library
typedef struct _gfx_Globals
{
    gfx_ClipNode_t *pClipNodes;         //!< Pointer to the available clip nodes
    gfx_RectNode_t *pRectNodes;         //!< Pointer to the available rect nodes
} gfx_Globals_t;

extern gfx_Globals_t g_gfx_Globals;
extern gfx_Color_t gfx_1BPP_DefaultPalette[2];


//! \brief Declares the size of a buffer in bytes to be used as a gfx_Bitmap_t (see \ref GFX_INIT_STATIC_BITMAP)
#define GFX_BITMAP_BUFFER_SIZE(width, height, bytesPerPixel) (sizeof(gfx_Bitmap_t) + ((width) * (height) * (bytesPerPixel)))
#define GFX_BITMAP_BUFFER_SZ_1BPP_VERT(width, height) (sizeof(gfx_Bitmap_t) + (((height+7)/8) * width))


//! \brief Initializes a pointer to a statically declared gfx_Bitmap_t buffer (see \ref GFX_BITMAP_BUFFER_SIZE)
#define GFX_INIT_STATIC_BITMAP(bitmapPtrName, bufferName, type, width, height)\
do { \
    (bitmapPtrName) = (gfx_Bitmap_t*) (bufferName); \
    (bitmapPtrName)->uType = (type); \
    (bitmapPtrName)->uWidth = (width); \
    (bitmapPtrName)->uHeight = (height); \
} while(0)

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////
//! \brief Creates a rectangle
//!
//! \param[in] x            X position of the rectangle.
//! \param[in] y            Y position of the rectangle.
//! \param[in] width        Width position of the rectangle.
//! \param[in] height       Height position of the rectangle.
//! \return Newly created gfx_Rect_t structure
////////////////////////////////////////////////////////////////////////////////
gfx_Rect_t gfx_rect_Make(int16_t x, int16_t y, uint16_t width, uint16_t height);

////////////////////////////////////////////////////////////////////////////////
//! \brief FOR_INTERNAL_USE_ONLY
//!
//! \internal
//! This function logically ANDs two gfx_Rect_t structures.
//!
//! \param[in,out] pRect  Source/destination rectangle to AND
//! \param[in]     pRectB Rectangle which to AND the source rect with.
//! \return true if the two there is any overlap; false if there is not.
//!
//! \note This function is an internal helper function and is not for calling from outside the graphics library.
//!
//! \todo [PUBS] This function is marked internal; it should be removed from the public header?
////////////////////////////////////////////////////////////////////////////////
bool gfx_rect_And(gfx_Rect_t *pRect, gfx_Rect_t *pRectB);

////////////////////////////////////////////////////////////////////////////////
//! \brief Determines if a rectangle is in the active clipping region.
//!
//!  This function can be used in the Widget->Draw function to determine if a rect is in
//!  the active clipping region.
//!  When this function is called, the coordinates system is
//!  translated to widget coordinates and no other translation is necessary.
//!
//! \param[in] pDC   Device context passed into the Draw() function
//! \param[in] pRect Pointer to the rectangle
//!
//! \retval TRUE If the rectangle is in the clipping region
//! \retval FALSE If the rectangle is not in the clipping region
////////////////////////////////////////////////////////////////////////////////
bool gfx_clip_IsRectInClippingRegion(gfx_DeviceContext_t *pDC, gfx_Rect_t *pRect);

////////////////////////////////////////////////////////////////////////////////
//! \brief Intersects the current clipping region with a given rectangle.
//!
//! This function intersects (logically ANDs) the current clipping region with the given rectangle.
//!
//! \param[in] pDC  gfx_DeviceContext_t on which to operate.
//! \param[in] x0   Left of the rectangle to intersect with
//! \param[in] y0   Top of the rectangle to intersect with
//! \param[in] x1   Right of the rectangle to intersect with
//! \param[in] y1   Bottom of the rectangle to intersect with
//!
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_IntersectByPoints(gfx_DeviceContext_t *pDC, int x0, int y0, int x1, int y1);

////////////////////////////////////////////////////////////////////////////////
//! \brief Intersects the current clipping region with a given rectangle.
//!
//! This function intersects (logically ANDs) the current clipping region with the given rectangle.
//!
//! \param[in] pDC    gfx_DeviceContext_t on which to operate.
//! \param[in] rect   Rectangle to intersect with
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_Intersect(gfx_DeviceContext_t *pDC, gfx_Rect_t *rect);

////////////////////////////////////////////////////////////////////////////////
//! \brief TBD
//!
//! \param[in] pDC  gfx_DeviceContext_t on which to operate.
//! \param[in] rect Pointer to the rectangle
//!
//! \todo [PUBS] Add brief description for function
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_Union(gfx_DeviceContext_t *pDC, gfx_Rect_t *rect);

//! \brief FOR_INTERNAL_USE_ONLY
//! \todo [PUBS] This function is marked internal in gfx_clip.c
void gfx_clip_AddRect(gfx_DeviceContext_t *pDC, gfx_Rect_t *rect);

////////////////////////////////////////////////////////////////////////////////
//! \brief Excludes a given rectangle from the current clipping region.
//!
//! This function excludes (subtracts)  the given rectangle from the current clipping region.
//!
//! \param[in] pDC    gfx_DeviceContext_t on which to operate.
//! \param[in] left   Left of the rectangle to exclude
//! \param[in] top    Top of the rectangle to exclude
//! \param[in] right  Right of the rectangle to exclude
//! \param[in] bottom Bottom of the rectangle to exclude
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_ExcludeByPoints(gfx_DeviceContext_t *pDC, int left, int top, int right, int bottom);

////////////////////////////////////////////////////////////////////////////////
//! \brief Excludes a given rectangle from the current clipping region.
//!
//! This function excludes (subtracts)  the given rectangle from the current clipping region.
//!
//! \param[in] pDC    gfx_DeviceContext_t on which to operate.
//! \param[in] rect   Rectangle to subtract
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_Exclude(gfx_DeviceContext_t *pDC, gfx_Rect_t *rect);

////////////////////////////////////////////////////////////////////////////////
//! \brief Restore the clip region of the gfx_DeviceContext_t to a state previously saved.
//!
//! This function frees up the resources of the current clip region and pops off the clip region stored
//! in the gfx_DeviceContext_t's clip region stack .
//!
//! \param[in] pDC gfx_DeviceContext_t to pop the clip region from the stack.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_PopClippingRegion(gfx_DeviceContext_t* pDC);

////////////////////////////////////////////////////////////////////////////////
//! \brief Saves the clip region of the gfx_DeviceContext_t for restoring at a later time.
//!
//! This function pushes a copy of the current clip region onto the gfx_DeviceContext_t's clip region
//! stack.
//!
//! \param[in] pDC gfx_DeviceContext_t on which to operate.
////////////////////////////////////////////////////////////////////////////////
void gfx_clip_PushClippingRegion(gfx_DeviceContext_t* pDC);

//! \brief TBD
//! \todo [PUBS] Where is this function defined? Cannot find in source tree.
void gfx_clip_ResetClippingRect(gfx_DeviceContext_t* pDC);

//! \brief FOR_INTERNAL_USE_ONLY
//! \todo [PUBS] This function is marked internal in gfx_clip.c
void gfx_clip_ReleaseClipChain(gfx_RectNode_t *pChain);

////////////////////////////////////////////////////////////////////////////////
//! \brief Draws a bitmap into a device context.
//!
//! This function draws a bitmap onto the device context, bounded by the clipping region.
//!
//! \param[in] pDC   gfx_DeviceContext_t to draw onto
//! \param[in] ...   Pointer to the source bitmap
//! \param[in] x0    X position (relative to current origin)
//! \param[in] y0    Y position (relative to current origin)
////////////////////////////////////////////////////////////////////////////////
void gfx_bmp_DrawBitmapByAddr(gfx_DeviceContext_t *pDC, gfx_Bitmap_t*, int x0, int y0);

////////////////////////////////////////////////////////////////////////////////
//! \brief Rotate a bitmap
//!
//! \param[in]  pSrc            Source Bitmap
//! \param[in]  pDest           Destination Bitmap
//! \param[in]  eOrientation    Orientation to be rotate
//!
//! \returns NULL
////////////////////////////////////////////////////////////////////////////////
void gfx_bmp_RotateBitmap(gfx_Color_t *pPalette, gfx_Bitmap_t *pSrc, gfx_Bitmap_t *pDest, gfx_OrientationEnum_t eOrientation);

////////////////////////////////////////////////////////////////////////////////
//! \brief Creates a bitmap from the handle allocation heap of specific size and format.
//!
//! \param[in]  eBitmapType Bitmap format
//! \param[in]  u16x        Width of bitmap
//! \param[in]  u16y        Height of bitmap
//!
//! \returns Pointer to a bitmap structure
////////////////////////////////////////////////////////////////////////////////
gfx_Bitmap_t *gfx_bmp_CreateBitmap(unsigned char eBitmapType, uint16_t u16x, uint16_t u16y);

////////////////////////////////////////////////////////////////////////////////
//! \brief Draws a solid rectangle onto the current context, clipped by the active clipping region.
//! 
//! \param[in] pDC      The target gfx_DeviceContext_t
//! \param[in] pRect    The rectangle to draw
//! \param[in] color    The color of the filled rectangle to draw.
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_FillRect(gfx_DeviceContext_t *pDC, gfx_Rect_t *pRect, gfx_Color_t color);
////////////////////////////////////////////////////////////////////////////////
//! \brief Draws a rectangle onto the current context, clipped by the active clipping region.
//! 
//! \param[in] pDC          The target gfx_DeviceContext_t
//! \param[in] left         Left edge of the rectangle
//! \param[in] top          Top edge of the rectangle
//! \param[in] right        Right edge of the rectangle
//! \param[in] bottom       Bottom edge of the rectangle
//! \param[in] StartColor   The start color of the filled rectangle to draw.
//! \param[in] EndColor     The end color of the filled rectangle to draw.
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_GradientRectangleByPoints(gfx_DeviceContext_t *pDC, 
                                        int left, int top, 
                                        int right, int bottom,
                                        gfx_Color_t StartColor, gfx_Color_t EndColor);

////////////////////////////////////////////////////////////////////////////////
//! \brief Draws a rectangle onto the context, clipped by the active clipping region.
//! 
//! This function differs from gfx_2d_GradientRectangleByPoints in that the gradient
//! will have the same color on the top and bottom of the rectangle and will fade into 
//! the same color at the middle of the rectangle.
//!
//! \param[in] pDC          The target gfx_DeviceContext_t
//! \param[in] left         Left edge of the rectangle
//! \param[in] top          Top edge of the rectangle
//! \param[in] right        Right edge of the rectangle
//! \param[in] bottom       Bottom edge of the rectangle
//! \param[in] StartColor   The start color of the filled rectangle to draw.
//! \param[in] EndColor     The end color of the filled rectangle to draw
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_GradientInwards(gfx_DeviceContext_t *pDC, 
                                  int left, int top, 
                                  int right, int bottom,
                                  gfx_Color_t StartColor, gfx_Color_t EndColor);

////////////////////////////////////////////////////////////////////////////////
//! \brief Draw a rectangle onto the current context, clipped by the active clipping region.
//! 
//! \param[in] pDC      The target gfx_DeviceContext_t
//! \param[in] pRect    Pointer to a data structure containing the rectangle dimensions
//! \param[in] color    The color of the rectangle
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_Rectangle(gfx_DeviceContext_t *pDC, gfx_Rect_t *pRect, gfx_Color_t color);

////////////////////////////////////////////////////////////////////////////////
//! \brief Draw a rectangle onto the current context, clipped by the active clipping region.
//! 
//! \param[in] pDC      The target gfx_DeviceContext_t
//! \param[in] left     Left edge of the rectangle
//! \param[in] top      Top edge of the rectangle
//! \param[in] right    Right edge of the rectangle
//! \param[in] bottom   Bottom edge of the rectangle
//! \param[in] color    The color of the rectangle
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_RectangleByPoints(gfx_DeviceContext_t *pDC,int left, int top, int right, int bottom, gfx_Color_t color);

////////////////////////////////////////////////////////////////////////////////
//! \brief Draws a line onto the current context, clipped by the active clipping region.
//! 
//! \param[in] pDC      The target gfx_DeviceContext_t
//! \param[in] x0       x of point 0
//! \param[in] y0       y of point 0
//! \param[in] x1       x of point 1
//! \param[in] y1       y of point 1
//! \param[in] color    The color of the line
//!
//! \todo This function may be excess functionality than required.  The typical usage of line drawing
//!       in our system is horizontal or vertical lines, which are much simpler to clip.
////////////////////////////////////////////////////////////////////////////////
void gfx_2d_DrawLine(gfx_DeviceContext_t *pDC, int x0, int y0, int x1, int y1, gfx_Color_t color);

////////////////////////////////////////////////////////////////////////////////
//! \brief Offsets the origin of the gfx_DeviceContext_t
//! \param[in] pDC  The gfx_DeviceContext_t to operate on to check within.
//! \param[in] x    The x offset
//! \param[in] y    The y offset
////////////////////////////////////////////////////////////////////////////////
void gfx_org_Offset(gfx_DeviceContext_t *pDC, int x, int y);

////////////////////////////////////////////////////////////////////////////////
//! \brief Returns the width (in pixels) for a text string in any encoding
//!
//!
//! \param[in] StringType   String encoding format
//! \param[in] StringPtr    Pointer to the string to find the width of
//! \param[in] pFont        Pointer to a font to use for finding the width
//! \return Width in pixels of the string.
////////////////////////////////////////////////////////////////////////////////
//uint16_t gfx_text_GetStringWidth(txt_StringTypesEnum_t StringType, void* StringPtr, gfx_Font_t *pFont);

////////////////////////////////////////////////////////////////////////////////
//! \brief Selects the current drawing color, returning the previous
//! \returns The previous drawing color.
//! \todo Come up with how this is really going to work.  This API hasn't been fully explored.
//! \param[in] pDC  The gfx_DeviceContext_t to operate on to check within.
//! \param[in] ...  The new drawing color
//! \return previous draw color
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t gfx_2d_SetDrawColor(gfx_DeviceContext_t *pDC, gfx_Color_t);

////////////////////////////////////////////////////////////////////////////////
//! \brief Selects the current drawing color, returning the previous
//! \returns The previous background color.
//! \todo Come up with how this is really going to work.  This API hasn't been fully explored.
//! \param[in] pDC  The gfx_DeviceContext_t to operate on to check within.
//! \param[in] ...  The new background color
//! \return previous background color
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t gfx_2d_SetBackgroundColor(gfx_DeviceContext_t *pDC, gfx_Color_t);

////////////////////////////////////////////////////////////////////////////////
//! \brief Gets the gfx_Color_t that is considered the "system Background Color"
//! \return The System Background Color.
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t gfx_2d_GetSystemBackgroundColor(void);
////////////////////////////////////////////////////////////////////////////////
//! \brief Gets the gfx_Color_t that is considered the "system Draw Color"
//! \return The System Draw Color.
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t gfx_2d_GetSystemDrawColor(void);

////////////////////////////////////////////////////////////////////////////////
//! \brief Changes the palette of a DC returning the previous.
//! \returns The previous palette.
//! \param[in] pDC         The gfx_DeviceContext_t to set the palette for.
//! \param[in] pPalette    The new palette.
//! \return previous palette
////////////////////////////////////////////////////////////////////////////////
gfx_Color_t* gfx_2d_SetColorPalette(gfx_DeviceContext_t *pDC, gfx_Color_t *pPalette);

////////////////////////////////////////////////////////////////////////////////
//! \brief Gets a pixel's color value from a bitmap
//!
//! This function gets the pixel color at a given x,y converting it as best it can to match the ideal RGB
//! definition.  This function checks the x,y coordinate and ensures its within the bounds of the bitmap, then
//! calls the bitmap type specific get pixel function.
//!
//! \param[in]  pPalette Pointer to the color palette
//! \param[in]  pSrc     Pointer to the bitmap
//! \param[in]  x        x position of the pixel
//! \param[in]  y        y position of the pixel
//! \param[out] pPixel   Pointer to a gfx_Color_t object to populate with the RGB color
//!
//! \return True if pixel is within the bitmap, false if not.
////////////////////////////////////////////////////////////////////////////////
bool gfx_bmp_GetPixel(gfx_Color_t *pPalette, gfx_Bitmap_t *pSrc, int x , int y, gfx_Color_t *pPixel);

////////////////////////////////////////////////////////////////////////////////
//! \brief Sets a pixel's color value in a bitmap
//!
//! This function sets the pixel color at a given x,y converting it as best it can to match the ideal RGB
//! definition.  This function checks the x,y coordinate and ensures its within the bounds of the bitmap, then
//! calls the bitmap type specific set pixel function.
//!
//! \param[in] pPalette Pointer to the color palette
//! \param[in] pSrc     Pointer to the bitmap
//! \param[in] x        x position of the pixel
//! \param[in] y        y position of the pixel
//! \param[in] uPixel   RGB color to set
//!
//! \return True if pixel is within the bitmap, false if not.
////////////////////////////////////////////////////////////////////////////////
bool gfx_bmp_PutPixel(gfx_Color_t *pPalette, gfx_Bitmap_t *pSrc, int x , int y, gfx_Color_t uPixel);

////////////////////////////////////////////////////////////////////////////////
//! \brief Loads a bitmap from the resource file.
//!
//! Returns a pointer to a newly allocated gfx_Bitmap_t object after populating it with bitmap data from
//! the resource file.
//!
//! \param[in] uResourceID  Resource ID to load.
//! \param[in] pHndl        Pointer to a handle to assign the handle for the memory allocated for the loaded resource.
//!
//! \return  Pointer to a newly allocated gfx_Bitmap_t object.
//!
//! \note This bitmap is not under control of the bitmap caching system.
////////////////////////////////////////////////////////////////////////////////
//gfx_Bitmap_t *gfx_rsrc_LoadBitmap(uint32_t uResourceID, util_Handle_t *pHndl);

////////////////////////////////////////////////////////////////////////////////
//! \brief Initializes the Graphics component
//!
//! \return SUCCESS or error from memory allocation
//!
//! \param[in] u16RectNodes Number of rectanlge nodes that can by used by the grpahics library
//! \param[in] u16ClipNodes Number of clipping nodes to be used by the graphics library
////////////////////////////////////////////////////////////////////////////////
uint32_t gfx_Initialize(uint16_t u16RectNodes, uint16_t u16ClipNodes);


uint32_t gfx_2d_DeviceContextInit(gfx_Bitmap_t* pFrameBuffer, gfx_Color_t *pPalette,
                                  uint16_t u16RectNodes, uint16_t u16ClipNodes);

gfx_DeviceContext_t* gfx_2d_DeviceContextGet(void);
gfx_RectNode_t *gfx_clip_Get(void);
gfx_ClipNode_t *gfx_clip_GetClipNode(void);
void gfx_clip_ReleaseClipChain(gfx_RectNode_t *pChain);
#ifdef __cplusplus
}
#endif

#endif
////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}
