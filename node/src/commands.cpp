#include "commands.h"
#include "machineres.h"

int deploy(MContainer& container)
{
  return -2;
}

int usage(solunet::ISocket *socket)
{
  MUsage usage = getUsage();
  socket->writeBuffer(&usage.MemUsage, 8);
  return 0;
}
