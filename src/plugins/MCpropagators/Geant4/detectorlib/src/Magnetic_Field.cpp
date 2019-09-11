//! \file
//!
//! Source file for Magnetic_Field class.
//!
//! This defines the Magnetic_Field class and the member routines which
//! construct the OLYMPUS toroidal magnetic field.
//!
//! \author D.K. Hasell
//! \version 1.0
//! \date 2010-10-14
//!
//! \ingroup detector

// Include the Magnetic_Field and other user header files.

#include "Magnetic_Field.h"


// Include the GEANT4 header files referenced here.

#include "globals.hh"
#include "G4ios.hh"

// Include the C++ header files referenced here.

#include <fstream>
#include <iostream>
#include <stdio.h>
#include "printfcolors.h"

// Constructor for Magnetic_Field class.

Magnetic_Field::Magnetic_Field( const char * filename, G4double scale,int type)
  : G4MagneticField(), fScale( scale ) {


  G4cout << "     reading field grid\n" << flush;


  if (type==0)
    {
      // Open file with grid data.

      ifstream grid( filename );

      // Check if file has been opened successfully:

      if (!grid.is_open())
	{
	  G4cerr << ERROR_MSG << "Cannot open magnetic field grid file >"
		 << filename  << "<, exiting..." << NORMAL << G4endl;
	  exit(1);
	};

      // Read the number of steps, starting value, and step size for grid.

      grid >> fNdata >> fNx >> fNy >> fNz
	   >> fXmin >> fYmin >> fZmin
	   >> fDx >> fDy >> fDz;

      // Convert to proper units.

      fXmin *= millimeter;
      fYmin *= millimeter;
      fZmin *= millimeter;

      fDx *= millimeter;
      fDy *= millimeter;
      fDz *= millimeter;

      fiDx =1/fDx;
      fiDy =1/fDy;
      fiDz =1/fDz;


      pfBx=new G4double[fNx*fNy*fNz];
      pfBy=new G4double[fNx*fNy*fNz];
      pfBz=new G4double[fNx*fNy*fNz];
  
      fNxy=fNx*fNy;
      /*
      // Resize the vectors which store the grid data.

      fBx.resize( fNx );
      fBy.resize( fNx );
      fBz.resize( fNx );

      for( G4int ix = 0; ix < fNx; ++ix ){

      fBx[ix].resize( fNy );
      fBy[ix].resize( fNy );
      fBz[ix].resize( fNy );

      for( G4int iy = 0; iy < fNy; ++iy ) {

      fBx[ix][iy].resize( fNz );
      fBy[ix][iy].resize( fNz );
      fBz[ix][iy].resize( fNz );

      }

      }
      */
      // Read field data and fill vectors with field components in proper units.

      double bx, by, bz;

      for( G4int iz = 0; iz < fNz; ++iz ) {
         for( G4int iy = 0; iy < fNy; ++iy ) {
            for( G4int ix = 0; ix < fNx; ++ix ) {

               grid >> bx >> by >> bz;
	    /*
	      fBx[ix][iy][iz] = bx * gauss;
	      fBy[ix][iy][iz] = by * gauss;
	      fBz[ix][iy][iz] = bz * gauss;
	    */
	    pfBx[ix+fNx*iy+fNxy*iz]=bx*gauss; //other layouts might be better cache aligned....
	    pfBy[ix+fNx*iy+fNxy*iz]=by*gauss;
	    pfBz[ix+fNx*iy+fNxy*iz]=bz*gauss;

	  }
	}
	if (iz%(fNz/50)==0) G4cout << "." << flush;
      }
      G4cout << endl;

      // Close data file.
      grid.close();
    }

  else
    {

      ifstream grid( filename ,ifstream::binary);

      // Check if file has been opened successfully:

      if (!grid.is_open())
	{
	  G4cerr << ERROR_MSG << "Cannot open magnetic field grid file >"
		 << filename  << "<, exiting..." << NORMAL << G4endl;
	  exit(1);
	};

      grid.read((char *)&fNdata,sizeof(fNdata));
      grid.read((char *)&fNx,sizeof(fNx));
      grid.read((char *)&fNy,sizeof(fNy));
      grid.read((char *)&fNz,sizeof(fNz));
      grid.read((char *)&fXmin,sizeof(fXmin));
      grid.read((char *)&fYmin,sizeof(fYmin));
      grid.read((char *)&fZmin,sizeof(fZmin));
      grid.read((char *)&fDx,sizeof(fDx));
      grid.read((char *)&fDy,sizeof(fDy));
      grid.read((char *)&fDz,sizeof(fDz));
      pfBx=new G4double[fNx*fNy*fNz];
      pfBy=new G4double[fNx*fNy*fNz];
      pfBz=new G4double[fNx*fNy*fNz];
      fNxy=fNx*fNy;
      grid.read((char *)pfBx,sizeof(G4double)*fNxy*fNz);
      grid.read((char *)pfBy,sizeof(G4double)*fNxy*fNz);
      grid.read((char *)pfBz,sizeof(G4double)*fNxy*fNz);
      grid.close();
      fiDx =1/fDx;
      fiDy =1/fDy;
      fiDz =1/fDz;

    }

  //
  /*
  for (int u=0;u<100;u++)
  for (int v=0;v<100;v++)
    {
      G4double p[4];
      G4double f[6];
      p[0]=-2000+u*40;
      p[1]=-2000+v*40;
      p[2]=0;
      GetFieldValue(p,f);
      //printf("GT %g %g 0 %g %g %g\n",p[0],p[1],f[0],f[1],f[2]);
    }

  //printf("GT \n\n");
  for (int u=0;u<100;u++)
  for (int v=0;v<100;v++)
    {
      G4double p[4];
      G4double f[6];
      p[1]=0;
      p[2]=-2000+u*40;
      p[0]=-2000+v*40;
      GetFieldValue(p,f);
      //printf("GT %g 0 %g  %g %g %g\n",p[0],p[2],f[0],f[1],f[2]);
    }
  */



}


