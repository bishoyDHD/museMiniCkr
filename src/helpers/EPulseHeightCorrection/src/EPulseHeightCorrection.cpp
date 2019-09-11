//
//  EPulseHeightCorrection.cpp
//  
//
//  Created by Erez Cohen on 1/14/15.
//
//

#include "helpers/EPulseHeightCorrection.h"

// constructors
EPulseHeightCorrection::EPulseHeightCorrection(){}


EPulseHeightCorrection::EPulseHeightCorrection( TTree* TDCTree , TTree* ADCTree , TBranch* TDCBranch , TBranch* ADCBranch , int TDCChannel , int ADCChannel , int offset ){
    this -> TDCTree = TDCTree;
    this -> ADCTree = ADCTree;
    this -> TDCBranch = TDCBranch;
    this -> ADCBranch = ADCBranch;
    this -> TDCChannel = TDCChannel;
    this -> ADCChannel = ADCChannel;
    this -> offset = offset;        // offset between TDC tree and ADC tree
    ht = new TH1F("ht","TDC spectrum", 1201 , -300 , 300);
    ha = new TH1F("ha","ADC spectrum", 4097 , 0 , 4096);
    hta = new TH2F("hta","TDC-ADC correlation", 1201 , -300 , 300 , 4097 , 0 , 4096);
}


// destructor
EPulseHeightCorrection::~EPulseHeightCorrection(){}


// methods
int EPulseHeightCorrection::GetTDCChannel(){
    return this -> TDCChannel;
}
int EPulseHeightCorrection::GetADCChannel(){
    return this -> ADCChannel;
}
void EPulseHeightCorrection::SetTDCChannel(int TDCChannel){
    this -> TDCChannel = TDCChannel;
}
void EPulseHeightCorrection::SetADCChannel(int ADCChannel){
    this -> ADCChannel = ADCChannel;
}

int EPulseHeightCorrection::GetTDCEntries(){
    return this -> TDCTree -> GetEntries();
}

int EPulseHeightCorrection::GetADCEntries(){
    return this -> ADCTree -> GetEntries();
}

void EPulseHeightCorrection::GetTDCADCSpectra() {
    
    for (int i = offset; i < GetTDCEntries() ; i++) {
       
        // -- TDC Data -- //
        TDCTree -> GetEntry( i );
        t.push_back( i );//TDCBranch -> calibratedtime[TDCChannel] );
        ht -> Fill(t.back());
        
        
        // -- ADC Data -- //
        ADCTree -> GetEntry( i-offset );
        a.push_back(  i-offset );//ADCBranch -> adcvalue[ADCChannel] );
        ha -> Fill(a.back());
        
        hta -> Fill(t.back(),a.back());
    }
    
}

void EPulseHeightCorrection::DrawTDC() {
    TCanvas * cTDC = new TCanvas();
    ht -> GetXaxis() -> SetTitle("TDC [ns]");
    ht -> Draw();
}

void EPulseHeightCorrection::DrawADC() {
    TCanvas * cADC = new TCanvas();
    ha -> GetXaxis() -> SetTitle("ADC");
    ha -> Draw();
}

void EPulseHeightCorrection::DrawTDCvsADC() {
    TCanvas * cTDCADC = new TCanvas();
    hta -> GetXaxis() -> SetTitle("TDC [ns]");
    hta -> GetYaxis() -> SetTitle("ADC");
    hta -> Draw("colz");
}






