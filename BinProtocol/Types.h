#ifndef _TYPES_H_
#define _TYPES_H_

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
