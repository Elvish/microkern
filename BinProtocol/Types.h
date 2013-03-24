#ifndef _TYPES_H_
#define _TYPES_H_

/*******************************************
 *
** Copyright (C) Kern Elvish.
** All rights reserved.
** Contact: Kern Elvish (KernElvish@yandex.ru)
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Kern Elvish.
**
**
**
 ******************************************/


#ifdef __COMPILER_FCC907__
#define FUJITSU_CPU_16
#endif
#ifdef __COMPILER_FCC911__
#define FUJITSU_CPU_16 
#endif
#ifdef __COMPILER_FCC896__	
#define FUJITSU_CPU_16 
#endif


#ifdef __BORLANDC__
  //#ifdef FUJITSU_CPU_16
  //#define int ERROR
  //#endif
#else
  #define __int64 long long
  #define __int32 long
  #define __int16 short
  #define __int8  char
#endif

#ifndef UINT_TYPES
  #define UINT_TYPES
  typedef unsigned __int8  	uint8_t;	// unsigned char
  typedef unsigned __int16 	uint16_t;	// unsigned short
  typedef unsigned __int32 	uint32_t;	// unsigned long
  typedef __int8 		 int8_t;	// char
  typedef __int16		 int16_t;	// short
  typedef __int32		 int32_t;	// long
  #ifndef FUJITSU_CPU_16
  typedef __int64		 int64_t;	// long long
  typedef unsigned __int64	uint64_t;	// unsigned long long
  #endif
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef time_t
#ifndef _TIME
	#ifdef __cplusplus
	  namespace std {
	#endif // __cplusplus 
	#ifndef  _TIME_T
		typedef
			#ifndef __BORLANDC__
			unsigned
			#endif
				long time_t;
		#ifdef __cplusplus
		#  define _TIME_T std::time_t
		#else
		#  define _TIME_T time_t
		#endif // __cplusplus 
	#endif
	#ifdef __cplusplus
	  }
	  using namespace std;
	#endif
#endif
#endif



#ifndef __cplusplus
	typedef enum {false=0,true} bool;
#endif

#ifndef BIGTYPES
#define BIGTYPES
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
#endif
#ifndef BIGBOOL
//typedef unsigned int   BOOL;
//#ifndef __BORLANDC__
#ifndef __wtypes_h__
typedef enum { FALSE = 0, TRUE } BOOL;
#endif
//#define BOOL 034
//typedef int BOOL
//const in
//#endif

#endif


#ifndef DISABLE_UINT_TYPES
	typedef unsigned __int32	UINT;
	typedef uint8_t		UCHAR;
	//#ifndef __BORLANDC__
	#ifndef __wtypes_h__
	typedef uint32_t	ULONG;
	#endif
	//#endif
#endif

#if ((int)-1) != ((long)-1)
#error BAD INT!!!
#endif


#endif
