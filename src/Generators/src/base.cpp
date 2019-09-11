#include "base.h"


#include "gsl/gsl_rng.h"
#include "gsl/gsl_qrng.h"

#include "CLHEP/Units/PhysicalConstants.h"

struct rng:public gsl_rng{};
struct qrng:public gsl_qrng{};


typedef struct
{
  unsigned int sequence_count;
}
halton_state_t;


GeneratorBase::GeneratorBase(int num_halton_dim, unsigned int skip,unsigned long int seed)
    {
      nHaltonDim=num_halton_dim; // the number of non used dimensions. We will use the remaining ones for the vertex
      setBeamOrigin(TVector3(0.,0.,0.));
      setBeamDirection(TVector3(0.,0.,1.));
      setBeamEnergy(2010.);

      //quasi random numbers
      quasiRandom=(qrng *) gsl_qrng_alloc(gsl_qrng_halton,nHaltonDim+1); //so far, we need one dimension for the vertex;
      qrndNumbers=new double[nHaltonDim+1];
      // advance state by skipstruct rng:public gsl_rng;

      ((halton_state_t *) gsl_qrng_state(quasiRandom))->sequence_count=skip; // we need this so we can split the simulation of a single run.

      //pseudo random
      random=(rng *) gsl_rng_alloc(gsl_rng_mt19937);
      gsl_rng_set(random,seed);

      useQuasi=true;
    }

GeneratorBase::~GeneratorBase()
{
  delete[] qrndNumbers;
  gsl_qrng_free(quasiRandom);
  gsl_rng_free(random);
}

void GeneratorBase::useQuasiRandom(bool uqr)
{
  useQuasi=uqr;
}

double GeneratorBase::getRandom()
{
  return gsl_rng_uniform(random);
}

int GeneratorBase::generate(GeneratorEvent * eventinfo)
{
  if (useQuasi) //this is the normal mode
    gsl_qrng_get(quasiRandom,qrndNumbers); //get new quasi random values
  else
    for( int i=0;i<nHaltonDim+1;i++)
      qrndNumbers[i]=gsl_rng_uniform(random);

  eventinfo->weight=1; //start with weight=1
  eventinfo->particles.clear(); 

  generateVertex(eventinfo);

  int value = generateEvent(eventinfo);
  handleBeamGeometry(eventinfo);
  return value;
}


int GeneratorBase::generateEvent(GeneratorEvent *) //do nothing, has to be overloaded.
{
  printf("Called Base class generateEvent - no particles generated...\n");
  return 0;

}


void GeneratorBase::setBeamEnergy(double en)
{
  beamEnergy=en;
}

void GeneratorBase::setBeamCharge(int q)
{
  beamCharge = q;
}

void GeneratorBase::setBeamOrigin(TVector3 origin)
{
  beamCenter = origin;
}

void GeneratorBase::setBeamDirection(TVector3 dir)
{
  beamDirection=dir.Unit();
  TVector3 zAxis(0.,0.,1.);
  TVector3 rotationAxis = dir.Cross(zAxis);
  beamRotation.SetToIdentity();
  beamRotation.Rotate(dir.Theta(),rotationAxis);
}

void GeneratorBase::setTargetDensity(double maxdens)
{
  targetDensity=maxdens;
}


void GeneratorBase::generateVertex(GeneratorEvent *ev)
{
  double rnd=qrndNumbers[nHaltonDim]; //get the first unused halton number
  int side=1;
  if (rnd>=0.5) 
    {
      rnd-=0.5;
      side=-1;
    }
  double pos=300*side*(1-sqrt(2*rnd)); //this is a triangular shaped vertex position with an amplitude of +-300;

  ev->vertex=beamCenter+pos*beamDirection;
  ev->lepton_prescatter.particle=(beamCharge==-1) ? "e-" : "e+";
  ev->lepton_prescatter.momentum = TLorentzVector(0.,0.,sqrt(beamEnergy*beamEnergy - CLHEP::electron_mass_c2*CLHEP::electron_mass_c2), beamEnergy);
 }

void GeneratorBase::handleBeamGeometry(GeneratorEvent *ev)
{
  // first, rotate the beam axis.
  ev->lepton_prescatter.momentum.Transform(beamRotation);
  
  // loop over final state
  for (std::vector<GeneratorParticle>::iterator it=ev->particles.begin() ;
       it != ev->particles.end() ;
       ++it)
      it->momentum.Transform(beamRotation);
}

//static!
double GeneratorBase::getMass(std::string particle)
{
  if (particle=="e+" || particle=="e-")
    return  CLHEP::electron_mass_c2/CLHEP::MeV;
  
  if (particle=="proton" )
    return  CLHEP::proton_mass_c2/CLHEP::MeV;
  
  if (particle=="neutron" )
    return  CLHEP::neutron_mass_c2/CLHEP::MeV;

  
  return 0;
}
