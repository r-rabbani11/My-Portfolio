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

//TRIAL PROGRAM FOR DNCH/DETA MULTIPLICITY CLASS AND YIELD RATIOS AGAIN
#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include <vector>
#include <cmath>
using namespace Pythia8;

bool isForwardTrack(const Particle& p) {
  if (!p.isCharged() || p.pT() < 0.1) return false;
  double eta = p.eta();
  return ((eta > -3.7 && eta < -1.7) || (eta > 2.8 && eta < 5.1));
}

bool isGoodTrack(const Particle& p) {
  if (!p.isFinal()) return false;
  if (p.pT() < 0.5) return false;
  if (abs(p.eta()) > 2.5) return false;
  return true;
}

int main() {
  // Initialize ROOT output
  TFile* outFile = new TFile("main442exp4_junctions.root", "RECREATE");
  TTree* tree = new TTree("tree", "Event tree for junction analysis");

  // Event-wise variables
  int nCharged, nPions, nKaons, nProtons, nLambdas, nXis, nOmegas, nForward;
  int nTotalJunctions, nXiJunctions, nPionJunctions;
  float eventWeight;
  
  // Tree branches
  tree->Branch("nCharged", &nCharged, "nCharged/I");
  tree->Branch("nPions", &nPions, "nPions/I");
  tree->Branch("nKaons", &nKaons, "nKaons/I");
  tree->Branch("nProtons", &nProtons, "nProtons/I");
  tree->Branch("nLambdas", &nLambdas, "nLambdas/I");
  tree->Branch("nXis", &nXis, "nXis/I");
  tree->Branch("nOmegas", &nOmegas, "nOmegas/I");
  tree->Branch("nForward", &nForward, "nForward/I");
  tree->Branch("nTotalJunctions", &nTotalJunctions, "nTotalJunctions/I");
  tree->Branch("nXiJunctions", &nXiJunctions, "nXiJunctions/I");
  tree->Branch("nPionJunctions", &nPionJunctions, "nPionJunctions/I");
  tree->Branch("eventWeight", &eventWeight, "eventWeight/F");

  // Histograms
  TH1D* hXiKKMass = new TH1D("hXiKKMass", "Xi+K+K invariant mass; Mass [GeV]; Counts", 100, 1.5, 5.0);
  TH1D* hPionKKMass = new TH1D("hPionKKMass", "#pi+K+K invariant mass; Mass [GeV]; Counts", 100, 0.5, 5.0);
  TH1D* hXiPt = new TH1D("hXiPt", "Xi pT distribution; pT [GeV]; Counts", 100, 0, 10);
  TH1D* hPionPt = new TH1D("hPionPt", "Pion pT distribution; pT [GeV]; Counts", 100, 0, 10);
  TH1D* hKaonPt = new TH1D("hKaonPt", "Kaon pT distribution; pT [GeV]; Counts", 100, 0, 10);
  TH2D* hXiKKvsMult = new TH2D("hXiKKvsMult", "XiKK vs multiplicity; N_{ch}; XiKK pairs", 100, 0, 200, 50, 0, 10);
  TH1D* hJunctionTypes = new TH1D("hJunctionTypes", "Types of junctions formed; Type; Counts", 5, 0, 5);

  // Pythia initialization
  Pythia pythia;
  
  // Basic collision setup
  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  pythia.readString("Beams:eCM = 13000.");
  pythia.readString("HardQCD:all = on");
  pythia.readString("SoftQCD:nonDiffractive = on");
  pythia.readString("PhaseSpace:pTHatMin = 10.");

  // Enhanced junction formation
  pythia.readString("ColourReconnection:mode = 1");
  pythia.readString("ColourReconnection:allowJunctions = on");
  pythia.readString("ColourReconnection:junctionCorrection = 1.5");
  pythia.readString("ColourReconnection:range = 2.0");

  // Strange and baryon enhancement
  pythia.readString("StringFlav:probStoUD = 0.3");
  pythia.readString("StringFlav:probQQtoQ = 0.1");
  pythia.readString("StringFlav:probSQtoQQ = 0.08");
  pythia.readString("StringZ:aLund = 0.68");
  pythia.readString("StringZ:bLund = 0.98");

  // Particle decays (keep stable for analysis)
  pythia.readString("310:mayDecay = off");  // K0s
  pythia.readString("3122:mayDecay = off"); // Lambda
  pythia.readString("3312:mayDecay = off"); // Xi
  pythia.readString("3334:mayDecay = off"); // Omega

  // Random seed
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = 42");

  if (!pythia.init()) {
    std::cerr << "Error in Pythia initialization!" << std::endl;
    return 1;
  }

  const int nEvents = 10000;
  const double etaCut = 2.5;
  const double pTCut = 0.5;

  // Event loop
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    if (!pythia.next()) continue;

    // Reset counters
    nCharged = nPions = nKaons = nProtons = nLambdas = nXis = nOmegas = nForward = 0;
    nTotalJunctions = nXiJunctions = nPionJunctions = 0;
    eventWeight = pythia.info.weight();

    Event& event = pythia.event;

    // First pass: count particles
    for (int i = 0; i < event.size(); ++i) {
      Particle& p = event[i];
      if (!p.isFinal()) continue;

      if (isForwardTrack(p)) nForward++;
      if (!isGoodTrack(p)) continue;

      int id = p.id();
      int absid = abs(id);
      
      if (p.isCharged()) nCharged++;
      if (absid == 211) nPions++;
      else if (absid == 321 || absid == 310) nKaons++;
      else if (absid == 2212) nProtons++;
      else if (absid == 3122) nLambdas++;
      else if (absid == 3312) nXis++;
      else if (absid == 3334) nOmegas++;
    }

    // Second pass: junction analysis
    for (int iJun = 0; iJun < event.sizeJunction(); ++iJun) {
      int kind = event.kindJunction(iJun);
      hJunctionTypes->Fill(kind);
      
      // Only consider baryon junctions (type 3)
      if (kind != 3) continue;
      nTotalJunctions++;

      // Get particles connected to this junction
      std::vector<int> junctionParts;
      for (int iEnd = 0; iEnd < 3; ++iEnd) {
        int col = event.colJunction(iJun, iEnd);
        for (int iPart = 0; iPart < event.size(); ++iPart) {
          Particle& p = event[iPart];
          if ((p.col() == col || p.acol() == col) && p.isFinal()) {
            junctionParts.push_back(iPart);
            break;
          }
        }
      }

      // Need exactly 3 particles to analyze
      if (junctionParts.size() != 3) continue;

      // Check particle content
      bool hasXi = false, hasPion = false;
      std::vector<int> kaonIndices;
      
      for (int idx : junctionParts) {
        Particle& p = event[idx];
        int absid = abs(p.id());
        
        if (absid == 3312) hasXi = true;
        else if (absid == 211) hasPion = true;
        else if (absid == 310 || absid == 321) kaonIndices.push_back(idx);
      }

      // Xi + K + K case
      if (hasXi && kaonIndices.size() >= 2) {
        nXiJunctions++;
        Vec4 pSum = event[junctionParts[0]].p();
        for (size_t i = 1; i < junctionParts.size(); ++i) 
          pSum += event[junctionParts[i]].p();
        hXiKKMass->Fill(pSum.mCalc());
        hXiKKvsMult->Fill(nCharged, 1);
      }

      // Pion + K + K case
      if (hasPion && kaonIndices.size() >= 2) {
        nPionJunctions++;
        Vec4 pSum = event[junctionParts[0]].p();
        for (size_t i = 1; i < junctionParts.size(); ++i)
          pSum += event[junctionParts[i]].p();
        hPionKKMass->Fill(pSum.mCalc());
      }
    }

    // Fill pT spectra
    for (int i = 0; i < event.size(); ++i) {
      Particle& p = event[i];
      if (!p.isFinal() || !isGoodTrack(p)) continue;
      
      int absid = abs(p.id());
      if (absid == 3312) hXiPt->Fill(p.pT());
      else if (absid == 211) hPionPt->Fill(p.pT());
      else if (absid == 310 || absid == 321) hKaonPt->Fill(p.pT());
    }

    tree->Fill();
    
    // Event progress
    if (iEvent % 1000 == 0) 
      std::cout << "Processed " << iEvent << " events" << std::endl;
  }

  // Save and close
  outFile->Write();
  outFile->Close();
  
  // Final statistics
  pythia.stat();
  return 0;
}


