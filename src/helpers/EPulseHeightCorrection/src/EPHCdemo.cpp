#include "helpers/EPulseHeightCorrection.h"

#include "muserawtree.h"
#include "musetdc1190tree.h"
#include "museadctree.h"


int main(int argc, char **argv)
{

  if (argc!=2)
    {
      printf("Please give root file as argument\n");
      exit(-1);
    }
    gROOT->SetStyle("Modern");

    // Read in the TDC and TRB files. TChains may not work
    TFile *file = new TFile(argv[1]);
    TTree *TDCTree = (TTree*)file->Get("tdcadccooked");
    TTree *ADCTree = (TTree*)file->Get("tdcadccooked");
    TBranch *TDCBranch = TDCTree -> GetBranch("v1190");
    TBranch *ADCBranch = TDCTree -> GetBranch("v792");

    int TDCChannel = 0 , ADCChannel = 16 , offset = 0;
    
    EPulseHeightCorrection *phc = new EPulseHeightCorrection(TDCTree,ADCTree,TDCBranch,ADCBranch,TDCChannel,ADCChannel,offset);
    std::cout << Form("Considering TDC %d vs. ADC %d",
                      phc -> GetTDCChannel() , phc -> GetADCChannel()) << std::endl;
    phc -> DrawTDC();
    phc -> DrawADC();
    phc -> DrawTDCvsADC();
   
    return 0;
}
