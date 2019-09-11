/* 
 * Definitions of the gemControl class and methods
 * Noah Wuerfel nwuerfel@umich.edu 10-12-18 goblu
 * ~AP AP AP AP~
 */

#include <gemControl.h>
#include <iostream>
#include <fstream>
#include <cmath>

gemControl::gemControl(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):Plugin(in,out,inf_,outf_,p){
    // gem's labeled by ID from upstream to downstream
    // cleaned up in destructor
    for(std::vector<gem*>::size_type i=0; i < NUM_GEMS; i++){
        gem* gem_to_add = new gem(i);
        if(!gem_to_add){
            std::cout << "failed to create gem with id: " << i
                << std::endl;
            exit(-1);
        }
        gem_list.push_back(gem_to_add);
    }
}

// clean up
gemControl::~gemControl(){
    std::cout << "cleaning up...\n";
    std::cout << "cleaning up blob reader...\n";
    delete binBlobReader;
    std::cout << "cleaning up gems...\n";
    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        assert(gem_list[i]!=NULL);
        delete gem_list[i];
    }
}

Long_t gemControl::defineHistograms(){
    std::cout << "here it is, the big gemControl...\n";
    std::vector<TH2D*> temp_apv_hist_vec;
    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        const char* name = gem_list[i]->name;

        TH2D* gem_h2_x = dH2(Form("adcSpec_gem_%i_x",(int)i), 
            Form("%s GEM X spectrum",name), NUM_CHAN_PER_AXIS, 
            SPEC_RANGE_MIN, NUM_CHAN_PER_AXIS,NUM_CHAN_PER_AXIS, 
            RANGE_MIN, RANGE_MAX); 

        TH2D* gem_h2_y = dH2(Form("adcSpec_gem_%i_y",(int)i), 
            Form("%s GEM Y spectrum",name), NUM_CHAN_PER_AXIS, 
            SPEC_RANGE_MIN, NUM_CHAN_PER_AXIS,NUM_CHAN_PER_AXIS, 
            RANGE_MIN, RANGE_MAX); 

        TH2D* gem_hitmap = dH2(Form("hitmap_gem_%i",(int)i),
            Form("%s GEM Hitmap",name), NUM_CHAN_PER_AXIS, 
            SPEC_RANGE_MIN,NUM_CHAN_PER_AXIS, NUM_CHAN_PER_AXIS,
            SPEC_RANGE_MIN, NUM_CHAN_PER_AXIS);

        for(std::vector<apv*>::size_type j=0; 
            j<gem_list[i]->apv_list.size(); j++){
            uint32_t apv_id = gem_list[i]->apv_list[j]->id;
            uint32_t num_skip = gem_list[i]->apv_list[j]->\
                num_chan_to_skip; 
            TH2D* h2 = dH2(Form("adcSpec_gem_%i_apv_%i", (int)i,
                (int)apv_id), Form("%s GEM Apv %i Spectrum",name,
                (int)apv_id), NUM_MAX_CHAN_PER_APV,SPEC_RANGE_MIN,
                NUM_MAX_CHAN_PER_APV, NUM_MAX_CHAN_PER_APV,RANGE_MIN, 
                RANGE_MAX);
            temp_apv_hist_vec.push_back(h2);
        }
        adc_spectra.push_back(temp_apv_hist_vec);
        gem_spectra_x.push_back(gem_h2_x);
        gem_spectra_y.push_back(gem_h2_y);
        gem_hitmaps.push_back(gem_hitmap);
        temp_apv_hist_vec.clear();
    }
    return 0;
}

// prep IO
Long_t gemControl::startup(){
    initInput();
    initOutput();
    binBlobReader = new binaryBlobReader();
    std::cout << "binBlobReader: " << binBlobReader << std::endl;
    return 0;
}

// read in calibration vals
Long_t gemControl::readCommonMode(){
    std::ifstream common_mode_data("gemControl_cmode.dat");
    if(!common_mode_data){
        std::cout << "no cmode data found... wrong file?\n";
        exit(-1);
    }
    std::string cmode_line;
    uint32_t gem_id,apv_id;
    float cmode_data_val;
    while(std::getline(common_mode_data,cmode_line)){
        std::istringstream line_data(cmode_line);
        line_data >> gem_id >> apv_id >> cmode_data_val;
        gem_list[gem_id]->apv_list[apv_id]->cmode = cmode_data_val;
        std::cout << "gem: " << gem_id << " apv: " << apv_id
            << " cmode read in: " << cmode_data_val << std::endl;
    }
}

