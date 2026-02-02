#include <TArrayD.h>
#include <TError.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TString.h>

#include "th1_get_cumulative_incexc.h"

TH1 *GetPunziSignificance(TH1 *h_signal, TH1 *h_background, TH1 *&h_signalEfficiency, TH1 *&h_backgroundCumu) {
    TH1 *h_punziSignificance = GetHistCumulativeIncExc(h_signal, false, "_punzi");
    h_punziSignificance->Scale(
        1. / h_punziSignificance->GetBinContent(
            h_punziSignificance->GetXaxis()->GetFirst(),
            h_punziSignificance->GetYaxis()->GetFirst(),
            h_punziSignificance->GetZaxis()->GetFirst()
        )
    );
    h_signalEfficiency = static_cast<TH1 *>(h_punziSignificance->Clone(Form("%s_sigeff", h_signal->GetName())));
    TH1 *h_backgroundCumuSqrtPlus1 = GetHistCumulativeIncExc(h_background, false, "_cumu_sqrt_plus_one");
    h_backgroundCumu = static_cast<TH1 *>(h_backgroundCumuSqrtPlus1->Clone(Form("%s_backcumu", h_background->GetName())));
    // h_backgroundEfficiency->Scale(
    //     1. / h_backgroundEfficiency->GetBinContent(
    //         h_backgroundEfficiency->GetXaxis()->GetFirst(),
    //         h_backgroundEfficiency->GetYaxis()->GetFirst(),
    //         h_backgroundEfficiency->GetZaxis()->GetFirst()
    //     )
    // );
    const int idxXFirstBkg = h_backgroundCumuSqrtPlus1->GetXaxis()->GetFirst();
    const int idxXLastBkg = h_backgroundCumuSqrtPlus1->GetXaxis()->GetLast();
    const int idxYFirstBkg = (h_backgroundCumuSqrtPlus1->GetDimension() >=2) ? h_backgroundCumuSqrtPlus1->GetYaxis()->GetFirst() : 1;
    const int idxYLastBkg = (h_backgroundCumuSqrtPlus1->GetDimension() >=2) ? h_backgroundCumuSqrtPlus1->GetYaxis()->GetLast() : 1;
    const int idxZFirstBkg = (h_backgroundCumuSqrtPlus1->GetDimension() >=3) ? h_backgroundCumuSqrtPlus1->GetZaxis()->GetFirst() : 1;
    const int idxZLastBkg = (h_backgroundCumuSqrtPlus1->GetDimension() >=3) ? h_backgroundCumuSqrtPlus1->GetZaxis()->GetLast() : 1;
    for (int iz = idxZFirstBkg; iz <= idxZLastBkg; ++iz) {
        for (int iy = idxYFirstBkg; iy <= idxYLastBkg; ++iy) {
            for (int ix = idxXFirstBkg; ix <= idxXLastBkg; ++ix) {
                const int bin = h_backgroundCumuSqrtPlus1->GetBin(ix, iy, iz);
                // Info("xAna_monoZ_PunziSignificance.C: GetPunziSignificance", "iz: %d, iy: %d, ix: %d, bin: %d", iz, iy, ix, bin);
                // if (bin < 1) {
                //     Error("xAna_monoZ_PunziSignificance.C: GetPunziSignificance", "bin (%d) < 1", bin);
                // }
                const Double_t binContentOr0 = TMath::Max(0., h_backgroundCumuSqrtPlus1->GetBinContent(ix, iy, iz));
                h_backgroundCumuSqrtPlus1->SetBinContent(bin, 1. + TMath::Sqrt(binContentOr0));
                TArrayD *p_sumw2 = h_backgroundCumuSqrtPlus1->GetSumw2();
                if (p_sumw2->fN) {
                    p_sumw2->SetAt(p_sumw2->At(bin) * 0.25 / binContentOr0, bin);
                }
            }
        }
    }
    h_punziSignificance->Divide(h_backgroundCumuSqrtPlus1);
    h_backgroundCumuSqrtPlus1->Delete();
    return h_punziSignificance;
}

TH1 *GetPunziSignificance(TH1 *h_signal, TH1 *h_background) {
    TH1 *h_signalEfficiency = nullptr, *h_backgroundCumu = nullptr;
    TH1 *result = GetPunziSignificance(h_signal, h_background, h_signalEfficiency, h_backgroundCumu);
    h_signalEfficiency->Delete();
    h_backgroundCumu->Delete();
    return result;
}

void xAna_monoZ_PunziSignificance(const char *fileOutput, const char *fileSignal, const char *fileBackground) {
    TFile *tfSignal = TFile::Open(fileSignal);
    TFile *tfBackground = TFile::Open(fileBackground);
    TFile *tfOut = TFile::Open(fileOutput, "RECREATE");
    const char *subdir = "Kinematics_Variable_afterEachLeptonSelection";
    tfOut->mkdir(subdir)->cd();
    {
        const char *histname = "h_ZbosonPt";
        TH1 *h_signal = tfSignal->Get<TH1>(Form("%s/%s", subdir, histname));
        TH1 *h_background = tfBackground->Get<TH1>(Form("%s/%s", subdir, histname));
        TH1 *h_sigeff = nullptr;
        TH1 *h_backcumu = nullptr;
        TH1 *h_punzi = GetPunziSignificance(h_signal, h_background, h_sigeff, h_backcumu);
        h_punzi->Write();
        h_sigeff->Write();
        h_backcumu->Write();
        h_signal->Delete();
        h_background->Delete();
    }
    {
        const char *histname = "h_pfMetCorrPt";
        TH1 *h_signal = tfSignal->Get<TH1>(Form("%s/%s", subdir, histname));
        TH1 *h_background = tfBackground->Get<TH1>(Form("%s/%s", subdir, histname));
        TH1 *h_sigeff = nullptr;
        TH1 *h_backcumu = nullptr;
        TH1 *h_punzi = GetPunziSignificance(h_signal, h_background, h_sigeff, h_backcumu);
        h_punzi->Write();
        h_sigeff->Write();
        h_backcumu->Write();
        h_signal->Delete();
        h_background->Delete();
    }
    {
        const char *histname = "h2_ZbosonPt_pfMetCorrPt";
        TH2 *h_signal = tfSignal->Get<TH2>(Form("%s/%s", subdir, histname));
        TH2 *h_background = tfBackground->Get<TH2>(Form("%s/%s", subdir, histname));
        TH1 *h_sigeff_raw = nullptr;
        TH1 *h_backcumu_raw = nullptr;
        TH2 *h_punzi = static_cast<TH2 *>(GetPunziSignificance(h_signal, h_background, h_sigeff_raw, h_backcumu_raw));
        h_punzi->Write();
        static_cast<TH2 *>(h_sigeff_raw)->Write();
        static_cast<TH2 *>(h_backcumu_raw)->Write();
        h_signal->Delete();
        h_background->Delete();
    }
    tfOut->Close();
    tfSignal->Close();
    tfBackground->Close();
}

int main(int argc, char *argv[]) {
    if (argc <= 3) {
        Error("xAna_monoZ_PunziSignificance.C: main", "Expect fileOutput fileSignal fileBackground");
        return 1;
    }
    xAna_monoZ_PunziSignificance(argv[1], argv[2], argv[3]);
    return 0;
}
