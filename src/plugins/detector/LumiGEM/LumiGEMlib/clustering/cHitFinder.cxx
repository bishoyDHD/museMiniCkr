#include "TGraph.h"
#include "TMath.h"

#include "math.h"

#include "cHitFinder.h"

cHitFinder::cHitFinder(int nx, int ny)
  : nsax(nx), nsay(ny)
//! Constructor for the GEM Hit Finder
{
  peakpos[0] = NULL;
  peakpos[1] = NULL;
  data[0] = new double[nx];
  data[1] = new double[ny];

  refgauss = new TF1("refgauss", "gaus");
  refgauss->SetParameters(1.0, 0.0, 1.0);
  refgaussarray = new double[11];
  refgaussarray += 5;
  for (int i=-5; i<=+5; i++)
    refgaussarray[i] = refgauss->Eval(double(i));

  clusterminsigma = 0.45;
  clustermaxsigma = 1.10;
}

cHitFinder::~cHitFinder()
//! Destructor for the GEM Hit Finder
{
  printf("d'tor\n");
  delete[] data[0];
  delete[] data[1];
  if (peakpos[0])
    for (int i=0; i<2; i++)
      {
	printf("peakpos[%d]\n", i);
	delete[] peakpos[i];
	delete[] peakampl[i];
	delete[] peaksigma[i];
      };
  delete[] refgaussarray;
  delete refgauss;
  printf("leaving now\n");
}

double **cHitFinder::GetXarray()
{
  return &(data[0]);
}

double **cHitFinder::GetYarray()
{
  return &(data[1]);
}

// ... chi gauss

int cHitFinder::SetPreProcessMethod(ePreProcess m, char *options)
//!
//! Choose a pre-processing method and pass options to the
//! pre-processin algorithm.
//!
{
  if (options)
    {
    };
  return preprocessMethod = m;
}

int cHitFinder::PreProcess()
//!
//! Pre-process data, e.g. apply filters, remove common mode
//! baseline fluctuations.
//!
{
  switch (preprocessMethod)
    {
    case ppRemoveLinearBaseline:
      RemoveLinearBaseline(nsax, data[0]);
      RemoveLinearBaseline(nsay, data[1]);
      break;
    case ppDoNothing:
      break;
    default:
      printf(" Unknown pre-processing method %d...\n", preprocessMethod);
    };
  return 0;
}

int cHitFinder::ChargeFaking(const std::vector <LumiGEMhit> &oneDlist, std::vector <LumiGEMhit> *twoDlist)
//!
//! This method "fakes" a cluster in x or y with huge sigma in order to fix a missing
//! axis in the readout of a GEM detector.
//!
{
  int lverb=0;
  int chargemode=0;
  // detector center positions (in pitch) and widths:
  double xcenter = 125., xwidth=250.;
  double ycenter = 125., ywidth=250.;
  // loop over all 1d clusters:
  if (lverb) printf("-----cHitFinder::ChargeFaking():   searching a list of %d entries for matching clusters\n", (int)oneDlist.size());
  for (unsigned int j=0; j<oneDlist.size(); j++)
    {
      if (lverb) printf("oneDlist: hit #%d\n", j);
      double charge = oneDlist[j].charge;
      if (chargemode==1) // use charge or amplitude?
	charge = oneDlist[j].ampl;
      double amplx=0., amply=0.;
      bool   isx  = (oneDlist[j].yl<0.);
      if (isx)
	amplx = charge;
      else
	amply = charge;
     
      LumiGEMhit ahit;
      ahit.GEMid = oneDlist[j].GEMid;
      if (isx)
	{
	  ahit.xl = oneDlist[j].xl; ahit.xlerr = oneDlist[j].xlerr;
	  ahit.yl = ycenter;        ahit.ylerr = ywidth/2.;
	  amply   = charge;
	}
      else
	{
	  ahit.xl = xcenter;        ahit.xlerr = xwidth/2.;
	  ahit.yl = oneDlist[j].yl; ahit.ylerr = oneDlist[j].ylerr;
	  amply   = charge;
	};
      ahit.ampl      = oneDlist[j].ampl;
      ahit.charge    = charge;
      ahit.sigma     = oneDlist[j].sigma;
      ahit.amplerr   = oneDlist[j].amplerr;
      ahit.chargeerr = oneDlist[j].chargeerr;
      ahit.sigmaerr  = oneDlist[j].sigmaerr;
      ahit.chisquare = oneDlist[j].chisquare;
      ahit.quality   = 0.5;
      if (lverb) printf("      pushing matching 2d hit @(%.1lf/%.1lf)\n",
			ahit.xl, ahit.yl);
      twoDlist->push_back(ahit);
    };
  if (lverb) printf("-----cHitFinder::ChargeFaking(): done\n");
  return 0;
}

