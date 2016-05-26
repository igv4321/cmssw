// -*- C++ -*-
//
// Package:    RecoLocalCalo/HcalRecProducers
// Class:      HFPhase1Reconstructor
// 
/**\class HFPhase1Reconstructor HFPhase1Reconstructor.cc RecoLocalCalo/HcalRecProducers/src/HFPhase1Reconstructor.cc

 Description: Phase 1 HF reco with QIE 10 and split-anode readout

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Igor Volobouev
//         Created:  Thu, 25 May 2016 00:17:51 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

#include "CalibFormats/HcalObjects/interface/HcalDbService.h"
#include "CalibFormats/HcalObjects/interface/HcalDbRecord.h"

#include "CondFormats/HcalObjects/interface/AbsHFPhase1AlgoData.h"

// Headers for phase 1 HF reco algorithms
#include "RecoLocalCalo/HcalRecAlgos/interface/HFSimpleTimeCheck.h"

//
// If you need to make HFPhase1Reconstructor aware of some new
// algorithms, update the functions "fetchHFPhase1AlgoData" and/or
// "parseHFPhase1AlgoDescription" in the unnamed namespace below.
// It is not necessary to modify the code of HFPhase1Reconstructor
// class for this purpose.
//
namespace {
    // Class Data must inherit from AbsHFPhase1AlgoData
    // and must have a copy constructor. This function
    // returns an object allocated on the heap.
    template <class Data, class Record>
    Data* fetchHFPhase1AlgoDataHelper(const edm::EventSetup& es)
    {
        edm::ESHandle<Data> p;
        es.get<Record>().get(p);
        return new Data(*p.product());
    }

    // Factory function for fetching (from EventSetup) objects
    // of the types inheriting from AbsHFPhase1AlgoData
    std::unique_ptr<AbsHFPhase1AlgoData>
    fetchHFPhase1AlgoData(const std::string& className, const edm::EventSetup& es)
    {
        AbsHFPhase1AlgoData* data = 0;
        // Compare with possibe class names
        // if (className == "MyHFPhase1AlgoData")
        //     data = fetchHFPhase1AlgoDataHelper<MyHFPhase1AlgoData, MyHFPhase1AlgoDataRcd>(es);
        // else if (className == "OtherHFPhase1AlgoData")
        //     ...;
        return std::unique_ptr<AbsHFPhase1AlgoData>(data);
    }

    // Factory function for creating objects of types
    // inheriting from AbsHFPhase1Algo out of parameter sets
    std::unique_ptr<AbsHFPhase1Algo>
    parseHFPhase1AlgoDescription(const edm::ParameterSet& ps)
    {
        std::unique_ptr<AbsHFPhase1Algo> algo;

        const std::string& className = ps.getParameter<std::string>("Class");

        if (className == "HFSimpleTimeCheck")
        {
            const std::vector<double>& tlimitsVec =
                ps.getParameter<std::vector<double> >("tlimits");
            const std::vector<double>& energyWeightsVec =
                ps.getParameter<std::vector<double> >("energyWeights");

            std::pair<float,float> tlimits[2];
            float energyWeights[HFSimpleTimeCheck::N_POSSIBLE_STATES][HFSimpleTimeCheck::N_POSSIBLE_STATES][2];
            const unsigned sz = sizeof(energyWeights)/sizeof(energyWeights[0][0][0]);

            if (tlimitsVec.size() == 4 && energyWeightsVec.size() == sz)
            {
                tlimits[0] = std::pair<float,float>(tlimitsVec[0], tlimitsVec[1]);
                tlimits[1] = std::pair<float,float>(tlimitsVec[2], tlimitsVec[3]);

                // Same order of elements as in the natural C++ array mapping
                float* to = &energyWeights[0][0][0];
                for (unsigned i=0; i<sz; ++i)
                    to[i] = energyWeightsVec[i];

                algo = std::unique_ptr<AbsHFPhase1Algo>(
                    new HFSimpleTimeCheck(tlimits, energyWeights));
            }
        }

        return algo;
    }
}


//
// class declaration
//
class HFPhase1Reconstructor : public edm::stream::EDProducer<>
{
public:
    explicit HFPhase1Reconstructor(const edm::ParameterSet&);
    ~HFPhase1Reconstructor();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    virtual void beginRun(const edm::Run&, const edm::EventSetup&) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;

    // Configuration parameters
    std::string algoConfigClass_;

    // Other members
    edm::EDGetTokenT<HFPreRecHitCollection> tok_PreRecHit_;
    std::unique_ptr<AbsHFPhase1Algo> reco_;
    std::unique_ptr<AbsHFPhase1AlgoData> recoConfig_;
};

//
// constructors and destructor
//
HFPhase1Reconstructor::HFPhase1Reconstructor(const edm::ParameterSet& conf)
    : algoConfigClass_(conf.getParameter<std::string>("algoConfigClass")),
      reco_(parseHFPhase1AlgoDescription(conf.getParameter<edm::ParameterSet>("algorithm")))
{
    // Check that the algorithm has been successfully configured
    if (!reco_.get())
        throw cms::Exception("HFPhase1BadConfig")
            << "Invalid HFPhase1Reconstructor algorithm configuration"
            << std::endl;

    // Describe consumed data
    tok_PreRecHit_ = consumes<HFPreRecHitCollection>(
        conf.getParameter<edm::InputTag>("inputLabel"));

    // Register the product
    produces<HFRecHitCollection>();
}


HFPhase1Reconstructor::~HFPhase1Reconstructor()
{
 
   // do anything here that needs to be done at destruction time
   // (e.g. close files, deallocate resources etc.)

}


void
HFPhase1Reconstructor::beginRun(const edm::Run& r, const edm::EventSetup& es)
{
    if (reco_->isConfigurable())
    {
        recoConfig_ = fetchHFPhase1AlgoData(algoConfigClass_, es);
        if (!recoConfig_.get())
            throw cms::Exception("HFPhase1BadConfig")
                << "Invalid HFPhase1Reconstructor \"algoConfigClass\" parameter value \""
                << algoConfigClass_ << '"' << std::endl;
        if (!reco_->configure(recoConfig_.get()))
            throw cms::Exception("HFPhase1BadConfig")
                << "Failed to configure HFPhase1Reconstructor algorithm from EventSetup"
                << std::endl;
    }
}

// ------------ method called to produce the data  ------------
void
HFPhase1Reconstructor::produce(edm::Event& e, const edm::EventSetup& eventSetup)
{
    using namespace edm;

    // Get the calibrations
    ESHandle<HcalDbService> conditions;
    eventSetup.get<HcalDbRecord>().get(conditions);

    // Get the input data
    Handle<HFPreRecHitCollection> preRecHits;
    e.getByToken(tok_PreRecHit_, preRecHits);

    // Create a new output collection
    std::unique_ptr<HFRecHitCollection> out(std::make_unique<HFRecHitCollection>());
    out->reserve(preRecHits->size());

    // Iterate over the input and fill the output collection
    for (HFPreRecHitCollection::const_iterator it = preRecHits->begin();
         it != preRecHits->end(); ++it)
    {
        const HFRecHit& rechit = reco_->reconstruct(
            *it, conditions->getHcalCalibrations(it->id()));
        if (rechit.id().rawId())
            out->push_back(rechit);
    }

    // Add the output collection to the event record
    e.put(std::move(out));
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
HFPhase1Reconstructor::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
    edm::ParameterSetDescription desc;

    desc.add<edm::InputTag>("inputLabel");
    desc.add<std::string>("algoConfigClass");
    edm::ParameterSetDescription algoArgs;
    algoArgs.setAllowAnything();
    desc.add<edm::ParameterSetDescription>("algorithm", algoArgs);

    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HFPhase1Reconstructor);
