
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

// Usage from a ROOT prompt:
//
//   .L ../libRootEvent.so
//   root [0] .L analysis.C+
//   root [1] read({"pythia.root"})
//   root [2] read({"pythia1.root", "pythia2.root", ...})
//

#include "TLegend.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TChain.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TLine.h"
#include <iostream>
#include <vector>
#include <string>
#include "RootEvent.h"
#include "MyEvent.h"
#include "MyParticle.h"
#include "TClonesArray.h"


// Test whether or not a track sits in the forward detectors
bool isForwardTrack(RootTrack& trk) {
  if (trk.isHadron && trk.isCharged && trk.pT > 0.1) 
    if ((trk.eta > -3.7 && trk.eta < -1.7) || 
      (trk.eta > 2.8 && trk.eta < 5.1)) return true;
  return false;
}

// Find centrality percentiles of the centrality histogram
std::vector<double> findPercentiles(TH1D* hCalib) {
  double totalIntegral = hCalib->Integral("width");
  //vector<double> percentiles = {1.,5.,10.,15.,20., 30., 40., 50., 70., 100.};
  vector<double> percentiles = {0.95, 4.7, 9.5, 14., 19., 28., 38., 48., 68., 100.};
  vector<double> xPercentiles;

  double cumulativeIntegral = 0;
  int nbins = hCalib->GetNbinsX();
  
  for (double p : percentiles) {
    double threshold = p/100. * totalIntegral;
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

// Read one or more files, loop over events, make calibration hist. Save to
// a new root file.
// Function to add files matching a pattern to a TChain
void addFilesToChain(TChain& chain, const std::string& directory, const std::string& pattern) {
    void* dir = gSystem->OpenDirectory(directory.c_str());
    if (!dir) {
        std::cerr << "Error: Could not open directory " << directory << std::endl;
        return;
    }

    const char* file;
    while ((file = gSystem->GetDirEntry(dir))) {
        std::string filename(file);
        if (filename.find(pattern) != std::string::npos) {
            chain.Add((directory + "/" + filename).c_str());
        }
    }
    gSystem->FreeDirectory(dir);
}

void calibration(const std::string& pythiaFiles = "../pythiarun") {

  // Create a TChain and add the files
  TChain pythiaTree("t");
  /*for (auto &fn : filenames) {
    tree.Add(fn.c_str());
  }*/
  addFilesToChain(pythiaTree, pythiaFiles, "pythiarun");
    
  // Create a calibration histogram 
  TH1D hCalib("hCalib", "nFwd", 100, 0, 200);
  
  RootEvent* evt = nullptr;
  pythiaTree.SetBranchAddress("events", &evt);

  // Loop over entries
  Long64_t nEntries = pythiaTree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    pythiaTree.GetEntry(i);
    
    int nFwd = 0;
    // Fill histogram for each track
    for (auto &trk : evt->tracks)
      if (isForwardTrack(trk)) ++nFwd;
    hCalib.Fill(nFwd);
  }
  // Normalize the histogram
  hCalib.Scale(1.0/hCalib.Integral("width"));
  // Save histogram to a new ROOT file
  TFile outFile("calibration.root", "RECREATE");
  hCalib.Write();
  outFile.Close();
  delete evt;
  
}

void drawCalibration(TH1D* hCalib) {
  auto percentiles = findPercentiles(hCalib);
  
// Create canvas
  TCanvas* c1 = new TCanvas("c1", "Forward Multiplicity Calibration", 1700, 1000);
  c1->SetLogy();
  //c1->SetGrid();

  // Set histogram title and axis labels
  hCalib->SetTitle("Forward Multiplicity Distribution with Centrality Bins");
  hCalib->GetXaxis()->SetTitle("Forward Multiplicity (nFwd)");
  hCalib->GetYaxis()->SetTitle("Forward Tracks");
  hCalib->SetLineColor(kBlue+2);
  hCalib->SetLineWidth(2);
  hCalib->Draw();

  c1->SetLogy();

  // Draw percentile lines
  for (double p : percentiles) {
    TLine* l = new TLine(p, 0, p, hCalib->GetBinContent(hCalib->FindBin(p)));
    l->SetLineColor(kBlack);
    l->SetLineStyle(1);  // dashed
    l->Draw();
  }

  // Add legend for percentile lines
  TLegend* leg = new TLegend(0.6, 0.75, 0.88, 0.88);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hCalib, "Forward multiplicity", "l");
  leg->AddEntry((TObject*)0, "Vertical lines = centrality bins", "");
  leg->Draw();

  // Save to file
  c1->SaveAs("/mnt/c/MoveToWSL/calibration_plot.png");
}


//Function to read Thermal-FIST data

void readFISTData(const std::string& filename, TProfile* hPiCent, TProfile* hPCent, TProfile* hKCent, TProfile* hLCent, TProfile* hXiCent, TProfile* hOmegaCent) {
  TFile* fistFile = TFile::Open(filename.c_str(), "READ");
  if (!fistFile || fistFile->IsZombie()) {
      std::cerr << "Error: Could not open file " << filename << std::endl;
      return;
  }

  TTree* tree = fistFile->Get<TTree>("TT");
  if (!tree) {
      std::cerr << "Error: Could not find tree 'TT' in file " << filename << std::endl;
      fistFile->Close();
      return;
  }

  TClonesArray* trackArray = nullptr;
  MyEvent* event = nullptr;
  tree->SetBranchAddress("tracks", &trackArray);
  tree->SetBranchAddress("event", &event);

  // Define centrality classes
  constexpr int nMultClasses = 10;
  constexpr double kCentClasses[nMultClasses + 1] = {0., 0.95, 4.7, 9.5, 14., 19., 28., 38., 48., 68., 100.};

  Long64_t nEntries = tree->GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
      tree->GetEntry(i);

      int nPi = 0, nP = 0, nK = 0, nL = 0, nXi = 0, nO = 0;

      // Count particles
      for (Int_t j = 0; j < trackArray->GetEntriesFast(); j++) {
          MyParticle* track = (MyParticle*)trackArray->At(j);
          int id = abs(track->fPDG);

          if (id == 211) ++nPi;          // Pion
          else if (id == 2212) ++nP;     // Proton
          else if (id == 311) ++nK;      // K0S (we need 310)
          else if (id == 3122) ++nL;     // Lambda
          else if (id == 3312) ++nXi;    // Xi
          else if (id == 3334) ++nO;     // Omega
      }

      // Map the random centrality value to a bin index
      double centrality = event->fV0A; // centrality value
      int binIndex = 0;
      for (int k = 0; k < nMultClasses; k++) {
        if (centrality >= kCentClasses[k] && centrality < kCentClasses[k + 1]) {
            binIndex = nMultClasses - 1 - k; // Reverse the order
            break;
        }
      }

      // Fill profiles with the bin index (0 to 9)
      hPiCent->Fill(binIndex, nPi);
      hPCent->Fill(binIndex, nP);
      hKCent->Fill(binIndex, nK);
      hLCent->Fill(binIndex, nL);
      hXiCent->Fill(binIndex, nXi);
      hOmegaCent->Fill(binIndex, nO);
  }

  fistFile->Close();
}


vector<string> anaFiles = {"../firstdata.root"};

void analyze(/*vector<string> pythiaFiles = {"../pythia.root"}*/const std::string& pythiaFiles = "../pythiarun", const std::string& fistDirectory = "/home/rrabbani11/fist_data") {
  
  TFile calibFile("calibration.root", "READ");
  TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
  auto percentiles = findPercentiles(hCalib);
  delete hCalib;

  // Create profiles for PYTHIA data
  TProfile hPiCent("hPiCent", "nPi", 10, 0, 10);
  TProfile hPCent("hPCent", "nP", 10, 0, 10);
  TProfile hKCent("hKCent", "nK", 10, 0, 10);
  TProfile hLCent("hLCent", "nL", 10, 0, 10);
  TProfile hXiCent("hXCent", "nXi", 10, 0, 10);
  TProfile hOmegaCent("hOmegaCent", "nOmega", 10, 0, 10);

  // Create profiles for Thermal-FIST data
  TProfile hPiCentFIST("hPiCentFIST", "nPi FIST", 10, 0, 10);
  TProfile hPCentFIST("hPCentFIST", "nP FIST", 10, 0, 10);
  TProfile hKCentFIST("hKCentFIST", "nK FIST", 10, 0, 10);
  TProfile hLCentFIST("hLCentFIST", "nL FIST", 10, 0, 10);
  TProfile hXiCentFIST("hXCentFIST", "nXi FIST", 10, 0, 10);
  TProfile hOmegaCentFIST("hOmegaCentFIST", "nOmega FIST", 10, 0, 10);

  // Process PYTHIA data
  TChain pythiaTree("t");
  /*for (auto &fn : pythiaFiles) {
      pythiaTree.Add(fn.c_str());
  }*/
  addFilesToChain(pythiaTree, pythiaFiles, "pythiarun");

  RootEvent* evt = nullptr;
  pythiaTree.SetBranchAddress("events", &evt);

  Long64_t nEntries = pythiaTree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
      pythiaTree.GetEntry(i);

      int nFwd = 0, nPi = 0, nP = 0, nK = 0, nL = 0, nXi = 0, nO = 0;

      for (auto &trk : evt->tracks) {
          if (isForwardTrack(trk)) ++nFwd;
          else if (abs(trk.y) < 0.5) {
              int id = abs(trk.pid);
              if (id == 211) ++nPi;
              else if (id == 2212) ++nP;
              else if (id == 310) ++nK;
              else if (id == 3122) ++nL;
              else if (id == 3312) ++nXi;
              else if (id == 3334) ++nO;
          }
      }

      double cIndex = 9.5;
      for (auto p : percentiles) {
          if (double(nFwd) > p) break;
          cIndex -= 1.0;
      }

      hPiCent.Fill(cIndex, nPi);
      hPCent.Fill(cIndex, nP);
      hKCent.Fill(cIndex, nK);
      hLCent.Fill(cIndex, nL);
      hXiCent.Fill(cIndex, nXi);
      hOmegaCent.Fill(cIndex, nO);
  }

  
  // Process Thermal-FIST data
  /*TChain fistTree("TT"); 
  for (const auto& file : fistFiles) {
      fistTree.Add(file.c_str());
  }*/
  // Process Thermal-FIST data
  TChain fistTree("TT");
  addFilesToChain(fistTree, fistDirectory, "fist_data-");


  TClonesArray* trackArray = nullptr;
  MyEvent* event = nullptr;
  fistTree.SetBranchAddress("tracks", &trackArray);
  fistTree.SetBranchAddress("event", &event);

  // Define centrality classes (same as in FISTOnly.C)
  const Int_t nMultClasses = 10;
  constexpr double kCentClasses[nMultClasses+1] = {0.,    0.95, 4.7, 9.5, 14., 19., 28., 38., 48., 68., 100.};
  constexpr double kMultCharged[nMultClasses]   = {26.0,  20.0,  16.2,  13.75, 10.0,  8.0,   6.3,   4.5,   2.5};
  constexpr double beta_avg[nMultClasses]       = {0.488, 0.44,  0.4,   0.378, 0.325, 0.287, 0.25,  0.20,  0.11};
  constexpr double T_kin[nMultClasses]          = {0.163, 0.174, 0.180, 0.181, 0.184, 0.184, 0.183, 0.181, 0.173};
  constexpr double n[nMultClasses]              = {1.47,  1.70,  2.01,  2.25,  2.89,  3.48,  4.2,   5.71,  11.6};

  Long64_t nFistEntries = fistTree.GetEntries();
  for (Long64_t i = 0; i < nFistEntries; i++) {
      fistTree.GetEntry(i);

      int nPi = 0, nP = 0, nK = 0, nL = 0, nXi = 0, nO = 0;

      // Count particles
      for (int j = 0; j < trackArray->GetEntriesFast(); j++) {
          MyParticle* track = (MyParticle*)trackArray->At(j);
          if (abs(track->Y()) < 0.5) {
              int id = abs(track->fPDG);
              if (id == 211) ++nPi;          // Pion
              else if (id == 2212) ++nP;     // Proton
              else if (id == 311) ++nK;      // K0S (310 needs to be checked)
              else if (id == 3122) ++nL;     // Lambda
              else if (id == 3312) ++nXi;    // Xi
              else if (id == 3334) ++nO;     // Omega
          }
      }

      // Map the random centrality value to a bin index
      double centrality = event->fV0A; // Random centrality value
      int binIndex = 0;
      for (int k = 0; k < nMultClasses; k++) {
        if (centrality >= kCentClasses[k] && centrality < kCentClasses[k + 1]) {
            binIndex = nMultClasses - 1 - k; // Reverse the order
            break;
        }
      }

      // Fill profiles with the bin index (0 to 9)
      hPiCentFIST.Fill(binIndex, nPi);
      hPCentFIST.Fill(binIndex, nP);
      hKCentFIST.Fill(binIndex, nK);
      hLCentFIST.Fill(binIndex, nL);
      hXiCentFIST.Fill(binIndex, nXi);
      hOmegaCentFIST.Fill(binIndex, nO);
  }

  // Save results to a ROOT file
  TFile outFile("yields.root", "RECREATE");
  hPiCent.Write();
  hPCent.Write();
  hKCent.Write();
  hLCent.Write();
  hXiCent.Write();
  hOmegaCent.Write();

  hPiCentFIST.Write();
  hPCentFIST.Write();
  hKCentFIST.Write();
  hLCentFIST.Write();
  hXiCentFIST.Write();
  hOmegaCentFIST.Write();

  outFile.Close();
  delete evt;
}

