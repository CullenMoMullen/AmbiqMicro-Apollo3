////////////////////////////////////////////////////////////////////////////////
//! \addtogroup cmp_gfx
//! @{
//
//! \file    gfx_format.h
//! \brief   Contains structures and function prototypes for supported bitmap types.
//!
//! The tables in this file define support for specific bitmap formats and acceleration.
//!
//! \todo [PUBS] Add definitions for TBDs in this file.
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//! \brief Typedef for DrawLine function pointer
//!
//! Function typedef for a optimized DrawLine function to be used with the gfx library.  A function
//! pointer to a function of this type is returned from a lookup table during a DrawLine
//! so that the appropriate DrawLine function is called for the specfifc color format.  If no
//! optimized version of DrawLine is available for the color format then the graphics library
//! will use PutPixel to accomplish the drawing of a line.
////////////////////////////////////////////////////////////////////////////////
typedef void (gfx_format_LineDraw_t)(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest,
                                     int x0, int y0, int x1, int y1, gfx_Color_t color);
////////////////////////////////////////////////////////////////////////////////
//! \brief Typedef for PutPixel function pointer
//!
//! Function typedef for a PutPixel function to be used with the gfx library.  A function
//! pointer to a function of this type is returned from a lookup table during a PutPixel
//! so that the appropriate PutPixel function is called for the specfifc color format
////////////////////////////////////////////////////////////////////////////////
typedef void(gfx_format_PutPixel_t)(gfx_Color_t *pPalette, gfx_Bitmap_t *, uint16_t,
                                    uint16_t, gfx_Color_t );

////////////////////////////////////////////////////////////////////////////////
//! \brief Typedef for GetPixel function pointer
//!
//! Function typedef for a GetPixel function to be used with the gfx library.  A function
//! pointer to a function of this type is returned from a lookup table during a GetPixel
//! so that the appropriate GetPixel function is called for the specfifc color format
////////////////////////////////////////////////////////////////////////////////
typedef void(gfx_format_GetPixel_t)(gfx_Color_t *pPalette, gfx_Bitmap_t *, uint16_t,
                                    uint16_t, gfx_Color_t *);

////////////////////////////////////////////////////////////////////////////////
//! \brief Typedef for GetDataSize function pointer
//!
//! Function typedef for a GetDataSize function to be used with the gfx library.  A function
//! pointer to a function of this type is returned from a lookup table during a GetDataSize
//! so that the appropriate GetDataSize function is called for the specfifc color format
////////////////////////////////////////////////////////////////////////////////
typedef uint32_t(gfx_format_GetDataSize_t)(uint16_t,uint16_t);

////////////////////////////////////////////////////////////////////////////////
//! \brief Typedef for DrawLine function pointer
//!
//! Function typedef for a optimized DrawLine function to be used with the gfx library.  A function
//! pointer to a function of this type is returned from a lookup table during a DrawLine
//! so that the appropriate DrawLine function is called for the specfifc color format.  If no
//! optimized version of DrawLine is available for the color format then the graphics library
//! will use PutPixel to accomplish the drawing of a line.
////////////////////////////////////////////////////////////////////////////////
typedef void(gfx_format_DrawLine_t)(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest,
                      int x0, int y0, int x1, int y1, gfx_Color_t color);
////////////////////////////////////////////////////////////////////////////////
//! \brief Typedef for BitBlt function pointer
//!
//! Function typedef for a BitBlt function to be used with the gfx library.  A function
//! pointer to a function of this type is returned from a lookup table during a bitblt
//! so that the appropriate bitblt function is called for the specfifc source/destination
//! format pair.
////////////////////////////////////////////////////////////////////////////////
typedef void (gfx_format_Bitblt_t)(gfx_Color_t *pPalette, gfx_Bitmap_t*,gfx_Rect_t,
                                        gfx_Bitmap_t*,int,int);
////////////////////////////////////////////////////////////////////////////////
//! \brief Typedef for BitBlt function pointer
//!
//! Function typedef for a BitBlt function to be used with the gfx library.  A function
//! pointer to a function of this type is returned from a lookup table during a bitblt
//! so that the appropriate bitblt function is called for the specfifc source/destination
//! format pair.
////////////////////////////////////////////////////////////////////////////////
typedef void(gfx_format_GetBitBlt_t)(gfx_Color_t *pPalette, gfx_Bitmap_t*,gfx_Rect_t,
                                        gfx_Bitmap_t*,int,int);


