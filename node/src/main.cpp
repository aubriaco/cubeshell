#include <solunet.h>
#include <signal.h>
#include <unistd.h>
#include <cubeshelldb.h>
#include <cubeshellconfig.h>
#include <rapidjson/document.h>
#include "MNode.h"
#include "MContainer.h"
#include "cli_server.h"
#include "commands.h"
#include "machineres.h"
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
        MUsage usage = getUsage();
        socket->writeBuffer(&usage, sizeof(MUsage));
      }
      else
        nodeCommand_recv(socket, action);

      //sleep(15);
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

  solunet::ISocket *socket = solunet::createSocket(true);
  socket->setSSLCertificatePassword(Config->get("certificate/password").getString().c_str());
  socket->setSSLCertificate(Config->get("certificate/path").getString().c_str());
  socket->setSSLPrivateKeyFile(Config->get("certificate/path").getString().c_str());
  socket->setSSLMutual(true);
  if(!socket->connect(node->Host.c_str(), Config->get("global/port").getInt()))
  {
    socket->dispose();
    fprintf(stdout, "Failed to connect to node: %s\n", node->Name.c_str());
    return 0;
  }

  socket->setThrowExceptions(true);

  try
  {
    int action;
    while(!g_Stop)
    {

      action = 0;
      socket->writeBuffer(&action, 4);
      if(action == 0)
      {
        MUsage usage;
        socket->readBuffer(&usage, sizeof(MUsage));
        fprintf(stdout, "Usage reported [MEM: %f]\n", usage.MemUsage);
      }
      else
        nodeCommand_send(socket, action);

      sleep(15);
    }
  }
  catch(int e)
  {
    fprintf(stdout, "Connector socket exception: %i\n", e);
  }

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
        fprintf(stdout, "Creating node connect thread...\n");
        pthread_create(&threadId, &attr, connectNode, node);
      }
      Nodes.push_back(node);
    }
  }
  DB->close();
}


void *loop(void * param)
{
  while(!g_Stop)
  {
    if(getContainerQueue().size() > 0)
    {
      MContainer container = getContainerQueue().front();
      fprintf(stdout, "Deploying %s/%s: %s\n", container.Namespace.c_str(), container.Name.c_str(), container.ImageName.c_str());
      if(system(std::string("docker run -d --rm --name=\"" + container.Namespace + "." + container.Name + "\" " + container.ImageName).c_str()) == 0)
      {
        fprintf(stdout, "Deployed %s/%s: %s\n", container.Namespace.c_str(), container.Name.c_str(), container.ImageName.c_str());
      }
      getContainerQueue().pop();
    }
    sleep(5);
  }
  return 0;
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
  pthread_create(&threadId, &attr, loop, 0);
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
