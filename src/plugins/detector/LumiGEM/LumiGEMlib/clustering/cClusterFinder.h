/**********************************/
/*                                */
/*         cClusterFinder         */
/*                                */
/* Locate clusters in APV data    */
/*                                */
/* 2011, J. Diefenbach, Hampton U */
/*                                */
/**********************************/
//
// 02.11.2011
//   Started by copying methods from cHitFinder
//


#ifndef _CCLUSTERFINDER_H_
#define _CCLUSTERFINDER_H_

#include <cstddef>
#include <math.h>

#include "TF1.h"
#include "TGraph.h"
#include "TH1.h"
#include "TMath.h"

#include "cGEM.h"
#include "lumigemtree.h"


#define NUMAXES 2


class cClusterFinder
{
  ////////////////////////////////////////////////////////////
 public:
  cClusterFinder(int nx, int ny);
  ~cClusterFinder();
  ////////////////////////////////////////////////////////////
  enum eClustMethod
  {
    mGaussFit=0,
    mGaussTemplate
  };
  enum ePreProcess
  {
    ppNone=0,
    ppRemoveLinearBaseline=1
  };
  ////////////////////////////////////////////////////////////
  void SetVerbosity(int level) {verbosity=level;};
  int  UseGEMData(cGEM *gem);
  int  UseXData(int *datax);
  int  UseYData(int *datay);
  void InvalidateXData() {validdata[0]=false;};
  void InvalidateYData() {validdata[1]=false;};
  bool ValidXData() {return validdata[0];};
  bool ValidYData() {return validdata[1];};
  void InvalidateData() {InvalidateXData();InvalidateYData();};
  int  SetPreProcessMethod(ePreProcess m, char *options=NULL);
  int  SetClusteringMethod(eClustMethod m, char *options=NULL);
  int  PreProcess();
  int  Clustering();
  ////////////////////////////////////////////////////////////
  int  RemoveLinearBaseLine(int n, int *dat);
  int  GaussFilter(int n, int *dat, double sigma);
  int  FindLocalMaxima(int n, int *dat, int *list, int nmaxima, int mindist, int threshold=0);
  ////////////////////////////////////////////////////////////
 private:
  double ChiGauss(int n, int *data, int pos, double *pedestalval, TGraph *peakgraph=NULL);
  int    CheckClusterCandidate(int locmax, int threshold, int *dat, int *rawdat, int nsamples, LumiGEMhit *hit=NULL);
  ////////////////////////////////////////////////////////////
  int      verbosity;
  bool     validdata[NUMAXES];
  int      nsa[NUMAXES];
  int     *data[NUMAXES];
  TF1     *refgauss;
  double  *refgaussarray;
  int      preprocessMethod, clusteringMethod;
  int     *locmaxlist[NUMAXES], nlocmax[NUMAXES], locmaxmindist[NUMAXES], locmaxthreshold[NUMAXES];
  double   clusterminsigma[NUMAXES], clustermaxsigma[NUMAXES];
  double   gampl, gped, gchisqr;
  ////////////////////////////////////////////////////////////
};

#endif
