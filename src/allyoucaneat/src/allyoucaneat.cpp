/***************************************************************/
/*                                                             */
/*         A L L  Y O U  C A N  E A T                          */
/*                                                             */
/*   A graphical user interface to all your raw and cooked     */
/*   ROOT files                                                */
/*                                                             */
/***************************************************************/

#include <stdlib.h>

#include "TGTableLayout.h"

#include "TApplication.h"
#include "TSystem.h"
#include "TROOT.h"
#include "allyoucaneat.h"
#include "TQObject.h"
#include "TTimer.h"
#include "TColor.h"
#include "TStyle.h"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

int daemonmode, testmode;

const char *PATH_TO_ROOTFILES  = "/data/fall2012";
const char *PATH_TO_NEARONLINE = "/data/fall2012/NearOnline";

AYCEgui::AYCEgui(const TGWindow *p, UInt_t w, UInt_t h):TGMainFrame(p,w,h)
{
  FILENAME_DYNAMICBUTTONS_CONFIG = "src/allyoucaneat/dynamicbuttons.conf";

  gClient->GetColorByName("red",    red);
  gClient->GetColorByName("green",  green);
  gClient->GetColorByName("blue",   blue);
  gClient->GetColorByName("orange", orange);
  gClient->GetColorByName("yellow", yellow);
  gClient->GetColorByName("gray",   gray); gray-=1;
  gClient->GetColorByName("white",  white);

  TGCompositeFrame *baseframe = new TGCompositeFrame(this);
  baseframe->SetLayoutManager(new TGVerticalLayout(baseframe));

  // frame for static gui elements:
  TGGroupFrame *FrStaticButtons = new TGGroupFrame(baseframe, " General ");
  FrStaticButtons->SetLayoutManager(new TGTableLayout(FrStaticButtons, 18, 14));
  {
    int cx=0, cy=0;
    TGLabel *l = new TGLabel(FrStaticButtons, "Runnumber ");
    FrStaticButtons->AddFrame(l, new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
    NERunnumber = new TGNumberEntry(FrStaticButtons, 4393);
    FrStaticButtons->AddFrame(NERunnumber, new TGTableLayoutHints(cx, cx+2, cy, cy+1)); cx+=2;
    NERunnumber->Connect("ValueSet(Long_t)", "AYCEgui", this, "RunnumberChanged()");

    LBRunList = new TGListBox(FrStaticButtons);
    FrStaticButtons->AddFrame(LBRunList, new TGTableLayoutHints(cx, cx+4, 0, 12));
    LBRunList->Resize(250, 180);
    LBRunList->Connect("DoubleClicked(Int_t)", "AYCEgui", this, "SetRunnumber(int)");

    TBRefreshRunList = new TGTextButton(FrStaticButtons, "refresh");
    TBRefreshRunList->SetBackgroundColor(gray);
    FrStaticButtons->AddFrame(TBRefreshRunList, new TGTableLayoutHints(cx, cx+3, 10, 11, kLHintsFillX));
    TBRefreshRunList->Connect("Clicked()", "AYCEgui", this, "FillRunList()");

    NERunListLength = new TGNumberEntry(FrStaticButtons, 500);
    FrStaticButtons->AddFrame(NERunListLength, new TGTableLayoutHints(cx+3, cx+4, 10, 11, kLHintsExpandX));

    cx=0; cy++;
    PBBurger = new TGPictureButton(FrStaticButtons, "src/allyoucaneat/mcdonalds-Big-Mac_small.xpm");
    FrStaticButtons->AddFrame(PBBurger, new TGTableLayoutHints(cx, cx+2, cy, cy+2)); cy+=2;
    PBBurger->Resize(100,100);
    PBBurger->SetCommand(".! firefox http://www.hallo-pizza.de");

    // TGTextButton *TBBrowser = new TGTextButton(FrStaticButtons, "new TBrowser");
    // TBBrowser->SetBackgroundColor(gray);
    // FrStaticButtons->AddFrame(TBBrowser, new TGTableLayoutHints(cx, cx+2, cy, cy+1, kLHintsFillX)); cy++;
    // TBBrowser->SetCommand("new TBrowser();");
    // //TBBrowser->SetFont("-adobe-helvetica-medium-r-*-*-14-*-*-*-*-*-iso8859-1", kTRUE);

    TGTextButton *TBPushWatchList = new TGTextButton(FrStaticButtons, " push to Michelin ");
    TBPushWatchList->SetBackgroundColor(gray);
    FrStaticButtons->AddFrame(TBPushWatchList, new TGTableLayoutHints(cx, cx+2, cy, cy+1, kLHintsFillX)); cy++;
    TBPushWatchList->Connect("Clicked()", "AYCEgui", this, "PushWatchList()");

    FrStaticButtons->AddFrame(new TGLabel(FrStaticButtons, "Michelin Reference Run: "),
			      new TGTableLayoutHints(cx, cx+2, cy, cy+1)); cy++;
    char labelstr[4][32] = {"B+ e+ ", "B+ e- ", "B- e+ ", "B- e- "};
    //int  refrun[4]       = { 5553,     5540,     5662,      5702  };
    refrun[0] = 5553;
    refrun[0] = 5540;
    refrun[0] = 5662;
    refrun[0] = 5702;
    for (int i=0; i<4; i++)
      {
	RBRefRun[i] = new TGRadioButton(FrStaticButtons, labelstr[i]);
	FrStaticButtons->AddFrame(RBRefRun[i], new TGTableLayoutHints(cx, cx+1, cy, cy+1));
	NEMichelinReference[i] = new TGNumberEntry(FrStaticButtons, refrun[i]);
	FrStaticButtons->AddFrame(NEMichelinReference[i], new TGTableLayoutHints(cx+1, cx+2, cy, cy+1)); cy++;
	RBRefRun[i]->Connect("Clicked()", "AYCEgui", this, "ServiceRefRunRadioButtons()");
      };
    RBRefRun[0]->SetState(kButtonDown);
    SelectedRefRun = 0;

    CBMichelinHeadless = new TGCheckButton(FrStaticButtons, "headless");
    FrStaticButtons->AddFrame(CBMichelinHeadless, new TGTableLayoutHints(cx, cx+2, cy, cy+1)); cy++;

    TGTextButton *TBListPendNearOnline = new TGTextButton(FrStaticButtons, "List Pending Runs");
    TBListPendNearOnline->SetBackgroundColor(gray);
    FrStaticButtons->AddFrame(TBListPendNearOnline, new TGTableLayoutHints(cx, cx+2, cy, cy+1, kLHintsFillX)); cy++;
    TBListPendNearOnline->Connect("Clicked()", "AYCEgui", this, "ListPendingNearOnline()");
  }
  FrStaticButtons->Resize(800, 50);
  baseframe->AddFrame(FrStaticButtons, new TGLayoutHints(kLHintsFillX, 5, 5, 5, 5));

  int configerrcnt=0;
  if (!daemonmode)
    {
      // frame for "dynamic buttons" (connected to external macro files):
      TGGroupFrame *FrDynamicButtons = new TGGroupFrame(baseframe, " Analysis Plots ");
      configerrcnt = LoadDynamicButtons(FrDynamicButtons, 14, 6); // nrows=8, ncols=6 for table layouts
      baseframe->AddFrame(FrDynamicButtons, new TGLayoutHints(kLHintsFillX, 5, 5, 5, 5));
      // 
      if (configerrcnt)
      	{
      	  TGLabel *label=new TGLabel(baseframe,
      				     Form("    %d syntax errors in %s\n",
      					  configerrcnt, FILENAME_DYNAMICBUTTONS_CONFIG.c_str()));
      	  label->SetTextColor(red);
      	  baseframe->AddFrame(label);
      	};
      LBRunList->SetMultipleSelections(kFALSE);
    }
  else
    {
      baseframe->AddFrame(new TGLabel(baseframe, "\n           running in daemon mode...\n"));
      TGTextButton *TBForceNearOnline =
	new TGTextButton(baseframe, " Force Near Online Cooking \n      for Selected Run");
      TBForceNearOnline->SetBackgroundColor(gray);
      baseframe->AddFrame(TBForceNearOnline, new TGLayoutHints(kLHintsExpandX, 3, 3, 3, 3));
      TBForceNearOnline->Connect("Clicked()", "AYCEgui", this, "ForceCooking()");
      LBRunList->SetMultipleSelections(kTRUE);
    };

  AddFrame(baseframe);
  baseframe->Resize();

  SetWindowName("All You Can Eat");
  MapSubwindows();
  Resize(GetDefaultWidth(), GetDefaultHeight());
  MapWindow();

  debug = 0;

  lastrunnumber = -1;

  lastruns.clear();
  runstowatch.clear();
  FillRunList();
  if (daemonmode)
    timer = new TTimer(5000);
  else
    timer = new TTimer(20000);
  timer->Connect("Timeout()", "AYCEgui", this, "FillRunList()");
  timer->Start();
}

AYCEgui::~AYCEgui()
{
  timer->Stop();
  Cleanup();
  gApplication->Terminate();
}

void AYCEgui::DynamicButtonHandler(int id)
{
  printf("button %d: >%s<\n", id, buttonlist[id].name.c_str());
  dynamicbutton button = buttonlist[id];
  int    runno = NERunnumber->GetNumber();
  string cmd = "";
  if (button.commandtype=="Macro")
    {
      cmd += "xterm -geometry 120x20 -e root -l \"src/allyoucaneat/macros/" + button.command + "+(" + Form("%d", runno);
      //printf("  --- processing %d gui elements\n", button.guielements.size());
      for (unsigned int i=0; i<button.guielements.size(); i++)
	{
	  guielement ele = button.guielements[i];
	  double val=0.;
	  if ( (ele.type=="NumberEntry")||(ele.type=="NumberEntryHex") )
	    val = ((TGNumberEntry*)(ele.element))->GetNumber();
	  cmd += Form(", %lf", val);
	};
      cmd += ")\" &"; // close bracket and send xterm with macro execution to background
    }
  else
    if (button.commandtype=="Script")
      {
	cmd += "src/allyoucaneat/macros/" + button.command + Form(" %d", runno) + " ";
      }
    else
      printf(" * ERROR * - Cannot execute unknown type of command >%s<...\n", button.commandtype.c_str());

  printf(" Executing command >%s< now!\n", cmd.c_str());
  gSystem->Exec(cmd.c_str());
}

void AYCEgui::SetRunnumber(int runno)
{
  NERunnumber->SetNumber(runno);
}

void AYCEgui::PushWatchList()
{
  int runnumber = NERunnumber->GetNumber();
  for (unsigned int i=0; i<runstowatch.size(); i++)
    {
      if (runstowatch[i]==runnumber)
	{
	  printf(" Already watching run #%d!\n", runnumber);
	  return;
	};
    };
  runstowatch.push_back(runnumber);
  printf(" Pushed run #%d to list of runs to watch for nearonline to finish!\n", runnumber);
  NormalmodeAction ();
  return;
}

void AYCEgui::ListPendingNearOnline()
{
  printf("\n===== Runs with pending Nearonline analysis =====\n");
  printf("-------- (normal data taking runs only!) --------\n");
  for (unsigned int i=0; i<nearonlinependinglist.size(); i++)
    printf(" %6d\n", nearonlinependinglist[i]);
  if (nearonlinependinglist.size()==0)
    printf(" --- none ---\n");
  printf("=================================================\n");
}

void AYCEgui::RunnumberChanged()
{
  if (!daemonmode)
    return;
  int runno=NERunnumber->GetNumber();
  LBRunList->SetMultipleSelections(kFALSE);
  LBRunList->Select(runno);
  LBRunList->SetMultipleSelections(kTRUE);
}

void AYCEgui::ServiceRefRunRadioButtons()
{
  for (int i=0; i<4; i++)
    {
      if ( (SelectedRefRun!=i)&&(RBRefRun[i]->GetState()==kButtonDown) )
	{
	  SelectedRefRun = i;
	  for (int j=0; j<4; j++)
	    if (SelectedRefRun!=j)
	      RBRefRun[j]->SetState(kButtonUp);
	  break;
	};
    };
  RBRefRun[SelectedRefRun]->SetState(kButtonDown);
}

int AYCEgui::GetSelectedRefRun()
{
  for (int i=0; i<4; i++)
    if (RBRefRun[i]->GetState()==kButtonDown)
      return i;
  return -1;
}

Int_t AYCEgui::LoadDynamicButtons(TGCompositeFrame *pframe, int nrows, int ncols)
{
  printf("loading dynamic buttons from >%s<...\n", FILENAME_DYNAMICBUTTONS_CONFIG.c_str());
  // open dynamic buttons config file:
  FILE *conffile=NULL;
  if ((conffile=fopen(FILENAME_DYNAMICBUTTONS_CONFIG.c_str(), "r"))==NULL)
    {
      printf("Cannot open dynamic buttons config file >%s<...\n",
             FILENAME_DYNAMICBUTTONS_CONFIG.c_str());
      ((TGGroupFrame*)pframe)->SetTitle(" RUNTIME ERROR ");
      pframe->AddFrame(new TGLabel(pframe, "*** Couldn't find dynamic buttons config file!\n*** Please start program from top level source directory!\n     For example:\n        > cd ~/olympus_release/\n        > allyoucaneat"),
		      new TGTableLayoutHints(0, 1, 0, 1, kLHintsFillX|kLHintsFillY));
      return 1;
    };
  // Buttons will be added to "frame" which may or may not be the same as "pframe":
  TGCompositeFrame *frame = pframe;
  // As soon as "NewTab" appears in the config file we will create a TGTab frame inside "pframe"
  // and add tabs to that one using TGTab::AddTab()
  TGTab *Tabs=NULL;
  bool tablelayout=false;

  // read configuration:
  Int_t cx=0, cy=0;
  //char *line=NULL, buff[1024], *here=NULL;
  char line[1025], buff[1024], *here=NULL; // fix for MacOS
  size_t length=0;
  TGTextButton *thisbutton=NULL;
  int id=0, errcnt=0;
  bool complexbutton=false;
  //vector <guielement> guielements;
  string macroname="", scriptname="", buttoncommand="";
  dynamicbutton abutton; // temp dynamicbutton used to push new ones to the buttonlist
  while (!feof(conffile))
    {
      length=fscanf(conffile, "%s", buff);
      // ignore comments:
      if ((buff[0]=='#')||(buff[0]=='%'))
        {
	  length = strlen(fgets(line, 1024, conffile)); // fix for MacOS
          continue;
        };
      //////////////////////////////////////////////////////////////////////
      if (!strcmp(buff, "END"))
	break;
      //////////////////////////////////////////////////////////////////////
      if (!strcmp(buff, "RefRuns"))
	{
	  if (strlen(fgets(line, 1024, conffile)))
	    {
	      sscanf(line, "%d %d %d %d",
		     &(refrun[0]),  &(refrun[1]),  &(refrun[2]),  &(refrun[3]));
	      printf(" Found reference runs %d %d %d %d in config file\n",
		     refrun[0], refrun[1], refrun[2], refrun[3]);
	      for (int i=0; i<4; i++)
		NEMichelinReference[i]->SetNumber(refrun[i]);
	    };
	  continue;
	};
      //////////////////////////////////////////////////////////////////////
      if ( !strcmp(buff, "NewTab"))
	{
	  if (strlen(fgets(line, 1024, conffile))) // fix for MacOS
	    {
	      if (!Tabs)
		{
		  Tabs = new TGTab(pframe);
		};
	      here=IgnoreWhitespace(line);
	      cout << " new Tab: >" << here << "<" << endl;
	      frame = Tabs->AddTab(here);
	      UInt_t n = Tabs->GetNumberOfTabs();
	      UInt_t color = TColor::RGB2Pixel(255-n*32, 192, 64+n*16);
	      Tabs->GetTabTab(here)->SetBackgroundColor(color);//gray+Tabs->GetNumberOfTabs()*8);
	      frame->SetLayoutManager(new TGTableLayout(frame, nrows, ncols));
	      cx=0; cy=0;
	    }
	  else
	    printf(" Syntax Error - Missing Tab name\n");
	  continue;
	};
      //////////////////////////////////////////////////////////////////////
      if ( (!strcmp(buff, "SimpleButton")) ||
	   (!strcmp(buff, "ComplexButton"))   )
        {
	  if ((Tabs==NULL)&&(tablelayout==false))
	    pframe->SetLayoutManager(new TGTableLayout(pframe, nrows, ncols));
	  if (strlen(fgets(line, 1024, conffile))) // fix for MacOS
            {
	      if (!strcmp(buff, "SimpleButton"))
		complexbutton=false;
	      else if (!strcmp(buff, "ComplexButton"))
		complexbutton=true;
              here=IgnoreWhitespace(line);
              cout << " new " << buff << ": >" << here << "<" << endl;
              thisbutton = new TGTextButton(frame, here, id);
	      thisbutton->SetBackgroundColor(gray);
	      abutton.id = id;
	      abutton.name = here;
	      abutton.button = thisbutton;
	      abutton.guielements.clear();
	      abutton.commandtype="";
	      abutton.command="";
              id++;
            }
          else
            printf(" Syntax Error - Missing Button title...\n");
	  continue;
        };
      //////////////////////////////////////////////////////////////////////
      if (!strcmp(buff, "ToolTip"))
        {
	  if (strlen(fgets(line, 1024, conffile))) // fix for MacOS
            {
              line[strlen(line)]='\0';
              if (thisbutton!=NULL)
                thisbutton->SetToolTipText(line);
              else
                printf(" Syntax Error - Cannot set Tooltip without Button!\n");
            }
          else
            printf(" Syntax Error - Missing ToolTip text...\n");
	  continue;
        };
      //////////////////////////////////////////////////////////////////////
      if (!strcmp(buff, "Script"))
	{
	  abutton.commandtype = buff;
	  if (fscanf(conffile, "%s", buff))
	    {
	      abutton.command = buff;
	      if (thisbutton!=NULL)
		scriptname = buff;
	      else
		printf(" Syntax Error - Cannot set Script without Button!\n");
	    }
	    else
	      printf(" Syntax Error - Missing Script filename...\n");
	  continue;
	};
      //////////////////////////////////////////////////////////////////////
      if (!strcmp(buff, "Macro"))
        {
	  abutton.commandtype = buff;
          if (fscanf(conffile, "%s", buff))
            {
	      abutton.command = buff;
              if (thisbutton!=NULL)
		macroname = buff;
              else
                printf(" Syntax Error - Cannot set Macro without Button!\n");
            }
          else
            printf(" Syntax Error - Missing Macro filename...\n");
	  continue;
        };
      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% SPECIAL STUFF FOR COMPLEX BUTTONS
      if (complexbutton)
	{
	  if ( (!strcmp(buff, "NumberEntry")) ||
	       (!strcmp(buff, "NumberEntryHex")) )
	    {
	      double defaultnum=0.0;
	      int    retval, intdefault=0;
	      if (strstr(buff, "Hex"))
		{
		  retval=fscanf(conffile, "%x", &intdefault);
		  defaultnum = intdefault;
		}
	      else
		{
		  retval=fscanf(conffile, "%lf", &defaultnum);
		};
	      if (strlen(fgets(line, 1024, conffile))) // fix for MacOS
		{
		  printf(" Number Entry: %s\n", line);
		  guielement ele;
		  ele.type = buff;
		  ele.label = new TGLabel(frame, line);
		  ele.element = new TGNumberEntry(frame, defaultnum, 6);
		  if (strstr(buff, "Hex"))
		    ((TGNumberEntry*)ele.element)->SetFormat(TGNumberFormat::kNESHex);
		  abutton.guielements.push_back(ele);
		}
	      else
		printf(" Syntax Error - Missing NumberEntry description...\n");
	      continue;
	    };
	};
      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      if (!strcmp(buff, "CreateButton"))
        {
          int wx=0, wy=0;
          if (fscanf(conffile, "%d %d", &wx, &wy)==0)
            {
              printf(" Syntax Error - Missing button size, assuming 1/1...\n");
              wx=1; wy=1;
            };
          frame->AddFrame(thisbutton, new TGTableLayoutHints(cx, cx+wx, cy, cy+wy,
                                                             kLHintsFillX|kLHintsFillY)); cx+=wx;
          thisbutton->Associate(this);
          thisbutton->Resize(wx*100, wy*20);
	  if ((macroname=="doesntexistyet")||(scriptname=="doesntexistyet"))
	    thisbutton->SetEnabled(kFALSE);
	  if (complexbutton)
	    {
	      for (unsigned int i=0; i<abutton.guielements.size(); i++)
		{
		  frame->AddFrame(abutton.guielements[i].label, new TGTableLayoutHints(cx, cx+1, cy, cy+1,
									       kLHintsFillX|kLHintsExpandY)); cx++;
		  frame->AddFrame(abutton.guielements[i].element, new TGTableLayoutHints(cx, cx+1, cy, cy+1,
										 kLHintsFillX|kLHintsExpandY)); cx++;
		};
	    };

	  buttonlist.push_back(abutton);
	  //printf(" --- pushed button >%s< with %d gui elements\n", abutton.name.c_str(), abutton.guielements.size());
	  abutton.guielements.clear();

	  thisbutton->Connect("Clicked()", "AYCEgui", this, 
			      Form("DynamicButtonHandler(=%d)", (int)(buttonlist.size()-1)));

	  macroname="";
	  scriptname="";
	  complexbutton=false;
          cy++; cx=0;
	  continue;
        };
      //////////////////////////////////////////////////////////////////////
      printf(" SYNTAX ERROR - cannot parse >%s<\n", buff);
      errcnt++;
      //////////////////////////////////////////////////////////////////////
    };
  fclose(conffile);
  
  if (Tabs)
    pframe->AddFrame(Tabs, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY));

  return errcnt;
}

