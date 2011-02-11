#ifndef GP_H
#define GP_H

/*
 * Generator Polinomial
 * For a given number of errorCorrection Blocks a different Generator Polinomial is needed.
 * These are saved in gp.rc
 * in initialize_gp() the polynomials are read from the file and saved in gp
 * with get_gp() we get the polynomial corresponding to the number of need errorCorrection Blocks
 */

#include "util.h"

//static int gp_initialized;
byte** gp;

/*
 * setupt gp
 */
void initialize_gp();

/* 
 * free allocated memory
 */
void destroy_gp();

/*
 * get the corresponsing polynomial
 */
const byte* get_gp(unsigned int numBlocks); 


#endif //GP_H

