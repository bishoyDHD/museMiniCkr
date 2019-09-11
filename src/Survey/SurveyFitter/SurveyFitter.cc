#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
#include "cminpack.h" 
#include <boost/algorithm/string.hpp>
#include<map>
#include <vector>
using namespace std;

#ifdef __APPLE__
#include <vecLib/vecLib.h>
typedef __CLPK_integer LPK_INT;
#else
#include "cblas.h"
typedef int LPK_INT;
extern "C"
{

/* Subroutine */ int dsptrf_(char *uplo, int *n, double *ap, int *
        ipiv, int *info);
/* Subroutine */ int dsptri_(char *uplo, int *n, double *ap, int *
        ipiv, double *work, int *info);

}
#endif

class datapoint
{
public:
  int gid,pid;
  double v[3];
  datapoint(int g, int n, double v1,double v2,double v3)
  {
    gid=g;
    pid=n;
    v[0]=v1;
    v[1]=v2;
    v[2]=v3;
  }
};

class rotation
{
public:
  double x,y,z;
  double mat[9];
};


//global constants
int ngroups; // number of groups
int nnames; // number of pointnames

std::map< std::string , int > groups;
std::map< std::string , int > names;
std::vector< datapoint> data;

std::vector<rotation*> rotcache;


void loadASCII(string filename)    
{
  ngroups=0;
  nnames=0;
  ifstream f(filename.c_str());
  while (f.good())
    {
      int gid,nid;
      char line[1024];
      f.getline(line,1024);
      char group[1024],net[1024],point[1024],date1[1024],date2[1024],active[1024];
      double azi,ele,distance,offset1,offset2;
      int n=sscanf(line,"%s %s %s %lg %lg %lg %lg %lg %s %s %s",group,net,point,&azi,&ele,&distance,&offset1,&offset2,date1,date2, active);
      if (n==11 )
	{
	  string g(group);
	  boost::to_upper(g);

	  string n(point);
	  boost::to_upper(n);
	  if (groups.find(g)==groups.end())
	    groups[g]=ngroups++;
	  gid=groups[g];
	  if (names.find(n)==names.end())
	    names[n]=nnames++;
	  nid=names[n];
	  azi*=M_PI/180;
	  ele*=M_PI/180;
	  //dist=float(distance)#+float(offset1)#+float(offset2)
	  data.push_back(datapoint(gid,nid,distance*cos(azi)*sin(ele),distance*sin(azi)*sin(ele),distance*cos(ele)));
	}
    }
}


double *params;
int offset;

void loadcache(string filename)
{

  ifstream f(filename.c_str());
  
  while (f.good())
    {
      char line[1024];
      f.getline(line,1024);
      double a,b,c,d,e,f;
      char name[1024];
      int n=sscanf(line,"%s %lg %lg %lg %lg %lg %lg",name,&a,&b,&c,&d,&e,&f);
      if (n==7)
	{
	if (groups.find(name)!=groups.end())
	  {
	    params[groups[name]*6+0]=a;
	    params[groups[name]*6+1]=b;
	    params[groups[name]*6+2]=c;
	    params[groups[name]*6+3]=d;
	    params[groups[name]*6+4]=e;
	    params[groups[name]*6+5]=f;
	  }
	}
      else
	{
	  n=sscanf(line,"%s %lg %lg %lg",name,&a,&b,&c);

	  if (n==4)
	    if (names.find(name)!=names.end())
	      {
		params[names[name]*3+offset+0]=a;
		params[names[name]*3+offset+1]=b;
		params[names[name]*3+offset+2]=c;
	      }
		
	}
    }
}


void makerot(int gid,double x,double y,double z)
{
  rotation *rot =rotcache[gid];
  if (!(rot->x==x && rot->y==y && rot->z==z))
    {
      double s0=sin(x);
      double s1=sin(y);
      double s2=sin(z);
      double c0=cos(x);
      double c1=cos(y);
      double c2=cos(z);
      rot->mat[0]=c1*c2-s0*s1*s2;
      rot->mat[1]=-c2*s0*s1-c1*s2;
      rot->mat[2]=c0*s1;
      rot->mat[3]=c0*s2;
      rot->mat[4]=c0*c2;
      rot->mat[5]=s0;
      rot->mat[6]=-c2*s1-c1*s0*s2;
      rot->mat[7]=-c1*c2*s0+s1*s2;
      rot->mat[8]=c0*c1;
    }

}



