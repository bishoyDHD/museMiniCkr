
/** 
 ** This file has the definition of the LumiGEM tree branch objects 
 **/

#ifndef __LUMIGEMTREE_H__
#define __LUMIGEMTREE_H__
#define GEM_NUM 4

#include "muserawtree.h"

class LumiGEMhit : public MRTBase
{
 public:
  Int_t    GEMid;       // ID of GEM (0=left upstream, 1=left middle, ... , 5=right downstream)
  Double_t xl, yl;      // Cluster coordinates in GEM local coordinate system
  Double_t xlerr, ylerr;//   ...uncertainties thereof
  Double_t ampl, sigma; // Cluster amplitude and sigma
  Double_t amplerr, sigmaerr;// ...uncertainties thereof
  Double_t chisquare;   // Chisquare of Gaussian fit to cluster
  Double_t charge;      // Integrated cluster charge
  Double_t chargeerr;   //  ...uncertainty thereof
  Double_t quality;     // Cluster quality estimate (not used right now)

  LumiGEMhit();
  virtual ~LumiGEMhit();

  ClassDef(LumiGEMhit,2);
};

class LumiGEMeval : public MRTBase
{
 public:
  Int_t    GEMid;   // ID of GEM (as above)
  Double_t rmsx;    // RMS of all raw data samples
  Double_t rmsy;    // RMS of all raw data samples
                    //  ...are calculated after subtraction of linear baseline
                    //  These values can be used to monitor the data quality,
                    //  but be careful, they are not easy to interprete!

  LumiGEMeval();
  virtual ~LumiGEMeval();

  ClassDef(LumiGEMeval,1);
};

//class LumiGEMlut : public ORTBase
//{
// public:
//  Int_t    GEMid;
//  TString  GEMname;
//  
//  LumiGEMlut();
//  virtual ~LumiGEMlut();
//
//  ClassDEf(LumiGEMlut,1);
//};



class LumiGEM : public MRTBase
{
 public:
  //std::vector <LumiGEMLUT> lut; // look up table
  std::vector <LumiGEMhit>  hits; // List of clusters
  std::vector <LumiGEMeval> eval; // data quality evaluation 
  LumiGEM();
  virtual ~LumiGEM();

  int    GetNClusters(int gemid);
  int    GetNXClusters(int gemid);
  int    GetNYClusters(int gemid);
  Bool_t IsXCluster(unsigned int index);
  Bool_t IsYCluster(unsigned int index);

  enum eStatus {
    unknown =     0,
    /*---------- general status and problems ---*/
    good         = 1<< 0,
    suspicious   = 1<< 1,
    partiallybad = 1<< 2,
    bad          = 1<< 3,
    specialrun   = 1<< 4,
    empty3       = 1<< 5,
    empty4       = 1<< 6,
    empty5       = 1<< 7,
    /*---------- bad / error flags ---*/
    bad_hv       = 1<< 8,
    bad_daq      = 1<< 9,
    bad_daqsync  = 1<<10,
    bad_daqtrig  = 1<<11,
    bad_daqbuff  = 1<<12,
    bad_daqconf  = 1<<13,
    bad_trigger  = 1<<14,
    bad_zerodata = 1<<15,
    /*---------- not in use ---*/
    empty14      = 1<<16,
    empty15      = 1<<17,
    empty16      = 1<<18,
    empty17      = 1<<19,
    empty18      = 1<<20,
    empty19      = 1<<21,
    empty20      = 1<<22,
    empty21      = 1<<23,
    /*---------- special run flags ---*/
    sr_nobfield  = 1<<24,
    sr_hv        = 1<<25,
    sr_daq       = 1<<26,
    sr_gas       = 1<<27,
    empty26      = 1<<28,
    empty27      = 1<<29,
    empty28      = 1<<30,
    empty29      = 1<<31,
    /*--------------*/
  };

  ClassDef(LumiGEM,2);
};

#endif
