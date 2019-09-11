#ifndef __GENERATOR_TREE_
#define __GENERATOR_TREE_

#include "muserawtree.h"

#include "TVector3.h"
#include "TLorentzVector.h"
#include <string>
#include <vector>

class GeneratorParticle:public MRTBase
{
 public:
  TLorentzVector momentum;
  std::string particle;
  GeneratorParticle();
  ~GeneratorParticle();

  ClassDef(GeneratorParticle,1);
};

class GeneratorEvent:public MRTBase
{
 public:
  TVector3 vertex; //the Vertex 
  double weight; // the weight (should ALSO be saved into EventInfo)
  GeneratorParticle lepton_prescatter; //the simulated particle momentum pre scatter (with external energy loss, WITHOUT internal energy loss)
  std::vector<GeneratorParticle> particles;
  GeneratorEvent();
  ~GeneratorEvent();
  ClassDef(GeneratorEvent,1);
};

#endif