// Function to analyze K/pi ratio within Xi-Xi-bar productions
// Function to analyze K/pi ratio within Xi-Xi-bar productions
void analyzek2pi(const std::string& pythiaFiles = "../pythiarun", const std::string& fistDirectory = "/home/rrabbani11/fist_data") {
    // Open the calibration file
    TFile calibFile("calibration.root", "READ");
    TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
    auto percentiles = findPercentiles(hCalib);
    delete hCalib;

    // Create profiles for PYTHIA data
    TProfile hKFromXiCent("hKFromXiCent", "nK from Xi", 10, 0, 10);
    TProfile hPiFromXiCent("hPiFromXiCent", "nPi from Xi", 10, 0, 10);
    
    // Create profiles for Thermal-FIST data
    TProfile hKFromXiCentFIST("hKFromXiCentFIST", "nK from Xi FIST", 10, 0, 10);
    TProfile hPiFromXiCentFIST("hPiFromXiCentFIST", "nPi from Xi FIST", 10, 0, 10);

    // Process PYTHIA data
    TChain pythiaTree("t");
    /*for (auto &fn : pythiaFiles) {
        pythiaTree.Add(fn.c_str());
    }*/
    addFilesToChain(pythiaTree, pythiaFiles, "pythiarun");

    RootEvent* evt = nullptr;
    pythiaTree.SetBranchAddress("events", &evt);

    Long64_t nEntries = pythiaTree.GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        pythiaTree.GetEntry(i);

        // Check if event has Ξ-Ξ̄ pair
        bool hasXi = false, hasXibar = false;
        for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue;
            if (trk.pid == 3312)  hasXi = true;
            else if (trk.pid == -3312) hasXibar = true;
            if (hasXi && hasXibar) break;
        }
        if (!hasXi || !hasXibar) continue;

        int nFwd = 0, nKFromXi = 0, nPiFromXi = 0;

        for (auto &trk : evt->tracks) {
            if (isForwardTrack(trk)) nFwd++;
        }

        // Count kaons and pions
        for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue; // theoretical rapidity cut
            if (abs(trk.pid) == 310) nKFromXi++;    // Kₛ⁰
            else if (abs(trk.pid) == 211) nPiFromXi++; // π±
        }

        // Map nFwd to centrality bin 
        double cIndex = 9.5;
        for (auto p : percentiles) {
            if (double(nFwd) > p) break;
            cIndex -= 1.0;
        }


        hKFromXiCent.Fill(cIndex, nKFromXi);
        hPiFromXiCent.Fill(cIndex, nPiFromXi);
    }

    // Process Thermal-FIST data
    /*TChain fistTree("TT"); 
    for (const auto& file : fistFiles) {
        fistTree.Add(file.c_str());
    }*/
    TChain fistTree("TT");
    addFilesToChain(fistTree, fistDirectory, "fist_data-");


    TClonesArray* trackArray = nullptr;
    MyEvent* event = nullptr;
    fistTree.SetBranchAddress("tracks", &trackArray);
    fistTree.SetBranchAddress("event", &event);
    

    // Define centrality classes (same as in FISTOnly.C)
    const Int_t nMultClasses = 10;
    constexpr double kCentClasses[nMultClasses+1] = {0., 0.95, 4.7, 9.5, 14., 19., 28., 38., 48., 68., 100.};

    Long64_t nFistEntries = fistTree.GetEntries();
    for (Long64_t i = 0; i < nFistEntries; i++) {
        fistTree.GetEntry(i);

        // Count kaons and pions originating from Ξ particles
        bool hasXi = false, hasXibar = false;
        for (int j = 0; j < trackArray->GetEntriesFast(); j++) {
            MyParticle* track = (MyParticle*)trackArray->At(j);
            if (abs(track->fEta) > 1) continue; // Rapidity cut
            if (track->fPDG == 3312) hasXi = true;       // Ξ⁻
            else if (track->fPDG == -3312) hasXibar = true; // Ξ̄⁺
            if (hasXi && hasXibar) break;
        }
        if (!hasXi || !hasXibar) continue; // Skip events without Ξ-Ξ̄ pairs

        int nKFromXiFIST = 0, nPiFromXiFIST = 0;
        for (int j = 0; j < trackArray->GetEntriesFast(); j++) {
            MyParticle* track = (MyParticle*)trackArray->At(j);
            if (abs(track->fEta) > 1) continue; // Rapidity cut
            int id = abs(track->fPDG);
            if (id == 311) nKFromXiFIST++;    // Kₛ⁰
            else if (id == 211) nPiFromXiFIST++; // π±
        }

        // Map centrality to bin index (in increasing order)
        double centrality = event->fV0A; // Random centrality value
        int binIndex = 0;
        for (int k = 0; k < nMultClasses; k++) {
            if (centrality >= kCentClasses[k] && centrality < kCentClasses[k + 1]) {
                binIndex = nMultClasses - 1 - k; 
                break;
            }
        }

        hKFromXiCentFIST.Fill(binIndex, nKFromXiFIST);
        hPiFromXiCentFIST.Fill(binIndex, nPiFromXiFIST);
    }

    // Save results to a ROOT file
    TFile outFile("k2pi_xi_yields.root", "RECREATE");
    hKFromXiCent.Write();
    hPiFromXiCent.Write();
    hKFromXiCentFIST.Write();
    hPiFromXiCentFIST.Write();
    outFile.Close();
    delete evt;
}

void analyzexixibar(const std::string& pythiaFiles = "../pythiarun") {
    // Open the calibration file
    TFile calibFile("calibration.root", "READ");
    TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
    auto percentiles = findPercentiles(hCalib);
    delete hCalib;

    // Create profiles for PYTHIA data
    TProfile hKPlusCent("hKPlusCent", "nK+ from Xi", 10, 0, 10);
    TProfile hPiPlusCent("hPiPlusCent", "nPi+ from Xi", 10, 0, 10);
    TProfile hKMinusCent("hKMinusCent", "nK- from Xibar", 10, 0, 10);
    TProfile hPiMinusCent("hPiMinusCent", "nPi- from Xibar", 10, 0, 10);

    // Process PYTHIA data
    TChain pythiaTree("t");
    /*for (auto &fn : pythiaFiles) {
        pythiaTree.Add(fn.c_str());
    }*/
    addFilesToChain(pythiaTree, pythiaFiles, "pythiarun");

    RootEvent* evt = nullptr;
    pythiaTree.SetBranchAddress("events", &evt);

    Long64_t nEntries = pythiaTree.GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        pythiaTree.GetEntry(i);

        int nFwd = 0;  // Number of forward tracks
        int nKPlus = 0; // Number of K+
        int nKMinus = 0; // Number of K-
        int nPiPlus = 0; // Number of pi+
        int nPiMinus = 0;  // Number of pi-

        // Check if event has Ξ-Ξ̄ pair
        bool hasXi = false, hasXibar = false;
        
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue;
            if (trk.pid == 3312)  hasXi = true;
            else if (trk.pid == -3312) hasXibar = true;
            if (hasXi && hasXibar) break;
        }*/

        // eta of xi and xibar.
        double etaXi = 0, etaXiBar = 0;

        for (auto& trk : evt->tracks) {
          // make sure you are only taking tracks within
          // |eta| < 1.
          if (abs(trk.eta) > 1) continue;
          if (trk.pid == 3312) {
            hasXi = true;
            double etaXi = trk.eta;
          }
          else if (trk.pid == -3312) {
            hasXibar = true;
            double etaXiBar = trk.eta;
          }
          //if (hasXi && hasXibar) break;
        }


        if (!hasXi || !hasXibar) continue;

        /*for (auto &trk : evt->tracks) {
            if (isForwardTrack(trk)) nFwd++;
        }*/

        // Count kaons and pions
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue; // theoretical rapidity cut
            if ((trk.pid) == 321) nKFromXi++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == 211) nPiFromXi++; // π-
            else if ((trk.pid) == -321) nKFromXibar++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == -211) nPiFromXibar++; // π+
        }*/

        // Loop over all particles again, accumulate forward
        // particles for centrality calibration and find particles of interest.
        // These are:

        // K+ from Xi, K- from Xibar, pi+ from Xi, pi- from Xibar
        for (auto &trk : evt->tracks) {
            // First we do the calib
            if (isForwardTrack(trk)) nFwd++;
            // Our other particles of interest needs to be within |eta| < 1 as well.
            if (abs(trk.eta) > 1) continue;
            // Do the pions
            if (abs(trk.pid) == 211 && abs(trk.eta - etaXi) < 0.2)
                ++nPiPlus; // Only count pi+ within 0.2 of the Xi.
            if (abs(trk.pid) == 211 && abs(trk.eta - etaXiBar) < 0.2)
                ++nPiMinus; // Only count pi- within 0.2 of the XiBar.
            // Do the kaons
            if (trk.pid == 321 && abs(trk.eta - etaXi) < 0.2)
                ++nKPlus; // Only count K+ within 0.2 of the Xi.
            if (trk.pid == -321 && abs(trk.eta - etaXiBar) < 0.2)
                ++nKMinus; // Only count K- within 0.2 of the XiBar.
        }
                // Map nFwd to centrality bin 
        double cIndex = 9.5;
        for (auto p : percentiles) {
            if (double(nFwd) > p) break;
            cIndex -= 1.0;
        }


        hKPlusCent.Fill(cIndex, nKPlus);
        hPiPlusCent.Fill(cIndex, nPiPlus);
        hKMinusCent.Fill(cIndex, nKMinus);
        hPiMinusCent.Fill(cIndex, nPiMinus);
    }
    // Save results to a ROOT file
    TFile outFile("k2pi_xixibar_yields.root", "RECREATE");
    hKPlusCent.Write();
    hPiPlusCent.Write();
    hKMinusCent.Write();
    hPiMinusCent.Write();


    outFile.Close();
    delete evt;
}


