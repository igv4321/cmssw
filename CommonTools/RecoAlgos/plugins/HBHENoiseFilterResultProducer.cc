// -*- C++ -*-
//
// Package:    HBHENoiseFilterResultProducer
// Class:      HBHENoiseFilterResultProducer
//
/**\class HBHENoiseFilterResultProducer

 Description: Produces the result from the HBENoiseFilter

 Implementation:
              Use the HcalNoiseSummary to make cuts on an event-by-event basis
*/
//
// Original Author:  John Paul Chou (Brown)
//
//

#include <string>

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/METReco/interface/HcalNoiseSummary.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"

//
// class declaration
//

class HBHENoiseFilterResultProducer : public edm::EDProducer {
   public:
      explicit HBHENoiseFilterResultProducer(const edm::ParameterSet&);
      ~HBHENoiseFilterResultProducer();

   private:
      virtual void produce(edm::Event&, const edm::EventSetup&) override;

      // ----------member data ---------------------------

      // parameters
      edm::EDGetTokenT<HcalNoiseSummary> noisetoken_;
      std::string resultLabel_;
      double minRatio_, maxRatio_;
      int minHPDHits_, minRBXHits_, minHPDNoOtherHits_;
      int minZeros_;
      double minHighEHitTime_, maxHighEHitTime_;
      double maxRBXEMF_;
      int minNumIsolatedNoiseChannels_;
      double minIsolatedNoiseSumE_, minIsolatedNoiseSumEt_;

      bool useTS4TS5_;
      bool useRBXRechitR45Loose_;
      bool useRBXRechitR45Tight_;

      bool IgnoreTS4TS5ifJetInLowBVRegion_;
      edm::InputTag jetlabel_;
      edm::EDGetTokenT<reco::PFJetCollection> jettoken_;
      int maxjetindex_;
      double maxNHF_;
};


//
// constructors and destructor
//

HBHENoiseFilterResultProducer::HBHENoiseFilterResultProducer(const edm::ParameterSet& iConfig)
{
  //now do what ever initialization is needed
  noisetoken_ = consumes<HcalNoiseSummary>(iConfig.getParameter<edm::InputTag>("noiselabel"));
  resultLabel_ = iConfig.getParameter<std::string>("resultLabel");
  minRatio_ = iConfig.getParameter<double>("minRatio");
  maxRatio_ = iConfig.getParameter<double>("maxRatio");
  minHPDHits_ = iConfig.getParameter<int>("minHPDHits");
  minRBXHits_ = iConfig.getParameter<int>("minRBXHits");
  minHPDNoOtherHits_ = iConfig.getParameter<int>("minHPDNoOtherHits");
  minZeros_ = iConfig.getParameter<int>("minZeros");
  minHighEHitTime_ = iConfig.getParameter<double>("minHighEHitTime");
  maxHighEHitTime_ = iConfig.getParameter<double>("maxHighEHitTime");
  maxRBXEMF_ = iConfig.getParameter<double>("maxRBXEMF");
  minNumIsolatedNoiseChannels_ = iConfig.getParameter<int>("minNumIsolatedNoiseChannels");
  minIsolatedNoiseSumE_ = iConfig.getParameter<double>("minIsolatedNoiseSumE");
  minIsolatedNoiseSumEt_ = iConfig.getParameter<double>("minIsolatedNoiseSumEt");
  useTS4TS5_ = iConfig.getParameter<bool>("useTS4TS5");
  useRBXRechitR45Loose_ = iConfig.getParameter<bool>("useRBXRechitR45Loose");
  useRBXRechitR45Tight_ = iConfig.getParameter<bool>("useRBXRechitR45Tight");

  IgnoreTS4TS5ifJetInLowBVRegion_ = iConfig.getParameter<bool>("IgnoreTS4TS5ifJetInLowBVRegion");
  jetlabel_ =  iConfig.getParameter<edm::InputTag>("jetlabel");
  jettoken_ = mayConsume<reco::PFJetCollection>(jetlabel_);
  maxjetindex_ = iConfig.getParameter<int>("maxjetindex");
  maxNHF_ = iConfig.getParameter<double>("maxNHF");

  produces<bool>(resultLabel_.c_str());
}


