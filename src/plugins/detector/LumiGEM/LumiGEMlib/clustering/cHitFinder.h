/**********************************/
/*                                */
/*           cHitFinder           */
/*                                */
/* Locate hits in APV data        */
/*                                */
/* 2011, J. Diefenbach, Hampton U */
/*                                */
/**********************************/
//
// 11.07.2011
//   Files and empty c'tor, d'tor created
// 25.09.2011
//   Made some first hit finding code compile
// 18.10.2011
//   Checked in into OLYMPUS offline repository. The following functionality
//   is implemented now:
//    - 
//


#ifndef _cHitFinder_h_
#define _cHitFinder_h_

#include <cstddef>
#include <math.h>

#include "TF1.h"
#include "TGraph.h"
#include "TH1.h"
#include "TMath.h"

#include "lumigemtree.h"
//#include "../../Plugin/src/include/Det_LumiGEM.h"

//#define NOPEAKFIT


template <class T>
class mypair {
    T values [2];
  public:
    mypair (T first, T second)
    {
      values[0]=first; values[1]=second;
    }
};

class cHitFinder
{
  ////////////////////////////////////////////////////////////
 public:
  cHitFinder(int nx, int ny);
  ~cHitFinder();

  ////////////////////////////////////////////////////////////
  enum eMethod
  {
    mGaussFit=0,
    mGaussTemplate
  };
  enum ePreProcess
  {
    ppRemoveLinearBaseline=0,ppDoNothing=1
  };
  ////////////////////////////////////////////////////////////
  struct sClusterList
  {
    int size;
    double chisquare, charge, quality,
      loc, locerr,
      ampl, emplerr,
      sigma, sigmaerr;
  };
  ////////////////////////////////////////////////////////////
  double **GetXarray();
  double **GetYarray();
  int      SetPreProcessMethod(ePreProcess m, char *options=NULL);
  int      PreProcess();
  // mask the dead channels
  //  int deadchannels[250*2*3*2];
  // Long_t Maskdeadchannels(int id, int ,int ,int,int,int);

  int ChargeFaking(const std::vector <LumiGEMhit> &oneDlist, std::vector <LumiGEMhit> *twoDlist);
  int ChargeSharing(const std::vector <LumiGEMhit> &oneDlist, std::vector <LumiGEMhit> *twoDlist);
  void PrintHitlist(const std::vector <LumiGEMhit> &hitlist);

  // template<class T> void sort(Array<T>& v) { }
  // template void sort(Array<char>&); // explicit instantiation
  
  /* template <typename T> */
  /*   int    RemoveLinearBaseline(int n, T *dat); */
  template <typename T> int RemoveLinearBaseline(int n, T *dat)
    //!
    //! Fit equation of a line f=mx+b to the APV frame samples
    //! and subtract the fit function from the data then.
    //!
    {
      // dump data into a TGraph object:
      T *index = new T[n];
      for (int i=0; i<n; i++)
	index[i] = (T)i;
      TGraph *gtmp  = new TGraph(n, index, dat);
      // fit linear baseline:
      TF1    *fpol1 = new TF1("fpol1", "pol1", 0, n-1);
      gtmp->Fit(fpol1, "QN", "");
      // get parameters of fpol1(x) = m*x + b
      double b = fpol1->GetParameter(0);
      double m = fpol1->GetParameter(1);
      // remove linear baseline from data array:
      for (int i=0; i<n; i++)
	{
	  double baseline = m*double(index[i])+b;
	  dat[i] -= baseline;
	};
      // clean up a bit:
      delete   fpol1;
      delete   gtmp;
      delete[] index;
      return 0;
    };

  template <typename T> int RemoveLinearBaseline(int *nsampleslist, T *dat)
    {
      if (nsampleslist==NULL)
	{
	  printf(" cHitFinder::RemoveLinearBaseline(): WARNING - not removing baseline - nsampleslist=(nil)!\n");
	  return 0;
	};
      int result=0, samplecnt=0;
      for (int i=0; nsampleslist[i]>=0; samplecnt+=nsampleslist[i], i++)
	result += RemoveLinearBaseline(nsampleslist[i], dat+samplecnt);
      return result;
    };

