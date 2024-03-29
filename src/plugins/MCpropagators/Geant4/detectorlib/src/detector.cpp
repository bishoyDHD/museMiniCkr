
#include "cookerDetector.h"
#include "Magnetic_Field.h"
#include "Plugin.h"
#include "G4GDMLParser.hh"
#include "G4FieldManager.hh"
#include "G4SDManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"
#include "base_SD.h"


cookerDetector::cookerDetector(char * filename, Plugin * plg) 
{
  gdmlfilename=filename;
  plugin =plg;
}


cookerDetector::~cookerDetector()
{}


G4VPhysicalVolume* cookerDetector::Construct() 
{
  G4GDMLParser parser;
  parser.Read(gdmlfilename);
  
  // Retrieve the world volume from GDML processor
  G4VPhysicalVolume *  fWorld = parser.GetWorldVolume();

  // Check if the world volume seems to be fine
 if( fWorld == 0 ) 
   {
     plugin->debug(0,"Could not construct gdml world for propagation\n");
     exit(-1);
   }


 //Init magnetic field
 char fnamebuffer[1024];
 snprintf( fnamebuffer, 1020,
	   "%s/.olympus/shared/MC/OLYMPUS.grid.bin", getenv( "COOKERHOME" ) );
 pMF = new Magnetic_Field( fnamebuffer, 1.0,1);
 
 G4FieldManager * FieldMgr =
   G4TransportationManager::GetTransportationManager()->GetFieldManager();
 FieldMgr->SetDetectorField( pMF );
 FieldMgr->CreateChordFinder( pMF );

 // Adding the sensitive detectors
 //G4cout << "\nAdding Sensitive Detectors ...\n" << G4endl;
 G4SDManager* SDman = G4SDManager::GetSDMpointer();
 

  
 
 
 const G4GDMLAuxMapType* auxmap = parser.GetAuxMap();
 
 for(G4GDMLAuxMapType::const_iterator iter=auxmap->begin();
     iter!=auxmap->end(); iter++) 
   {
     
     for (G4GDMLAuxListType::const_iterator vit=iter->second.begin();
          vit!=iter->second.end();vit++)
     {
       if (vit->type=="SensDet")
         if(G4VSensitiveDetector* mydet = SDman->FindSensitiveDetector(vit->value)) 
           iter->first->SetSensitiveDetector(mydet);
         else
	   {
	     // this could be folded into the xml, but allows overloading of a specific class.
	   if (vit->value=="OLYMPUS/TF")
	       SDman->AddNewDetector( new base_SD(vit->value,"MC_ToF",plugin));
	   else
	     if (vit->value=="OLYMPUS/WC")
	       SDman->AddNewDetector( new base_SD(vit->value,"MC_WireChamber",plugin));
	     else
	       if (vit->value=="OLYMPUS/WC_Cell")
		 SDman->AddNewDetector( new base_SD(vit->value,"MC_WireChamberCell",plugin));
	       else
		 if (vit->value=="OLYMPUS/SM")
		   SDman->AddNewDetector( new base_SD(vit->value,"MC_SymmetricMoeller",plugin));
		 else
		   if (vit->value=="OLYMPUS/SI")
		     SDman->AddNewDetector( new base_SD(vit->value,"MC_SiPM",plugin));
		   else
		   if (vit->value=="OLYMPUS/MW")
		     SDman->AddNewDetector( new base_SD(vit->value,"MC_MWPC",plugin));
		   else
		     if (vit->value=="OLYMPUS/LM")
		       SDman->AddNewDetector( new base_SD(vit->value,"MC_LumiGem",plugin));
		     else
		       if (vit->value=="OLYMPUS/LC")
			 SDman->AddNewDetector( new base_SD(vit->value,"MC_LeadGlass",plugin));
		       else
			 if (vit->value=="OLYMPUS/GT")
			   SDman->AddNewDetector( new base_SD(vit->value,"MC_GemTracker",plugin));
			 else
			   {
			     plugin->debug(0,"Can not resolve SD %s\n",vit->value.c_str());
			     exit(-1);
			   }	     
	   iter->first->SetSensitiveDetector(    SDman->FindSensitiveDetector(vit->value));
	   }
       

       if (vit->type=="CopyNo")
	 {
	   //we have to construct the physical name from the logical name:
	   G4String n=iter->first->GetName();
	   n.replace(n.index("log"),3,"phys");
	   std::cout<<iter->first->GetName()<<" "<<n<<std::endl;
	   if (G4VPhysicalVolume *p=G4PhysicalVolumeStore::GetInstance()->GetVolume(n,true))
	     p->SetCopyNo(atoi(vit->value));
	 }
       if (vit->type=="Color")
       {
	 G4String trrrgggbbb(vit->value);
	 char colorinfo[15];
	 strcpy(colorinfo, trrrgggbbb.c_str());

	 if(colorinfo[9] && !colorinfo[10])
	   {
	     G4bool visible;
	     if(colorinfo[0] == 'f') visible = false;
	     else if(colorinfo[0] == 't') visible = true;
	     else plugin->debug(0,"Colorinfo does not have visibility flag\n");
	     char red[3] = {colorinfo[1], colorinfo[2], colorinfo[3]};
	     G4double redvalue = atof(red) / 1000.0;
	     char green[3] = {colorinfo[4], colorinfo[5], colorinfo[6]};
	     G4double greenvalue = atof(green) / 1000.0;
	     char blue[3] = {colorinfo[7], colorinfo[8], colorinfo[9]};
	     G4double bluevalue = atof(blue) / 1000.0;
	     iter->first->SetVisAttributes(G4VisAttributes(visible, G4Color(redvalue, greenvalue, bluevalue)));
	   }
         else
	   plugin->debug(0,"%s improper format for color info\n",vit->value.c_str());
       }

     }
   }

 return fWorld;
}
