#include <HRGEventGenerator/RandomGenerators.h>
#include <HRGBase/ThermalParticleSystem.h>
#include <HRGBase/ThermalModelCanonical.h>
#include <HRGBase/ThermalModelParameters.h>
#include <HRGBase/ThermalParticle.h>
#include <HRGEventGenerator/EventGeneratorBase.h>
#include <HRGEventGenerator/CylindricalBlastWaveEventGenerator.h>

#include <TRandom.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TH1.h>
#include <TClonesArray.h>
#include <TMath.h>
#include <TDatabasePDG.h>

#include "MyEvent.h"
#include "MyParticle.h"

#include <vector>
#include <iostream>

using namespace thermalfist;

constexpr double kVolVsMult = 2.4;
constexpr double kVolOffset = 0.;

const Int_t nMultClasses = 9;
constexpr double kCentClasses[nMultClasses + 1] = {0., 1., 5., 10., 20., 30., 40., 50., 70., 100.};
constexpr double kMultCharged[nMultClasses] = {26.0, 20.0, 16.2, 13.75, 10.0, 8.0, 6.3, 4.5, 2.5};
constexpr double beta_avg[nMultClasses] = {0.488, 0.44, 0.4, 0.378, 0.325, 0.287, 0.25, 0.20, 0.11};
constexpr double T_kin[nMultClasses] = {0.163, 0.174, 0.180, 0.181, 0.184, 0.184, 0.183, 0.181, 0.173};
constexpr double n[nMultClasses] = {1.47, 1.70, 2.01, 2.25, 2.89, 3.48, 4.2, 5.71, 11.6};

constexpr int kNSample = 1;

void GenFistPP(Int_t nEventsPerPercent = 10, const double kCorrVolume = 3., const int kSeed = 0) {
    gRandom->SetSeed(kSeed);
    RandomGenerators::SetSeed(gRandom->Integer(10000));

    TStopwatch clock;
    clock.Start();

    // Thermal model (CE)
    ThermalParticleSystem parts("/home/rrabbani11/Thermal-FIST/input/list/PDG2020/list.dat");

    // TODO: check if file exists
    TFile* out_file = new TFile("~/fist_data/fist_data10.root", "recreate");
    out_file->cd();
    TTree* treeOut = 0;

    // Create trackArray and event only once
    TClonesArray* trackArray = new TClonesArray("MyParticle", 1000);
    TClonesArray* parentsArray = new TClonesArray("MyParticle", 1000);
    MyEvent* event = 0;

    treeOut = new TTree("TT", "Pythia Tree");
    treeOut->Bronch("tracks", "TClonesArray", &trackArray);
    treeOut->Branch("parents", &parentsArray);
    treeOut->Branch("event", &event);

    Int_t nTotalEvents = 0;
    TDatabasePDG* pdgBase = TDatabasePDG::Instance();

    for (Int_t kMultClass = 0; kMultClass < nMultClasses; kMultClass++) {
        std::cout << "Generating mult class " << kMultClass << std::endl;

        ThermalModelCanonical* model = new ThermalModelCanonical(&parts);
        ThermalModelParameters params;
        params.muB = 0.0;
        params.muQ = 0.0;
        params.muS = 0.0;
        params.gammaS = 1. - 0.25 * exp(-kMultCharged[kMultClass] / 59.);
        params.gammaq = 1.0;
        params.T = 0.176 - 0.0026 * log(kMultCharged[kMultClass]);
        params.B = params.Q = params.S = 0;
        model->SetParameters(params);
        model->SetVolume(kCorrVolume * (kVolVsMult * kMultCharged[kMultClass] + kVolOffset));
        model->SetCanonicalVolume(kCorrVolume * (kVolVsMult * kMultCharged[kMultClass] + kVolOffset));
        model->ConserveBaryonCharge(true);
        model->ConserveElectricCharge(true);
        model->ConserveStrangeness(true);
        model->SetStatistics(1);
        model->CalculateQuantumNumbersRange(true);
        model->SetUseWidth(ThermalParticle::eBW);
        model->FillChemicalPotentials();

        EventGeneratorConfiguration configMC;
        configMC.fModelType = EventGeneratorConfiguration::PointParticle;
        configMC.fEnsemble = EventGeneratorConfiguration::CE;
        configMC.B = model->Parameters().B;
        configMC.Q = model->Parameters().Q;
        configMC.S = model->Parameters().S;
        configMC.CFOParameters = model->Parameters();

        CylindricalBlastWaveEventGenerator* generator = new CylindricalBlastWaveEventGenerator(model->TPS(), configMC);
        double betaS = (2. + n[kMultClass]) / 2. * beta_avg[kMultClass];
        std::cout << "betaS: " << betaS << std::endl;
        generator->SetParameters(T_kin[kMultClass], betaS, kCorrVolume * 0.5, n[kMultClass]);

        const Int_t kNumberOfEvents = (kCentClasses[kMultClass + 1] - kCentClasses[kMultClass]) * nEventsPerPercent;
        for (int i = 0; i < kNumberOfEvents; ++i) {
            nTotalEvents++;
            if (nTotalEvents % 1000 == 0) std::cout << "generated " << nTotalEvents << " events..." << std::endl;

            auto ev = generator->GetEvent(true);
            Int_t nAccepted = 0;
            trackArray->Clear();
            parentsArray->Clear();

            std::vector<SimpleParticle> part = ev.Particles;
            for (auto p : part) {
                const Int_t absid = TMath::Abs(p.PDGID);
                if (absid != 211 && absid != 321 && absid != 310 && absid != 311 &&
                    absid != 2212 && absid != 3122 && absid != 3312 &&
                    absid != 333 && absid != 313 && absid != 3334)
                    continue;

                MyParticle* track = new ((*trackArray)[nAccepted]) MyParticle();
                nAccepted++;
                track->fPt = p.GetPt();
                track->fPhi = TMath::ATan2(p.py, p.px);
                track->fY = p.GetY();
                track->fMass = p.m;
                track->fEta = p.GetEta();
                track->fPDG = p.PDGID;
                track->fCharge = Int_t((pdgBase->GetParticle(p.PDGID))->Charge() / 3);
                track->fIsPrimary = 1;
                if (absid == 333 || absid == 313)
                    track->fIsPrimary = 2;

                // Add parent information to parentsArray
                if (p.MotherIndex >= 0) {
                    const auto& parentParticle = part[p.MotherIndex];
                    MyParticle* parent = new ((*parentsArray)[nAccepted - 1]) MyParticle();
                    parent->fPt = parentParticle.GetPt();
                    parent->fPhi = TMath::ATan2(parentParticle.py, parentParticle.px);
                    parent->fY = parentParticle.GetY();
                    parent->fMass = parentParticle.m;
                    parent->fEta = parentParticle.GetEta();
                    parent->fPDG = parentParticle.PDGID;
                    parent->fCharge = Int_t((pdgBase->GetParticle(parentParticle.PDGID))->Charge() / 3);
                    parent->fIsPrimary = 1;
                }
            }

            const Double_t cent = gRandom->Rndm() * (kCentClasses[kMultClass + 1] - kCentClasses[kMultClass]) + kCentClasses[kMultClass];
            event->fV0A = cent;
            event->fV0C = cent;
            event->fCL1 = kMultCharged[kMultClass];

            treeOut->Fill();
            trackArray->Delete();
            parentsArray->Delete();
        }

        delete model;
        delete generator;
    }

    out_file->cd();
    out_file->Write();
    out_file->Close();

    clock.Stop();
    clock.Print();
}

int main() {
    GenFistPP();
    return 0;
}