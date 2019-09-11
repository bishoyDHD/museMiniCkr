
#include "MCGenPlugin.h"

#include "generators.h"
#include "TH1.h"
#include "TH2.h"

MCGenPlugin::MCGenPlugin(TTree *in_,TTree *out_,TFile *inf_, TFile * outf_,TObject *p_):Plugin(in_,out_,inf_,outf_,p_),useQuasi(true),useBeamGeometrySlowCtrl(true)
{
  numDataEvents=in->GetEntries();
  numSimulEvents=numDataEvents;
}

Long_t MCGenPlugin::startup()
{
  if (!generator)
    {
      debug(0,"Generator not created! Bailing out!\n");
      exit(-1);
    }
  generator->useQuasiRandom(useQuasi);
  eventcounter=0;
  ge=new GeneratorEvent();
  makeBranch("Generator",(TObject ** ) &ge);

  // Define some histograms
  vertexHist=dH1("MC/Gen/vertexZ","Z position of vertex;Z (mm);Expected Counts",1000,-500.,500.);
  beamProfileHist=dH2("MC/Gen/vertexXY","XY Profile of vertex;X (mm);Y (mm);Expected Counts",100,-5.,5.,100,-5.,5.);
  beamSlopeHist=dH2("MC/Gen/beamSlope","Beam Slope;X Slope;Y Slope;Expected Counts",100,-5.E-2,5.E-2,100,-5.E-2,5.E-2);

  return  genStartup();
}

Long_t MCGenPlugin::setEventCount(unsigned int c)
{

  numSimulEvents=c;
  return 0;
}

Long_t MCGenPlugin::useQuasiRandom(bool q)
{
  useQuasi=q;
  return 0;
}

Long_t MCGenPlugin::process()
{
  //for now, give every data event the same number of simulation events;
  debug(1000,"Eventcount is at %i, numData is %i,num Simul is %i\n",eventcounter,numDataEvents,numSimulEvents);
  if (eventcounter>= 0)    //eventcounter will be incremented/decremented by the event counts. This is essentially the Bresenham algorithm. yeah, oldskool.
    {
    //we simulate
      if (int r=genProcess()<0) return r;
      setWeight(ge->weight);
      // Fill some histograms
      vertexHist->Fill(ge->vertex.Z());
      beamProfileHist->Fill(ge->vertex.X(),ge->vertex.Y());
      beamSlopeHist->Fill(ge->lepton_prescatter.momentum.X()/ge->lepton_prescatter.momentum.Z(),
			  ge->lepton_prescatter.momentum.Y()/ge->lepton_prescatter.momentum.Z());
      // Back to loop logic
      eventcounter-=numDataEvents;   
      if (eventcounter>=0) //still do another?
	return Plugin::redo;
      else
	{
	  eventcounter+=numSimulEvents;
	  return Plugin::ok;
	}
    }
  eventcounter+=numSimulEvents;
  return Plugin::skip; //do not write output

  // todo: fill sensible weight
}


Long_t MCGenPlugin::finalize()
{
 Long_t r= genFinalize();

 delete ge;
 return r;
}

Long_t MCGenPlugin::setBeamOffset(double mmX, double mmY, double mmZ)
{
  generator->setBeamOrigin(TVector3(mmX,mmY,mmZ));
  return 0;
}

Long_t MCGenPlugin::setBeamSlope(double xSlope, double ySlope)
{
  generator->setBeamDirection(TVector3(xSlope,ySlope,1.));
  return 0;
}

Long_t MCGenPlugin::useBeamGeoSC(bool trueOrFalse)
{
  useBeamGeometrySlowCtrl = trueOrFalse;
  return 0;
}

unsigned int MCGenPlugin::getSkip()
{
  return 0;
}

unsigned long int MCGenPlugin::getSeed()
{
  return 0;
}

ClassImp(MCGenPlugin);