/*

#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TProfile.h"
#include <fstream>
#include <vector>
using namespace Pythia8;

// Function to check if a track is in the forward detectors
bool isForwardTrack(const Particle& p) {
  if (p.isHadron() && p.isCharged() && p.pT() > 0.1) {
    double eta = p.eta();
    if ((eta > -3.7 && eta < -1.7) || (eta > 2.8 && eta < 5.1)) return true;
  }
  return false;
}

// Function to find centrality percentiles from a histogram
std::vector<double> findPercentiles(TH1D* hCalib) {
  double totalIntegral = hCalib->Integral("width");
  std::vector<double> percentiles = {1., 5., 10., 15., 20., 30., 40., 50., 70., 100.};
  std::vector<double> xPercentiles;

  double cumulativeIntegral = 0;
  int nbins = hCalib->GetNbinsX();

  for (double p : percentiles) {
    double threshold = p / 100. * totalIntegral;
    for (int bin = nbins; bin > 0; --bin) {
      cumulativeIntegral = hCalib->Integral(bin, nbins, "width");
      if (cumulativeIntegral >= threshold) {
        double xValue = hCalib->GetBinCenter(bin);
        xPercentiles.push_back(xValue);
        break;
      }
    }
  }
  return xPercentiles;
}

int main() {
  // ROOT File and Tree
  TFile* outFile = new TFile("main442exp4.root", "RECREATE");
  TTree* tree = new TTree("tree", "tree");

  int nCharged, nPions, nKaons, nLambdas, nXis, nOmegas, nForward;
  tree->Branch("nCharged", &nCharged, "nCharged/I");
  tree->Branch("nPions", &nPions, "nPions/I");
  tree->Branch("nKaons", &nKaons, "nKaons/I");
  tree->Branch("nLambdas", &nLambdas, "nLambdas/I");
  tree->Branch("nXis", &nXis, "nXis/I");
  tree->Branch("nOmegas", &nOmegas, "nOmegas/I");
  tree->Branch("nForward", &nForward, "nForward/I");

  // Histograms
  TH1D* hForwardMult = new TH1D("hForwardMult", "Forward Multiplicity; N_{ch} (2.5 < |#eta| < 5); Events", 100, 0, 200);

  // Profiles for particle yields vs. centrality
  TProfile* pPion = new TProfile("pPion", "Pions; Centrality Class; N_{#pi}", 10, 0, 10);
  TProfile* pKaon = new TProfile("pKaon", "Kaons; Centrality Class; N_{K}", 10, 0, 10);
  TProfile* pLambda = new TProfile("pLambda", "Lambdas; Centrality Class; N_{#Lambda}", 10, 0, 10);
  TProfile* pXi = new TProfile("pXi", "Xis; Centrality Class; N_{#Xi}", 10, 0, 10);
  TProfile* pOmega = new TProfile("pOmega", "Omegas; Centrality Class; N_{#Omega}", 10, 0, 10);

  // Profiles for particle ratios vs. centrality
  TProfile* pKp = new TProfile("pKp", "Kaon / Pion; Centrality Class; K / #pi", 10, 0, 10);
  TProfile* pLp = new TProfile("pLp", "Lambda / Pion; Centrality Class; #Lambda / #pi", 10, 0, 10);
  TProfile* pXp = new TProfile("pXp", "Xi / Pion; Centrality Class; #Xi / #pi", 10, 0, 10);
  TProfile* pOp = new TProfile("pOp", "Omega / Pion; Centrality Class; #Omega / #pi", 10, 0, 10);

  // Generator initialization
  Pythia pythia;
  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  pythia.readString("Beams:eCM = 7000.");
  pythia.readString("SoftQCD:nonDiffractive = on");

  // Set relevant particles to be stable (ALICE convention)
  pythia.readString("310:onMode = off");
  pythia.readString("311:onMode = off");
  pythia.readString("3122:onMode = off");
  pythia.readString("3312:onMode = off");
  pythia.readString("3334:onMode = off");
  pythia.readString("3222:onMode = off");
  pythia.readString("3322:onMode = off");
  pythia.readString("3112:onMode = off");

  // Parameter of the MPI model to keep total multiplicity reasonable
  pythia.readString("MultiPartonInteractions:pT0Ref = 2.15");

  // Parameters related to Junction formation/QCD-based Color Reconnection (CR)
  pythia.readString("BeamRemnants:remnantMode = 1");
  pythia.readString("BeamRemnants:saturation = 5");
  pythia.readString("ColourReconnection:mode = 1");
  pythia.readString("ColourReconnection:allowDoubleJunRem = off");
  pythia.readString("ColourReconnection:m0 = 0.3");
  pythia.readString("ColourReconnection:allowJunctions = on");
  pythia.readString("ColourReconnection:junctionCorrection = 1.2");
  pythia.readString("ColourReconnection:timeDilationMode = 2");
  pythia.readString("ColourReconnection:timeDilationPar = 0.18");

  // Enable rope hadronization
  pythia.readString("Ropewalk:RopeHadronization = on");

  // Enable string shoving (without affecting dynamics)
  pythia.readString("Ropewalk:doShoving = on");
  pythia.readString("Ropewalk:tInit = 1.5");
  pythia.readString("Ropewalk:deltat = 0.05");
  pythia.readString("Ropewalk:tShove = 0.1");
  pythia.readString("Ropewalk:gAmplitude = 0.");

  // Enable rope flavor effects
  pythia.readString("Ropewalk:doFlavour = on");

  // Parameters of the rope model
  pythia.readString("Ropewalk:r0 = 0.5");
  pythia.readString("Ropewalk:m0 = 0.2");
  pythia.readString("Ropewalk:beta = 0.1");

  // Enable setting of vertex information
  pythia.readString("PartonVertex:setVertex = on");
  pythia.readString("PartonVertex:protonRadius = 0.7");
  pythia.readString("PartonVertex:emissionWidth = 0.1");


  if (!pythia.init()) return 1;

  // Event loop
  const int nEvent = 10000;

  // Store forward multiplicity and particle yields for each event
  std::vector<int> forwardMultiplicities;
  std::vector<int> pionYields;
  std::vector<int> kaonYields;
  std::vector<int> lambdaYields;
  std::vector<int> xiYields;
  std::vector<int> omegaYields;

  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) continue;

    nCharged = nPions = nKaons = nLambdas = nXis = nOmegas = nForward = 0;
    Event& event = pythia.event;

    // Loop over particles in the event
    for (int i = 0; i < event.size(); ++i) {
      Particle& p = event[i];
      if (p.isFinal() && p.pT() > 0.1) {
        if (isForwardTrack(p)) ++nForward;  // Forward multiplicity
        if (abs(p.eta()) < 2.5) {          // Midrapidity particles
          if (p.isCharged()) ++nCharged;
          int absid = abs(p.id());
          if (absid == 211) ++nPions;
          else if (absid == 310) ++nKaons;
          else if (absid == 3122) ++nLambdas;
          else if (absid == 3312) ++nXis;
          else if (absid == 3334) ++nOmegas;
        }
      }
    }

    //if (nCharged < 10) continue;

    // Store event information
    forwardMultiplicities.push_back(nForward);
    pionYields.push_back(nPions);
    kaonYields.push_back(nKaons);
    lambdaYields.push_back(nLambdas);
    xiYields.push_back(nXis);
    omegaYields.push_back(nOmegas);

    // Fill forward multiplicity histogram
    hForwardMult->Fill(nForward);

    // Fill tree
    tree->Fill();
  }

  // Normalize forward multiplicity histogram
  hForwardMult->Scale(1.0 / hForwardMult->Integral("width"));

  // Determine centrality percentiles
  std::vector<double> percentiles = findPercentiles(hForwardMult);

  // Fill centrality-based profiles
  for (size_t i = 0; i < forwardMultiplicities.size(); ++i) {
    nForward = forwardMultiplicities[i];
    double cIndex = 9.5;
    for (double p : percentiles) {
      if (double(nForward) > p) break;
      cIndex -= 1.0;
    }

    // Fill centrality-based profiles
    pPion->Fill(cIndex, pionYields[i]);
    pKaon->Fill(cIndex, kaonYields[i]);
    pLambda->Fill(cIndex, lambdaYields[i]);
    pXi->Fill(cIndex, xiYields[i]);
    pOmega->Fill(cIndex, omegaYields[i]);

    if (pionYields[i] > 0) {
      pKp->Fill(cIndex, double(kaonYields[i]) / pionYields[i]);
      pLp->Fill(cIndex, 2.0 * double(lambdaYields[i]) / pionYields[i]);  // Scale by 2
      pXp->Fill(cIndex, 6.0 * double(xiYields[i]) / pionYields[i]);      // Scale by 6
      pOp->Fill(cIndex, 16.0 * double(omegaYields[i]) / pionYields[i]);  // Scale by 16
    }
  }

  // Write output
  outFile->Write();
  outFile->Close();

  pythia.stat();
  return 0;
}

*/