void analyzexixibar01(const std::string& pythiaFiles = "../pythiarun") {
    // Open the calibration file
    TFile calibFile("calibration.root", "READ");
    TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
    auto percentiles = findPercentiles(hCalib);
    delete hCalib;

    // Create profiles for PYTHIA data
    TProfile hKPlusCent("hKPlusCent", "nK+ from Xi", 10, 0, 10);
    TProfile hPiPlusCent("hPiPlusCent", "nPi+ from Xi", 10, 0, 10);
    TProfile hKMinusCent("hKMinusCent", "nK- from Xibar", 10, 0, 10);
    TProfile hPiMinusCent("hPiMinusCent", "nPi- from Xibar", 10, 0, 10);

    // Process PYTHIA data
    TChain pythiaTree("t");
    /*for (auto &fn : pythiaFiles) {
        pythiaTree.Add(fn.c_str());
    }*/
    addFilesToChain(pythiaTree, pythiaFiles, "pythiarun");

    RootEvent* evt = nullptr;
    pythiaTree.SetBranchAddress("events", &evt);

    Long64_t nEntries = pythiaTree.GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        pythiaTree.GetEntry(i);

        int nFwd = 0;  // Number of forward tracks
        int nKPlus = 0; // Number of K+
        int nKMinus = 0; // Number of K-
        int nPiPlus = 0; // Number of pi+
        int nPiMinus = 0; // Number of pi-

        // Check if event has Ξ-Ξ̄ pair
        bool hasXi = false, hasXibar = false;
        
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue;
            if (trk.pid == 3312)  hasXi = true;
            else if (trk.pid == -3312) hasXibar = true;
            if (hasXi && hasXibar) break;
        }*/

        // eta of xi and xibar.
        double etaXi = 0, etaXiBar = 0;

        for (auto& trk : evt->tracks) {
          // make sure you are only taking tracks within
          // |eta| < 1.
          if (abs(trk.eta) > 1) continue;
          if (trk.pid == 3312) {
            hasXi = true;
            double etaXi = trk.eta;
          }
          else if (trk.pid == -3312) {
            hasXibar = true;
            double etaXiBar = trk.eta;
          }
          //if (hasXi && hasXibar) break;
        }


        if (!hasXi || !hasXibar) continue;

        /*for (auto &trk : evt->tracks) {
            if (isForwardTrack(trk)) nFwd++;
        }*/

        // Count kaons and pions
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue; // theoretical rapidity cut
            if ((trk.pid) == 321) nKFromXi++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == 211) nPiFromXi++; // π-
            else if ((trk.pid) == -321) nKFromXibar++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == -211) nPiFromXibar++; // π+
        }*/

        // Loop over all particles again, accumulate forward
        // particles for centrality calibration and find particles of interest.
        // These are:

        // K+ from Xi, K- from Xibar, pi+ from Xi, pi- from Xibar
        for (auto &trk : evt->tracks) {
            // First we do the calib
            if (isForwardTrack(trk)) nFwd++;
            // Our other particles of interest needs to be within |eta| < 1 as well.
            if (abs(trk.eta) > 1) continue;
            // Do the pions
            if (abs(trk.pid) == 211 && abs(trk.eta - etaXi) < 0.1)
                ++nPiPlus; // Only count pi+ within 0.2 of the Xi.
            if (abs(trk.pid) == 211 && abs(trk.eta - etaXiBar) < 0.1)
                ++nPiMinus; // Only count pi- within 0.2 of the XiBar.
            // Do the kaons
            if (trk.pid == 321 && abs(trk.eta - etaXi) < 0.1)
                ++nKPlus; // Only count K+ within 0.2 of the Xi.
            if (trk.pid == -321 && abs(trk.eta - etaXiBar) < 0.1)
                ++nKMinus; // Only count K- within 0.2 of the XiBar.
        }
                // Map nFwd to centrality bin 
        double cIndex = 9.5;
        for (auto p : percentiles) {
            if (double(nFwd) > p) break;
            cIndex -= 1.0;
        }


        hKPlusCent.Fill(cIndex, nKPlus);
        hPiPlusCent.Fill(cIndex, nPiPlus);
        hKMinusCent.Fill(cIndex, nKMinus);
        hPiMinusCent.Fill(cIndex, nPiMinus);
    }
    // Save results to a ROOT file
    TFile outFile("k2pi_xixibar_yields01.root", "RECREATE");
    hKPlusCent.Write();
    hPiPlusCent.Write();
    hKMinusCent.Write();
    hPiMinusCent.Write();


    outFile.Close();
    delete evt;
}

void analyzexixibar005(const std::string& pythiaFiles = "../pythiarun") {
    // Open the calibration file
    TFile calibFile("calibration.root", "READ");
    TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
    auto percentiles = findPercentiles(hCalib);
    delete hCalib;

    // Create profiles for PYTHIA data
    TProfile hKPlusCent("hKPlusCent", "nK+ from Xi", 10, 0, 10);
    TProfile hPiPlusCent("hPiPlusCent", "nPi+ from Xi", 10, 0, 10);
    TProfile hKMinusCent("hKMinusCent", "nK- from Xibar", 10, 0, 10);
    TProfile hPiMinusCent("hPiMinusCent", "nPi- from Xibar", 10, 0, 10);

    // Process PYTHIA data
    TChain pythiaTree("t");
    /*for (auto &fn : pythiaFiles) {
        pythiaTree.Add(fn.c_str());
    }*/
    addFilesToChain(pythiaTree, pythiaFiles, "pythiarun");

    RootEvent* evt = nullptr;
    pythiaTree.SetBranchAddress("events", &evt);

    Long64_t nEntries = pythiaTree.GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        pythiaTree.GetEntry(i);

        int nFwd = 0;  // Number of forward tracks
        int nKPlus = 0; // Number of K+
        int nKMinus = 0; // Number of K-
        int nPiPlus = 0; // Number of pi+
        int nPiMinus = 0; // Number of pi-

        // Check if event has Ξ-Ξ̄ pair
        bool hasXi = false, hasXibar = false;
        
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue;
            if (trk.pid == 3312)  hasXi = true;
            else if (trk.pid == -3312) hasXibar = true;
            if (hasXi && hasXibar) break;
        }*/

        // eta of xi and xibar.
        double etaXi = 0, etaXiBar = 0;

        for (auto& trk : evt->tracks) {
          // make sure you are only taking tracks within
          // |eta| < 1.
          if (abs(trk.eta) > 1) continue;
          if (trk.pid == 3312) {
            hasXi = true;
            double etaXi = trk.eta;
          }
          else if (trk.pid == -3312) {
            hasXibar = true;
            double etaXiBar = trk.eta;
          }
          //if (hasXi && hasXibar) break;
        }


        if (!hasXi || !hasXibar) continue;

        /*for (auto &trk : evt->tracks) {
            if (isForwardTrack(trk)) nFwd++;
        }*/

        // Count kaons and pions
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue; // theoretical rapidity cut
            if ((trk.pid) == 321) nKFromXi++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == 211) nPiFromXi++; // π-
            else if ((trk.pid) == -321) nKFromXibar++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == -211) nPiFromXibar++; // π+
        }*/

        // Loop over all particles again, accumulate forward
        // particles for centrality calibration and find particles of interest.
        // These are:

        // K+ from Xi, K- from Xibar, pi+ from Xi, pi- from Xibar
        for (auto &trk : evt->tracks) {
            // First we do the calib
            if (isForwardTrack(trk)) nFwd++;
            // Our other particles of interest needs to be within |eta| < 1 as well.
            if (abs(trk.eta) > 1) continue;
            // Do the pions
            if (abs(trk.pid) == 211 && abs(trk.eta - etaXi) < 0.05)
                ++nPiPlus; // Only count pi+ within 0.2 of the Xi.
            if (abs(trk.pid) == -211 && abs(trk.eta - etaXiBar) < 0.05)
                ++nPiMinus; // Only count pi- within 0.2 of the XiBar.
            // Do the kaons
            if (trk.pid == 321 && abs(trk.eta - etaXi) < 0.05)
                ++nKPlus; // Only count K+ within 0.2 of the Xi.
            if (trk.pid == -321 && abs(trk.eta - etaXiBar) < 0.05)
                ++nKMinus; // Only count K- within 0.2 of the XiBar.
        }
                // Map nFwd to centrality bin 
        double cIndex = 9.5;
        for (auto p : percentiles) {
            if (double(nFwd) > p) break;
            cIndex -= 1.0;
        }


        hKPlusCent.Fill(cIndex, nKPlus);
        hPiPlusCent.Fill(cIndex, nPiPlus);
        hKMinusCent.Fill(cIndex, nKMinus);
        hPiMinusCent.Fill(cIndex, nPiMinus);
    }
    // Save results to a ROOT file
    TFile outFile("k2pi_xixibar_yields005.root", "RECREATE");
    hKPlusCent.Write();
    hPiPlusCent.Write();
    hKMinusCent.Write();
    hPiMinusCent.Write();


    outFile.Close();
    delete evt;
}


void analyzexixibar002(const std::string& pythiaFiles = "../pythiarun") {
    // Open the calibration file
    TFile calibFile("calibration.root", "READ");
    TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
    auto percentiles = findPercentiles(hCalib);
    delete hCalib;

    // Create profiles for PYTHIA data
    TProfile hKPlusCent("hKPlusCent", "nK+ from Xi", 10, 0, 10);
    TProfile hPiPlusCent("hPiPlusCent", "nPi+ from Xi", 10, 0, 10);
    TProfile hKMinusCent("hKMinusCent", "nK- from Xibar", 10, 0, 10);
    TProfile hPiMinusCent("hPiMinusCent", "nPi- from Xibar", 10, 0, 10);

    // Process PYTHIA data
    TChain pythiaTree("t");
    /*for (auto &fn : pythiaFiles) {
        pythiaTree.Add(fn.c_str());
    }*/
    addFilesToChain(pythiaTree, pythiaFiles, "pythiarun");

    RootEvent* evt = nullptr;
    pythiaTree.SetBranchAddress("events", &evt);

    Long64_t nEntries = pythiaTree.GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        pythiaTree.GetEntry(i);

        int nFwd = 0;  // Number of forward tracks
        int nKPlus = 0; // Number of K+
        int nKMinus = 0; // Number of K-
        int nPiPlus = 0; // Number of pi+
        int nPiMinus = 0; // Number of pi-

        // Check if event has Ξ-Ξ̄ pair
        bool hasXi = false, hasXibar = false;
        
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue;
            if (trk.pid == 3312)  hasXi = true;
            else if (trk.pid == -3312) hasXibar = true;
            if (hasXi && hasXibar) break;
        }*/

        // eta of xi and xibar.
        double etaXi = 0, etaXiBar = 0;

        for (auto& trk : evt->tracks) {
          // make sure you are only taking tracks within
          // |eta| < 1.
          if (abs(trk.eta) > 1) continue;
          if (trk.pid == 3312) {
            hasXi = true;
            double etaXi = trk.eta;
          }
          else if (trk.pid == -3312) {
            hasXibar = true;
            double etaXiBar = trk.eta;
          }
          //if (hasXi && hasXibar) break;
        }


        if (!hasXi || !hasXibar) continue;

        /*for (auto &trk : evt->tracks) {
            if (isForwardTrack(trk)) nFwd++;
        }*/

        // Count kaons and pions
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue; // theoretical rapidity cut
            if ((trk.pid) == 321) nKFromXi++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == 211) nPiFromXi++; // π-
            else if ((trk.pid) == -321) nKFromXibar++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == -211) nPiFromXibar++; // π+
        }*/

        // Loop over all particles again, accumulate forward
        // particles for centrality calibration and find particles of interest.
        // These are:

        // K+ from Xi, K- from Xibar, pi+ from Xi, pi- from Xibar
        for (auto &trk : evt->tracks) {
            // First we do the calib
            if (isForwardTrack(trk)) nFwd++;
            // Our other particles of interest needs to be within |eta| < 1 as well.
            if (abs(trk.eta) > 1) continue;
            // Do the pions
            if (abs(trk.pid) == 211 && abs(trk.eta - etaXi) < 0.02)
                ++nPiPlus; // Only count pi+ within 0.2 of the Xi.
            if (abs(trk.pid) == -211 && abs(trk.eta - etaXiBar) < 0.02)
                ++nPiMinus; // Only count pi- within 0.2 of the XiBar.
            // Do the kaons
            if (trk.pid == 321 && abs(trk.eta - etaXi) < 0.02)
                ++nKPlus; // Only count K+ within 0.2 of the Xi.
            if (trk.pid == -321 && abs(trk.eta - etaXiBar) < 0.02)
                ++nKMinus; // Only count K- within 0.2 of the XiBar.
        }
                // Map nFwd to centrality bin 
        double cIndex = 9.5;
        for (auto p : percentiles) {
            if (double(nFwd) > p) break;
            cIndex -= 1.0;
        }


        hKPlusCent.Fill(cIndex, nKPlus);
        hPiPlusCent.Fill(cIndex, nPiPlus);
        hKMinusCent.Fill(cIndex, nKMinus);
        hPiMinusCent.Fill(cIndex, nPiMinus);
    }
    // Save results to a ROOT file
    TFile outFile("k2pi_xixibar_yields002.root", "RECREATE");
    hKPlusCent.Write();
    hPiPlusCent.Write();
    hKMinusCent.Write();
    hPiMinusCent.Write();


    outFile.Close();
    delete evt;
}

