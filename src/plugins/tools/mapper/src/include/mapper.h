#ifndef __MAPPER__
#define __MAPPER__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include <iostream>
#include "muserawtree.h"
#include "SciFitree.h"
#include "StrawTubetree.h"
#include "trb3tree.h"
#include "v1190tree.h"
#include "v1290tree.h"
#include "v792tree.h"
#include "mqdctree.h"

#include "mappedchannelstree.h"
#include "BHtimetree.h"
#include "SPStree.h"
#include "PbGlasstree.h"
#include "BMtree.h"
#include "VETOtree.h"
#include "TRB_TDCtree.h"

#include <vector>
#include <map>

#include <utility>

class trbtdchit
{
 public:
  unsigned int coarse;
  unsigned int fine;
  unsigned int epoch;
  unsigned int edge;
  unsigned int id;
  trbtdchit(unsigned int id,unsigned int edge, unsigned int epoch, unsigned int coarse, unsigned int fine):
   id(id),edge(edge),epoch(epoch),coarse(coarse),fine(fine)
  {}; 
  trbtdchit(){};
  
};

class trbhit_w_channel
{
 public:
  unsigned int channel;
  trbhit hit;
  trbhit_w_channel(int ch, trbhit hit):channel(ch),hit(hit){};
  trbhit_w_channel(){};  
};

class v1190hit_w_channel
{
 public:
  unsigned int channel;
  v1190hit hit;
  v1190hit_w_channel(int ch, v1190hit hit):channel(ch),hit(hit){};
  v1190hit_w_channel(){};  
};

class v1290hit_w_channel
{
 public:
  unsigned int channel;
  v1290hit hit;
  v1290hit_w_channel(int ch, v1290hit hit):channel(ch),hit(hit){};
  v1290hit_w_channel(){};  
};

class v792hit_w_channel
{
 public:
  unsigned int channel;
  short value; 
  v792hit_w_channel(int ch, short value):channel(ch),value(value){};
  v792hit_w_channel(){};
};

class mqdchit_w_channel
{
 public:
  unsigned int channel;
  short value; 
  mqdchit_w_channel(int ch, short value):channel(ch),value(value){};
  mqdchit_w_channel(){};
};

class trbmapinfo
{
 public:
  unsigned int channelnumber;
  std::multimap<unsigned int,double> * hitmap;

 trbmapinfo(unsigned int to,  std::multimap<unsigned int,double> * hitmap):channelnumber(to),hitmap(hitmap){};
  trbmapinfo(){};
};

class mappingdir
{
 public:
  int from;
  int plane;
  int side;
  int to;
 mappingdir(int from,int plane,int side, int to):from(from),plane(plane),side(side),to(to){};
  mappingdir();
};


#define FINE_TIME_BINS 601      // number of bins in fine time histogram
#define MIN_STATS 10000         // minimum number of entries in fine time histogram for
                                // a valid calibration (this is a guide only)
#define CLOCK_CYCLE_LENGTH 5.0  // clock cycle length in ns

#define TRB_num 10              // number of trb3 boards in the system
#define QDC_num 10              //Number of MQDCs in system

class mapper:public Plugin
{
 private:
  MRTBinaryBlob *SLOWCTRL_names,*SLOWCTRL_updates;

 MRTBinaryBlob *in_gems;

 MRTBinaryBlob *in_TRB[TRB_num];
 MRTBinaryBlob *in_TBL[TRB_num];
 MRTBinaryBlob *in_TLA[TRB_num];
 MRTBinaryBlob *in_TRA[TRB_num];
 MRTBinaryBlob *in_TSL[TRB_num];
 MRTBinaryBlob *in_TSR[TRB_num];

 SciFi *scifi; 
 std::vector<trbtdchit>trbtdchits;      // For storing raw time info
 std::vector<trbhit_w_channel> trbhits; // For storing calibrated time and edge
 std::map<unsigned int, double*> trbtdccalibtab;
 std::map<unsigned int,double> trbzeros;

 void processTRB(bool);
 TRB3 *trbinfo;
 void * trbtdcmap; //otherwise, rootcint dies horribly

 StrawTube *stt;
 MRTEventInfo *eveinfo;

 MRTBinaryBlob *in_v1190;
 v1190 *v1190info;
 std::vector<v1190hit_w_channel> v1190hits;
 void processV1190(const char *name, MRTBinaryBlob *in,v1190 * info, std::vector<v1190hit_w_channel> & hits);
 void *v1190map;
 
 MRTBinaryBlob *in_v1290;
 v1290 *v1290info;
 std::vector<v1290hit_w_channel> v1290hits;
 void processV1290(const char *name, MRTBinaryBlob *in,v1290 * info, std::vector<v1290hit_w_channel> & hits);
 void *v1290map;

 MRTBinaryBlob *in_v792;
 v792 *v792info;
 std::vector<v792hit_w_channel> v792hits;
 void processV792(const char *name, MRTBinaryBlob *in,v792 * info, std::vector<v792hit_w_channel> & hits);
 void *v792map;
  
