#ifndef __M_NODE_INCLUDED__
#define __M_NODE_INCLUDED__
#include "MUsage.h"

class MNode
{
public:
  std::string Name, Host;
  bool Self;

  MUsage Usage;

  MNode() : Self(false) {}
};

#endif