//! \brief Structure for encapsulating all the function handlers for a particular bitmap format.
typedef struct _gfx_BitmapFormat
{
    //! \brief Unique identifier for this format.
    unsigned char uUniqueIdentifier;
    //! \brief Get Data Size member
    gfx_format_GetDataSize_t * const GetBitmapDataSize;
    //! \brief Get Pixel Member
    gfx_format_GetPixel_t    * const GetPixelFunction;
    //! \brief Put pixel member
    gfx_format_PutPixel_t    * const PutPixelFunction;
    //! \brief Draw Line member
    gfx_format_DrawLine_t    * const DrawLineFunction;
} gfx_BitmapFormat_t;

//! \brief Structure for defining the accellerated  bitblt formats.
typedef struct _gfx_format_BitBltEntry
{
    //! \brief Identifier for source bitmap type
    unsigned char uSourceIdentifier;
    //! \brief Identifier for destination bitmap type
    unsigned char uDestinationIdentifier;
    //! \brief BitBlt function pointer.
    gfx_format_Bitblt_t *const pBitBltFunction;
} gfx_format_BitBltEntry_t;

//! \brief Null terminated table of supported bitmap types.
extern const gfx_BitmapFormat_t g_gfx_BitmapSupportTable[];

//! \brief Reference to global Function pointer look up table for bitblt functions
extern const gfx_format_BitBltEntry_t   g_gfx_BitBltFunctions[];

///////////////////////////////////////////////////////////////////////////////
//! \brief Function to obtain the 'GetPixel' function of a bitmap type.
//! 
//! \param[in] * Pointer to a bitmap for which the 'getpixel' function is needed
//!
//! \return Pointer to the function for 'get pixel'.
//!
//! \note This function is called by many of the graphics APIs internally, however
//!       it is exposed in case the customer would like to get the pointer to
//!       the GetPixel function for a particular bitmap type.
///////////////////////////////////////////////////////////////////////////////
gfx_format_GetPixel_t    *gfx_format_GetPixel(gfx_Bitmap_t*);

///////////////////////////////////////////////////////////////////////////////
//! \brief Function to obtain the 'PutPixel' function of a bitmap type.
//!
//! Internal function to obtain the 'PutPixel' function of a bitmap type.
//! 
//! \param[in] * Pointer to a bitmap for which the 'putpixel' function is needed
//! \return Pointer to the function for 'put pixel'.
//!
//! \note This function is called by many of the graphics APIs internally, however
//!       it is exposed in case the customer would like to get the pointer to
//!       the PutPixel function for a particular bitmap type.
///////////////////////////////////////////////////////////////////////////////
gfx_format_PutPixel_t    *gfx_format_PutPixel(gfx_Bitmap_t*);

///////////////////////////////////////////////////////////////////////////////
//! \brief Function to obtain the 'BitBlt' function between two bitmaps. 
//! 
//! \param[in] * Pointer to the source bitmap.
//! \param[in] * Pointer to destination bitmap.
//!
//! \return Pointer to the function for 'BitBlt'.
//!
//! \note This function is called by many of the graphics APIs internally, however
//!       it is exposed in case the customer would like to get the pointer to
//!       the BitBlt function for a particular bitmap pair.
///////////////////////////////////////////////////////////////////////////////
gfx_format_GetBitBlt_t   *gfx_format_BitBlt(gfx_Bitmap_t*, gfx_Bitmap_t*);


///////////////////////////////////////////////////////////////////////////////
//! \brief Function to obtain the 'Get Data Size' function of a bitmap type.
//! 
//! \param[in] ... Format code for to obtain the 'get data size' function for.
//! \return Pointer to the function for 'get pixel'.
//!
//! \note This function is called by many of the graphics APIs internally, however
//!       it is exposed in case the customer would like to get the pointer to
//!       the GetDataSize function for a particular bitmap type.
///////////////////////////////////////////////////////////////////////////////
gfx_format_GetDataSize_t *gfx_format_GetDataSize(uint8_t);

///////////////////////////////////////////////////////////////////////////////
//! \brief Function to obtain the 'DrawLine' function of a bitmap type.
//! 
//! \param[in] * Pointer to a bitmap for which the 'DrawLine' function is needed
//! \return Pointer to the function for 'DrawLine'.
//!
//! \note This function is called by many of the graphics APIs internally, however
//!       it is exposed in case the customer would like to get the pointer to
//!       the GetLineDraw function for a particular bitmap type.
///////////////////////////////////////////////////////////////////////////////
gfx_format_LineDraw_t    *gfx_format_GetLineDraw(gfx_Bitmap_t*);

