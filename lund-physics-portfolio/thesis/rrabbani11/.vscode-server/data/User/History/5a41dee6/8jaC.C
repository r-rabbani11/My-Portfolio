/*
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

#include "TFile.h"
#include "TTree.h"
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
  vector<double> percentiles = {1.,5.,10.,15.,20., 30., 40., 50., 70., 100.};
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
void calibration(vector<string> filenames = {"../pythia.root"}) {

  // Create a TChain and add the files
  TChain tree("t");
  for (auto &fn : filenames) {
    tree.Add(fn.c_str());
  }
    
  // Create a calibration histogram 
  TH1D hCalib("hCalib", "nFwd", 100, 0, 200);
  
  RootEvent* evt = nullptr;
  tree.SetBranchAddress("events", &evt);

  // Loop over entries
  Long64_t nEntries = tree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree.GetEntry(i);
    
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
  TCanvas* c1 = new TCanvas();
  hCalib->Draw();
  c1->SetLogy();
  for (double p : percentiles) {
    TLine* l = new TLine(p, 0, p, hCalib->GetBinContent(hCalib->FindBin(p)));
    l->Draw();
  }
}

void drawRatios() {
  // Open the results file
 
  TFile* yieldsFile = new TFile("yields.root", "READ");
  // Load the histograms
  TProfile* pi = yieldsFile->Get<TProfile>("hPiCent");
  TProfile* p = yieldsFile->Get<TProfile>("hPCent");
  TProfile* k = yieldsFile->Get<TProfile>("hKCent");
  TProfile* L = yieldsFile->Get<TProfile>("hLCent");
  TProfile* X = yieldsFile->Get<TProfile>("hXCent");
  TProfile* O = yieldsFile->Get<TProfile>("hOmegaCent");
  TCanvas* c1 = new TCanvas("c1", "Yield ratios", 500,900);
  c1->SetLogy();
  k->Scale(2.);
  k->Divide(pi);
  k->SetLineColor(kBlack);
  k->SetLineWidth(2);
  k->Draw();
  L->Divide(pi);
  L->SetLineColor(kGreen);
  L->Draw("same");
    
}

vector<string> anaFiles = {"../pythia-000.root"};

void analyze(vector<string> filenames = {"../pythia.root"}) {

  // Open the calibration file
  TFile calibFile("calibration.root", "READ");
  // Load the calibration histogram
  TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
  auto percentiles = findPercentiles(hCalib);
  delete hCalib;
  // Create profiles binned in centrality class index.
  TProfile hPiCent("hPiCent", "nPi", 10, 0, 10);
  TProfile hPCent("hPCent", "nP", 10, 0, 10);
  TProfile hKCent("hKCent", "nK", 10, 0, 10);
  TProfile hLCent("hLCent", "nL", 10, 0, 10);
  TProfile hXiCent("hXCent", "nXi", 10, 0, 10);
  TProfile hOmegaCent("hOmegaCent", "nOmega", 10, 0, 10);

  // Create a TChain and add the files
  TChain tree("t");
  for (auto &fn : filenames) {
    tree.Add(fn.c_str());
  }
  
  RootEvent* evt = nullptr;
  tree.SetBranchAddress("events", &evt);

  // Loop over entries
  Long64_t nEntries = tree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree.GetEntry(i);
    
    // Forward multiplicity
    int nFwd = 0;
    // Number of pions
    int nPi = 0;
    // Number of protons
    int nP = 0;
    // Number of Kaons
    int nK = 0;
    // Number of Lambda
    int nL = 0;
    // Number of Xi
    int nXi = 0;
    // Number of Omega
    int nO = 0;

    // Count tracks
    for (auto &trk : evt->tracks) {
      if (isForwardTrack(trk)) ++nFwd;
      else if (abs(trk.y) < 0.5) {
        int id = abs(trk.pid);
        if (id == 211) ++nPi;
        else if(id == 2212) ++nP;
        else if (id == 310) ++nK;
        else if (id == 3122) ++nL;
        else if (id == 3312) ++nXi;
        else if (id == 3334) ++nO;
      }  
    }
    // Find the calibration index
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
  TFile outFile("yields.root", "RECREATE");
  hPiCent.Write();
  hPCent.Write();
  hKCent.Write();
  hLCent.Write();
  hXiCent.Write();
  hOmegaCent.Write();

  outFile.Close();
  delete evt;
}
*/



















