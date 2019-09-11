#include "MIDASreader.h"
#include <iostream>

using namespace std;

int main(int argc ,char **argv)
{

  MIDASfile mf(argv[1]);

  std::cout<<mf.GetName()<<std::endl;

}