Long_t gemControl::determineCommonMode(){
    // TODO clear output tree data holder
    // TODO visco stuff, not really interested in touching this

    // clear all APV data from last event
    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        gem_list[i]->clearGemData();
        for(std::vector<apv*>::size_type j=0; 
            j < gem_list[i]->apv_list.size(); j++){
            gem_list[i]->apv_list[j]->clearApvData();
        }
    }

    // fill APVs with raw data from input tree
    size_t apv_data_read = readRawEventData();
    if(apv_data_read == 0){
        std::cout << "no APV data for event: " << event_number 
            << std::endl;
        event_number++;
        return 0;
    }

    // fills apv specific histos
    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        gem_list[i]->setGemAxisData();
        for(std::vector<apv*>::size_type j=0; 
            j < gem_list[i]->apv_list.size(); j++){

            // point and counter
            uint32_t* data_ptr = 
                gem_list[i]->apv_list[j]->apv_channel_data;
            uint32_t first_valid_chan = gem_list[i]->\
                apv_list[j]->num_chan_to_skip;

            for(uint32_t k=first_valid_chan;k<NUM_MAX_CHAN_PER_APV;k++){
                adc_spectra[i][j]->Fill(k,
                    *(data_ptr+inverse_mapping[k]));
                cmode_data[i][j]+=*(data_ptr + inverse_mapping[k]);
                cmode_data_ct[i][j]++;
            }
        }
    }   

    // now I can do gems separately 
    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        for(uint32_t j=0; j<NUM_CHAN_PER_AXIS; j++){
            uint32_t* x_ptr = gem_list[i]->getDataPointerForChannel(j,
                Axis::X);
            uint32_t* y_ptr = gem_list[i]->getDataPointerForChannel(j,
                Axis::Y);
            gem_spectra_x[i]->Fill(j,*x_ptr);
            gem_spectra_y[i]->Fill(j,*y_ptr);
        }
    }

    event_number++;
    return 0;
}

Long_t gemControl::process_avg(){

    // TODO clear output tree data holder
    // TODO visco stuff, not really interested in touching this

    // clear all APV data from last event
    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        gem_list[i]->clearGemData();
        for(std::vector<apv*>::size_type j=0; 
            j < gem_list[i]->apv_list.size(); j++){
            gem_list[i]->apv_list[j]->clearApvData();
        }
    }

    // fill APVs with raw data from input tree
    size_t apv_data_read = readRawEventData();

    if(apv_data_read == 0){
        std::cout << "no APV data for event: " << event_number 
            << std::endl;
        event_number++;
        return 0;
    }

    // fills apv specific histos
    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        gem_list[i]->setGemAxisData();
        for(std::vector<apv*>::size_type j=0; 
            j < gem_list[i]->apv_list.size(); j++){
            // point and counter
            uint32_t* data_ptr = 
                gem_list[i]->apv_list[j]->apv_channel_data;
            uint32_t first_valid_chan = gem_list[i]->\
                apv_list[j]->num_chan_to_skip;
            uint32_t cmode = gem_list[i]->apv_list[j]->cmode;
            for(uint32_t k=first_valid_chan;k<NUM_MAX_CHAN_PER_APV;k++){
                adc_spectra[i][j]->Fill(k,
                   (int)(*(data_ptr+inverse_mapping[k]) - cmode));
            }
        }
    }   

    // now I can do gems separately 
    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        for(uint32_t j=0; j<NUM_CHAN_PER_AXIS; j++){
            uint32_t* x_ptr = gem_list[i]->getDataPointerForChannel(j,
                Axis::X);
            uint32_t x_cmode=gem_list[i]->getCmodeForChannel(j,Axis::X);
            uint32_t* y_ptr = gem_list[i]->getDataPointerForChannel(j,
                Axis::Y);
            uint32_t y_cmode=gem_list[i]->getCmodeForChannel(j,Axis::Y); 
            gem_spectra_x[i]->Fill(j,(int)(*x_ptr-x_cmode));
            gem_spectra_y[i]->Fill(j,(int)(*y_ptr-y_cmode));
        }
    }

    // now find clusters
    // generate hitmaps and we're done
    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        gem_list[i]->findClusters(event_number); 
        gem_cluster biggest_cluster = gem_list[i]->gem_clusters[0];
        gem_hitmaps[i]->Fill(biggest_cluster.x_chan_num,
            biggest_cluster.y_chan_num);
    }

    // TODO writeout the gem clusters
    event_number++;
    return 0;
}

// debugging things can go here as well mostly just writes out common mode
// values
Long_t gemControl::finalize(){
    //dumpAllHistos();
    dumpAllGemInfo();
    //dumpCmodeInfo();
    return 0;
}

