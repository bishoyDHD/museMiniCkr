/* 
 * Definitions of the gem class and methods
 * Noah Wuerfel nwuerfel@umich.edu 10-12-18 goblu
 * ~AP AP AP AP~
 *
 */

#include "gem.h"
#include <assert.h>
#include <algorithm>

gem::gem(uint32_t id) : id(id){
    name = gem_names[id];
}

gem::~gem(){
    // free all apvs
    for(std::vector<apv*>::size_type i=0; i < apv_list.size(); i++){
        delete apv_list[i];
    }

}

// adds an APV to the apv_list of a gem, checks for bad init
// returns -1 on bad
int gem::addApv(Axis axis_to_add, uint32_t apv_id, 
    uint32_t chan_to_skip){
    // check we're not over max
    assert(apv_list.size() < NUM_APV_PER_GEM);
    uint32_t num_of_axis = 0;
    for(std::vector<apv*>::size_type i=0; i < apv_list.size(); i++){
        if(apv_list[i]){
            if(apv_list[i]->axis == axis_to_add){
                num_of_axis++;
                // check for no duplicate apv_ids per axis
                assert(apv_id != apv_list[i]->id);
            }
        }
    }
    if(axis_to_add==Axis::X){
        assert(num_of_axis < NUM_X_APV);
    }
    else{
        assert(num_of_axis < NUM_Y_APV);
    }

    // cleaned up by gem at EOL
    apv* apv_to_add = new apv(axis_to_add, apv_id, chan_to_skip);
    if(!apv_to_add){
        std::cout << "failed to create apv of axis: " << axis_to_add << 
            " and id: " << apv_id;
        return -1;
    }
    apv_list.push_back(apv_to_add);
    return 0;
}

void gem::clearGemData(){
    x_apv_1_data=NULL;
    x_apv_2_data=NULL;
    y_apv_1_data=NULL;
    y_apv_2_data=NULL;
    gem_clusters.clear();
}

// stupid data copy version
void gem::setGemAxisData(){
    for(std::vector<apv*>::size_type i=0; i < apv_list.size(); i++){
        if(apv_list[i]->axis == Axis::X){
            if(apv_list[i]->num_chan_to_skip!=0){
                for(uint32_t j=apv_list[i]->num_chan_to_skip;
                    j<NUM_MAX_CHAN_PER_APV;j++){
                    x_data[j-apv_list[i]->num_chan_to_skip]=
                        *(apv_list[i]->apv_channel_data 
                        + inverse_mapping[j]);
                    x_apv_1_cmode = apv_list[i]->cmode;
                }
            }
            else{
                for(uint32_t j=0; j<NUM_MAX_CHAN_PER_APV;j++){
                    x_data[j+NUM_CHAN_APV_1]=*(apv_list[i]->\
                        apv_channel_data + inverse_mapping[j]);
                    x_apv_2_cmode = apv_list[i]->cmode;
                }            
            }
        }
        else{
            if(apv_list[i]->num_chan_to_skip!=0){
                for(uint32_t j=apv_list[i]->num_chan_to_skip; 
                    j<NUM_MAX_CHAN_PER_APV;j++){
                    y_data[j-apv_list[i]->num_chan_to_skip]=
                        *(apv_list[i]->apv_channel_data 
                        + inverse_mapping[j]);
                    y_apv_1_cmode = apv_list[i]->cmode;
                }            
            }
            else{
                for(uint32_t j=0; j<NUM_MAX_CHAN_PER_APV;j++){
                    y_data[j+NUM_CHAN_APV_1]=*(apv_list[i]->\
                        apv_channel_data + inverse_mapping[j]);
                    y_apv_2_cmode = apv_list[i]->cmode;
                }            
            }
        }
    }
}

