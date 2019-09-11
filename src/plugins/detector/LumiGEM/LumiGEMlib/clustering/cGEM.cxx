#include "cGEM.h"
#include "cAPV.h"

#include<iostream>
#include <cstddef>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

cGEM::cGEM(int numberofAPVs, int samplesperAPV)
  : nAPVs(numberofAPVs), nSamplesAPV(samplesperAPV)
    //! Constructor for cGEM class.
    //!   numberofAPVs:  how many APVs are connected to the GEM
    //!   samplesperAPV: how many data samples are in the data
    //!                  stream from each APV (default=128)
{
  APV       = new cAPV*[nAPVs];
  lAPVid    = new int[nAPVs];
  xy        = new int[nAPVs];
  for (int i=0; i<nAPVs; i++)
    {
      APV[i]       = NULL;
      lAPVid[i]    = undefined;
      xy[i]        = 0;
    };
  xdata = NULL;
  ydata = NULL;
  nxch = 0;
  nych = 0;
  xnsamplelist = NULL;
  ynsamplelist = NULL;
  nxapvs = 0;
  nyapvs = 0;
  swapx = false;
  swapy = false;
}

cGEM::~cGEM()
//! Destructor for cGEM class.
{
  for (int i=0; i<nAPVs; i++)
    if (APV[i]) delete APV[i];
  delete[] APV;
  delete[] lAPVid;
  delete[] xy;
  delete[] xdata;
  delete[] ydata;

}

int cGEM::AddAPV(int lAPVnum, int skip)
//! Private method internally used to add the description of an
//! APV. "xy" needs to be set accordingly by the calling method
//! afterwards.
{
  for (int i=0; i<nAPVs; i++)
    {
      if (APV[i]==NULL)
	{
	  APV[i]    = new cAPV(nSamplesAPV);
	  lAPVid[i] = lAPVnum;
	  xy[i]     = undefined;
	  APV[i]->SkipFirst(skip);
	  return i;
	};
    };
  return -1;
}

int cGEM::AddAPVx(int lAPVnum, int skip)
//! Adds the description of an "x" APV to the internal list of APVs.
//!   lAPVnum: the logical APV id number as in the data stream,
//!   usually assigned by the data source during readout
{
  int id = AddAPV(lAPVnum, skip); 
  if (id>=0)
    {
      xy[id] = xaxis;
      nxapvs++;
    };
  return id;
}

int cGEM::AddAPVy(int lAPVnum, int skip)
//! Adds the description of an "y" APV to the internal list of APVs.
//!   lAPVnum: the logical APV id number as in the data stream,
//!   usually assigned by the data source during readout
{
  int id = AddAPV(lAPVnum, skip);
  if (id>=0)
    {
      xy[id] = yaxis;
      nyapvs++;
    };
  return id;
}

int cGEM::AddAPVData(int lAPVnum, int *data)
//! Fills unordered raw APV data into the corresponding APV object to
//! take care of skipping unconnected APV input pins and reordering
//! the raw data.
{
  for (int i=0; i<nAPVs; i++)
    {
      if (lAPVid[i]==lAPVnum)
	{
	  if (APV[i])
	    return APV[i]->AddData(data);
	  else
	    {
	      // this should never ever happen, anyway...
	      printf(" cGEM::AddAPVData(): ERROR - uninitialized APV #%d found!\n", lAPVnum);
	      return -2;
	    };
	};
    };
  return -1;
}

int cGEM::ClearAPVs()
{
  for (int i=0; i<nAPVs; i++)
    if (APV[i])
      APV[i]->Clear();
  return 0;
}

