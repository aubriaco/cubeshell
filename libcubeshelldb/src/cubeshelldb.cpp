#include <cubeshelldb.h>
#include "CDatabase.h"

namespace cubeshell
{
  IDatabase *createDatabase()
  {
    return new CDatabase();
  }
}
