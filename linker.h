
#ifndef __LINKER_H_
#define __LINKER_H_

#include <PalmOS.h>

/*Note: you don't have to "send" the SysAppLaunch command specifically to
Linker.
It's always watching, so the cardNo and dbID are irrelevent. However, we have
to make sure they are at least valid numbers, in case Linker isn't there to
watch!
We can be sure to not conflict with other apps that already use 0x9181, but
requiring
a 'Linx' message within the commandParameterBlock.
*/

typedef struct {
    UInt32 creator; /* set this to 'Linx', just to make sure only the
                        desired function calls get responded to */
    UInt16 message; /* Just one message needed for now... */
    Char linkTag[6]; /* Passes empty to Linker.  Linker will fill with
                        the 5 character "<aaa>" which the calling
                        program will write to the record.
                        Has an extra char for the null to ease copying
                        into the new record */
    UInt32 gotoTargetAppCreator; /* Creator ID of the target of the link */
    GoToParamsType gotoLinkParams; /* Goto params of the target of the link */
} LinkerCPB;

/* message parameters */
#define LinkerNewLinkMessage    0

/* SysAppLaunch command */
#define linkerAppLaunchCmdSignalLinker  0x9181

#endif /* __LINKER_H_ */
