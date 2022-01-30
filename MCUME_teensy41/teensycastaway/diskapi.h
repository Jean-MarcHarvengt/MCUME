#ifndef DISKAPI_H
#define DISKAPI_H

int disk_Size(char * filename);
int disk_Open(char * filename);
int disk_Read(char * buf, int size, int handler);
int disk_Seek(int seek, int handler);

#endif