 MRTBinaryBlob *in_mqdc;
 mqdc *mqdcinfo;
 std::vector<mqdchit_w_channel> mqdchits;
 void processMQDC(const char *name, MRTBinaryBlob *in,mqdc * info, std::vector<mqdchit_w_channel> & hits);
 void *mqdcmap;


  // 03/15/17 Added binary blob for multiple MQDCs:
 MRTBinaryBlob *in_qdc[QDC_num];
 MRTBinaryBlob *in_qbl[QDC_num];
 MRTBinaryBlob *in_qla[QDC_num];
 MRTBinaryBlob *in_qra[QDC_num];

 mqdc *qdcinfo[QDC_num];
 std::vector<mqdchit_w_channel> qblhits[QDC_num];
 std::vector<mqdchit_w_channel> qrahits[QDC_num];
 std::vector<mqdchit_w_channel> qlahits[QDC_num];

 // void process_mult_MQDC(const char *name, MRTBinaryBlob *in,mqdc * info, std::vector<mqdchit_w_channel> & hits);
 void *qdcmap[QDC_num];
 void *qlamap[QDC_num];
 void *qblmap[QDC_num];
 void *qramap[QDC_num];

  MRTSlowData *slowdata;
  void processSlowCtrl();

 // --------------- End ----------------------------//

 mappedchannels *tbt;
 BHraw *BH;
 SPSraw *SPS;
 PbGlassraw *PbGlass;
 BMraw *BM;
 VETOraw *VETO;
 TRB_TDC_Board *trb_tdc_debug;


 public:
  mapper(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p);
  virtual ~mapper();
  // add funtions with return value Long_t here:
  
  Long_t startup_SciFi();
  Long_t startup_trb_tdc_debug();
  Long_t startup_slowctrl();
  Long_t startup_StrawTube();
  Long_t startup_TRB3debug();
  Long_t startup_GEM();
  Long_t startup_V1190debug();
  Long_t startup_V1290debug();
  Long_t startup_V792debug();
  Long_t startup_MQDCdebug();
  Long_t startup_mult_MQDCdebug();
  Long_t startup_mappedchannels();
  Long_t startup_BH();
  Long_t startup_SPS();
  Long_t startup_PbGlass();
  Long_t startup_BM();
  Long_t startup_VETO();


  Long_t scanTRB();
  Long_t calibrateTRB();
    
  Long_t map();


  Long_t finalize();

  Long_t addSciFiChannel(int id,int bank,int from, int plane, int side, int to);
  Long_t addSciFiReferenceClock(int id,int bank, int from);


  Long_t addTRB_TDC_DebugChannel(int tdc_num, int bank);


  Long_t addStrawChannel(int side,int plane, int straw_in_plane,int bank,int from);
  Long_t addStrawReferenceClock(int bank, int from);

  Long_t addTestBeamTDC(char *tgt,  int TDCtype, int channel);
  Long_t addTestBeamTRB(char *tgt, int bank, int channel);
  Long_t addTestBeamADC(char *tgt, int ADCtype, int channel);

  Long_t addBHTDC(int plane, int bar, int side,  int TDCtype, int channel);
  Long_t addBHTRB(int plane, int bar, int side, int bank, int channel);
  Long_t addBHADC(int plane, int bar, int side, int ADCtype, int channel);
  Long_t addBHReferenceClock(int id,int TDCtype, int channel);
  Long_t addBH_MQDC(int plane, int bar, int side, int QDC_bank, int channel);
  Long_t addBHTrigger(int id, int bank, int channel);

  Long_t addVETOTRB(int bar, int bank, int channel);
  Long_t addVETOReferenceClock(int id, int TDCtype, int channel);
  Long_t addVETOTrigger(int id, int bank, int channel);
  Long_t addVETO_MQDC(int bar, int QDC_bank, int channel);

  Long_t addSPSTDC(int wall, int bar, int up, int TDCtype, int channel);
  Long_t addSPSTRB(int wall, int bar, int up, int bank, int channel);
  Long_t addSPSADC(int wall, int bar, int up, int ADCtype, int channel);
  Long_t addSPSReferenceClock(int id,int TDCtype, int channel);
  Long_t addSPS_MQDC(int wall, int bar, int up, int QDC_bank, int channel);
  Long_t addSPSTrigger(int id, int bank, int channel);
  
  Long_t addPbGlassTDC(int x, int y, int TDCtype, int channel);
  Long_t addPbGlassTRB(int x, int y, int bank, int channel);
  Long_t addPbGlassADC(int x, int y, int ADCtype, int channel);
  Long_t addPbGlassReferenceClock(int id,int TDCtype, int channel);
  Long_t addPbGlass_MQDC(int x, int y, int QDC_bank, int channel);
  Long_t addPbGlassTrigger(int id, int bank, int channel);
  
  Long_t addBMTRB(int plane, int bar, int up, int bank, int channel);
  Long_t addBMReferenceClock(int id,int TDCtype, int channel);
  Long_t addBM_MQDC(int plane, int bar, int up, int QDC_bank, int channel);
  Long_t addBMTrigger(int id, int bank, int channel);

  virtual Long_t cmdline(char * cmd);

  ClassDef(mapper,1);
    };

#endif