//debugging the skipped chanz
/*void gem::setGemAxisData(){
    for(std::vector<apv*>::size_type i=0; i < apv_list.size(); i++){
        if(apv_list[i]->axis == Axis::X){
            if(apv_list[i]->num_chan_to_skip!=0){
                x_apv_1_data= (uint32_t* )(apv_list[i]->apv_channel_data);
                x_apv_1_cmode = apv_list[i]->cmode;
            }
            else{
                x_apv_2_data=apv_list[i]->apv_channel_data;
                x_apv_2_cmode = apv_list[i]->cmode;
            }
        }
        else{
            if(apv_list[i]->num_chan_to_skip!=0){
                y_apv_1_data= (uint32_t* )(apv_list[i]->apv_channel_data);
                y_apv_1_cmode = apv_list[i]->cmode;
            }
            else{
                y_apv_2_data=apv_list[i]->apv_channel_data;
                y_apv_2_cmode = apv_list[i]->cmode;
            }
        }
    }
}
*/
/*
void gem::setGemAxisData(){
    for(std::vector<apv*>::size_type i=0; i < apv_list.size(); i++){
        if(apv_list[i]->axis == Axis::X){
            if(apv_list[i]->num_chan_to_skip!=0){
                x_apv_1_data= (uint32_t* )(apv_list[i]->apv_channel_data 
                    + apv_list[i]->num_chan_to_skip);
                x_apv_1_cmode = apv_list[i]->cmode;
            }
            else{
                x_apv_2_data=apv_list[i]->apv_channel_data;
                x_apv_2_cmode = apv_list[i]->cmode;
            }
        }
        else{
            if(apv_list[i]->num_chan_to_skip!=0){
                y_apv_1_data= (uint32_t* )(apv_list[i]->apv_channel_data 
                    + apv_list[i]->num_chan_to_skip);
                y_apv_1_cmode = apv_list[i]->cmode;
            }
            else{
                y_apv_2_data=apv_list[i]->apv_channel_data;
                y_apv_2_cmode = apv_list[i]->cmode;
            }
        }
    }
}
*/

// NOTE THE FREQUENT CMODE DOESNT HAVE THE SAME FORMAT last 6 are dead
// rathre than front 6 so no num_chan to skip offset, probably should
// change that but my design fell apart like my life
/*void gem::setGemAxisData(){
    for(std::vector<apv*>::size_type i=0; i < apv_list.size(); i++){
        if(apv_list[i]->axis == Axis::X){
            if(apv_list[i]->num_chan_to_skip!=0){
                x_apv_1_data= (uint32_t* )(apv_list[i]->apv_channel_data 
                    + apv_list[i]->num_chan_to_skip);
                x_apv_1_cmode = apv_list[i]->frequent_cmode;
            }
            else{
                x_apv_2_data=apv_list[i]->apv_channel_data;
                x_apv_2_cmode = apv_list[i]->frequent_cmode;
            }
        }
        else{
            if(apv_list[i]->num_chan_to_skip!=0){
                y_apv_1_data= (uint32_t* )(apv_list[i]->apv_channel_data 
                    + apv_list[i]->num_chan_to_skip);
                y_apv_1_cmode = apv_list[i]->frequent_cmode;
            }
            else{
                y_apv_2_data=apv_list[i]->apv_channel_data;
                y_apv_2_cmode = apv_list[i]->frequent_cmode;
            }
        }
    }
}*/

// custom comparator for findclusters
bool gem::comparePeaks(apv_peak_info p1, apv_peak_info p2){
    return(p1.peak_adc_value < p2.peak_adc_value);
}

// helper for cluster finder
bool checkPeakListForElement(const std::vector<apv_peak_info>& vec,
    apv_peak_info peak){

    for(std::vector<apv_peak_info>::size_type i = 0; i < vec.size(); i++){
        if(peak.max_channel_num == vec[i].max_channel_num){
            return true;
        }
    }
    return false;
}

// take initialized vector of length NUM_MAX_CLUSTER_CANDIDATES and
// then fills them with peaks
// find cluster candidates
// the game is to check each chan against list of candidates
// the array gets sorted so that we kick out smaller peaks
// grows as cN so keeping num candidates low is desirable 
// TODO UNSIGNED TO SIGNED CONVERSION IS DANGEROUS AM I GUARUNTEED TO
// NEVER GET A VALUE LARGER THAN INT MAX??
// with only 12 bit data words i'd be suprised?

