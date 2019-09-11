//! \file
//!
//! Header file for Magnetic_Field class.
//!
//! This defines the Magnetic_Field class and the member routines which
//! construct the OLYMPUS toroidal magnetic field.
//!
//! \author D.K. Hasell
//! \version 1.0
//! \date 2010-02-27
//!
//! \ingroup detector

// Ensure this header file is included only once.

#ifndef MAGNETIC_FIELD_H
#define MAGNETIC_FIELD_H

// Include the GEANT4 header files referenced here.

#ifndef __MAKECINT__
#include "G4MagneticField.hh"
#else
class G4MagneticField;
class G4double;
class G4int;
#endif

// Include the STL header files referenced here.

#include <vector>

// Use the STD namespace.

using namespace std;

//! Declare the Magnetic_Field class.
//!
//! The Magnetic_Field class stores the grid of magnetic field values in an
//! array of stl vectors.  The routine reads a file with the magnetic field
//! grid values and loads these into the arrays. The member function
//! GetMagneticField is then used to return the field at a given point.

class Magnetic_Field : public G4MagneticField {

private:
  /*
    vector< vector< vector< G4double > > > fBx; //!< X component of field.
    vector< vector< vector< G4double > > > fBy; //!< Y component of field.
    vector< vector< vector< G4double > > > fBz; //!< Z component of field.
  */

     // sane allocation

     G4double *pfBx, *pfBy, *pfBz;

     G4int fNdata;            //!< Number of grid points.
     G4int fNx;               //!< Number of grid steps in X direction.
     G4int fNy;               //!< Number of grid steps in Y direction.
     G4int fNz;               //!< Number of grid steps in Z direction.
     G4int fNxy;              //!< Accelerate index lookup.

     G4double fXmin;          //!< Starting X coordinate for grid.
     G4double fYmin;          //!< Starting Y coordinate for grid.
     G4double fZmin;          //!< Starting Z coordinate for grid.

     G4double fDx;            //!< Step size in X direction.
     G4double fDy;            //!< Step size in Y direction.
     G4double fDz;            //!< Step size in Z direction.


     G4double fiDx;            //!< inv. Step size in X direction.
     G4double fiDy;            //!< inv. Step size in Y direction.
     G4double fiDz;            //!< inv. Step size in Z direction.

     G4double fScale;         //!< Scale factor for calculating the field.

public:

     //! Constructor for Magnetic_Field class.
     //!
     //! Reads the file specified by \a filename for the magnetic field value
     //! grid and stores this into the arrays.  Also stores the scale factor
     //! \a scale used in scaling the magnetic field reurned by the member
     //! function.
     //!
     //! \param[in] filename - name of file containing the magnetic field grid
     //! \param[in] scale - multiplicative factor used to scale the calculated
     //! magnetic field returned by the member function GetFieldValue.

     Magnetic_Field( const char * filename, G4double scale = 1.0,int type=0);
     ~Magnetic_Field();
     //! Member function to return the magnetic field value at a given point.
     //!
     //! Performs a simple linear interpolation over the grid of stored values
     //! which bracket the requested space point \a point and returns the 
     //! magnetic field \a Bfield at that point.
     //!
     //! \param[in] Point - array of x, y, z, t coordinates specifying the 
     //! coordinates where the field should be evaluated (time coordinate
     //! ignored in this code).
     //! \param[out] Bfield - pointer to a six dimensional array giving the
     //! magnetic and electric field components at \a Point 

     void GetFieldValue( const G4double Point[4], G4double * Bfield ) const;

     //! Member function to set the magnetic field scaling factor.
     //!
     //! The interpolated magnetic field is scaled, multiplicatively, by
     //! this factor to determine the actual field.  Typically this is set
     //! to 1.0 for normal operation but can be set to any value in the
     //! range [-1.0, 1.0] for reasonable operation.
     //!
     //! \param[in] scale - scale factor should be in range [-1, 1]

     G4double setScale( G4double scale = 1.0 );

     //! Member function to get the magnetic field scaling factor.
     //!
     //! Returns the current magnetic field scale factor.

     G4double getScale();
     void save(const char *filename);
};

#endif
