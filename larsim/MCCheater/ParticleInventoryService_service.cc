////////////////////////////////////////////////////////////////////////////
//
// ParticleInventoryService.cc
// Author: JStock
// EMail:  jason.stock@mines.sdsmt.edu
// 2017-09-12
//
// Maintinence Notes: When the ParticleInventory is initialized, none of the prep work (previously 
// the BackTracker rebuild stage) will be done. Each function needs to check and make sure the 
// needed data products have been loaded. To see what objects a function uses, you will have to 
// check the appropriate part of ParticleInventory. After this, you will need to manually write the check
// into whatever function you are writing. You will also want to include a call to prepare the needed items
// if your check fails. 
//
// Example:
// std::set<int> ParticleInventoryService::GetSetOfTrackIds(){
//   if(!this->priv_ParticleListReady()){this->priv_PrepParticleList();} //The GetTrackIds in ParticleInventory needs the ParticleList. 
//                                                                         So, we check if it's ready, and if it isn't we ready it.
//   return ParticleInventory::GetSetOfTrackIds();
// }
//
// If you have any questions about how to incorperate something in here, let me know. I know this is a rather odd
// use model. The rationale is to allow the BackTracker service to be lazy, while at the same time allowing gallery 
// to use backtracker functions (the gallery implimentation is not lazy).
////////////////////////////////////////////////////////////////////////////

//STL includes
#include <map>
//ROOT includes
//Framework includes
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/View.h"

//LArSoft includes
#include "larsim/MCCheater/ParticleInventoryService.h"
#include "nutools/ParticleNavigation/EmEveIdCalculator.h"
#include "nusimdata/SimulationBase/MCParticle.h"
#include "larsim/Simulation/SimListUtils.h"
#include "lardataobj/Simulation/sim.h"
#include "lardata/Utilities/AssociationUtil.h"


namespace cheat{

  //----------------------------------------------------------------------
  ParticleInventoryService::ParticleInventoryService(const ParticleInventoryServiceConfig& config, art::ActivityRegistry& reg)
  :ParticleInventory(config.ParticleInventoryTable())
  {
//    std::cout<<"Config Dump from ParticleInventoryService using fhicl Table\n";
//    config.ParticleInventoryTable.print_allowed_configuration(std::cout);
    reg.sPreProcessEvent.watch(this, &ParticleInventoryService::priv_PrepEvent);
  }

  //----------------------------------------------------------------------
  ParticleInventoryService::ParticleInventoryService(const fhicl::ParameterSet& pSet, art::ActivityRegistry& reg)
  :ParticleInventory(pSet.get<fhicl::ParameterSet>("ParticleInventory"))
  {
//    std::cout<<"\n\n\n\nConfigDump from ParticleInventoryService using ParameterSet.\n"<<pSet.to_string()<<"\n\n\n\n";
    reg.sPreProcessEvent.watch(this, &ParticleInventoryService::priv_PrepEvent);
  }


  //----------------------------------------------------------------------
  void ParticleInventoryService::priv_PrepEvent( const art::Event& evt){
    fEvt=&evt;
    ParticleInventory::ClearEvent();
  }

  //----------------------------------------------------------------------
  bool ParticleInventoryService::priv_CanRun(const art::Event& evt) const{
    return ParticleInventory::CanRun(evt);
  }

  //----------------------------------------------------------------------
  void ParticleInventoryService::priv_PrepParticleList(){
    if(!this->priv_CanRun(*fEvt)) {throw;}
    if(this->priv_ParticleListReady()){ return; }
    ParticleInventory::PrepParticleList(*fEvt);
  }


  void ParticleInventoryService::priv_PrepTrackIdToMCTruthIndex( ){
    if(!this->priv_CanRun(*fEvt)){throw;}
    if( this->priv_TrackIdToMCTruthReady()){ return; }
    ParticleInventory::PrepTrackIdToMCTruthIndex(*fEvt);
  }//End priv_PrepTrackIdToMCTruthIndexList

  void ParticleInventoryService::priv_PrepMCTruthList( ){
    if(!this->priv_CanRun(*fEvt)){throw;}
    if(this->priv_MCTruthListReady( ) ){ return;} //If the event is data or if the truth list is already built there is nothing for us to do.
    ParticleInventory::PrepMCTruthList(*fEvt);
  }//End PrepMCTruthList


  //Loop Event and grab MCTruths. Quick and clean as possible.

  //deliverables
  
  const sim::ParticleList& ParticleInventoryService::ParticleList() { 
    if(!this->priv_ParticleListReady()){this->priv_PrepParticleList();}
    return ParticleInventory::ParticleList(); 
  } //This should be replaced with a public struct so we can get away from the nutools dependency.
  
  const std::vector< art::Ptr<simb::MCTruth> >& ParticleInventoryService::MCTruthVector_Ps() {
    if(!this->priv_MCTruthListReady()){priv_PrepMCTruthList();}
    return ParticleInventory::MCTruthVector_Ps();
  }

  //TrackIdToParticleP

  const simb::MCParticle* ParticleInventoryService::TrackIdToParticle_P(int const& id) {
    if(!this->priv_ParticleListReady()){this->priv_PrepParticleList();}
    return ParticleInventory::TrackIdToParticle_P(id);
  }//End TrackIdToParticle


  const simb::MCParticle* ParticleInventoryService::TrackIdToMotherParticle_P(int const& id) 
  {   
    if(!this->priv_ParticleListReady()){this->priv_PrepParticleList();}
    return ParticleInventory::TrackIdToMotherParticle_P(id);
  }

  const art::Ptr<simb::MCTruth>& ParticleInventoryService::TrackIdToMCTruth_P(int const& id) 
  {
    if(!this->priv_TrackIdToMCTruthReady()){this->priv_PrepTrackIdToMCTruthIndex();}
    return ParticleInventory::TrackIdToMCTruth_P(id);
  }

  const art::Ptr<simb::MCTruth>& ParticleInventoryService::ParticleToMCTruth_P(const simb::MCParticle* p)
  {
    if(!this->priv_TrackIdToMCTruthReady()){this->priv_PrepTrackIdToMCTruthIndex();}
    return this->TrackIdToMCTruth_P(p->TrackId());
  }

  const std::vector<const simb::MCParticle*> ParticleInventoryService::MCTruthToParticles_Ps(art::Ptr<simb::MCTruth> const& mct) 
  {
    if(!this->priv_ParticleListReady()){this->priv_PrepParticleList();}
    if(!this->priv_MCTruthListReady()){this->priv_PrepMCTruthList();}
    return ParticleInventory::MCTruthToParticles_Ps(mct);
  }

  std::set<int> ParticleInventoryService::GetSetOfTrackIds(){
    if(!this->priv_ParticleListReady()){this->priv_PrepParticleList();}
    return ParticleInventory::GetSetOfTrackIds();
  }

  std::set<int> ParticleInventoryService::GetSetOfEveIds(){
    if(!this->priv_ParticleListReady()){this->priv_PrepParticleList();}
    return ParticleInventory::GetSetOfEveIds();
  }

  DEFINE_ART_SERVICE(ParticleInventoryService)

} //namespace



