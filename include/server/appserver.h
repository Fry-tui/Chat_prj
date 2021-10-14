#ifndef __APP_H_
#define __APP_H_

#include "server.h"

void reactRegister(int ,char[]);
void reactLogin(int ,char[],int,struct sockaddr_in,struct sockaddr_in);

#endif