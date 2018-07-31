#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "Geometry/TrackerGeometryBuilder/interface/PixelGeomDetUnit.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h" // 
#include "Geometry/TrackerGeometryBuilder/interface/RectangularPixelTopology.h"

#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "CondFormats/SiPixelObjects/interface/SiPixelLorentzAngle.h"

#include "DQM/SiPixelPhase1Common/interface/SiPixelCoordinates.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementPoint.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementError.h"
#include "DataFormats/GeometrySurface/interface/GloballyPositioned.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"


#include "SiPixelLorentzAngleDBLoader.h"

using namespace std;
using namespace edm;

  //Constructor

SiPixelLorentzAngleDBLoader::SiPixelLorentzAngleDBLoader(edm::ParameterSet const& conf) : 
  conf_(conf){
  //magneticField_ = conf_.getParameter<double>("magneticField");
  recordName_ = conf_.getUntrackedParameter<std::string>("record","SiPixelLorentzAngleRcd");
  useFile_ = conf_.getParameter<bool>("useFile");		
  fileName_ = conf_.getParameter<string>("fileName");
  BPixParameters_   = conf_.getUntrackedParameter<Parameters>("BPixParameters");
  FPixParameters_   = conf_.getUntrackedParameter<Parameters>("FPixParameters");
  ModuleParameters_ = conf_.getUntrackedParameter<Parameters>("ModuleParameters");
  bPixLorentzAnglePerTesla1x2_ = (float)conf_.getUntrackedParameter<double>("bPixLorentzAnglePerTesla1x2",
								-9999.);
  bPixLorentzAnglePerTesla2x2_ = (float)conf_.getUntrackedParameter<double>("bPixLorentzAnglePerTesla2x2",
								-9999.);
  fPixLorentzAnglePerTesla1x2_ = (float)conf_.getUntrackedParameter<double>("fPixLorentzAnglePerTesla1x2",
								-9999.);
  fPixLorentzAnglePerTesla2x2_ = (float)conf_.getUntrackedParameter<double>("fPixLorentzAnglePerTesla2x2",
								-9999.);
}

//BeginJob

void SiPixelLorentzAngleDBLoader::beginJob(){
  
}

// Virtual destructor needed.

SiPixelLorentzAngleDBLoader::~SiPixelLorentzAngleDBLoader() {  

}  

// Analyzer: Functions that gets called by framework every event

