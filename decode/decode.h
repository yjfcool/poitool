/*******************************************************************************
	版权所有(C) 2006 新科软件, 保留所有权利。
*******************************************************************************/

/*******************************************************************************
	文 件 名: Decode.h
	描    述: Decode格式文件
	创 建 者: 谈勇亮
	创建日期: 2006年4月20日
*******************************************************************************/
#ifndef _unDecode_H
#define _unDecode_H

#ifndef _Decode_H
DECLARE_HANDLE(HDECODE);
#endif

typedef DWORD ZRESULT;


typedef struct{ 
	int index;                 
	TCHAR name[MAX_PATH];     
	DWORD attr;  
	FILETIME atime,ctime,mtime;
	long comp_size; 
	long unc_size; 
} DECODEENTRY;


HDECODE OpenDecode(void *pSrcDataBuf, unsigned int srcDataSize, const char *password);
ZRESULT GetDecodeItem(HDECODE hz,  DECODEENTRY *ze, int index = 0);
ZRESULT DecodeItem(HDECODE hz, int index, void *pDesBlankBuf,unsigned int desDataSize);


ZRESULT CloseDecode(HDECODE hz);
unsigned int FormatDecodeMessage(ZRESULT code, TCHAR *buf,unsigned int len);

#define ZR_OK         0x00000000 
#define ZR_RECENT     0x00000001 

#define ZR_GENMASK    0x0000FF00
#define ZR_NODUPH     0x00000100    
#define ZR_NOFILE     0x00000200     
#define ZR_NOALLOC    0x00000300     
#define ZR_WRITE      0x00000400    
#define ZR_NOTFOUND   0x00000500    
#define ZR_MORE       0x00000600    
#define ZR_CORRUPT    0x00000700   
#define ZR_READ       0x00000800    
#define ZR_PASSWORD   0x00001000    

#define ZR_CALLERMASK 0x00FF0000
#define ZR_ARGS       0x00010000     
#define ZR_NOTMMAP    0x00020000    
#define ZR_MEMSIZE    0x00030000    
#define ZR_FAILED     0x00040000    
#define ZR_ENDED      0x00050000    
#define ZR_MISSIZE    0x00060000   
#define ZR_PARTIALUNZ 0x00070000   
#define ZR_ZMODE      0x00080000   

#define ZR_BUGMASK    0xFF000000
#define ZR_NOTINITED  0x01000000     
#define ZR_SEEK       0x02000000     
#define ZR_NOCHANGE   0x04000000  
#define ZR_FLATE      0x05000000   


ZRESULT CloseDecodeU(HDECODE hz);
unsigned int FormatDecodeMessageU(ZRESULT code, TCHAR *buf,unsigned int len);
bool IsDecodeHandleU(HDECODE hz);
#ifdef _Decode_H
#undef CloseDecode
#define CloseDecode(hz) (IsDecodeHandleU(hz)?CloseDecodeU(hz):CloseDecodeZ(hz))
#else
#define CloseDecode CloseDecodeU
#define FormatDecodeMessage FormatDecodeMessageU
#endif



#endif 
