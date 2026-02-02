#include <TError.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>
#include <TSystem.h>

#include <string>
#include <iostream>

#include "Cross_section.h"

std::string GetDYExclusiveName(const Int_t lowerBound, const Int_t upperBound) {
    return "DYJetsToLL_M-50_HT-" + std::to_string(lowerBound) + "to" + ((upperBound >= 0) ? std::to_string(upperBound) : "Inf") + "_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8";
}

std::string GetGGName(const std::string product) {
    return "GluGluToContinToZZTo" + product + "_TuneCP5_13TeV-mcfm701-pythia8";
}

void xAna_scale_backgrounds(const char *outputMergedScaledSpace="/eos/user/y/yuehshun/SkimmedFiles_2017Background_Lists_outputmerged_scaled.tmp", const char *outputMergedSpace="/eos/user/y/yuehshun/SkimmedFiles_2017Background_Lists_outputmerged.tmp");
void xAna_scale_backgrounds(const char *outputMergedScaledSpace, const char *outputMergedSpace) {
    const std::string filenameSuffixMerged = "_merged.root";
    const std::string filenameSuffixScaled = "_merged_scaled.root";
    const std::string tSubDir = "Kinematics_Variable_afterEachLeptonSelection";

    (void)gSystem->mkdir(outputMergedScaledSpace);

    auto f_scale = [&outputMergedSpace, &outputMergedScaledSpace, &tSubDir, &filenameSuffixMerged, &filenameSuffixScaled](const std::string inputFilename, const Float_t xsec) {
        const std::string pathFileIn = outputMergedSpace + std::string("/") + inputFilename + filenameSuffixMerged;
        const std::string pathFileOut = outputMergedScaledSpace + std::string("/") + inputFilename + filenameSuffixScaled;
        TFile *tfIn = TFile::Open(pathFileIn.c_str());
        if (!tfIn || tfIn->IsZombie()) {
          Warning("xAna_scale_backgrounds", "Input file %s failed to open", pathFileIn.c_str());
          return;
        }
        if (!tfIn->GetListOfKeys()->GetSize()) {
          Warning("xAna_scale_backgrounds", "Input file %s has no keys", pathFileIn.c_str());
          return;
        }
        TFile *tfOut = TFile::Open(pathFileOut.c_str(), "RECREATE");
        TH1 *h_totevent = tfIn->Get<TH1>("Event_Variable/h_totevent");
        std::cout << "Cross section: " << xsec << std::endl;
        std::cout << "Total event before preselection: " << h_totevent->GetBinContent(2) << std::endl;
        // GlobalConstants::Lumi2017 is in pb (10^-12 barn) and the cross section unit here should be fb (10^-15 barn)
        // 1 pb = 1000 fb
        const Double_t corr = (GlobalConstants::Lumi2017 * 1000) * xsec / h_totevent->GetBinContent(2);
        std::cout << "corr: " << corr << std::endl;
        std::cout << std::endl;
        h_totevent->Delete();
        tfOut->mkdir(tSubDir.c_str())->cd();
        for (std::string nameH1: {
            "h_ZbosonPt",
            "h_ZbosonPt_HasLPairsEle",
            "h_ZbosonPt_HasVtxEle",
            "h_ZbosonPt_NoTauEle",
            "h_ZbosonPt_LPairsPassPtEle",
            "h_ZbosonPt_ZMassCutEle",
            "h_ZbosonPt_NoExtraLEle",
            "h_ZbosonPt_HasTHINJetEle",
            "h_pfMetCorrPt" }) {
          TH1 *h1Scaled = tfIn->Get<TH1>((tSubDir + "/" + nameH1).c_str());
          h1Scaled->Scale(corr);
          h1Scaled->Write();
          h1Scaled->Delete();
        }
        TH2 *h2_ZbosonPt_pfMetCorrPt_Scaled = tfIn->Get<TH2>((tSubDir + "/h2_ZbosonPt_pfMetCorrPt").c_str());
        h2_ZbosonPt_pfMetCorrPt_Scaled->Scale(corr);
        h2_ZbosonPt_pfMetCorrPt_Scaled->Write();
        h2_ZbosonPt_pfMetCorrPt_Scaled->Delete();
        tfOut->Close();
        tfIn->Close();
    };

    f_scale(GetDYExclusiveName(0, 70), GlobalConstants::HT0to70CS);
    f_scale(GetDYExclusiveName(70, 100), GlobalConstants::HT70to100CS);
    f_scale(GetDYExclusiveName(100, 200), GlobalConstants::HT100to200CS);
    f_scale(GetDYExclusiveName(200, 400), GlobalConstants::HT200to400CS);
    f_scale(GetDYExclusiveName(400, 600), GlobalConstants::HT400to600CS);
    f_scale(GetDYExclusiveName(600, 800), GlobalConstants::HT600to800CS);
    f_scale(GetDYExclusiveName(800, 1200), GlobalConstants::HT800to1200CS);
    f_scale(GetDYExclusiveName(1200, 2500), GlobalConstants::HT1200to2500CS);
    f_scale(GetDYExclusiveName(2500, -1), GlobalConstants::HT2500toInfCS);
    f_scale(GetGGName("2e2mu"), GlobalConstants::gg_ZZ_2e2mu_CS);
    f_scale(GetGGName("2e2nu"), GlobalConstants::gg_ZZ_2e2nu_CS);
    f_scale(GetGGName("2e2tau"), GlobalConstants::gg_ZZ_2e2tau_CS);
    f_scale(GetGGName("2mu2nu"), GlobalConstants::gg_ZZ_2mu2nu_CS);
    f_scale(GetGGName("2mu2tau"), GlobalConstants::gg_ZZ_2mu2tau_CS);
    f_scale(GetGGName("4e"), GlobalConstants::gg_ZZ_4e_CS);
    f_scale(GetGGName("4mu"), GlobalConstants::gg_ZZ_4mu_CS);
    f_scale(GetGGName("4tau"), GlobalConstants::gg_ZZ_4tau_CS);
    f_scale("ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8", GlobalConstants::ST_tW_antitop_5f_CS);
    f_scale("ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8", GlobalConstants::ST_tW_top_5f_CS);
    f_scale("TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8", GlobalConstants::TTTo2L2Nu_CS);
    f_scale("TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8", GlobalConstants::TTWJetsToLNu_CS);
    f_scale("TTWJetsToQQ_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8", GlobalConstants::TTWJetsToQQ_CS);
    f_scale("TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::TTZToLLNuNu_CS);
    f_scale("TTZToQQ_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::TTZToQQ_CS);
    f_scale("WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8", GlobalConstants::qq_WW_2L2Nu_CS);
    f_scale("WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::WWZ_CS);
    f_scale("WZTo3LNu_TuneCP5_13TeV-amcatnloFXFX-pythia8", GlobalConstants::WZ_3LNu_CS);
    f_scale("WZZ_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::WZZ_CS);
    f_scale("ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia8", GlobalConstants::ZZ_2L2Nu_CS);
    f_scale("ZZTo4L_TuneCP5_13TeV_powheg_pythia8", GlobalConstants::ZZ_4L_CS);
    f_scale("ZZZ_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::ZZZ_CS);
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        xAna_scale_backgrounds(argv[1], argv[2]);
    } else if (argc > 1) {
        xAna_scale_backgrounds(argv[1]);
    } else {
        xAna_scale_backgrounds();
    }
    return 0;
}
