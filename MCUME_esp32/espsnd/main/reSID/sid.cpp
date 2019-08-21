/*
	Arduino SID
	Copyright (c) 2015 Frank Bösing
	This library is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this library.  If not, see <http://www.gnu.org/licenses/>.

	Diese Bibliothek ist freie Software: Sie können es unter den Bedingungen
	der GNU General Public License, wie von der Free Software Foundation,
	Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
	veröffentlichten Version, weiterverbreiten und/oder modifizieren.
	Diese Bibliothek wird in der Hoffnung, dass es nützlich sein wird, aber
	OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
	Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
	Siehe die GNU General Public License für weitere Details.
	Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
	Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.

 */

#include "reSID/envelope.cc"
#include "reSID/extfilt.cc"
#include "reSID/filter.cc"
#include "reSID/pot.cc"
#include "reSID/version.cc"
#include "reSID/voice.cc"


#include "reSID/wave6581__ST.cc" 
#include "reSID/wave6581_P_T.cc"
#include "reSID/wave6581_PS_.cc"
#include "reSID/wave6581_PST.cc"

/*
#include "reSID/wave8580__ST.cc" 
#include "reSID/wave8580_P_T.cc"
#include "reSID/wave8580_PS_.cc"
#include "reSID/wave8580_PST.cc"
*/
#include "reSID/wave.cc"

#include "reSID/sid.cc"