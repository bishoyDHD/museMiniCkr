/* 
 * Declarations of the gem class and methods
 * Noah Wuerfel nwuerfel@umich.edu 10-12-18 goblu
 * ~AP AP AP AP~
 */
#ifndef __GEM_H__
#define __GEM_H__


#include "gemTypes.h"
#include "apv.h"

#include "limits.h"

#define GEMNAMESIZE 10

const char gem_names[NUM_GEMS][GEMNAMESIZE] = {{"US-IFP"},
    {"DS_IFP"},{"US"},{"US-MI"},{"DS-MI"},{"DS"}};

/*
const uint32_t inverse_mapping[NUM_MAX_CHAN_PER_APV] = {0, 16, 32, 48, 64,
80, 96, 112, 4, 20, 36, 52, 68, 84, 100, 116, 8, 24, 40, 56, 72, 88, 104, 
120, 12, 28, 44, 60, 76, 92, 108, 124, 1, 17, 33, 49, 65, 81, 97, 113, 5, 
21, 37, 53, 69, 85, 101, 117, 9, 25, 41, 57, 73, 89, 105, 121, 13, 29, 45,
61,77, 93, 109, 125, 2, 18, 34, 50, 66, 82, 98, 114, 6, 22, 38, 54, 70, 
86, 102, 118, 10, 26, 42, 58, 74, 90, 106, 122, 14, 30, 46, 62, 78, 94, 
110, 126, 3, 19, 35, 51, 67, 83, 99, 115, 7, 23, 39, 55, 71, 87, 103, 
119, 11, 27, 43, 59, 75, 91, 107, 123, 15, 31, 47, 63, 79, 95, 111, 127};
*/

/*
const uint32_t raw_data_to_channel[NUM_MAX_CHAN_PER_APV] = {0, 32, 64, 96, 8, 
40, 72, 104, 16, 48, 80, 112, 24, 56, 88, 120, 1, 
33, 65, 97, 9, 41, 73, 105, 17, 49, 81, 113, 25, 57, 89, 121, 2, 34, 
66, 98, 10, 42, 74, 106, 18, 50, 82, 114, 26, 58, 90, 122, 3, 35, 67, 
99, 11, 43, 75, 107, 19, 51, 83, 115, 27, 59, 91, 123, 4, 36, 68, 
100, 12, 44, 76, 108, 20, 52, 84, 116, 28, 60, 92, 124, 5, 37, 69, 
101, 13, 45, 77, 109, 21, 53, 85, 117, 29, 61, 93, 125, 6, 38, 70, 
102, 14, 46, 78, 110, 22, 54, 86, 118, 30, 62, 94, 126, 7, 39, 71, 
103, 15, 47, 79, 111, 23, 55, 87, 119, 31, 63, 95, 127};
*/
class gem{

    public:
        uint32_t id;
        const char* name;
        std::vector<apv*> apv_list;
        std::vector<gem_cluster> gem_clusters;

        //Debug
        std::vector<apv_peak_info> x_axis_peaks;
        std::vector<apv_peak_info> y_axis_peaks;
    
        uint32_t* x_apv_1_data;
        uint32_t* x_apv_2_data;
        uint32_t* y_apv_1_data;
        uint32_t* y_apv_2_data;
        uint32_t x_data[NUM_CHAN_PER_AXIS];
        uint32_t y_data[NUM_CHAN_PER_AXIS];
/*
        uint32_t* x_apv_1_cmode;
        uint32_t* x_apv_2_cmode;
        uint32_t* y_apv_1_cmode;
        uint32_t* y_apv_2_cmode;
*/
        float x_apv_1_cmode;
        float x_apv_2_cmode;
        float y_apv_1_cmode;
        float y_apv_2_cmode;

        gem(uint32_t id);
        ~gem();

        int addApv(Axis axis, uint32_t apv_id, uint32_t chan_to_skip);

        uint32_t* getDataPointerForChannel(uint32_t apv_channel, 
            Axis axis);
        uint32_t* getDataPointerForAllChannel(uint32_t apv_channel, 
            Axis axis);

        uint32_t getCmodeForChannel(uint32_t apv_channel,
            Axis axis);
    
        void clearGemData();
        void setGemAxisData();
        void findPeaks(std::vector<apv_peak_info>& peaks, Axis axis);
        void findClusters(int event_id);

        double crossAverage(uint32_t x_chan, uint32_t y_chan);
        double averageCharge(int x_charge, int y_charge);

        bool validateGem();
        static bool comparePeaks(apv_peak_info p1, apv_peak_info p2);
         
        // debuggging tools
        void printApvList();
        void dumpGemInfo();

    private:

};

#endif // __GEM_H__
