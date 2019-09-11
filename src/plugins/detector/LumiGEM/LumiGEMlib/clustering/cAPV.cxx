#include "cAPV.h"

#include<cstddef>
#include <stdio.h>
#include <vector>


cAPV::cAPV(int numberofsamples)
//! Constructor for cAPV class.
//!   numberofsamples: number of samples to be processed later by the
//!                    cAPC object
{
  nsamples = numberofsamples;
  // allocate memory for APV samples:
   dat  = new int[nsamples];
   skip = new int[nsamples];
   modi = new int[nsamples];

  // std::vector<int> dat;
  // std::vector<int> skip;
  // std::vector<int> modi;
  // dat.resize(nsamples);
  // skip.resize(nsamples);
  // modi.resize(nsamples);
  // initialize everything:
  for (int i=0; i<nsamples; i++)
    {
      dat[i]  = 0;
      skip[i] = 0;
      modi[i]  = 0;
    };
}

cAPV::~cAPV()
//! Destructor for cAPV class.
{
  delete[] dat;

  delete[] skip;

  delete[] modi;
}

int cAPV::SkipFirst(int nskip)
//! Call this method to indicate that the first "skip" input pins of
//! this APV are not connected to detector strips / channels and their
//! data samples should be skipped.
{
  if (nskip<0) return 0;
  if (nskip>nsamples) nskip = nsamples;
  for (int i=0; i<nskip; i++)
    skip[i] = 1;
  return nskip;
}

int cAPV::SkipLast(int nskip)
//! Call this method to indicate that the last "skip" input pins of
//! this APV are not connected to detector strips / channels and their
//! data samples should be skipped.
{
  if (nskip<0) return 0;
  if (nskip>nsamples) nskip = nsamples;
  for (int i=nsamples-nskip; i<nsamples; i++)
    skip[i] = 1;
  return nskip;
}

int cAPV::SkipSamples(int *chskip, int size)
//! Call this method to provide a list of pins of this APV that are
//! not connected to detector strips / channels, so that their data
//! samples will be skipped. The size of the array must be specified
//! by "size".
{
  if (chskip==NULL) return 0;
  for (int i=0; i<size; i++)
    skip[chskip[i]] = 1;
  return size;
}

int cAPV::SkipSample(int sample)
//! Mark a single APV sample to be skipped (e.g. an unconnected APV
//! input channel).
{
  if (sample<0)         return 0;
  if (sample>=nsamples) return 0;
  skip[sample] = 1;
  return 1;
}

int cAPV::ModifySamples(int *chmod, int size, eSampleModification m)
//! Mark APV samples in list "chmod" to be modified by cAPV::GetData().
//! Type of modification will be the same for all samples in the list.
{
  if (chmod==NULL) return 0;
  for (int i=0; i<size; i++)
      modi[chmod[i]] = m;
  return size;
}

int cAPV::wordid2sample(int wordid)
//! Conversion from APV wordid number (APV ordering)
//! to the APV sample number (APV input pin number):
{
  int s = wordid;
  return ( 32*(s%4)+8*int(s/4)-31*int(s/16) );
  // return ( 127-(32*(s%4)+8*int(s/4)-31*int(s/16)) );
}

int cAPV::Clear()
{
  // loop over data array:
  for (int wordid=0; wordid<nsamples; wordid++)
    dat[wordid] = 0;
  return 0;
}

int cAPV::AddData(int *data, bool reorder)
//! Fill internal data array from data array given by user.
{
  if (data==NULL) return 0;
  // loop over data array:
  for (int wordid=0; wordid<nsamples; wordid++)
    if (reorder) // doing the reordering on the fly:
      dat[wordid2sample(wordid)] = data[wordid];
    else // without reordering:
      dat[wordid] = data[wordid];
  return nsamples;
}

int cAPV::AddData(int wordid, int data, bool reorder)
//! Fill a single dataword into internal data array.
{
  if (reorder)
    dat[wordid2sample(wordid)]=data;
  else
    dat[wordid]=data;
  return wordid;
}

int cAPV::GetData(int sample)
//! Retrieve a single data word from internal array.
//! Returns -1 if sample is marked as to be skippped.
{
  if (skip[sample]) return -1;
  return dat[sample];
}

//This is the one used by cGEM.cxx GetData
int cAPV::GetData(int *data)
//! Writes the reordered data into the array "data",
//! skipping those samples that were specified earlier by
//! the Skip...() methods.
{
  int channel=0;
  for (int sample=0; sample<nsamples; sample++)
    if (skip[sample]==0)
      {
	switch (modi[sample])
	  {
	  case None:
	    data[channel] = dat[sample];
	    break;
	  case Zero:
	    data[channel] = 0;
	    break;
	  case Average:
	    if (sample==0)
	      {
		data[channel] = (dat[sample+1]+dat[sample+2])/2;
	      }
	    else if (sample==nsamples)
	      {
		data[channel] = (dat[sample-1]+dat[sample-2])/2;
	      }
	    else
	      {
		// look for next useable sample:
		int k=0;
		for (k=sample+1; k<nsamples; k++) if (skip[sample]==0) break;
		data[channel] = (dat[channel-1]+dat[k])/2;
	      };
	    break;
	  case Baseline:
	    break;
	    // bail out if modification method is not defined:
	    return -1;
	  };
	channel++;
      };
  return channel;
}

int cAPV::GetNsamples()
//! Returns the number of APV samples (without skipping!).
{
  return nsamples;
}

int cAPV::GetNchannels()
//! Returns the number of detector channels, i.e. the number of APV
//! samples minus those to be skipped as their APV input pins are
//! unconnected.
{
  int cnt=0;
  for (int i=0; i<nsamples; i++)
    if (skip[i]==0)
      cnt++;
  return cnt;
}