//function prototypes for BITMAP_TYPE_1BPP_IDEAL
////////////////////////////////////////////////////////////////////////////////
//! \brief Gets a pixel's color value from a BITMAP_TYPE_1BPP_IDEAL bitmap
//!
//! This function gets the pixel color at a given x,y converting it as best it can to match the ideal RGB
//! definition.  This function expects X and Y to be within the bitmap width and height.
//! (That check is done by gfx_bmp_GetPixel())
//!
//! \param[in]  pPalette  Pointer to the color palette.
//! \param[in]  pBitmap   Pointer to the bitmap
//! \param[in]  x         x position of the pixel
//! \param[in]  y         y position of the pixel
//! \param[out] pPixel    Pointer to a gfx_Color_t object to populate with the RGB color
//!
//! \note The prefered API to use instead of this function is gfx_bmp_GetPixel
//!       which will analyze the type of bitmap for which a pixel is being gotten
//!       and will call this function automatically, however this function is
//!       exposed so the customer may call it directly if desired.
////////////////////////////////////////////////////////////////////////////////
void gfx_GetPixel_1BPP_Ideal(gfx_Color_t *pPalette, gfx_Bitmap_t *pBitmap,
                             uint16_t x, uint16_t y, gfx_Color_t *pPixel);

////////////////////////////////////////////////////////////////////////////////
//! \brief Sets a pixel in a BITMAP_TYPE_1BPP_IDEAL bitmap
//!
//! This function sets the pixel at a given x,y to a given RGB color(converting it as best it can to match
//! the capabilities of the bitmap. This function expects X and Y to be within the bitmap width and height.
//! (That check is done by gfx_bmp_SetPixel())
//!
//! \param[in] pPalette  Pointer to the color palette.
//! \param[in] pBitmap   Pointer to the bitmap
//! \param[in] x                    x position of the pixel
//! \param[in] y                    y position of the pixel
//! \param[in] uRGB                 Color value to place in the bitmap
//!
//! \note The prefered API to use instead of this function is gfx_bmp_PutPixel
//!       which will analyze the type of bitmap for which a pixel is being put
//!       and will call this function automatically, however this function is
//!       exposed so the customer may call it directly if desired.
///////////////////////////////////////////////////////////////////////////////
void gfx_PutPixel_1BPP_Ideal(gfx_Color_t *pPalette, gfx_Bitmap_t *pBitmap,
                             uint16_t x, uint16_t y, gfx_Color_t uRGB);

////////////////////////////////////////////////////////////////////////////////
//! \brief Calculates the data size required to support a bitmap of format type BITMAP_TYPE_1BPP_IDEAL
//!
//! \param[in] u16x  Width of the bitmap to calculate the data size.
//! \param[in] u16y Height of the bitmap to calculate the data size.
//!
//! \note This function is called internally by graphics functions but is exposed
//!       in case the user would like to call it directly.
////////////////////////////////////////////////////////////////////////////////
uint32_t gfx_GetDataSize_1BPP_Ideal(uint16_t u16x, uint16_t u16y);

//function prototypes for BITMAP_TYPE_1BPP_VERTICAL
////////////////////////////////////////////////////////////////////////////////
//! \brief Gets a pixel's color value from a BITMAP_TYPE_1BPP_VERTICAL bitmap
//!
//! This function gets the pixel color at a given x,y converting it as best it can to match the ideal RGB
//! definition.  This function expects X and Y to be within the bitmap width and height.
//! (That check is done by gfx_bmp_GetPixel())
//!
//! \param[in]  pPalette  Pointer to the color palette.
//! \param[in]  pBitmap   Pointer to the bitmap
//! \param[in]  x         x position of the pixel
//! \param[in]  y         y position of the pixel
//! \param[out] pRGB      Pointer to a gfx_Color_t object to populate with the RGB color
//!
//! \note The prefered API to use instead of this function is gfx_bmp_GetPixel
//!       which will analyze the type of bitmap for which a pixel is being gotten
//!       and will call this function automatically, however this function is
//!       exposed so the customer may call it directly if desired.
////////////////////////////////////////////////////////////////////////////////
void gfx_GetPixel_1BPP_Vertical(gfx_Color_t *pPalette, gfx_Bitmap_t *pBitmap, 
                                uint16_t x, uint16_t y, gfx_Color_t *pRGB);

