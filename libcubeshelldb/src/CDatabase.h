#ifndef __C_DATABASE_INCLUDED__
#define __C_DATABASE_INCLUDED__
#include <cubeshelldb/IDatabase.h>
#include <mongoc/mongoc.h>
namespace cubeshell
{
class CDatabase
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
  void open();
  void close();
};
}
#endif
