#include "comparewindow.h"
#include <fstream>
#include <iostream>
#include "TFile.h"
#include "TStyle.h"
#include "TText.h"
#include "TPaveStats.h"
#include <cstring>
#include "stdlib.h"
#include "unistd.h"

void CompareWindow::Watchdog()
{
  watchdogcounter++;
  // Notify shift crew that new histos to be looked at are waiting:
  int retval;
  if (watchdogcounter==0)
    {
      retval=system("ssh -i $HOME/.ssh/id_rsa_sound olympus@omac afplay -v 0.05 /Users/olympus/sound/Floete.wav&");
      if (retval<0) std::cerr << " Cannot connect to soundserver! " << std::endl;
      //retval=system("ssh -i $HOME/.ssh/id_rsa_sound olympus@omac afplay -v 0.05 /Users/olympus/sound/drmthm1.wav&");
      //if (retval<0) std::cerr << " Cannot connect to soundserver! " << std::endl;
      //usleep(3000000);
      //retval=system("ssh -i $HOME/.ssh/id_rsa_sound olympus@omac afplay -v 0.12 /Users/olympus/sound/putdieauge.aiff&");
      //if (retval<0) std::cerr << " Cannot connect to soundserver! " << std::endl;
      return;
    };
  // Show hwo long nobody gave a damn on our data quality ;-)
  if ((watchdogcounter%30)==0)
    printf(" waiting for user be interested in histograms since %d seconds...\n", watchdogcounter);
  // Start to get a little bit more aggressive:
  if (watchdogcounter==120)
    {
      printf(" Hey, shiftcrew!\n");
      retval=system("ssh -i $HOME/.ssh/id_rsa_sound olympus@omac afplay -v 0.06 /Users/olympus/sound/dogbark4.wav");
      if (retval<0) std::cerr << " Cannot connect to soundserver! " << std::endl;
      retval=system("ssh -i $HOME/.ssh/id_rsa_sound olympus@omac afplay -v 0.07 /Users/olympus/sound/HUPE_schaaf.wav");
      if (retval<0) std::cerr << " Cannot connect to soundserver! " << std::endl;
      usleep(2000000);
      retval=system("ssh -i $HOME/.ssh/id_rsa_sound olympus@omac afplay -v 0.10 /Users/olympus/sound/wakeup.aiff");
      if (retval<0) std::cerr << " Cannot connect to soundserver! " << std::endl;
    };
  // Play something more annyoing when there are really lazy guys on shift:
  if ((watchdogcounter%300)==0)
    {
      printf(" Wake up!\n");
      retval=system("ssh -i $HOME/.ssh/id_rsa_sound olympus@omac afplay -v 0.05 /Users/olympus/sound/HUPE_nerv.wav");
      if (retval<0) std::cerr << " Cannot connect to soundserver! " << std::endl;
      usleep(2000000);
      retval=system("ssh -i $HOME/.ssh/id_rsa_sound olympus@omac afplay -v 0.15 /Users/olympus/sound/complain.aiff");
      if (retval<0) std::cerr << " Cannot connect to soundserver! " << std::endl;
    };
}

void CompareWindow::WatchdogReset()
{
  watchdogcounter = 0;
}

void CompareWindow::SelectHisto(UInt_t i)
{
  if (currentHisto==i) return;
  currentHisto = i;
  skip(0);
}

void CompareWindow::skip(int i)
{
  WatchdogReset();
  //printf("Histo: %i %i \n",currentHisto,i);
  currentHisto+=i;
  if (leftbtn)  leftbtn->SetEnabled(currentHisto>0);  
  if (rightbtn) rightbtn->SetEnabled(currentHisto+1<histosleft.size());
  if (LBhistos) LBhistos->Select(currentHisto);
  presentHisto();
}

void CompareWindow::TogglePalette()
{
  WatchdogReset();
  if (palette==1)
      palette = 8;
  else
      palette = 1;
  gStyle->SetPalette(palette);
}

