#include "HRGEventGenerator/RandomGenerators.h"
#include "HRGBase/ThermalParticleSystem.h"
#include "HRGBase/ThermalModelCanonical.h"
#include "HRGBase/ThermalModelParameters.h"
#include "HRGBase/ThermalParticle.h"
#include "HRGEventGenerator/EventGeneratorBase.h"
#include "HRGEventGenerator/CylindricalBlastWaveEventGenerator.h"

#include "TFile.h"
#include "TTree.h"
#include "TDatabasePDG.h"
#include "TMath.h"

#include <vector>
#include <iostream>

using namespace thermalfist;

constexpr int kNumberOfEvents = 40000;
constexpr double kVolVsMult = 2.4;
constexpr double kVolOffset = 0.;

const int nMultClasses = 9;
constexpr double kCentClasses[nMultClasses + 1] = {0., 1., 5., 10., 20., 30., 40., 50., 70., 100.};
constexpr double kMultCharged[nMultClasses] = {26.0, 20.0, 16.2, 13.75, 10.0, 8.0, 6.3, 4.5, 2.5};
constexpr double beta_avg[nMultClasses] = {0.488, 0.44, 0.4, 0.378, 0.325, 0.287, 0.25, 0.20, 0.11};
constexpr double T_kin[nMultClasses] = {0.163, 0.174, 0.180, 0.181, 0.184, 0.184, 0.183, 0.181, 0.173};
constexpr double n[nMultClasses] = {1.47, 1.70, 2.01, 2.25, 2.89, 3.48, 4.2, 5.71, 11.6};

constexpr int kNSample = 1;

struct MyParticle {
    float fPt;
    float fEta;
    float fPhi;
    int fPDG;
    int fCharge;
};

void GenFistPP(int nEventsPerPercent = 10, const double kCorrVolume = 3., const int kSeed = 0) {
    // Initialize TDatabasePDG
    TDatabasePDG::Instance();

    // Create a ROOT file
    TFile* outFile = new TFile("fist_output.root", "RECREATE");
    if (!outFile || outFile->IsZombie()) {
        std::cerr << "Error: Could not create ROOT file!" << std::endl;
        return;
    }

    // Create a TTree
    TTree* tree = new TTree("events", "Event Tree");

    // Create a branch for particles
    MyParticle particle;
    tree->Branch("particle", &particle, "fPt/F:fEta/F:fPhi/F:fPDG/I:fCharge/I");

    // Thermal model (CE)
    ThermalParticleSystem parts("/home/rrabbani11/Thermal-FIST/input/list/PDG2020/list.dat");

    int nTotalEvents = 0;

    for (int kMultClass = 0; kMultClass < nMultClasses; kMultClass++) {
        std::cout << "Generating mult class " << kMultClass << std::endl;

        ThermalModelCanonical* model = new ThermalModelCanonical(&parts);
        ThermalModelParameters params;
        // Chemical potentials are fixed to zero
        params.muB = 0.0;
        params.muQ = 0.0;
        params.muS = 0.0;
        params.gammaS = 1. - 0.25 * exp(-kMultCharged[kMultClass] / 59.);
        params.gammaq = 1.0;
        // Initial temperature value in fits
        params.T = 0.176 - 0.0026 * log(kMultCharged[kMultClass]);
        // Quantum numbers are zero
        params.B = params.Q = params.S = 0;
        model->SetParameters(params);
        model->SetVolume(kCorrVolume * (kVolVsMult * kMultCharged[kMultClass] + kVolOffset));
        model->SetCanonicalVolume(kCorrVolume * (kVolVsMult * kMultCharged[kMultClass] + kVolOffset));
        model->ConserveBaryonCharge(true);
        model->ConserveElectricCharge(true);
        model->ConserveStrangeness(true);
        // Use quantum statistics
        model->SetStatistics(1);
        // Quantum numbers
        model->CalculateQuantumNumbersRange(true);
        // Resonance width
        model->SetUseWidth(ThermalParticle::eBW);
        // Set chemical potentials
        model->FillChemicalPotentials();

        // Config MC
        EventGeneratorConfiguration configMC;
        configMC.fModelType = EventGeneratorConfiguration::PointParticle;
        configMC.fEnsemble = EventGeneratorConfiguration::CE;
        configMC.B = model->Parameters().B;
        configMC.Q = model->Parameters().Q;
        configMC.S = model->Parameters().S;
        configMC.CFOParameters = model->Parameters();

        // Event generator
        CylindricalBlastWaveEventGenerator* generator = new CylindricalBlastWaveEventGenerator(model->TPS(), configMC);
        double betaS = (2. + n[kMultClass]) / 2. * beta_avg[kMultClass];
        std::cout << "betaS: " << betaS << std::endl;
        generator->SetParameters(T_kin[kMultClass], betaS, kCorrVolume * 0.5, n[kMultClass]);

        const int kNumberOfEvents = (kCentClasses[kMultClass + 1] - kCentClasses[kMultClass]) * nEventsPerPercent;

        for (int i = 0; i < kNumberOfEvents; ++i) {
            auto ev = generator->GetEvent(true);

            // Loop over generated particles
            for (auto p : ev.Particles) {
                const int absid = abs(p.PDGID);
                if (absid != 211 && absid != 321 && absid != 310 && absid != 2212 &&
                    absid != 3122 && absid != 3312 && absid != 333 && absid != 313 && absid != 3334)
                    continue;

                // Fill particle information
                particle.fPt = p.GetPt();
                particle.fEta = p.GetEta();
                particle.fPhi = TMath::ATan2(p.py, p.px);
                particle.fPDG = p.PDGID;
                particle.fCharge = TDatabasePDG::Instance()->GetParticle(p.PDGID)->Charge() / 3;

                // Fill the tree
                tree->Fill();
            }
        }

        // Clean up
        delete model;
        delete generator;
    }

    // Write the tree to the file
    outFile->cd();
    tree->Write();
    outFile->Close();

    std::cout << "ROOT file created: fist_output.root" << std::endl;
}

/*
int main() {
    GenFistPP();
    return 0;
}
    */