void AYCEgui::RunScript(const char *scriptname)
//! Simply wraps the TSystem::Exec() call as I did not get it work
//! without this.
//! If one wants to run a shell script with more arguments than just
//! the runnumber this methods needs to be overloaded.
{
  gSystem->Exec( Form("%s %d\n", scriptname, int(NERunnumber->GetNumber())) );
}

int AYCEgui::FillRunList()
{
  // first, check reference run:
  if (CBMichelinHeadless->GetState()==kButtonDown) // michelin headless ==> load refrun from file
    {
      FILE *rrf=fopen(".refrunselect", "r");
      if (rrf)
	{
	  int val=0, runno=0;
	  fscanf(rrf, "%d %d", &val, &runno);
	  printf(" Refrun: %d %d\n", val, runno);
	  if ((val>=0)&&(val<4))
	    for (int i=0; i<4; i++)
	      if (val==i)
		{
		  RBRefRun[i]->SetState(kButtonDown);
		  NEMichelinReference[i]->SetNumber(runno);
		}
	      else
		RBRefRun[i]->SetState(kButtonUp);
	  fclose(rrf);
	};
    }
  else // michelin not headless ==> we're saying which refrun to use!
    if (!strcmp(getenv("HOME"), "/home/monitor-olymp"))
      {
	FILE *rrf=fopen(".refrunselect", "w");
	for (int i=0; i<4; i++)
	  {
	    if (RBRefRun[i]->GetState()==kButtonDown)
	    fprintf(rrf, "%d %d\n", i, int(NEMichelinReference[i]->GetNumber()));
	  };
	fclose(rrf);
      };

  // remember what run was selected by the user:
  int selectedrun=-1;
  if (!daemonmode)
    {
      selectedrun=LBRunList->GetSelected();
      // clear the list of runs:
      LBRunList->RemoveAll();
    };
  std::vector<int> runs;
  int runno, length, lastrun=0;
  char fn[256];
  // we're interested in raw data ROOT files:
  const char runno_tag[]="ROOTfiles/run_";
  FILE *pipe=NULL;
  if (testmode)
    pipe=gSystem->OpenPipe(Form("ls -1r ROOTfiles/run_*_*.root |head -n %d", 
				(int)(NERunListLength->GetNumber())), "r");
  else
    pipe=gSystem->OpenPipe(Form("ls -1r %s/ROOTfiles/run_*_*.root |head -n %d",
				PATH_TO_ROOTFILES, (int)(NERunListLength->GetNumber())), "r");

  // read from pipe (output of 'ls -lt'):
  runs.clear();
  nearonlinependinglist.clear();
  while (!feof(pipe))
    {
      length=fscanf(pipe, "%s", fn);
      if (feof(pipe)) break;
      char *runno_str = strstr(fn, runno_tag);
      if (runno_str)
	{
	  sscanf(runno_str+strlen(runno_tag), "%d", &runno);
	  if ((LBRunList->GetEntry(runno)==NULL)||(!daemonmode))
	    {
	      LBRunList->AddEntry(runno_str+strlen("ROOTfiles/"), runno);
	      if (!FileExists(Form("%s/%d/done", PATH_TO_NEARONLINE, runno)))
		{
		  LBRunList->GetEntry(runno)->SetBackgroundColor(yellow);
		  // check if this was a normal data taking run:
		  if (strstr(runno_str+strlen("ROOTfiles/"), "_trigger_t"))
		    nearonlinependinglist.push_back(runno);
		}
	      else
		LBRunList->GetEntry(runno)->SetBackgroundColor(white);

	    };
	  runs.push_back(runno);
	  if (lastrun==0)
	    lastrun=runno;
	};
    };
  gSystem->ClosePipe(pipe);
  LBRunList->Layout();
  // select the run that had been selected (not to confuse the user):
  if (!daemonmode)
    LBRunList->Select(selectedrun);

  int process=1;
  // in daemonmode we need to find out was has happened (list of new runs) since last update:
  //if (daemonmode)
    {
      if (lastruns.size()==0)
	{
	  lastruns.push_back(0);
	  process=0; // don't process everything if we have just started from scratch...
	};
      // std::equal and std::set_difference work only on sorted vectors:
      std::sort(runs.begin(), runs.end());
      std::sort(lastruns.begin(), lastruns.end());
      // check if list of runs has changed at all:
      if ( !(std::equal(runs.begin(), runs.end(), lastruns.begin())) )
	{
	  // printf(" runlists are different:\n");
	  // printf("    runlist:  ");
	  // for (size_t i=0; i<runs.size(); i++)
	  // 	printf("[%d]", runs[i]);
	  // printf("\n");
	  // printf("    lastruns: ");
	  // for (size_t i=0; i<lastruns.size(); i++)
	  // 	printf("[%d]", lastruns[i]);
	  // printf("\n");

	  vector<int> newruns;
	  std::set_difference(runs.begin(), runs.end(),
			      lastruns.begin(), lastruns.end(),
			      std::back_inserter(newruns));
	  if (process)
	    {
	      if (daemonmode)
		{
		  // in daemonmode do nearonline cooking (or whatever)
		  // for all new runs in list:
		  DaemonAction(newruns);
		}
	      else
		{
		  // in normalmode push new runs to list of runs to watch:
		  printf(" adding %d runs to watch...\n", (int)(newruns.size()));
		  for (unsigned int i=0; i<newruns.size(); i++)
		    runstowatch.push_back(newruns[i]);
		  // process list of runs to watch:
		  //NormalmodeAction();
		};
	    };
	  lastruns = runs;
	};
    };

    if ((process)&&(!daemonmode))
      {
	NormalmodeAction();
      };

  return 0;
}