////////////////////////////////////////////////////////////////////////////////
//! \brief Sets a pixel in a BITMAP_TYPE_1BPP_VERTICAL bitmap
//!
//! This function sets the pixel at a given x,y to a given RGB color(converting it as best it can to match
//! the capabilities of the bitmap. This function expects X and Y to be within the bitmap width and height.
//! (That check is done by gfx_bmp_SetPixel())
//!
//! \param[in] pPalette  Pointer to the color palette.
//! \param[in] pBitmap   Pointer to the bitmap
//! \param[in] x                    x position of the pixel
//! \param[in] y                    y position of the pixel
//! \param[in] cRGB                 Color value to place in the bitmap
//!
//! \note The prefered API to use instead of this function is gfx_bmp_PutPixel
//!       which will analyze the type of bitmap for which a pixel is being put
//!       and will call this function automatically, however this function is
//!       exposed so the customer may call it directly if desired.
///////////////////////////////////////////////////////////////////////////////
void gfx_PutPixel_1BPP_Vertical(gfx_Color_t *pPalette, gfx_Bitmap_t *pBitmap, 
                                uint16_t x, uint16_t y, gfx_Color_t cRGB);

///////////////////////////////////////////////////////////////////////////////
//! \brief Calculates the data size required to support a bitmap of format type BITMAP_TYPE_1BPP_VERTICAL
//!
//! \param[in] u16x Width of the bitmap to calculate the data size.
//! \param[in] u16y Height of the bitmap to calculate the data size.
//!
//! \note This function is called internally by graphics functions but is exposed
//!       in case the user would like to call it directly.
///////////////////////////////////////////////////////////////////////////////
uint32_t gfx_GetDataSize_1BPP_Vertical(uint16_t u16x, uint16_t u16y);

//Generic Graphics Functions
////////////////////////////////////////////////////////////////////////////////
//! \brief This function is the generic bitblt operation.
//! 
//! This function copies a rectangle from one bitmap to another.  It is not optimized for any particular format,
//! but will work for all formats supported by gfx_bmp_PutPixel and gfx_bmp_GetPixel.
//!
//! \param[in] pPalette      Pointer to the color palette
//! \param[in] pDest         Pointer to the destination bitmap
//! \param[in] pSrc          Pointer to the source bitmap
//! \param[in] Rect          Rectangle to draw within
//! \param[in] SourceXoffset Value to offset the X coordinate of the source bitmap within the rectangle.
//! \param[in] SourceYoffset Value to offset the Y coordinate of the source bitmap within the rectangle.
//!
//! \note The prefered API to use instead of this function is gfx_bmp_DrawBitmap and gfx_bmp_DrawBitmapByAddress
//!       which will analyze the type of bitmaps for which a BitBlt is being performed
//!       and will call this function automatically, however this function is
//!       exposed so the customer may call it directly if desired.
///////////////////////////////////////////////////////////////////////////////
void gfx_bitblt_Generic(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest, gfx_Rect_t Rect,
                        gfx_Bitmap_t *pSrc, int SourceXoffset, int SourceYoffset);

////////////////////////////////////////////////////////////////////////////////
//! \brief Draws a line, pre-clipped, in a given color to a given bitmap.
//! 
//! This function draws a line, pre-clipped, onto any supported bitmap.
//! 
//! \param[in] pPalette  Pointer to the color palette.
//! \param[in] pDest     gfx_DeviceContext_t to operate on to check within.
//! \param[in] x0        x of point 0
//! \param[in] y0        y of point 0
//! \param[in] x1        x of point 1
//! \param[in] y1        y of point 1
//! \param[in] color     Color of the line
//!
//! \todo This function may be excess functionality than required.  The typical usage of line drawing
//!       in our system is horizontal or vertical lines, which are much simpler to clip.
//! \todo optimize:   the two branches (whether x or y is the incrementer) can be collapesed into one using
//!                   pointers to the original data.
//! 
//! \note The prefered API to use instead of this function is gfx_2d_DrawLine
//!       which will draw a line onto a device context.  This function is exposed
//!       so that a line may be drawn directly onto a bitmap instead of a device
//!       context.  The caller should check all bounds to make sure the line
//!       will fit within the bitmap being drawn to.
///////////////////////////////////////////////////////////////////////////////
void gfx_2d_DrawClippedLineGeneric(gfx_Color_t *pPalette, gfx_Bitmap_t *pDest, int x0,
                                   int y0, int x1, int y1, gfx_Color_t color);

////////////////////////////////////////////////////////////////////////////////
// End of file
////////////////////////////////////////////////////////////////////////////////
//! @}
