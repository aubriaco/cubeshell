#include <solunet.h>
#include <string>
#include <signal.h>
#include <termios.h>
#include <string.h>

static struct termios g_DefaultTerm;
static bool g_Stop = false;

void interruptCallback(int sig)
{
  printf("Interrupt signal called.\n");
  tcsetattr(fileno(stdin), TCSANOW, &g_DefaultTerm);
  g_Stop = true;
}


void writeLString(solunet::ISocket *socket, const std::string& s)
{
  size_t sz = s.size();
  socket->writeBuffer(&sz, 8);
  if(sz > 0)
    socket->writeBuffer(s.c_str(), sz);
}

int main(int argc, char *argv[])
{
  struct sigaction sa_sigint;
  memset(&sa_sigint, 0, sizeof(struct sigaction));
  sa_sigint.sa_handler = interruptCallback;
  sa_sigint.sa_flags = 0;
  sigaction(SIGINT, &sa_sigint, 0);

  std::string host("127.0.0.1"), username("admin"), password, port("5812");
  std::string *next = 0;
  for(int n = 0; n < argc; n++)
  {
    std::string a(argv[n]);
    if(a == "-h")
      next = &host;
    else if(a == "-U")
      next = &username;
    else if(a == "-p")
      next = &port;
    else if(next)
    {
      *next = a;
      next = 0;
    }
  }
  solunet::ISocket *socket = solunet::createSocket(true);

  struct termios term;

  tcgetattr(fileno(stdin), &g_DefaultTerm);
  term = g_DefaultTerm;
  term.c_lflag &= ~ECHO;
  tcsetattr(fileno(stdin), TCSANOW, &term);
  fprintf(stdout, "Username: %s\n", username.c_str());
  fprintf(stdout, "Password: ");
  int i = 0;
  char c = 0;
  char buf[128];
  do
  {
    if(g_Stop)
      break;
    c = fgetc(stdin);
    if(isascii(c) && '\r' != c && '\n' != c)
    {
        buf[i] = c;
        if(i >= 127)
          break;
    }
    i++;
  } while (c != '\n');
  if(g_Stop)
    return 2;
  i++;
  buf[i] = 0;
  password = buf;
  fprintf(stdout, "\n");
  tcsetattr(fileno(stdin), TCSANOW, &g_DefaultTerm);
  fprintf(stdout, "Connecting to: %s\n", host.c_str());

  if(!socket->connect(host.c_str(), std::stoi(port)))
  {
    socket->dispose();
    fprintf(stderr, "Could not connect.\n");
    return 1;
  }
  socket->setThrowExceptions(true);
  try
  {
    int action = 1;
    int r = 0;
    socket->writeBuffer(&action, 4);
    writeLString(socket, username);
    writeLString(socket, password);

    socket->readBuffer(&r, 4);

    if(r == 0)
      fprintf(stdout, "Logged in!\n");
    else
    {
      fprintf(stderr, "Not authenticated.\n");
      socket->dispose();
      return 3;
    }

    action = 2;
    while(!g_Stop)
    {
      fprintf(stdout, "cubeshell> ");
      fgets(buf, 128, stdin);
      std::string s(buf);
      if(s.size() > 0)
        s = s.substr(0, s.size()-1);
      if(s == "exit")
        break;
      socket->writeBuffer(&action, 4);
      writeLString(socket, s);
      socket->readBuffer(&r, 4);
      fprintf(stdout, "%i\n", r);
    }
  }
  catch(int e)
  {
    fprintf(stderr, "Socket exception: %i\n", e);
  }

  socket->dispose();
  return 0;
}