/*void analyzexixibarkminus(const std::string& pythiaFiles = "../pythiarun") {
    // Open the calibration file
    TFile calibFile("calibration.root", "READ");
    TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
    auto percentiles = findPercentiles(hCalib);
    delete hCalib;

    // Create profiles for PYTHIA data
    TProfile hKPlusCent("hKPlusCent", "nK+ from Xi", 10, 0, 10);
    TProfile hPiPlusCent("hPiPlusCent", "nPi+ from Xibar", 10, 0, 10);
    TProfile hKMinusCent("hKMinusCent", "nK- from Xibar", 10, 0, 10);
    TProfile hPiMinusCent("hPiMinusCent", "nPi- from Xi", 10, 0, 10);

    // Process PYTHIA data
    TChain pythiaTree("t");
    /*for (auto &fn : pythiaFiles) {
        pythiaTree.Add(fn.c_str());
    }
    addFilesToChain(pythiaTree, pythiaFiles, "pythiarun");

    RootEvent* evt = nullptr;
    pythiaTree.SetBranchAddress("events", &evt);

    Long64_t nEntries = pythiaTree.GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        pythiaTree.GetEntry(i);

        int nFwd = 0;  // Number of forward tracks
        int nKPlus = 0; // Number of K+
        int nKMinus = 0; // Number of K-
        int nPiPlus = 0; // Number of pi+
        int nPiMinus = 0; // Number of pi-

        // Check if event has Ξ-Ξ̄ pair
        bool hasXi = false, hasXibar = false;
        
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue;
            if (trk.pid == 3312)  hasXi = true;
            else if (trk.pid == -3312) hasXibar = true;
            if (hasXi && hasXibar) break;
        }

        // eta of xi and xibar.
        double etaXi = 0, etaXiBar = 0;

        for (auto& trk : evt->tracks) {
          // make sure you are only taking tracks within
          // |eta| < 1.
          if (abs(trk.eta) > 1) continue;
          if (trk.pid == 3312) {
            hasXi = true;
            double etaXi = trk.eta;
          }
          else if (trk.pid == -3312) {
            hasXibar = true;
            double etaXiBar = trk.eta;
          }
          //if (hasXi && hasXibar) break;
        }


        if (!hasXi || !hasXibar) continue;

        /*for (auto &trk : evt->tracks) {
            if (isForwardTrack(trk)) nFwd++;
        }

        // Count kaons and pions
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue; // theoretical rapidity cut
            if ((trk.pid) == 321) nKFromXi++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == 211) nPiFromXi++; // π-
            else if ((trk.pid) == -321) nKFromXibar++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == -211) nPiFromXibar++; // π+
        }

        // Loop over all particles again, accumulate forward
        // particles for centrality calibration and find particles of interest.
        // These are:

        // K+ from Xi, K- from Xibar, pi+ from Xi, pi- from Xibar
        for (auto &trk : evt->tracks) {
            // First we do the calib
            if (isForwardTrack(trk)) nFwd++;
            // Our other particles of interest needs to be within |eta| < 1 as well.
            if (abs(trk.eta) > 1) continue;
            // Do the pions
            if (trk.pid == -211 && abs(trk.eta - etaXi) < 0.2)
                ++nPiMinus; // Only count pi+ within 0.2 of the Xi.
            if (trk.pid == 211 && abs(trk.eta - etaXiBar) < 0.2)
                ++nPiPlus; // Only count pi- within 0.2 of the XiBar.
            // Do the kaons
            if (trk.pid == 321 && abs(trk.eta - etaXi) < 0.2)
                ++nKPlus; // Only count K+ within 0.2 of the Xi.
            if (trk.pid == -321 && abs(trk.eta - etaXiBar) < 0.2)
                ++nKMinus; // Only count K- within 0.2 of the XiBar.
        }
                // Map nFwd to centrality bin 
        double cIndex = 9.5;
        for (auto p : percentiles) {
            if (double(nFwd) > p) break;
            cIndex -= 1.0;
        }


        hKPlusCent.Fill(cIndex, nKPlus);
        hPiPlusCent.Fill(cIndex, nPiPlus);
        hKMinusCent.Fill(cIndex, nKMinus);
        hPiMinusCent.Fill(cIndex, nPiMinus);
    }
    // Save results to a ROOT file
    TFile outFile("k2pi_xixibar_yieldskminus.root", "RECREATE");
    hKPlusCent.Write();
    hPiPlusCent.Write();
    hKMinusCent.Write();
    hPiMinusCent.Write();


    outFile.Close();
    delete evt;
}*/

/*void analyzexixibarkminus01(const std::string& pythiaFiles = "../pythiarun") {
    // Open the calibration file
    TFile calibFile("calibration.root", "READ");
    TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
    auto percentiles = findPercentiles(hCalib);
    delete hCalib;

    // Create profiles for PYTHIA data
    TProfile hKPlusCent("hKPlusCent", "nK+ from Xi", 10, 0, 10);
    TProfile hPiPlusCent("hPiPlusCent", "nPi+ from Xibar", 10, 0, 10);
    TProfile hKMinusCent("hKMinusCent", "nK- from Xibar", 10, 0, 10);
    TProfile hPiMinusCent("hPiMinusCent", "nPi- from Xi", 10, 0, 10);

    // Process PYTHIA data
    TChain pythiaTree("t");
    /*for (auto &fn : pythiaFiles) {
        pythiaTree.Add(fn.c_str());
    }
    addFilesToChain(pythiaTree, pythiaFiles, "pythiarun");

    RootEvent* evt = nullptr;
    pythiaTree.SetBranchAddress("events", &evt);

    Long64_t nEntries = pythiaTree.GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        pythiaTree.GetEntry(i);

        int nFwd = 0;  // Number of forward tracks
        int nKPlus = 0; // Number of K+
        int nKMinus = 0; // Number of K-
        int nPiPlus = 0; // Number of pi+
        int nPiMinus = 0; // Number of pi-

        // Check if event has Ξ-Ξ̄ pair
        bool hasXi = false, hasXibar = false;
        
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue;
            if (trk.pid == 3312)  hasXi = true;
            else if (trk.pid == -3312) hasXibar = true;
            if (hasXi && hasXibar) break;
        }

        // eta of xi and xibar.
        double etaXi = 0, etaXiBar = 0;

        for (auto& trk : evt->tracks) {
          // make sure you are only taking tracks within
          // |eta| < 1.
          if (abs(trk.eta) > 1) continue;
          if (trk.pid == 3312) {
            hasXi = true;
            double etaXi = trk.eta;
          }
          else if (trk.pid == -3312) {
            hasXibar = true;
            double etaXiBar = trk.eta;
          }
          //if (hasXi && hasXibar) break;
        }


        if (!hasXi || !hasXibar) continue;

        /*for (auto &trk : evt->tracks) {
            if (isForwardTrack(trk)) nFwd++;
        }

        // Count kaons and pions
        /*for (auto &trk : evt->tracks) {
            if (abs(trk.eta) > 1) continue; // theoretical rapidity cut
            if ((trk.pid) == 321) nKFromXi++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == 211) nPiFromXi++; // π-
            else if ((trk.pid) == -321) nKFromXibar++;    // Kₛ⁰ 321 for xi and 
            else if ((trk.pid) == -211) nPiFromXibar++; // π+
        }

        // Loop over all particles again, accumulate forward
        // particles for centrality calibration and find particles of interest.
        // These are:

        // K+ from Xi, K- from Xibar, pi+ from Xi, pi- from Xibar
        for (auto &trk : evt->tracks) {
            // First we do the calib
            if (isForwardTrack(trk)) nFwd++;
            // Our other particles of interest needs to be within |eta| < 1 as well.
            if (abs(trk.eta) > 1) continue;
            // Do the pions
            if (trk.pid == -211 && abs(trk.eta - etaXi) < 0.1)
                ++nPiMinus; // Only count pi+ within 0.2 of the Xi.
            if (trk.pid == 211 && abs(trk.eta - etaXiBar) < 0.1)
                ++nPiPlus; // Only count pi- within 0.2 of the XiBar.
            // Do the kaons
            if (trk.pid == 321 && abs(trk.eta - etaXi) < 0.1)
                ++nKPlus; // Only count K+ within 0.2 of the Xi.
            if (trk.pid == -321 && abs(trk.eta - etaXiBar) < 0.1)
                ++nKMinus; // Only count K- within 0.2 of the XiBar.
        }
                // Map nFwd to centrality bin 
        double cIndex = 9.5;
        for (auto p : percentiles) {
            if (double(nFwd) > p) break;
            cIndex -= 1.0;
        }


        hKPlusCent.Fill(cIndex, nKPlus);
        hPiPlusCent.Fill(cIndex, nPiPlus);
        hKMinusCent.Fill(cIndex, nKMinus);
        hPiMinusCent.Fill(cIndex, nPiMinus);
    }
    // Save results to a ROOT file
    TFile outFile("k2pi_xixibar_yieldskminus01.root", "RECREATE");
    hKPlusCent.Write();
    hPiPlusCent.Write();
    hKMinusCent.Write();
    hPiMinusCent.Write();


    outFile.Close();
    delete evt;
}*/


