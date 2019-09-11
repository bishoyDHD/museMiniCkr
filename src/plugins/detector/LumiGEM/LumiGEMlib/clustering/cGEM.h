//!        ===== cGEM =====
//!
//!
//! 
//!    Class to process GEM data
//!
//! The class processes data from a GEM with a
//! number "nAPVs" of APVs connected to it.
//!
//! APV raw data need to be provided in "APV order" and will be
//! distributed automatically to internally created cAPV
//! objects. Re-mapping from APV order to physical
//! APV input order is done by the APV objects, as well as skipping
//! unused APV input channels (e.g. channels that are not connected to
//! detector readout strips) when pushing data from APV objects into the
//! GEM class internal data buffers.
//!
//! APV channels can also be modified (e.g. zeroing, averaging) in
//! order to suppress noise hits from noisy or defective APV or detector
//! channels.
//! 
//! Before retrieving copies of the data buffers the class will flip
//! the contents of the data buffers if requested by the user in order to
//! match the orientation of the GEM readout axes.
//!
//! For further processing by other code lists of the number of
//! samples that originate from each APV of an axis are passed to the user
//! by the GetNchannelsX/Y() methods. This can be helpful for example
//! to process baseline subtraction that needs to be performed "per-APV"
//! and not "per-axis".
//!



#ifndef _cGEM_h_
#define _cGEM_h_

#include<cstddef>

#include "cAPV.h"

class cGEM
{
  ////////////////////////////////////////////////////////////
 public:
  cGEM(int numberofAPVs, int samplesperAPV=128);
  ~cGEM();

  //! Add an APV to the list of APVs, either in x or in y,
  //! smples number ascending in positive or negative x/y
  //! direction (order=+1,-1), skip first "skip" samples (i.e.
  //! not connected APV input pins):
  int AddAPVx(int lAPVnum, int skip=0); 
  int AddAPVy(int lAPVnum, int skip=0);

  //! Import APV data for logical APV #lAPVnum:
  int AddAPVData(int lAPVnum, int *data);

  //! Ask all APVs to clear/zero their data arrays:
  int ClearAPVs();

  //! Get pointer to x or y data, returns number of
  //! channels in use for x or y:
  int APVDataPointerX(int *data);
  int APVDataPointerY(int *data);

  //! Define APV channels to be skipped when pushing data into GEM class
  int SkipFirstAPVChannels(int lAPVnum, int skip);
  int SkipLastAPVChannels(int lAPVnum, int skip);
  int SkipAPVChannels(int lAPVnum, int *channelstoskip, int size);

  //! Change orientation of axes
  void SwapXaxis(bool swapping=true) {swapx=swapping;};
  void SwapYaxis(bool swapping=true) {swapy=swapping;};

  //! Apply modifications (e.g. averaging, zeroing) to certain APV channels
  int ModifyAPVChannels(int lAPVnum, int *channelstomodify, int size, cAPV::eSampleModification m);
  int ModifyAPVChannels(int lAPVnum, const char *channelstomodifystr, int size, cAPV::eSampleModification m);

  //! Get number of channels for axes
  int GetNchannelsX();
  int GetNchannelsY();

  //! Push data from internally used APV classes into GEM class data buffers
  int PushXdata(bool swapping); int PushXdata() {return PushXdata(swapx);};
  int PushYdata(bool swapping); int PushYdata() {return PushYdata(swapy);};

  //! Retrieve copy of internal data buffers
  int* GetXdata(int *data=NULL);
  int* GetYdata(int *data=NULL);

  //! Get list of number of samples belonging to separate APVs for axes
  int* GetNXsamplesList() {return xnsamplelist;};
  int* GetNYsamplesList() {return ynsamplelist;};

  //! Print list of internally used APV classes and their configuration
  void PrintList();

  //! Enumerator type for GEM detector readout axes
  enum eGEMaxis
  {
    undefined=-1,
    xaxis,
    yaxis
  };
  ////////////////////////////////////////////////////////////
 protected:
  ////////////////////////////////////////////////////////////
 private:
  int AddAPV(int lAPVnum,  int skip=0);
  void swap(int *a, int *b); 
  void swaparray(int *array, int size);
  ////////////////////////////////////////////////////////////
  //! Data members:
  int    nAPVs, nSamplesAPV;
  cAPV **APV;
  int   *lAPVid, *xy, *nchannels; 
  int   *xdata, *ydata;
  int   *xnsamplelist, *ynsamplelist;
  int    nxch, nych;
  int    nxapvs, nyapvs;
  bool   swapx, swapy;
  ////////////////////////////////////////////////////////////
};

#endif
