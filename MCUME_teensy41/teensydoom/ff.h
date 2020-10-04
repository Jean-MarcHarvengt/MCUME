
#ifndef _FFWRAP
#define _FFWRAP

typedef int * FIL;


#define	FA_READ				0x01	/* Specifies read access to the object. Data can be read from the file. Combine with FA_WRITE for read-write access. */
#define	FA_OPEN_EXISTING	0x00	/* Opens the file. The function fails if the file is not existing. (Default) */

#define	FA_WRITE			0x02	/* Specifies write access to the object. Data can be written to the file. Combine with FA_READ for read-write access. */
#define	FA_CREATE_NEW		0x04	/* Creates a new file. The function fails with FR_EXIST if the file is existing. */
#define	FA_CREATE_ALWAYS	0x08	/* Creates a new file. If the file is existing, it will be truncated and overwritten. */
#define	FA_OPEN_ALWAYS		0x10	/* Opens the file if it is existing. If not, a new file is created. */
#define FA__WRITTEN			0x20
#define FA__DIRTY			0x40

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid */
} FRESULT;

typedef struct {
	signed long	fsize;			/* File size */
	signed short	fdate;			/* Last modified date */
	signed short	ftime;			/* Last modified time */
	signed char	fattrib;		/* Attribute */
	char *	fname[13];		/* Short file name (8.3 format) */
} FILINFO;


#ifdef __cplusplus  
extern "C" {
#endif
FRESULT f_open (FIL* fp, const char * path, unsigned char mode);
FRESULT f_close (FIL* fp);
FRESULT f_read (FIL* fp, void* buff, unsigned int  btr, unsigned int * br);
FRESULT f_readn (FIL* fp, void* buff, unsigned int btr, unsigned int * br);
FRESULT f_write (FIL* fp, const void* buff, unsigned int  btw, unsigned int * bw);
FRESULT f_writen (FIL* fp, const void* buff, unsigned int btw, unsigned int * bw);
FRESULT f_lseek (FIL* fp, unsigned long ofs);
FRESULT f_unlink (const char * path);
FRESULT f_rename (const char * path_old, const  char * path_new);
FRESULT f_stat (const char * path, FILINFO* fno);
FRESULT f_mkdir (const char* path);

unsigned long f_tell (FIL * fp);
unsigned long f_size (FIL * fp);

#ifdef __cplusplus  
} 
#endif

#endif
