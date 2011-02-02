#ifndef GP_H
#define GP_H

#include "util.h"

//static int gp_initialized;
byte** gp;

void initialize_gp();

void destroy_gp();

const byte* get_gp(unsigned int numBlocks); 


#endif //GP_H