void analyzexixibarFIST(const std::string& fistDirectory = "/home/rrabbani11/fist_data") {

    // Create profiles for FIST data
    TProfile hKPlusFISTCent("hKPlusFISTCent", "nK+ from Xi FIST", 10, 0, 10);
    TProfile hPiPlusFISTCent("hPiPlusFISTCent", "nPi+ from Xi FIST", 10, 0, 10);
    TProfile hKMinusFISTCent("hKMinusFISTCent", "nK- from Xibar FIST", 10, 0, 10);
    TProfile hPiMinusFISTCent("hPiMinusFISTCent", "nPi- from Xibar FIST", 10, 0, 10);

    // Process FIST data
    TChain fistTree("TT");
    addFilesToChain(fistTree, fistDirectory, "fist_data-");

    TClonesArray* trackArray = nullptr;
    MyEvent* event = nullptr;
    fistTree.SetBranchAddress("tracks", &trackArray);
    fistTree.SetBranchAddress("event", &event);

    // Define centrality classes (same as in FISTOnly.C)
    const Int_t nMultClasses = 10;
    constexpr double kCentClasses[nMultClasses+1] = {0., 0.95, 4.7, 9.5, 14., 19., 28., 38., 48., 68., 100.};


    Long64_t nEntries = fistTree.GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        fistTree.GetEntry(i);

        int nKPlus = 0, nKMinus = 0, nPiPlus = 0, nPiMinus = 0;

        // Find eta of Xi and Xibar within |eta|<1
        bool hasXi = false, hasXibar = false;
        double etaXi = 0, etaXiBar = 0;
        for (int j = 0; j < trackArray->GetEntriesFast(); ++j) {
            MyParticle* track = (MyParticle*)trackArray->At(j);
            if (fabs(track->fEta) > 1) continue;
            if (track->fPDG == 3312) {
                hasXi = true;
                etaXi = track->fEta;
            } else if (track->fPDG == -3312) {
                hasXibar = true;
                etaXiBar = track->fEta;
            }
        }
        if (!hasXi || !hasXibar) continue;

        // Loop again for centrality and particles of interest
        for (int j = 0; j < trackArray->GetEntriesFast(); ++j) {
            MyParticle* track = (MyParticle*)trackArray->At(j);
            // Only |eta|<1 for particles of interest
            if (fabs(track->fEta) > 1) continue;
            // Pi+
            if (fabs(track->fPDG) == 211 && fabs(track->fEta - etaXi) < 0.2)
                ++nPiPlus;
            // Pi-
            if (fabs(track->fPDG) == 211 && fabs(track->fEta - etaXiBar) < 0.2)
                ++nPiMinus;
            // K+
            if (track->fPDG == 321 && fabs(track->fEta - etaXi) < 0.2)
                ++nKPlus;
            // K-
            if (track->fPDG == -321 && fabs(track->fEta - etaXiBar) < 0.2)
                ++nKMinus;
        }

        double centrality = event->fV0A; // Random centrality value
        int binIndex = 0;
        for (int k = 0; k < nMultClasses; k++) {
            if (centrality >= kCentClasses[k] && centrality < kCentClasses[k + 1]) {
                binIndex = nMultClasses - 1 - k; 
                break;
            }
        }
        
        hKPlusFISTCent.Fill(binIndex, nKPlus);
        hPiPlusFISTCent.Fill(binIndex, nPiPlus);
        hKMinusFISTCent.Fill(binIndex, nKMinus);
        hPiMinusFISTCent.Fill(binIndex, nPiMinus);
    }

    // Save results to a ROOT file
    TFile outFile("k2pi_xixibar_yieldsFIST.root", "RECREATE");
    hKPlusFISTCent.Write();
    hPiPlusFISTCent.Write();
    hKMinusFISTCent.Write();
    hPiMinusFISTCent.Write();
    outFile.Close();
}

// Function to load experimental yields from ROOT files
TGraphAsymmErrors* loadExperimentalRatio(const std::string& filename, const std::string& tableName) {
  TFile* file = TFile::Open(filename.c_str(), "READ");
  if (!file || file->IsZombie()) {
      std::cerr << "Error: Could not open file " << filename << std::endl;
      return nullptr;
  }

  TDirectory* dir = file->GetDirectory(tableName.c_str());
  if (!dir) {
      std::cerr << "Error: Could not find directory " << tableName << " in file " << filename << std::endl;
      file->Close();
      return nullptr;
  }

  TGraphAsymmErrors* graph = dir->Get<TGraphAsymmErrors>("Graph1D_y1");
  if (!graph) {
      std::cerr << "Error: Could not find TGraphAsymmErrors in directory " << tableName << std::endl;
      file->Close();
      return nullptr;
  }

  file->Close();
  return graph;
}

// Function to draw ratio plots with experimental and simulated data
void drawRatioWithExperimentalData() {
  gStyle->SetOptStat(0);
  gStyle->SetTitleFontSize(0.04);

  TFile* yieldsFile = new TFile("yields.root", "READ");
  if (!yieldsFile || yieldsFile->IsZombie()) {
      std::cerr << "Error: Could not open yields.root" << std::endl;
      return;
  }

  // Convert TProfile to TH1D for PYTHIA data
  TH1D* pi_sim = yieldsFile->Get<TProfile>("hPiCent")->ProjectionX();
  TH1D* k_sim = yieldsFile->Get<TProfile>("hKCent")->ProjectionX();
  TH1D* L_sim = yieldsFile->Get<TProfile>("hLCent")->ProjectionX();
  TH1D* X_sim = yieldsFile->Get<TProfile>("hXCent")->ProjectionX();
  TH1D* O_sim = yieldsFile->Get<TProfile>("hOmegaCent")->ProjectionX();

  // Convert TProfile to TH1D for Thermal-FIST data
  
  TH1D* pi_fist = yieldsFile->Get<TProfile>("hPiCentFIST")->ProjectionX();
  TH1D* k_fist = yieldsFile->Get<TProfile>("hKCentFIST")->ProjectionX();
  TH1D* L_fist = yieldsFile->Get<TProfile>("hLCentFIST")->ProjectionX();
  TH1D* X_fist = yieldsFile->Get<TProfile>("hXCentFIST")->ProjectionX();
  TH1D* O_fist = yieldsFile->Get<TProfile>("hOmegaCentFIST")->ProjectionX();

  // Load experimental ratios
  TGraphAsymmErrors* k_exp = loadExperimentalRatio("Kaon-piratio.root", "Table 36");
  TGraphAsymmErrors* L_exp = loadExperimentalRatio("lambda-piratio.root", "Table 37");
  TGraphAsymmErrors* X_exp = loadExperimentalRatio("xi-piratio.root", "Table 38");
  TGraphAsymmErrors* O_exp = loadExperimentalRatio("omega-piratio.root", "Table 39");

  if (!k_exp || !L_exp || !X_exp || !O_exp) {
      std::cerr << "Error: Failed to load experimental ratios." << std::endl;
      return;
  }

  // Adjust x-coordinates of HEPData points
  auto adjustXCoordinates = [](TGraphAsymmErrors* graph) {
      if (!graph) return;
      int nPoints = graph->GetN();
      for (int i = 0; i < nPoints; ++i) {
          double x, y;
          graph->GetPoint(i, x, y);
          double newX = 9.5 - i;
          graph->SetPoint(i, newX, y);
          graph->SetPointEXlow(i, 0.5);
          graph->SetPointEXhigh(i, 0.5);
      }
  };

  adjustXCoordinates(k_exp);
  adjustXCoordinates(L_exp);
  adjustXCoordinates(X_exp);

  auto adjustXOCoordinates = [](TGraphAsymmErrors* graph) {
    if (!graph) return;
    int nPoints = graph->GetN();
    for (int i = 0; i < nPoints; ++i) {
        double x, y;
        graph->GetPoint(i, x, y);
        double newX = 9.5 - 2*i;
        graph->SetPoint(i, newX, y);
        graph->SetPointEXlow(i, 1.0);
        graph->SetPointEXhigh(i, 1.0);
    }
};

  adjustXOCoordinates(O_exp);

  // Create a canvas for the ratio plot
  TCanvas* c1 = new TCanvas("c1", "Yield Ratios: Simulated vs Experimental", 1700, 1000);
  c1->SetLogy();
  c1->SetGrid();

  

  // PYTHIA data ratios
  k_sim->Scale(2.);
  k_sim->Divide(pi_sim);
  k_sim->SetTitle("Particle Yield Ratios vs Centrality;Centrality Class;(Strange Hadron)/(#pi^{+} + #pi^{-})");
  k_sim->GetYaxis()->SetTitle("");
  k_sim->GetYaxis()->SetTitleOffset(1.2);
  k_sim->SetLineWidth(1);
  k_sim->SetMarkerStyle(20);
  k_sim->SetMarkerSize(2);
  k_sim->SetMarkerColor(kBlack);
  k_sim->SetMinimum(0.002);
  k_sim->SetMaximum(0.2);
  k_sim->Draw("E1");

  L_sim->Scale(2.);
  L_sim->Divide(pi_sim);
  L_sim->SetLineWidth(1);
  L_sim->SetMarkerStyle(21);
  L_sim->SetMarkerSize(2);
  L_sim->SetMarkerColor(kGreen + 2);
  L_sim->Draw("E1 SAME");

  X_sim->Scale(6.);
  X_sim->Divide(pi_sim);
  X_sim->SetLineWidth(1);
  X_sim->SetMarkerStyle(22);
  X_sim->SetMarkerSize(2);
  X_sim->SetMarkerColor(kBlue);
  X_sim->Draw("E1 SAME");

  O_sim->Scale(16.);
  O_sim->Divide(pi_sim);
  O_sim->SetLineWidth(1);
  O_sim->SetMarkerStyle(23);
  O_sim->SetMarkerSize(2);
  O_sim->SetMarkerColor(kRed);
  O_sim->Draw("E1 SAME");

  // Thermal-FIST data ratios
  
  k_fist->Scale(1.);
  k_fist->Divide(pi_fist);
  k_fist->SetLineWidth(1);
  k_fist->SetMarkerStyle(24);
  k_fist->SetMarkerSize(2);
  k_fist->SetMarkerColor(kPink);
  k_fist->Draw("E1 SAME");

  L_fist->Scale(2.);
  L_fist->Divide(pi_fist);
  L_fist->SetLineWidth(1);
  L_fist->SetMarkerStyle(25);
  L_fist->SetMarkerSize(2);
  L_fist->SetMarkerColor(kViolet);
  L_fist->Draw("E1 SAME");

  X_fist->Scale(6.);
  X_fist->Divide(pi_fist);
  X_fist->SetLineWidth(1);
  X_fist->SetMarkerStyle(26);
  X_fist->SetMarkerSize(2);
  X_fist->SetMarkerColor(kAzure);
  X_fist->Draw("E1 SAME");

  O_fist->Scale(16.);
  O_fist->Divide(pi_fist);
  O_fist->SetLineWidth(1);
  O_fist->SetMarkerStyle(27);
  O_fist->SetMarkerSize(2);
  O_fist->SetMarkerColor(kTeal);
  O_fist->Draw("E1 SAME");

  // Experimental data ratios
  k_exp->SetMarkerStyle(24);
  k_exp->SetMarkerSize(2);
  k_exp->SetMarkerColor(kMagenta);
  k_exp->SetLineColor(kBlack);
  k_exp->Draw("P SAME");

  L_exp->Scale(2.);
  L_exp->SetMarkerStyle(25);
  L_exp->SetMarkerSize(2);
  L_exp->SetMarkerColor(kCyan);
  L_exp->SetLineColor(kCyan);
  L_exp->Draw("P SAME");

  X_exp->Scale(6.);
  X_exp->SetMarkerStyle(26);
  X_exp->SetMarkerSize(2);
  X_exp->SetMarkerColor(kOrange);
  X_exp->SetLineColor(kOrange);
  X_exp->Draw("P SAME");

  O_exp->Scale(16.);
  O_exp->SetMarkerStyle(27);
  O_exp->SetMarkerSize(2);
  O_exp->SetMarkerColor(kBlue);
  O_exp->SetLineColor(kBlue);
  O_exp->Draw("P SAME");

// Print bin values and error bars
  auto printHistogram = [](TH1D* hist, const std::string& name) {
      std::cout << name << " bins and errors:" << std::endl;
      for (int i = 1; i <= hist->GetNbinsX(); ++i) {
          std::cout << "Bin " << i << ": Value = " << hist->GetBinContent(i)
                    << ", Error = " << hist->GetBinError(i) << std::endl;
      }
  };

  auto printGraph = [](TGraphAsymmErrors* graph, const std::string& name) {
      std::cout << name << " experimental data:" << std::endl;
      for (int i = 0; i < graph->GetN(); ++i) {
          double x, y;
          graph->GetPoint(i, x, y);
          double exl = graph->GetErrorXlow(i);
          double exh = graph->GetErrorXhigh(i);
          double eyl = graph->GetErrorYlow(i);
          double eyh = graph->GetErrorYhigh(i);
          std::cout << "Point " << i + 1 << ": X = " << x << " ± [" << exl << ", " << exh << "]"
                    << ", Y = " << y << " ± [" << eyl << ", " << eyh << "]" << std::endl;
      }
  };


  // Beautify legend
  TLegend* leg = new TLegend(0.895, 0.2, 1.0, 0.9);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.028);
  leg->AddEntry((TObject*)0, "PYTHIA:", "");
  leg->AddEntry(k_sim, "2K^{0}_{S} #times2", "ep");
  leg->AddEntry(L_sim, "#Lambda + #bar{#Lambda} #times2", "ep");
  leg->AddEntry(X_sim, "#Xi^{-} + #bar{#Xi}^{+} #times6", "ep");
  leg->AddEntry(O_sim, "#Omega^{-} + #bar{#Omega}^{+} #times16", "ep");
  leg->AddEntry((TObject*)0, "FIST:", "");
  leg->AddEntry(k_fist, "K^{0}", "ep");
  leg->AddEntry(L_fist, "#Lambda + #bar{#Lambda} #times2", "ep");
  leg->AddEntry(X_fist, "#Xi^{-} + #bar{#Xi}^{+} #times6", "ep");
  leg->AddEntry(O_fist, "#Omega^{-} + #bar{#Omega}^{+} #times16", "ep");
  leg->AddEntry((TObject*)0, "ALICE:", "");
  leg->AddEntry(k_exp, "2K^{0}_{S} #times2", "p");
  leg->AddEntry(L_exp, "#Lambda + #bar{#Lambda} #times2", "p");
  leg->AddEntry(X_exp, "#Xi^{-} + #bar{#Xi}^{+} #times6", "p");
  leg->AddEntry(O_exp, "#Omega^{-} + #bar{#Omega}^{+} #times16", "p");
  leg->Draw();

  printHistogram(k_sim, "K/pi (PYTHIA)");
  printHistogram(L_sim, "Lambda/pi (PYTHIA)");
  printHistogram(X_sim, "Xi/pi (PYTHIA)");
  printHistogram(O_sim, "Omega/pi (PYTHIA)");

  printHistogram(k_fist, "K/pi (FIST)");
  printHistogram(L_fist, "Lambda/pi (FIST)");
  printHistogram(X_fist, "Xi/pi (FIST)");
  printHistogram(O_fist, "Omega/pi (FIST)");

  printGraph(k_exp, "K/pi (Exp)");
  printGraph(L_exp, "Lambda/pi (Exp)");
  printGraph(X_exp, "Xi/pi (Exp)");
  printGraph(O_exp, "Omega/pi (Exp)");



  c1->Update();
}

