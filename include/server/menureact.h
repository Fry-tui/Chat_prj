#ifndef __MENUREACT_H_
#define __MENUREACT_H_

#include "server.h"

struct User * init_user_menureact;

void reactMainMenu(int *);
void reactUserMenu(struct User *user);

#endif