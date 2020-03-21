#ifndef __I_DATABASE_INCLUDED__
#define __I_DATABASE_INCLUDED__
#include <vector>
#include <string>
#include <map>
namespace cubeshell
{
  typedef std::map<std::string, std::string> basicfilter_t;
  typedef std::vector< basicfilter_t > basicdoc_t;

  class IDatabase
  {
  public:
    virtual void setConnectionString(const std::string& cs) = 0;
    virtual void setDatabaseName(const std::string& name) = 0;
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual basicdoc_t basicFind(const std::string& dbname, const std::string& collname, basicfilter_t filter = basicfilter_t()) = 0;
    virtual void dispose() = 0;
  };
}
#endif
