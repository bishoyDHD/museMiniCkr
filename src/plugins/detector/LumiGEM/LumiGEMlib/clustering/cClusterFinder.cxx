#include "TGraph.h"
#include "TMath.h"

#include "math.h"

#include "cClusterFinder.h"


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


cClusterFinder::cClusterFinder(int nx, int ny)
//! Constructor for the GEM Hit Finder
//!   nx, ny are the number of samples in x and y.
{
  nsa[0] = nx;
  nsa[1] = ny;

  verbosity = 0;

  preprocessMethod = ppNone;
  clusteringMethod    = mGaussFit;

  for (int i=0; i<NUMAXES; i++)
    {
      data[i]            = new int[nx];
      validdata[i]       = false;
      locmaxlist[i]      = new int[100];
      nlocmax[i]         = 0;
      locmaxmindist[i]   = 5;
      locmaxthreshold[i] = 100;
      clusterminsigma[i] = 0.45;
      clustermaxsigma[i] = 1.10;
    };

  refgauss = new TF1("refgauss", "gaus");
  refgauss->SetParameters(1.0, 0.0, 1.0);
  refgaussarray = new double[11];
  refgaussarray += 5;
  for (int i=-5; i<=+5; i++)
    refgaussarray[i] = refgauss->Eval(double(i));
}

cClusterFinder::~cClusterFinder()
//! Destructor for the GEM Hit Finder
{
  printf("d'tor\n");
  delete[] data[0];
  delete[] data[1];
  delete refgaussarray;
  delete refgauss;
}

////////////////////////////////////////////////////////////

int cClusterFinder::UseGEMData(cGEM *gem)
//! Get x and y data from cGEM object.
{
  if (gem==NULL)
    return 1;
  if (gem->GetXdata(data[0])) validdata[0] = true;
  if (gem->GetYdata(data[1])) validdata[1] = true;
  return 0;
}

int cClusterFinder::UseXData(int *datax)
//! Copy data provided in array datax to internal x dataset.
{
  if (datax==NULL)
    return 1;
  for (int i=0; i<nsa[0]; i++)
    data[0][i] = datax[i];
  validdata[0] = true;
  return 0;
}

int cClusterFinder::UseYData(int *datay)
//! Copy data provided in array datay to internal y dataset.
{
  if (datay==NULL)
    return 1;
  for (int i=0; i<nsa[1]; i++)
    data[1][i] = datay[i];
  validdata[1] = true;
  return 0;
}

int cClusterFinder::SetPreProcessMethod(ePreProcess m, char *options)
{
  preprocessMethod = m;
  return m;
}

int cClusterFinder::SetClusteringMethod(eClustMethod m, char *options)
{
  clusteringMethod = m;
  return m;
}

int cClusterFinder::PreProcess()
//! Preprocess the data.
{
  switch(preprocessMethod)
    {
    case ppNone:
      break;
    case ppRemoveLinearBaseline:
      if (validdata[0]) RemoveLinearBaseLine(nsa[0], data[0]);
      if (validdata[1]) RemoveLinearBaseLine(nsa[1], data[1]);
      break;
    default:
      if (verbosity) printf(" cClusterFinder::PreProcess(): Unknown method %d!\n", preprocessMethod);
      return 1;
    };
  return 0;
}

int cClusterFinder::Clustering()
//! Locate local maxima in the datasets and perform selected
//! clustering algorithm on local maxima.
{
  if (validdata[0]) FindLocalMaxima(nsa[0], data[0], locmaxlist[0], nlocmax[0], locmaxmindist[0], locmaxthreshold[0]);
  if (validdata[1]) FindLocalMaxima(nsa[1], data[1], locmaxlist[1], nlocmax[1], locmaxmindist[1], locmaxthreshold[1]);
  switch(clusteringMethod)
    {
    case mGaussFit:
      //CheckClusterCandidate();
      break;
    case mGaussTemplate:
      break;
    default:
      if (verbosity) printf(" cClusterFinder::Clustering(): Unknown method %d!\n", clusteringMethod);
      return 1;
    };
  return 0;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


int cClusterFinder::RemoveLinearBaseLine(int n, int *dat)
//!
//! Fit equation of a line f=mx+b to the APV frame samples
//! and subtract the fit function from the data then.
//!
{
  return 0;
  // dump data into a TGraph object:
  int *index = new int[n];
  for (int i=0; i<n; i++)
    index[i] = (int)i;
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
}

int cClusterFinder::FindLocalMaxima(int n, int *dat, int *list, int nmaxima, int mindist, int threshold)
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
      // Loop over data array:
      for (int i=0; i<n; i++)
	{
	  // Sample higher than the previous highest one?
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
		};
	    };
	};
      // If new local maximum is below threshold we can stop here:
      if (amp<threshold) break;
      // Otherwise we have found another local maximum:
      list[lmcnt] = loc;
      lmcnt++;
    };
  // Put termination mark at the end of the list:
  list[lmcnt] = -1;
  return lmcnt;
}