/*


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
  vector<double> percentiles = {1.,5.,10.,15.,20., 30., 40., 50., 70., 100.};
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
void calibration(vector<string> filenames = {"../pythia.root"}) {

  // Create a TChain and add the files
  TChain tree("t");
  for (auto &fn : filenames) {
    tree.Add(fn.c_str());
  }
    
  // Create a calibration histogram 
  TH1D hCalib("hCalib", "nFwd", 100, 0, 200);
  
  RootEvent* evt = nullptr;
  tree.SetBranchAddress("events", &evt);

  // Loop over entries
  Long64_t nEntries = tree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree.GetEntry(i);
    
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
  TCanvas* c1 = new TCanvas();
  hCalib->Draw();
  c1->SetLogy();
  for (double p : percentiles) {
    TLine* l = new TLine(p, 0, p, hCalib->GetBinContent(hCalib->FindBin(p)));
    l->Draw();
  }
}

vector<string> anaFiles = {"../pythia-000.root"};

void analyze(vector<string> filenames = {"../pythia.root"}) {

  // Open the calibration file
  TFile calibFile("calibration.root", "READ");
  // Load the calibration histogram
  TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
  auto percentiles = findPercentiles(hCalib);
  delete hCalib;
  // Create profiles binned in centrality class index.
  TProfile hPiCent("hPiCent", "nPi", 10, 0, 10);
  TProfile hPCent("hPCent", "nP", 10, 0, 10);
  TProfile hKCent("hKCent", "nK", 10, 0, 10);
  TProfile hLCent("hLCent", "nL", 10, 0, 10);
  TProfile hXiCent("hXCent", "nXi", 10, 0, 10);
  TProfile hOmegaCent("hOmegaCent", "nOmega", 10, 0, 10);

  // Create a TChain and add the files
  TChain tree("t");
  for (auto &fn : filenames) {
    tree.Add(fn.c_str());
  }
  
  RootEvent* evt = nullptr;
  tree.SetBranchAddress("events", &evt);

  // Loop over entries
  Long64_t nEntries = tree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree.GetEntry(i);
    
    // Forward multiplicity
    int nFwd = 0;
    // Number of pions
    int nPi = 0;
    // Number of protons
    int nP = 0;
    // Number of Kaons
    int nK = 0;
    // Number of Lambda
    int nL = 0;
    // Number of Xi
    int nXi = 0;
    // Number of Omega
    int nO = 0;

    // Count tracks
    for (auto &trk : evt->tracks) {
      if (isForwardTrack(trk)) ++nFwd;
      else if (abs(trk.y) < 0.5) {
        int id = abs(trk.pid);
        if (id == 211) ++nPi;
        else if(id == 2212) ++nP;
        else if (id == 310) ++nK;
        else if (id == 3122) ++nL;
        else if (id == 3312) ++nXi;
        else if (id == 3334) ++nO;
      }  
    }
    // Find the calibration index
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
  TFile outFile("yields.root", "RECREATE");
  hPiCent.Write();
  hPCent.Write();
  hKCent.Write();
  hLCent.Write();
  hXiCent.Write();
  hOmegaCent.Write();

  outFile.Close();
  delete evt;
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
  // Open the results file for simulated data
  gStyle->SetOptStat(0);   // Remove the stats box
  gStyle->SetTitleFontSize(0.04);

  TFile* yieldsFile = new TFile("yields.root", "READ");
  if (!yieldsFile || yieldsFile->IsZombie()) {
      std::cerr << "Error: Could not open yields.root" << std::endl;
      return;
  }

  // Convert TProfile to TH1D for simulated data
  TH1D* pi_sim = yieldsFile->Get<TProfile>("hPiCent")->ProjectionX();
  TH1D* k_sim = yieldsFile->Get<TProfile>("hKCent")->ProjectionX();
  TH1D* L_sim = yieldsFile->Get<TProfile>("hLCent")->ProjectionX();
  TH1D* X_sim = yieldsFile->Get<TProfile>("hXCent")->ProjectionX();
  TH1D* O_sim = yieldsFile->Get<TProfile>("hOmegaCent")->ProjectionX();

  // Load experimental ratios
  TGraphAsymmErrors* k_exp = loadExperimentalRatio("Kaon-piratio.root", "Table 36");
  TGraphAsymmErrors* L_exp = loadExperimentalRatio("lambda-piratio.root", "Table 37");
  TGraphAsymmErrors* X_exp = loadExperimentalRatio("xi-piratio.root", "Table 38");
  TGraphAsymmErrors* O_exp = loadExperimentalRatio("omega-piratio.root", "Table 39");

  if (!k_exp || !L_exp || !X_exp || !O_exp) {
      std::cerr << "Error: Failed to load experimental ratios." << std::endl;
      return;
  }

  // Adjust x-coordinates of HEPData points to match simulation's centrality indices (9.5, 8.5, ..., 0.5)
  auto adjustXCoordinates = [](TGraphAsymmErrors* graph) {
    if (!graph) return;
    int nPoints = graph->GetN();
    for (int i = 0; i < nPoints; ++i) {
      double x, y;
      graph->GetPoint(i, x, y);
      double newX = 9.5 - i; // Map Class I (0-0.95%) to 9.5, Class II to 8.5, etc.
      graph->SetPoint(i, newX, y);
      // Set x-error to span the entire bin (width = 1.0)
      graph->SetPointEXlow(i, 0.5);
      graph->SetPointEXhigh(i, 0.5);
    }
  };

  adjustXCoordinates(k_exp);
  adjustXCoordinates(L_exp);
  adjustXCoordinates(X_exp);
  adjustXCoordinates(O_exp);

  // Create a canvas for the ratio plot
  TCanvas* c1 = new TCanvas("c1", "Yield Ratios: Simulated vs Experimental", 500, 900);
  c1->SetLogy();
  c1->SetGrid();

  // Simulated data ratios
  k_sim->Scale(2.);
  k_sim->Divide(pi_sim);
  k_sim->SetTitle("Particle Yield Ratios vs Centrality;Centrality Class;Yield Ratio");
  k_sim->SetLineWidth(1);
  k_sim->SetMarkerStyle(20); // Circle marker
  k_sim->SetMarkerSize(2);
  k_sim->SetMarkerColor(kBlack);
  k_sim->SetMinimum(0.0001);
  k_sim->SetMaximum(0.2);
  k_sim->Draw("E1");  // Error bars only, no lines

  L_sim->Scale(2.);
  L_sim->Divide(pi_sim);
  L_sim->SetLineWidth(1);
  L_sim->SetMarkerStyle(21);  // Square marker
  L_sim->SetMarkerSize(2);
  L_sim->SetMarkerColor(kGreen + 2);
  L_sim->Draw("E1 SAME");

  X_sim->Scale(6.);
  X_sim->Divide(pi_sim);
  X_sim->SetLineWidth(1);
  X_sim->SetMarkerStyle(22);  // Diamond marker
  X_sim->SetMarkerSize(2);
  X_sim->SetMarkerColor(kBlue);
  X_sim->Draw("E1 SAME");

  O_sim->Scale(16.);
  O_sim->Divide(pi_sim);
  O_sim->SetLineWidth(1);
  O_sim->SetMarkerStyle(23);  // Triangle-up marker
  O_sim->SetMarkerSize(2);
  O_sim->SetMarkerColor(kRed);
  O_sim->Draw("E1 SAME");

  // Experimental data ratios
  k_exp->SetMarkerStyle(24);  // Cross marker
  k_exp->SetMarkerSize(2);
  k_exp->SetMarkerColor(kMagenta);
  k_exp->Draw("P SAME");

  L_exp->SetMarkerStyle(25);  // Plus marker
  L_exp->SetMarkerSize(2);
  L_exp->SetMarkerColor(kCyan);
  L_exp->Draw("P SAME");

  X_exp->SetMarkerStyle(26);  // Asterisk marker
  X_exp->SetMarkerSize(2);
  X_exp->SetMarkerColor(kOrange);
  X_exp->Draw("P SAME");

  O_exp->SetMarkerStyle(27);  // Diamond marker
  O_exp->SetMarkerSize(2);
  O_exp->SetMarkerColor(kYellow);
  O_exp->Draw("P SAME");

  // Beautify legend
  TLegend* leg = new TLegend(0.9, 0.6, 1.0, 0.9);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->AddEntry(k_sim, "K/#pi (Sim, #times2)", "ep");
  leg->AddEntry(L_sim, "#Lambda/#pi (Sim, #times2)", "ep");
  leg->AddEntry(X_sim, "#Xi/#pi (Sim, #times6)", "ep");
  leg->AddEntry(O_sim, "#Omega/#pi (Sim, #times16)", "ep");
  leg->AddEntry(k_exp, "K/#pi (Exp)", "p");
  leg->AddEntry(L_exp, "#Lambda/#pi (Exp)", "p");
  leg->AddEntry(X_exp, "#Xi/#pi (Exp)", "p");
  leg->AddEntry(O_exp, "#Omega/#pi (Exp)", "p");
  leg->Draw();

  c1->Update();
}

void drawRatios() {
  // Open the results file
  gStyle->SetOptStat(0);   // Remove the stats box
  gStyle->SetTitleFontSize(0.04);
 
  TFile* yieldsFile = new TFile("yields.root", "READ");

  // Convert TProfile to TH1D
  TH1D* pi = yieldsFile->Get<TProfile>("hPiCent")->ProjectionX();
  TH1D* p = yieldsFile->Get<TProfile>("hPCent")->ProjectionX();
  TH1D* k = yieldsFile->Get<TProfile>("hKCent")->ProjectionX();
  TH1D* L = yieldsFile->Get<TProfile>("hLCent")->ProjectionX();
  TH1D* X = yieldsFile->Get<TProfile>("hXCent")->ProjectionX();
  TH1D* O = yieldsFile->Get<TProfile>("hOmegaCent")->ProjectionX();

  TCanvas* c1 = new TCanvas("c1", "Yield ratios", 500, 900);
  c1->SetLogy();
  //c1->SetLogx();
  c1->SetGrid();
  
  // Now divide TH1D histograms (not TProfile)
  k->Scale(2.);
  k->Divide(pi);
  k->SetTitle("Particle Yield Ratios vs Centrality;Centrality Class;Yield Ratio");
  k->SetLineWidth(1);  
  k->SetMarkerStyle(20); // Circle marker
  k->SetMarkerSize(2); 
  k->SetMarkerColor(kBlack);
  k->SetMinimum(0.001);
  k->SetMaximum(0.2);
  //k->GetXaxis()->SetLimits(0.7, 100);
  k->Draw("E1");  // Error bars only, no lines



  L->Scale(2.);
  L->Divide(pi);
  L->SetLineWidth(1);
  L->SetMarkerStyle(21);  // Square marker
  L->SetMarkerSize(2);
  L->SetMarkerColor(kGreen+2);
  L->Draw("E1 SAME");

  X->Scale(6.);
  X->Divide(pi);
  X->SetLineWidth(1);
  X->SetMarkerStyle(22);  // Diamond marker
  X->SetMarkerSize(2);
  X->SetMarkerColor(kBlue);
  X->Draw("E1 SAME");

  O->Scale(16.);
  O->Divide(pi);
  O->SetLineWidth(1);
  O->SetMarkerStyle(23);  // Triangle-up marker
  O->SetMarkerSize(2);
  O->SetMarkerColor(kRed);
  O->Draw("E1 SAME");

  // Beautify legend
  TLegend* leg = new TLegend(0.9, 0.6, 1.0, 0.9);
  leg->SetBorderSize(0);  
  leg->SetFillStyle(0); 
  leg->SetTextFont(42);                 
  leg->SetTextSize(0.03);
  leg->AddEntry(k, "K/#pi (#times2)", "ep");
  leg->AddEntry(L, "#Lambda/#pi (#times2)", "ep");
  leg->AddEntry(X, "#Xi/#pi (#times6)", "ep");
  leg->AddEntry(O, "#Omega/#pi (#times16)", "ep");
  leg->Draw();

  c1->Update();
}




*/