/* 
 * Declarations of the apv class and methods
 * Noah Wuerfel nwuerfel@umich.edu 10-12-18 goblu
 * ~AP AP AP AP~
 */

#ifndef __APV_H__
#define __APV_H__

#include "gemTypes.h"
#include <bitset>


class apv{

    public:
        uint32_t id;
        uint32_t num_channels=NUM_MAX_CHAN_PER_APV;
        uint32_t num_chan_to_skip=0;
        float cmode;

        Axis axis;
        apv_peak_info peak_info;

        // array decay 
        uint32_t* apv_channel_data;

        apv(Axis axis, uint32_t id, uint32_t chan_to_skip);
        ~apv();

        void clearApvData(bool frequent=false);
        void loadApvData(uint32_t* raw_data);

        // debugging tools
        void dumpApvInfo();
        void dumpApvData();

    private:
};

#endif /* __APV_H__ */
