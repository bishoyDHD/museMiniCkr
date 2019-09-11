#ifndef __MCPROPGEANT4__
#define __MCPROPGEANT4__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include <iostream>

class cookerRM;

class MCPropGeant4:public Plugin
{
 private:
  std::string gdmlFileName;
  cookerRM *runmanager;

 public:
  MCPropGeant4(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
  virtual ~MCPropGeant4();


  //setup routines
  Long_t setGDMLFileName(char *);



  // process routines
  Long_t startup();
  Long_t process();
  Long_t finalize();

  virtual Long_t cmdline(char * cmd);

  ClassDef(MCPropGeant4,1);
    };

#endif