void SiPixelLorentzAngleDBLoader::analyze(const edm::Event& e, const edm::EventSetup& es) {

	SiPixelLorentzAngle* LorentzAngle = new SiPixelLorentzAngle();

        // Initialize SiPixelCoordinates, used for the individual LA selection and printout
	SiPixelCoordinates coord;
	coord.init(es);

	//	edm::ESHandle<TrackerTopology> tTopo;
	//	es.get<TrackerTopologyRcd>().get(tTopo);

        //Retrieve old style tracker geometry from geometry
	edm::ESHandle<TrackerGeometry> tGeometry;
	es.get<TrackerDigiGeometryRecord>().get( tGeometry);
	std::cout<<" There are "<<tGeometry->detUnits().size() <<" detectors (old)"<<std::endl;
	
	//for(TrackerGeometry::DetContainer::const_iterator it = tGeometry->detUnits().begin(); 
	for(auto it = tGeometry->detUnits().begin(); it != tGeometry->detUnits().end(); it++){
	  
	  // for phase2 this does not select the inner tracker module but all modules 
	  const PixelGeomDetUnit * pixelGeomDetUnit = dynamic_cast<PixelGeomDetUnit const*>(*it);
	  if( pixelGeomDetUnit!=0 ) {
	    const DetId detid = (*it)->geographicalId();
	    auto detType= detid.det(); // det type, tracker=1
	    auto  rawId = detid.rawId();
	    int found = 0;

	    const PixelTopology * pixTopo            = &(pixelGeomDetUnit->specificTopology());	    	   

	    //cout<<detType<<endl;
	    // fill bpix values for LA 
	    if(detid.subdetId() == static_cast<int>(PixelSubdetector::PixelBarrel)) {
	      
	      // 
	      cout <<" pixel barrel:" 
		   <<" layer="<<coord.layer(rawId)<<" ladder="<<coord.ladder(rawId)<<" module="<<coord.module(rawId) 
		   <<" rawId=" << rawId;

	      // use a commmon value (e.g. for MC)
	      // choose here if 1x2 or 2x2
	      float bPixLorentzAnglePerTesla_ = -9999.; 
	      if ( pixTopo->rocsX() == 1 ) {
		bPixLorentzAnglePerTesla_ = bPixLorentzAnglePerTesla1x2_ ; 
	      } else if ( pixTopo->rocsX() == 2 ) {
		bPixLorentzAnglePerTesla_ = bPixLorentzAnglePerTesla2x2_ ; 
	      } else {
		cout << "UNKNOWN rocsX" << endl;
 	      } 
	      if(bPixLorentzAnglePerTesla_ != -9999. ) {  // use common value for all 
		cout<<" LA = "<< bPixLorentzAnglePerTesla_ << endl;		
		if(!LorentzAngle->putLorentzAngle(detid.rawId(),bPixLorentzAnglePerTesla_))
		  cout<<"[SiPixelLorentzAngleDB::analyze] ERROR!: detid already exists"<<std::endl;
		
	      } else if(useFile_) {

		  cout<<"method for reading file not implemented yet" << std::endl;

	      } else {
		
		//first individuals are put
		for(Parameters::iterator it = ModuleParameters_.begin(); 
		    it != ModuleParameters_.end(); ++it) {
		  if( it->getParameter<unsigned int>("rawid") == detid.rawId() ) {
		    float lorentzangle = (float)it->getParameter<double>("angle");
		    if (!found) {
		      LorentzAngle->putLorentzAngle(detid.rawId(),lorentzangle);
		      cout << " LA= " << lorentzangle 
			   << " individual value " << detid.rawId() << endl;
		      found = 1;
		    } else cout<<"[SiPixelLorentzAngleDB::analyze] ERROR!: detid already exists"<<std::endl;
		  }
		}
		
		//modules already put are automatically skipped
		for(Parameters::iterator it = BPixParameters_.begin(); 
		    it != BPixParameters_.end(); ++it) {
		  if (it->exists("layer"))  if (it->getParameter<int>("layer")  != coord.layer(detid))  continue;
		  if (it->exists("ladder")) if (it->getParameter<int>("ladder") != coord.ladder(detid)) continue;
		  if (it->exists("module")) if (it->getParameter<int>("module") != coord.module(detid)) continue;
		  if (it->exists("side"))   if (it->getParameter<int>("side")   != coord.side(detid)) continue;
		  if (!found) {
		    float lorentzangle = (float)it->getParameter<double>("angle");
		    LorentzAngle->putLorentzAngle(detid.rawId(),lorentzangle);
		    cout << " LA= " << lorentzangle << endl;
		    found = 2;
		  } else if (found==1) {
		    cout<<"[SiPixelLorentzAngleDB::analyze] detid already given in ModuleParameters, skipping ..."<<std::endl;
		  } else cout<<"[SiPixelLorentzAngleDB::analyze] ERROR!: detid already exists"<<std::endl;
		}
		
	      }
	      
	      // fill fpix values for LA (for phase2 fpix & epix)
	    } else if(detid.subdetId() == static_cast<int>(PixelSubdetector::PixelEndcap)) {
	      
	      // for phase2: ring=blade, panel==1 
	      cout << " pixel endcap:" 
		   <<" side="<<coord.side(detid)<<" disk=" << coord.disk(detid)<<" blade(ring)="<<coord.blade(detid)<<" panel="<< coord.panel(detid) << "  module=" << coord.module(detid) << "  rawId=" << rawId;
	      
	      // use a commmon value (e.g. for MC)
	      // choose here if 1x2 or 2x2
	      float fPixLorentzAnglePerTesla_ = -9999.; 
	      if ( pixTopo->rocsX() == 1 ) {
		fPixLorentzAnglePerTesla_ = fPixLorentzAnglePerTesla1x2_ ; 
	      } else if ( pixTopo->rocsX() == 2 ) {
		fPixLorentzAnglePerTesla_ = fPixLorentzAnglePerTesla2x2_ ; 
	      }
	      if(fPixLorentzAnglePerTesla_ != -9999.) {  // use common value for all 
		cout<<" LA = "<< fPixLorentzAnglePerTesla_<<" common for all fpix"<<endl;
		if(!LorentzAngle->putLorentzAngle(detid.rawId(),fPixLorentzAnglePerTesla_))
		  cout<<"[SiPixelLorentzAngleDB::analyze] detid already exists"<<std::endl;
		
	      } else if(useFile_) {

		  cout<<"method for reading file not implemented yet" << std::endl;

	      } else {
		
		//first individuals are put
		for(Parameters::iterator it = ModuleParameters_.begin(); 
		    it != ModuleParameters_.end(); ++it) {
		  if( it->getParameter<unsigned int>("rawid") == detid.rawId() ) {
		    float lorentzangle = (float)it->getParameter<double>("angle");
		    if (!found) {
		      LorentzAngle->putLorentzAngle(detid.rawId(),lorentzangle);
		      cout << " LA= " << lorentzangle 
			   << " individual value " << detid.rawId() << endl;
		      found = 1;
		    } else cout<<"[SiPixelLorentzAngleDB::analyze] ERROR!: detid already exists"<<std::endl;
		  } // if
		} // for 
		
		//modules already put are automatically skipped
		for(Parameters::iterator it = FPixParameters_.begin(); 
		    it != FPixParameters_.end(); ++it) {
		  if (it->exists("side"))    if (it->getParameter<int>("side")    != coord.side(detid))   continue;
		  if (it->exists("disk") )   if (it->getParameter<int>("disk")    != coord.disk(detid))   continue;
		  if (it->exists("ring") )   if (it->getParameter<int>("ring")    != coord.ring(detid))   continue;
		  if (it->exists("blade"))   if (it->getParameter<int>("blade")   != coord.blade(detid))  continue;
		  if (it->exists("panel"))   if (it->getParameter<int>("panel")   != coord.panel(detid))  continue;
		  if (it->exists("plq"))     if (it->getParameter<int>("plq")     != coord.module(detid)) continue;
		  if (it->exists("HVgroup")) if (it->getParameter<int>("HVgroup") != 
						 HVgroup(coord.panel(detid), coord.module(detid)))    continue;
		  if (!found) {
		    float lorentzangle = (float)it->getParameter<double>("angle");
		    LorentzAngle->putLorentzAngle(detid.rawId(),lorentzangle);
		    cout << " LA= " << lorentzangle << endl;
		    found = 2;
		  } else if (found==1) {
		    cout<<"[SiPixelLorentzAngleDB::analyze] detid already given in ModuleParameters, skipping ..."<<std::endl;
		  } else cout<<"[SiPixelLorentzAngleDB::analyze] ERROR!: detid already exists"<<std::endl;
		} // for  
	
	      } // if 	

	    } else { // bpix/fpix 
	      cout<<"detid  is Pixel but neither bpix nor fpix: rawId " << detid.rawId() << " det type  "<<detType<<" subdet "<<detid.subdetId()<<std::endl;
	      float defaultLA = 0.07;
	      LorentzAngle->putLorentzAngle(detid.rawId(),defaultLA); // some default value 
	    } // bpix/fpix
	    
	  }
	  
	}      
  	
	
	edm::Service<cond::service::PoolDBOutputService> mydbservice;
	if( mydbservice.isAvailable() ){
	  try{
	    if( mydbservice->isNewTagRequest(recordName_) ){
	      mydbservice->createNewIOV<SiPixelLorentzAngle>(LorentzAngle,
							     mydbservice->beginOfTime(),
							     mydbservice->endOfTime(),
							     recordName_);
	    } else {
	      mydbservice->appendSinceTime<SiPixelLorentzAngle>(LorentzAngle,
								mydbservice->currentTime(),
								recordName_);
	    }
	  }catch(const cond::Exception& er){
	    cout<<"SiPixelLorentzAngleDBLoader"<<er.what()<<std::endl;
	  }catch(const std::exception& er){
	    cout<<"SiPixelLorentzAngleDBLoader"<<"caught std::exception "<<er.what()<<std::endl;
	  }catch(...){
	    cout<<"Funny error"<<std::endl;
	  }
	}else{
	  cout<<"Service is unavailable"<<std::endl;
	}
	
}

int SiPixelLorentzAngleDBLoader::HVgroup(int panel, int module){

   if( 1 == panel && ( 1 == module || 2 == module ))  {
      return 1;
   }
   else if( 1 == panel && ( 3 == module || 4 == module ))  {
      return 2;
   }
   else if( 2 == panel && 1 == module )  {
      return 1;
   }
   else if( 2 == panel && ( 2 == module || 3 == module ))  {
      return 2;
   }
   else {
      cout << " *** error *** in SiPixelLorentzAngleDBLoader::HVgroup(...), panel = " << panel << ", module = " << module << endl;
      return 0;
   }
   
}


void SiPixelLorentzAngleDBLoader::endJob(){

}

//define this as a plug-in
DEFINE_FWK_MODULE(SiPixelLorentzAngleDBLoader);
