import FWCore.ParameterSet.Config as cms

from SimCalorimetry.EcalSimProducers.apdSimParameters_cff import *
from SimCalorimetry.EcalSimProducers.ecalSimParameterMap_cff import *
from SimCalorimetry.EcalSimProducers.ecalElectronicsSim_cff import *
from SimCalorimetry.EcalSimProducers.esElectronicsSim_cff import *
from SimCalorimetry.EcalSimProducers.ecalNotContainmentSim_cff import *
from SimCalorimetry.EcalSimProducers.ecalCosmicsSim_cff import *
simEcalUnsuppressedDigis = cms.EDProducer("EcalDigiProducer",
    apd_sim_parameters,
    ecal_electronics_sim,
    ecal_cosmics_sim,
    ecal_sim_parameter_map,
    ecal_notCont_sim,
    es_electronics_sim,
    hitsProducer = cms.string('g4SimHits'),
    EEdigiCollection = cms.string(''),
    EBdigiCollection = cms.string(''),
    ESdigiCollection = cms.string(''),

    EBCorrNoiseMatrixG12 = cms.vdouble (
    1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804, 0.32350, 0.31461, 0.30373,
    0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804, 0.32350, 0.31461,
    0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804, 0.33250,
    0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804,
    0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784,
    0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324,
    0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993,
    0.32350, 0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573,
    0.31461, 0.32350, 0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968,
    0.30373, 0.31461, 0.32350, 0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000) ,

    EECorrNoiseMatrixG12 = cms.vdouble (
    1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908, 0.11177, 0.10172, 0.09383, 0.08207,
    0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908, 0.11177, 0.10172, 0.09383,
    0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908, 0.11177, 0.10172,
    0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14909, 0.11177,
    0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908,
    0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628,
    0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340,
    0.10172, 0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156,
    0.09383, 0.10172, 0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668,
    0.08207, 0.09383, 0.10172, 0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000) ,

    EBCorrNoiseMatrixG06 = cms.vdouble (
    1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804, 0.32350, 0.31461, 0.30373,
    0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804, 0.32350, 0.31461,
    0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804, 0.33250,
    0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804,
    0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784,
    0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324,
    0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993,
    0.32350, 0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573,
    0.31461, 0.32350, 0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968,
    0.30373, 0.31461, 0.32350, 0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000) ,

    EECorrNoiseMatrixG06 = cms.vdouble (
    1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908, 0.11177, 0.10172, 0.09383, 0.08207,
    0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908, 0.11177, 0.10172, 0.09383,
    0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908, 0.11177, 0.10172,
    0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14909, 0.11177,
    0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908,
    0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628,
    0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340,
    0.10172, 0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156,
    0.09383, 0.10172, 0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668,
    0.08207, 0.09383, 0.10172, 0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000),

    EBCorrNoiseMatrixG01 = cms.vdouble (
    1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804, 0.32350, 0.31461, 0.30373,
    0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804, 0.32350, 0.31461,
    0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804, 0.33250,
    0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784, 0.33804,
    0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324, 0.35784,
    0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993, 0.40324,
    0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573, 0.45993,
    0.32350, 0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968, 0.55573,
    0.31461, 0.32350, 0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000, 0.70968,
    0.30373, 0.31461, 0.32350, 0.33804, 0.35784, 0.40324, 0.45993, 0.55573, 0.70968, 1.00000) ,

    EECorrNoiseMatrixG01 = cms.vdouble (
    1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908, 0.11177, 0.10172, 0.09383, 0.08207,
    0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908, 0.11177, 0.10172, 0.09383,
    0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908, 0.11177, 0.10172,
    0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14909, 0.11177,
    0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628, 0.14908,
    0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340, 0.21628,
    0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156, 0.30340,
    0.10172, 0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668, 0.45156,
    0.09383, 0.10172, 0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000, 0.71668,
    0.08207, 0.09383, 0.10172, 0.11177, 0.14908, 0.21628, 0.30340, 0.45156, 0.71668, 1.00000)
)