// Function to draw K/pi ratios within Xi-Xi-bar productions

void drawRatiosk2pi() {
  gStyle->SetOptStat(0);
  gStyle->SetTitleFontSize(0.04);

  TFile* yieldsFile = new TFile("k2pi_xi_yields.root", "READ");
  if (!yieldsFile || yieldsFile->IsZombie()) {
    std::cerr << "Error: Could not open k2pi_xi_yields.root" << std::endl;
    return;
  }

  // Get profiles and convert to TH1D
  TH1D* hKFromXi = yieldsFile->Get<TProfile>("hKFromXiCent")->ProjectionX();
  TH1D* hPiFromXi = yieldsFile->Get<TProfile>("hPiFromXiCent")->ProjectionX();
  TH1D* hKFromXiFIST = yieldsFile->Get<TProfile>("hKFromXiCentFIST")->ProjectionX();
  TH1D* hPiFromXiFIST = yieldsFile->Get<TProfile>("hPiFromXiCentFIST")->ProjectionX();

  // Print total number of K and Pi in each class (PYTHIA)
std::cout << "PYTHIA K in Xi-Xibar events per class:" << std::endl;
for (int i = 1; i <= hKFromXi->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hKFromXi->GetBinContent(i) << std::endl;
std::cout << "PYTHIA Pi in Xi-Xibar events per class:" << std::endl;
for (int i = 1; i <= hPiFromXi->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hPiFromXi->GetBinContent(i) << std::endl;

// Print total number of K and Pi in each class (FIST)
std::cout << "FIST K in Xi-Xibar events per class:" << std::endl;
for (int i = 1; i <= hKFromXiFIST->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hKFromXiFIST->GetBinContent(i) << std::endl;
std::cout << "FIST Pi in Xi-Xibar events per class:" << std::endl;
for (int i = 1; i <= hPiFromXiFIST->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hPiFromXiFIST->GetBinContent(i) << std::endl;


  TCanvas* c1 = new TCanvas("c1", "K/pi ratio within Xi-Xi-bar", 800, 600);
  c1->SetLogy();
  c1->SetGrid();

  hKFromXi->Scale(2.);
  hKFromXi->Divide(hPiFromXi);
  hKFromXi->SetTitle("K/#pi ratio within Xi-#bar{Xi} productions;Centrality Class;K/#pi Ratio");
  hKFromXi->SetLineColor(kRed);
  hKFromXi->SetMarkerColor(kRed);
  hKFromXi->SetMarkerStyle(20);
  hKFromXi->Draw("E1");
  

  hKFromXiFIST->Divide(hPiFromXiFIST);
  hKFromXiFIST->SetLineColor(kBlue);
  hKFromXiFIST->SetMarkerColor(kBlue);
  hKFromXiFIST->SetMarkerStyle(21);
  hKFromXiFIST->Draw("E1 SAME");

  // Print bin values and error bars
  auto printHistogram = [](TH1D* hist, const std::string& name) {
      std::cout << name << " bins and errors:" << std::endl;
      for (int i = 1; i <= hist->GetNbinsX(); ++i) {
          std::cout << "Bin " << i << ": Value = " << hist->GetBinContent(i)
                    << ", Error = " << hist->GetBinError(i) << std::endl;
      }
  };


  /*
  
  // Set reasonable axis range
  double max_val = max(hRatioPythia->GetMaximum(), hRatioFIST->GetMaximum()) * 1.2;
  hRatioPythia->SetMaximum(max_val);
  hRatioPythia->SetMinimum(0);
  */
  

  // Add legend
  TLegend* leg = new TLegend(0.7, 0.8, 0.9, 0.9);
  leg->AddEntry(hKFromXi, "PYTHIA", "lep");
  leg->AddEntry(hKFromXiFIST, "Thermal-FIST", "lep");
  leg->Draw();

  printHistogram(hKFromXi, "K/pi (PYTHIA)");
  printHistogram(hKFromXiFIST, "K/pi (FIST)");

  c1->Update();

  
}

/*void drawRatiosXiXibar() {
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.04);

    // Open the ROOT file
    TFile* yieldsFile = new TFile("k2pi_xixibar_yields.root", "READ");
    if (!yieldsFile || yieldsFile->IsZombie()) {
        std::cerr << "Error: Could not open k2pi_xixibar_yields.root" << std::endl;
        return;
    }

    // Get profiles and convert to TH1D
    TProfile* hKPlusProfile = yieldsFile->Get<TProfile>("hKPlusCent");
    TProfile* hPiPlusProfile = yieldsFile->Get<TProfile>("hPiPlusCent");
    TProfile* hKMinusProfile = yieldsFile->Get<TProfile>("hKMinusCent");
    TProfile* hPiMinusProfile = yieldsFile->Get<TProfile>("hPiMinusCent");

    if (!hKPlusProfile || !hPiPlusProfile || !hKMinusProfile || !hPiMinusProfile) {
        std::cerr << "Error: One or more required TProfile histograms are missing in the file." << std::endl;
        yieldsFile->Close();
        return;
    }

    // Convert TProfile to TH1D
    TH1D* hKPlus = hKPlusProfile->ProjectionX();
    TH1D* hPiPlus = hPiPlusProfile->ProjectionX();
    TH1D* hKMinus = hKMinusProfile->ProjectionX();
    TH1D* hPiMinus = hPiMinusProfile->ProjectionX();

    // Print total number of each hadron in each class
std::cout << "K+ per class:" << std::endl;
for (int i = 1; i <= hKPlus->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hKPlus->GetBinContent(i) << std::endl;
std::cout << "Pi+ per class:" << std::endl;
for (int i = 1; i <= hPiPlus->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hPiPlus->GetBinContent(i) << std::endl;
std::cout << "K- per class:" << std::endl;
for (int i = 1; i <= hKMinus->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hKMinus->GetBinContent(i) << std::endl;
std::cout << "Pi- per class:" << std::endl;
for (int i = 1; i <= hPiMinus->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hPiMinus->GetBinContent(i) << std::endl;

    // Debug: Print histogram contents
    std::cout << "hKPlus contents:" << std::endl;
    for (int i = 1; i <= hKPlus->GetNbinsX(); ++i) {
        std::cout << "Bin " << i << ": " << hKPlus->GetBinContent(i) << std::endl;
    }

    // Print histogram contents
    auto printHistogram = [](TH1D* hist, const std::string& name) {
        std::cout << name << " bins and errors:" << std::endl;
        for (int i = 1; i <= hist->GetNbinsX(); ++i) {
            std::cout << "Bin " << i << ": Value = " << hist->GetBinContent(i)
                      << ", Error = " << hist->GetBinError(i) << std::endl;
        }
    };

    printHistogram(hKPlus, "K+");
    printHistogram(hPiPlus, "Pi+");
    printHistogram(hKMinus, "K-");
    printHistogram(hPiMinus, "Pi-");


    // Create a canvas
    TCanvas* c1 = new TCanvas("c1", "K/pi ratio within Xi-Xi-bar", 1700, 1000);
    //TCanvas* c1 = new TCanvas("c1", "K/pi ratio within Xi-Xi-bar", 800, 600);
    c1->SetLogy();
    c1->SetGrid();

    // Scale and divide histograms
    if (hPiPlus->Integral() == 0 || hPiMinus->Integral() == 0) {
        std::cerr << "Error: Denominator histogram has zero integral, cannot divide." << std::endl;
        yieldsFile->Close();
        return;
    }

    hKPlus->Scale(1.);
    hKPlus->Divide(hPiMinus);
    hKPlus->SetTitle("K^{+/-}/(#pi^{+} + #pi^{-}) ratio within #Xi-#bar{Xi} productions;Centrality Class;Yield Ratio");
    hKPlus->SetLineColor(kRed);
    hKPlus->SetMarkerColor(kRed);
    hKPlus->SetMarkerStyle(20);
    hKPlus->SetMinimum(0.001); // Set axis range
    hKPlus->SetMaximum(0.5);
    hKPlus->Draw("E1");

    hKMinus->Divide(hPiPlus);
    hKMinus->SetLineColor(kBlue);
    hKMinus->SetMarkerColor(kBlue);
    hKMinus->SetMarkerStyle(21);
    hKMinus->Draw("E1 SAME");

    // Add legend
    TLegend* leg = new TLegend(0.895, 0.2, 1.0, 0.9);
    leg->AddEntry(hKPlus, "K^{+}/(#pi^{+} + #pi^{-})", "lep");
    leg->AddEntry(hKMinus, "K^{-}/(#pi^{+} + #pi^{-})", "lep");
    leg->Draw();

    // Update canvas and save to file
    c1->Update();
    c1->SaveAs("outputxixibar2.png");

    yieldsFile->Close();
}*/

void drawRatiosXiXibar() {
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.04);

    // Open the ROOT file
    TFile* yieldsFile = new TFile("k2pi_xixibar_yields.root", "READ");
    if (!yieldsFile || yieldsFile->IsZombie()) {
        std::cerr << "Error: Could not open k2pi_xixibar_yields.root" << std::endl;
        return;
    }

    // Get profiles and convert to TH1D
    TProfile* hKPlusProfile = yieldsFile->Get<TProfile>("hKPlusCent");
    TProfile* hPiPlusProfile = yieldsFile->Get<TProfile>("hPiPlusCent");
    TProfile* hKMinusProfile = yieldsFile->Get<TProfile>("hKMinusCent");
    TProfile* hPiMinusProfile = yieldsFile->Get<TProfile>("hPiMinusCent");

    if (!hKPlusProfile || !hPiPlusProfile || !hKMinusProfile || !hPiMinusProfile) {
        std::cerr << "Error: One or more required TProfile histograms are missing in the file." << std::endl;
        yieldsFile->Close();
        return;
    }

    // Convert TProfile to TH1D
    TH1D* hKPlus = hKPlusProfile->ProjectionX();
    TH1D* hPiPlus = hPiPlusProfile->ProjectionX();
    TH1D* hKMinus = hKMinusProfile->ProjectionX();
    TH1D* hPiMinus = hPiMinusProfile->ProjectionX();

    // Combine every two bins into one (5 bins total)
    int nOldBins = hKPlus->GetNbinsX();
    int nNewBins = nOldBins / 2;
    double xlow = hKPlus->GetXaxis()->GetXmin();
    double xup = hKPlus->GetXaxis()->GetXmax();
    TH1D* hKPlus5 = new TH1D("hKPlus5", "K+ (5 bins)", nNewBins, xlow, xup);
    TH1D* hPiPlus5 = new TH1D("hPiPlus5", "Pi+ (5 bins)", nNewBins, xlow, xup);
    TH1D* hKMinus5 = new TH1D("hKMinus5", "K- (5 bins)", nNewBins, xlow, xup);
    TH1D* hPiMinus5 = new TH1D("hPiMinus5", "Pi- (5 bins)", nNewBins, xlow, xup);

    for (int i = 1; i <= nNewBins; ++i) {
        int bin1 = 2 * i - 1;
        int bin2 = 2 * i;
        // Sum contents
        double sumKPlus = hKPlus->GetBinContent(bin1) + hKPlus->GetBinContent(bin2);
        double sumPiPlus = hPiPlus->GetBinContent(bin1) + hPiPlus->GetBinContent(bin2);
        double sumKMinus = hKMinus->GetBinContent(bin1) + hKMinus->GetBinContent(bin2);
        double sumPiMinus = hPiMinus->GetBinContent(bin1) + hPiMinus->GetBinContent(bin2);
        // Sum errors in quadrature
        double errKPlus = std::sqrt(std::pow(hKPlus->GetBinError(bin1), 2) + std::pow(hKPlus->GetBinError(bin2), 2));
        double errPiPlus = std::sqrt(std::pow(hPiPlus->GetBinError(bin1), 2) + std::pow(hPiPlus->GetBinError(bin2), 2));
        double errKMinus = std::sqrt(std::pow(hKMinus->GetBinError(bin1), 2) + std::pow(hKMinus->GetBinError(bin2), 2));
        double errPiMinus = std::sqrt(std::pow(hPiMinus->GetBinError(bin1), 2) + std::pow(hPiMinus->GetBinError(bin2), 2));
        // Set new bin content and error
        hKPlus5->SetBinContent(i, sumKPlus);
        hKPlus5->SetBinError(i, errKPlus);
        hPiPlus5->SetBinContent(i, sumPiPlus);
        hPiPlus5->SetBinError(i, errPiPlus);
        hKMinus5->SetBinContent(i, sumKMinus);
        hKMinus5->SetBinError(i, errKMinus);
        hPiMinus5->SetBinContent(i, sumPiMinus);
        hPiMinus5->SetBinError(i, errPiMinus);
    }

    // Now use the new 5-bin histograms for your ratios
    if (hPiPlus5->Integral() == 0 || hPiMinus5->Integral() == 0) {
        std::cerr << "Error: Denominator histogram has zero integral, cannot divide." << std::endl;
        yieldsFile->Close();
        return;
    }

    hKPlus5->Divide(hPiMinus5);
    hKPlus5->SetTitle("K^{+/-}/(#pi^{+} + #pi^{-}) ratio within #Xi and #bar{#Xi} |#Delta#eta < 0.2|;Centrality Class;Yield Ratio");
    hKPlus5->SetLineColor(kRed);
    hKPlus5->SetMarkerColor(kRed);
    hKPlus5->SetMarkerStyle(20);
    hKPlus5->SetMinimum(0.001);
    hKPlus5->SetMaximum(0.5);

    hKMinus5->Divide(hPiPlus5);
    hKMinus5->SetLineColor(kBlue);
    hKMinus5->SetMarkerColor(kBlue);
    hKMinus5->SetMarkerStyle(21);

    // Draw
    TCanvas* c1 = new TCanvas("c1", "K/pi ratio within #Xi and #bar[#Xi] ", 1700, 1000);
    c1->SetLogy();
    c1->SetGrid();
    hKPlus5->Draw("E1");
    hKMinus5->Draw("E1 SAME");

    // Add legend
    TLegend* leg = new TLegend(0.895, 0.2, 1.0, 0.9);
    leg->AddEntry(hKPlus5, "K^{+}/(#pi^{+} + #pi^{-})", "lep");
    leg->AddEntry(hKMinus5, "K^{-}/(#pi^{+} + #pi^{-})", "lep");
    leg->Draw();

    c1->Update();
    c1->SaveAs("outputxixibar2.png");

    yieldsFile->Close();
}

void drawRatiosXiXibarFIST() {
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.04);

    // Open the ROOT file
    TFile* yieldsFile = new TFile("k2pi_xixibar_yieldsFIST.root", "READ");
    if (!yieldsFile || yieldsFile->IsZombie()) {
        std::cerr << "Error: Could not open k2pi_xixibar_yieldsFIST.root" << std::endl;
        return;
    }

    // Get profiles and convert to TH1D
    TProfile* hKPlusFISTProfile = yieldsFile->Get<TProfile>("hKPlusFISTCent");
    TProfile* hPiPlusFISTProfile = yieldsFile->Get<TProfile>("hPiPlusFISTCent");
    TProfile* hKMinusFISTProfile = yieldsFile->Get<TProfile>("hKMinusFISTCent");
    TProfile* hPiMinusFISTProfile = yieldsFile->Get<TProfile>("hPiMinusFISTCent");

    if (!hKPlusFISTProfile || !hPiPlusFISTProfile || !hKMinusFISTProfile || !hPiMinusFISTProfile) {
        std::cerr << "Error: One or more required TProfile histograms are missing in the file." << std::endl;
        yieldsFile->Close();
        return;
    }

    // Convert TProfile to TH1D
    TH1D* hKPlusFIST = hKPlusFISTProfile->ProjectionX();
    TH1D* hPiPlusFIST = hPiPlusFISTProfile->ProjectionX();
    TH1D* hKMinusFIST = hKMinusFISTProfile->ProjectionX();
    TH1D* hPiMinusFIST = hPiMinusFISTProfile->ProjectionX();

    // Print total number of each hadron in each class (FIST)
std::cout << "K+ (FIST) per class:" << std::endl;
for (int i = 1; i <= hKPlusFIST->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hKPlusFIST->GetBinContent(i) << std::endl;
std::cout << "Pi+ (FIST) per class:" << std::endl;
for (int i = 1; i <= hPiPlusFIST->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hPiPlusFIST->GetBinContent(i) << std::endl;
std::cout << "K- (FIST) per class:" << std::endl;
for (int i = 1; i <= hKMinusFIST->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hKMinusFIST->GetBinContent(i) << std::endl;
std::cout << "Pi- (FIST) per class:" << std::endl;
for (int i = 1; i <= hPiMinusFIST->GetNbinsX(); ++i)
    std::cout << "Bin " << i << ": " << hPiMinusFIST->GetBinContent(i) << std::endl;

    // Debug: Print histogram contents
    std::cout << "hKPlusFIST contents:" << std::endl;
    for (int i = 1; i <= hKPlusFIST->GetNbinsX(); ++i) {
        std::cout << "Bin " << i << ": " << hKPlusFIST->GetBinContent(i) << std::endl;
    }

    // Print histogram contents
    auto printHistogram = [](TH1D* hist, const std::string& name) {
        std::cout << name << " bins and errors:" << std::endl;
        for (int i = 1; i <= hist->GetNbinsX(); ++i) {
            std::cout << "Bin " << i << ": Value = " << hist->GetBinContent(i)
                      << ", Error = " << hist->GetBinError(i) << std::endl;
        }
    };

    printHistogram(hKPlusFIST, "K+");
    printHistogram(hPiPlusFIST, "Pi+");
    printHistogram(hKMinusFIST, "K-");
    printHistogram(hPiMinusFIST, "Pi-");


    // Create a canvas
    TCanvas* c1 = new TCanvas("c1", "K/pi ratio within Xi-Xi-bar FIST", 800, 600);
    c1->SetLogy();
    c1->SetGrid();

    // Scale and divide histograms
    if (hPiPlusFIST->Integral() == 0 || hPiMinusFIST->Integral() == 0) {
        std::cerr << "Error: Denominator histogram has zero integral, cannot divide." << std::endl;
        yieldsFile->Close();
        return;
    }

    hKPlusFIST->Scale(1.);
    hKPlusFIST->Divide(hPiPlusFIST);
    hKPlusFIST->SetTitle("K/#pi ratio within Xi-#bar{Xi} productions FIST;Centrality Class;K/#pi Ratio");
    hKPlusFIST->SetLineColor(kRed);
    hKPlusFIST->SetMarkerColor(kRed);
    hKPlusFIST->SetMarkerStyle(20);
    /*hKFromXi->SetMinimum(0.001); // Set axis range
    hKFromXi->SetMaximum(10);*/
    hKPlusFIST->Draw("E1");

    hKMinusFIST->Divide(hPiMinusFIST);
    hKMinusFIST->SetLineColor(kBlue);
    hKMinusFIST->SetMarkerColor(kBlue);
    hKMinusFIST->SetMarkerStyle(21);
    hKMinusFIST->Draw("E1 SAME");

    // Add legend
    TLegend* leg = new TLegend(0.7, 0.8, 0.9, 0.9);
    leg->AddEntry(hKPlusFIST, "K+/#pi+", "lep");
    leg->AddEntry(hKMinusFIST, "K-/#pi-", "lep");
    leg->Draw();

    // Update canvas and save to file
    c1->Update();
    c1->SaveAs("outputxixibar2FIST.png");

    yieldsFile->Close();
}
/*
void drawRatiosXiXibarpiplusonly() {
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.04);

    // Open the ROOT file
    TFile* yieldsFile = new TFile("k2pi_xixibar_yields.root", "READ");
    if (!yieldsFile || yieldsFile->IsZombie()) {
        std::cerr << "Error: Could not open k2pi_xixibar_yields.root" << std::endl;
        return;
    }

    // Get profiles and convert to TH1D
    TProfile* hKFromXiProfile = yieldsFile->Get<TProfile>("hKFromXiCent");
    TProfile* hPiplusProfile = yieldsFile->Get<TProfile>("hPiplusCent");
    TProfile* hKFromXibarProfile = yieldsFile->Get<TProfile>("hKFromXibarCent");
    

    if (!hKFromXiProfile || !hPiplusProfile || !hKFromXibarProfile) {
        std::cerr << "Error: One or more required TProfile histograms are missing in the file." << std::endl;
        yieldsFile->Close();
        return;
    }

    // Convert TProfile to TH1D
    TH1D* hKFromXi = hKFromXiProfile->ProjectionX();
    TH1D* hPiplus = hPiplusProfile->ProjectionX();
    TH1D* hKFromXibar = hKFromXibarProfile->ProjectionX();
    

    // Debug: Print histogram contents
    std::cout << "hKFromXi contents:" << std::endl;
    for (int i = 1; i <= hKFromXi->GetNbinsX(); ++i) {
        std::cout << "Bin " << i << ": " << hKFromXi->GetBinContent(i) << std::endl;
    }

    // Create a canvas
    TCanvas* c1 = new TCanvas("c1", "K/pi+ ratio within Xi-Xi-bar", 800, 600);
    c1->SetLogy();
    c1->SetGrid();

    // Scale and divide histograms
    if (hPiplus->Integral() == 0) {
        std::cerr << "Error: Denominator histogram has zero integral, cannot divide." << std::endl;
        yieldsFile->Close();
        return;
    }

    hKFromXi->Scale(1.);
    hKFromXi->Divide(hPiplus);
    hKFromXi->SetTitle("K/#pi+ ratio within Xi-#bar{Xi} productions;Centrality Class;K/#pi+ Ratio");
    hKFromXi->SetLineColor(kRed);
    hKFromXi->SetMarkerColor(kRed);
    hKFromXi->SetMarkerStyle(20);
    hKFromXi->SetMinimum(0.001); // Set axis range
    hKFromXi->SetMaximum(10);
    hKFromXi->Draw("E1");

    hKFromXibar->Divide(hPiplus);
    hKFromXibar->SetLineColor(kBlue);
    hKFromXibar->SetMarkerColor(kBlue);
    hKFromXibar->SetMarkerStyle(21);
    hKFromXibar->Draw("E1 SAME");

    // Add legend
    TLegend* leg = new TLegend(0.7, 0.8, 0.9, 0.9);
    leg->AddEntry(hKFromXi, "K+/#pi+", "lep");
    leg->AddEntry(hKFromXibar, "K-/#pi+", "lep");
    leg->Draw();

    // Update canvas and save to file
    c1->Update();
    c1->SaveAs("outputxixibarpiplus.png");

    yieldsFile->Close();
}

void drawRatiosXiXibarpiplusminus() {
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.04);

    // Open the ROOT file
    TFile* yieldsFile = new TFile("k2pi_xixibar_yields.root", "READ");
    if (!yieldsFile || yieldsFile->IsZombie()) {
        std::cerr << "Error: Could not open k2pi_xixibar_yields.root" << std::endl;
        return;
    }

    // Get profiles and convert to TH1D
    TProfile* hKFromXiProfile = yieldsFile->Get<TProfile>("hKFromXiCent");
    TProfile* hPiplusProfile = yieldsFile->Get<TProfile>("hPiplusCent");
    TProfile* hKFromXibarProfile = yieldsFile->Get<TProfile>("hKFromXibarCent");
    TProfile* hPiminusProfile = yieldsFile->Get<TProfile>("hPiminusCent");

    if (!hKFromXiProfile || !hPiplusProfile || !hKFromXibarProfile || !hPiminusProfile) {
        std::cerr << "Error: One or more required TProfile histograms are missing in the file." << std::endl;
        yieldsFile->Close();
        return;
    }

    // Convert TProfile to TH1D
    TH1D* hKFromXi = hKFromXiProfile->ProjectionX();
    TH1D* hPiplus = hPiplusProfile->ProjectionX();
    TH1D* hKFromXibar = hKFromXibarProfile->ProjectionX();
    TH1D* hPiminus = hPiminusProfile->ProjectionX();

    // Debug: Print histogram contents
    std::cout << "hKFromXi contents:" << std::endl;
    for (int i = 1; i <= hKFromXi->GetNbinsX(); ++i) {
        std::cout << "Bin " << i << ": " << hKFromXi->GetBinContent(i) << std::endl;
    }

    // Create a canvas
    TCanvas* c1 = new TCanvas("c1", "K/pi ratio within Xi-Xi-bar", 800, 600);
    c1->SetLogy();
    c1->SetGrid();

    // Scale and divide histograms
    if (hPiplus->Integral() == 0 || hPiminus->Integral() == 0) {
        std::cerr << "Error: Denominator histogram has zero integral, cannot divide." << std::endl;
        yieldsFile->Close();
        return;
    }

    hKFromXi->Scale(1.);
    hKFromXi->Divide(hPiplus);
    hKFromXi->SetTitle("K/#pi ratio within Xi-#bar{Xi} productions;Centrality Class;K/#pi Ratio");
    hKFromXi->SetLineColor(kRed);
    hKFromXi->SetMarkerColor(kRed);
    hKFromXi->SetMarkerStyle(20);
    hKFromXi->SetMinimum(0.001); // Set axis range
    hKFromXi->SetMaximum(10);
    hKFromXi->Draw("E1");

    hKFromXibar->Divide(hPiminus);
    hKFromXibar->SetLineColor(kBlue);
    hKFromXibar->SetMarkerColor(kBlue);
    hKFromXibar->SetMarkerStyle(21);
    hKFromXibar->Draw("E1 SAME");

    // Add legend
    TLegend* leg = new TLegend(0.7, 0.8, 0.9, 0.9);
    leg->AddEntry(hKFromXi, "K+/#pi+", "lep");
    leg->AddEntry(hKFromXibar, "K-/#pi-", "lep");
    leg->Draw();

    // Update canvas and save to file
    c1->Update();
    c1->SaveAs("outputxixibarpiplusminus.png");

    yieldsFile->Close();
}
*/


/*

Long64_t nEntries = pythiaTree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
      pythiaTree.GetEntry(i);

      // Check if event has Ξ-Ξ̄ pair
      bool hasXi = false, hasXibar = false;
      for (auto &trk : evt->tracks) {
          if (trk.pid == 3312)  hasXi = true;
          else if (trk.pid == -3312) hasXibar = true;
          if (hasXi && hasXibar) break;
      }
      
      int nFwd = 0, nKFromXi = 0, nPiFromXi = 0;
      
      for (auto &trk : evt->tracks) {
          if (isForwardTrack(trk)) nFwd++;
      }

      // Count kaons and pions
      int nKfromXi = 0, nPifromXi = 0;
      for (auto &trk : evt->tracks) {
          if (abs(trk.y) > 0.5) continue; // Mid-rapidity cut
          if (abs(trk.pid) == 310) nKfromXi++;    // Kₛ⁰
          else if (abs(trk.pid) == 211) nPifromXi++; // π±
      }

      // Map nFwd to centrality bin (in increasing order)
      double cIndex = 9.5;
      for (auto p : percentiles) {
          if (double(nFwd) > p) break;
          cIndex -= 1.0;
      }
      
      
      hKFromXiCent.Fill(cIndex, nKFromXi);
      hPiFromXiCent.Fill(cIndex, nPiFromXi);
  }


    for (Long64_t i = 0; i < pythiaTree.GetEntries(); i++) {
        pythiaTree.GetEntry(i);

        // Check if event has Ξ-Ξ̄ pair
        bool hasXi = false, hasXibar = false;
        for (auto &trk : evt->tracks) {
            if (trk.pid == 3312)  hasXi = true;
            else if (trk.pid == -3312) hasXibar = true;
            if (hasXi && hasXibar) break;
        }

        // Count kaons and pions
        int nKfromXi = 0, nPifromXi = 0;
        for (auto &trk : evt->tracks) {
            if (abs(trk.y) > 0.5) continue; // Mid-rapidity cut
            if (abs(trk.pid) == 310) nKfromXi++;    // Kₛ⁰
            else if (abs(trk.pid) == 211) nPifromXi++; // π±
        }

        // Fill profiles based on Ξ-Ξ̄ presence
        double cIndex = getCentralityIndex(evt); // Your centrality mapping
        if (hasXi && hasXibar) {
            hKXiEvents.Fill(cIndex, nK);
            hPiXiEvents.Fill(cIndex, nPi);
        }


to check if the xi xibar pair comes from a common ancestor. My code counts any xi xibar pair 
even if they come from different strings. (uncorrelated production)
only xi xibar from the same string are likely to produce a local
strangeness enhancement. Mixing in uncorrelated pairs dilutes the signal. 
bool isSameStringXiXibar = false;
for (auto &trk1 : evt->tracks) {
    if (trk1.pid != 3312) continue; // Find a Ξ⁻
    for (auto &trk2 : evt->tracks) {
        if (trk2.pid != -3312) continue; // Find a Ξ̄⁺
        if (shareCommonAncestor(trk1, trk2)) { // Hypothetical function
            isSameStringXiXibar = true;
            break;
        }
    }
    if (isSameStringXiXibar) break;
}
if (!isSameStringXiXibar) continue; // Skip uncorrelated Ξ-Ξ̄

or

// Add this helper function (place near the top of your code)
bool isSameStringXiXibar(const RootEvent* evt) {
    std::vector<int> xi_indices, xibar_indices;
    
    // Step 1: Find all Ξ and Ξ̄ in the event
    for (size_t i = 0; i < evt->tracks.size(); i++) {
        const auto& trk = evt->tracks[i];
        if (trk.pid == 3312) xi_indices.push_back(i);
        else if (trk.pid == -3312) xibar_indices.push_back(i);
    }
    
    // Step 2: Check if any Ξ-Ξ̄ pair shares ancestors
    for (int xi_idx : xi_indices) {
        for (int xibar_idx : xibar_indices) {
            // PYTHIA-specific: Check if they have common ancestor indices
            if (evt->tracks[xi_idx].mother1 == evt->tracks[xibar_idx].mother1 || 
                evt->tracks[xi_idx].mother1 == evt->tracks[xibar_idx].mother2 ||
                evt->tracks[xi_idx].mother2 == evt->tracks[xibar_idx].mother1) {
                return true;
            }
        }
    }
    return false;
}

// In your PYTHIA event loop, replace the Ξ-Ξ̄ check with:
bool hasXiXibarSameString = isSameStringXiXibar(evt);
if (!hasXiXibarSameString) continue; // Skip non-same-string pairs

// Rest of your existing PYTHIA analysis...


// Detailed status check (using PYTHIA's event record)
      
      bool fromStringFrag = false;
      for (int j = 0; j < pythia.event.size(); ++j) {
          auto& p = pythia.event[j];
          if ((p.id() == 3312 || p.id() == -3312) && 
              (p.statusAbs() == 83 || p.statusAbs() == 84)) {
              fromStringFrag = true;
              break;
          }
      }
      
      //if (!fromStringFrag) continue; // Skip non-string-fragmented Ξ-Ξ̄


      hKFromXi contents:
Bin 1: 0.384615
Bin 2: 0.125
Bin 3: 0.5
Bin 4: 0.363636
Bin 5: 0.708333
Bin 6: 0.470588
Bin 7: 0.95122
Bin 8: 1.02632
Bin 9: 1.04
Bin 10: 0.769231
*/