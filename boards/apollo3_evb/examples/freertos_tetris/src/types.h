///////////////////////////////////////////////////////////////////////////////
//! \addtogroup common
//! @{
//
//! \file types.h
//! \brief Contains standard data types.
///////////////////////////////////////////////////////////////////////////////


#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
//! \brief TBD
//! \todo [PUBS] Add definition(s)...
//! \todo Where does this really go?

#ifndef uint128_t
typedef struct _uint128_t_
{
    //! \brief TBD
    uint32_t val[4];
} uint128_t;
#endif
//! \brief TBD
#ifndef RETCODE
#define RETCODE int32_t
#endif

//------------------------------------------------------------------------------
// All of the following defs are included for compatibility.  Please use the
// ANSI C99 defs for all new work.
//------------------------------------------------------------------------------

//! \brief TBD
#if !defined(FALSE)
#define FALSE false
#endif

//! \brief TBD
#if !defined(TRUE)
#define TRUE  true
#endif

//! \brief TBD
#if !defined(NULL)
#define NULL 0
#endif

//! \brief TBD
typedef bool        BOOL;

//! \brief TBD
typedef uint8_t     UINT8;
//! \brief TBD
typedef uint8_t     BYTE;
//! \brief TBD
typedef uint8_t     PACKED_BYTE;

//! \brief TBD
typedef uint16_t    UINT16;
//! \brief TBD
typedef uint16_t    USHORT;
//! \brief TBD
typedef uint16_t    WCHAR;
//! \brief TBD
typedef uint16_t    UCS3;
//! \brief TBD
typedef int16_t     SHORT;

//! \brief TBD
typedef uint32_t    UINT32;
//! \brief TBD
typedef uint32_t    WORD;
//! \brief TBD
typedef uint32_t    SECTOR_BUFFER;
//! \brief TBD
typedef uint32_t *  P_SECTOR_BUFFER;

//! \brief TBD
typedef uint64_t    DWORD;
//! \brief TBD
typedef int64_t     INT64;
//! \brief TBD
typedef int64_t     UINT64;

//! \brief TBD
typedef uint128_t   UINT128;

//! \brief TBD
typedef float       FLOAT;

//------------------------------------------------------------------------------
// Huh?
//------------------------------------------------------------------------------

//! \brief TBD
#define REENTRANT

//------------------------------------------------------------------------------
// Debug macros in types.h?
//------------------------------------------------------------------------------

//! \brief TBD
#ifdef DEBUG
#define IFDEBUG(x) (x)
#else
#define IFDEBUG(x)
#endif


//! \brief TBD
#ifdef _WIN32
#define inline __inline
#endif

#endif // #ifndef _TYPES_H

///////////////////////////////////////////////////////////////////////////////
// End of file
///////////////////////////////////////////////////////////////////////////////
//! @}
