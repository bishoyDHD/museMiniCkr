#ifndef _CTELESCOPE_H_
#define _CTELESCOPE_H_

#include <cstddef>

#include "cGEM.h"

class cTelescope
{
 public:
  cTelescope(int numberofGEMs);
  ~cTelescope();
 private:
  int    nGEMs;
  cGEM **GEM;
};

#endif
