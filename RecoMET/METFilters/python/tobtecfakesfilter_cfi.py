import FWCore.ParameterSet.Config as cms

tobtecfakesfilter = cms.EDFilter("TobTecFakesFilter",
                                 minEta = cms.double(0.9), # beginning of transition region for "jet" search
                                 maxEta = cms.double(1.6), # end of transition region for "jet" search
                                 phiWindow = cms.double(0.7), # size of phi region for "jet" search
                                 filter = cms.bool(True), # if true, only events passing filter (bad events) will pass
                                 trackCollection = cms.InputTag("generalTracks"), # track collection to use
                                 ratioAllCut = cms.double(-1.0), # minimum ratio of TOBTEC-seeded tracks / pixel-seeded tracks
                                 ratioJetCut = cms.double(3.0), # minimum ratio of TOBTEC-seeded tracks / pixel-seeded tracks in "jet"
                                 absJetCut = cms.double(20.0) # minimum number of TOBTEC-seeded tracks in "jet"
                                 )
# Filter passes events which pass all three of the cuts.  The "jet" is found by looping on TOBTEC tracks with minEta < |eta| < maxEta
# region and finding the point in phi which gives the most tracks inside a window of phiWindow radians.