HBHENoiseFilterResultProducer::~HBHENoiseFilterResultProducer()
{

}


//
// member functions
//

// ------------ method called on each new Event  ------------
void
HBHENoiseFilterResultProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

  // get the Noise summary object
  edm::Handle<HcalNoiseSummary> summary_h;
  iEvent.getByToken(noisetoken_, summary_h);
  if(!summary_h.isValid()) {
    throw edm::Exception(edm::errors::ProductNotFound) << " could not find HcalNoiseSummary.\n";
    return;
  }
  const HcalNoiseSummary& summary = *summary_h;

  bool goodJetFoundInLowBVRegion=false; // checks whether a jet is in a low BV region, where false noise flagging rate is higher.

  if ( IgnoreTS4TS5ifJetInLowBVRegion_==true)
    {
      edm::Handle<reco::PFJetCollection> pfjet_h;
      iEvent.getByToken(jettoken_, pfjet_h);
      if(pfjet_h.isValid())  // valid jet collection found
	{
	  int jetindex=0;
	  for( reco::PFJetCollection::const_iterator jet = pfjet_h->begin(); jet != pfjet_h->end(); ++jet)
	    {
	      if (jetindex>maxjetindex_) break; // only look at first N jets (N specified by user via maxjetindex_)
	      // Check whether jet is in low-BV region (0<eta<1.4, -1.8<phi<-1.4)
	      if (jet->eta()>0 && jet->eta()<1.4 &&
		  jet->phi()>-1.8 && jet->phi()<-1.4)
		{
		  // Look for a good jet in low BV region; if found, we will keep event
		  if  (maxNHF_<0 ||  jet->neutralHadronEnergyFraction()<maxNHF_)
		    {
		      goodJetFoundInLowBVRegion=true;
		      break;
		    }
		}
	      ++jetindex;
	    }
	} // if (pfjet_h.isValid())
      else // no valid jet collection found
	{
	  // If no jet collection found, do we want to throw a fatal exception?  Or just proceed normally, not treating the lowBV region as special?
	  //throw edm::Exception(edm::errors::ProductNotFound) << " could not find PFJetCollection with label "<<jetlabel_<<".\n";
	}
    } // if (IgnoreTS4TS5ifJetInLowBVRegion_==true)

  const bool fail = (summary.minE2Over10TS()<minRatio_) ||
      (summary.maxE2Over10TS()>maxRatio_) ||
      (summary.maxHPDHits()>=minHPDHits_) ||
      (summary.maxRBXHits()>=minRBXHits_) ||
      (summary.maxHPDNoOtherHits()>=minHPDNoOtherHits_) ||
      (summary.maxZeros()>=minZeros_) ||
      (summary.min25GeVHitTime()<minHighEHitTime_) ||
      (summary.max25GeVHitTime()>maxHighEHitTime_) ||
      (summary.minRBXEMF()<maxRBXEMF_) ||
      (summary.numIsolatedNoiseChannels()>=minNumIsolatedNoiseChannels_) ||
      (summary.isolatedNoiseSumE()>=minIsolatedNoiseSumE_) ||
      (summary.isolatedNoiseSumEt()>=minIsolatedNoiseSumEt_) ||
      (useTS4TS5_ && summary.HasBadRBXTS4TS5() == true && !goodJetFoundInLowBVRegion) ||
      (useRBXRechitR45Loose_ && summary.HasBadRBXRechitR45Loose() == true && !goodJetFoundInLowBVRegion) ||
      (useRBXRechitR45Tight_ && summary.HasBadRBXRechitR45Tight() == true && !goodJetFoundInLowBVRegion);

  std::auto_ptr<bool> pOut(new bool(!fail));
  iEvent.put(pOut, resultLabel_.c_str());
}

//define this as a plug-in
DEFINE_FWK_MODULE(HBHENoiseFilterResultProducer);
