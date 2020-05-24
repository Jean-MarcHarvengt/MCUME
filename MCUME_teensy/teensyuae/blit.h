static __inline__ ULONG blit_func(ULONG srca, ULONG srcb, ULONG srcc, UBYTE mt)
{
switch(mt){
case 0x0:
	return 0;
case 0x1:
	return (~srcc & ~srcb & ~srca);
case 0x2:
	return (srcc & ~srcb & ~srca);
case 0x3:
	return (~srcb & ~srca);
case 0x4:
	return (~srcc & srcb & ~srca);
case 0x5:
	return (~srcc & ~srca);
case 0x6:
	return (srcc & ~srcb & ~srca) | (~srcc & srcb & ~srca);
case 0x7:
	return (~srcb & ~srca) | (~srcc & ~srca);
case 0x8:
	return (srcc & srcb & ~srca);
case 0x9:
	return (~srcc & ~srcb & ~srca) | (srcc & srcb & ~srca);
case 0xa:
	return (srcc & ~srca);
case 0xb:
	return (~srcb & ~srca) | (srcc & ~srca);
case 0xc:
	return (srcb & ~srca);
case 0xd:
	return (~srcc & ~srca) | (srcb & ~srca);
case 0xe:
	return (srcc & ~srca) | (srcb & ~srca);
case 0xf:
	return (~srca);
case 0x10:
	return (~srcc & ~srcb & srca);
case 0x11:
	return (~srcc & ~srcb);
case 0x12:
	return (srcc & ~srcb & ~srca) | (~srcc & ~srcb & srca);
case 0x13:
	return (~srcb & ~srca) | (~srcc & ~srcb);
case 0x14:
	return (~srcc & srcb & ~srca) | (~srcc & ~srcb & srca);
case 0x15:
	return (~srcc & ~srca) | (~srcc & ~srcb);
case 0x16:
	return (srcc & ~srcb & ~srca) | (~srcc & srcb & ~srca) | (~srcc & ~srcb & srca);
case 0x17:
	return (~srcb & ~srca) | (~srcc & ~srca) | (~srcc & ~srcb);
case 0x18:
	return (srcc & srcb & ~srca) | (~srcc & ~srcb & srca);
case 0x19:
	return (~srcc & ~srcb) | (srcc & srcb & ~srca);
case 0x1a:
	return (srcc & ~srca) | (~srcc & ~srcb & srca);
case 0x1b:
	return (~srcb & ~srca) | (srcc & ~srca) | (~srcc & ~srcb);
case 0x1c:
	return (srcb & ~srca) | (~srcc & ~srcb & srca);
case 0x1d:
	return (~srcc & ~srca) | (srcb & ~srca) | (~srcc & ~srcb);
case 0x1e:
	return (srcc & ~srca) | (srcb & ~srca) | (~srcc & ~srcb & srca);
case 0x1f:
	return (~srca) | (~srcc & ~srcb);
case 0x20:
	return (srcc & ~srcb & srca);
case 0x21:
	return (~srcc & ~srcb & ~srca) | (srcc & ~srcb & srca);
case 0x22:
	return (srcc & ~srcb);
case 0x23:
	return (~srcb & ~srca) | (srcc & ~srcb);
case 0x24:
	return (~srcc & srcb & ~srca) | (srcc & ~srcb & srca);
case 0x25:
	return (~srcc & ~srca) | (srcc & ~srcb & srca);
case 0x26:
	return (srcc & ~srcb) | (~srcc & srcb & ~srca);
case 0x27:
	return (~srcb & ~srca) | (~srcc & ~srca) | (srcc & ~srcb);
case 0x28:
	return (srcc & srcb & ~srca) | (srcc & ~srcb & srca);
case 0x29:
	return (~srcc & ~srcb & ~srca) | (srcc & srcb & ~srca) | (srcc & ~srcb & srca);
case 0x2a:
	return (srcc & ~srca) | (srcc & ~srcb);
case 0x2b:
	return (~srcb & ~srca) | (srcc & ~srca) | (srcc & ~srcb);
case 0x2c:
	return (srcb & ~srca) | (srcc & ~srcb & srca);
case 0x2d:
	return (~srcc & ~srca) | (srcb & ~srca) | (srcc & ~srcb & srca);
case 0x2e:
	return (srcc & ~srca) | (srcb & ~srca) | (srcc & ~srcb);
case 0x2f:
	return (~srca) | (srcc & ~srcb);
case 0x30:
	return (~srcb & srca);
case 0x31:
	return (~srcc & ~srcb) | (~srcb & srca);
case 0x32:
	return (srcc & ~srcb) | (~srcb & srca);
case 0x33:
	return (~srcb);
case 0x34:
	return (~srcc & srcb & ~srca) | (~srcb & srca);
case 0x35:
	return (~srcc & ~srca) | (~srcb & srca);
case 0x36:
	return (srcc & ~srcb) | (~srcc & srcb & ~srca) | (~srcb & srca);
case 0x37:
	return (~srcb) | (~srcc & ~srca);
case 0x38:
	return (srcc & srcb & ~srca) | (~srcb & srca);
case 0x39:
	return (~srcc & ~srcb) | (srcc & srcb & ~srca) | (~srcb & srca);
case 0x3a:
	return (srcc & ~srca) | (~srcb & srca);
case 0x3b:
	return (~srcb) | (srcc & ~srca);
case 0x3c:
	return (srcb & ~srca) | (~srcb & srca);
case 0x3d:
	return (~srcc & ~srca) | (srcb & ~srca) | (~srcb & srca);
case 0x3e:
	return (srcc & ~srca) | (srcb & ~srca) | (~srcb & srca);
case 0x3f:
	return (~srca) | (~srcb);
case 0x40:
	return (~srcc & srcb & srca);
case 0x41:
	return (~srcc & ~srcb & ~srca) | (~srcc & srcb & srca);
case 0x42:
	return (srcc & ~srcb & ~srca) | (~srcc & srcb & srca);
case 0x43:
	return (~srcb & ~srca) | (~srcc & srcb & srca);
case 0x44:
	return (~srcc & srcb);
case 0x45:
	return (~srcc & ~srca) | (~srcc & srcb);
case 0x46:
	return (srcc & ~srcb & ~srca) | (~srcc & srcb);
case 0x47:
	return (~srcb & ~srca) | (~srcc & ~srca) | (~srcc & srcb);
case 0x48:
	return (srcc & srcb & ~srca) | (~srcc & srcb & srca);
case 0x49:
	return (~srcc & ~srcb & ~srca) | (srcc & srcb & ~srca) | (~srcc & srcb & srca);
case 0x4a:
	return (srcc & ~srca) | (~srcc & srcb & srca);
case 0x4b:
	return (~srcb & ~srca) | (srcc & ~srca) | (~srcc & srcb & srca);
case 0x4c:
	return (srcb & ~srca) | (~srcc & srcb);
case 0x4d:
	return (~srcc & ~srca) | (srcb & ~srca) | (~srcc & srcb);
case 0x4e:
	return (srcc & ~srca) | (srcb & ~srca) | (~srcc & srcb);
case 0x4f:
	return (~srca) | (~srcc & srcb);
case 0x50:
	return (~srcc & srca);
case 0x51:
	return (~srcc & ~srcb) | (~srcc & srca);
case 0x52:
	return (srcc & ~srcb & ~srca) | (~srcc & srca);
case 0x53:
	return (~srcb & ~srca) | (~srcc & srca);
case 0x54:
	return (~srcc & srcb) | (~srcc & srca);
case 0x55:
	return (~srcc);
case 0x56:
	return (srcc & ~srcb & ~srca) | (~srcc & srcb) | (~srcc & srca);
case 0x57:
	return (~srcb & ~srca) | (~srcc);
case 0x58:
	return (srcc & srcb & ~srca) | (~srcc & srca);
case 0x59:
	return (~srcc & ~srcb) | (srcc & srcb & ~srca) | (~srcc & srca);
case 0x5a:
	return (srcc & ~srca) | (~srcc & srca);
case 0x5b:
	return (~srcb & ~srca) | (srcc & ~srca) | (~srcc & srca);
case 0x5c:
	return (srcb & ~srca) | (~srcc & srca);
case 0x5d:
	return (~srcc) | (srcb & ~srca);
case 0x5e:
	return (srcc & ~srca) | (srcb & ~srca) | (~srcc & srca);
case 0x5f:
	return (~srca) | (~srcc);
case 0x60:
	return (srcc & ~srcb & srca) | (~srcc & srcb & srca);
case 0x61:
	return (~srcc & ~srcb & ~srca) | (srcc & ~srcb & srca) | (~srcc & srcb & srca);
case 0x62:
	return (srcc & ~srcb) | (~srcc & srcb & srca);
case 0x63:
	return (~srcb & ~srca) | (srcc & ~srcb) | (~srcc & srcb & srca);
case 0x64:
	return (~srcc & srcb) | (srcc & ~srcb & srca);
case 0x65:
	return (~srcc & ~srca) | (srcc & ~srcb & srca) | (~srcc & srcb);
case 0x66:
	return (srcc & ~srcb) | (~srcc & srcb);
case 0x67:
	return (~srcb & ~srca) | (~srcc & ~srca) | (srcc & ~srcb) | (~srcc & srcb);
case 0x68:
	return (srcc & srcb & ~srca) | (srcc & ~srcb & srca) | (~srcc & srcb & srca);
case 0x69:
	return (~srcc & ~srcb & ~srca) | (srcc & srcb & ~srca) | (srcc & ~srcb & srca) | (~srcc & srcb & srca);
case 0x6a:
	return (srcc & ~srca) | (srcc & ~srcb) | (~srcc & srcb & srca);
case 0x6b:
	return (~srcb & ~srca) | (srcc & ~srca) | (srcc & ~srcb) | (~srcc & srcb & srca);
case 0x6c:
	return (srcb & ~srca) | (srcc & ~srcb & srca) | (~srcc & srcb);
case 0x6d:
	return (~srcc & ~srca) | (srcb & ~srca) | (srcc & ~srcb & srca) | (~srcc & srcb);
case 0x6e:
	return (srcc & ~srca) | (srcb & ~srca) | (srcc & ~srcb) | (~srcc & srcb);
case 0x6f:
	return (~srca) | (srcc & ~srcb) | (~srcc & srcb);
case 0x70:
	return (~srcb & srca) | (~srcc & srca);
case 0x71:
	return (~srcc & ~srcb) | (~srcb & srca) | (~srcc & srca);
case 0x72:
	return (srcc & ~srcb) | (~srcb & srca) | (~srcc & srca);
case 0x73:
	return (~srcb) | (~srcc & srca);
case 0x74:
	return (~srcc & srcb) | (~srcb & srca);
case 0x75:
	return (~srcc) | (~srcb & srca);
case 0x76:
	return (srcc & ~srcb) | (~srcc & srcb) | (~srcb & srca);
case 0x77:
	return (~srcb) | (~srcc);
case 0x78:
	return (srcc & srcb & ~srca) | (~srcb & srca) | (~srcc & srca);
case 0x79:
	return (~srcc & ~srcb) | (srcc & srcb & ~srca) | (~srcb & srca) | (~srcc & srca);
case 0x7a:
	return (srcc & ~srca) | (~srcb & srca) | (~srcc & srca);
case 0x7b:
	return (~srcb) | (srcc & ~srca) | (~srcc & srca);
case 0x7c:
	return (srcb & ~srca) | (~srcb & srca) | (~srcc & srca);
case 0x7d:
	return (~srcc) | (srcb & ~srca) | (~srcb & srca);
case 0x7e:
	return (srcc & ~srca) | (srcb & ~srca) | (~srcb & srca) | (~srcc & srca);
case 0x7f:
	return (~srca) | (~srcb) | (~srcc);
case 0x80:
	return (srcc & srcb & srca);
case 0x81:
	return (~srcc & ~srcb & ~srca) | (srcc & srcb & srca);
case 0x82:
	return (srcc & ~srcb & ~srca) | (srcc & srcb & srca);
case 0x83:
	return (~srcb & ~srca) | (srcc & srcb & srca);
case 0x84:
	return (~srcc & srcb & ~srca) | (srcc & srcb & srca);
case 0x85:
	return (~srcc & ~srca) | (srcc & srcb & srca);
case 0x86:
	return (srcc & ~srcb & ~srca) | (~srcc & srcb & ~srca) | (srcc & srcb & srca);
case 0x87:
	return (~srcb & ~srca) | (~srcc & ~srca) | (srcc & srcb & srca);
case 0x88:
	return (srcc & srcb);
case 0x89:
	return (~srcc & ~srcb & ~srca) | (srcc & srcb);
case 0x8a:
	return (srcc & ~srca) | (srcc & srcb);
case 0x8b:
	return (~srcb & ~srca) | (srcc & ~srca) | (srcc & srcb);
case 0x8c:
	return (srcb & ~srca) | (srcc & srcb);
case 0x8d:
	return (~srcc & ~srca) | (srcb & ~srca) | (srcc & srcb);
case 0x8e:
	return (srcc & ~srca) | (srcb & ~srca) | (srcc & srcb);
case 0x8f:
	return (~srca) | (srcc & srcb);
case 0x90:
	return (~srcc & ~srcb & srca) | (srcc & srcb & srca);
case 0x91:
	return (~srcc & ~srcb) | (srcc & srcb & srca);
case 0x92:
	return (srcc & ~srcb & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb & srca);
case 0x93:
	return (~srcb & ~srca) | (~srcc & ~srcb) | (srcc & srcb & srca);
case 0x94:
	return (~srcc & srcb & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb & srca);
case 0x95:
	return (~srcc & ~srca) | (~srcc & ~srcb) | (srcc & srcb & srca);
case 0x96:
	return (srcc & ~srcb & ~srca) | (~srcc & srcb & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb & srca);
case 0x97:
	return (~srcb & ~srca) | (~srcc & ~srca) | (~srcc & ~srcb) | (srcc & srcb & srca);
case 0x98:
	return (srcc & srcb) | (~srcc & ~srcb & srca);
case 0x99:
	return (~srcc & ~srcb) | (srcc & srcb);
case 0x9a:
	return (srcc & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb);
case 0x9b:
	return (~srcb & ~srca) | (srcc & ~srca) | (~srcc & ~srcb) | (srcc & srcb);
case 0x9c:
	return (srcb & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb);
case 0x9d:
	return (~srcc & ~srca) | (srcb & ~srca) | (~srcc & ~srcb) | (srcc & srcb);
case 0x9e:
	return (srcc & ~srca) | (srcb & ~srca) | (~srcc & ~srcb & srca) | (srcc & srcb);
case 0x9f:
	return (~srca) | (~srcc & ~srcb) | (srcc & srcb);
case 0xa0:
	return (srcc & srca);
case 0xa1:
	return (~srcc & ~srcb & ~srca) | (srcc & srca);
case 0xa2:
	return (srcc & ~srcb) | (srcc & srca);
case 0xa3:
	return (~srcb & ~srca) | (srcc & srca);
case 0xa4:
	return (~srcc & srcb & ~srca) | (srcc & srca);
case 0xa5:
	return (~srcc & ~srca) | (srcc & srca);
case 0xa6:
	return (srcc & ~srcb) | (~srcc & srcb & ~srca) | (srcc & srca);
case 0xa7:
	return (~srcb & ~srca) | (~srcc & ~srca) | (srcc & srca);
case 0xa8:
	return (srcc & srcb) | (srcc & srca);
case 0xa9:
	return (~srcc & ~srcb & ~srca) | (srcc & srcb) | (srcc & srca);
case 0xaa:
	return (srcc);
case 0xab:
	return (~srcb & ~srca) | (srcc);
case 0xac:
	return (srcb & ~srca) | (srcc & srca);
case 0xad:
	return (~srcc & ~srca) | (srcb & ~srca) | (srcc & srca);
case 0xae:
	return (srcc) | (srcb & ~srca);
case 0xaf:
	return (~srca) | (srcc);
case 0xb0:
	return (~srcb & srca) | (srcc & srca);
case 0xb1:
	return (~srcc & ~srcb) | (~srcb & srca) | (srcc & srca);
case 0xb2:
	return (srcc & ~srcb) | (~srcb & srca) | (srcc & srca);
case 0xb3:
	return (~srcb) | (srcc & srca);
case 0xb4:
	return (~srcc & srcb & ~srca) | (~srcb & srca) | (srcc & srca);
case 0xb5:
	return (~srcc & ~srca) | (~srcb & srca) | (srcc & srca);
case 0xb6:
	return (srcc & ~srcb) | (~srcc & srcb & ~srca) | (~srcb & srca) | (srcc & srca);
case 0xb7:
	return (~srcb) | (~srcc & ~srca) | (srcc & srca);
case 0xb8:
	return (srcc & srcb) | (~srcb & srca);
case 0xb9:
	return (~srcc & ~srcb) | (srcc & srcb) | (~srcb & srca);
case 0xba:
	return (srcc) | (~srcb & srca);
case 0xbb:
	return (~srcb) | (srcc);
case 0xbc:
	return (srcb & ~srca) | (~srcb & srca) | (srcc & srca);
case 0xbd:
	return (~srcc & ~srca) | (srcb & ~srca) | (~srcb & srca) | (srcc & srca);
case 0xbe:
	return (srcc) | (srcb & ~srca) | (~srcb & srca);
case 0xbf:
	return (~srca) | (~srcb) | (srcc);
case 0xc0:
	return (srcb & srca);
case 0xc1:
	return (~srcc & ~srcb & ~srca) | (srcb & srca);
case 0xc2:
	return (srcc & ~srcb & ~srca) | (srcb & srca);
case 0xc3:
	return (~srcb & ~srca) | (srcb & srca);
case 0xc4:
	return (~srcc & srcb) | (srcb & srca);
case 0xc5:
	return (~srcc & ~srca) | (srcb & srca);
case 0xc6:
	return (srcc & ~srcb & ~srca) | (~srcc & srcb) | (srcb & srca);
case 0xc7:
	return (~srcb & ~srca) | (~srcc & ~srca) | (srcb & srca);
case 0xc8:
	return (srcc & srcb) | (srcb & srca);
case 0xc9:
	return (~srcc & ~srcb & ~srca) | (srcc & srcb) | (srcb & srca);
case 0xca:
	return (srcc & ~srca) | (srcb & srca);
case 0xcb:
	return (~srcb & ~srca) | (srcc & ~srca) | (srcb & srca);
case 0xcc:
	return (srcb);
case 0xcd:
	return (~srcc & ~srca) | (srcb);
case 0xce:
	return (srcc & ~srca) | (srcb);
case 0xcf:
	return (~srca) | (srcb);
case 0xd0:
	return (~srcc & srca) | (srcb & srca);
case 0xd1:
	return (~srcc & ~srcb) | (srcb & srca);
case 0xd2:
	return (srcc & ~srcb & ~srca) | (~srcc & srca) | (srcb & srca);
case 0xd3:
	return (~srcb & ~srca) | (~srcc & srca) | (srcb & srca);
case 0xd4:
	return (~srcc & srcb) | (~srcc & srca) | (srcb & srca);
case 0xd5:
	return (~srcc) | (srcb & srca);
case 0xd6:
	return (srcc & ~srcb & ~srca) | (~srcc & srcb) | (~srcc & srca) | (srcb & srca);
case 0xd7:
	return (~srcb & ~srca) | (~srcc) | (srcb & srca);
case 0xd8:
	return (srcc & srcb) | (~srcc & srca);
case 0xd9:
	return (~srcc & ~srcb) | (srcc & srcb) | (srcb & srca);
case 0xda:
	return (srcc & ~srca) | (~srcc & srca) | (srcb & srca);
case 0xdb:
	return (~srcb & ~srca) | (srcc & ~srca) | (~srcc & srca) | (srcb & srca);
case 0xdc:
	return (srcb) | (~srcc & srca);
case 0xdd:
	return (~srcc) | (srcb);
case 0xde:
	return (srcc & ~srca) | (srcb) | (~srcc & srca);
case 0xdf:
	return (~srca) | (~srcc) | (srcb);
case 0xe0:
	return (srcc & srca) | (srcb & srca);
case 0xe1:
	return (~srcc & ~srcb & ~srca) | (srcc & srca) | (srcb & srca);
case 0xe2:
	return (srcc & ~srcb) | (srcb & srca);
case 0xe3:
	return (~srcb & ~srca) | (srcc & srca) | (srcb & srca);
case 0xe4:
	return (~srcc & srcb) | (srcc & srca);
case 0xe5:
	return (~srcc & ~srca) | (srcc & srca) | (srcb & srca);
case 0xe6:
	return (srcc & ~srcb) | (~srcc & srcb) | (srcb & srca);
case 0xe7:
	return (~srcb & ~srca) | (~srcc & ~srca) | (srcc & srca) | (srcb & srca);
case 0xe8:
	return (srcc & srcb) | (srcc & srca) | (srcb & srca);
case 0xe9:
	return (~srcc & ~srcb & ~srca) | (srcc & srcb) | (srcc & srca) | (srcb & srca);
case 0xea:
	return (srcc) | (srcb & srca);
case 0xeb:
	return (~srcb & ~srca) | (srcc) | (srcb & srca);
case 0xec:
	return (srcb) | (srcc & srca);
case 0xed:
	return (~srcc & ~srca) | (srcb) | (srcc & srca);
case 0xee:
	return (srcc) | (srcb);
case 0xef:
	return (~srca) | (srcc) | (srcb);
case 0xf0:
	return (srca);
case 0xf1:
	return (~srcc & ~srcb) | (srca);
case 0xf2:
	return (srcc & ~srcb) | (srca);
case 0xf3:
	return (~srcb) | (srca);
case 0xf4:
	return (~srcc & srcb) | (srca);
case 0xf5:
	return (~srcc) | (srca);
case 0xf6:
	return (srcc & ~srcb) | (~srcc & srcb) | (srca);
case 0xf7:
	return (~srcb) | (~srcc) | (srca);
case 0xf8:
	return (srcc & srcb) | (srca);
case 0xf9:
	return (~srcc & ~srcb) | (srcc & srcb) | (srca);
case 0xfa:
	return (srcc) | (srca);
case 0xfb:
	return (~srcb) | (srcc) | (srca);
case 0xfc:
	return (srcb) | (srca);
case 0xfd:
	return (~srcc) | (srcb) | (srca);
case 0xfe:
	return (srcc) | (srcb) | (srca);
case 0xff:
	return 0xFFFF;
}
return 0;
}
