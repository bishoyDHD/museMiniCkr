/* 
 * Definitions of the binaryBlobReader class and methods
 * Noah Wuerfel nwuerfel@umich.edu 10-12-18 goblu
 * ~AP AP AP AP~
 */

#include "binaryBlobReader.h"
#include "gemControltree.h"
#include "gemTypes.h"

#include <iostream>

binaryBlobReader::binaryBlobReader(){
}

binaryBlobReader::~binaryBlobReader(){
}

/* 
 * binBlob structure:
 * empty header word
 * 128 apv data words
 * empty trailer word
 */

size_t binaryBlobReader::readBank(MRTBinaryBlob* binBlobHook){

    assert(binBlobHook != NULL);

    size_t size=(binBlobHook->size)/sizeof(uint32_t);

    if (size==0){
        return 0;
    }

    uint32_t* dataptr = (uint32_t*)binBlobHook->data;

    // then what is the point?
    // this isn't from the raw data? or is it and it just got fumbled?
    rawDataBlob.event_id = 0;
    rawDataBlob.num_apvs = NUM_APV_PER_GEM*NUM_GEMS;

    uint32_t wordcnt=0;

    for (uint32_t i=0; i<rawDataBlob.num_apvs; i++){
        
        assert(dataptr!=NULL);
        rawDataBlob.apv_header[i] = *dataptr;
        dataptr++;


        // TODO wtf happens when the apv has a dead channel?
        // TODO need to make sure this maps to the right apvs
        for (int j=0; j<NUM_MAX_CHAN_PER_APV; j++){
            assert(dataptr!=NULL);
            rawDataBlob.apv_data[i][j] = *dataptr;
            dataptr++;
            wordcnt++;
        };

        // skip trailer:
        // then why even have one?
        // shouldn't it be a canary or something useful to check
        // datastream?
        dataptr++;
    };
    return wordcnt;
}

uint32_t* binaryBlobReader::rawApvData(int apv_num){
    return rawDataBlob.apv_data[apv_num];
}