void gem::findPeaks(std::vector<apv_peak_info>& peaks, Axis axis){
    assert(peaks.size() == NUM_MAX_CLUSTER_CANDIDATES);

    int adc_val;
    uint32_t cmode;
    bool found;
    bool replaced;
    apv_peak_info temp_peak;

    for(uint32_t i=0; i<NUM_MAX_CLUSTER_CANDIDATES; i++){
        peaks[i].max_channel_num = -1; 
        peaks[i].peak_adc_value = -10000;
    }

    // now find peaks
    for(uint32_t i=0; i< NUM_MAX_CLUSTER_CANDIDATES; i++){
        for(uint32_t j=0; j<NUM_CHAN_PER_AXIS; j++){
            adc_val = (int)*(getDataPointerForChannel(j,axis));
            cmode = getCmodeForChannel(j,axis);
            adc_val-=cmode;
            for(uint32_t k=0; k<peaks.size(); k++){
                if(replaced){
                    break;
                }
                if(!replaced && adc_val>peaks[k].peak_adc_value){
                // finally check if we already have it
                    temp_peak.max_channel_num=j;
                    temp_peak.peak_adc_value=adc_val;
                    found = checkPeakListForElement(peaks,temp_peak);
                    if(!found){
                        peaks[k]=temp_peak;
                        replaced=true;
                    }
                }
            }
            if(replaced){
                std::sort(peaks.begin(),peaks.end(),comparePeaks);
                replaced = false;
            }
        }
    }
}

bool compareClusters(gem_cluster c1, gem_cluster c2){
    return (c1.adc_xy_avg_weight>c2.adc_xy_avg_weight);
}

void gem::findClusters(int event_id){

    bool x_replaced = false; 
    bool y_replaced = false;
    bool found;
    int x_adc_val;
    int y_adc_val;
    apv_peak_info temp_peak;

    // setup candidate vectors
    std::vector<apv_peak_info> x_peaks(NUM_MAX_CLUSTER_CANDIDATES);
    std::vector<apv_peak_info> y_peaks(NUM_MAX_CLUSTER_CANDIDATES);

    findPeaks(x_peaks,Axis::X);
    findPeaks(y_peaks,Axis::Y);

    //DEBUG
    if(event_id==0){
        x_axis_peaks = x_peaks;
        y_axis_peaks = y_peaks;
    }
/*
    std::cout << "forwards peaks: \n";
    for(int i=0; i<NUM_MAX_CLUSTER_CANDIDATES;i++){
        std::cout << "peak: " << i << " chan: " 
            << x_peaks[i].max_channel_num << " data: " 
            << x_peaks[i].peak_adc_value  << std::endl;
    }

    std::cout << "backwards peaks: \n";
    for(int i = NUM_MAX_CLUSTER_CANDIDATES -1; i>-1; i--){
        std::cout << "peak: " << i << " chan: " 
            << x_peaks[i].max_channel_num << " data: " 
            << x_peaks[i].peak_adc_value  << std::endl;
    }
*/

    // take combos and find maxavg
    // now that peaks are found we take averages to give weight to them
    // sorted from smallest to largest so go backwards
    // O(N^2)

    gem_cluster cluster_candidate;

    for(int i=NUM_MAX_CLUSTER_CANDIDATES-1; i>-1; i--){
        uint32_t x_chan = x_peaks[i].max_channel_num;
        for(int j=NUM_MAX_CLUSTER_CANDIDATES-1; j>-1; j--){
            uint32_t y_chan = y_peaks[j].max_channel_num;

            double weight = crossAverage(x_chan,y_chan);
            
            cluster_candidate.event_id = event_id;
            cluster_candidate.gem_id = id;
            cluster_candidate.x_chan_num = x_chan;
            cluster_candidate.y_chan_num = y_chan;
            cluster_candidate.adc_xy_avg_weight = weight;

            gem_clusters.push_back(cluster_candidate);
            
        }
    }
    // after making all clusters sort by weight and take the highest
    // weights
    // sorted in decreasing weight
    std::sort(gem_clusters.begin(),gem_clusters.end(),compareClusters);
}


// check rollover and going outside range
bool channelInRange(uint32_t chan){
    return (chan < NUM_CHAN_PER_AXIS && chan < INT_MAX);
}

