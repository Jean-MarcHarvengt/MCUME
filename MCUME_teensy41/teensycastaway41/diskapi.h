#ifndef DISKAPI_H
#define DISKAPI_H

int disk_Size(char * filename);
int disk_Open(char * filename, int disk);
int disk_Read(char * buf, int size, int disk);
int disk_Seek(int seek, int disk);

#endif
