//
//  EPulseHeightCorrection.h
//  
//
//  Created by Erez Cohen on 1/14/15.
//
//

//#ifndef ____EPulseHeightCorrection__
//#define ____EPulseHeightCorrection__

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <fstream>

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TProfile.h>
#include <TPad.h>
#include <TStyle.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TChain.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TMath.h>



class EPulseHeightCorrection{
private:
    TTree * TDCTree;
    TTree * ADCTree;
    TBranch * TDCBranch;
    TBranch * ADCBranch;
    int TDCChannel;
    int ADCChannel;
    int offset;
    std::vector <double> t;
    std::vector <double> a;
    TH1F *ht;
    TH1F *ha;
    TH2F *hta;
    
    
public:
    // constructors
    EPulseHeightCorrection();
    EPulseHeightCorrection( TTree* TDCTree , TTree* ADCTree , TBranch* TDCBranch , TBranch* ADCBranch , int TDCChannel , int ADCChannel , int offset=0 );

    // destructor
    ~EPulseHeightCorrection();
    
    // methods
    int GetTDCChannel();
    int GetADCChannel();
    void SetTDCChannel(int TDC);
    void SetADCChannel(int ADC);
    
    int GetTDCEntries();
    int GetADCEntries();
    void GetTDCADCSpectra();
    
    void DrawTDC();
    void DrawADC();
    void DrawTDCvsADC();

};

//#endif /* defined(____EPulseHeightCorrection__) */
