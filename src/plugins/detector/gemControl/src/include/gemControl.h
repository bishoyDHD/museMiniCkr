/* 
 * Declarations of the gemControl class and methods
 * Noah Wuerfel nwuerfel@umich.edu 10-12-18 goblu
 * ~AP AP AP AP~
 */

#ifndef __GEMCONTROL__
#define __GEMCONTROL__

#include "TObject.h"
#include "Plugin.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "lumigemtree.h"
#include "gemControltree.h"
#include "gem.h"
#include "binaryBlobReader.h"
#include "gemTypes.h"


#include <iostream>
#include <sstream>

class gemControl:public Plugin{
    private:

        Long_t initInput();
        Long_t initOutput();

        size_t readRawEventData(bool dump=false);

        // debug
        // ~~~~~
        void dumpAllGemInfo();
        void dumpAllHistos();
        void testfun(int trash, int i);

        std::vector<gem*> gem_list;

        // each gem gets an x and y spectra
        std::vector<TH2D*> gem_spectra_x;
        std::vector<TH2D*> gem_spectra_y;
        std::vector<std::vector<TH2D*>> adc_spectra;
        // and hitmap
        std::vector<TH2D*> gem_hitmaps;
        double cmode_data[NUM_GEMS][NUM_APV_PER_GEM];
        double cmode_data_ct[NUM_GEMS][NUM_APV_PER_GEM];

        MRTBinaryBlob* binInputDataHook=NULL;
        MRTEventInfo* eventInfo=NULL;
        MRTRunInfo* runInfo=NULL;

        binaryBlobReader* binBlobReader;

        uint32_t event_number=0;


    public:
        gemControl(TTree *in, TTree *out,TFile *inf_, 
            TFile * outf_, TObject *p);

        virtual ~gemControl();
        virtual Long_t cmdline(char * cmd);

        Long_t defineHistograms();
        Long_t startup();
        Long_t readCommonMode();
        Long_t readFrequentCommonMode();
        Long_t frequentCommonMode();
        Long_t determineCommonMode();
        Long_t process();
        Long_t process_avg();
        Long_t finalize();
        Long_t associateApvToGem(int trash, uint32_t gem_id, 
            uint32_t apv_id, Axis apv_axis, uint32_t chan_to_skip);

        Long_t frequentTest();

        void dumpFrequentCmodeInfo();
        void dumpCmodeInfo();

        ClassDef(gemControl,1);
};

#endif /*__GEMCONTROL__*/
