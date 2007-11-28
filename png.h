#ifndef PNG_H
#define PNG_H

/*

(c) 2003 Peter Putzer <pputzer@users.sourceforge.net>

Licensed under the GNU General Public License (GPL).

*/

#include "sections.h"

extern Err EncodePNG(UInt8* orig_buffer, MemHandle stream, UInt32* bytes_written,
		     MemHandle note, MemHandle title, Boolean hires) SUTIL3;
extern Err DecodePNG(MemPtr stream, UInt32 size, UInt8* buffer, MemHandle* note,
		     Boolean hires);

#endif /* PNG_H */