double cClusterFinder::ChiGauss(int n, int *data, int pos, double *pedestalval, TGraph *peakgraph)
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
  double pedestal = 0.0;
  int pedfrom = 3;
  int pedto   = 5;
  int xsafrom = 0;
  int xsato   = n;
  int cnt=0;
  if (verbosity>2) printf("cHitFinder::ChiGauss(): calculating pedestal...\n");
  for (int x=pos-pedto; x<=pos+pedto; x++)
    {
      if ((x<pos-pedfrom)||(x>pos+pedfrom))
	if ((x>=xsafrom)&&(x<=xsato))
	  {
	    pedestal += double(data[x]);
	    cnt++;
	  };
    };
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
      //double val = refgauss->Eval(double(x-pos))*ampl;
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

int cClusterFinder::CheckClusterCandidate(int locmax, int threshold, int *dat, int *rawdat, int nsamples, LumiGEMhit *hit)
//! Check a cluster candidate by comparing it to a reference cluster waveform.
//! A localmaximum "locmax" from the data array "dat" needs to be provided.
{
  double maxchisqr = 1.0;
  if (threshold<=0.) return -1;
  if (verbosity>2) printf("cHitFinder::CheckClusterCandidate() checking locmax at %d\n", locmax);
  // Let's check if the vicinity looks Gaussian or not:
  if (verbosity>1) printf("cHitFinder::FindHitCandidate(): calculating chisquare\n");
  gchisqr = ChiGauss(nsamples, dat, locmax, NULL, NULL);
  if (verbosity>1)
    printf("cHitFinder::CheckClusterCandidate(): Candidate @%d has chisqr=%lf\n", locmax, gchisqr);
  if (gchisqr>maxchisqr)
    return -1;
  //
  // Explicitly fit a Gaussian to the candidate:
  double ampl  = dat[locmax];
  double mean  = locmax;
  double sigma = 0.;
  TF1 *fpeak = new TF1("fpeak", "pol0(0)+gaus(1)", 0., 1000.);
  fpeak->SetParameters(600., ampl, mean, 1.5);
  fpeak->SetParLimits(1, ampl/4., ampl*4.);
  fpeak->SetParLimits(2, mean-5., mean+5.);
  TH1I *dath = new TH1I("dath", "dath", nsamples, -0.5, nsamples-0.5);
  for (int sa=0; sa<nsamples; sa++)
    {
      dath->SetBinContent(sa+1, rawdat[sa]);
      dath->SetBinError(sa+1, 1.);
    };
  dath->Fit(fpeak, "Q", "", mean-5., mean+5.);
  ampl  = fpeak->GetParameter(1);
  mean  = fpeak->GetParameter(2);
  sigma = fabs(fpeak->GetParameter(3));

  delete dath;

  if ( (sigma>=clusterminsigma[0])&&(sigma<=clustermaxsigma[0]))
    {
      if (hit)
	{
	  hit->ampl      = ampl;
	  hit->xl        = mean;
	  hit->yl        = -1;
	  hit->sigma     = sigma;
	  hit->chisquare = fpeak->GetChisquare()/fpeak->GetNDF();
	  hit->charge    = 0;
	  hit->quality   = 1;
	};
      delete fpeak;
      return locmax;
    }
  else
    {
      delete fpeak;
      return -1;
    };
}
