#ifndef DQMOffline_Trigger_LepHTMonitor_h
#define DQMOffline_Trigger_LepHTMonitor_h

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMOffline/Trigger/interface/TriggerDQMBase.h"
#include "CommonTools/TriggerUtils/interface/GenericTriggerEventFlag.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METCollection.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerEventWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerEventWithRefs.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/EgammaCandidates/interface/ConversionFwd.h"
#include "DataFormats/EgammaCandidates/interface/Conversion.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

class LepHTMonitor : public DQMEDAnalyzer, public TriggerDQMBase {
public:
  typedef dqm::reco::MonitorElement MonitorElement;
  typedef dqm::reco::DQMStore DQMStore;

  LepHTMonitor(const edm::ParameterSet& ps);
  ~LepHTMonitor() throw() override;

protected:
  void bookHistograms(DQMStore::IBooker& ibooker, const edm::Run&, const edm::EventSetup&) override;
  void analyze(const edm::Event& e, const edm::EventSetup& eSetup) override;

private:
  edm::InputTag theElectronTag_;
  edm::EDGetTokenT<edm::View<reco::GsfElectron> > theElectronCollection_;
  edm::InputTag theElectronVIDTag_;
  edm::EDGetTokenT<edm::ValueMap<bool> > theElectronVIDMap_;
  edm::InputTag theMuonTag_;
  edm::EDGetTokenT<reco::MuonCollection> theMuonCollection_;
  edm::InputTag thePfMETTag_;
  edm::EDGetTokenT<reco::PFMETCollection> thePfMETCollection_;
  edm::InputTag thePfJetTag_;
  edm::EDGetTokenT<reco::PFJetCollection> thePfJetCollection_;
  edm::InputTag theJetTagTag_;
  edm::EDGetTokenT<reco::JetTagCollection> theJetTagCollection_;
  edm::InputTag theVertexCollectionTag_;
  edm::EDGetTokenT<reco::VertexCollection> theVertexCollection_;
  edm::InputTag theConversionCollectionTag_;
  edm::EDGetTokenT<reco::ConversionCollection> theConversionCollection_;
  edm::InputTag theBeamSpotTag_;
  edm::EDGetTokenT<reco::BeamSpot> theBeamSpot_;

  std::unique_ptr<GenericTriggerEventFlag> num_genTriggerEventFlag_;
  std::unique_ptr<GenericTriggerEventFlag> den_lep_genTriggerEventFlag_;
  std::unique_ptr<GenericTriggerEventFlag> den_HT_genTriggerEventFlag_;

  const std::string folderName_;

  const bool requireValidHLTPaths_;
  bool hltPathsAreValid_;

  int muonIDlevel_;

  double jetPtCut_;
  double jetEtaCut_;
  double metCut_;
  double htCut_;
  double nmusCut_;
  double nelsCut_;
  double lep_pt_plateau_;
  double lep_counting_threshold_;
  double lep_iso_cut_;
  double lep_eta_cut_;
  double lep_d0_cut_b_;
  double lep_dz_cut_b_;
  double lep_d0_cut_e_;
  double lep_dz_cut_e_;

  std::vector<double> ptbins_;
  std::vector<double> htbins_;
  int nbins_eta_;
  int nbins_phi_;
  int nbins_npv_;
  float etabins_min_;
  float etabins_max_;
  float phibins_min_;
  float phibins_max_;
  float npvbins_min_;
  float npvbins_max_;

  // Histograms
  MonitorElement* h_pfHTTurnOn_num_;
  MonitorElement* h_pfHTTurnOn_den_;
  MonitorElement* h_lepPtTurnOn_num_;
  MonitorElement* h_lepPtTurnOn_den_;
  MonitorElement* h_lepEtaTurnOn_num_;
  MonitorElement* h_lepEtaTurnOn_den_;
  MonitorElement* h_lepPhiTurnOn_num_;
  MonitorElement* h_lepPhiTurnOn_den_;
  MonitorElement* h_lepEtaPhiTurnOn_num_;
  MonitorElement* h_lepEtaPhiTurnOn_den_;
  MonitorElement* h_NPVTurnOn_num_;
  MonitorElement* h_NPVTurnOn_den_;
};

#endif
