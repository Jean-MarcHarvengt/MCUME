#ifndef DISKAPI_H
#define DISKAPI_H

int disk_Size(char * filename);
int disk_Open(char * filename);
int disk_Read(unsigned char * buf, int size);
int disk_Seek(int seek);

#endif