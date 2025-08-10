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
#include <algorithm>

using namespace thermalfist;

constexpr double kVolVsMult = 2.4;
constexpr double kVolOffset = 0.;

const Int_t nMultClasses = 9;
constexpr double kCentClasses[nMultClasses+1] = {0.,    1.,    5.,    10.,   20.,   30.,   40.,   50.,   70.,   100.};
constexpr double kMultCharged[nMultClasses]   = {26.0,  20.0,  16.2,  13.75, 10.0,  8.0,   6.3,   4.5,   2.5};
constexpr double beta_avg[nMultClasses]       = {0.488, 0.44,  0.4,   0.378, 0.325, 0.287, 0.25,  0.20,  0.08};  // Further reduced
constexpr double T_kin[nMultClasses]          = {0.163, 0.174, 0.180, 0.181, 0.184, 0.184, 0.183, 0.181, 0.173};
constexpr double n[nMultClasses]              = {1.47,  1.70,  2.01,  2.25,  2.89,  3.48,  4.2,   5.71,  6.0};  // Further reduced

void GenFistPP(Int_t nEventsPerPercent=100, const double kCorrVolume = 3., const int kSeed = 0) {
    gRandom->SetSeed(kSeed);
    RandomGenerators::SetSeed(gRandom->Integer(10000));

    TStopwatch clock;
    clock.Start();

    ThermalParticleSystem parts("/home/rrabbani11/Thermal-FIST/input/list/PDG2020/list.dat");
    TDatabasePDG* pdgBase = TDatabasePDG::Instance();

    TFile* out_file = new TFile("~/fist_data/fist_data2.root", "recreate");
    if (!out_file || out_file->IsZombie()) {
        std::cerr << "Error: Could not create output file!" << std::endl;
        return;
    }

    TTree* treeOut = new TTree("TT", "Pythia Tree");
    TClonesArray* trackArray = new TClonesArray("MyParticle", 1000);
    MyEvent* event = new MyEvent();

    treeOut->Branch("tracks", &trackArray, 32000, 99);
    treeOut->Branch("event", "MyEvent", &event, 32000, 99);

    Int_t nTotalEvents = 0;
    const int MAX_RETRIES = 5;  // Increased retry attempts
    const int EVENT_CHUNK = 500; // Process in chunks

    for (Int_t kMultClass = 0; kMultClass < nMultClasses; kMultClass++) {
        std::cout << "\nProcessing multiplicity class " << kMultClass 
                  << " (" << kCentClasses[kMultClass] << "-" << kCentClasses[kMultClass+1] << "%)"
                  << std::endl;

        try {
            ThermalModelCanonical model(&parts);
            ThermalModelParameters params;
            params.muB = params.muQ = params.muS = 0.0;
            params.gammaS = 1.- 0.25 * exp(-kMultCharged[kMultClass] / 59.);
            params.gammaq = 1.0;
            params.T = 0.176 - 0.0026 * log(kMultCharged[kMultClass]);
            params.B = params.Q = params.S = 0;
            
            // Enhanced stability for last class
            if (kMultClass == nMultClasses - 1) {
                params.T = std::max(params.T, 0.170);  // Higher minimum temperature
                std::cout << "Applying enhanced stability fixes for last multiplicity class..." << std::endl;
            }
            
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

            EventGeneratorConfiguration configMC;
            configMC.fModelType = EventGeneratorConfiguration::PointParticle;
            configMC.fEnsemble = EventGeneratorConfiguration::CE;
            configMC.B = model.Parameters().B;
            configMC.Q = model.Parameters().Q;
            configMC.S = model.Parameters().S;
            configMC.CFOParameters = model.Parameters();

            double betaS = (2. + n[kMultClass]) / 2. * beta_avg[kMultClass];
            betaS = std::min(betaS, 0.5);  // More conservative limit
            double actual_n = std::min(n[kMultClass], 6.0);
            
            std::cout << "Stable Parameters: T_kin=" << T_kin[kMultClass] 
                      << ", betaS=" << betaS 
                      << ", n=" << actual_n << std::endl;

            CylindricalBlastWaveEventGenerator generator(model.TPS(), configMC);
            generator.SetParameters(T_kin[kMultClass], betaS, kCorrVolume * 0.5, actual_n);

            const Int_t kNumberOfEvents = (kCentClasses[kMultClass+1] - kCentClasses[kMultClass]) * nEventsPerPercent;
            
            // Process in chunks for last class
            if (kMultClass == nMultClasses - 1) {
                const int NUM_CHUNKS = (kNumberOfEvents / EVENT_CHUNK) + 1;
                for (int chunk = 0; chunk < NUM_CHUNKS; chunk++) {
                    int chunk_start = chunk * EVENT_CHUNK;
                    int chunk_end = std::min((chunk + 1) * EVENT_CHUNK, kNumberOfEvents);
                    
                    std::cout << "Processing chunk " << chunk+1 << "/" << NUM_CHUNKS 
                              << " (events " << chunk_start << "-" << chunk_end << ")" << std::endl;
                    
                    for (int i = chunk_start; i < chunk_end; ++i) {
                        int retries = 0;
                        bool success = false;
                        
                        while (retries < MAX_RETRIES && !success) {
                            try {
                                delete event;
                                event = new MyEvent();
                                trackArray->Clear();

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
                                    if (!pdgPart) continue;

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
                                success = true;
                                nTotalEvents++;
                                
                                if (nTotalEvents % 50 == 0) {  // More frequent updates
                                    std::cout << "  Generated " << nTotalEvents << " events (" 
                                              << 100. * (i - chunk_start) / (chunk_end - chunk_start) 
                                              << "% of current chunk)" << std::endl;
                                }

                            } catch (const std::exception& e) {
                                retries++;
                                if (retries >= MAX_RETRIES) {
                                    std::cerr << "Skipping event after " << MAX_RETRIES 
                                              << " retries: " << e.what() << std::endl;
                                } else {
                                    // Full generator reset
                                    generator = CylindricalBlastWaveEventGenerator(model.TPS(), configMC);
                                    generator.SetParameters(T_kin[kMultClass], betaS, kCorrVolume * 0.5, actual_n);
                                }
                            }
                        }
                    }
                    
                    // Write after each chunk for safety
                    out_file->cd();
                    treeOut->Write();
                    std::cout << "Completed chunk " << chunk+1 << "/" << NUM_CHUNKS << std::endl;
                }
            } else {
                // Normal processing for other classes
                for (int i = 0; i < kNumberOfEvents; ++i) {
                    // ... [same as before, but with MAX_RETRIES=5]
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

    delete trackArray;
    delete event;
    delete treeOut;
    delete out_file;

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