// handles taking cross average to generate cluster weight
// checks and ensures we don't leave our data blobs when crossing APV
// boundary. At edges of the gem, assigns a value of 0 to those points
double gem::crossAverage(uint32_t x_chan, uint32_t y_chan){

    uint32_t* x_ptr = getDataPointerForChannel(x_chan, Axis::X);
    uint32_t* y_ptr = getDataPointerForChannel(y_chan, Axis::Y);
    uint32_t x_cmode = getCmodeForChannel(x_chan, Axis::X);
    uint32_t y_cmode = getCmodeForChannel(y_chan, Axis::Y);
    uint32_t x_corner_chan;
    uint32_t y_corner_chan;
    bool x_in_range;
    bool y_in_range;

    assert(x_chan < NUM_CHAN_PER_AXIS && y_chan < NUM_CHAN_PER_AXIS);
    
    double weight = 4.0 * averageCharge((int)*x_ptr-x_cmode,
        (int)*y_ptr-y_cmode) * NUM_NEIGHBOR_CHANNELS;

    double neighbor_weight = 0;

//    std::cout << "starting from channels- x: " << x_chan 
 //       << " y: " << y_chan << " with weight: "  << weight << std::endl;

    // sum over corners going out as far as we choose
    // start at 1 or you're not going anywhere
    for(int i = 1; i<NUM_NEIGHBOR_CHANNELS+1; i++){
        for(int j = -1; j<2; j+=2){
            x_corner_chan = x_chan + i * j;
            x_in_range = channelInRange(x_corner_chan);
            for(int k = -1; k<2; k+=2){
                y_corner_chan = y_chan + i * k; 
                y_in_range = channelInRange(y_corner_chan); 

  //              std::cout << "corners are- x: " << x_corner_chan
   //                 << " y: " << y_corner_chan << std::endl;

                if(x_in_range && y_in_range){
                    x_ptr = getDataPointerForChannel(x_corner_chan,
                        Axis::X); 

                    x_cmode = getCmodeForChannel(x_corner_chan, Axis::X);

    //                std::cout << "raw x_weight: " << (int)*x_ptr-x_cmode
     //                   << std::endl;

                    y_ptr = getDataPointerForChannel(y_corner_chan,
                        Axis::Y);

                    y_cmode = getCmodeForChannel(y_corner_chan, Axis::Y);

      //              std::cout << "raw y_weight: " << *y_ptr-y_cmode 
       //                 << std::endl;

                    neighbor_weight += averageCharge((int)*x_ptr-x_cmode,
                        (int)*y_ptr-y_cmode);
                }
                // nonexistant neighbor channels contribute 0
                else{
                    neighbor_weight += 0;
                }

        //        std::cout << "corner weight is: " << neighbor_weight
         //           << std::endl;
            }
        }
    }

    return weight-neighbor_weight;
}


double gem::averageCharge(int x_charge, int y_charge){
    return ((double) x_charge + (double) y_charge)/2.0;
}

// debug tool
uint32_t* gem::getDataPointerForAllChannel(uint32_t apv_channel, 
    Axis axis){
    uint32_t* data_ptr = NULL;
    uint32_t offset = 0;
    if(axis == Axis::X){
        if(apv_channel < 128){
            data_ptr = x_apv_1_data;
            offset = apv_channel;
        }
        else{
            data_ptr = x_apv_2_data;
            offset = apv_channel - 128;
        }
    }
    else{
        if(apv_channel < 128){
            data_ptr = y_apv_1_data;
            offset = apv_channel;
        }
        else{
            data_ptr = y_apv_2_data;
            offset = apv_channel - 128;
        }
    }
    // TEMP TEMP TEMP TODO 
    //offset = inverse_mapping[offset];
    return data_ptr+offset;
}
// after copying data
uint32_t* gem::getDataPointerForChannel(uint32_t apv_channel, 
    Axis axis){
    assert(apv_channel <= NUM_CHAN_PER_AXIS);
    uint32_t* data_ptr = NULL;
    uint32_t offset = 0;
    if(axis == Axis::X){
        data_ptr = &(x_data[apv_channel]);
    }
    else{
        data_ptr = &(y_data[apv_channel]);
    }
    return data_ptr;
}