void rot(int gid,double vec[3],double *tgt)
{
  cblas_dgemv(CblasRowMajor,CblasNoTrans,3,3,1,rotcache[gid]->mat,3,vec,1,0,tgt,1);
}

void calc(datapoint d, double *params,double *ret,double scale=1)
{
  int soff=d.gid*6;
  int poff=offset+d.pid*3;
  rot(d.gid,d.v,ret);
  for (int i=0;i<3;i++)
    ret[i]+=params[soff+i]-scale*params[poff+i];
}


int ncons=6+2+2;

int tid[8];
int iteration=0;

int chifunc(void *p,int m,int n,const double *x,double *fvec,int iflag)
{
  for (int i=0;i<ngroups;i++)
    makerot(i,x[i*6+3],x[i*6+4],x[i*6+5]);
  
  int ind=0;
  for(std::vector<datapoint>::iterator iter=data.begin();iter!=data.end();iter++)
    {
      int soff=iter->gid*6;
      int poff=offset+iter->pid*3;
      for (int i=0;i<3;i++)
	fvec[ind+i]=x[soff+i]-x[poff+i];
      cblas_dgemv(CblasRowMajor,CblasNoTrans,3,3,1,rotcache[iter->gid]->mat,3,iter->v,1,1,&fvec[ind],1);
      ind+=3;
    }

  // target constraints
  //beamline y=0;
  for (int i=0;i<6;i++)
    fvec[ind++]=x[tid[i]+1];

  // z around flange
  fvec[ind++]=x[tid[0]+2]+x[tid[1]+2];
  fvec[ind++]=x[tid[4]+2]+x[tid[5]+2];
  //   pos in x
  fvec[ind++]=x[tid[2]]+599.2;
  fvec[ind++]=x[tid[3]]-515.102;

  if (iteration %200==0) 
    {
      cout.precision(10);
      cout<<iteration<<" "<<x[0]<<" "<<x[1]<<" "<<x[2]<<" "<<enorm(ind,fvec)*enorm(ind,fvec)<<endl;
    }
  iteration++;
   return 0;
 }



void writecache(string filename,double *p)
{

  ofstream f(filename.c_str());

  for (std::map<string,int>::iterator iter=groups.begin();iter!=groups.end();iter++)
    {
      f<<iter->first;
      for (int i=0;i<<6;i++) f<<" "<<p[iter->second*6+i];
      f<<endl;
    }
  f<<endl;
  for (std::map<string,int>::iterator iter=names.begin();iter!=names.end();iter++)
    {
      f<<iter->first;
      for (int i=0;i<<3;i++) f<<" "<<p[offset+iter->second*3+i];
      f<<endl;
    }
  f.close();
}

int main(int argc,char **argv)
{
  loadASCII(argv[1]);
  params=new double[nnames*3+ngroups*6];
  offset=ngroups*6;
  
  loadcache(argv[2]);
  rotcache.resize(ngroups);
  for (int i=0;i<ngroups;i++)
    rotcache[i]=new rotation();


  // for target constraints
  tid[0]=names["T1"];  tid[1]=names["T2"];  tid[2]=names["T3"];  tid[3]=names["T4"];
  tid[4]=names["T5"];  tid[5]=names["T6"];  tid[6]=names["T8"];  tid[7]=names["T10"];
  for (int i=0;i<8;i++) tid[i]=offset+3*tid[i];

  int nparams=ngroups*6+nnames*3;
  int ndata=data.size()*3+ncons;
  double *fvec=new double[ndata];
  int *iwork=new int[ndata];
  int sdwork=ndata*nparams+5*nparams+ndata;
  double *dwork=new double[sdwork];

  int r=lmdif1(chifunc,NULL,ndata,nparams ,params,fvec,1e-7,iwork,dwork,sdwork);
  
  writecache(argv[3],params);
  std::cout<<"Return "<<r<<" Error:"<<enorm(ndata,fvec)<<endl;
}
