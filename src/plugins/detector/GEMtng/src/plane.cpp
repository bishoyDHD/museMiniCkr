#include "plane.h"
#include "TFile.h"
#include "TROOT.h"
#include <cstdlib>
#include <cmath>

void plane::dH_raw()
{
  h_raw=plugin->dH1((prefix+"/raw").c_str(),"Raw strip data",250,-0.5,249.5);

}

void plane::dH_processed()
{
  h_processed=plugin->dH1((prefix+"/processed").c_str(),"Processed strip data",250,-0.5,249.5);
  h_chi=plugin->dH1((prefix+"/chi").c_str(),"Pseudo chi of possible hits",1000,0,12);
}

void plane::dH_1dhits()
{
h_striphits=plugin->dH1((prefix+"/striphits").c_str(),"Strips with hit detected",250,-0.5,249.5);
}

void plane::dH_sum()
{
  h_sum=plugin->dH1((prefix+"/sum").c_str(),"Processed strip data",250,-0.5,249.5);
}

plane::plane(Plugin *p,std::string pre,unsigned int l0,unsigned int l1):plugin(p),prefix(pre),h_raw(NULL),h_processed(NULL),h_chi(NULL),h_sum(NULL),h_striphits(NULL),lai0(l0),lai1(l1),count(0)
{
  TFile *f=new TFile(Form("%s/.muse/shared/GEM/gembias.root",getenv("COOKERHOME")));
  TH1D *avg=NULL;
  if (f)
    avg=(TH1D*)f->Get((prefix+"/sum").c_str());
  if (avg)
    for (int i=0;i<250;i++)
      baseline[i]=avg->GetBinContent(i+1);
  else
    for (int i=0;i<250;i++)
      baseline[i]=0;

  if (f)
    {
    f->Close();
    delete f;
    }
  double s=0;
  for (int i=-5;i<=5;i++)
    {
      double w=exp(-0.5*(i*i));
      weight[i+5]=w;
      s+=w;
      }
  dw=0;
  for (int i=-5;i<=5;i++)
    {
      weight[i+5]/=s;
      dw+=weight[i+5]*weight[i+5];
    }
  if (p->outfile)
    p->outfile->cd();
  else
    gROOT->cd();
};


inline double min(double x,double y)
{
  return x<y?x:y;
}

void plane::analyze(unsigned int * rawdata)
{
  raw=rawdata;
  hits.clear();

  //todo: check header


  //copy and decode data
  for (int s=0;s<128;s++) 
    {
      int pin= ( 32*(s%4)+8*int(s/4)-31*int(s/16) );
      if (pin>5)  //skip the first 6 for the first apv
	decoded[pin-6]=raw[lai0*130+1+s];
      decoded[122+pin]=raw[lai1*130+1+s];
    }

  if (h_raw)
    for (int s=0;s<250;s++)
      h_raw->SetBinContent(s+1,decoded[s]);

  //notch filter
  for (int i=1; i<249; i++)
    if (fabs(decoded[i-1]-decoded[i+1])<30 && fabs(decoded[i]-(decoded[i+1]+decoded[i-1])*0.5)>30) decoded[i]=(decoded[i+1]+decoded[i-1])*0.5;

  if (h_sum)
    for (int s=0;s<250;s++)
      h_sum->Fill(s,decoded[s]);
 
  double temp[250];
  //subtract average baseline
  for (int i=0; i<250; i++)
    temp[i]=decoded[i]-baseline[i];

  //   // gaussian blur (with wrap around)
  // for (int i=0; i<250; i++)
  //   {
  //     double t=0;
  //     for (int s=-5;s<=5;s++)
  // 	t+=temp[(250+i+s) % 250]*weight[s+5];
  //     decoded[i]=t;
  //   }


  // find chi^2 of gauss at this position
  double last=0;
  bool upwards=false;
   for (int i=0; i<250; i++)
    {
      double t=0;
      double min=10000;
      for (int s=-5;s<=5;s++)
  	if (min>temp[(250+i+s) % 250]) min=temp[(250+i+s) % 250];
      for (int s=-5;s<=5;s++)
  	t+=(temp[(250+i+s) % 250]-min)*weight[s+5];
      t/=dw;
      double chi=0;
      for (int s=-5;s<=5;s++)
	{
	  double c=(((temp[(250+i+s)  % 250]-min)-weight[s+5]*t));
	  chi+=c*c;
	  }
      decoded[i]=t*t*t*t/chi; 
      //scan for local maxima:
      if (decoded[i]<last && upwards) // found a local maxima
	{
	  upwards=false;
	  if (last>=exp(2*6.5))//6.5)
	    {
	      hits.push_back(planehit(i-1,log(decoded[i-1])/2));
	    if (h_striphits )
	      h_striphits->Fill(i-1);
	    }
	  if (h_chi)
	    h_chi->Fill(last);

	}
      if (decoded[i]>last) upwards=true;
      last=decoded[i];
    }


  if (h_processed)
    for (int s=0;s<250;s++)
      h_processed->SetBinContent(s+1,decoded[s]);

  count++;
}

void plane::finalize()
  {
    if (h_sum)
      h_sum->Scale(1.0/count);
  }
  
