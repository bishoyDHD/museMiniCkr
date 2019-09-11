//!        ===== cAPV =====
//!
//!
//! 
//!    Class to handle APV raw data
//!
//! The class handles raw data from an APV25
//! chip. The user can specify which of the
//! APV input pins are actually connected to
//! detector strips and are to be passed on
//! by the output routines.
//! The re-mapping from APV data stream order
//! back to APV input pin order is taken care
//! of. The data are referred to as datawords
//! to as long as they are in the APV (analog)
//! datastream order, and to as samples when
//! in APV input pin order.
//!
//! For testing purposes it is possible to
//! disable re-ordering by calling the AddData()
//! methods with reorder=false.
//!

#ifndef _cAPV_h_
#define _cAPV_h_

class cAPV
{
 public:
  cAPV(int numberofsamples=128);
  ~cAPV();

  int SkipFirst(int nskip);
  int SkipLast(int nskip);
  int SkipSamples(int *chskip, int size);
  int SkipSample(int sample);

  enum eSampleModification { None=0, Zero, Average, Baseline };
  int ModifySamples(int *chmod, int size, eSampleModification m);

  int Clear();

  int AddData(int *data, bool reorder=true); 
  int AddData(int wordid, int data, bool reorder=true); 

  int GetData(int sample);
  int GetData(int *data);

  int GetNsamples();
  int GetNchannels();
  int wordid2sample(int wordid);

 private:
  int  nsamples;
  int *dat;
  int *skip;
  int *modi;

}; 

#endif