void CompareWindow::ZoomPad(int pad)
{
  WatchdogReset();
  if (pad==1)
    {
      double xmin, ymin, xmax, ymax;
      sc->GetPad(1)->GetRangeAxis(xmin, ymin, xmax, ymax);
      if (histosright[currentHisto]) histosright[currentHisto]->GetXaxis()->SetRangeUser(xmin, xmax);
      // Now notify histo in right pad that it needs to be redrawed:
      sc->GetPad(2)->Modified();
      // ...doing this in any other way will either not work at all
      // or destroy the left pad...
    };
}

void CompareWindow::HistoOperation(int operation, TH1* h1, TH1* h2)
{
  switch (operation)
    {
    case add:
      h1->Add(h2); break;
    case subtract:
      h1->Add(h2, -1.0); break;
    case multiply:
      h1->Multiply(h2); break;
    case divide:
      h1->Divide(h2); break;
    };
}

void CompareWindow::SetStyle(TH1 *histo, std::string style, int samepad)
{
  if (histo==NULL) return;
  //printf("SetStyle! >>%s<<\n", style.c_str());
  char *stylecpy = new char[strlen(style.c_str())+1];
  char *here=stylecpy;
  strcpy(here, (char*)style.c_str());
  char *token = NULL;
  while (token=strtok(here, ":"))
    {
      here=NULL;
      //---------------------------------------//
      if (!strcmp(token, "logx"))
	gPad->SetLogx();
      //---------------------------------------//
      if (!strcmp(token, "logy"))
	gPad->SetLogy();
      //---------------------------------------//
      if (!strcmp(token, "logz"))
	gPad->SetLogz();
      //---------------------------------------//
      if (strstr(token, "axisrange"))
	{
	  char whichaxis = token[0];
	  double from, to;
	  token=strtok(NULL, ":");
	  sscanf(token, "%lf", &from);
	  //printf(" from=%lf\n", from);
	  token=strtok(NULL, ":");
	  sscanf(token, "%lf", &to);
	  //printf(" from=%lf\n", to);
	  TAxis *axis=NULL;
	  if (whichaxis=='x') axis = histo->GetXaxis();
	  if (whichaxis=='y') axis = histo->GetYaxis();
	  if (whichaxis=='z') axis = histo->GetZaxis();
	  if (axis==NULL)
	    {
	      printf(" WARNING - unknown axis >%c<\n", whichaxis);
	      continue;
	    };
	  axis->SetRangeUser(from, to);
	  //printf("%caxis %.1lf --> %.1lf\n", whichaxis, from, to);
	};
      //---------------------------------------//
      if (!strcmp(token, "linecolor"))
	{
	  token=strtok(NULL, ":");
	  int col; sscanf(token, "%d", &col);
	  histo->SetLineColor(col);
	};
      //---------------------------------------//
      if (!strcmp(token, "linewidth"))
	{
	  token=strtok(NULL, ":");
	  int wid; sscanf(token, "%d", &wid);
	  histo->SetLineWidth(wid);
	};
      //---------------------------------------//
      if (!strcmp(token, "markercolor"))
	{
	  token=strtok(NULL, ":");
	  int col; sscanf(token, "%d", &col);
	  histo->SetMarkerColor(col);
	};
      //---------------------------------------//
      if (!strcmp(token, "markerstyle"))
	{
	  token=strtok(NULL, ":");
	  int sty; sscanf(token, "%d", &sty);
	  histo->SetMarkerStyle(sty);
	};
      //---------------------------------------//
      if (strstr(token, "vline"))
	{
	  int linestyle=1;
	  if (token[strlen("vline")]!=':')
	    sscanf(token+strlen("vline"), "%d", &linestyle);
	  token=strtok(NULL, ":");
	  int x; sscanf(token, "%d", &x);
	  TAxis *ya = histo->GetYaxis();
	  TLine *l = new TLine();
	  l->SetLineStyle(linestyle);
	  l->DrawLine(x, ya->GetBinLowEdge(1), x, histo->GetMaximum()*1.05);
	};
      //---------------------------------------//
      if (!strcmp(token, "hline"))
	{
	  int linestyle=1;
	  if (token[strlen("hline")]!=':')
	    sscanf(token+strlen("hline"), "%d", &linestyle);
	  token=strtok(NULL, ":");
	  int y; sscanf(token, "%d", &y);
	  TLine *l = new TLine();
	  l->SetLineStyle(linestyle);
	  l->DrawLine(histo->GetXaxis()->GetBinLowEdge(1), y,
		      histo->GetXaxis()->GetBinUpEdge(histo->GetXaxis()->GetNbins()), y);
	};
      //---------------------------------------//
      if (!strcmp(token, "nostats"))
	{
	  histo->SetStats(0);
	};
      //---------------------------------------//
      if (!strcmp(token, "leftstats"))
	{
	  gPad->Update();
	  TPaveStats *st = (TPaveStats*)histo->FindObject("stats");
	  if (st)
	    {
	      st->SetX1NDC(0.05);
	      st->SetX2NDC(0.25);
	      st->Draw();
	    };
	};
      //---------------------------------------//
      if (!strcmp(token, "movestats"))
	{
	  double val;
	  gPad->Update();
	  TPaveStats *st = (TPaveStats*)histo->FindObject("stats");
	  if (st)
	    {
	      token=strtok(NULL, ":");
	      sscanf(token, "%lf", &val);
	      st->SetX1NDC(val);
	      token=strtok(NULL, ":");
	      sscanf(token, "%lf", &val);
	      st->SetY1NDC(val);
	      token=strtok(NULL, ":");
	      sscanf(token, "%lf", &val);
	      st->SetX2NDC(val);
	      token=strtok(NULL, ":");
	      sscanf(token, "%lf", &val);
	      st->SetY2NDC(val);
	      st->Draw();
	    };
	};
      //---------------------------------------//
      if (!strcmp(token, "frequency"))
	{
	  histo->Scale(1./histo->Integral());
	};
      //---------------------------------------//
      if (!strcmp(token, "rebin2d"))
	{
	  int val1, val2;
	  token=strtok(NULL, ":");
	  sscanf(token, "%d", &val1);
	  token=strtok(NULL, ":");
	  sscanf(token, "%d", &val2);
	  ((TH2*)(histo))->Rebin2D(val1, val2);
	};
      //---------------------------------------//
      if (!strcmp(token, "padwidth"))
	{
	  double val1, val2;
	  token=strtok(NULL, ":");
	  sscanf(token, "%lf", &val1);
	  token=strtok(NULL, ":");
	  sscanf(token, "%lf", &val2);
	  gPad->SetPad(val1, 0.04, val2, 0.99);
	};
      //---------------------------------------//
    };
  delete[] stylecpy;
  return;
}

