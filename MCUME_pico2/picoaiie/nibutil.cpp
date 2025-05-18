#include "nibutil.h"

#ifdef TEENSYDUINO
#include <Arduino.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#endif

// Long gaps are more "correct" in the sense that they're
// nib-disk-like; but they mean the VM has to chew on a lot of disk
// gaps to find the real data, which takes a noticeable amount of
// time. With this off, we present a minimum number of gaps (that
// hopefully aren't too short for the ROM to be able to write
// correctly)
//#define LONGGAPS

#define DISK_VOLUME 254

// dos 3.3 to physical sector conversion
const static uint8_t dephys[16] = { 
  0x00, 0x07, 0x0e, 0x06, 0x0d, 0x05, 0x0c, 0x04, 
  0x0b, 0x03, 0x0a, 0x02, 0x09, 0x01, 0x08, 0x0f };

// Prodos to physical sector conversion
const uint8_t deProdosPhys[] = { 
  0x00, 0x08, 0x01, 0x09, 0x02, 0x0a, 0x03, 0x0b,
  0x04, 0x0c, 0x05, 0x0d, 0x06, 0x0e, 0x07, 0x0f };

const static uint8_t _trans[64] = {0x96, 0x97, 0x9a, 0x9b, 0x9d, 0x9e, 0x9f, 0xa6,
				   0xa7, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb2, 0xb3,
				   0xb4, 0xb5, 0xb6, 0xb7, 0xb9, 0xba, 0xbb, 0xbc,
				   0xbd, 0xbe, 0xbf, 0xcb, 0xcd, 0xce, 0xcf, 0xd3,
				   0xd6, 0xd7, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde,
				   0xdf, 0xe5, 0xe6, 0xe7, 0xe9, 0xea, 0xeb, 0xec,
				   0xed, 0xee, 0xef, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6,
				   0xf7, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};

const static uint8_t _detrans[0x80] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 
				       0x00, 0x00, 0x08, 0x0C, 0x00, 0x10, 0x14, 0x18, 
				       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x20, 
				       0x00, 0x00, 0x00, 0x24, 0x28, 0x2C, 0x30, 0x34, 
				       0x00, 0x00, 0x38, 0x3C, 0x40, 0x44, 0x48, 0x4C, 
				       0x00, 0x50, 0x54, 0x58, 0x5C, 0x60, 0x64, 0x68, 
				       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				       0x00, 0x00, 0x00, 0x6C, 0x00, 0x70, 0x74, 0x78, 
				       0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x80, 0x84, 
				       0x00, 0x88, 0x8C, 0x90, 0x94, 0x98, 0x9C, 0xA0, 
				       0x00, 0x00, 0x00, 0x00, 0x00, 0xA4, 0xA8, 0xAC, 
				       0x00, 0xB0, 0xB4, 0xB8, 0xBC, 0xC0, 0xC4, 0xC8, 
				       0x00, 0x00, 0xCC, 0xD0, 0xD4, 0xD8, 0xDC, 0xE0, 
				       0x00, 0xE4, 0xE8, 0xEC, 0xF0, 0xF4, 0xF8, 0xFC };

void nibblizeTrack(RingBuf *trackBuffer, uint8_t *rawTrackBuffer,
                   uint8_t diskType, int8_t track)
{
  int checksum;

  for (uint8_t sector=0; sector<16; sector++) {

    for (uint8_t i=0; 
#ifdef LONGGAPS
	 i < (sector==0 ? 0x63 : 0x13);
#else
	 i < 8;
#endif
	 i++) {
      trackBuffer->addByte(GAP);
    }

    trackBuffer->addByte(0xD5); // prolog
    trackBuffer->addByte(0xAA);
    trackBuffer->addByte(0x96);
    
    trackBuffer->addByte(nib1(DISK_VOLUME));
    trackBuffer->addByte(nib2(DISK_VOLUME));
    
    trackBuffer->addByte(nib1(track));
    trackBuffer->addByte(nib2(track));
    
    trackBuffer->addByte(nib1(sector));
    trackBuffer->addByte(nib2(sector));
    
    checksum = DISK_VOLUME ^ track ^ sector;
    trackBuffer->addByte(nib1(checksum));
    trackBuffer->addByte(nib2(checksum));
    
    trackBuffer->addByte(0xDE); // epilog
    trackBuffer->addByte(0xAA);
    trackBuffer->addByte(0xEB); // Not strictly necessary, but the DiskII controller does it, so we will too.
    
    // The DiskII controller puts out 5 GAP bytes here.
    for (uint8_t i=0; i<5; i++) {
      trackBuffer->addByte(GAP);
    }
    
    trackBuffer->addByte(0xD5); // data prolog
    trackBuffer->addByte(0xAA);
    trackBuffer->addByte(0xAD);
    
    uint8_t physicalSector = (diskType == prodosDisk ? deProdosPhys[sector] : dephys[sector]);
    encodeData(trackBuffer, &rawTrackBuffer[physicalSector * 256]);
    
    trackBuffer->addByte(0xDE); // data epilog
    trackBuffer->addByte(0xAA);
    trackBuffer->addByte(0xEB);
    
    //  trackBuffer->addByte(GAP);
  }
}

