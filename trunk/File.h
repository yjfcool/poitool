// x_file.h: interface for the x_file class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(X_FILE_H_)
#define X_FILE_H_
#include "mem_block.h"
#include "file_exception.h"

enum FILE_MODE
{
	READ_ONLY,
	WRITE_ONLY,
	READ_WIRTE,
	CREATE
};
enum FILE_POS
{
	CURRENT,
	BEGIN,
	END
};


class CLFile
{
public:
	CLFile():m_hFile(NULL)
	{
	}
	virtual ~CLFile();
	BOOL hang(void){return m_hFile == NULL;}
	BOOL attach(char* file, FILE_MODE mode);
	BOOL seek(int off, FILE_POS begin);
	BOOL read(void * buf, size_t sz);
	BOOL write(void* buf, size_t sz);
	BOOL fsize(size_t& size);
	BOOL ftell(size_t& fp);
	const char* fname(unsigned int* slen);
protected:
	HANDLE m_hFile;
	b_char_mem m_bName;
};

#endif // !defined(AFX_X_FILE_H__C56414BB_311A_4B84_B6A7_40985942E864__INCLUDED_)

