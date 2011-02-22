#ifndef GP_H
#define GP_H

/*
 * Generator Polinomial
 * For a given number of errorCorrection Blocks a different Generator Polinomial is needed.
 */

#include "util.h"


/*
 * get the corresponsing polynomial
 */
const byte* get_gp(unsigned int numBlocks); 


#endif //GP_H