#define SIXBIT_SPAN 0x56 // 86 bytes

// Pop the next 343 bytes off of trackBuffer, which should be 342
// 6:2-bit GCR encoded values, which we decode back in to 256 8-byte
// output values; and one checksum byte.
//
// Return true if we've successfully consumed 343 bytes from
// trackBuf. This reads from the circular buffer trackBuffer, so if
// there's not enough data there, the results are somewhat
// unpredictable.
bool decodeData(RingBuf *trackBuffer, uint16_t startAt, uint8_t *output)
{
  // Basic check that there's enough buffer data in trackBuffer. Note
  // that we're not checking it against startAt; we could be wrapping
  // around.
  if (trackBuffer->count() < 343)
    return false;

  static uint8_t workbuf[342];

  for (int i=0; i<342; i++) {
    uint8_t in = trackBuffer->peek(startAt++) & 0x7F; // strip high bit
    workbuf[i] = _detrans[in];
  }

  // fixme: collapse this in to the previous loop
  uint8_t prev = 0;
  for (int i=0; i<342; i++) {
    workbuf[i] = prev ^ workbuf[i];
    prev = workbuf[i];
  }

  // Put the checksum on the track - only necessary if we're about to
  // write the nibblized version of the track back out
  /*  uint16_t cursor = trackBuffer->Cursor();
  trackBuffer->setPeekCursor(startAt++);
  trackBuffer->replaceByte(prev); // 'prev' holds the checksum
  trackBuffer->setPeekCursor(cursor); // put it back where we found it
  */

  // Start with all of the bytes with 6 bits of data
  for (uint16_t i=0; i<256; i++) {
    output[i] = workbuf[SIXBIT_SPAN + i] & 0xFC; // 6 bits
  }

  // Then pull in all of the 2-bit values, which are stuffed 3 to a byte. That gives us
  // 4 bits more than we need - the last two skip two of the bits.
  for (uint8_t i=0; i<SIXBIT_SPAN; i++) {
    // This byte (workbuf[i]) has 2 bits for each of 3 output bytes:
    //     i, SIXBIT_SPAN+i, and 2*SIXBIT_SPAN+i
    uint8_t thisbyte = workbuf[i];
    output[                i] |= ((thisbyte & 0x08) >> 3) | ((thisbyte & 0x04) >> 1);
    output[  SIXBIT_SPAN + i] |= ((thisbyte & 0x20) >> 5) | ((thisbyte & 0x10) >> 3);
    if (i < SIXBIT_SPAN-2) {
      output[2*SIXBIT_SPAN + i] |= ((thisbyte & 0x80) >> 7) | ((thisbyte & 0x40) >> 5);
    }
  }
  // FIXME: check or update the checksum?

  return true;
}

