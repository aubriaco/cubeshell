#include "cli_server.h"
#include <solunet.h>
#include <cubeshelldb.h>
#include "main.h"

using namespace cubeshell;

int cli_cmd(IDatabase *db, std::string s)
{
  std::vector<std::string> p;

  size_t n = 0;
  while((n = s.find(" ")) != std::string::npos)
  {
    std::string i = s.substr(0, n);
    s = s.substr(n+1);
    p.push_back(i);
  }
  p.push_back(s);

  fprintf(stdout, "Param count: %lu\n", p.size());

  return -1;
}

std::string readLString(solunet::ISocket *socket)
{
  size_t len = 0;
  socket->readBuffer(&len, 8);
  if(len == 0)
    return std::string();
  if(len > SN_MAX_LEN)
  {
    fprintf(stderr, "Length of command too long...\n");
    throw 99;
  }
  char *buf = new char[len];
  size_t r = socket->readBuffer(buf, len);
  buf[r] = 0;
  std::string s(buf);
  delete[] buf;

  return s;
}

void *cli_server(void* param)
{
  solunet::ISocket *socket = (solunet::ISocket*)param;
  socket->setThrowExceptions(true);
  IDatabase *db = createDatabase();
  db->setConnectionString(getConfig()->get("db/connection_string").getString());
  db->open();
  fprintf(stdout, "Enter cli.\n");

  bool loggedIn = false;
  socket->setTimeout(5000);
  try
  {
    while(!isStopped())
    {

      int action = 0;
      socket->readBuffer(&action, 4);
      if(action == 1)
      {
        std::string username(readLString(socket));
        fprintf(stdout, "Username: %s\n", username.c_str());
        std::string password(readLString(socket));

        basicfilter_t filter;
        filter["username"] = username;

        basicdoc_t u = db->basicFind("cubeshell", "users", filter);

        if(u.size() > 0)
        {
          int r = 0;
          socket->writeBuffer(&r, 4);
          loggedIn = true;
          fprintf(stdout, "Logged in.\n");
        }

        if(!loggedIn)
        {
          fprintf(stdout, "Failed to log in.\n");
          int r = 1;
          socket->writeBuffer(&r, 4);
        }
      }
      else if(action == 2)
      {
        if(!loggedIn)
        {
          fprintf(stderr, "Not logged in closing...\n");
          break;
        }

        std::string s(readLString(socket));

        int r = cli_cmd(db, s);

        socket->writeBuffer(&r, 4);
      }
    }
  }
  catch(int e)
  {
    fprintf(stderr, "Socket exception: %i\n", e);
  }

  fprintf(stdout, "Exit cli.\n");
  db->dispose();
  socket->dispose();
  return 0;
}

void *cli_listener(void *param)
{
  std::string cert(getConfig()->get("certificate/path").getString());
  std::string pwd(getConfig()->get("certificate/password").getString());
  solunet::ISocket *socket = solunet::createSocket(true);
  socket->setSSLCertificatePassword(pwd.c_str());
  socket->setSSLCertificate(cert.c_str());
  socket->setSSLPrivateKeyFile(cert.c_str());
  socket->setSSLMutual(false);

  socket->bind(getConfig()->get("global/cli_port").getInt());

  socket->listen();

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  pthread_t threadId = 0;
  while(!isStopped())
  {
    solunet::ISocket *s = socket->accept();
    pthread_create(&threadId, &attr, cli_server, s);
  }

  socket->dispose();
  return 0;
}