int cGEM::SkipFirstAPVChannels(int lAPVnum, int skip)
//! Call this method to indicate that the first "skip" input pins of
//! this APV are not connected to detector strips / channels and their
//! data samples should be skipped.
{
  for (int i=0; i<nAPVs; i++)
    {
      if (lAPVid[i]==lAPVnum)
	return APV[i]->SkipFirst(skip);
    };
  return -1;
}
int cGEM::SkipLastAPVChannels(int lAPVnum, int skip)
//! Call this method to indicate that the last "skip" input pins of
//! this APV are not connected to detector strips / channels and their
//! data samples should be skipped.
{
  for (int i=0; i<nAPVs; i++)
    {
      if (lAPVid[i]==lAPVnum)
	return APV[i]->SkipLast(skip);
    };
  return -1;
}
int cGEM::SkipAPVChannels(int lAPVnum, int *channelstoskip, int size)
//! Call this method to provide a list of pins of this APV that are
//! not connected to detector strips / channels, so that their data
//! samples will be skipped. The size of the array must be specified
//! by "size".
{
  for (int i=0; i<nAPVs; i++)
    {
      if (lAPVid[i]==lAPVnum)
	return APV[i]->SkipSamples(channelstoskip, size);
    };
  return -1;
}

int cGEM::ModifyAPVChannels(int lAPVnum, int *channelstomodify, int size, cAPV::eSampleModification m)
{
   for (int i=0; i<nAPVs; i++)
    {
      if (lAPVid[i]==lAPVnum)
	return APV[i]->ModifySamples(channelstomodify, size, m);
    };
  return -1;
}

int cGEM::ModifyAPVChannels(int lAPVnum, const char *channelstomodify, int size, cAPV::eSampleModification m)
{
  //printf("cGEM::ModifyAPVChannels():\n");
  int *list = new int[size];
  char buff[128];
  char *where=(char*)channelstomodify;
  for (int i=0; i<size; i++)
    {
      while (isspace(where[0]))
	where++;
      sscanf(where, "%s", buff);
      list[i] = atoi(buff);
      //printf("   ===%3d===\n", list[i]);
      // jump to next token:
      where = strchr(where+1, ' ');
    };
  int retval = ModifyAPVChannels(lAPVnum, list, size, m);
  delete[] list;
  if (retval<0)
    return 0; // makes counting in LumiGEM plugin more simple
  return retval;
}

int cGEM::GetNchannelsX()
//! Returns the number of detector channels / strip in x.
{
  nxch=0;
  for (int i=0; i<nAPVs; i++)
    {
      if (APV[i]==NULL) break;
      if (xy[i]==xaxis)
	nxch += APV[i]->GetNchannels();
    };
  return nxch;
}

int cGEM::GetNchannelsY()
//! Returns the number of detector channels / strip in y.
{
  nych=0;
  for (int i=0; i<nAPVs; i++)
    {
      if (APV[i]==NULL) break;
      if (xy[i]==yaxis)
	nych += APV[i]->GetNchannels();
    };
  return nych;
}

int cGEM::PushXdata(bool swapping)
//! Make the APV objects push their data into the internal xdata
//! array. Returns the number of detector channels / strip in x.
{
  // We start at the beginning of the xdata array (i.e. channel=0) to
  // fill the data of the first "x" APV:
  if (xdata)        delete[] xdata;
  if (xnsamplelist) delete[] xnsamplelist;
  nxch=0;
  for (int i=0; i<nAPVs; i++)
    {
      if (APV[i]==NULL) break;
      if (xy[i]==xaxis)
	nxch += APV[i]->GetNchannels();
    };
    if(nxch!=250)
      printf("Insufficient channels\n");
  xdata        = new int[nxch];
  xnsamplelist = new int[nxapvs+1];
  // now, push the data from apvs into our internal xdata array:
  int channel=0, apvcnt=0;
  for (int i=0; i<nAPVs; i++)
    {
      if (APV[i]==NULL) break;
      if (xy[i]==xaxis)
	{
	  xnsamplelist[apvcnt] = APV[i]->GetData( &(xdata[channel]) );
	  channel += xnsamplelist[apvcnt];
	  // cAPV::GetData() returns the number of detector channels it
	  // filled, so we advance to the position xdata[channel] in the
	  // array to find the next free space for the next "x" APV's data
	  // (if there is such).
	  apvcnt++;
	};
    };
  nxch = channel;
  if(nxch!=250)
  printf("ERROR %d nxch\n",nxch);
  

  //Swapping data samples in X axis
  //This is never true though...Is it necessary?
  if (swapping)
    {
      //for(int j=0; j<nxch/2; j++)
      //swap( &(xdata[j]), &(xdata[nxch-j-1]) );
      swaparray(xdata, nxch);
      swaparray(xnsamplelist, nxapvs);
    };

  // terminate list of number of x samples with -1:
  xnsamplelist[nxapvs] = -1;

  return channel;
}

