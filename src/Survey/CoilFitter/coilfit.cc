

#include <cmath>
#include <cstdio>
#include <vector>
#include "cminpack.h"
#include <fstream>
#include <cstdlib>
#define NP 100
#define NPARAMS (24+1+8+8)


double plx[8*NP],ply[8*NP],plz[8*NP],pdx[8*NP],pdy[8*NP],pdz[8*NP];
double oavgx=0;
double oavgz=0;
void precalc(int n, const double c[],const double par[])
{
  //#pragma omp parallel for
  for (int j=0;j<8;j++)
    {
      double avgx=0;
      double avgz=0;
      for (int i=0;i<n;i++)
	{
	  double phi=2*M_PI/(n)*i+par[32+j];
	  avgx+=cos(phi)*c[i];
	  avgz+=sin(phi)*c[i];
	}
      avgx/=n;
      avgz/=n;
      avgx-=oavgx;
      avgz-=oavgz;
      for (int i=0;i<n;i++)
	{
	  
	  // calculate the wire dx,dy
	  double phi=2*M_PI/(n)*i+par[32+j];
	  double phi2=2*M_PI/(n)*(i+1)+par[32+j];
	  
	  double rad,rad2;
	  rad=c[i];
	  rad2=c[(i+1) % n];
	  double x1=cos(phi)*rad;
	  double x2=cos(phi2)*rad2;
	  double z1=sin(phi)*rad;
	  double z2=sin(phi2)*rad2;
	  plx[i*8+j]=(x2-x1)*cos(par[j])*par[NPARAMS-1];
	  ply[i*8+j]=(x2-x1)*sin(par[j])*par[NPARAMS-1];
	  plz[i*8+j]=(z2-z1)*par[NPARAMS-1];
	  pdx[i*8+j]=((x1+x2)/2-avgx)*cos(par[j])+par[8+j];
	  pdy[i*8+j]=((x1+x2)/2-avgx)*sin(par[j])+par[16+j];
	  pdz[i*8+j]=((z1+z2)/2-avgz)+par[24+j];
	}
    }
}


void fastcalc(int n,double px,double py, double pz,double b[])
{
  b[0]=0;
  b[1]=0;
  b[2]=0;
  for (int i=0;i<n;i++)
    for (int j=0;j<8;j++)
      {
	double lx=plx[8*i+j];
	double ly=ply[8*i+j];
	double lz=plz[8*i+j];
	double dx=(px-pdx[i*8+j]);
	double dy=(py-pdy[i*8+j]);
	double dz=(pz-pdz[i*8+j]);

	double dist=dx*dx+dy*dy+dz*dz;
       	dist=1/(sqrt(dist)*dist);
	b[0]+=(ly*dz-lz*dy)*dist;
	b[1]+=(lz*dx-lx*dz)*dist;
	b[2]+=(lx*dy-ly*dx)*dist;
	}
}


class dp
{
public:
  double b[3];
  double co[3];
};


int iteration=0;
int ndata=0;
dp *data;


double par[NPARAMS];

 double testcurve[NP+NPARAMS];


int fitfunc(void *p, int m, int n, const double *x, double *fvec, int iflag)
{

  precalc(NP,&testcurve[NPARAMS],x);
#pragma omp parallel for 
  for (int index =0;index<ndata;index++)
      {
	double b[3];
	fastcalc(NP,data[index].co[0],data[index].co[1],data[index].co[2],b);
	double bsx=data[index].b[0];
	double bsy=data[index].b[1];
	double bsz=data[index].b[2];
	
	fvec[index*3]=bsx-b[0];
	fvec[index*3+1]=bsy-b[1];
	fvec[index*3+2]=bsz-b[2];
      }
  double res=0;

  if (iteration % 30 ==0)
  {
    for(int i=0;i<m;i++)
      res+=fvec[i]*fvec[i];
    //    printf ("Iteration %i, res %g, scale %g  ang  %g pos %g %g %g\n",iteration,res,x[32],x[0],x[8],x[16],x[24]);
    printf ("Iteration %i, res %g, scale %g  ang  %g pos %g %g %g\n",iteration,res,x[NPARAMS-1],x[0],x[8],x[16],x[24]);
  }
  iteration++;
  if (iteration>10000) return -1;
  return 0;

}



