#ifndef __PLANE_H_
#define __PLANE_H_
#include "Plugin.h"
#include <string>
#include "TH1.h"
#include <vector>

class planehit
{
 public:
  int strip; //strip index;
  double hitscore; //score from hit finding
  double pos; //later
  double height; 
 planehit(int s,double hs):strip(s),hitscore(hs){};
};


class plane
{
  Plugin *plugin;
  std::string prefix;
  TH1D *h_raw;
  TH1D *h_processed;
  TH1D *h_sum;
  TH1D *h_chi;
  TH1D *h_striphits;
  unsigned int * raw;
  double decoded[250]; // this is the decoded information
  double baseline[250]; // the average baseline
  double weight[11];
  double dw;
  unsigned int lai0,lai1; //Logical Apv/ADC Index 
  unsigned int count;

 public:
  void analyze(unsigned int *);
  void dH_raw();
  void dH_processed();
  void dH_sum();
  void dH_1dhits();

  void startup();
  void finalize();

  std::vector<planehit> hits;
  plane(Plugin *p,std::string pre,unsigned int l0,unsigned int l1);
  ~plane(){};
};

#endif 
