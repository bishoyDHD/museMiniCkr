#include "TGFrame.h"
#include "TGButton.h"
#include "TGListBox.h"
#include "TSuperClasses.h"
#include "TH1.h"
#include "TTimer.h"
#include <string>

class CompareWindow:public TGMainFrame
{
 private:
  bool headless;

  TGTextButton *leftbtn, *rightbtn, *palettebtn;
  TGListBox *LBhistos;
  TSuperCanvas *sc;

  TTimer *timer;
  int     watchdogcounter;

  int palette;

  int thisrunno, refrunno;

  void HistoOperation(int operation, TH1* h1, TH1* h2);
  void SetStyle(TH1 *histo, std::string style, int samepad=0);
  void presentHisto();
  void loadHistos();
  unsigned int  currentHisto;
  std::vector<TH1*> histosleft;
  std::vector<TH1*> histosright;
  std::vector<std::string> histname;
  std::vector<std::string> drawopts;
  std::vector<std::string> styleopts;
  
  enum hoperator
  {
    none=0,
    add,
    subtract,
    multiply,
    divide
  };

 public:
  void Watchdog();
  void WatchdogReset();
  void SelectHisto(UInt_t i);
  void skip(int i);
  void TogglePalette();
  void ZoomPad(int);
  CompareWindow(const TGWindow *p, UInt_t w, UInt_t h,bool gui=true);
  ~CompareWindow();
};