void preptest()
{

 double b[3];
 precalc(NP,&testcurve[NPARAMS],par);
 data=new dp[10000];
 // double w=0;
 double v=0;
 for (double w=-2;w<2;w+=0.1)
 // for (double v=-2;v<2;v+=0.1)
   for (double u=-2;u<2;u+=0.1)
     {
       dp d;
       fastcalc(NP,u,v,w,data[ndata].b);
       data[ndata].co[0]=u; data[ndata].co[1]=v;data[ndata].co[2]=w;
       ndata++;
     }
}

void loaddata(char *name)
{
  std::ifstream f(name);
  int nx,ny,nz;
  double sx,sy,sz;
  double dx,dy,dz;
  int ddata;
  f>>ddata>>nx>>ny>>nz>>sx>>sy>>sz>>dx>>dy>>dz;
  data=new dp[ddata];  double oldbx=0,oldby=0,oldbz=0;
  double mbx=0,mby=0,mbz=0;
  int xix,xiy,xiz;
  int yix,yiy,yiz;
  int zix,ziy,ziz;

  

  int i=0;
  for (int iz=0;iz<nz;iz++)
    for (int iy=0;iy<ny;iy++)
      for (int ix=0;ix<nx;ix++)
	{
	  double bx,by,bz;
	  f>> bx>>by>>bz;
	  if (fabs(bx)>4900 ||fabs(bz)>4900 ||fabs(bz)>4900 ) continue;
	  if ( ( fabs(iy-20)<=1)  && (iz<80) &&  (fabs(ix-50)<40) && (fabs(ix-50)>5) )
	    {
	      data[i].co[0]=(sx+ix*dx)/1000;
	      data[i].co[1]=(sy+iy*dy)/1000;
	      data[i].co[2]=(sz+iz*dz)/1000;
	      data[i].b[0]=bx/1000;
	      data[i].b[1]=by/1000;
	      data[i].b[2]=bz/1000;
	      i++;
	    }
	}
  ndata=i;
  f.close();
  
}


int main(int argc, char ** argv)
{

 //first, 8 start angles; x/ y shift of radius 2
 for (int i=0;i<8;i++)
   {
     par[i]=2*i*M_PI/8+M_PI/8;
     par[8+i]=cos(par[i])*1.25;
     par[16+i]=sin(par[i])*1.25;
     par[24+i]=0.75;
     par[32+i]=0;
   }
 par[NPARAMS-1]=-0.18;
  loaddata(argv[1]);


 //setup done;
 printf("Preparing Testcurve\n");


 std::ifstream fa(argv[2]);

 for (int i=0;i<NP;i++)
   {
     double dummy;
       fa>> dummy>>  testcurve[NPARAMS+i];

      double phi=2*M_PI/(NP)*i;
      oavgx+=cos(phi)*testcurve[NPARAMS+i];
      oavgz+=sin(phi)*testcurve[NPARAMS+i];
   }
 oavgx/=NP;
 oavgz/=NP;


 for  (int i=0;i<NPARAMS;i++)
   testcurve[i]=par[i];

// preptest();


 //let's fit
 int *iwork=new int[NP+NPARAMS];
 double *dwork=new double[10000000];
 double *fvec =new double[ndata*3];

 printf("Fitting\n");
 // int erg=lmdif1(fitfunc,testcurve,ndata*3,NPARAMS,testcurve,fvec,1e-6,iwork,dwork,10000000);


 int index=0;
 FILE *f=fopen("output.txt","w");
 precalc(NP,&testcurve[NPARAMS],testcurve);
 for (int i=0;i<ndata;i++)
 {
   double b[3];
       fastcalc(NP,data[i].co[0],data[i].co[1],data[i].co[2],b);
       double bsx=data[i].b[0];
       double bsy=data[i].b[1];
       double bsz=data[i].b[2];
       fprintf(f,"%g %g %g  %g %g %g  %g %g %g\n",data[i].co[0],data[i].co[1],data[i].co[2],bsx,bsy,bsz,b[0],b[1],b[2]);
     }

 fprintf(f,"\n\n\n");
 for (int i=0;i<NP;i++)
   fprintf(f,"%i %g %g\n",i,testcurve[i+NPARAMS]*cos(i*2*M_PI/NP),testcurve[i+NPARAMS]*sin(i*2*M_PI/NP));

 fprintf(f,"\n\n\n");
 for (int i=0;i<NPARAMS;i++)
   fprintf(f,"%i %g %g\n",i,par[i],testcurve[i]);

 fclose(f);


return 0;
}
