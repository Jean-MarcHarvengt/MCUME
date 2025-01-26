#ifndef _H_MOS6522
#define _H_MOS6522

#include "IC.h"
#include "mos6502.h"

/*
Notes,
This single object will serve as emulation 
for both VIA chips in the VIC-20.

References,
http://vic-20.appspot.com/docs/viausage.txt
http://vic-20.appspot.com/docs/vickey.txt
*/
class MOS6522 : public IC {
public:
	MOS6522();
	~MOS6522();

	// Joystick 
	enum Vic20JoyStickButton { Fire = 0, Up = 1, Down = 2, Left = 3, Right = 4 };

	// Via joystick interface
	void setJoyStickPressed(Vic20JoyStickButton button, bool state);

	// Via 2 Keyboard interface
	void setKeyPressed(uint16_t key);
	void setShiftPressed(bool state);
	void setCbmPressed(bool state);

	// Interface
	void tick();
	// Organize operations into separate methods for cleaner code
	void tickTimers();
	void joy1Input();
	void joy2Input();
	void keyboardInput();	
	void setCpu(mos6502 *omos6502);
	void initialize();

private:

	// Cpu reference
	mos6502 *omos6502;

	// Via 1 & 2 Joystick
	bool joyStick[5];

	// Via 2 Keyboard 
	uint16_t keyPressed;
	bool shiftPressed;
	bool cbmPressed; 

	// VIA 1 Joystick addresses
	static const uint16_t via1PORTB = 0x9110;
	static const uint16_t via1PORTA = 0x9111;
	static const uint16_t via1PORTAMIRROR = 0x911F;

    // Port A
	static const uint8_t joyStickUpBit = 0x2;
	static const uint8_t joyStickDownBit = 0x4;
	static const uint8_t joyStickLeftBit = 0x8;
	static const uint8_t joyStickFireBit = 0x10;

	// VIA 1 ddrs
	static const uint16_t via1PortBDDR = 0x9112;
	static const uint16_t via1PortADDR = 0x9113;

	// VIA 1 Timer
	static const uint16_t via1timer2DAddress = 0x9118; 	//and 0x9119


	// Via 2 Keyboard addresses + joystick right
	static const uint16_t via2PORTB = 0x9120;
	static const uint16_t via2PORTA = 0x9121;
	static const uint16_t via2PORTAMIRROR = 0x912F;
    
    // Port B
	static const uint8_t joyStickRightBit = 0x80;

	// VIA 2 ddrs
	static const uint16_t via2PortBDDR = 0x9122; 
	static const uint16_t via2PortADDR = 0x9123;

	// VIA 2 Timer
	static const uint16_t via2timer1LowByteLatch = 0x9124;
	static const uint16_t via2timer1HighByteLatch = 0x9125;
	static const uint16_t via2timer1DAddress = 0x9126;	//and 0x9127
	static const uint16_t via2timer2LowByteLatch = 0x9128;
	static const uint16_t via2timer2HighByteLatch = 0x9129;



	// Via 2 Registers
	static const uint16_t auxControlAddress = 0x912B;
	static const uint16_t irqFlagsAddress = 0x912D;
	static const uint16_t irqEnableAddress = 0x912E;
};

#endif
