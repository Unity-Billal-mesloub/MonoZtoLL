/// plotAll.C
///
/// SPDX-FileCopyrightText: 2020-2022 Yueh-Shun Li (Shamrock Lee) <44064051+ShamrockLee@users.noreply.github.com>
///
/// This ROOT macro simply plots all the histograms in the given ROOT file / ROOT directory
/// and save the image into the specified output directory.
///
/// Additional options can be specified to adjust the histogram.
/// It also optionally accept a pointer to function that modifies each histogram,
/// two such functions, rebinTo100 and adjustWithJSONTitle, are provided in this file.

#include <TROOT.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TKey.h>
#include <TLeaf.h>
#include <TPad.h>
#include <TCanvas.h>
#include <TString.h>
#include <TH1.h>
#include <TLegend.h>
#include <TMath.h>
#include <TStyle.h>
#include <TList.h>

#include <functional>
#include <iostream>
#include <cstdio>

void plotAll(TString pathFileIn, TString dirOut, const Bool_t plotSubdir = true, const Bool_t normalize = false, const Bool_t logy = false, const Option_t *optionDraw = "", const std::function<TH1*(TH1*)> funAdjustHist = nullptr);

void plotAll(TDirectory *tdirIn, TString dirOut, const Bool_t plotSubdir, const Bool_t normalize, const Bool_t logy, const Option_t *optionDraw, const std::function<TH1*(TH1*)> funAdjustHist) {
  const TString seperatorPath("/");
  gSystem->mkdir(dirOut);
  if (dirOut.Length() > 1 && dirOut.EndsWith(seperatorPath)) {
    dirOut.Resize(dirOut.Length()-1);
  }
  if (!gPad) {
    gROOT->MakeDefCanvas();
  }
  gPad->SetLogy(logy);
  for (auto&& keyRaw: *tdirIn->GetListOfKeys()) {
    TKey *key = (TKey *) keyRaw;
    if (key == nullptr) continue;
    TString name = key->GetName();
    TObject *obj = key->ReadObj();
    if (obj == nullptr) continue;
    if (plotSubdir && obj->IsA()->InheritsFrom("TDirectory")) {
      plotAll(static_cast<TDirectory*>(obj), dirOut + seperatorPath + name, true, normalize, logy, optionDraw, funAdjustHist);
    }
    if (obj->IsA()->InheritsFrom("TH1")
        && ! obj->IsA()->InheritsFrom("TH2")
        && ! obj->IsA()->InheritsFrom("TH3")
      ) {
      TH1 *hist = (TH1 *) key->ReadObj();
      if (normalize) {
        hist->Scale(1.0 / hist->Integral());
      }
      if (funAdjustHist != nullptr) hist = funAdjustHist(hist);
      const char *optionDrawNew = optionDraw;
      if (hist->GetNbinsX() <= 13 && (! TString(optionDraw).Contains("Text"))) {
        optionDrawNew = (TString(optionDraw) + "Text").Data();
      }
      hist->Draw(optionDrawNew);
      gStyle->SetOptStat(111111);
      gPad->Print(dirOut + seperatorPath + name + ".svg");
    } else if (obj->IsA()->InheritsFrom("TH2")) {
      TH1 *hist = (TH1 *) key->ReadObj();
      const char *optionDrawNew = "colz";
      hist->Draw(optionDrawNew);
      gStyle->SetOptStat(111111);
      gPad->Print(dirOut + seperatorPath + name + ".svg");
    }
    obj->Delete();
  }
}

void plotAll(TSystemFile *tsysfIn, const TString dirOut, const Bool_t plotSubdir, const Bool_t normalize, const Bool_t logy, const Option_t *optionDraw, const std::function<TH1*(TH1*)> funAdjustHist) {
  std::cerr << "dirOut: " << dirOut << std::endl;
  std::cerr << "tsysfIn path: " << tsysfIn->GetTitle() << " basename: " << tsysfIn->GetName() << std::endl;
  std::cerr << "isDirectory: " << tsysfIn->IsDirectory() << std::endl;
  const TString seperatorPath ("/");
  gSystem->mkdir(dirOut);
  if (tsysfIn->IsDirectory()) {
    TSystemDirectory *tsysdIn = new TSystemDirectory(gSystem->BaseName(tsysfIn->GetName()), tsysfIn->GetTitle());
    delete tsysfIn;
    std::cerr << "tsysdIn: " << tsysdIn << std::endl;
    TList *listOfFiles = tsysdIn->GetListOfFiles();
    std::cerr << "listOfFiles: " << listOfFiles << std::endl;
    for (const auto&& tsysfChildRaw: *listOfFiles) {
      std::cerr << "tsysfChildRaw: " << tsysfChildRaw << std::endl; 
      TSystemFile *tsysfChild = static_cast<TSystemFile *>(tsysfChildRaw);
      TString basenameFileInChild(tsysfChildRaw->GetName());
      if (basenameFileInChild == "." || basenameFileInChild == "..") continue;
      std::cerr << "basenameFileInChild: " << basenameFileInChild << std::endl;
      TString basenameDirOutChild = basenameFileInChild.EndsWith(".root") ? basenameFileInChild(0, basenameFileInChild.Length() - 5) : basenameFileInChild;
      plotAll(tsysfChild, (dirOut.EndsWith(seperatorPath) ? dirOut : dirOut + seperatorPath) + basenameDirOutChild, plotSubdir, normalize, logy, optionDraw, funAdjustHist);
    }
  } else {
    TFile *fileIn = TFile::Open(tsysfIn->GetTitle() + seperatorPath + tsysfIn->GetName());
    delete tsysfIn;
    plotAll(fileIn, dirOut, plotSubdir, normalize, logy, optionDraw, funAdjustHist);
    fileIn->Close();
  }
}

