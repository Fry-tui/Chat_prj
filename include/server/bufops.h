#ifndef __BUFOPS_H
#define __BUFOPS_H

/************************************
 *      		INCLUDE				*
 ************************************/
#include "server.h"
struct Buffer * init_buffer;

/************************************
 *      	  FUNCTION				*
 ************************************/
void readBuffer(int,int,char[],struct Buffer *,char []);

#endif