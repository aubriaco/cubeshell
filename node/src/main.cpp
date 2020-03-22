#include <solunet.h>
#include <signal.h>
#include <unistd.h>
#include <cubeshelldb.h>
#include <cubeshellconfig.h>
#include <rapidjson/document.h>
#include "MNode.h"
#include "MContainer.h"
#include "cli_server.h"
#include <queue>

using namespace cubeshell;

static bool g_Stop = false;
static IConfig *Config = 0;
static IDatabase *DB = 0;
static std::vector<MNode*> Nodes;
static std::queue<MContainer> ContainerQueue;

bool isStopped()
{
  return g_Stop;
}

IConfig *getConfig()
{
  return Config;
}

std::queue<MContainer>& getContainerQueue()
{
  return ContainerQueue;
}

void interruptCallback(int sig)
{
  printf("Interrupt signal called.\n");
  g_Stop = true;
  solunet::ISocket *socket = solunet::createSocket(false);
  socket->connect("127.0.0.1",Config->get("global/port").getInt());
  int action = -1;
  socket->writeBuffer(&action, 4);
  socket->dispose();
}

void nodeCommand(solunet::ISocket *socket, int action)
{

}

void* node(void *param)
{
  fprintf(stdout, "Node connection.\n");
  solunet::ISocket *socket = (solunet::ISocket*)param;
  socket->setThrowExceptions(true);
  try
  {
    while(!g_Stop)
    {
      int action = 0;

      socket->readBuffer(&action, 4);

      if(action == 0)
      {
        socket->writeBuffer(&action, 4);
      }
      else
        nodeCommand(socket, action);

      sleep(5);
    }
  }
  catch(int e)
  {
    fprintf(stderr, "Socket exception: %i\n", e);
  }
  socket->dispose();
  fprintf(stdout, "Node exit.\n");
  return 0;
}

void configure()
{
  Config = createConfig("../conf/node.json");
  DB = createDatabase();
  DB->setConnectionString(Config->get("db/connection_string").getString());
}

void* connectNode(void *param)
{
  MNode *node = (MNode*)param;

  return 0;
}

MNode *findNodeByName(const std::string &name)
{
  for(std::vector<MNode*>::iterator it = Nodes.begin(); it != Nodes.end(); ++it)
  {
    if((*it)->Name == name)
    {
      return (*it);
    }
  }
  return 0;
}

void scanNodes()
{
  fprintf(stdout, "Scanning nodes...\n");
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  pthread_t threadId = 0;

  DB->open();
  basicdoc_t doc = DB->basicFind("cubeshell", "nodes");
  for(basicdoc_t::iterator it = doc.begin(); it != doc.end(); ++it)
  {
    MNode *node = new MNode();
    node->Name = (*it)["name"];
    node->Host = (*it)["host"];
    fprintf(stdout, "Found Node: %s | Host: %s ... ", node->Name.c_str(), node->Host.c_str());
    if(findNodeByName(node->Name))
    {
      fprintf(stdout, "Exists\n");
      delete node;
    }
    else
    {
      if(node->Name == Config->get("name").getString())
      {
        node->Self = true;
        fprintf(stdout, "Thats me!\n");
      }
      else
      {
        fprintf(stdout, "\n");
        pthread_create(&threadId, &attr, connectNode, node);
      }
      Nodes.push_back(node);
    }
  }
  DB->close();
}


int main(int argc, char *argv[])
{
  fprintf(stdout, "=======================================================\n");
  fprintf(stdout, "CUBESHELL NODE\n");
  fprintf(stdout, "=======================================================\n");

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = interruptCallback;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  configure();
  fprintf(stdout, "Configured!\n");

  if(!DB->open())
  {
    fprintf(stderr, "Failed to connect to database.\n");
    return 1;
  }

  std::string cert(Config->get("certificate/path").getString());
  std::string pwd(Config->get("certificate/password").getString());
  solunet::ISocket *socket = solunet::createSocket(true);
  socket->setSSLCertificatePassword(pwd.c_str());
  socket->setSSLCertificate(cert.c_str());
  socket->setSSLPrivateKeyFile(cert.c_str());
  socket->setSSLMutual(true);
  socket->bind(Config->get("global/port").getInt());
  socket->listen();

  scanNodes();

  fprintf(stdout, "Initialized! Listening...\n");


  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  pthread_t threadId = 0;
  pthread_create(&threadId, &attr, cli_listener, 0);
  while(!g_Stop)
  {
    solunet::ISocket *s = socket->accept();
    pthread_create(&threadId, &attr, node, s);
  }

  for(std::vector<MNode*>::iterator it = Nodes.begin(); it != Nodes.end(); ++it)
  {
    delete (*it);
  }

  socket->dispose();
  DB->dispose();
  Config->dispose();
  return 0;
}
