#include "CDatabase.h"
namespace cubeshell
{
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
  close();
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

bool CDatabase::open()
{
  bson_error_t error;
  mongoc_uri_t *uri = mongoc_uri_new(ConnectionString.c_str());
  if(uri)
  {
    Client = mongoc_client_new_from_uri(uri);
    if(Client)
    {
      mongoc_client_set_appname(Client, "ShellCubeDB");
      DB = mongoc_client_get_database(Client, DBName.c_str());
      mongoc_uri_destroy(uri);
      bson_t reply;
      bson_t *command = BCON_NEW("ping", BCON_INT32(1));
      bool retval = mongoc_client_command_simple(Client, "admin", command, NULL, &reply, &error);
      bson_destroy(&reply);
      bson_destroy(command);
      if (!retval)
      {
        fprintf (stderr, "%s\n", error.message);
        return false;
      }
    }
  }

  return true;
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
}