void AYCEgui::ForceCooking()
{
  TList *selruns=new TList();
  LBRunList->GetSelectedEntries(selruns);
  int nsel=selruns->GetSize();
  std::vector <int> runstocook;
  int runno = NERunnumber->GetNumber();
  if (nsel==0)
    {
      runstocook.push_back(runno);
      printf(" %d\n", runno);
    }
  else
    {
      TGLBEntry *thisentry = (TGLBEntry*)(selruns->First());
      while (thisentry)
	{
	  runstocook.push_back(thisentry->EntryId());
	  printf(" %d\n", thisentry->EntryId());
	  thisentry = (TGLBEntry*)(selruns->After(thisentry));
	};
    };
  delete selruns;
  DaemonAction(runstocook);
}

void AYCEgui::DaemonAction(std::vector<int>runlist)
{
  printf(" **** All You Can Eat - Daemon Mode - processing");
  string cmd="src/allyoucaneat/macros/daemonmode.sh";
  for (size_t i=0; i<runlist.size(); i++)
    if (runlist[i]>0)
      {
	cmd += Form(" %d.nearonline", runlist[i]);
	printf(" %d", runlist[i]);
      };
  printf("\n");
  //if (!testmode)
  gSystem->Exec(cmd.c_str());
  //else
  //printf("  -----> >%s<\n", cmd.c_str());
}

