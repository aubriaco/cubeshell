#include <solunet.h>

int main(int argc, char *argv[])
{
  solunet::ISocket *socket = solunet::createSocket(true);

  socket->dispose();
  return 0;
}
