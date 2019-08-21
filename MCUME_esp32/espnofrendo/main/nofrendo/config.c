/* Nofrendo Configuration Braindead Sample Implementation
**
** $Id: config.c,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "noftypes.h"
#include "log.h"
#include "osd.h"
#include "nofconfig.h"
#include "version.h"


/* interface */
config_t config =
{
   CONFIG_FILE
};

/*
** $Log: config.c,v $
** Revision 1.2  2001/04/27 14:37:11  neil
** wheeee
**
** Revision 1.1.1.1  2001/04/27 07:03:54  neil
** initial
**
** Revision 1.14  2000/11/05 06:23:10  matt
** realloc was incompatible with memguard
**
** Revision 1.13  2000/10/10 13:58:13  matt
** stroustrup squeezing his way in the door
**
** Revision 1.12  2000/09/20 01:13:28  matt
** damn tabs
**
** Revision 1.11  2000/08/04 12:41:04  neil
** current not a bug
**
** Revision 1.10  2000/07/31 04:28:46  matt
** one million cleanups
**
** Revision 1.9  2000/07/24 04:30:42  matt
** slight cleanup
**
** Revision 1.8  2000/07/23 15:16:08  matt
** changed strcasecmp to stricmp
**
** Revision 1.7  2000/07/19 15:58:55  neil
** config file now configurable (ha)
**
** Revision 1.6  2000/07/18 03:28:32  matt
** help me!  I'm a complete mess!
**
** Revision 1.5  2000/07/12 11:03:08  neil
** Always write a config, even if no defaults are changed
**
** Revision 1.4  2000/07/11 15:09:30  matt
** suppressed all warnings
**
** Revision 1.3  2000/07/11 14:59:27  matt
** minor cosmetics.. =)
**
** Revision 1.2  2000/07/11 13:35:38  bsittler
** Changed the config API, implemented config file "nofrendo.cfg". The
** GGI drivers use the group [GGI]. Visual= and Mode= keys are understood.
**
** Revision 1.1  2000/07/11 09:21:10  bsittler
** This is a skeletal configuration system.
**
*/