int cGEM::PushYdata(bool swapping)
//! Make the APV objects push their data into the internal ydata
//! array. Returns the number of detector channels / strip in y.
{
  // See cGEM::PushXData() for comments on how this method works.
  if (ydata)        delete[] ydata;
  if (ynsamplelist) delete[] ynsamplelist;
  nych=0;
  for (int i=0; i<nAPVs; i++)
    {
      if (APV[i]==NULL) break;
      if (xy[i]==yaxis)
	nych += APV[i]->GetNchannels();
    };
  ydata        = new int[nych];
  ynsamplelist = new int[nyapvs+1];
  // now, push the data from apvs into our internal ydata array:
  int channel=0, apvcnt=0;
  for (int i=0; i<nAPVs; i++)
    {
      if (APV[i]==NULL) break;
      if (xy[i]==yaxis)
	{
	  ynsamplelist[apvcnt] = APV[i]->GetData( &(ydata[channel]) );
	  channel += ynsamplelist[apvcnt];
	  apvcnt++;
	};
    };
  nych = channel;

  //Swapping data samples in Y axis
  if (swapping)
    {
      //for(int j=0; j<nych/2; j++)
      //swap( &(ydata[j]), &(ydata[nych-j-1]) );
      swaparray(ydata, nych);
      swaparray(ynsamplelist, nyapvs);
    };

  // terminate list of number of y samples with -1:
  ynsamplelist[nyapvs] = -1;
  
  return channel;
}

int* cGEM::GetXdata(int *data)
//! Returns an array of integers with the x data values. The user
//! needs to take care of deleting this array after use!
{
  if (nxch==0) return NULL;
  // We make a copy of our internal array so that the user cannot
  // damage our data integrity ;)
  if (data==NULL)
    data = new int[nxch];
  
  for (int i=0; i<nxch; i++)
    data[i] = xdata[i];

  //ETHAN
  //memcpy? more like memleak
  //Don't use, causes segfault
  //memcpy(data, xdata, nxch*sizeof(int));

  return data;
}

int* cGEM::GetYdata(int *data)
//! Returns an array of integers with the y data values. The user
//! needs to take care of deleting this array after use!
{
  if (nych==0) return NULL;
  // We make a copy of our internal array so that the user cannot
  // damage our data integrity ;)
  if (data==NULL)
    data = new int[nych];
  for (int i=0; i<nych; i++)
    data[i] = ydata[i];
  //memcpy(data, ydata, nych*sizeof(int));
  return data;
}

void cGEM::PrintList()
{
  printf(" lAPVid\txy\tnch\tnsa\tcAPVptr\n"); 
  printf("==================================================\n");
  for (int i=0; i<nAPVs; i++)
    printf(" %2d\t%d\t%3d\t%3d\t%p\n", 
	   lAPVid[i], xy[i], 
	   APV[i]->GetNchannels(), APV[i]->GetNsamples(), APV[i]);
  return;
}

void cGEM::swap(int *a, int *b)
{
  int t = *a;
  *a=*b;
  *b=t;
}

void cGEM::swaparray(int *array, int size)
{
  for (int j=0; j<size/2; j++)
    swap( &(array[j]), &(array[size-j-1]) );
}