void CompareWindow::presentHisto()
{
  sc->Clear();
  int samepad=0; // draw reference histo into same pad? By default: no!
  if (strstr(styleopts[currentHisto].c_str(), "samepad"))
    samepad=1;
  TText *t1 = new TText(0.12, 0.005, Form("        this Run (%d)         ", thisrunno));
  TText *t2 = new TText(0.57, 0.005, Form("     Reference Run (%d)     ", refrunno));
  if (samepad==0) /////////////////////////////////////////////////////////////////////
    {
      sc->Divide(2);
      sc->cd(0);
      t1->Draw();
      t2->Draw();
      sc->cd(1);
      gPad->SetPad(0.01, 0.04, 0.49, 0.99); // make a small gap below the pad
      if (histosleft[currentHisto])
	{
	  TText *tl = new TText(0.1, 0.5, "Drawing histogram - please wait...");
	  tl->Draw();
	  tl->SetTextColor(kBlue);
	  gPad->Update();
	  histosleft[currentHisto]->Draw(drawopts[currentHisto].c_str());
	  SetStyle(histosleft[currentHisto], styleopts[currentHisto]);
	}
      else
	{
	  TText *tl = new TText(0.05, 0.5, Form("Histogram \n>%s<\n not found!", histname[currentHisto].c_str()));
	  tl->Draw();
	  tl->SetTextColor(kRed);
	}
      gPad->Connect("RangeAxisChanged()", "CompareWindow", this, "ZoomPad(=1)");
      sc->cd(2);
      gPad->SetPad(0.51, 0.04, 0.99, 0.99); // make a small gap below the pad
      //gPad->Connect("RangeAxisChanged()", "CompareWindow", this, "ZoomPad(=2)");
      if (histosright[currentHisto])
	{
	  TText *tr = new TText(0.1, 0.5, "Drawing histogram - please wait...");
	  tr->Draw();
	  tr->SetTextColor(kBlue);
	  gPad->Update();
	  histosright[currentHisto]->Draw(drawopts[currentHisto].c_str());
	  SetStyle(histosright[currentHisto], styleopts[currentHisto]);
	}
      else
	{
	  TText *tr = new TText(0.05, 0.5, Form("Histogram \n>%s<\nnot found!", histname[currentHisto].c_str()));
	  tr->Draw();
	  tr->SetTextColor(kRed);
	};
    }
  else if (samepad!=0) ////////////////////////////////////////////////////////////////
    {
      sc->Divide(1);
      sc->cd(0);
      t1->Draw(); t1->SetTextColor(kBlue);
      t2->Draw(); t2->SetTextColor(kRed);
      sc->cd(1);
      gPad->SetPad(0.01, 0.04, 0.99, 0.99); // make a small gap below the pad
      if (histosright[currentHisto])
	{
	  TText *tr = new TText(0.1, 0.5, "Drawing histograms - please wait...");
	  tr->Draw();
	  tr->SetTextColor(kBlue);
	  gPad->Update();
	  histosright[currentHisto]->Draw(drawopts[currentHisto].c_str());
	  SetStyle(histosright[currentHisto], styleopts[currentHisto]);
	  histosright[currentHisto]->SetLineColor(kRed);
	};
      if (histosleft[currentHisto])
	{
	  histosleft[currentHisto]->Draw((drawopts[currentHisto]+"same").c_str());
	  SetStyle(histosleft[currentHisto], styleopts[currentHisto], samepad);
	  histosleft[currentHisto]->SetLineColor(kBlue);
	};
      // in the end make sure y axis scale makes sense - unless this is not wanted:
      if (!strstr(styleopts[currentHisto].c_str(), "norescaley"))
	{
	  if (histosright[currentHisto]&&histosleft[currentHisto])
	    {
	      double ymin = TMath::Min(histosright[currentHisto]->GetMinimum(), histosleft[currentHisto]->GetMinimum());
	      double ymax = TMath::Max(histosright[currentHisto]->GetMaximum(), histosleft[currentHisto]->GetMaximum());
	      histosright[currentHisto]->GetYaxis()->SetRangeUser(0., ymax*1.05); // right histo owns the y axis!
	    };
	};
    }; ////////////////////////////////////////////////////////////////////////////////

  sc->Update();
}
void CompareWindow::loadHistos()
{
  if (gApplication->Argc()>4)
    {
      printf("Need two arguments: run number directories to compare\n");
      exit(-1);
    }

  std::string leftpath(gApplication->Argv(1));
  std::string rightpath(gApplication->Argv(2));
  std::string configfile="histos.conf";
  if (gApplication->Argc()==3) (configfile=gApplication->Argv(3));
  printf("CONFIGFILE: >%s<\n", configfile.c_str());
  // get the runnumbers:
  thisrunno=-1;
  refrunno=-1;
  {
    const char *aux=strrchr((const char*)(leftpath.c_str()), '/');
    if (aux)
      sscanf(aux+1, "%d", &thisrunno);
    aux=strrchr((const char*)(rightpath.c_str()), '/');
    if (aux)
      sscanf(aux+1, "%d", &refrunno);
    printf(" RUNS: ===%d===  and  ===%d===\n", thisrunno, refrunno);
  };
  //first, we read the list of histogram names. Format: root-file histoname
  std::ifstream histoconf(configfile.c_str());
  if (!(histoconf.good()))
    {
      printf(" Cannot open %s - bailing out!\n", configfile.c_str());
      exit(1);
    };
  std::string tmp, rootfile, histoname, drawoptions, styleoptions;
  char hname[1024], opname[1024]={'\0'}, aux[1024];
  while (histoconf.good())
    {
      histoconf >> tmp;
      if (strchr(tmp.c_str(), '#')) // comment line?
	continue;
      // read line of histogram plot defintion:
      rootfile = tmp;
      histoconf >> histoname >> drawoptions >> styleoptions;
      std::cout << "Trying to read " << histoname << " from " << rootfile;
      int   hnlen = strlen(histoname.c_str());
      //char *hname  = new char[hnlen];
      //char *opname = new char[hnlen];
      TH1  *hoperator=NULL;
      strcpy(hname, (char*)(histoname.c_str()));
      //---------------------------------------//
      int operation=none;
      char *where=NULL;
      if (strstr(hname, "(+)")) { operation = add;      where = strstr(hname, "(+)"); };
      if (strstr(hname, "(-)")) { operation = subtract; where = strstr(hname, "(-)"); };
      if (strstr(hname, "(*)")) { operation = multiply; where = strstr(hname, "(*)"); };
      if (strstr(hname, "(/)")) { operation = divide;   where = strstr(hname, "(/)"); };
      if (operation!=none)
	{
	  strcpy(opname, where+3);
	  hname[where-hname] = '\0';
	};
      //---------------------------------------//

      int maxtry = 3;

      sprintf(aux, "%s/%s", leftpath.c_str(), rootfile.c_str());
      TFile *lf=NULL;
      TH1   *lh=NULL;
      lf = new TFile(aux, "READ");
      // if histo was not found, reopen file and try it once more:
      for (int tries=0; tries<maxtry; tries++)
	{
	  lh = (TH1*)lf->Get(hname);
	  if (lh!=NULL) break;
	  lf->Close();
	  printf("."); fflush(stdout);
	  usleep(250000);
	  lf->Open(aux, "READ");
	  lh = (TH1*)lf->Get(hname);
	};
      if (lh)
	{
	  printf("\t\033[32m\033[1mOK\033[0m\n");
	  if (LBhistos) LBhistos->AddEntry(hname, histname.size());
	}
      else
	printf("\t\033[31m\033[1mnot found\033[0m\n");
      if (lh)      lh->SetDirectory(0);
      if (lh)      lh->SetOption(drawoptions.c_str());
      if (strlen(opname)!=0)
	{
	  TH1 *oph = (TH1*)lf->Get(opname);
	  if (oph) HistoOperation(operation, lh, oph);
	};
      histosleft.push_back(lh);
      drawopts.push_back(drawoptions);
      styleopts.push_back(styleoptions);
      histname.push_back(histoname.c_str());
      lf->Close();

      sprintf(aux, "%s/%s", rightpath.c_str(), rootfile.c_str());
      TFile *rf=NULL;
      TH1   *rh=NULL;
      rf = new TFile(aux, "READ");
      // if histo was not found, reopen file and try it once more:
      for (int tries=0; tries<maxtry; tries++)
	{
	  rh = (TH1*)rf->Get(hname);
	  if (rh!=NULL) break;
	  rf->Close();
	  printf("."); fflush(stdout);
	  usleep(250000);
	  lf->Open(aux, "READ");
	  rh = (TH1*)rf->Get(hname);
	};
      if (rh)     rh->SetDirectory(0);
      if (rh)     rh->SetOption(drawoptions.c_str());
      if (opname)
	{
	  TH1 *oph = (TH1*)rf->Get(opname);
	  if (oph) HistoOperation(operation, rh, oph);
	};
      histosright.push_back(rh);
      rf->Close();
    }
  
  histoconf.close();

}


