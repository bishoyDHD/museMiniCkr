#include <iostream>
#include "TApplication.h"
#include <comparewindow.h>

using namespace std;



int main(int argc, char **argv)
{

  TApplication theApp("michelin",&argc,argv);
  bool gui=true;
  for (int i=0; i<argc; i++)
    if (!strcmp(argv[i], "--headless"))
      gui=false;
  CompareWindow *mainWindow=new CompareWindow(gClient->GetRoot(),1000,480,gui);
  
  if (gui)
    theApp.Run();

  return 0;
}
