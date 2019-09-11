#ifndef __TELESCOPE_H__
#define __TELESCOPE_H__
#include "Plugin.h"
#include <string>
#include "stack.h"
#include <vector>

class gemtrack
{
 public:
  double x,y; //position on projected plane;
  double slope_x,slope_y;// slope on that plane
 gemtrack(double x_,double y_, double sx, double sy):x(x_),y(y_),slope_x(x),slope_y(y){};
};



class telescope
{
 private:
  Plugin *plugin;
  std::string prefix;
  TH1D *h_alldata;
  TH1D *h_dx,*h_dy;
  TH2D *h_dxdy;
  TH2D *h_projected;
  TH2D *h_acceptedU,*h_acceptedM,*h_acceptedD;
  unsigned int * raw;
  double dist_UM,dist_MD,plane;
  stack *stacks[3];
  
 public:
  void analyze(unsigned int * rawdata);
  void dH_allRaw();
  void dH_planeRaw();
  void dH_planeProcessed();
  void dH_planeSum();
  void dH_planeHits();
  void dH_stackHits();
  void dH_dxdy();
  void finalize();

  std::vector<gemtrack> tracks;

 telescope(Plugin *p,std::string pre,unsigned int ux0,unsigned int ux1,unsigned int uy0,unsigned int uy1,unsigned int mx0,unsigned int mx1,unsigned int my0,unsigned int my1,unsigned int dx0,unsigned int dx1,unsigned int dy0,unsigned int dy1,double distum,double distmd,double pl):plugin(p),prefix(pre),h_alldata(NULL),h_dx(NULL),h_dy(NULL),dist_UM(distum),dist_MD(distmd),plane(pl){
    stacks[0]=new stack(p,pre+"/U",ux0,ux1,uy0,uy1);
    stacks[1]=new stack(p,pre+"/M",mx0,mx1,my0,my1);
    stacks[2]=new stack(p,pre+"/D",dx0,dx1,dy0,dy1);
  };
  ~telescope(){};
};

#endif
