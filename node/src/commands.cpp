#include "commands.h"
#include "machineres.h"

int deploy(MContainer& container)
{
  return -2;
}

int usage_send(solunet::ISocket *socket)
{
  MUsage usage = getUsage();
  socket->writeBuffer(&usage.MemUsage, 8);
  return 0;
}

int usage_recv(solunet::ISocket *socket)
{
  MUsage usage = getUsage();
  socket->readBuffer(&usage.MemUsage, 8);
  return 0;
}


void nodeCommand_send(solunet::ISocket *socket, int action)
{
  if(action == 3)
    usage_send(socket);
}

void nodeCommand_recv(solunet::ISocket *socket, int action)
{
  if(action == 3)
    usage_recv(socket);
}
