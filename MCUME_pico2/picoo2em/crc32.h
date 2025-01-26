#ifndef __CRC32_H
#define __CRC32_H

unsigned long crc32_buf(const void *buf, long len);
unsigned long crc32_file(const char *filename);

#endif
