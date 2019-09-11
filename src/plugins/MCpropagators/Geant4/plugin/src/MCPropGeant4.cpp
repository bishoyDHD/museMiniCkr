#include <MCPropGeant4.h>

#include<iostream>
#include<cmath>
#include "cookerRM.h"
#include "cookerDetector.h"
#include "Physics.h"
#include "cookerGen.h"


MCPropGeant4::MCPropGeant4(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):Plugin(in,out,inf_,outf_,p)
{

  gdmlFileName = "Geant4_survey2.gdml";
};

MCPropGeant4::~MCPropGeant4()
{
};




Long_t MCPropGeant4::setGDMLFileName(char * name)
{
  gdmlFileName=name;
  return 0;
}



Long_t MCPropGeant4::startup()
{
  // construct geant 4 and geometry
  debug(100,"Reading GDML to construct geometry\n");


  char buffer[1024];
  snprintf(buffer,1000,"%s/.olympus/shared/gdml/%s",getenv("COOKERHOME"),gdmlFileName.c_str());


  runmanager=new cookerRM();
  runmanager->SetUserInitialization(new cookerDetector(buffer,this));

  runmanager->SetUserInitialization( new Physics());
  
  GeneratorEvent * ge=NULL;
  getOutBranchObject("Generator",(TObject **) &ge);
  debug(0,"Out gen %p\n",ge);
  if (!ge)
    {
      copyBranch("Generator");
      getBranchObject("Generator",(TObject **) &ge);
    }
  if (!ge)
    {
      debug(0,"Failed to find Generator Branch.\n");
      exit(-1);
    }
   runmanager->SetUserAction(new cookerGen(ge));
  // runaction, eventaction?

  runmanager->Initialize();
  if (!runmanager->startRun())
    {
      debug(0,"Error starting geant4 run\n");
      exit(-1);
    }
  return 0;
}

Long_t MCPropGeant4::process()
{
  runmanager->doEvent( in->GetReadEntry());
  return 0;
}


Long_t MCPropGeant4::finalize()
{
  runmanager->endRun();
  delete runmanager;
  return 0;
}



Long_t MCPropGeant4::cmdline(char *cmd)
{
  //add cmdline hanling here

  return 0; // 0 = all ok
};


extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new MCPropGeant4(in,out,inf_,outf_,p);
}
}


ClassImp(MCPropGeant4);

