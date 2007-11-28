#ifndef THUMBNAILS_H
#define THUMBNAILS_H

/*

(c) 2002 Peter Putzer
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include <PalmOS.h>
#include "sections.h"

extern Boolean ThumbnailViewHandleEvent (EventPtr event) SUTIL3;
extern void ThumbnailViewNextCategory (void) SUTIL2;

#endif /* THUMBNAILS_H */
