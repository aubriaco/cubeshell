#include "CConfig.h"
namespace cubeshell
{
std::string readTextFile(const std::string& fname)
{
  FILE* file = fopen(fname.c_str(), "r");
  if(file)
  {
    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buf = new char[len];
    fread(buf, 1, len, file);
    fclose(file);
    std::string r(buf, buf + len);
    delete[] buf;
    return r;
  }
  return std::string();
}

CConfig::CConfig(const std::string &fn)
{
  std::string body = readTextFile(fn);
  if(body.size() > 0)
    D.Parse(body.c_str());
}

CConfig::~CConfig()
{
  //nothing
}

bool CConfig::isValid()
{
  return D.IsObject();
}

MConfigVar CConfig::get(const std::string& path)
{

}
}
