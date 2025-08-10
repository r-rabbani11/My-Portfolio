// main442.cc is a part of the PYTHIA event generator.
// Copyright (C) 2025 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Authors: Christian Bierlich <christian.bierlich@fysik.lu.se>

// Keywords: hadronization; colour reconnection; rope hadronization

// This program provides a demonstration of the flavour rope model supplied
// in the Rope Hadronization framework. It produces four histograms
// showing the ratio of respectively K^0_s, Lambda_0, Cascade and Omega^-
// to pions as function of event multiplicity.
// No kind of Levy-Tsallis fitting, triggering or similar is done
// is this simple demonstration analysis. It should therefore not
// be taken as anything but a proof of concept.

#include "Pythia8/Pythia.h"
using namespace Pythia8;
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TLegend.h"

int main() {
    // Output file
    TFile *outFile = new TFile("main442exp4.root", "RECREATE");

    // Tree to store event-wise information
    TTree *tree = new TTree("tree", "tree");

    // Variables for event-wise information
    int nCharged, nPions, nKaons, nLambdas, nXis, nOmegas;
    float dNch_dEta; // Charged particle density in |eta| < 0.5
    int multiplicityClass;

    tree->Branch("nCharged", &nCharged, "nCharged/I");
    tree->Branch("nPions", &nPions, "nPions/I");
    tree->Branch("nKaons", &nKaons, "nKaons/I");
    tree->Branch("nLambdas", &nLambdas, "nLambdas/I");
    tree->Branch("nXis", &nXis, "nXis/I");
    tree->Branch("nOmegas", &nOmegas, "nOmegas/I");
    tree->Branch("dNch_dEta", &dNch_dEta, "dNch_dEta/F");
    tree->Branch("multiplicityClass", &multiplicityClass, "multiplicityClass/I");

    // Histograms for particle yields
    TH1F *hPion = new TH1F("hPion", "pions (mult)", 50, 10., 135.);
    TH1F *hKaon = new TH1F("hKaon", "kaons (mult)", 50, 10., 135.);
    TH1F *hLambda = new TH1F("hLambda", "lambdas (mult)", 50, 10., 135.);
    TH1F *hXi = new TH1F("hXi", "xi (mult)", 50, 10., 135.);
    TH1F *hOmega = new TH1F("hOmega", "omega (mult)", 50, 10., 135.);

    // Histograms for yield ratios
    TH1F *hKp = new TH1F("hKp", "kaon / pion (multiplicity)", 50, 10., 135.);
    TH1F *hLp = new TH1F("hLp", "lambda / pion (multiplicity)", 50, 10., 135.);
    TH1F *hXp = new TH1F("hXp", "xi / pion (multiplicity)", 50, 10., 135.);
    TH1F *hOp = new TH1F("hOp", "omega / pion (multiplicity)", 50, 10., 135.);

    // Initialize PYTHIA
    Pythia pythia;
    pythia.readString("Beams:eCM = 7000.");
    pythia.readString("SoftQCD:nonDiffractive = on");
    pythia.readString("Ropewalk:RopeHadronization = on");
    pythia.readString("Ropewalk:doShoving = off");
    pythia.readString("Ropewalk:doFlavour = on");
    pythia.readString("Ropewalk:r0 = 0.5");
    pythia.readString("Ropewalk:m0 = 0.2");
    pythia.readString("Ropewalk:beta = 0.1");
    pythia.readString("PartonVertex:setVertex = on");
    pythia.readString("ParticleDecays:limitTau0 = on");
    pythia.readString("ParticleDecays:tau0Max = 10");
    if (!pythia.init()) return 1;

    // Multiplicity classes (based on dNch/dEta)
    const int nClasses = 10;
    float dNch_dEtaLimits[nClasses + 1] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50};

    // Arrays to store yields for each multiplicity class
    float yieldPions[nClasses] = {0};
    float yieldKaons[nClasses] = {0};
    float yieldLambdas[nClasses] = {0};
    float yieldXis[nClasses] = {0};
    float yieldOmegas[nClasses] = {0};

    // Event loop
    const int nEvent = 10000;
    for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
        if (!pythia.next()) continue;

        nCharged = nPions = nKaons = nLambdas = nXis = nOmegas = 0;
        dNch_dEta = 0;
        multiplicityClass = -1;

        Event& event = pythia.event;
        for (int i = 0; i < event.size(); ++i) {
            Particle& p = event[i];
            if (p.isFinal() && p.isCharged() && abs(p.eta()) < 0.5) {
                dNch_dEta++;
                if (abs(p.eta()) < 2.5 && p.pT() > 0.1) {
                    nCharged++;
                    int absid = abs(p.id());
                    if (absid == 211) nPions++;
                    else if (absid == 310) nKaons++;
                    else if (absid == 3122) nLambdas++;
                    else if (absid == 3312) nXis++;
                    else if (absid == 3334) nOmegas++;
                }
            }
        }

        // Normalize dNch_dEta to the eta range
        dNch_dEta /= 1.0; // |eta| < 0.5 corresponds to delta_eta = 1.0

        // Determine multiplicity class
        for (int iClass = 0; iClass < nClasses; ++iClass) {
            if (dNch_dEta >= dNch_dEtaLimits[iClass] && dNch_dEta < dNch_dEtaLimits[iClass + 1]) {
                multiplicityClass = iClass;
                break;
            }
        }

        if (multiplicityClass == -1 || nCharged < 10) continue;

        // Fill yields for each multiplicity class
        yieldPions[multiplicityClass] += nPions;
        yieldKaons[multiplicityClass] += nKaons;
        yieldLambdas[multiplicityClass] += nLambdas;
        yieldXis[multiplicityClass] += nXis;
        yieldOmegas[multiplicityClass] += nOmegas;

        // Fill histograms
        hPion->Fill(nCharged, nPions);
        hKaon->Fill(nCharged, nKaons);
        hLambda->Fill(nCharged, nLambdas);
        hXi->Fill(nCharged, nXis);
        hOmega->Fill(nCharged, nOmegas);

        if (nPions > 0) {
            hKp->Fill(nCharged, double(nKaons) / double(nPions));
            hLp->Fill(nCharged, 2.0 * double(nLambdas) / double(nPions));
            hXp->Fill(nCharged, 6.0 * double(nXis) / double(nPions));
            hOp->Fill(nCharged, 16.0 * double(nOmegas) / double(nPions));
        }

        tree->Fill();
    }

    // Calculate yield ratios and plot
    TGraphErrors *gKp = new TGraphErrors(nClasses);
    TGraphErrors *gLp = new TGraphErrors(nClasses);
    TGraphErrors *gXp = new TGraphErrors(nClasses);
    TGraphErrors *gOp = new TGraphErrors(nClasses);

    for (int iClass = 0; iClass < nClasses; ++iClass) {
        float dNch_dEtaAvg = 0.5 * (dNch_dEtaLimits[iClass] + dNch_dEtaLimits[iClass + 1]);
        float ratioKp = (yieldPions[iClass] > 0) ? yieldKaons[iClass] / yieldPions[iClass] : 0;
        float ratioLp = (yieldPions[iClass] > 0) ? yieldLambdas[iClass] / yieldPions[iClass] : 0;
        float ratioXp = (yieldPions[iClass] > 0) ? yieldXis[iClass] / yieldPions[iClass] : 0;
        float ratioOp = (yieldPions[iClass] > 0) ? yieldOmegas[iClass] / yieldPions[iClass] : 0;

        gKp->SetPoint(iClass, dNch_dEtaAvg, ratioKp);
        gLp->SetPoint(iClass, dNch_dEtaAvg, ratioLp);
        gXp->SetPoint(iClass, dNch_dEtaAvg, ratioXp);
        gOp->SetPoint(iClass, dNch_dEtaAvg, ratioOp);
    }

    // Plot the results
    TCanvas *c1 = new TCanvas("c1", "Strangeness Enhancement", 800, 600);
    gKp->SetTitle("Strangeness Enhancement;dN_{ch}/d#eta;Yield Ratio");
    gKp->SetMarkerStyle(20);
    gKp->SetMarkerColor(kRed);
    gLp->SetMarkerStyle(21);
    gLp->SetMarkerColor(kBlue);
    gXp->SetMarkerStyle(22);
    gXp->SetMarkerColor(kGreen);
    gOp->SetMarkerStyle(23);
    gOp->SetMarkerColor(kMagenta);

    gKp->Draw("AP");
    gLp->Draw("P SAME");
    gXp->Draw("P SAME");
    gOp->Draw("P SAME");

    TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg->AddEntry(gKp, "K/#pi", "p");
    leg->AddEntry(gLp, "#Lambda/#pi", "p");
    leg->AddEntry(gXp, "#Xi/#pi", "p");
    leg->AddEntry(gOp, "#Omega/#pi", "p");
    leg->Draw();

    c1->Write();

    // Write and close the output file
    outFile->Write();
    outFile->Close();

    pythia.stat();
    return 0;
}