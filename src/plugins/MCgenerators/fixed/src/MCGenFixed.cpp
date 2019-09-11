#include <MCGenFixed.h>

#include<iostream>
#include<cmath>


MCGenFixed::MCGenFixed(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p):MCGenPlugin(in,out,inf_,outf_,p)
{
  gen=new GeneratorFixed(getSkip(),getSeed());
  generator = gen;
  gen->setParticle("e-");
  gen->setMomentum(2000.);// 2GeV
  gen->setTheta(45.);
  gen->setPhi(0.);
};

MCGenFixed::~MCGenFixed()
{
  if (gen) delete gen;
};


Long_t MCGenFixed::cmdline(char *cmd)
{
  //add cmdline hanling here

  return 0; // 0 = all ok
};


extern "C"{
Plugin *factory(TTree *in, TTree *out,TFile *inf_, TFile * outf_, TObject *p)
{
  return (Plugin *) new MCGenFixed(in,out,inf_,outf_,p);
}
}

Long_t MCGenFixed::genStartup()
{
  return 0;
}

Long_t MCGenFixed::genProcess()
{
  gen->generate(ge);
  return Plugin::ok;
}

Long_t MCGenFixed::genFinalize()
{
  return 0;
}

Long_t MCGenFixed::setParticle(std::string p)
{
  gen->setParticle(p);
  return 0;
}

Long_t MCGenFixed::setTheta(double theta)
{
  gen->setTheta(theta);
  return 0;
}

Long_t MCGenFixed::setPhi(double phi)
{
  gen->setPhi(phi);
  return 0;
}

Long_t MCGenFixed::setMomentum(double mom)
{
  gen->setMomentum(mom);
  return 0;
}

ClassImp(MCGenFixed);

