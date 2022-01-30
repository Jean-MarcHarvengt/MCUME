 /* 
  * UAE - The Un*x Amiga Emulator
  * 
  * disk support
  *
  * (c) 1995 Bernd Schmidt
  */

/*
extern void DISK_init (void);
extern void DISK_select (uae_u8 data);
extern uae_u8 DISK_status (void);
extern int DISK_GetData (uae_u16 *mfm,uae_u16 *byt);
extern void DISK_InitWrite (void);
extern void DISK_WriteData (int);
extern void disk_eject (int num);
extern int disk_empty (int num);
extern void disk_insert (int num, const char *name);
extern void disk_swap(const char * disk0, const char * disk1);
extern int DISK_ReadMFM (uaecptr);
extern int DISK_PrepareReadMFM (int, uae_u16, int);
extern void DISK_check_change (void);
*/
// JMH: extern uae_u16* mfmwrite;

extern void DISK_init (void);
extern void DISK_select (uae_u8 data);
extern uae_u8 DISK_status (void);
extern void DISK_GetData (uae_u16 *mfm, uae_u16 *byt);
extern void DISK_InitWrite (void);
extern void DISK_WriteData (int);
extern void disk_eject (int num);
extern int disk_empty (int num);
extern void disk_insert (int num, const char *name);
extern int DISK_ReadMFM (uaecptr);
extern int DISK_PrepareReadMFM (int, uae_u16, int);
extern void DISK_check_change (void);
extern FILE *DISK_validate_filename (const char *, int, int *);
extern void DISK_hsync_handler (void);
extern int DISK_update_reads (uaecptr *, int *, int *, int);
extern void DISK_reset_cycles (void);
extern void DISK_StartRead (void);
extern void DISK_search_sync (int, unsigned int);

extern void disk_swap(const char * disk0, const char * disk1);

extern uae_u16* mfmwrite;

