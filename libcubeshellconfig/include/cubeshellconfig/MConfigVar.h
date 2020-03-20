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
    MConfigVar() { S = "__NULL__"; }
    MConfigVar(const std::string& v) { S = v; }
    MConfigVar(int v) { I = v; S = std::to_string(I); }
    MConfigVar(double v) { D = v; S = std::to_string(D); }
    std::string getString() { return S; }
    int getInt() { return I; }
    double getDouble() { return D; }
    bool isNull() { return S == "__NULL__"; }
  };
}

#endif