  //template <typename T>
  //  int    GaussFilter(int n, T *dat, double sigma, int verbosity=0);
  template <typename T> int GaussFilter(int n, T *dat, double sigma, int verbosity=0)
    //! 
    //! Apply Gaussian filter to the data.
    //! Detailed description below.
    //!
    {
      T *dest = new T[n];
      return GaussFilter(n, dat, sigma, dest, verbosity);
      delete[] dest;
    }
  template <typename T> int GaussFilter(int n, T *dat, double sigma, T *dest, int verbosity=0)
    //!
    //! Apply Gaussian filter to the data.
    //! The idea is that the clusters should look Gaussian and therefore
    //! will not suffer from filtering. Noise should be somewhat reduced.
    //! The Gaussian filter will NOT change the peak positions of Gaussian
    //! shaped clusters.
    //! Note: This code has been taken from "TComptonAnalyse" and adapted.
    //!
    {
      // use "src" and "dest" for better readability:
      T *src  = dat;
      double filterwidth=4.0; // half-width of filter in sigma
      const double s2p = sqrt(2.0*3.141592654);
      int numbins=n;
      for (int bin=0; bin<n; bin++)
	{
	  //	  if (dat[bin]==-100000) {
	  //	    src[bin]=0;
	  //	    continue;	    
	  //	  };

	  double val = 0.0, norm = 0.0, gausval = 0.0;
	  for (int k=bin-int(sigma*filterwidth); k<=bin+int(sigma*filterwidth); k++)
	    {
	      // printf(" go here %d %d %d\n",bin,k,int(sigma*filterwidth));
	      if (k<0) continue;
	      if (k>=numbins) break;
	      if (src[k]==-100000) continue;
	      gausval = 1./s2p*exp( -0.5*pow((k-bin)/sigma,2.0) );
	      val  += src[k] * gausval;
	      norm += gausval;
	    };
	  //printf("SetBinContent(%d) %.1lf at %p\n", bin, val, (*dest));
	  dest[bin] = T(val/norm);
	  if (verbosity)
	    {
	      printf("\r %3.1lf %% done... ", 100.0*double(bin)/double(numbins));
	      fflush(stdout);
	    };
	};
      // To conserve transparency, delete dest after copying it to dat:
      for (int bin=0; bin<n; bin++)
	//	printf("go here %d %d\n",bin,dat[bin]);

	if (dat[bin]!=-100000) dat[bin] = dest[bin];
            //dat[bin] = dest[bin];

      //  printf(" hitfinder %d  %d \n", bin, dat[bin]);
      //delete[] dest;
      return 0;
    }
  double clusterminsigma, clustermaxsigma;
  void SetClusterSigmaRange(double min, double max)
  {
    clusterminsigma = min;
    clustermaxsigma = max;
  }
  template <typename T> int CheckClusterCandidate(int locmax, int threshold, T *dat, T *rawdat, int nsamples, LumiGEMhit *hit=NULL, int verbosity=0)
    //! Check a cluster candidate by comparing it to a reference cluster waveform.
    //! A localmaximum "locmax" from the data array "dat" needs to be provided.
    {
      double maxchisqr = 1.0;
      int highthreshold=80000;
         if (locmax<3)          return -1; // take care of array limits!
        if (locmax>nsamples-3) return -1; // take care of array limits!
        if (threshold<=0.) return -1;

      if (verbosity>2) printf("cHitFinder::CheckClusterCandidate() checking locmax at %d\n", locmax);
      // Let's check if the vicinity looks Gaussian or not:
      if (verbosity>1) printf("cHitFinder::FindHitCandidate(): calculating chisquare\n");

for (int sa=0; sa<nsamples; sa++)
    {
      // if (rawdat[sa]==-100000) printf("dead channels %d %d %d\n",sa,rawdat[sa],dat[sa]);
    };
  
      if (dat[locmax]<highthreshold) // be efficient for "monster" clusters ;)
	{
	  gchisqr = ChiGauss(nsamples, dat, locmax, NULL, NULL, verbosity);
	  if (verbosity>1)
	    printf("cHitFinder::CheckClusterCandidate(): Candidate @%d has chisqr=%lf\n", locmax, gchisqr);
	  if (gchisqr>maxchisqr)
	    return -1;
	};
      //  if (locmax>139 && locmax<149) printf("dead channels %i\n",locmax);

      // Explicitly fit a Gaussian to the candidate:
      double ampl  = dat[locmax];
      double mean  = locmax;
      double sigma = 0.;
      TH1I *dath = new TH1I("dath", "dath", nsamples, -0.5, nsamples-0.5);
      for (int sa=0; sa<nsamples; sa++)
	{
	  // if (sa==130) printf("dead channels %d %d %d\n",sa,rawdat[sa],dat[sa]);
	  // if (rawdat[sa]>-10000) {
	  // if ((sa==124)||(sa==126)||(sa==128)||(sa==130)) continue;
	  //printf("dead channels %7.3lf %7.3lf %7.3lf\n",mean,ampl,sigma);
	   if ((rawdat[sa]==-100000)) continue;
	  //  if (deadchannels[1760]) continue;
	  //   if (deadchannels[1760]) printf("dead channels %d\n",deadchannels[1760]);
	    //  if (sa==130) printf("dead channels %d %d\n",sa,rawdat[sa]);

	    dath->SetBinContent(sa+1, rawdat[sa]);
	    dath->SetBinError(sa+1, 1.);
	    // };
	};
#ifdef NOPEAKFIT
      dath->GetXaxis()->SetRange(mean-5., mean+5.);
      mean = dath->GetMean();
      sigma = dath->GetRMS();
      ampl = dath->GetBinContent(dath->FindBin(mean));
      dath->GetXaxis()->SetRange(-0.5, nsamples-0.5);

#endif
      //printf(" range: ( %.1lf  --  %.1lf  --  %.1lf )\n", mean-5., mean, mean+5.);
#ifndef NOPEAKFIT
      //   if (mean>139 && mean<149) printf("dead channels %7.3lf %7.3lf %7.3lf\n",ampl,mean,sigma);
      TF1 *fpeak = new TF1("fpeak", "pol0(0)+gaus(1)", 0., 1000.);
      fpeak->SetParameters(0., ampl, mean, 1.0);
      fpeak->SetParLimits(1, ampl/2., ampl*2.);
      fpeak->SetParLimits(2, mean-5., mean+5.);
      dath->Fit(fpeak, "Q", "", mean-5., mean+5.);
      //ampl  = fpeak->GetParameter(1); // this is junk... better take the bin content!
      mean  = fpeak->GetParameter(2);
      ampl = dath->GetBinContent(dath->FindBin(mean));
      sigma = fabs(fpeak->GetParameter(3));
      //  if (mean>139 && mean<149) printf("dead channels %7.3lf %7.3lf %7.3lf\n",ampl,mean,sigma);

#endif
      delete dath;

    
      // printf("dead channels %7.3lf %7.3lf %7.3lf\n",mean,ampl,sigma);
   
           int charge=0;
      for (int bin=locmax-2; bin<locmax+3; bin++)
	{
	  if ((rawdat[bin]==-100000)) continue;
	  if ((bin>0)&&(bin<nsamples))
	    charge += rawdat[bin];
	};

      //     if ( ((ampl>=threshold)&&(sigma>=clusterminsigma)&&(sigma<=clustermaxsigma)) ||
      //	   ((ampl>=highthreshold)&&(sigma>=clusterminsigma)) )
     if ( (ampl>=threshold)&&(sigma>=clusterminsigma)&&(sigma<=clustermaxsigma)&&(ampl<=highthreshold)) 
	{
	  if (hit)
	    {
	      //   if (mean>139 && mean<149) printf("dead channels %7.3lf %7.3lf %7.3lf\n",ampl,mean,sigma);
	      hit->ampl      = ampl;
	      hit->xl        = mean;
	      hit->yl        = -1;
	      hit->ylerr     = -1;
	      hit->sigma     = sigma;
	      //   printf("second %7.3lf %7.3lf \n",hit->xl,hit->ampl);

#ifndef NOPEAKFIT
	      hit->amplerr   = fpeak->GetParError(1);
	      hit->xlerr     = fpeak->GetParError(2);
	      hit->sigmaerr  = fpeak->GetParError(3);
	      hit->chisquare = fpeak->GetChisquare()/fpeak->GetNDF();
#endif
	      hit->charge    = charge;
	      hit->chargeerr = 0;
	      hit->quality   = 1;
	      //    if (mean>139 && mean<149) printf("dead channels %7.3lf %7.3lf %7.3lf\n",ampl,hit->xl,sigma);
	    };
#ifndef NOPEAKFIT
	  delete fpeak;
#endif
	  // if (mean>139 && mean<149) printf("dead channels %7.3lf %7.3lf %i\n",ampl,mean,locmax);
	  return locmax; 
	}
      else
	{
#ifndef NOPEAKFIT
	  delete fpeak;
#endif
	  //	  printf("dead channels %7.3lf %7.3lf %i\n",ampl,mean,locmax);
	  return -1;
	};
    }

