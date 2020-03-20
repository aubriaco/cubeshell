#ifndef __I_CONFIG_INCLUDED__
#define __I_CONFIG_INCLUDED__
#include <string>
#include "MConfigVar.h"

namespace shellcube
{
  class IConfig
  {
  public:
    virtual MConfigVar get(const std::string& path) = 0;
    virtual bool isValid() = 0;
  };
}

#endif
