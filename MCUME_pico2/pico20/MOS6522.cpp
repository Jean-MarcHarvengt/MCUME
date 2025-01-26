#include "MOS6522.h"
#include "MOS6502Memory.h"

MOS6522::MOS6522() : IC()
					, keyPressed(0)
					, shiftPressed(false)
					, cbmPressed(false) {
	// Set clock speed
	this->setClockSpeed(2000000);
}

MOS6522::~MOS6522() {

}


void MOS6522::setCpu(mos6502 *omos6502) {
	this->omos6502 = omos6502;
}

void MOS6522::setKeyPressed(uint16_t key) {
	/* Store high byte first 
		= 0x9120 << 8 | 0x9121
	*/
	keyPressed = key;
}

void MOS6522::setShiftPressed(bool state) {
	this->shiftPressed = state;
}

void MOS6522::setCbmPressed(bool state) {
	this->cbmPressed = state;
}

void MOS6522::setJoyStickPressed(Vic20JoyStickButton button, bool state) {
	this->joyStick[button] = state;
}

void MOS6522::initialize() {
	// Clear joystick memory
	writeWord(0x9111, 0xFF);
}

void MOS6522::tick() {
	// Tick timer cycle
	this->tickTimers();

	// Handle input in this cycle
	this->joy1Input();
	this->joy2Input();
	this->keyboardInput();

	// Increment cycles counter
	this->cycles += 1;
}


void MOS6522::joy1Input() {
	// Handle joystick input on VIA1
	uint8_t via1PortAValue = silentReadWord(via1PORTA);
    via1PortAValue |= 0x3c;
	if (joyStick[Vic20JoyStickButton::Fire]) {
		via1PortAValue &= ~0x20;
	}

	if (joyStick[Vic20JoyStickButton::Up]) {
		via1PortAValue &= ~0x04;
	}

	if (joyStick[Vic20JoyStickButton::Down]) {
		via1PortAValue &= ~0x08;
	}

	if (joyStick[Vic20JoyStickButton::Left]) {
		via1PortAValue &= ~0x10;
	}
	silentWriteWord(via1PORTA, via1PortAValue);
	silentWriteWord(via1PORTAMIRROR, via1PortAValue);
}

void MOS6522::joy2Input() {
	uint8_t via2PortBValue = silentReadWord(this->via2PORTB);	
	// If this bit is input, set it to default value, else leave it as it is so it won't mess up keyboard input
	if (!(silentReadWord(via2PortBDDR) & 0x80)) {
		via2PortBValue |= 0x80;
	}
	// Joystick RIGHT ison VIA 2 
	if (joyStick[Vic20JoyStickButton::Right]) {
		via2PortBValue &= ~0x80;
	}
	silentWriteWord(via2PORTB, via2PortBValue);
}

void MOS6522::keyboardInput() {
	uint8_t via2PortAValue = silentReadWord(this->via2PORTA);
	uint8_t via2PortBValue = silentReadWord(this->via2PORTB);	

	// Get row and column of key press
	uint8_t column = (keyPressed >> 8) & 0xFF; // output
	uint8_t row = keyPressed & 0xFF; //input

	// Check whether keyboard should be scanned
	if (via2PortBValue == 0) {
		// If port b is 0 then the vic is asking whether any keys are pressed on the keyboard
		via2PortAValue = keyPressed == 0 ? 0xFF : row;
	}
	else {
		// Return keyboard row depending on column state
		via2PortAValue = (via2PortBValue == column ? (row == 0 ? 0xFF : row) : 0xFF);
	}

	if ((via2PortBValue == 0xF7) && shiftPressed) {
		// Return row of shift key
		via2PortAValue = 0xFD & (keyPressed == 0 ? 0xFF : (column == 0xF7 ? row : 0xFF));
	} else if (via2PortBValue == 0xDF && cbmPressed) {
		// Return row of cbm key
		via2PortAValue = 0xFE & (keyPressed == 0 ? 0xFF : (column == 0xDF ? row : 0xFF));
	}

	silentWriteWord(this->via2PORTA, via2PortAValue);
	silentWriteWord(this->via2PORTAMIRROR, via2PortAValue);	
}

void MOS6522::tickTimers() {
	// Grab values of timer related registers
	uint8_t interruptEnable = readWord(this->irqEnableAddress);
	uint8_t interruptFlags = readWord(this->irqFlagsAddress);

	// Get timer values
	uint16_t timer1 = silentReadDWord(this->via2timer1DAddress);
	uint16_t timer2 = silentReadDWord(this->via1timer2DAddress);
	// If timer1 is active, decrement it
	if (timer1 > 0) {
		silentWriteDWord(this->via2timer1DAddress, --timer1);
	}
	if (timer1 == 0) {
		// Set interrupt flag
		interruptFlags |= 0x40;
		silentWriteWord(this->irqFlagsAddress, interruptFlags);
	}

	// If timer2 is active and no latch, decrement it
	if (silentReadWord(this->via2timer2HighByteLatch) != 0) {
		if (timer2 > 0) {
			silentWriteDWord(this->via1timer2DAddress, --timer2);
		}
		if (timer2 == 0) {
			// Reset high byte latch
			silentWriteDWord(this->via1timer2DAddress, silentReadDWord(this->via2timer2LowByteLatch));
			silentWriteDWord(this->via2timer2HighByteLatch, 0);
			// Set interrupt flag
			interruptFlags |= 0x20;
			silentWriteWord(this->irqFlagsAddress, interruptFlags);
		}
	}

	// Check if an interrupt request exists in the system
	if (interruptFlags & interruptEnable & 0x7F) {
		// Send an interrupt request to the cpu
		//if (omos6502->interrupt()) {
		if (omos6502->IRQ()) {
			uint8_t auxControl = readWord(this->auxControlAddress);
			// Post interrupt operation
			if (interruptFlags & 0x40) {
				// Determine what to do with timer based on auxiliary control register
				if (auxControl & 0x40) {
					// Continuous interrupt
					silentWriteDWord(this->via2timer1DAddress, silentReadDWord(this->via2timer1LowByteLatch));
				}
				// Reset interrupt flag
				interruptFlags  &= ~0x40;
				silentWriteWord(irqFlagsAddress, interruptFlags);
			}

			if (interruptFlags & 0x20) {
				// Reset interrupt flag
				interruptFlags  &= ~0x20;
				silentWriteWord(irqFlagsAddress, interruptFlags);
			}
		}
	}
}