Long_t gemControl::cmdline(char *cmd){
    return 0; // 0 = all ok
}

// wrapper for gem addAPV to get hooked in init script
Long_t gemControl::associateApvToGem(int trash, uint32_t gem_id, 
    uint32_t apv_id, Axis apv_axis, uint32_t chan_to_skip){

    // check this is a valid gem_id
    assert(gem_id < NUM_GEMS);
    assert(gem_list[gem_id]!=NULL);

    // add apv to the gem
    int ret = gem_list[gem_id]->addApv(apv_axis, apv_id, chan_to_skip);
    if(ret){
        std::cout << "failed to add apv: " << apv_id << "of axis: "
            << apv_axis << "to gem: " << gem_id << std::endl;
        exit(-1);
    }
    return 0;
}

// IO
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//TODO figure out how to error check this root IO garbage
Long_t gemControl::initInput(){
    getBranchObject("GEM1", (TObject**)&binInputDataHook);
    assert(binInputDataHook!=NULL);
    std::cout << "binInputDataHook: " << binInputDataHook << std::endl;
    runInfo = (MRTRunInfo*)getFileObject("RunInfo");
    assert(runInfo != NULL);
    std::cout << "runInfo: " << runInfo << std::endl;
    return 0;    
}

// TODO setup output tree for hits
Long_t gemControl::initOutput(){
    return 0;
}

// if dumping the raw data gets read out to a file, but the file gets
// appended so make sure to delete it between runs for a fresh set of data
size_t gemControl::readRawEventData(bool dump){
    assert(binBlobReader != NULL);
    assert(binInputDataHook != NULL);

    size_t data_read_size = binBlobReader->readBank(binInputDataHook);
    if(!data_read_size){
        return data_read_size;
    }

    for(std::vector<gem*>::size_type i=0; i<gem_list.size(); i++){
        for(std::vector<apv*>::size_type j=0; 
            j < gem_list[i]->apv_list.size(); j++){

            uint32_t apv_id = gem_list[i]->apv_list[j]->id;

            gem_list[i]->apv_list[j]->loadApvData(binBlobReader->
                rawDataBlob.apv_data[apv_id]);
        
            if(dump){
                std::ofstream data_outfile;
                data_outfile.open("gemControl_AllData.dat", 
                    std::fstream::out | std::fstream::app);
                data_outfile << "gem: " << i << " apv: " 
                    << apv_id << std::endl;
                for(int k=0; k<128; k++){
                    data_outfile << "raw data: " 
                    <<  *(binBlobReader->rawDataBlob.apv_data[apv_id] + k)
                        << std::endl;
                }
            }

        }
    }
    return data_read_size;
}


// debugging tools
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~ */
void gemControl::dumpAllGemInfo(){
    for(std::vector<gem*>::size_type i=0; i < NUM_GEMS; i++){
        if(gem_list[i]){
            gem_list[i]->dumpGemInfo();
        }
    }
}

// Idk how to get a TH name
void gemControl::dumpAllHistos(){
    std::cout << "dumping all histos:\n";
    std::cout << "~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "gem_spectra x...\n"; 
    for(std::vector<TH1D*>::size_type i=0; i<gem_spectra_x.size(); i++){
        std::cout << "got histo: " << gem_spectra_x[i] << std::endl; 
    }
    std::cout << "gem_spectra y...\n"; 
    for(std::vector<TH1D*>::size_type i=0; i<gem_spectra_y.size(); i++){
        std::cout << "got histo: " << gem_spectra_y[i] << std::endl; 
    }
    std::cout << "apv_spectra...\n";
    //TODO get the typing right on this, idk size type for nested vectors
    // same with getting size of one slice of the array
    for(uint32_t i = 0; i < NUM_GEMS; i++){
        for(uint32_t j=0; j < NUM_APV_PER_GEM; j++){
            std::cout << "gem: " << i << " apv " << j << std::endl;
            std::cout << "got histo: " << adc_spectra[i][j] << std::endl;
        }
    }
}

void gemControl::dumpCmodeInfo(){
    std::cout << "dumping common mode info:\n";
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    std::ofstream cmode_outfile("gemControl_cmode.dat");
    for(uint32_t i=0; i < NUM_GEMS; i++){
        for(uint32_t j=0; j<NUM_APV_PER_GEM; j++){
            double cmode = cmode_data[i][j]/cmode_data_ct[i][j];
            cmode_outfile << i << "\t" << j  << "\t" << cmode
                << std::endl;
        }
    }       
    cmode_outfile.close();
}

extern "C"{
    Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p){
        return (Plugin *) new gemControl(in,out,inf_,outf_,p);
    }
}

ClassImp(gemControl);
