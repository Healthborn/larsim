////////////////////////////////////////////////////////////////////////
/// \file AuxDetReadoutGeometry.h
/// \brief Define the "parallel" geometry that's seen by the AuxDet.
/// \author miceli@fnal.gov
////////////////////////////////////////////////////////////////////////
///
/// This class defines the parallel geometry that will be divided into
/// the three-dimensional voxels for the detector read-out.
/// 
/// Why define a parallel geometry?  Here are some reasons:
///
/// - The regular LAr TPC is one large volume of liquid argon.  When
///   Geant4 does its physics modeling, it can be unconstrained in
///   step size by the voxels.  Only for readout would the steps be
///   sub-divided.
///
/// - There may be more than one kind of readout, depending on a
///   detector's instrumentation (e.g., OpDets in addition to the wire
///   planes).  It's possible that the voxelization appropriate for
///   the wire planes may not be an appropriate readout for the other
///   readouts.  Geant4 allows the construction of multiple parallel
///   readouts, so this mechanism is relatively easy to extend for
///   each type of readout.

#ifndef LArG4_AuxDetReadoutGeometry_h
#define LArG4_AuxDetReadoutGeometry_h

#include "Geometry/Geometry.h"
#include "Geant4/G4VUserParallelWorld.hh"
#include "Geant4/G4String.hh"
#include "Geant4/G4Transform3D.hh"
#include "Geant4/G4VPhysicalVolume.hh"

namespace larg4 {

  class AuxDetReadoutGeometry : public G4VUserParallelWorld
  {
  public:
    /// Constructor and destructor.
    AuxDetReadoutGeometry( const G4String name = "AuxDetReadoutGeometry" );
    virtual ~AuxDetReadoutGeometry();

    /// The key method in this class; creates a parallel world view of
    /// those volumes relevant to the auxiliary detector readout.
    /// Required of  any class that inherits from G4VUserParallelWorld
    virtual void Construct();

  private:

    G4VPhysicalVolume* FindNestedVolume(G4VPhysicalVolume* mother,
					                              G4Transform3D&     motherTransform,
					                              G4Transform3D&     daughterTransform,
					                              std::string&       daughterName,
					                              unsigned int       expectedNum);

    art::ServiceHandle<geo::Geometry>   fGeo; ///< Handle to the geometry    

  };

} // namespace larg4

#endif // LArG4_AuxDetReadoutGeometry_h