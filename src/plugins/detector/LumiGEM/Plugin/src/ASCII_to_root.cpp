//#include <Det_LumiGEM.h>
//#include "../../LumiGEMlib/clustering/cGEM.cxx"
#include "cAPV.cxx"
//#include <ASCII_to_root.h>

#include "TStyle.h"

#include<iostream>
#include<cmath>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <string>
#include<stdio.h> 

//#include "cLumiGEMAsciiRead.h"
//#include "cLumiGEMDataRead.h"

using namespace std;

// Read data (CERN staff) from an ascii file and create a root file with a Tree.
// see also a variant in staff.C
// Author: Rene Brun


//void ASCII_to_root(int runno=-1)
//{
// gStyle->SetOptFit(1);

// LoadEvents(Form("/home/anusha/work/muse/muse/Analysis/HUlab/data/GEMrun00%d.dat", runno));//events_01.dat
//}

void ASCII_to_root(int runno)
{

  //TFile *ASCII_to_root(Int_t get=0, Int_t print=1) {

   Int_t           GEM0;
   Int_t           GEM1;

FILE *asciifile;

   //    int eventid;
   //  int apvid;
   //  int numapvs;
   //  int apvdata[12][128];

   //The input file cern.dat is a copy of the CERN staff data base
   //from 1988
    TString filename = "run00650.root";
   //  TString dir = gSystem->UnixPathName(__FILE__);
   //  dir.ReplaceAll("cernbuild.C","");
   //  dir.ReplaceAll("/./","/");

    // printf(" using ascii input file as data source!\n");
 //char fn[256];
 //FILE *fn;
   asciifile = fopen(Form("/home/anusha/work/muse/muse/Analysis/HUlab/data/GEMrun00%d.dat", runno),"r");

   //asciifile = fopen(fn, "r");
      if (asciifile)
	//	printf(" using ascii input file >%s< as data source!\n", asciifile);
	printf("using ascii input file as data source!\n", asciifile);
      else
	{
	  printf("Cannot open >%s", asciifile);
	};
      /////////////////////////
 /*
 cGEM *mygem = new cGEM(4);
  cAPV *myapv =new cAPV();
 
  //  mygem->AddAPVy(0,1);
  //  mygem->AddAPVy(1,1);
  //  mygem->AddAPVx(2,1);
  // mygem->AddAPVx(3,1); 


   int data[128];

  int n[4];
   int m[4];
   int *x[4];
   int *y[4];
   
  
  
for(int i=0;i<128;i++)
    for(int j=0;j<128;j++)
   {
     if(myapv->wordid2sample(j)==i)
       data[j]=i;
   };



for(int i=1;i<=3;i++)
{
 
  mygem[i] = new cGEM(4);

  mygem[i]->AddAPVy(4*i-4,1,0); //0,4,8
  mygem[i]->AddAPVy(4*i-3,1,0); //1,5,9
  mygem[i]->AddAPVx(4*i-2,1,0); //2,6,10
  mygem[i]->AddAPVx(4*i-1,1,0); //3,7,11

  mygem[i]->AddAPVData(4*i-4,data);
  mygem[i]->AddAPVData(4*i-3,data);
  mygem[i]->AddAPVData(4*i-2,data);
  mygem[i]->AddAPVData(4*i-1,data);


     n[i]= mygem[i]->PushYdata();
     y[i] = mygem[i]->GetYdata();
     m[i]= mygem[i]->PushXdata();
     x[i] = mygem[i]->GetXdata();
  
}

 for(int t=1;t<=3;t++)

   {

printf("\n GEM[%d] APV Samples on Y axis\n", t);
 for (int i=0; i<n[t]; i++)
   {  printf("%d ", y[t][i]); };

 
 printf("\n GEM[%d]  APV Samples on X axis\n",t);
 for (int k=0; k<m[t]; k++)
   {  printf("%d ", x[t][k]); };

   }
 
    
 // for(int i=0;i<128;i++)
 //   for(int j=0;j<128;j++)
 //   {
 //    if(myapv->wordid2sample(j)==i)
 //       data[j]=i;
     

*/

}
   ////////////////////////////
   /*
      //////////////////////////
  asciireader = new cLumiGEMAsciiRead(12);

  //  InitLumiBranches_out();
  InitHitFinder();
  InitRMShistos();

  //////////////////////////////
  int cnt=0;
  if (asciifile)
    {
      asciireader->ReadBank(asciifile);
      for (int i=0; i<3; i++)// three GEMs per telescope
	{
	  int napvs = asciireader->GetNAPVs();
	  for (int apv=0; apv<napvs; apv++)
	    cnt += GEM[t][i]->AddAPVData(apv, asciireader->GetBankBuffer(apv));
	};
    }
  /////////////////////////
void Det_LumiGEM::InitLumiBranches_in()
{
  LUMI[0]=NULL;
  LUMI[1]=NULL;
  getBranchObject("LUMI0",(TObject**)&LUMI[0]);
  getBranchObject("LUMI1",(TObject**)&LUMI[1]);
  printf("LUMI[0] is %p\n", LUMI[0]);
  printf("LUMI[1] is %p\n", LUMI[1]);
}

void Det_LumiGEM::InitLumiBranches_out()
{
  //make new branch on output tree
  gemo = new LumiGEM();
  makeBranch("LumiGEMhits", (TObject **)&gemo);
  copyBranch("LumiTrigger");
}


   TFile *hfile = 0;
 
   //////////////



 ///////////////////////////////////////////////////////

  
 
  ///////////////////////////////////////////////////////
 



  //generate root files

   hfile = TFile::Open(filename,"RECREATE");
   TTree *tree = new TTree("T","HU Lab Raw Root Tree");
   tree->Branch("GEM0",&GEM0,"GEM0/I");
   tree->Branch("GEM1",&GEM1,"GEM1/I");



 



   char line[80];
   while (fgets(line,80,fp)) {
      sscanf(&line[0],"%d %d %d %d %d %d %d  %d %d %s %s",
      &Category,&Flag,&Age,&Service,&Children,&Grade,&Step,&Hrweek,&Cost,Division,Nation);
      tree->Fill();
   }
   if (print) tree->Print();
   tree->Write();

   fclose(fp);
   delete hfile;
   if (get) {
      //we come here when the script is executed outside $ROOTSYS/tutorials/tree
      hfile = TFile::Open(filename);
      return hfile;
   }
   return 0;
}
*/

