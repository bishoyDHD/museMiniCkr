#include "cLumiGEMBinaryBlobRead.h"
#include <iostream>


cLumiGEMBinaryBlobRead::cLumiGEMBinaryBlobRead()
{
}

cLumiGEMBinaryBlobRead::~cLumiGEMBinaryBlobRead()
{
}

int cLumiGEMBinaryBlobRead::ReadBank(MRTBinaryBlob *lumi)
{

  int size; size=(lumi->size)/sizeof(u32);
  if (size==0)
    return 0;
  u32 *dataptr = (u32*)lumi->data;
  if (dataptr==NULL) // otherwise explora crashes
    return 0;        // every once in a while...
  // structure:
  // eventheader (eventid, numapvs, VME module id)
  // 130 apv data words
  // trailer (0xffffffff)
  /// Three slashes mean "OLYMPUS specific":
  ///  u32 eventheader = *dataptr;
  ///  dataptr++;
  ///  GEMrawdata.eventid = eventheader&0x00ff;
  ///  GEMrawdata.numapvs = size/130; //eventheader&0x0f00
  //GEMrawdata.numapvs = (eventheader&0x0f00)>>8;
  GEMrawdata.eventid = 0;
  // In MUSE we have 6240 bytes per event
  // and telescope. Divide by 4 for long words and by 130, because we have
  // 1 header word + 128 data words + 1 trailer word per APV (12 APVs total)
  GEMrawdata.numapvs = 4*GEM_NUM; //six GEMs

  //printf("numapvs = %d\n", GEMrawdata.numapvs);
  int wordcnt=0;
  int currentapv=0;
  //std::cout << GEMrawdata.numapvs << std::endl;
  for (u32 apv=0; apv<GEMrawdata.numapvs; apv++)
    {
      GEMrawdata.apvheader[apv] = *dataptr;
      dataptr++;

      int numchans = 128;
      if(currentapv%2)
        numchans = 122;

      for (int i=0; i<128; i++)//This is WRONG! Not all GEMs have 128 channels!! But you have to leave it for now because the data stream expects 128 chans and throws out 6
	     {
	       GEMrawdata.apvdata[apv][i] = *dataptr;
	       dataptr++;
	       wordcnt++;
         //std::cout << GEMrawdata.apvdata[apv][i] << " " << wordcnt << " " << dataptr << std::endl;
	     };
      // skip trailer 0xffffffff:
      dataptr++;
      currentapv++;
    };
    //std::cout << wordcnt << std::endl;
  return wordcnt;
}

int cLumiGEMBinaryBlobRead::GetNAPVs()
{
  return GEMrawdata.numapvs;
}

int* cLumiGEMBinaryBlobRead::GetBankBuffer(int lAPVnum)
{
  return GEMrawdata.apvdata[lAPVnum];
}
