#ifndef __M_NODE_INCLUDED__
#define __M_NODE_INCLUDED__

class MNode
{
public:
  std::string Name, Host;
  bool Self;

  MNode() : Self(false) {}
};

#endif