/*


ASCII_to_root::ASCII_to_root(TTree *in, TTree *out, TFile *inf_, TFile *outf_, TObject *p)
  : Plugin(in, out, inf_, outf_, p)
{
  cout << "LumiGEM: Plugin constructor called" << endl;

  verbose=0;

  // generate six cGEM objects to build two telescopes:
  for (int t=0; t<2; t++)  // two telescopes
    for (int i=0; i<3; i++)// three GEMs per telescope
      {
	GEM[t][i] = new cGEM(4); // our GEMs have 4 APVs each
	rmshisto[t][i] = NULL;
      };

  // some hard-coded defaults for the configuration:
  valLMGaussFilterSigma = 1.5;
  valLMMinimumAmplitude =  25;
  valLMMinimumDistance  =   5;
  valLMMaximumNLM       =  10;
  valCFMinimumAmplitude = 100;
  valCFSigmaMin         = 0.4;
  valCFSigmaMax         = 1.5;
  valmake1dhits         =   0;
  valrepairbrokenstrips =   1;
  // triggerpatternOI      = 0x4;
  valAPVsyncDataRangeLow      = -65535;
  valAPVsyncDataRangeHigh     = +65535;
  valAPVoutOfSyncSampleThreshold   = 0;
  valAPVoutOfSyncEventThreshold    = 1;
  valAPVzeroSamplesEventThreshold  = 1;

  // Not event display mode by default
  vis = false;
  //asciifile = NULL;
  // trigger pattern of interest: by default we're interested in anything:
  triggerpatternOI = 0xff;

  // slowcontrol lumi info not yet connected:
  //  SClumi = NULL;
  // eventinfo branch not yet connected:
  // eventinfo = NULL;

  // Detector status undefined at the beginning:
  ProcessDetectorFlagOnly = false;
  DetectorStatus = LumiGEM::unknown;
  WriteDetectorFlagFilename[0]=0;

  LumiGUI = new Tab_LumiGEM();

  for (int i=0;i<250*3*2*2;i++)
    perchannelped[i]=0;

  someSpecHist=false;
  allSpecHist=false;
}

Det_LumiGEM::~Det_LumiGEM()
{
}
*/
