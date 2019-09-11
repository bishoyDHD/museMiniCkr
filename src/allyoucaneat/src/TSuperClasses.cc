//
// Implementation of ROOT "super" GUI classes.
//
// They're meant to supplement some "missing" methods, which are very useful in everyday live,
// but anyhow for some reasons not implemented in original ROOT.
//
// J. Diefenbach, A4 collaboration, Mainz, 2009
//

#include "TSuperClasses.h"

#include <math.h>

//##################################################################################################################//
//##################################################################################################################//
TGSuperListBox::TGSuperListBox(const TGWindow* p, Int_t id, UInt_t options, Pixel_t back) : TGListBox(p, id, options, back)
{
}
TGSuperListBox::~TGSuperListBox()
{
}
//##################################################################################################################//
//##################################################################################################################//
TGRangeSelector::TGRangeSelector(TGCompositeFrame *where, TGCompositeFrame *parent, const char* title, int width, int digitwidth,
                                 double sliderangefrom, double sliderangeto,
                                 int entryrangefrom, int entryrangeto,
                                 long cmdSlider, long cmdEntry, int TLx0, int TLy0)
  : mywindow(where), myparent(parent), mytitle(title), pixelwidth(width),
    sliderfrom(sliderangefrom), sliderto(sliderangeto), entryfrom(entryrangefrom), entryto(entryrangeto),
    parmSlider(cmdSlider), parmEntry(cmdEntry), cx(TLx0), cy(TLy0)
{
  sliderrange = sliderto-sliderfrom;
  entryrange  = entryto-entryfrom;

  mywindow->AddFrame(new TGLabel(mywindow, mytitle), new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;

  entry[0] = new TGNumberEntry(mywindow, entryfrom, digitwidth, parmEntry);
  mywindow->AddFrame(entry[0], new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
  entry[0]->SetLimits(TGNumberFormat::kNELLimitMinMax, entryfrom, entryto);
  entry[0]->Associate(this);

  slider = new TGDoubleHSlider(mywindow, pixelwidth, 1, parmSlider);
  slider->SetRange(sliderfrom, sliderto);
  slider->SetPosition(sliderfrom, sliderto);
  mywindow->AddFrame(slider, new TGTableLayoutHints(cx, cx+4, cy, cy+2)); cx+=4;
  slider->Associate(this);

  entry[1] = new TGNumberEntry(mywindow, entryto, digitwidth, parmEntry);
  mywindow->AddFrame(entry[1], new TGTableLayoutHints(cx, cx+1, cy, cy+1)); cx++;
  entry[1]->SetLimits(TGNumberFormat::kNELLimitMinMax, entryfrom, entryto);
  entry[1]->Associate(this);
}
TGRangeSelector::~TGRangeSelector()
{
}
Bool_t TGRangeSelector::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
  if (msg) { }; // get rid of compiler warning "unused parameter"...
  if (parm1==parmEntry)
    {
      double sliderlow   = (entry[0]->GetNumber() - entryfrom)/entryrange*sliderrange+sliderfrom;
      double sliderhigh  = (entry[1]->GetNumber() - entryfrom)/entryrange*sliderrange+sliderfrom;
      slider->SetPosition(sliderlow, sliderhigh);
      slider->Layout();
      myparent->ProcessMessage(kC_HSLIDER|kSL_POS, parmSlider, 0);
    };
  if (parm1==parmSlider)
    {
      float low, high;
      slider->GetPosition(&low, &high);
      double entrylow   = (low - sliderfrom)/sliderrange*entryrange+entryfrom;
      double entryhigh  = (high - sliderfrom)/sliderrange*entryrange+entryfrom;
      entry[0]->SetNumber(entrylow);
      entry[1]->SetNumber(entryhigh);
      entry[0]->Layout();
      entry[1]->Layout();
      myparent->ProcessMessage(kC_HSLIDER|kSL_POS, parmSlider, 0);
    };
  return kTRUE;
}
//##################################################################################################################//
//##################################################################################################################//
TSuperCanvas::TSuperCanvas(const char* name, const char* title, Int_t ww, Int_t wh, Float_t menumargin)
  : TCanvas(name, title, ww, wh), menu_margin(menumargin) { Initialize(); }
TSuperCanvas::TSuperCanvas(const char* name, const char* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh, Float_t menumargin)
  : TCanvas(name, title, wtopx, wtopy, ww, wh), menu_margin(menumargin) { Initialize(); }
TSuperCanvas::~TSuperCanvas()
{
  if (closebutton) delete closebutton;
  if (crossbutton) delete crossbutton;
  if (QuitOnClose) exit(0);
}
void TSuperCanvas::Initialize()
{
  QuitOnClose = false;

  closebutton = NULL;
  crossbutton = NULL;

  SamplingRate = 1.0;
  ScrollRate   = 0.03;

  meas_status = 0;
  meas_x1 = 0;
  meas_x2 = 0;
  meas_y1 = 0;
  meas_y2 = 0;

  for (int p=0; p<10; p++)
    {
      peakpos[p]  = 0.0;
      peakline[p] = NULL;
    };

  MaximizedPad = NULL;
}
void TSuperCanvas::SetQuitOnClose(Bool_t quit)
{
  QuitOnClose = quit;
}
void TSuperCanvas::ClearPointers()
{
  MaximizedPad = NULL;
}
void TSuperCanvas::Divide(Int_t nx, Int_t ny, Float_t xmargin, Float_t ymargin, Int_t color, Float_t menumargin)
{// this method was derived from TCanvas::Divide
  if (!IsEditable()) return;

  if (gThreadXAR) {
    void *arr[7];
    arr[1] = this; arr[2] = (void*)&nx;arr[3] = (void*)& ny;
    arr[4] = (void*)&xmargin; arr[5] = (void *)& ymargin; arr[6] = (void *)&color;
    if ((*gThreadXAR)("PDCD", 7, arr, 0)) return;
  }

  TPad *padsav = (TPad*)gPad;
  cd();
  if (nx <= 0) nx = 1;
  if (ny <= 0) ny = 1;
  Int_t ix,iy;
  Double_t x1,y1,x2,y2;
  Double_t dx,dy;
  TPad *pad;
  char *name  = new char [strlen(GetName())+6];
  char *title = new char [strlen(GetTitle())+6];
  Int_t n = 0;
  if (color == 0) color = GetFillColor();
  // special bottom margin for menu:
  Float_t margin = menu_margin;
  if (menumargin>=0.0)
    margin = menumargin;
  if (xmargin > 0 && ymargin > 0)
    {
      //general case
      dy = 1/Double_t(ny);
      dx = 1/Double_t(nx);
      for (iy=0;iy<ny;iy++)
        {
          y2 = 1 - iy*dy - ymargin;
          y1 = y2 - dy + 2*ymargin;
          if (y1 < 0) y1 = 0;
          if (y1 > y2) continue;
          for (ix=0;ix<nx;ix++)
            {
              x1 = ix*dx + xmargin;
              x2 = x1 +dx -2*xmargin;
              if (x1 > x2) continue;
              n++;
              sprintf(name,"%s_%d",GetName(),n);
              // rescale and shift y1, y2 according to menumargin:
              Float_t y1m = y1*(1.0-margin)+margin;
              Float_t y2m = y2*(1.0-margin)+margin;
              pad = new TPad(name,name,x1,y1m,x2,y2m,color);
              pad->SetNumber(n);
              pad->Draw();
            }
        }
    } else {
    // special case when xmargin <= 0 && ymargin <= 0
    Double_t xl = GetLeftMargin();
    Double_t xr = GetRightMargin();
    Double_t yb = GetBottomMargin();
    Double_t yt = GetTopMargin();
    xl /= (1-xl+xr)*nx;
    xr /= (1-xl+xr)*nx;
    yb /= (1-xl+xr)*ny;
    yt /= (1-xl+xr)*ny;
    SetLeftMargin(xl);
    SetRightMargin(xr);
    SetBottomMargin(yb);
    SetTopMargin(yt);
    dx = (1-xl-xr)/nx;
    dy = (1-yb-yt)/ny;
    Int_t number = 0;
    for (Int_t i=0;i<nx;i++)
      {
        x1 = i*dx+xl;
        x2 = x1 + dx;
        if (i == 0) x1 = 0;
        if (i == nx-1) x2 = 1-xr;
        for (Int_t j=0;j<ny;j++)
          {
            number = j*nx + i +1;
            y2 = 1 -j*dy -yt;
            y1 = y2 - dy;
            if (j == 0)    y2 = 1-yt;
            if (j == ny-1) y1 = 0;
            sprintf(name,"%s_%d",GetName(),number);
            sprintf(title,"%s_%d",GetTitle(),number);
            pad = new TPad(name,title,x1,y1,x2,y2);
            pad->SetNumber(number);
            pad->SetBorderMode(0);
            if (i == 0)    pad->SetLeftMargin(xl*nx);
            else           pad->SetLeftMargin(0);
            pad->SetRightMargin(0);
            pad->SetTopMargin(0);
            if (j == ny-1) pad->SetBottomMargin(yb*ny);
            else           pad->SetBottomMargin(0);
            pad->Draw();
          }
      }
  }
  delete [] name;
  delete [] title;
  Modified();
  if (padsav) padsav->cd();
}
void TSuperCanvas::MakeMenu(TSuperCanvas **me, Float_t menumargin)
{
  float x=0, dx=0.2;
  Float_t margin = menu_margin;
  if (menumargin>0.0)
    margin = menumargin;
  if (me)
    closebutton = new TButton("close", Form("((TCanvas*)%p)->Close(); (*((TCanvas**)%p))=NULL;", *me, me), x, 0.0, x+dx/1.5, margin);
  else
    closebutton = new TButton("close", Form("((TCanvas*)%p)->Close();", this), x, 0.0, x+dx/1.5, margin);
  closebutton->Draw(); x+=dx/1.5;
  crossbutton = new TButton("crosshair", Form("((TCanvas*)%p)->SetCrosshair(!(((TCanvas*)%p)->HasCrosshair()));", this, this), x, 0.0, x+dx, margin);
  crossbutton->Draw(); x+=dx;
  eventbutton = new TButton("event status", Form("((TCanvas*)%p)->ToggleEventStatus();", this), x, 0.0, x+dx, margin);
  eventbutton->Draw(); x+=dx;
  toolbutton  = new TButton("toolbar", Form("((TCanvas*)%p)->ToggleToolBar();", this), x, 0.0, x+dx, margin);
  toolbutton->Draw(); x+=dx;

  logxbutton  = new TButton("lg x", "int val=gPad->GetLogx(); gPad->SetLogx(!val);", x, 0.0, x+dx/2.25, margin);
  logxbutton->Draw(); x+=dx/2.25;
  logybutton  = new TButton("lg y", "int val=gPad->GetLogy(); gPad->SetLogy(!val);", x, 0.0, x+dx/2.25, margin);
  logybutton->Draw(); x+=dx/2.25;

}
void TSuperCanvas::HandleInput(EEventType event, Int_t px, Int_t py)
{
  //printf("event=%d @( %d ; %d )\n", event, px, py);
  //-----------------------------------------------------------------//
  if (event==7) // i.e. mouse button 1 down + shift pressed
    {
      Measure(event, px, py);
      return;
    };
  //-----------------------------------------------------------------//
  if (event==kKeyPress)
    if ((py==4114)||(py==4116))
      {
        TObject *selected = gPad->GetSelected();
        TAxis *axis=NULL;

        if (strstr(selected->IsA()->GetName(), "TH1"))
          axis = ((TH1*)selected)->GetXaxis();
        //if (strstr(selected->IsA()->GetName(), "TH2"))
        //axis = ((TH2*)selected)->GetXaxis();
        if (strstr(selected->IsA()->GetName(), "TGraph"))
          axis = ((TGraph*)selected)->GetXaxis();

        if (axis==NULL)
          return;

        int    first = axis->GetFirst();
        int    last  = axis->GetLast();
        int    dist  = int(double(last-first)*ScrollRate);
        if (py==4114)
          dist *= -1;
        double low   = axis->GetBinLowEdge(first+dist);
        double up    = axis->GetBinUpEdge(last+dist);
        axis->SetRangeUser(low, up);
        if (strstr(selected->IsA()->GetName(), "TH1"))
          ((TH1*)selected)->Draw();
        //if (strstr(selected->IsA()->GetName(), "TH2"))
        //  ((TH2*)selected)->Draw();
        if (strstr(selected->IsA()->GetName(), "TGraph"))
          ((TGraph*)selected)->Draw();

        Update();
      };
  //-----------------------------------------------------------------//
  if (event==kKeyPress)
    {
      // 'px' contains the ASCII-code of the pressed key:
      if (px=='f') // show FFT of selected histogram
        {
          TObject *graphics = gPad->GetSelected();
          if (strstr(graphics->IsA()->GetName(), "TH1"))
            {
              TH1* ffth=NULL;
              ffth = ((TH1*)graphics)->FFT(ffth, (Option_t*)"MAG");
              if (ffth)
                {
                  ffth->SetTitle(Form("%s sepctrum", ((TH1*)graphics)->GetTitle()));
                  ffth->Draw();
                }
              else
                printf("%s TSuperCanvas: Sorry, not able to create FFT, check ROOT installation for FFT support...%s\n", WARNING_MSG, DEFAULT);
              TAxis *xa = ffth->GetXaxis();
              xa->SetRangeUser(xa->GetBinLowEdge(1), xa->GetBinUpEdge(xa->GetNbins())/2.0);
              gPad->SetLogy();
              Update();
              printf("FFT for >%s< generated...\n", graphics->GetName());
            }
          else
            printf("%s TSuperCanvas: Sorry, no FFT possible for non-TH1 graphics objects...%s\n", WARNING_MSG, NORMAL);
          return;
        };
      //-----------------------------------------------------------------//
      if ( (px>='0') && (px<='9') )      //     SAVE PEAK POSIITONS      //
        {
          if (meas_status==1) // user has selected coordinated by shift-click on pad?
            {
              int p = px-'0';
              peakpos[p] = meas_x1;
              peakline[p] = new TLine(peakpos[p], 0., peakpos[p], meas_y1);
              if (p==0 )
                { 
                  printf("\r pedestal position saved...\n");
                  peakline[p]->SetLineColor(kBlue);
                }
              else
                {
                  printf("\r peak position %d saved...\n", p);
                  peakline[p]->SetLineColor(kRed);
                };
              peakline[p]->Draw();
              meas_status=0; // reset measurement status
              Update();
            };
          return;
        };
      //-----------------------------------------------------------------//
      if (px=='c')                        //      DELETE PEAKS           //
        {
          for (int p=0; p<10; p++)
            {
              peakpos[p]=0.0;
              if (peakline[p]) delete peakline[p];
              Update();
              printf("peak positions have been deleted...\n");
            };
        };
      //-----------------------------------------------------------------//
      if (px=='l')                        //          LOG Y              //
        {
	  TObject *graphics = gPad->GetSelected();
          if (strstr(graphics->IsA()->GetName(), "TH1"))
	    gPad->SetLogy(!(gPad->GetLogy()));
	  else if (strstr(graphics->IsA()->GetName(), "TGraph"))
	    gPad->SetLogy(!(gPad->GetLogy()));
	  else if (strstr(graphics->IsA()->GetName(), "TH2"))
	    gPad->SetLogz(!(gPad->GetLogz()));
          Update();
        };
      //-----------------------------------------------------------------//
      if (px=='g')                        //           GRID              //
        {
          gPad->SetGridx(!(gPad->GetGridx()));
          gPad->SetGridy(!(gPad->GetGridy()));
          Update();
        };
      //-----------------------------------------------------------------//
      if (px=='C')                        //        CONFIGURE            //
        {
          Configure();
        };
      //-----------------------------------------------------------------//
      if (px=='m')                        //         MAGNIFY             //
        {
          TVirtualPad *thispad = gPad;
          // If there's already a maximized pad, shrink it to normal size again:
          if (MaximizedPad)
            {
              //printf(" shrinking old maximized pad to ( %lf ; %lf ) ( %lf ; %lf )...\n",
              //     MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup);
              MaximizedPad->SetPad(MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup);
            };
          // If the former maxmized pad is no equal to the selected pad, maximize selected pad:
          if (MaximizedPad!=thispad)
            {
              //printf(" maximizing selected pad...\n");
              // store original size of selected pad:
              thispad->GetPadPar(MaxmizedPad_xlow, MaxmizedPad_ylow, MaxmizedPad_xup, MaxmizedPad_yup);
              thispad->SetPad(0.05, 0.05, 0.95, 0.95);
              thispad->Pop();
              MaximizedPad = thispad;
            }
          else
            MaximizedPad = NULL;
          thispad->Update();
        };
    };
  //-----------------------------------------------------------------//
  // finally, if not yet returned, call default HandleInput method:
  TCanvas::HandleInput(event, px, py);
}
void TSuperCanvas::Measure(EEventType event, Int_t px, Int_t py)
{
  if (event) { }; // get rid of compiler warning "unused parameter"...
  double dx, dy;
  TPad *pad = Pick(px, py, NULL);
  pad->DrawCrosshair();
  switch(meas_status)
    {
    case 0:
      meas_x1 = pad->XtoPad(pad->AbsPixeltoX(px));
      meas_y1 = pad->YtoPad(pad->AbsPixeltoY(py));
      meas_status = 1;
      printf(" dist. meas.: "); fflush(stdout);
      break;
    case 1:
      meas_x2 = pad->XtoPad(pad->AbsPixeltoX(px));
      meas_y2 = pad->XtoPad(pad->AbsPixeltoY(py));
      dx = meas_x2-meas_x1;
      dy = meas_y2-meas_y1;
      meas_status = 0;
      printf(" dx = %e\tdy = %e\tdist=%e\n", dx, dy, sqrt(dx*dx+dy*dy));
      break;
    };
}
void TSuperCanvas::Configure()
{
  char input[512];
  printf(" --- Configuration of TSuperCanvas: ---\n\n");
  printf("Sampling rate: (currently %e Hz) ", SamplingRate); int retval=scanf("%s", input);
  if (atof(input)>0.0)
    SamplingRate = atof(input);
  printf("  SamplingRate = %e Hz\n\n", SamplingRate);
  printf("Scroll rate: (currently %e %%) ", ScrollRate); retval=scanf("%s", input);
  if ((atof(input)>0.0)&&(atof(input)<100.0))
    SamplingRate = atof(input)/100.0;
  printf("  ScrollRate = %e %%\n", ScrollRate*100.0);
}
//##################################################################################################################//
//##################################################################################################################//
// TMCH1::TMCH1(const TH1&) : TH1()
// {
// }
// Double_t TMCH1::GetRandom(TRandom *generator)
// {
//   TRandom *gen=NULL;
//   if (generator==NULL)
//     gen = gRandom;
//   // The following code is taken from TH1::GetRandom and modified in order to use
//   // "gen" as random generator instead of gRandom:
//    if (fDimension > 1) {
//       Error("GetRandom","Function only valid for 1-d histograms");
//       return 0;
//    }
//    Int_t nbinsx = GetNbinsX();
//    Double_t integral;
//    if (fIntegral) {
//       if (fIntegral[nbinsx+1] != fEntries) integral = ((TH1*)this)->ComputeIntegral();
//    } else {
//       integral = ((TH1*)this)->ComputeIntegral();
//       if (integral == 0 || fIntegral == 0) return 0;
//    }
//    Double_t r1 = gen->Rndm();
//    Int_t ibin = TMath::BinarySearch(nbinsx,fIntegral,r1);
//    Double_t x = GetBinLowEdge(ibin+1);
//    if (r1 > fIntegral[ibin]) x +=
//       GetBinWidth(ibin+1)*(r1-fIntegral[ibin])/(fIntegral[ibin+1] - fIntegral[ibin]);
//    return x;
// }
//##################################################################################################################//
//##################################################################################################################//
// TTiming::TTiming(const TH1&) : TH1()
// {
// }
// TH1* TTiming::GetHisto(int bins)
// {
//   double min = GetBinContent(GetMinimumBin());
//   double max = GetBinContent(GetMaximumBin());
//   if (bins==0)
//     bins = round(max-min+1);
//   TH1I *h = new TH1I(Form("%s_histo", GetName()), Form("%s_histo", GetTitle()), bins, min, max);
//   for (int i=1; i<=GetNbinsX(); i++)
//     h->Fill(GetBinContent(i));
//   return h;
// }
//##################################################################################################################//
//##################################################################################################################//

