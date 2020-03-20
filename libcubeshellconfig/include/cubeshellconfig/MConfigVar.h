#ifndef __M_CONFIG_VAR_INCLUDED__
#define __M_CONFIG_VAR_INCLUDED__
#include <string>

namespace cubeshell
{
  class MConfigVar
  {
  private:
    std::string S;
    int I;
    double D;
  public:
    std::string getString() { return S; }
    int getInt() { return I; }
    double getDouble() { return D; }
  };
}

#endif
