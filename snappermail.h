#ifndef SNAPPERMAIl_H
#define SNAPPERMAIL_H

/*

(c) 2003 Peter Putzer <pputzer@users.sourceforge.net>

Licensed under the GNU General Public License (GPL).

*/

#include "sections.h"

extern void Switch(UInt32 crId) SUTIL4;
extern Err ReceiveData(ExgSocketPtr exgSocketP, Boolean appIsActive);
extern Char* EscapeDotsInFilename(const Char* filename) SUTIL4;
extern Err Send(Char type[], Char filename[], MemPtr buffer, UInt32 bufferSize) SUTIL4;
extern void SendCurrentRecord(void) SUTIL4;

#endif /* SNAPPERMAIL_H */
