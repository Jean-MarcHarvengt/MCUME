uint8_t translatescancode(uint32_t keysym) {
  if ((keysym >= 'a') && (keysym <= 'z')) {
    keysym -= 0x20;
  }
  switch (keysym) {
    case 0xFF08: return 0x0E; //backspace
    case 0xFF09: return 0x0F; //tab
    case 0xFF0D: return 0x1C; //enter
    case 0xFF1B: return 0x01; //escape
    case 0xFF63: return 0x52; //KP 0 / insert
    case 0xFFFF: return 0x53; //KP . / delete
    case 0xFF55: return 0x49; //pgup
    case 0xFF56: return 0x51; //pgdn
    case 0xFF50: return 0x47; //home
    case 0xFF57: return 0x4F; //end
    case 'A': return 0x1E;
    case 'B': return 0x30;
    case 'C': return 0x2E;
    case 'D': return 0x20;
    case 'E': return 0x12;
    case 'F': return 0x21;
    case 'G': return 0x22;
    case 'H': return 0x23;
    case 'I': return 0x17;
    case 'J': return 0x24;
    case 'K': return 0x25;
    case 'L': return 0x26;
    case 'M': return 0x32;
    case 'N': return 0x31;
    case 'O': return 0x18;
    case 'P': return 0x19;
    case 'Q': return 0x10;
    case 'R': return 0x13;
    case 'S': return 0x1F;
    case 'T': return 0x14;
    case 'U': return 0x16;
    case 'V': return 0x2F;
    case 'W': return 0x11;
    case 'X': return 0x2D;
    case 'Y': return 0x15;
    case 'Z': return 0x2C;
    case '0': case ')': return 0x0B;
    case '1': case '!': return 0x02;
    case '2': case '@': return 0x03;
    case '3': case '#': return 0x04;
    case '4': case '$': return 0x05;
    case '5': case '%': return 0x06;
    case '6': case '^': return 0x07;
    case '7': case '&': return 0x08;
    case '8': case '*': return 0x09;
    case '9': case '(': return 0x0A;
    case '`': case '~': return 0x29;
    case '-': case '_': return 0x0C;
    case '=': case '+': return 0x0D;
    case '[': case '{': return 0x1A;
    case ']': case '}': return 0x1B;
    case '\\': case '|': return 0x2B;
    case ';': case ':': return 0x27;
    case '\'': case '"': return 0x28;
    case ' ': return 0x39;
    case ',': case '<': return 0x33;
    case '.': case '>': return 0x34;
    case '/': case '?': return 0x35;
    case 0xFFBE: return 0x3B; //F1
    case 0xFFBF: return 0x3C; //F2
    case 0xFFC0: return 0x3D; //F3
    case 0xFFC1: return 0x3E; //F4
    case 0xFFC2: return 0x3F; //F5
    case 0xFFC3: return 0x40; //F6
    case 0xFFC4: return 0x41; //F7
    case 0xFFC5: return 0x42; //F8
    case 0xFFC6: return 0x43; //F9
    case 0xFFC7: return 0x44; //F10
    case 0xFFC8: return 0x57; //F11
    case 0xFFC9: return 0x58; //F12
    case 0xFFE1: return 0x2A; //left shift
    case 0xFFE2: return 0x36; //right shift
    case 0xFFE3: case 0xFFE4: return 0x1D; //control
    case 0xFFE9: case 0xFFEA: return 0x38; //alt
    case 0xFF51: return 0x4B; //left
    case 0xFF52: return 0x48; //up
    case 0xFF53: return 0x4D; //right
    case 0xFF54: return 0x50; //down
  }
  return 0xFF;
}



