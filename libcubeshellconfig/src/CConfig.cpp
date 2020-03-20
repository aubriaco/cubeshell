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

  MConfigVar CConfig::getFromValue(rapidjson::Value &v, const std::string& name)
  {
    if(v.HasMember(name.c_str()))
    {
      if(v[name.c_str()].IsString())
      return MConfigVar(v[name.c_str()].GetString());
      else if(v[name.c_str()].IsInt())
      return MConfigVar(v[name.c_str()].GetInt());
      else if(v[name.c_str()].IsDouble())
      return MConfigVar(v[name.c_str()].GetDouble());
    }
    return MConfigVar("__NULL__");
  }

  MConfigVar& CConfig::get(std::string path)
  {
    if(!D.IsObject())
    {
      Dict[path] = MConfigVar("__NULL__");
      return Dict[path];
    }
    if(Dict.count(path) > 0)
    return Dict[path];
    std::string dpath(path);
    size_t n = path.find('/');
    rapidjson::Document d;
    d.CopyFrom(D, d.GetAllocator());
    rapidjson::Value v = d.GetObject();
    while(n != std::string::npos)
    {
      std::string p = path.substr(0, n);
      path = path.substr(n+1);

      if(v.IsObject() && v[p.c_str()].IsObject())
      v = v[p.c_str()].GetObject();
      n = path.find('/');
    }
    MConfigVar mcv = getFromValue(v, path);
    Dict[dpath] = mcv;
    return Dict[dpath];
  }
}
