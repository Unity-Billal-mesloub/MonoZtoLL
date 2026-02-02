#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TString.h>

#include <string>
#include <iostream>

#include "Cross_section.h"

std::string GetDYExclusiveName(const Int_t lowerBound, const Int_t upperBound) {
    return "DYJetsToLL_M-50_HT-" + std::to_string(lowerBound) + "to" + ((upperBound >= 0) ? std::to_string(upperBound) : "Inf") + "_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8";
}

std::string GetGGName(const std::string product) {
    return "GluGluToContinToZZTo" + product + "_TuneCP5_13TeV-mcfm701-pythia8";
}

void xAna_merge_backgrounds(const char *outputFile="/eos/user/y/yuehshun/merged_SkimmedFiles_2017Background.root", const char *outputMergedSpace="/eos/user/y/yuehshun/SkimmedFiles_2017Background_Lists_outputmerged.tmp");
void xAna_merge_backgrounds(const char *outputFile, const char *outputMergedSpace) {
    const std::string commonFilenameSuffix = "_merged.root";
    const std::string tSubDir = "Kinematics_Variable_afterEachLeptonSelection";

    TH1 *h_ZbosonPt = nullptr;
    TH1 *h_pfMetCorrPt = nullptr;
    TH2 *h2_ZbosonPt_pfMetCorrPt = nullptr;

    auto f_addup = [&h_ZbosonPt, &h_pfMetCorrPt, &h2_ZbosonPt_pfMetCorrPt, &outputMergedSpace, &tSubDir, &commonFilenameSuffix](const std::string inputFilename, const Float_t xsec) {
        TFile *tfToAdd = TFile::Open((outputMergedSpace + std::string("/") + inputFilename + commonFilenameSuffix).c_str());
        TH1 *h_totevent = tfToAdd->Get<TH1>("Event_Variable/h_totevent");
        std::cerr << "Cross section: " << xsec << std::endl;
        std::cerr << "Total event before preselection: " << h_totevent->GetBinContent(2) << std::endl;
        // GlobalConstants::Lumi2017 is in pb (10^-12 barn) and the cross section unit here should be fb (10^-15 barn)
        // 1 pb = 1000 fb
        const Double_t corr = (GlobalConstants::Lumi2017 * 1000) * xsec / h_totevent->GetBinContent(2);
        h_totevent->Delete();
        TH1 *h_ZbosonPtToAdd = tfToAdd->Get<TH1>((tSubDir + "/h_ZbosonPt").c_str());
        // (\sum_{all events being filled in this bin} weight^2) for each bin 
        h_ZbosonPtToAdd->Scale(corr);
        if (h_ZbosonPt) {
            h_ZbosonPt->Add(h_ZbosonPtToAdd);
            h_ZbosonPtToAdd->Delete();
        } else {
            h_ZbosonPt = h_ZbosonPtToAdd;
            h_ZbosonPt->SetDirectory(0);
        }
        TH1 *h_pfMetCorrPtToAdd = tfToAdd->Get<TH1>((tSubDir + "/h_pfMetCorrPt").c_str());
        h_pfMetCorrPtToAdd->Scale(corr);
        if (h_pfMetCorrPt) {
            h_pfMetCorrPt->Add(h_pfMetCorrPtToAdd);
            h_pfMetCorrPtToAdd->Delete();
        } else {
            h_pfMetCorrPt = h_pfMetCorrPtToAdd;
            h_pfMetCorrPt->SetDirectory(0);
        }
        TH2 *h2_ZbosonPt_pfMetCorrPt_ToAdd = tfToAdd->Get<TH2>((tSubDir + "/h2_ZbosonPt_pfMetCorrPt").c_str());
        h2_ZbosonPt_pfMetCorrPt_ToAdd->Scale(corr);
        if (h2_ZbosonPt_pfMetCorrPt) {
            h2_ZbosonPt_pfMetCorrPt->Add(h2_ZbosonPt_pfMetCorrPt_ToAdd);
            h2_ZbosonPt_pfMetCorrPt_ToAdd->Delete();
        } else {
            h2_ZbosonPt_pfMetCorrPt = h2_ZbosonPt_pfMetCorrPt_ToAdd;
            h2_ZbosonPt_pfMetCorrPt->SetDirectory(0);
        }
        tfToAdd->Close();
    };

    f_addup(GetDYExclusiveName(0, 70), GlobalConstants::HT0to70CS);
    f_addup(GetDYExclusiveName(70, 100), GlobalConstants::HT70to100CS);
    f_addup(GetDYExclusiveName(100, 200), GlobalConstants::HT100to200CS);
    f_addup(GetDYExclusiveName(200, 400), GlobalConstants::HT200to400CS);
    f_addup(GetDYExclusiveName(400, 600), GlobalConstants::HT400to600CS);
    f_addup(GetDYExclusiveName(600, 800), GlobalConstants::HT600to800CS);
    f_addup(GetDYExclusiveName(800, 1200), GlobalConstants::HT800to1200CS);
    f_addup(GetDYExclusiveName(1200, 2500), GlobalConstants::HT1200to2500CS);
    f_addup(GetDYExclusiveName(2500, -1), GlobalConstants::HT2500toInfCS);
    f_addup(GetGGName("2e2mu"), GlobalConstants::gg_ZZ_2e2mu_CS);
    f_addup(GetGGName("2e2nu"), GlobalConstants::gg_ZZ_2e2nu_CS);
    f_addup(GetGGName("2e2tau"), GlobalConstants::gg_ZZ_2e2tau_CS);
    f_addup(GetGGName("2mu2nu"), GlobalConstants::gg_ZZ_2mu2nu_CS);
    f_addup(GetGGName("2mu2tau"), GlobalConstants::gg_ZZ_2mu2tau_CS);
    f_addup(GetGGName("4e"), GlobalConstants::gg_ZZ_4e_CS);
    f_addup(GetGGName("4mu"), GlobalConstants::gg_ZZ_4mu_CS);
    f_addup(GetGGName("4tau"), GlobalConstants::gg_ZZ_4tau_CS);
    f_addup("ST_tW_antitop_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8", GlobalConstants::ST_tW_antitop_5f_CS);
    f_addup("ST_tW_top_5f_inclusiveDecays_TuneCP5_13TeV-powheg-pythia8", GlobalConstants::ST_tW_top_5f_CS);
    f_addup("TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8", GlobalConstants::TTTo2L2Nu_CS);
    f_addup("TTWJetsToLNu_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8", GlobalConstants::TTWJetsToLNu_CS);
    f_addup("TTWJetsToQQ_TuneCP5_13TeV-amcatnloFXFX-madspin-pythia8", GlobalConstants::TTWJetsToQQ_CS);
    f_addup("TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::TTZToLLNuNu_CS);
    f_addup("TTZToQQ_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::TTZToQQ_CS);
    f_addup("WWTo2L2Nu_TuneCP5_13TeV-powheg-pythia8", GlobalConstants::qq_WW_2L2Nu_CS);
    f_addup("WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::WWZ_CS);
    f_addup("WZTo3LNu_TuneCP5_13TeV-amcatnloFXFX-pythia8", GlobalConstants::WZ_3LNu_CS);
    f_addup("WZZ_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::WZZ_CS);
    f_addup("ZZTo2L2Nu_TuneCP5_13TeV_powheg_pythia8", GlobalConstants::ZZ_2L2Nu_CS);
    f_addup("ZZTo4L_TuneCP5_13TeV_powheg_pythia8", GlobalConstants::ZZ_4L_CS);
    f_addup("ZZZ_TuneCP5_13TeV-amcatnlo-pythia8", GlobalConstants::ZZZ_CS);

    TFile *tfOut = TFile::Open(outputFile, "RECREATE");
    tfOut->mkdir(tSubDir.c_str())->cd();
    h_ZbosonPt->Write();
    h_pfMetCorrPt->Write();
    h2_ZbosonPt_pfMetCorrPt->Write();
    tfOut->Close();
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        xAna_merge_backgrounds(argv[1], argv[2]);
    } else if (argc > 1) {
        xAna_merge_backgrounds(argv[1]);
    } else {
        xAna_merge_backgrounds();
    }
    return 0;
}
