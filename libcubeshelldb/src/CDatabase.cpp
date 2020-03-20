#include "CDatabase.h"

static size_t DBC = 0;

CDatabase::CDatabase()
{
  if(DBC == 0)
    mongoc_init();
  DBC++;
  Client = 0;
  DB = 0;
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

void CDatabase::setDatabaseName(const std::string& name)
{
  DBName = name;
}

void CDatabase::open()
{
  mongoc_uri_t *uri = mongoc_uri_new(ConnectionString.c_str());
  Client = mongoc_client_new_from_uri(uri);
  mongoc_client_set_appname(Client, "ShellCubeDB");
  DB = mongoc_client_get_database(Client, DBName.c_str());
  mongoc_uri_destroy(uri);
}

void CDatabase::close()
{
  if(DB)
  {
    mongoc_database_destroy(DB);
    DB = 0;
  }
  if(Client)
  {
    mongoc_client_destroy(Client);
    Client = 0;
  }
}
