#ifndef THUMBNAILS_DETAILS_H
#define THUMBNAILS_DETAILS_H

/*

(c) 2002, 2004 Peter Putzer
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include <PalmOS.h>
#include "sections.h"

extern Boolean ThumbnailDetailViewHandleEvent (EventPtr event) SUTIL3;
extern void ThumbnailDetailViewNextCategory (void) SUTIL2;
extern void ThumbnailDetailViewLoadRecords(FormType* frm) SUTIL3;

#endif /* THUMBNAILS_DETAILS_H */
