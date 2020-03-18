#ifndef __I_DATABASE_INCLUDED__
#define __I_DATABASE_INCLUDED__
#include <string>

class IDatabase
{
public:
  virtual void setConnectionString(const std::string& cs) = 0;
  virtual void open() = 0;
  virtual void close() = 0;
};

#endif
