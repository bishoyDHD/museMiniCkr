#ifndef __GEMCONTROLTREE_H__
#define __GEMCONTROLTREE_H__
/* This file contains declarations of gemControl tree objects
 *
 */

#include "muserawtree.h"

class gemControltree : public MRTBase{
    public:
        gemControltree();
        virtual ~gemControltree();
        ClassDef(gemControltree,1);
        // ADDING DESTRUCTOR HERE FUCKS UP COMPILATION
};


#endif // __GEMCONTROLTREE_H__
