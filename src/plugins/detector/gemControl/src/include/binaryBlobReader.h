/* 
 * Declarations of the binaryBlobReader class and methods
 * Noah Wuerfel nwuerfel@umich.edu 10-12-18 goblu
 * ~AP AP AP AP~
 */
#ifndef __BINARYBLOBREADER_H__
#define __BINARYBLOBREADER_H__

#include "gemControltree.h"
#include "gemTypes.h"

#include <cstddef>

class binaryBlobReader{

    private:

    public:
        binaryBlobReader();
        ~binaryBlobReader();
        apvRawData rawDataBlob;
        size_t  readBank(MRTBinaryBlob* binBlobHook);
        uint32_t* rawApvData(int apv_num);
};

#endif /*__BINARYBLOBREADER_H__*/