int cHitFinder::ChargeSharing(const std::vector <LumiGEMhit> &oneDlist, std::vector <LumiGEMhit> *twoDlist)
//!
//! This method assumes that oneDlist contains a list of 1d clusters belonging to
//! one GEM only. It will then try to find matching pairs of clusters in x and y
//! based on a comparison of cluster charges. Matching pairs are then dumped into
//! twoDlist as 2d clusters (with valid x and y clusterpositions).
//!
{
  int lverb=0; // "local verbosity" for debugging only
  int chargemode=0;
  // loop over all 1d clusters:
  if (lverb) printf("-----cHitFinder::ChargeSharing():   searching a list of %d entries for matching clusters\n", (int)oneDlist.size());
  for (unsigned int j=0; j<oneDlist.size(); j++)
    {
      if (lverb) printf("oneDlist: hit #%d\n", j);
      // We need to treat amplitudes in x and y differently as we'll
      // do charge sharing later on: (there can be offsets between
      // x and y charges!)
      double charge = oneDlist[j].charge;
      if (chargemode==1) // use charge or amplitude?
	charge = oneDlist[j].ampl;
      double amplx=0., amply=0.;
      bool   isx  = (oneDlist[j].yl<0.);
      if (isx)
	amplx = charge;
      else
	amply = charge;
      // Loop over the rest of hits in the list to figure out which
      // other clusters (in the "other" axis) match this one we just
      // got:
      for (unsigned int k=j+1; k<oneDlist.size(); k++)
	{
	  if ((oneDlist[k].yl<0.)==isx) continue; // hit needs to be on the opposite readout axis
	  if (lverb) printf("   try to match with #%d\n", k);
	  double charge = oneDlist[k].charge;
	  if (chargemode==1) // use charge or amplitude?
	    charge = oneDlist[k].ampl;
	  if (isx)
	    amply = charge;
	  else
	    amplx = charge;
	  double chargediff = amplx-amply;
// <<<<<<< HEAD
// 	  if (amplx+amply>2000) chargediff=0;
// 	  if ((chargediff<-550.)||(chargediff>350.))
// 	    {
// 	      if (lverb) printf("     charge diff = %.1lf(x)-%.1lf(y)=%.1lf ---> no match!\n", amplx, amply, chargediff);
// 	      double ampl1 = oneDlist[k].ampl; // maybe amplitudes are
// 	      double ampl2 = oneDlist[j].ampl; // somewhat similar?
// 	      if ((ampl1/ampl2<0.75)||(ampl1/ampl2>1.25))
// 		{
// 		  if (lverb) printf("     ampl ratio = %.2lf ---> still no match!\n", ampl1/ampl2);
// =======
	  double charge1 = oneDlist[k].charge; 
	  double charge2 = oneDlist[j].charge;
	  double ampl1 = oneDlist[k].ampl; // maybe amplitudes are
	  double ampl2 = oneDlist[j].ampl; // somewhat similar?

	  if (amplx+amply>2000) chargediff=0;

	  /*
	  if (((chargediff<-550.)||(chargediff>350.)))
	    {
	      if (lverb) printf("     charge diff = %.1lf(x)-%.1lf(y)=%.1lf ---> no match!\n", amplx, amply, chargediff);
	      // double ampl1 = oneDlist[k].ampl; // maybe amplitudes are
	      // double ampl2 = oneDlist[j].ampl; // somewhat similar?
	      //   double charge1 = oneDlist[k].charge; 
	      //   double charge2 = oneDlist[j].charge; // somewhat similar?
	      if (((ampl1/ampl2<0.50)||(ampl1/ampl2>1.50)))
		{
		  //  if (lverb) printf("     ampl ratio = %.2lf ---> still no match!\n", ampl1/ampl2);
>>>>>>> 21b12888d9da6bcfa19621675a55525544b7ac76
		  continue;
		};
	      if (lverb) printf("     anyway, ampl ratio was good (%.2lf) ---> accept this one!\n", ampl1/ampl2);
	    };
	  // it seems we found a possible partner for our hit/cluster:
<<<<<<< HEAD
=======
*/

	  // Right now, we accepat all possible partner for our hit/cluster:
	  if (lverb) printf("      found a match: j=%d k=%d\n", j, k);
	  LumiGEMhit ahit;
	  ahit.GEMid = oneDlist[j].GEMid;
	  if (isx)
	    {
	      ahit.xl = oneDlist[j].xl; ahit.xlerr = oneDlist[j].xlerr;
	      ahit.yl = oneDlist[k].yl;	ahit.ylerr = oneDlist[k].ylerr;
	    }
	  else
	    {
	      ahit.xl = oneDlist[k].xl; ahit.xlerr = oneDlist[k].xlerr;
	      ahit.yl = oneDlist[j].yl;	ahit.ylerr = oneDlist[j].ylerr;
	    };
	  ahit.ampl      = (oneDlist[k].ampl+oneDlist[j].ampl)/2.;
	  ahit.charge    = (oneDlist[k].charge+oneDlist[j].charge)/2.;
	  ahit.sigma     = (oneDlist[k].sigma+oneDlist[j].sigma)/2.;
	  ahit.amplerr   = sqrt(pow(oneDlist[k].amplerr,2.)+pow(oneDlist[j].amplerr,2.))/2.;
	  ahit.chargeerr = sqrt(pow(oneDlist[k].chargeerr,2.)+pow(oneDlist[j].chargeerr,2.))/2.;
	  ahit.sigmaerr  = sqrt(pow(oneDlist[k].sigmaerr,2.)+pow(oneDlist[j].sigmaerr,2.))/2.;
	  ahit.chisquare = (oneDlist[k].chisquare + oneDlist[j].chisquare)/2.;
	  ahit.quality   = 1.0;
	  if (lverb) printf("      pushing matching 2d hit @(%.1lf/%.1lf)\n",
			    ahit.xl, ahit.yl);
	  twoDlist->push_back(ahit);
	  //  printf("charge sharing  %5.2lf %5.2lf %5.2lf %5.2lf\n",ahit.xl,ahit.yl,ahit.ampl,ahit.charge);
	};
    };
  if (lverb) printf("-----cHitFinder::ChargeSharing(): done\n");
  return 0;
}

