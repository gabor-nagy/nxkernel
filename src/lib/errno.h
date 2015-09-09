#ifndef LIB_ERRNO_H
#define LIB_ERRNO_H

extern int errno;

/* Error code explanations below are from:
 * http://www.opengroup.org/onlinepubs/009695399/basedefs/errno.h.html
 */
 
/* ISO Standard C */

#define EDOM	18	/* Mathematics argument out of domain of function. */
#define EILSEQ	25	/* Illegal byte sequence. */
#define ERANGE	70	/* Result too large. */

#endif
