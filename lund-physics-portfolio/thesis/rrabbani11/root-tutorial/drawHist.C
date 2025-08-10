// example ROOT macro which illustrates 1-dimensional histograms (TH1F), 1-dimensional functions (TF1), 
// random number generation, and basic drawing options

// for all class and function documentation, see: https://root.cern/doc/master/annotated.html
// (for example, for TH1 go to https://root.cern/doc/master/classTH1.html)
// some very useful tutorials are here: https://root.cern/doc/master/group__Tutorials.html

// run this macro in root using either '.x drawHist.C(1000000)' (uncompiled) or '.x drawHist.C++(1000000)' (compiled)

#include "TStyle.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TCanvas.h"

void drawHist(Int_t n = 100)
{
  gStyle->SetOptStat(111111);

  // histogram
  TH1F* h = new TH1F("h","Example histogram",100,-5,5);
  h->GetXaxis()->SetTitle("x");
  h->GetYaxis()->SetTitle("dN/dx");
  h->GetYaxis()->SetTitleOffset(1.3);

  // normalized Gaussian function
  TF1* f = new TF1("f","[0]*exp(-0.5*((x-[1])/[2])**2)/sqrt(2*TMath::Pi()*[2]*[2])",-5,5); // [0],[1],[2] are the parameters of the function
  f->SetParameters(1.1,0.1,1.1); // initialize parameters of the function

  // initialize random number generation
  gRandom = new TRandom3(0); // use TRandom3 not TRandom

  // fill histogram h with n random numbers in a Gaussian distribution
  for(Int_t i = 0; i < n; i++)
    {
      // numerical method for turning two random numbers (x1,x2) with uniform distributions into a Gaussian distribution (y) with mean=0 and sigma=1
      /*
      Double_t x1 = gRandom->Rndm(); // generates a random double between 0 and 1
      Double_t x2 = gRandom->Rndm();
      x1 = 2.*x1-1.;
      x2 = 2.*x2-1.;
      Double_t z = x1*x1+x2*x2;
      if(z>=1.) continue;
      z = sqrt((-2.*log(z))/z);
      Double_t y = x1*z;
      */

      // or you can just use the ROOT function to get a random number from a function
      Double_t y = f->GetRandom();
      
      // fill histogram
      h->Fill(y);
    }
  h->Scale(1./(Double_t)n);
  h->Scale(1./h->GetBinWidth(1));


  // canvas for displaying the result
  TCanvas *c = new TCanvas("c","Example canvas",1000,800);
  c->cd();

  // draw histogram h in canvas
  h->SetMinimum(0); // some drawing options
  h->SetLineWidth(2);
  h->SetLineColor(1); // 1-black, 2-red, 3-green, 4-blue etc...
  h->DrawCopy();

  // fit histogram h with the function f
  h->Fit(f,"0"); // useful options are Q0RNM, see: https://root.cern/doc/master/classTH1.html#HFitOpt

  // draw f on the same canvas as h
  f->SetLineColor(kViolet);
  f->SetLineWidth(2);
  f->DrawCopy("Lsame");

  // print some information to stdout
  cout << endl << "The integral of the Gaussian is " << f->GetParameter(0) << " +/- " << f->GetParError(0) << endl;

}
