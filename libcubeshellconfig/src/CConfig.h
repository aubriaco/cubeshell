#ifndef __C_CONFIG_INCLUDED__
#define __C_CONFIG_INCLUDED__
#include <cubeshellconfig/IConfig.h>
#include <rapidjson/document.h>
#include <string>

namespace cubeshell
{
  class CConfig : public IConfig
  {
  protected:
    rapidjson::Document D;
  public:
    CConfig(const std::string &fn);
    ~CConfig();
    MConfigVar get(const std::string& path);
    bool isValid();
  };
}

#endif
