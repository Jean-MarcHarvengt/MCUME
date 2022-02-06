#ifdef TEENSYDUINO
#include <Arduino.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

#include "RingBuf.h"

#define NIBTRACKSIZE 0x1A00
// Minimum viable nibblized sector size. With GAP bytes, could be much longer.
#define MINNIBSECTORSIZE (343 + 13 + 3)

#define         nib1(a) (((a & 0xAA) >> 1) | 0xAA)
#define         nib2(b) (((b & 0x55)     ) | 0xAA)
#define denib(a, b) ((((a) & ~0xAA) << 1) | ((b) & ~0xAA))

#define GAP 0xFF

enum {
  dosDisk    = 0,
  prodosDisk = 1,
  nibDisk    = 2
};

enum nibErr {
  errorNone           = 0,
  errorShortTrack     = 1,
  errorMissingSectors = 2
};

void nibblizeTrack(RingBuf *trackBuffer, uint8_t *rawTrackBuffer,
		   uint8_t diskType, int8_t track);

nibErr denibblizeTrack(RingBuf *trackBuffer, uint8_t *rawTrackBuffer,
		       uint8_t diskType, int8_t track);


bool decodeData(RingBuf *trackBuffer, uint16_t startAt, uint8_t *output);
void encodeData(RingBuf *trackBuffer, uint8_t *data);