void AYCEgui::NormalmodeAction()
{
  int refrunnumber=NEMichelinReference[GetSelectedRefRun()]->GetNumber();
  if (runstowatch.size()>0)
    printf(" **** All You Can Eat - Nearonline Normal Mode - processing: (using #%d as refrun)\n",
	   refrunnumber);
  else
    return;
  for (size_t i=0; i<runstowatch.size(); i++)
    {
      if (i>5) break;
      if (runstowatch[i]>0)
	{
	  // check if file "done" has been created, meaning that nearonline
	  // cooking has finished:
	  if (FileExists(Form("%s/%d/done", PATH_TO_NEARONLINE, runstowatch[i])))
	    {
	      printf(" =====> run #%d nearonline DONE!\n", runstowatch[i]);
	      string cmd = "bin/michelin";
	      cmd += Form(" %s/%d", PATH_TO_NEARONLINE, runstowatch[i]);
	      cmd += Form(" %s/%d", PATH_TO_NEARONLINE, refrunnumber);
	      if (CBMichelinHeadless->GetState()==kButtonDown)
		cmd += " --headless";
	      gSystem->Exec(cmd.c_str());
	      runstowatch.erase(runstowatch.begin()+i);
	    }
	  else
	    printf(" ------> run #%d nearonline pending...\n", runstowatch[i]);
	};
    };
  printf("\n");
}

int AYCEgui::FileExists(const char *filename)
{
  ifstream thisfile(filename);
  if (thisfile)
    {
      thisfile.close();
      return 1;
    };
  return 0;
}


ClassImp(AYCEgui);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  daemonmode=0;
  testmode=0;

  for (int i=1; i<argc; i++)
    {
      if (!strcmp(argv[i], "-d"))
	daemonmode = 1;
      if (!strcmp(argv[i], "--testmode"))
	testmode = 1;
    };

  TApplication theApp("All You Can Eat", &argc, argv);
  new AYCEgui(gClient->GetRoot(), 800, 600);

  theApp.Run();

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
