#ifndef _TSUPERCLASSES_HXX_
#define _TSUPERCLASSES_HXX_

#include <TApplication.h>
#include <TROOT.h>
#include <TVirtualX.h>

#include <TGListBox.h>
#include <TGFrame.h>
#include <TGNumberEntry.h>
#include <TGDoubleSlider.h>
#include <TGLabel.h>
#include <TGTableLayout.h>
#include <TCanvas.h>
#include <TButton.h>
#include <TClass.h>
#include <TH1.h>
#include <TH2.h>
#include <TLine.h>
#include <TGraphErrors.h>
#include <TRandom.h>
#include <TMath.h>

#include "printfcolors.h"

//##################################################################################################################//
//##################################################################################################################//
class TGSuperListBox : public TGListBox
{
  // "Super"-ListBox class. Just like TGListBox, but it can return
  // a list of all entries - not only the selected ones.
public:
  TGSuperListBox(const TGWindow* p = 0, Int_t id = -1, UInt_t options = kSunkenFrame|kDoubleBorder, Pixel_t back = GetWhitePixel());
  virtual ~TGSuperListBox();
  inline void GetListOfEntries(TList *list)
  { // this method was derived from TGLBContainer::GetSelectedEntries
    TGLBEntry      *f;
    TGFrameElement *el;
    TIter next(fLbc->GetList());
    while ((el=(TGFrameElement*)next()))
      {
        f = (TGLBEntry*)el->fFrame;
        list->Add(f);
      }
  };
  // For some "Fons only knows"-reason the following code is only sometimes working properly...
  // When having all entries selected, de-selecting two rather often leads to the situation that
  // toggling the selection will end with list with only one entry selected. The last clicked entry
  // will not be selected anymore... Sometimes even different, but still strange behyviour occurs.
  // Neither TGListBox::GetSelection(id) nor TGLBEntry::IsActive() work, they both return the
  // (sometimes) wrong result!
  inline void ToggleSelection()
  {
    //printf("\n");
    TGLBEntry      *f;
    TGFrameElement *el;
    TIter next(fLbc->GetList());
    while ((el=(TGFrameElement*)next()))
      {
        f = (TGLBEntry*)el->fFrame;
        int id = f->EntryId();
        //Bool_t s = GetSelection(id);
        Bool_t s = f->IsActive();
        //printf ("id %d: %d\n", id, s);
        if (s==kTRUE)
          Select(id, kFALSE);
        else
          if (s==kFALSE)
            Select(id, kTRUE);
        //else
        //printf("höhö!\n");
        //Select(id, !s);
      }
    //printf("\n");
    Layout();
  };
};
//##################################################################################################################//
//##################################################################################################################//
class TGRangeSelector : public TGMainFrame
{
public:
  TGRangeSelector(TGCompositeFrame *where, TGCompositeFrame *parent, const char* title, int width, int digitwidth,
                  double sliderangefrom, double sliderangeto,
                  int entryrangefrom, int entryrangeto,
                  long cmdSlider, long cmdEntry, int TLx0, int TLy0);
  virtual ~TGRangeSelector();
  void inline SetEntryFormat(TGNumberFormat::EStyle style,
                             TGNumberFormat::EAttribute attr=TGNumberFormat::kNEAAnyNumber) 
  { entry[0]->SetFormat(style, attr); entry[1]->SetFormat(style, attr); };
  void inline GetRange(double *from, double *to) { *from=slider->GetMinPosition(); *to=slider->GetMaxPosition(); };
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
private:
  TGCompositeFrame *mywindow;
  TGCompositeFrame *myparent;
  const char  *mytitle;
  int       pixelwidth;
  double    sliderfrom, sliderto, sliderrange;
  double    entryfrom, entryto, entryrange;
  long      parmSlider, parmEntry;
  int       cx, cy;
  TGDoubleHSlider *slider;
  TGNumberEntry   *entry[2];
};
//##################################################################################################################//
//##################################################################################################################//
class TSuperCanvas : public TCanvas
{
public:
  TSuperCanvas(const char* name, const char* title, Int_t ww, Int_t wh, Float_t menumargin=0.0);
  TSuperCanvas(const char* name, const char* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh, Float_t menumargin=0.0);
  virtual ~TSuperCanvas();
  virtual void Initialize();
  virtual void SetQuitOnClose(Bool_t quit=true);
  virtual void ClearPointers();
  virtual void Divide(Int_t nx=1, Int_t ny=1, Float_t xmargin=0.01, Float_t ymargin=0.01, Int_t color=0, Float_t menumargin=-1.0);
  void MakeMenu(TSuperCanvas **me, Float_t menumargin=-1.0);
  virtual void HandleInput(EEventType event, Int_t px, Int_t py);
  virtual void Measure(EEventType event, Int_t px, Int_t py);
private:
  TGTransientFrame *ConfigureDialog;
  void Configure();
  void DrawEventStatus(Int_t event, Int_t px, Int_t py, TObject *selected);
  Float_t  menu_margin;
  Int_t    meas_status;
  Float_t  meas_x1, meas_x2, meas_y1, meas_y2;
  Float_t  peakpos[10];
  TLine   *peakline[10];

  Bool_t   QuitOnClose;

  double   SamplingRate, ScrollRate;

  TVirtualPad *MaximizedPad;
  Double_t MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup;

  TButton *closebutton, *crossbutton, *eventbutton, *toolbutton, *logxbutton, *logybutton;
};
//##################################################################################################################//
//##################################################################################################################//
// class TMCH1 : public TH1
// {
// public:
//   TMCH1(const TH1&);
//   inline virtual ~TMCH1() {};
//   Double_t GetRandom(TRandom *generator=NULL);
// };
//##################################################################################################################//
//##################################################################################################################//
// class TTiming : public TH1
// {
// public:
//   TTiming(const TH1&);
//   inline virtual ~TTiming() {};
//   TH1* GetHisto(int bins=0);
// };
//##################################################################################################################//
//##################################################################################################################//

#endif

