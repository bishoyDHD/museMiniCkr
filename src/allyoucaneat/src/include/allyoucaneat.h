#ifndef __ALLYOUCANEAT_H__
#define __ALLYOUCANEAT_H__

#include "TGClient.h"
#include "TGButton.h"
#include "TGStatusBar.h"
#include "TGTab.h"
#include "TGNumberEntry.h"
#include "TGListBox.h"
#include "TGDoubleSlider.h"
#include "TGLabel.h"
#include "TRootEmbeddedCanvas.h"
#include "TGListTree.h"
#include "TGProgressBar.h"
#include "chef.h"
#include "TH1.h"
#include "TGColorSelect.h"
#include "TString.h"
#include "TTimer.h"

#include <string>
#include <vector>

#include <RQ_OBJECT.h>

using namespace std;

class AYCEgui : public TGMainFrame
{
 public:
  AYCEgui(const TGWindow *p, UInt_t w, UInt_t h);
  ~AYCEgui();
  void DynamicButtonHandler(int id);
  void PushWatchList();
  void ListPendingNearOnline();
  void ServiceRefRunRadioButtons();
  void SetRunnumber(int runno);
  void RunnumberChanged();
  int  FillRunList();
  void ForceCooking();
  void RunScript(const char *scriptname);

  struct guielement
  {
    string    type;
    TGLabel  *label;
    TGFrame  *element;
  };

  struct dynamicbutton
  {
    int id;
    TGTextButton *button;
    std::vector <guielement> guielements;
    string name, commandtype, command;
  };

 private:
  std::string FILENAME_DYNAMICBUTTONS_CONFIG;
  int debug;
  int lastrunnumber;
  int SelectedRefRun;
  int refrun[4];

  Pixel_t red, green, blue, orange, yellow, gray, white;

  TGNumberEntry *NERunnumber, *NERunListLength, *NEMichelinReference[4];
  TGPictureButton *PBBurger;
  TGRadioButton *RBRefRun[4];
  TGCheckButton *CBMichelinHeadless;
  TGListBox     *LBRunList;
  TGTextButton  *TBRefreshRunList;

  std::vector <dynamicbutton> buttonlist;

  TTimer *timer;
  std::vector <int> lastruns;    // list for daemonmode to do nearonline cooking
  std::vector <int> runstowatch; // list for normal mode to pop-up michelin
  std::vector <int> nearonlinependinglist; // list of runs without completed nearonline
  int  GetSelectedRefRun();
  void DaemonAction(std::vector<int>);
  void NormalmodeAction();
  int  FileExists(const char *filename);

  int LoadDynamicButtons(TGCompositeFrame*, int, int);
  char *IgnoreWhitespace(char *line)
  {
    line[strlen(line)-1]='\0';
    unsigned int i=0;
    for  (i=0; i<strlen(line); i++)
      {
        if (line[i]==' ')  continue;
        if (line[i]=='\t') continue;
        break;
      };
    return line+i;
  };

  RQ_OBJECT("AYCEgui")
    ClassDef(AYCEgui,1);
};

#endif
