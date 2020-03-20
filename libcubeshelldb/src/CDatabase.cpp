#include "CDatabase.h"
#include <rapidjson/document.h>

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

  basicdoc_t CDatabase::basicFind(const std::string& dbname, const std::string& collname)
  {
    basicdoc_t m;
    const bson_t *doc;
    mongoc_collection_t *collection = mongoc_client_get_collection(Client, dbname.c_str(), collname.c_str());
    bson_t *query = bson_new();
    mongoc_cursor_t *cursor = mongoc_collection_find_with_opts(collection, query, 0, 0);
    while (mongoc_cursor_next(cursor, &doc))
    {
       std::map<std::string, std::string> a;
       char *str = bson_as_canonical_extended_json(doc, 0);
       rapidjson::Document doc;
       doc.Parse(str);
       for (rapidjson::Value::ConstMemberIterator p = doc.MemberBegin(); p != doc.MemberEnd(); ++p)
       {
         if(p->value.IsString())
          a[p->name.GetString()] = p->value.GetString();
       }
       bson_free(str);
       m.push_back(a);
    }

    bson_destroy(query);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    return m;
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
