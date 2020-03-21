#ifndef __C_DATABASE_INCLUDED__
#define __C_DATABASE_INCLUDED__
#include <cubeshelldb/IDatabase.h>
#include <mongoc/mongoc.h>

namespace cubeshell
{
  class CDatabase : public IDatabase
  {
  private:
    std::string ConnectionString, DBName;
    mongoc_client_t *Client;
    mongoc_database_t *DB;
  public:
    CDatabase();
    ~CDatabase();
    void setConnectionString(const std::string& cs);
    void setDatabaseName(const std::string& name);
    bool open();
    void close();
    basicdoc_t basicFind(const std::string& dbname, const std::string& collname, basicfilter_t filter = basicfilter_t());
    void dispose();
  };
}
#endif
