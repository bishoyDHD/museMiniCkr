/* 
 * Definitions of the APV class and methods
 * Noah Wuerfel nwuerfel@umich.edu 10-12-18 goblu
 * ~AP AP AP AP~
 */

#include "apv.h"

apv::apv(Axis axis, uint32_t id, uint32_t chan_to_skip) 
    : axis(axis), id(id), num_chan_to_skip(chan_to_skip){
}

apv::~apv(){

}

void apv::clearApvData(bool frequent){
    apv_channel_data = NULL;
}


// decays array to a pointer, but we always know length of blob
void apv::loadApvData(uint32_t* raw_data){
    apv_channel_data = raw_data;
}

// debugging tools
void apv::dumpApvInfo(){
    std::cout << "dumping info for APV: " << id << std::endl;
    std::cout << "Axis: " << (char)axis << std::endl;
    std::cout << "Skipping first " << num_chan_to_skip << " channels\n";
}

void apv::dumpApvData(){
    assert(apv_channel_data != NULL);
    std::cout << "apv: " << id << std::endl;
    for(uint32_t i = 0; i < NUM_MAX_CHAN_PER_APV; i++){
        std::cout << "data word: " << i << " got: " 
            << apv_channel_data[i] << std::endl;
    }
}

