#ifndef __C_CONFIG_INCLUDED__
#define __C_CONFIG_INCLUDED__
#include <cubeshellconfig/IConfig.h>
#include <rapidjson/document.h>
#include <string>
#include <map>

namespace cubeshell
{
  class CConfig : public IConfig
  {
  protected:
    std::map<std::string, MConfigVar> Dict;
    rapidjson::Document D;
    MConfigVar getFromValue(rapidjson::Value &v, const std::string& name);
  public:
    CConfig(const std::string &fn);
    ~CConfig();
    MConfigVar& get(std::string path);
    bool isValid();
    void dispose();
  };
}

#endif