Magnetic_Field::~Magnetic_Field() {
  delete [] pfBx;
  delete [] pfBy;
  delete [] pfBz;
}

// Member function to return the magnetic field value at a given point.

void Magnetic_Field::GetFieldValue( const G4double Point[4],
                                    G4double * Bfield ) const {

  G4double x = Point[0];
  G4double y = Point[1];
  G4double z = Point[2];

  // Find grid cubic containing the point.
  /*
    G4int ix0 = ( x - fXmin ) / fDx;
    G4int iy0 = ( y - fYmin ) / fDy;
    G4int iz0 = ( z - fZmin ) / fDz;

   

    G4int ix1 = ix0 + 1;
    G4int iy1 = iy0 + 1;
    G4int iz1 = iz0 + 1;


  */
  // Declare fractions of interval.


  G4double ffx0, ffx1, ffy0, ffy1, ffz0, ffz1;
  
   
   
  // does not work because modf (-0.5) gives 0, -0.5 instead of -1,0.5
  // ffx0 = modf( (x - fXmin ) * fiDx, &fx0); //fx0 will have integer part, ffx0 fractional part.
  // ffy0 = modf( (y - fYmin ) * fiDy, &fy0);
  // ffz0 = modf( (z - fZmin ) * fiDz, &fz0);
  

  ffx0=(x - fXmin ) * fiDx;
  ffy0=(y - fYmin ) * fiDy;
  ffz0=(z - fZmin ) * fiDz;


  G4int ix0=floor(ffx0);
  G4int iy0=floor(ffy0);
  G4int iz0=floor(ffz0);
  
    

  G4int base=ix0+iy0*fNx+iz0*fNxy; 
   
  // Test that point is within the range of the grid.


  if( ix0 >= 0 && ix0 < fNx-1 &&
			iy0 >= 0 && iy0 < fNy-1 &&
					  iz0 >= 0 && iz0 < fNz-1 ) {


    

	// Calculate the fractions of the interval for the given point.
	/*
	  fx0 = ( x - fXmin - ix0 * fDx ) / fDx;
	  fy0 = ( y - fYmin - iy0 * fDy ) / fDy;
	  fz0 = ( z - fZmin - iz0 * fDz ) / fDz;

	  fx1 = ( fXmin + ix1 * fDx - x ) / fDx;
	  fy1 = ( fYmin + iy1 * fDy - y ) / fDy;
	  fz1 = ( fZmin + iz1 * fDz - z ) / fDz;
	*/
    ffx0-=ix0;
    ffy0-=iy0;
    ffz0-=iz0;
    
    
    ffx1=1-ffx0;
    ffy1=1-ffy0;
    ffz1=1-ffz0;
    
	// Interpolate the magnetic field from the values at the 8 corners.
	/*
	  Bfield[0] = fz1*(fy1*( fx1*fBx[ix0][iy0][iz0]+fx0*fBx[ix1][iy0][iz0] )
	  + fy0*( fx1*fBx[ix0][iy1][iz0]+fx0*fBx[ix1][iy1][iz0] ))
	  + fz0*(fy1*( fx1*fBx[ix0][iy0][iz1]+fx0*fBx[ix1][iy0][iz1] )
	  + fy0*( fx1*fBx[ix0][iy1][iz1]+fx0*fBx[ix1][iy1][iz1] ));
      
	  Bfield[1] = fz1*(fy1*( fx1*fBy[ix0][iy0][iz0]+fx0*fBy[ix1][iy0][iz0] )
	  + fy0*( fx1*fBy[ix0][iy1][iz0]+fx0*fBy[ix1][iy1][iz0] ))
	  + fz0*(fy1*( fx1*fBy[ix0][iy0][iz1]+fx0*fBy[ix1][iy0][iz1] )
	  + fy0*( fx1*fBy[ix0][iy1][iz1]+fx0*fBy[ix1][iy1][iz1] ));

	  Bfield[2] = fz1*(fy1*( fx1*fBz[ix0][iy0][iz0]+fx0*fBz[ix1][iy0][iz0] )
	  + fy0*( fx1*fBz[ix0][iy1][iz0]+fx0*fBz[ix1][iy1][iz0] ))
	  + fz0*(fy1*( fx1*fBz[ix0][iy0][iz1]+fx0*fBz[ix1][iy0][iz1] )
	  + fy0*( fx1*fBz[ix0][iy1][iz1]+fx0*fBz[ix1][iy1][iz1] ));
	*/
     
	double f=ffz1*ffy1;

	double x1,x2,y1,y2,z1,z2;

	x1= f*pfBx[base];    y1= f*pfBy[base];   z1= f*pfBz[base]; 

	base++;
	x2= f*pfBx[base];  y2= f*pfBy[base]; z2= f*pfBz[base];
	f=ffz1*ffy0;
	base+=fNx;
	x2+=f*pfBx[base]; y2+=f*pfBy[base]; z2+=f*pfBz[base];
	base--;
	x1+=f*pfBx[base];   y1+=f*pfBy[base];   z1+=f*pfBz[base];   

	f=ffz0*ffy0;
	base+=fNxy;
	x1+=f*pfBx[base];  y1+=f*pfBy[base]; z1+=f*pfBz[base];
	base++;
	x2+=f*pfBx[base]; y2+=f*pfBy[base]; z2+=f*pfBz[base];
      
	f=ffz0*ffy1;
	base-=fNx;
	x2+=f*pfBx[base];y2+=f*pfBy[base];z2+=f*pfBz[base];
	base--;
	x1+=f*pfBx[base];y1+=f*pfBy[base];z1+=f*pfBz[base];

	Bfield[0]=x1*ffx1+x2*ffx0;
	Bfield[1]=y1*ffx1+y2*ffx0;
	Bfield[2]=z1*ffx1+z2*ffx0;
      
      
  }

  else {
     Bfield[0] = 0.0;
     Bfield[1] = 0.0;
     Bfield[2] = 0.0;
  }

//**+****1****+****2****+****3****+****4****+****5****+****6****+****7****+****
//
// Scale field value  !!!!!!!!!!!!!!!!!!!!!!!!!
//
// NOTE the minus sign in the following three lines of code are needed so the
// magnet polarity in the experiment has the same sign as the scaling factor
// used in the Monte Carlo.  Specifically a positive magnet polarity in the
// OLYMPUS experiment causes a positively charge particle to bend away from the
// beamline.  With this change a positive magnet scaling factor will result in
// positively charged particles bending away from the beamline also in the
// Monte Carlo.
//
//                        - D.K. Hasell 2012.02.16
//
//**+****1****+****2****+****3****+****4****+****5****+****6****+****7****+****

  Bfield[0] *= -fScale;
  Bfield[1] *= -fScale;
  Bfield[2] *= -fScale;
  
  /*
   G4cout << "\nPosition " << x << " " << z << " " << z << G4endl;
   G4cout << "Old field " << Bfield[0] << " " << Bfield[1] << " " << Bfield[2] 
          << G4endl;

   G4double phi = atan2( y, x );
   G4double Bphi = -Bfield[0] * sin( phi ) + Bfield[1] * cos( phi );

   Bfield[0] = -Bphi * sin( phi );
   Bfield[1] =  Bphi * cos( phi );
   Bfield[2] = 0.0;

   G4cout << "Phi " << phi << " " << Bphi << " " << G4endl; 
   G4cout << "New field " << Bfield[0] << " " << Bfield[1] << " " << Bfield[2] 
          << G4endl;

  */
// Get rid of r and z components (only phi component should survive)
	// to get a field with perfect cylindrical symmetry:
        //G4double phi = atan2(y,x);
	//Btot is total mag. field which is B phi
	//G4double Btot = -Bfield[0]*sin(phi)+Bfield[1]*cos(phi);

	//newly found mag. field components
	//G4double bxnew =-Btot*sin(phi);
	//G4double bynew =Btot*cos(phi);
	//G4double bznew =0.00;

	//then I print out the results
	//	printf("before: Bfield[0]: %f Bfield[1]: %f phi: %f Btot: %f bxnew: %f bynew: %f fScale: %f\n", Bfield[0],Bfield[1],phi,Btot,bxnew,bynew,fScale);

	//Bfield[0] = bxnew;
	//Bfield[1] = bynew;
	//Bfield[2] = bznew;
	
//printf("after: Bfield[0]: %f Bfield[1]: %f phi: %f Btot: %f bxnew: %f bynew: %f fScale: %f\n", Bfield[0],Bfield[1],phi,Btot,bxnew,bynew,fScale);

   

	// This is a pure magnetic field so set electric field to zero just in case.

	Bfield[3] = 0.0;
	Bfield[4] = 0.0;
	Bfield[5] = 0.0;

	return;

}

