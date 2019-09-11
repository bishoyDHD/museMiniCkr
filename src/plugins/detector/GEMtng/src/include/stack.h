#ifndef __GEMSTACK_H_
#define __GEMSTACK_H_
#include "Plugin.h"
#include <string>
#include "plane.h"
#include "TH2.h"
#include "TH1.h"
#include <vector>

class stackhit
{
 public:
  int stripx,stripy; 
 stackhit(int x,int y):stripx(x),stripy(y){};
};


class stack
{
  Plugin *plugin;
  std::string prefix;
  TH2D *h_hitscore2d;
  TH1D *h_hitscore1d;
  TH2D *h_striphits2d; 
  TH2D *h_striphits2dbig;
  plane *planes[2];

 public:
  void analyze(unsigned int *);
  void dH_2dhitmap();
  void dH_raw();
  void dH_processed();
  void dH_sum();
  void dH_1dhits();
  void dH_2dhits();
  
  void finalize();
  std::vector<stackhit>hits;

 stack(Plugin *p,std::string pre,unsigned int x0,unsigned int x1,unsigned int y0,unsigned int y1):plugin(p),prefix(pre),h_hitscore2d(NULL),h_hitscore1d(NULL),h_striphits2d(NULL){
    planes[0]=new plane(p,pre+"/X",x0,x1);
    planes[1]=new plane(p,pre+"/Y",y0,y1);
  };
  ~stack(){};
};


#endif
