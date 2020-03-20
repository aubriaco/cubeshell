#include <cubeshellconfig.h>
#include "CConfig.h"

namespace cubeshell
{
  IConfig *createConfig(const std::string& fn)
  {
    return new CConfig(fn);
  }
}