CompareWindow::CompareWindow(const TGWindow *p, UInt_t w, UInt_t h,bool gui){

  gStyle->SetOptStat(1001111);
  gStyle->SetOptFit(1);

  currentHisto=0;
  TGLayoutHints *btnhint=new TGLayoutHints(kLHintsExpandX,2,2,2,2);

  headless=false;
  if (gui==false)
    headless = true;

  if (gui)
    {
      TGVerticalFrame   *vframe = new TGVerticalFrame(this, w, h);

      TGPictureButton *michelinbtn = new TGPictureButton(vframe, "src/allyoucaneat/logo_michelin.gif");
      vframe->AddFrame(michelinbtn, btnhint);

      TGHorizontalFrame *hframe=new TGHorizontalFrame(vframe,w,h);

      vframe->AddFrame(hframe,btnhint);

      leftbtn=new TGTextButton(hframe,"  &< previous  ");
      rightbtn=new TGTextButton(hframe,"  next &>  ");
      hframe->AddFrame(leftbtn,btnhint);
      hframe->AddFrame(rightbtn,btnhint);
      leftbtn->Connect("Clicked()","CompareWindow",this,"skip(=-1)");
      rightbtn->Connect("Clicked()","CompareWindow",this,"skip(=1)");

      palettebtn = new TGTextButton(hframe, "  Toggle Palette  ");
      hframe->AddFrame(palettebtn, btnhint);
      palettebtn->Connect("Clicked()", "CompareWindow", this, "TogglePalette()");

      LBhistos = new TGListBox(vframe);
      vframe->AddFrame(LBhistos, btnhint);
      LBhistos->Resize(LBhistos->GetDefaultWidth(), 200);
      LBhistos->Connect("DoubleClicked(Int_t)", "CompareWindow", this, "SelectHisto(UInt_t)");
      //LBhistos->Connect("ProcessedEvent(Event_t*)", "CompareWindow", this, "selecthisto(Event_t*)");

      AddFrame(vframe,btnhint);
    };

  // In any case (even when running headless) we need a canvas (to draw histograms):
  if (headless)
    sc=new TSuperCanvas("","",1000,550,0); // empty name and title: window will not open (headless mode!)
  else
    sc=new TSuperCanvas("CompareSC","Compare with reference plots",1000,550,0);
  sc->ToggleEventStatus();
  sc->SetCrosshair(1);
  sc->SetQuitOnClose();
  
  loadHistos();
  if (gui)
    {
      skip(0);
      MapSubwindows();
      Layout();
      SetWindowName("Michelin");
      SetIconName("Michelin");
      Resize(GetDefaultSize());
      MapWindow();

      // Initialize watchdog that will bark at the shift crew
      // if they don't look at histograms ;-)
      watchdogcounter = -1;
      timer = new TTimer(1000);
      timer->Connect("Timeout()", "CompareWindow", this, "Watchdog()");
      if (!strcmp(getenv("HOME"), "/home/monitor-olymp"))
	timer->Start();
      else
	printf(" Not running as user monitor-olymp - not starting watchdog timer!\n");

      palette = 1;
    }
  else // gui==false, i.e. headless mode:
    {
      for (unsigned int i=0; i<histosleft.size(); i++)
	{
	  int retval=0;
	  printf("printing histo %d to file...\n", i);
	  currentHisto = i;
	  skip(0);
	  //if (strstr(styleopts[i].c_str(), "noheadless")) continue;
	  //sc->SaveAs(Form("lastnearonline_p%d.png", i));
	  sc->Update();
	  usleep(25000);
	  if (strstr(styleopts[i].c_str(), "isslow"))
	    usleep(2500000);
	  sc->SaveAs(Form("lastnearonline_p%d.eps", i));
	  usleep(25000);
	  //retval=system(Form("pstopnm -ppm -xborder 0 -yborder 0 -portrait lastnearonline_p%d.eps && sleep 1 && pnmtopng lastnearonline_p%d*.ppm > lastnearonline_p%d.png", i, i, i));
	  retval = system(Form("convert -gamma 0.75 lastnearonline_p%d.eps lastnearonline_p%d.png", i, i));
	  // if (strstr(styleopts[i].c_str(), "isslow"))
	  //   usleep(2500000);
	  // system(Form("pnmtopng lastnearonline_p%d*.ppm > lastnearonline_p%d.png", i, i));
	};
      //system("sleep 1 ; mv lastnearonline*.{png,ppm,eps} plots/");
      printf(" ...done! Exiting now!\n");
      gApplication->Terminate();
    };
}


CompareWindow::~CompareWindow()
{
  gApplication->Terminate();
}
