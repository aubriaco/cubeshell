#include <CDatabase.h>

CDatabase::CDatabase()
{
  if(DBC == 0)
    mongodb_init();
  DBC++;
}

CDatabase::~CDatabase()
{
  DBC--;
  if(DBC == 0)
    mongoc_cleanup();
}

void CDatabase::setConnectionString(const std::string& cs)
{
  ConnectionString = cs;
}

void CDatabase::open()
{
}

void CDatabase::close()
{
}