// again clunky because I have two pointers to deal with per axis so I
// couldn't think of a clean wrapper here, TODO clean up a bit
/*uint32_t* gem::getDataPointerForChannel(uint32_t apv_channel, 
    Axis axis){
    assert(apv_channel <= NUM_CHAN_PER_AXIS);
    uint32_t* data_ptr = NULL;
    uint32_t offset = 0;
    if(axis == Axis::X){
        if(apv_channel < NUM_CHAN_APV_1){
            data_ptr = x_apv_1_data;
            offset = apv_channel;
        }
        else{
            data_ptr = x_apv_2_data;
            offset = apv_channel - NUM_CHAN_APV_1;
        }
    }
    else{
        if(apv_channel < NUM_CHAN_APV_1){
            data_ptr = y_apv_1_data;
            offset = apv_channel;
        }
        else{
            data_ptr = y_apv_2_data;
            offset = apv_channel - NUM_CHAN_APV_1;
        }
    }
    // TEMP TEMP TEMP TODO 
    //offset = inverse_mapping[offset];
    return data_ptr+offset;
}
*/
uint32_t gem::getCmodeForChannel(uint32_t apv_channel, Axis axis){

    float cmode;
    assert(apv_channel <= NUM_CHAN_PER_AXIS);
    if(axis == Axis::X){
        if(apv_channel < NUM_CHAN_APV_1){
            cmode = x_apv_1_cmode;
        }
        else{
            cmode = x_apv_2_cmode;
        }
    }
    else{
        if(apv_channel < NUM_CHAN_APV_1){
            cmode = y_apv_1_cmode;
        }
        else{
            cmode = y_apv_2_cmode;
        }
    }

    return static_cast<unsigned int>(cmode);
}

/*
uint32_t gem::getCmodeForChannel(uint32_t apv_channel, Axis axis){

    assert(apv_channel <= NUM_CHAN_PER_AXIS);

    uint32_t* cmode_ptr = NULL;
    uint32_t offset = 0; 

    
    if(axis == Axis::X){
        if(apv_channel < NUM_CHAN_APV_1){
            cmode_ptr = x_apv_1_cmode;
            offset = apv_channel;
        }
        else{
            cmode_ptr = x_apv_2_cmode;
            offset= apv_channel - NUM_CHAN_APV_1;
        }
    }
    else{
        if(apv_channel < NUM_CHAN_APV_1){
            cmode_ptr = y_apv_1_cmode;
            offset = apv_channel;
        }
        else{
            cmode_ptr = y_apv_2_cmode;
            offset = apv_channel - NUM_CHAN_APV_1;
        }
    }

    // TEMP TEMP TEMP TODO
    offset = inverse_mapping[offset];

    return *(cmode_ptr+offset);

}
*/

// debugging tools
void gem::printApvList(){
    for(std::vector<apv*>::size_type i=0; i < apv_list.size(); i++){
        apv_list[i]->dumpApvInfo();
    }
}

void gem::dumpGemInfo(){
    std::cout << std::endl;
    std::cout << "dumping info for GEM: " << id << std::endl;
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~\n";
    std::cout << "name: " << name << std::endl;
    std::cout << "x peaks for event 0:\n";
    std::cout << "cmodes: \n";
    //std::cout << "x_1: " << *x_apv_1_cmode << " x_2: " << *x_apv_2_cmode
    //    <<std::endl;
    for(std::vector<apv_peak_info>::size_type i=0; i<x_axis_peaks.size();
        i++){
        std::cout << "peak: " << i << " channel: " 
            << x_axis_peaks[i].max_channel_num << "adc value: " 
            << x_axis_peaks[i].peak_adc_value << std::endl; 
    }
    std::cout << "y peaks for event 0: \n";
    for(std::vector<apv_peak_info>::size_type i=0; i<y_axis_peaks.size();
        i++){
        std::cout << "peak: " << i << " channel: " 
            << y_axis_peaks[i].max_channel_num << "adc value: " 
            << y_axis_peaks[i].peak_adc_value << std::endl; 
    }
    std::cout << "clusters for event 0: \n";
    for(std::vector<gem_cluster>::size_type i=0; i<gem_clusters.size();
        i++){
        std::cout << "cluster: " << i << " event_id: " 
            << gem_clusters[i].event_id << " x_chan: " 
            << gem_clusters[i].x_chan_num << " y_chan: "
            << gem_clusters[i].y_chan_num << " weight: "
            << gem_clusters[i].adc_xy_avg_weight << std::endl;
    }

    printApvList();
}