void plotAll(const TString pathFileIn, const TString dirOut, const Bool_t plotSubdir, const Bool_t normalize, const Bool_t logy, const Option_t *optionDraw, const std::function<TH1*(TH1*)> funAdjustHist) {
  // TString basenameFileIn = gSystem->GetFromPipe(
  //     Form("file=%s; test=${file##*/}; echo \"${test%%.root}\"",
  //     pathFileIn.Data()));
  plotAll(new TSystemFile(pathFileIn, pathFileIn), dirOut, plotSubdir, normalize, logy, optionDraw, funAdjustHist);
}

void plotAll(const char* pathFileIn, const char* dirout, const Bool_t plotSubdir = true, const Bool_t normalize = false, const Bool_t logy = false, const Option_t *optionDraw = "", const std::function<TH1*(TH1*)> funAdjustHist = nullptr) {
  plotAll((TString) pathFileIn, (TString) dirout, plotSubdir, normalize, logy, optionDraw, funAdjustHist);
}

TH1* rebinTo100(TH1 * hist) {
  TH1 *resultHist = hist;
  const Int_t nBins = hist->GetNbinsX();
  if (nBins) {
    const Double_t binWidth = hist->GetBinWidth(1);
    if (TMath::Abs(binWidth - TMath::Nint(binWidth)) < 0.001 && TMath::Nint(binWidth) > 0.5 && nBins >= 200) {
      resultHist = hist->Rebin(nBins / 100);
    }
  }
  return resultHist;
}

#if true

#include "nlohmann/json.hpp"