void cHitFinder::PrintHitlist(const std::vector <LumiGEMhit> &hitlist)
{
  printf("\n");
  printf(" ==================  List of Hits  ==================\n");
  printf(" entry\tGEMid\txl\tyl\tampl\tcharge\tsigma\n");
  printf(" ----------------------------------------------------\n");
  for (unsigned int i=0; i<hitlist.size(); i++)
    printf(" #%3d:\t%d\t%.1lf\t%.1lf\t%.1lf\t%.1lf\t%.2lf\n", i,
	   hitlist[i].GEMid, hitlist[i].xl, hitlist[i].yl,
	   hitlist[i].ampl, hitlist[i].charge, hitlist[i].sigma);
  printf(" ----------------------------------------------------\n");
  printf("  %d entries total\n", (int)(hitlist.size()));
  printf(" ====================================================\n");
}

//... remove linear baseline

// ... gauss filter

int cHitFinder::SetHitFinderMethod(eMethod m, char *options)
{
  if (options)
    {
    };
  return hitfindMethod = m;
}

// ... find hit candidate

double cHitFinder::GetChiSquare()
//!
//! Returns the chisquare of the last fit or hit finding.
//!
{
  return gchisqr;
}

double cHitFinder::GetAmplitude()
//!
//! Returns the amplitude of the last fit or hit finding.
//!
{
  return gampl;
}
double cHitFinder::GetPedestal()
//!
//! Returns the pedestal of the last fit or hit finding.
//!
{
  return gped;
}

double cHitFinder::GetFrameRMS(int xy)
//!
//! Returns the RMS of the x (xy=0) or y (xy=1) data set.
//!
{
  return TMath::RMS(nsax, data[xy]);
}

int cHitFinder::MatchClusters(sClusterList *a, sClusterList *b, LumiGEM *gemo)
{
  // We need to match clusters from the lists a and b using the charge
  // information, i.e. we will assume that clusters that belong together
  // have similar charge.
  //
  // The method should provide a list of possible (x,y) points with
  // some kind of score or quality measure attached in order of
  // descending quality/probability.
  //
  // So the most likely combinations should be first in the list.

  int na = a->size;
  int nb = b->size;
  // If there is no cluster found on one axis, we cannot produce any
  // (x,y) pairs:
  if ((na==0)||(nb==0))
    return NULL;
  // Setting defaults for criterion parameters:
  double chargematch = 0.30;
  double goodqratio  = 1.00;
  double a_minsigma  = 0.50, a_maxsigma  = 2.50;
  double b_minsigma  = 0.50, b_maxsigma  = 2.50;
  double a_goodsigma = 0.85, b_goodsigma = 0.85;
  double minquality  = 1.00;
  // Loop over all possible combinations of clusters in x and y:
  double quality = 1.0;
  LumiGEMhit hit;
  gemo->hits.clear();
  for (int i=0; i<na; i++)
    for (int j=0; j<nb; j++)
      {
	// 1. criterion: charges should match within some range:
	double qratio = a[i].charge / b[j].charge;
	quality *= pow(goodqratio-qratio, 2.);
	if ( (qratio < (1.0-chargematch)) || (qratio > (1.0+chargematch)) )
	  continue;
	// 2. criterion: sigma of x and y clusters should be in range:
	if ( (a[i].sigma < a_minsigma) || (a[i].sigma > a_maxsigma) )
	  continue;
	if ( (b[j].sigma < b_minsigma) || (b[j].sigma > b_maxsigma) )
	  continue;
	quality *= pow(a_goodsigma-a[i].sigma, 2.);
	quality *= pow(b_goodsigma-b[j].sigma, 2.);
	// 3. criterion: quality should not be smaller than minimum quality:
	if (quality < minquality)
	  continue;
	// push combination onto hitlist:
	hit.GEMid   = -1;
	hit.xl      = a[i].loc;
	hit.yl      = b[j].loc;
	hit.charge  = (a[i].charge + b[j].charge)/2.;
	hit.ampl    =  (a[i].ampl  +  b[j].ampl )/2.;
	hit.sigma   = sqrt(pow(a[i].sigma,2.) + pow(b[j].sigma,2.));
	hit.quality = quality;
	gemo->hits.push_back(hit);
      };
  

  return gemo->hits.size();
}
