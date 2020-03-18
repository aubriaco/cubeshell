#ifndef __C_DATABASE_INCLUDED__
#define __C_DATABASE_INCLUDED__
#include <IDatabase.h>

class CDatabase
{
private:
  std::string ConnectionString;
  static size_t DBC = 0;
public:
  CDatabase();
  ~CDatabase();
  void setConnectionString(const std::string& cs);
  void open();
  void close();
};

#endif
