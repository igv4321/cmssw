import FWCore.ParameterSet.Config as cms

hfPhase1Reco = cms.EDProducer("HFPhase1Reconstructor",
    # Label for the input HFPreRecHitCollection
    inputLabel = cms.InputTag("hfprereco"),

    # Reconstruction algorithm configuration to fetch from DB, if any
    algoConfigClass = cms.string(""),

    # Configure the reconstruction algorithm
    algorithm = cms.PSet(
        Class = cms.string("HFSimpleTimeCheck"),

        # Pass everything for now
        tlimits = cms.vdouble(-1000.0, 1000.0, -1000.0, 1000.0),

        # Linear mapping of the array with dimensions [3][3][2]
        energyWeights = cms.vdouble(
            0.0, 0.0,  # [0][0]
            0.0, 0.0,  # [0][1]
            0.0, 2.0,  # [0][2]
            0.0, 0.0,  # [1][0]
            0.0, 0.0,  # [1][1]
            0.0, 2.0,  # [1][2]
            2.0, 0.0,  # [2][0]
            2.0, 0.0,  # [2][1]
            1.0, 1.0   # [2][2]
        )
    )
)