void encodeData(RingBuf *trackBuffer, uint8_t *data)
{
  int16_t i;
  int ptr2 = 0;
  int ptr6 = 0x56;
  static int nibbles[0x156];

  for (i=0; i<0x156; i++) {
    nibbles[i] = 0;
  }

  int idx2 = 0x55;
  for (int idx6 = 0x101; idx6 >= 0; idx6--) {
    int val6 = data[idx6 & 0xFF];
    int val2 = nibbles[ptr2 + idx2];

    val2 = (val2 << 1) | (val6 & 1);
    val6 >>= 1;
    val2 = (val2 << 1) | (val6 & 1);
    val6 >>= 1;

    // There are 2 "extra" bytes of 2-bit data that we ignore here.
    if (ptr6 + idx6 < 0x156) {
      nibbles[ptr6 + idx6] = val6;
    }
    if (ptr2 + idx2 < 0x156) {
      nibbles[ptr2 + idx2] = val2;
    }

    if (--idx2 < 0) {
      idx2 = 0x55;
    }
  }

  int lastv = 0;
  for (int idx = 0; idx < 0x156; idx++) {
    int val = nibbles[idx];
    trackBuffer->addByte(_trans[lastv ^ val]);
    lastv = val;
  }
  trackBuffer->addByte(_trans[lastv]);
}

nibErr denibblizeTrack(RingBuf *trackBuffer, uint8_t *rawTrackBuffer,
		       uint8_t diskType, int8_t track)
{
  // We can't tell exactly what the length should be, b/c there might
  // be varying numbers of GAP bytes. But we can tell, generally, that
  // this is the minimum acceptable length that might hold all the
  // track data.
  if (trackBuffer->count() < 16*MINNIBSECTORSIZE) {
    return errorShortTrack;
  }

  // bitmask of the sectors that we've found while decoding. We should
  // find all 16.
  uint16_t sectorsUpdated = 0;

  // loop through the data twice, so we make sure we read anything 
  // that crosses the end/start boundary
  // FIXME: if this approach works, we probably want 1/16th extra, not 2*
  for (uint16_t i=0; i<2*trackBuffer->count();  ) {
    // Find the prolog
    while (trackBuffer->peek(i++) != 0xD5)
      ;
    if (trackBuffer->peek(i++) != 0xAA) {
      continue;
    }
    if (trackBuffer->peek(i++) != 0x96) {
      continue;
    }

    // And now we should be in the header section
    uint8_t volumeID = denib(trackBuffer->peek(i),
			     trackBuffer->peek(i+1));
    i += 2;
    uint8_t trackID = denib(trackBuffer->peek(i),
			    trackBuffer->peek(i+1));
    i += 2;
    uint8_t sectorNum = denib(trackBuffer->peek(i),
			      trackBuffer->peek(i+1));
    i += 2;
    uint8_t headerChecksum = denib(trackBuffer->peek(i),
				   trackBuffer->peek(i+1));
    i += 2;

    if (headerChecksum != (volumeID ^ trackID ^ sectorNum)) {
      continue;
    }

    // check for the epilog
    if (trackBuffer->peek(i++) != 0xDE) {
      continue;
    }
    if (trackBuffer->peek(i++) != 0xAA) {
      continue;
    }

    // Skip to the data prolog
    while (trackBuffer->peek(i++) != 0xD5)
      ;
    if (trackBuffer->peek(i++) != 0xAA) {
      continue;
    }
    if (trackBuffer->peek(i++) != 0xAD) {
      continue;
    }

    // Decode the data in to a temporary buffer: we don't want to overwrite 
    // something valid with partial data
    uint8_t output[256];
    if (!decodeData(trackBuffer, i, output)) {
      continue;
    }
    i += 343;

    // Check the data epilog
    if (trackBuffer->peek(i++) != 0xDE) {
      continue;
    }
    if (trackBuffer->peek(i++) != 0xAA) {
      continue;
    }
    if (trackBuffer->peek(i++) != 0xEB) {
      continue;
    }

    // We've got a whole block! Put it in the rawTrackBuffer and mark
    // the bit for it in sectorsUpdated.

    // FIXME: if trackID != curTrack, that's an error?

    uint8_t targetSector;
    if (diskType == prodosDisk) {
      targetSector = deProdosPhys[sectorNum];
    } else { 
      targetSector = dephys[sectorNum];
    } 

    memcpy(&rawTrackBuffer[targetSector * 256],
	   output,
	   256);
    sectorsUpdated |= (1 << sectorNum);
  }

  // Check that we found all of the sectors for this track
  if (sectorsUpdated != 0xFFFF) {
    return errorMissingSectors;
  }
  
  return errorNone;
}

