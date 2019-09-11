#ifndef __COOKER_DETECTOR__
#define __COOKER_DETECTOR__

#include "G4String.hh"
#include "G4VUserDetectorConstruction.hh"

class Plugin;
class G4VPhysicalVolume;
class Magnetic_Field;

class cookerDetector : public G4VUserDetectorConstruction
{

 private:
  G4String gdmlfilename;
  Plugin * plugin;
  Magnetic_Field *pMF;

public:
  cookerDetector(char *,Plugin *);
  ~cookerDetector();
  
public:
  G4VPhysicalVolume* Construct();     //The queen of methods one has to implement
  
};

#endif