  int      SetHitFinderMethod(eMethod m, char *options=NULL);
  template <typename T>
    //int    FindHitCandidate(int threshold, T *dat, int nsamples, int verbosity=0);
    //template <typename T>
    int FindHitCandidate(int threshold, T *dat, int nsamples, int verbosity=0)
    //!
    //! Find a candidate for a hit in the array data
    //!
    {
      if (threshold<=0.) return -1;
      if (verbosity>2) printf("cHitFinder::FindHitCandidate(): locating maximum\n");
      int locmax = TMath::LocMax<T>(nsamples, dat);
      if (verbosity>2) printf("cHitFinder::FindHitCandidate(): maximum at %d\n", locmax);
  
      // We located the bins with the maximum values in x and y. Let's
      // check if the vicinity looks Gaussian or not:
      if (verbosity>2) printf("cHitFinder::FindHitCandidate(): calculating chisquare\n");
      gchisqr = ChiGauss(nsamples, dat, locmax, NULL, NULL, verbosity);
      if (gchisqr<=1.0)
	{
	  //T *index = new T[nsamples];
	  //for (int i=0; i<nsamples; i++)
	  //  index[i] = i+1;
	  // // TGraph *g = new TGraph(nsamples, index, dat);
	  // // g->Draw("");
	  // // g->SetLineColor(kBlue);
	};
      if (verbosity)
	{
	  printf(" X=%3d   chisqr = %.3lf", locmax, gchisqr);
	  if (verbosity>1)
	    if (gchisqr<=1.0)
	      printf(" *****");
	  printf("\n");
	};
  
      return locmax;
    }

