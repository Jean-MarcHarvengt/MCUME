
#include <stdint.h>
#include <string.h>

#include "emu.h"


#ifdef USE_ENC28J60
#include <enc28j60.h>

byte ENC28J60::buffer[1514];
//uint8_t net_mac[6] = { 0x90, 0xAD, 0xBE, 0xEF, 0x13, 0x37 };
uint8_t net_mac[6] = { 0x2C, 0xFD, 0x13, 0x37, 0x13, 0x37 };

extern union _bytewordregs_ regs;
extern uint16_t segregs[6];

struct netstruct {
  uint8_t enabled;
  uint8_t canrecv;
  uint16_t pktlen;
} net;

void net_handler() {
  uint32_t i;
  uint16_t j;
  //if (ethif==254) return; //networking not enabled
  switch (regs.byteregs[regah]) { //function number
      case 0x00: //enable packet reception
        net.enabled = 1;
        net.canrecv = 1;
        return;
      case 0x01: //send packet of CX at DS:SI
        //if (verbose) {
            //Serial.println("Sending packet of %u bytes.", regs.wordregs[regcx]);
          //}
        //sendpkt (&RAM[ ( (uint32_t) segregs[regds] << 4) + (uint32_t) regs.wordregs[regsi]], regs.wordregs[regcx]);
        i = ( (uint32_t) segregs[regds] << 4) + (uint32_t) regs.wordregs[regsi];
        for (j=0; j<net.pktlen; j++) {
          ENC28J60::buffer[j] = read86(i++);
        }
        SPI.setClockDivider(SPI_CLOCK_ENC28J60);
        ENC28J60::packetSend(segregs[regcx]);
        SPI.setClockDivider(SPI_CLOCK_SPIRAM);
        return;
      case 0x02: //return packet info (packet buffer in DS:SI, length in CX)
        segregs[regds] = 0xD000;
        regs.wordregs[regsi] = 0x0000;
        regs.wordregs[regcx] = net.pktlen;
        return;
      case 0x03: //copy packet to final destination (given in ES:DI)
        //memcpy (&RAM[ ( (uint32_t) segregs[reges] << 4) + (uint32_t) regs.wordregs[regdi]], &RAM[0xD0000], net.pktlen);
        i = ( (uint32_t) segregs[reges] << 4) + (uint32_t) regs.wordregs[regdi];
        for (j=0; j<net.pktlen; j++) {
          write86(i++, ENC28J60::buffer[j]);
        }
        net.canrecv = 1;
        net.pktlen = 0;
        return;
      case 0x04: //disable packets
        net.enabled = 0;
        net.canrecv = 0;
        return;
      case 0x05: //DEBUG: dump packet (DS:SI) of CX bytes to stdout
        /*for (i=0; i<regs.wordregs[regcx]; i++) {
            printf ("%c", RAM[ ( (uint32_t) segregs[regds] << 4) + (uint32_t) regs.wordregs[regsi] + i]);
          }*/
        return;
      case 0x06: //DEBUG: print milestone string
        //print("PACKET DRIVER MILESTONE REACHED\n");
        return;
    }
}

uint8_t net_read_ram(uint32_t addr32) {
  if (addr32 < 1514) return ENC28J60::buffer[addr32];
  return 0;
}

/*void net_write_ram(uint32_t addr32, uint8_t value) {
  if (addr32 < 1514) ENC28J60::buffer[addr32] = value;
}*/

void net_loop() {
  uint16_t i, len;
  uint8_t cc;
  if (!net.enabled || !net.canrecv) return;
  SPI.setClockDivider(SPI_CLOCK_ENC28J60);
  len = ENC28J60::packetReceive();
  SPI.setClockDivider(SPI_CLOCK_SPIRAM);
  if (len > 0) {
    for (i=0; i<len; i++) {
      Serial.print(ENC28J60::buffer[i], HEX);
      Serial.write(' ');
    }
    Serial.println("");
    Serial.println("");
    net.canrecv = 0;
    net.pktlen = len;
    doirq(6);
  }
}

void net_init() {
  uint8_t ret;
  Serial.println("enter net_init");
  //SPI.setClockDivider(SPI_CLOCK_ENC28J60);
  ret = ENC28J60::initialize(1514, net_mac, NET_PIN);
  //SPI.setClockDivider(SPI_CLOCK_LCD);
  Serial.print("net init result = ");
  Serial.println(ret);
  ENC28J60::enablePromiscuous();
  //ENC28J60::broadcast_enabled = true;
  while(1) {
    uint16_t len, i;
    //delay(100);
    //SPI.setClockDivider(SPI_CLOCK_ENC28J60);
    len = ENC28J60::packetReceive();
    //SPI.setClockDivider(SPI_CLOCK_LCD);
    if (len > 0) {
      for (i=0; i<len; i++) {
        Serial.print(ENC28J60::buffer[i], HEX);
        Serial.write(' ');
      }
      Serial.println("");
      Serial.println("");
    }
  }
}
#else
uint8_t net_mac[6] = { 0x2C, 0xFD, 0x13, 0x37, 0x13, 0x37 };


void net_init() {
}  

void net_loop() {
}

void net_handler() {
}
uint8_t net_read_ram(uint32_t addr32) {
  return 0;
}
#endif

