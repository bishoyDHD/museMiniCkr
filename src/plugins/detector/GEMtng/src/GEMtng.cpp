#include <GEMtng.h>

#include<iostream>
#include<cmath>

#include "telescope.h"


GEMtng::GEMtng(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):Plugin(in,out,inf_,outf_,p)
{
};

GEMtng::~GEMtng()
{
};


Long_t GEMtng::dH_allRaw()
{

  for (int i=0;i<2;i++)
    telescopes[i]->dH_allRaw();
  return 0;
}


Long_t GEMtng::dH_planeRaw()
{

  for (int i=0;i<2;i++)
    telescopes[i]->dH_planeRaw();
  return 0;
}

Long_t GEMtng::dH_planeProcessed()
{

  for (int i=0;i<2;i++)
    telescopes[i]->dH_planeProcessed();
  return 0;
}

Long_t GEMtng::dH_planeSum()
{

  for (int i=0;i<2;i++)
    telescopes[i]->dH_planeSum();
  return 0;
}

Long_t GEMtng::dH_planeHits()
{

  for (int i=0;i<2;i++)
    telescopes[i]->dH_planeHits();
  return 0;
}


Long_t GEMtng::dH_stackHits()
{

  for (int i=0;i<2;i++)
    telescopes[i]->dH_stackHits();
  return 0;
}


Long_t GEMtng::dH_dxdy()
{
  for (int i=0;i<2;i++)
    telescopes[i]->dH_dxdy();
  return 0;
}



Long_t GEMtng::startup()
{
  LUMI[0]=NULL;
  LUMI[1]=NULL;

  getBranchObject("GEM0",(TObject**)&LUMI[0]);
  getBranchObject("GEM1",(TObject**)&LUMI[1]);

  printf("LUMI[0] is %p\n", LUMI[0]);
  printf("LUMI[1] is %p\n", LUMI[1]);
  

  telescopes[0]=new telescope(this,"GEMtng/OUT",1,0,3,2,5,4,7,6,9,8,11,10,2180.3-1846.2,2600.7-2180.3,390);//about 39cm upstream of first GEM
  telescopes[1]=new telescope(this,"GEMtng/IN",9,8,11,10,5,4,7,6,1,0,3,2,  2176.5-1840.7, 2597.2- 2176.5,1840.7-2597.2-450 ); //about 45 cm downstream of last GEM

  h_xcorrelation=dH2("GEMtng/xcorr","Correlation: x_out vs. x_in;x_in;x_out",100,-100,100,100,-100,100);
  h_ycorrelation=dH2("GEMtng/ycorr","Correlation: y_out vs. y_in;y_in;y_out",100,-100,100,100,-100,100);


  return 0;
}


Long_t GEMtng::process()
{

  // now we analyze both telescopes
  for (int i=0;i<2;i++)
    telescopes[i]->analyze((unsigned int *) LUMI[i]->data);  //can not move the data pointer to constructor - will change during readout


  for (unsigned int u=0;u<telescopes[0]->tracks.size();u++)
    for (unsigned int v=0;v<telescopes[1]->tracks.size();v++)
      {
	h_xcorrelation->Fill(telescopes[1]->tracks[v].x,telescopes[0]->tracks[u].x);
	h_ycorrelation->Fill(telescopes[1]->tracks[v].y,telescopes[0]->tracks[u].y);

	//todo make cuts and save good track combinations to file
	
      }

  return 0;
}

Long_t GEMtng::finalize()
{
  for (int i=0;i<2;i++)
    telescopes[i]->finalize();
  return 0;
}


Long_t GEMtng::cmdline(char *cmd)
{
  //add cmdline handling here

  return 0; // 0 = all ok
};


extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new GEMtng(in,out,inf_,outf_,p);
}
}


ClassImp(GEMtng);

