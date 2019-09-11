#ifndef _CLUMIGEMDATAREAD_H_
#define _CLUMIGEMDATAREAD_H_

#include <cstddef>

#include<muserawtree.h>
#include "lumigemtree.h"

class cLumiGEMDataRead
{
 protected:
  typedef unsigned char        byte;
  typedef unsigned char        u8;
  typedef unsigned short       u16;
  //typedef unsigned long        u32;
  typedef UInt_t               u32;
  typedef unsigned long long   u64;
  struct sGEMrawData
  {
    u32 eventid;
    int apvid;
    u32 numapvs;
    u32 apvheader[4*GEM_NUM];
    int apvdata[4*GEM_NUM][128];
  };
  sGEMrawData GEMrawdata;
 public:
  cLumiGEMDataRead();
  virtual ~cLumiGEMDataRead();
  int ReadBank(FILE*) {printf(" Hier simmer falsch!\n"); return 0;};//What the hell? German?
  int ReadBank(MRTBinaryBlob*) {printf(" Und hier erst recht!\n"); return 0;};
  //virtual int ReadBank() =0;
  int  GetNAPVs();
  int* GetBankBuffer(int lAPVnum);
};

#endif
