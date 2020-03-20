#include <solunet.h>
#include <signal.h>
#include <unistd.h>
#include <cubeshellconfig.h>
#include <cubeshelldb.h>

using namespace cubeshell;

static bool g_Stop = false;
static IConfig *Config = 0;
static IDatabase *DB = 0;

void interruptCallback(int sig)
{
	printf("Interrupt signal called.\n");
  g_Stop = true;
  solunet::ISocket *socket = solunet::createSocket(false);
  socket->connect("127.0.0.1",5811);
  int action = -1;
  socket->writeBuffer(&action, 4);
  socket->dispose();
}

void* node(void *param)
{
  fprintf(stdout, "Node connection.\n");
  solunet::ISocket *socket = (solunet::ISocket*)param;
  while(!g_Stop)
  {
    int action = 0;
    socket->writeBuffer(&action, 4);
    socket->readBuffer(&action, 4);
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
	fprintf(stdout, "Connection string set %s\n", Config->get("db/connection_string").getString().c_str());
}

int main(int argc, char *argv[])
{
  fprintf(stdout, "=======================================================\n");
  fprintf(stdout, "SHELLCUBE NODE\n");
  fprintf(stdout, "=======================================================\n");

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = interruptCallback;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  solunet::ISocket *socket = solunet::createSocket(true);
  socket->setSSLCertificatePassword("1234");
  socket->setSSLCertificate("../certs/node.pem");
  socket->setSSLPrivateKeyFile("../certs/node.pem");
  socket->setSSLMutual(true);
  socket->bind(5811);
  socket->listen();
	fprintf(stdout, "Initialized! Listening...\n");

	configure();


  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  while(!g_Stop)
  {
    solunet::ISocket *s = socket->accept();
    pthread_t threadId = 0;
    pthread_create(&threadId, &attr, node, s);
  }

  socket->dispose();
  return 0;
}