// Set the magnetic field scaling factor.

G4double Magnetic_Field::setScale( G4double scale ) {
	return fScale = scale;
}

// Get the magnetic field scaling factor.

G4double Magnetic_Field::getScale() {
	return fScale;
}


void Magnetic_Field::save(const char *filename) {
   ofstream outfile (filename,ofstream::binary);

   if (!outfile.is_open())	{
      G4cerr << ERROR_MSG << "Could not open output file of magnetic field  >"
             << filename  << "<, exiting..." << NORMAL << G4endl;
      exit(1);
	}

   outfile.write((const char *)&fNdata,sizeof(fNdata));
   outfile.write((const char *)&fNx,sizeof(fNx));
   outfile.write((const char *)&fNy,sizeof(fNy));
   outfile.write((const char *)&fNz,sizeof(fNz));
   outfile.write((const char *)&fXmin,sizeof(fXmin));
   outfile.write((const char *)&fYmin,sizeof(fYmin));
   outfile.write((const char *)&fZmin,sizeof(fZmin));
   outfile.write((const char *)&fDx,sizeof(fDx));
   outfile.write((const char *)&fDy,sizeof(fDy));
   outfile.write((const char *)&fDz,sizeof(fDz));
   outfile.write((const char *)pfBx,sizeof(G4double)*fNxy*fNz);
   outfile.write((const char *)pfBy,sizeof(G4double)*fNxy*fNz);
   outfile.write((const char *)pfBz,sizeof(G4double)*fNxy*fNz);
   outfile.close();
}

