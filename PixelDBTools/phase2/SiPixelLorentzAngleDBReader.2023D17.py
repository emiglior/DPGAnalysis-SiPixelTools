import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process("Test",eras.Phase2)
process.load('Configuration.Geometry.GeometryExtended2023D17Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2023D17_cff')

process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')




process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)
process.source = cms.Source("EmptySource",
    firstRun = cms.untracked.uint32(1)
)

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("histo.root")
                                   )

process.MessageLogger = cms.Service("MessageLogger",
    cout = cms.untracked.PSet(
        threshold = cms.untracked.string('WARNING')
    ),
    destinations = cms.untracked.vstring('cout')
)

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag  import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:phase2_realistic', '')


# DB stuff
useLocalDB = True
if useLocalDB :
  from CondCore.CondDB.CondDB_cfi import *
  myCondDB = CondDB.clone( connect = cms.string('sqlite_file:SiPixelLorentzAngle_2023D17.db') )
  process.DBReader = cms.ESSource("PoolDBESSource",
                                  myCondDB,
                                  toGet = cms.VPSet(cms.PSet(record =
                                                             cms.string("SiPixelLorentzAngleRcd"),                                                 
                                                             tag = cms.string("SiPixelLorentzAngle_2023D17_test")
                                                             ),
                                                     ),
                                  ) # end process
process.es_prefer_DBReader = cms.ESPrefer("PoolDBESSource","DBReader")
# end if
   

process.LorentzAngleReader = cms.EDAnalyzer("SiPixelLorentzAngleDBReader",
    printDebug = cms.untracked.bool(True),
#    label = cms.untracked.string("fromAlignment"),
    useSimRcd = cms.bool(False)                                    
)

process.p = cms.Path(process.LorentzAngleReader)

#process.Tracer = cms.Service("Tracer")
