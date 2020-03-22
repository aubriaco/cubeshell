#ifndef __COMMANDS_INCLUDED__
#define __COMMANDS_INCLUDED__
#include "MContainer.h"
#include <solunet.h>

void nodeCommand_send(solunet::ISocket *socket, int action);
void nodeCommand_recv(solunet::ISocket *socket, int action);

#endif
