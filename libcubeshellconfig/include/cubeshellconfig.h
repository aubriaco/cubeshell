#ifndef __CUBESHELLCONFIG_INCLUDED__
#define __CUBESHELLCONFIG_INCLUDED__
#include "cubeshellconfig/IConfig.h"

namespace cubeshell
{
  IConfig *createConfig(const std::string& fn);
}

#endif
