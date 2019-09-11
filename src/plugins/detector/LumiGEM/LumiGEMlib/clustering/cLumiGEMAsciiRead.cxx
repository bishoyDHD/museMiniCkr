#include "cLumiGEMAsciiRead.h"

cLumiGEMAsciiRead::cLumiGEMAsciiRead(int numberofapvs)
{
  GEMrawdata.numapvs = numberofapvs;
}

cLumiGEMAsciiRead::~cLumiGEMAsciiRead()
{
}

int cLumiGEMAsciiRead::ReadBank(FILE *file)
{
  u32 dataptr;
  int wordcnt=0;
  char s[1024];
  // int eventread=-1;

   for(unsigned int apv=0; apv<GEMrawdata.numapvs; apv++)
    {
      if (fscanf(file, "%d %d\n", &GEMrawdata.eventid, &GEMrawdata.apvid)!=2) {
	printf(" cLumiGEMAsciiRead::ReadBank(): Warning - error reading eventid and apvid block header!\n");
	//	if (GEMrawdata.eventid=eventread) break;	
      };
      printf(" cLumiGEMAsciiRead::ReadBank(): %d %d\n",GEMrawdata.eventid, GEMrawdata.apvid);
      // eventread=GEMrawdata.eventid;

      if(feof(file)) break;
      for (int i=0; i<128; i++)
	{	 
	  if (fscanf(file, "%d", &dataptr)!=1)
	    printf(" cLumiGEMAsciiRead::ReadBank(): Warning - error reading data word %d!\n", i);

	    //GEMrawdata.apvdata[apv][i] = dataptr; 
	  if (dataptr!=0) GEMrawdata.apvdata[apv][i] = dataptr;//i did for PSI cable checkup
	  //if (dataptr!=0) printf("apv: %d data: %ld\n", apv, dataptr);//i did  PSI cable checkup
	 
	  //printf("apv: %d  ch: %d   data: %ld\n", apv, i, dataptr);

	  // usleep(10000);
	};
      wordcnt++;
    };

  return wordcnt;
}

int cLumiGEMAsciiRead::GetNAPVs()
{
  return GEMrawdata.numapvs;
}

int* cLumiGEMAsciiRead::GetBankBuffer(int lAPVnum)
{
  return GEMrawdata.apvdata[lAPVnum];
}