TH1* adjustWithJSONTitle(TH1 *hist) {
  const char* strSetting = hist->GetTitle();
  std::cerr << "name: " << hist->GetName() << ", ";
  std::cerr << "strSetting: "  << strSetting << std::endl;
  const nlohmann::json jSetting = nlohmann::json::parse(strSetting);
  const double alignment = jSetting["alignment"].get<double>();
  const int binDensityOrder = jSetting["binDensityOrder"].get<int>();
  const bool isLowerAssigned = jSetting["isLowerAssigned"].get<bool>();
  const int lowerLimitBins = jSetting["lowerLimitBins"].get<int>();
  const bool isUpperAssigned = jSetting["isUpperAssigned"].get<bool>();
  const int upperLimitBins = jSetting["upperLimitBins"].get<int>();
  if (!isLowerAssigned || !isUpperAssigned) {
    // Round to 12 to 20 instead of 100
    constexpr int biasFactor = 2.;
    int lowerLimitBinsCorrect = lowerLimitBins;
    int upperLimitBinsCorrect = upperLimitBins;
    const int lowerLimitBinsNonzero = hist->FindFirstBinAbove() + lowerLimitBins - 1;
    const int upperLimitBinsNonzero = hist->FindLastBinAbove() + lowerLimitBins;
    if (upperLimitBinsNonzero == lowerLimitBins - 1) {
      if (isLowerAssigned) {
        upperLimitBinsCorrect = lowerLimitBins + 1;
      } else if (isUpperAssigned) {
        lowerLimitBinsCorrect = upperLimitBins - 1;
      } else {
        lowerLimitBinsCorrect = -(alignment > 0);
        upperLimitBinsCorrect = 1 - (alignment > 0);
      }
    }
    if (isLowerAssigned) {
      if (upperLimitBinsNonzero > 0) {
        const int limitOrder = TMath::Ceil(TMath::Log10(upperLimitBinsNonzero / biasFactor + (upperLimitBinsNonzero % biasFactor != 0)));
        const int limitBase = static_cast<int>(TMath::Power(10, limitOrder));
        upperLimitBinsCorrect = limitBase * (upperLimitBinsNonzero / limitBase + (upperLimitBinsNonzero % limitBase != 0));
      } else {
        upperLimitBinsCorrect = (alignment < 0);
      }
    } else if (isUpperAssigned) {
      if (lowerLimitBinsNonzero < 0) {
        const int limitOrder = TMath::Ceil(TMath::Log10(-(lowerLimitBinsNonzero / biasFactor + (lowerLimitBinsNonzero % biasFactor != 0))));
        const int limitBase = static_cast<int>(TMath::Power(10, limitOrder));
        lowerLimitBinsCorrect = limitBase * (lowerLimitBinsNonzero / limitBase + (lowerLimitBinsNonzero % limitBase != 0));
      } else {
        lowerLimitBinsCorrect = -(alignment > 0);
      }
    } else if (lowerLimitBinsNonzero <= 0  && upperLimitBinsNonzero >= 0) {
      int upperLimitOrder = -1, lowerLimitOrder = -1;
      if (upperLimitBinsNonzero > 0) {
        upperLimitOrder = TMath::Max(0, static_cast<int>(TMath::Ceil(TMath::Log10(upperLimitBinsNonzero / biasFactor + (upperLimitBinsNonzero % biasFactor != 0)))));
      }
      if (lowerLimitBinsNonzero < 0) {
        lowerLimitOrder = TMath::Max(0, static_cast<int>(TMath::Ceil(TMath::Log10(-(lowerLimitBinsNonzero / biasFactor + (lowerLimitBinsNonzero % biasFactor != 0))))));
      }
      const int limitOrder = TMath::Max(upperLimitOrder, lowerLimitOrder);
      const int limitBase = static_cast<int>(TMath::Power(10, limitOrder));
      if (limitOrder < 0) {
        std::fprintf(stderr, "lowerLimitBinsNonzero: %d, upperLimitBinsNonzero: %d", lowerLimitBinsNonzero, upperLimitBinsNonzero);
        Fatal("adjustWithJSONTitle", "limitOrder (%d) == -1", limitOrder);
      }
      if (upperLimitBinsNonzero > 0) {
        upperLimitBinsCorrect = limitBase * (upperLimitBinsNonzero / limitBase + (upperLimitBinsNonzero % limitBase != 0));
      } else {
        upperLimitBinsCorrect = (alignment < 0);
      }
      if (lowerLimitBinsNonzero < 0) {
        lowerLimitBinsCorrect = limitBase * (lowerLimitBinsNonzero / limitBase + (lowerLimitBinsNonzero % limitBase != 0));
      } else {
        lowerLimitBinsCorrect = -(alignment > 0);
      }
    } else {
      const int nBinsNonzero = upperLimitBinsNonzero - lowerLimitBinsNonzero;
      const int limitOrder = TMath::Ceil(TMath::Log10(nBinsNonzero / biasFactor + (nBinsNonzero % biasFactor != 0)));
      const int limitBase = static_cast<int>(TMath::Power(10, limitOrder));
      if (upperLimitBinsNonzero > 0) {
        upperLimitBinsCorrect = limitBase * (upperLimitBinsNonzero / limitBase + (upperLimitBinsNonzero % limitBase != 0));
      } else {
        upperLimitBinsCorrect = (alignment < 0);
      }
      if (lowerLimitBinsNonzero < 0) {
        lowerLimitBinsCorrect = limitBase * (lowerLimitBinsNonzero / limitBase + (lowerLimitBinsNonzero % limitBase != 0));
      } else {
        lowerLimitBinsCorrect = -(alignment > 0);
      }
    }
    const double binWidth = TMath::Power(10, -binDensityOrder);
    const double lowerLimit = alignment + binWidth * lowerLimitBinsCorrect;
    const double upperLimit = alignment + binWidth * upperLimitBinsCorrect;
    const double nBinsNew = upperLimitBinsCorrect - lowerLimitBinsCorrect;
    TH1 *histNew = new TH1D(TString(hist->GetName()) + "Adjusted", hist->GetName(), nBinsNew, lowerLimit, upperLimit);
    for (int iBin = TMath::Max(lowerLimitBins, lowerLimitBinsCorrect) + 1; iBin <= TMath::Min(upperLimitBins, upperLimitBinsCorrect); ++iBin) {
      histNew->SetBinContent(iBin - lowerLimitBinsCorrect, hist->GetBinContent(iBin - lowerLimitBins));
      histNew->SetBinError(iBin - lowerLimitBinsCorrect, hist->GetBinError(iBin - lowerLimitBins));
    }
    histNew->SetBinContent(0, hist->GetBinContent(0));
    histNew->SetBinContent(nBinsNew + 1, hist->GetBinContent(upperLimitBins - lowerLimitBins + 1));
    histNew->SetEntries(hist->GetEntries());
    hist->Delete();
    return histNew;
  }
  hist->SetTitle(hist->GetName());
  if (hist->GetEntries()) {
    constexpr double borderRatio = 1.1;
    double yMin, yMax;
    hist->GetMinimumAndMaximum(yMin, yMax);
    if (yMin > 0) {
      hist->GetYaxis()->SetRangeUser(0., yMax * borderRatio);
    } else if (yMax < 0) {
      hist->GetYaxis()->SetRangeUser(yMin * borderRatio, 0.);
    } else if (yMin == 0 && yMax == 0) {
      hist->GetYaxis()->SetRangeUser(0., 1.);
    }
  }
  return hist;
}
#endif
