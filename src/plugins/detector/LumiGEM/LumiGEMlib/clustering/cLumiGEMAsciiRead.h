#ifndef _CLUMIGEMASCIIREAD_H_
#define _CLUMIGEMASCIIREAD_H_

#include <cstddef>
#include <stdio.h>

#include "cLumiGEMDataRead.h"

class cLumiGEMAsciiRead : public cLumiGEMDataRead
{
 private:

//struct sGEMrawData
//{
//  int eventid;
//  int apvid;
//  int numapvs;
//  int apvdata[12][128];
//
//
//};

  sGEMrawData GEMrawdata;

 public:

  // cLumiGEMAsciiRead(int numberofapvs=4); //i did for PSI cable checkup
 cLumiGEMAsciiRead(int numberofapvs=24); //i did for PSI cable checkup
 ~cLumiGEMAsciiRead();

 int ReadBank(FILE *file);
 int GetNAPVs();
 int* GetBankBuffer(int lAPVnum);
};

#endif