  template <typename T> int FindLocalMaxima(int n, T *dat, int *list, int nmaxima, int mindist, int threshold=0, int verbosity=0)

    //! Method to find local maxima in a data array.
    //!   n:         number of data points in array
    //!   dat:       data array
    //!   list:      list with local maxima positions found
    //!   nmaxima:   max. number of local maxima to serach for
    //!   mindist:   minimum distance between two local maxima
    //!   threshold: minimum amplitude of local maximum to be accepted
    //!   verbosity: 0=quiet, 1=some output, 2=more debugging output

    //! The method returns the number of local maxima that were found in the
    //! data. Alternatively the user can as well loop over the list and stop
    //! when the terminating "-1" is found to determine the number of list
    //! elements.
    {     
      int lmcnt=0;
      for (int nlm=0; nlm<nmaxima; nlm++)
	{
	  int loc = -1;
	  int amp = -1;
	  //int amp = 300;
	  // Loop over data array:
	  for (int i=0; i<n; i++)
	    {
	      // Sample higher than the previous highest one?
	      //if (i>199) printf(" sa%03d ", i); fflush(stdout);
	      if (dat[i]>amp)
		{
		  // Check if we're not in the vicinity of any previously found local maximum:
		  bool isolated=true;
		  for (int j=0; j<lmcnt; j++)
		    if (abs(i-list[j])<mindist)
		      {
			isolated=false;
			break;
		      };
		  // Only if the candidate is not close to a previously found l.m.:
		  if (isolated)
		    {
		      amp = dat[i];
		      loc = i;
		      if (verbosity) printf(" found local max @%d\n", i);
		    };
		};
	    };
	  // If new local maximum is below threshold we can stop here:
	  	  if (amp<threshold) break;
	 
	  // Otherwise we have found another local maximum:
	  list[lmcnt] = loc;

	  //  printf(" local maxima %d %d \n", loc, amp);
	  //  if (loc>139 && loc<149) printf(" sample %d %d %d %d\n", amp, loc,lmcnt,list[lmcnt]);
	  lmcnt++;
	

	};
      // Put termination mark at the end of the list:
      list[lmcnt] = -1;
      return lmcnt;


    }


  template <typename T> int RepairBrokenStrips(int n, T *dat, std::vector<int> *list=NULL)
    //! Method to "repair" (possibly) broken strips or strips that
    //! seem to have a problem in the current frame.
    {
      int repaircnt=0;
      for (int i=0; i<n; i++)
	{
	  // not yet at end of data array?
	  if (i+2<n)
	    // If strip i has good amplitude, check if strip i+2 also
	    // has significant signal, but strip i+1 has too less signal:
	    // Note that this is incomplete, we should also look backward to i-1, i-2...
	    if ( (dat[i]+dat[i+2]>200) && (dat[i+2]>dat[i]*0.30) && (dat[i]>dat[i+2]*0.3) )
	      {
		//printf(" cHitFinder::RepairBrokenStrips(): Dead strip candidate at %d: %d %d %d\n", i+1,
		//     dat[i], dat[i+1], dat[i+2]);
		if (dat[i+1]<dat[i]*0.25)
		  {
		    dat[i+1] = (dat[i]+dat[i+2])/1.0;
		    // printf(" cHitFinder::RepairBrokenStrips(): Corrected dead strip at %d\n", i+1);
		    repaircnt++;
		    if (list)
		      list->push_back(i+1);
		  };
	      };
	};
      return repaircnt;
    }
  
