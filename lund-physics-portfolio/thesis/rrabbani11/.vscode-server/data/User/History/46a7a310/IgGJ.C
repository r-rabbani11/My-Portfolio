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
#include <memory>

using namespace thermalfist;

constexpr double kVolVsMult = 2.4;
constexpr double kVolOffset = 0.;

const Int_t nMultClasses = 9;
constexpr double kCentClasses[nMultClasses+1] = {0.,    1.,    5.,    10.,   20.,   30.,   40.,   50.,   70.,   100.};
constexpr double kMultCharged[nMultClasses]   = {26.0,  20.0,  16.2,  13.75, 10.0,  8.0,   6.3,   4.5,   2.5};
constexpr double beta_avg[nMultClasses]       = {0.488, 0.44,  0.4,   0.378, 0.325, 0.287, 0.25,  0.20,  0.11};
constexpr double T_kin[nMultClasses]          = {0.163, 0.174, 0.180, 0.181, 0.184, 0.184, 0.183, 0.181, 0.173};
constexpr double n[nMultClasses]              = {1.47,  1.70,  2.01,  2.25,  2.89,  3.48,  4.2,   5.71,  11.6};

constexpr int kNSample = 1;

void GenFistPP(Int_t nEventsPerPercent=100, const double kCorrVolume = 3., const int kSeed = 0) {
    gRandom->SetSeed(kSeed);
    RandomGenerators::SetSeed(gRandom->Integer(10000));

    TStopwatch clock;
    clock.Start();

    // Initialize thermal particle system
    ThermalParticleSystem parts("/home/rrabbani11/Thermal-FIST/input/list/PDG2020/list.dat");
    TDatabasePDG* pdgBase = TDatabasePDG::Instance();

    // Output file
    std::unique_ptr<TFile> out_file(new TFile("~/fist_data/fist_data2.root", "recreate"));
    if (!out_file || out_file->IsZombie()) {
        std::cerr << "Error: Could not create output file!" << std::endl;
        return;
    }

    // Create output tree
    std::unique_ptr<TTree> treeOut(new TTree("TT", "Pythia Tree"));
    std::unique_ptr<TClonesArray> trackArray(new TClonesArray("MyParticle", 1000));
    std::unique_ptr<MyEvent> event(new MyEvent());

    treeOut->Branch("tracks", &trackArray);
    treeOut->Branch("event", &event);

    Int_t nTotalEvents = 0;

    for (Int_t kMultClass = 0; kMultClass < nMultClasses; kMultClass++) {
        std::cout << "\nProcessing multiplicity class " << kMultClass 
                  << " (" << kCentClasses[kMultClass] << "-" << kCentClasses[kMultClass+1] << "%)"
                  << std::endl;

        try {
            // Set up thermal model
            ThermalModelCanonical model(&parts);
            ThermalModelParameters params;
            params.muB = params.muQ = params.muS = 0.0;
            params.gammaS = 1.- 0.25 * exp(-kMultCharged[kMultClass] / 59.);
            params.gammaq = 1.0;
            params.T = 0.176 - 0.0026 * log(kMultCharged[kMultClass]);
            params.B = params.Q = params.S = 0;
            
            model.SetParameters(params);
            double volume = kCorrVolume * (kVolVsMult * kMultCharged[kMultClass] + kVolOffset);
            model.SetVolume(volume);
            model.SetCanonicalVolume(volume);
            model.ConserveBaryonCharge(true);
            model.ConserveElectricCharge(true);
            model.ConserveStrangeness(true);
            model.SetStatistics(1);
            model.CalculateQuantumNumbersRange(true);
            model.SetUseWidth(ThermalParticle::eBW);
            model.FillChemicalPotentials();

            // Set up event generator
            EventGeneratorConfiguration configMC;
            configMC.fModelType = EventGeneratorConfiguration::PointParticle;
            configMC.fEnsemble = EventGeneratorConfiguration::CE;
            configMC.B = model.Parameters().B;
            configMC.Q = model.Parameters().Q;
            configMC.S = model.Parameters().S;
            configMC.CFOParameters = model.Parameters();

            double betaS = (2. + n[kMultClass]) / 2. * beta_avg[kMultClass];
            std::cout << "Parameters: T_kin=" << T_kin[kMultClass] 
                      << ", betaS=" << betaS 
                      << ", n=" << n[kMultClass] << std::endl;

            CylindricalBlastWaveEventGenerator generator(model.TPS(), configMC);
            generator.SetParameters(T_kin[kMultClass], betaS, kCorrVolume * 0.5, n[kMultClass]);

            const Int_t kNumberOfEvents = (kCentClasses[kMultClass+1] - kCentClasses[kMultClass]) * nEventsPerPercent;
            
            for (int i = 0; i < kNumberOfEvents; ++i) {
                nTotalEvents++;
                if (nTotalEvents % 100 == 0) {
                    std::cout << "Generated " << nTotalEvents << " events (" 
                              << 100. * i / kNumberOfEvents << "% of current class)" << std::endl;
                }

                try {
                    auto ev = generator.GetEvent(true);
                    Int_t nAccepted = 0;

                    for (const auto& p : ev.Particles) {
                        const Int_t absid = TMath::Abs(p.PDGID);
                        if (absid != 211 && absid != 321 && absid != 310 && absid != 311 &&
                            absid != 2212 && absid != 3122 && absid != 3312 && absid != 333 &&
                            absid != 313 && absid != 3334) {
                            continue;
                        }

                        TParticlePDG* pdgPart = pdgBase->GetParticle(p.PDGID);
                        if (!pdgPart) {
                            std::cerr << "Warning: Unknown particle with PDG ID: " << p.PDGID << std::endl;
                            continue;
                        }

                        MyParticle* track = new((*trackArray)[nAccepted]) MyParticle();
                        nAccepted++;
                        
                        track->fPt = p.GetPt();
                        track->fPhi = TMath::ATan2(p.py, p.px);
                        track->fY = p.GetY();
                        track->fMass = p.m;
                        track->fEta = p.GetEta();
                        track->fPDG = p.PDGID;
                        track->fCharge = Int_t(pdgPart->Charge()/3);
                        track->fIsPrimary = (absid == 333 || absid == 313) ? 2 : 1;
                    }

                    event->fV0A = gRandom->Rndm()*(kCentClasses[kMultClass+1] - kCentClasses[kMultClass]) + kCentClasses[kMultClass];
                    event->fV0C = event->fV0A;
                    event->fCL1 = kMultCharged[kMultClass];

                    treeOut->Fill();
                    trackArray->Clear();

                } catch (const std::exception& e) {
                    std::cerr << "Error processing event " << i << ": " << e.what() << std::endl;
                    trackArray->Clear();
                    continue;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in multiplicity class " << kMultClass << ": " << e.what() << std::endl;
            continue;
        }
    }

    out_file->cd();
    treeOut->Write();
    out_file->Close();

    clock.Stop();
    std::cout << "\nSuccessfully generated " << nTotalEvents << " events in ";
    clock.Print();
}

int main() {
    try {
        GenFistPP();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}