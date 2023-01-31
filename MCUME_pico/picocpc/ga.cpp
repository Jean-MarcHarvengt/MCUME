/**
 * Gate Array is connected to a 16Mhz clock signal, which is then divided to 4Mhz to feed the Z80, and 1Mhz for the CRTC. 
 * Gate Array's READY signal is connected to Z80's WAIT input for "memory contention". 
 * This means CPU execution is halted every time Gate Array and CPU accesses the memory at the same time. 
 * The reason for this is that both GA and the CPU share the same address/data bus for memory access.
*/