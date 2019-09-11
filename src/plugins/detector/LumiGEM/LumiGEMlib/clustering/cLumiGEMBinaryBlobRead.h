#ifndef _CLUMIGEMBINARYBLOBREAD_H_
#define _CLUMIGEMBINARYBLOBREAD_H_

#include <cstddef>

#include<muserawtree.h>
#include "cLumiGEMDataRead.h"
#include "lumigemtree.h"


class cLumiGEMBinaryBlobRead : public cLumiGEMDataRead
{
 private:
//  typedef unsigned char        byte;
//  typedef unsigned char        u8;
//  typedef unsigned short       u16;
//  typedef unsigned long        u32;
//  typedef unsigned long long   u64;
//  struct sGEMrawData
//  {
//    u32 eventid;
//    int apvid;
//    u32 numapvs;
//    u32 apvheader[12];
//    int apvdata[12][128];
//  };
  sGEMrawData GEMrawdata;
 public:
  cLumiGEMBinaryBlobRead();
  ~cLumiGEMBinaryBlobRead();
  int  ReadBank(MRTBinaryBlob *lumi);

  int  GetNAPVs();
  int* GetBankBuffer(int lAPVnum);
};

#endif