  double   GetChiSquare();
  double   GetAmplitude();
  double   GetPedestal();
  double   GetFrameRMS(int xy=0);
  int      MatchClusters(sClusterList *a, sClusterList *b, LumiGEM *hits);
  ////////////////////////////////////////////////////////////
 protected:
  ////////////////////////////////////////////////////////////
 private:
  int      nsax, nsay;
  double  *data[2];
  double  *peakpos[2], *peakampl[2], *peaksigma[2];
  double   gchisqr, gampl, gsigma, gped;
  //  template <typename T>
  //double ChiGauss(T *data, int pos, double *pedestalval=NULL,
  //		     TGraph *peakgraph=NULL, int verbosity=0);
  template <typename T> double ChiGauss(int n, T *data, int pos,
					double *pedestalval, TGraph *peakgraph, int verbosity)
    //!
    //! Calculate sum of squares normalized to amplitude of a
    //! peak candidate assuming a typical Gaussian.
    //! Parameters needed:
    //!    width of peak window
    //!    width of pedestal side windows (around peak window)
    //!
    //! If pedestalval is not NULL it will be used to store the
    //! value of the pedestal.
    //! If peakgraph is not NULL it will be used to return a
    //! graph of the "testpeak".
    //!    
    {
      for (int x=0; x<n; x++){
	// printf(" go here %d %d\n", x,data[x]);
       };
      double pedestal = 0.0;
      int pedfrom = 3;
      int pedto   = 5;
      int xsafrom = 0;
      int xsato   = n-1;
      int cnt=0;
      if (pos<=xsafrom) return 99999.9; // take care of array limits!
      if (pos>=xsato)   return 99999.9; // take care of array limits!
      if (verbosity>2) printf("cHitFinder::ChiGauss(): calculating pedestal...\n");
      //printf(" ---> ");
      for (int x=pos-pedto; x<=pos+pedto; x++)
	{
	  //	  if (x==130) printf(" go here %d %d\n", x,data[x]);
	    if (data[x]==-100000) continue;
	  if ((x<pos-pedfrom)||(x>pos+pedfrom))
	    if ((x>=xsafrom)&&(x<=xsato))
	      {
		//printf("%d ", x); fflush(stdout);
		pedestal += double(data[x]);
		cnt++;
		//	if (pos>139 && pos<149) printf(" go here %d %d %d %d %6.3lf\n", pos,x,data[x],cnt,pedestal);
	      };
	};
      //printf("\n");
      pedestal /= cnt;
      if (verbosity)
	printf("     est ped = %.1lf\n", pedestal);
      // guess the amplitude from entries at supposed position
      // and pedestal:
      double ampl = double(data[pos])-pedestal;
      double chisqr = 0.0;
      if (verbosity)
	printf("     est ampl = %.1lf\n", ampl);
      int peakpoints=9;
      if (peakgraph)
	peakgraph->Set(peakpoints);
      if (verbosity>2) printf("cHitFinder::ChiGauss(): calculating chisquare from refgauss...\n");
      for (int x=pos-peakpoints/2; x<=pos+peakpoints/2; x++)
	{
	  if ((x<xsafrom)||(x>xsato)) continue; // take care of array limits!
	  //double val = refgauss->Eval(double(x-pos))*ampl;
	  if ((x-pos<-5)||(x-pos>+5)) continue;
	  if (data[x]==-100000) continue;
	  double val = refgaussarray[x-pos]*ampl;
	  chisqr += pow( double(data[x])
			 - pedestal
			 - val
			 , 2.0);
	  if (peakgraph)
	    peakgraph->SetPoint(x-(pos-peakpoints/2), double(x), val + pedestal);
	};

      // Instead of a reduced chisquare we return the chisquare,
      // normalized to the peak amplitude:
      if (pedestalval)  *pedestalval = pedestal;
      gampl = ampl;
      gped  = pedestal;
      if (ampl<40.)
	return 99999.9;
      return chisqr/pow(ampl, 2.0);
    }

  TF1     *refgauss;
  double  *refgaussarray;
  int      preprocessMethod, hitfindMethod;
  ////////////////////////////////////////////////////////////
};

#endif
