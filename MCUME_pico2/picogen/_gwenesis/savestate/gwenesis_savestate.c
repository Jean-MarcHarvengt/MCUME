/*
Gwenesis : Genesis & megadrive Emulator.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.

__author__ = "bzhxx"
__contact__ = "https://github.com/bzhxx"
__license__ = "GPLv3"

*/
#pragma GCC optimize("Ofast")

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../cpus/M68K/m68k.h"
#include "../io/gwenesis_io.h"
#include "../bus/gwenesis_bus.h"
#include "../vdp/gwenesis_vdp.h"
#include "../sound/z80inst.h"

#include "../savestate/gwenesis_savestate.h"

#include <assert.h>

void gwenesis_save_state() {
  /* DO NOT CHANGE ORDER - NEEDS TO BE SAME AS IN LOAD */
  gwenesis_m68k_save_state();
  gwenesis_io_save_state();
  gwenesis_bus_save_state();
  gwenesis_vdp_gfx_save_state();
  gwenesis_vdp_mem_save_state();

}

void gwenesis_load_state() {
  /* DO NOT CHANGE ORDER - NEEDS TO BE SAME AS IN SAVE */
  gwenesis_m68k_load_state();
  gwenesis_io_load_state();
  gwenesis_bus_load_state();
  gwenesis_vdp_gfx_load_state();
  gwenesis_vdp_mem_load_state();

}
