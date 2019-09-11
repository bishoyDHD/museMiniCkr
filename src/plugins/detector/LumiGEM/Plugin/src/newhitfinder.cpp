#include <Det_LumiGEM.h>

#include <iostream>
#include <stdio.h>
#include <cmath>
#include <algorithm>

//using namespace std;

std::vector<unsigned int> Det_LumiGEM::localMax(std::vector<double> dat, std::vector<double> thresh)
{
   std::vector<unsigned int> max;
   
   // Check the front end
    if (dat[1]>dat[0] && dat[0]>thresh[0])
      max.push_back(0);

   // Run over the vector
      for (unsigned int j=1; j<(dat.size()-1); j++)
	if ((dat[j]>=dat[j-1] && dat[j]>=dat[j+1]) && dat[j]>thresh[j]){
	  max.push_back(j);
	  //printf("Brian - local maxima %d %5.2lf\n",j,thresh[j]);
	};

   if (dat[dat.size()-1] > dat[dat.size()-2] && dat[dat.size()-1] > thresh[dat.size()-1])
      max.push_back(dat.size()-1);  

   //  for (unsigned int a=0; a<max.size(); a++) printf("Brian - local maxima %d %8.2lf\n",max[a],dat[max[a]]);  

   return max;
};

// Overloaded call for local max for a constant threshold value

std::vector<unsigned int> Det_LumiGEM::localMax(std::vector<double> dat, double thresh)
{
   std::vector<double> tv(dat.size(),thresh);
   
   std::vector<unsigned int> max = localMax(dat,tv);

   return max;
};

// Hit-finding routines
double Det_LumiGEM::meanHit(unsigned int ind, std::vector<double> dat, LumiGEMhit *hit=NULL)
{
   double pos = 0;

   unsigned dind = 0;
   unsigned uind = 0;
   
   if (ind<3)
      dind = 0;
   else
      dind = ind-2;

   if (ind>(dat.size()-3))
      uind = dat.size()-1;
   else
      uind = ind+2;

   // Sanity check
   if (uind<=dind)
   {
     printf("uind=%d, dind=%d \n",uind,dind);
      printf("\n\nCrap mean position request! ABORT!\n\n");
      exit(0);
   }

  double charge=0;
   double num = 0;
   double den = 0;

   for (unsigned int j=dind; j<(uind+1); j++)
   {
     // if (dat[j]==-100000) dat[j]=0;
   
	charge += dat[j];
      num += ((double)j)*dat[j];
      den += dat[j];
   }
 
   double ampl;
   double mean;
  double sigma = 0.;

  pos = num/den;
  mean = num/den;
  //ampl =dat[ind];
   ampl =dat[mean];
  // printf("charge %d %5.3lf %5.3lf %5.3lf\n",ind, dat[ind],mean,dat[mean]);
	  if (hit)
	    {	    
	      hit->ampl      = ampl;
	      hit->xl        = mean;
	      hit->yl        = -1;
	      hit->xlerr     = -1;
	      hit->sigma     = sigma;
	      hit->charge    = charge;
	      hit->chargeerr = 0;
	      hit->quality   = 1;
	      //printf("charge %5.3lf \n",hit->charge);
  return pos;
	    };
}

unsigned int Det_LumiGEM::maxind(std::vector<double> const &dat, std::vector<bool> const &mask)
{
   unsigned int ind = 999999;
   for (unsigned int j=0; j< dat.size(); j++)
      if (mask[j])
      {
		//printf("first index %d %d %5.2lf \n",j,dat.size(),dat[j]);
         ind = j;
         break;
      }

   if (ind==999999)
   {
     printf("\n\nERROR: NO VALID DATA TO FIND MAXIMUM!\n\n");
      exit(0);
   }
    //printf("starting index %d %d  \n",ind,dat.size());  

   for (unsigned int j=ind+1; j<dat.size(); j++)
     if (mask[j])
       if (dat[j] > dat[ind]) {
	   //printf("find maximum index %d %d %5.2lf  %5.2lf\n",j,*dat.size(),*dat[j],*dat[ind]);
	 ind = j;	 
       };
   return ind;
};


