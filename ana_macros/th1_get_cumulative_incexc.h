#ifndef __TH1_GET_CUMULATIVE_INCEXC__
#define __TH1_GET_CUMULATIVE_INCEXC__

#include <Rtypes.h>
#include <TH1.h>
#include <TString.h>

TH1 *GetHistCumulativeIncExc(const TH1 *hIn, const Bool_t forward = kTRUE, const char *suffix = "_cumulative");
TH1 *GetHistCumulativeIncExc(const TH1 *hIn, const Bool_t forward, const char *suffix)
{
   const Int_t firstX = hIn->GetXaxis()->GetFirst();
   const Int_t lastX = hIn->GetXaxis()->GetLast();
   const Int_t firstY = (hIn->GetDimension() > 1) ? hIn->GetYaxis()->GetFirst() : 1;
   const Int_t lastY = (hIn->GetDimension() > 1) ? hIn->GetYaxis()->GetLast() : 1;
   const Int_t firstZ = (hIn->GetDimension() > 1) ? hIn->GetZaxis()->GetFirst() : 1;
   const Int_t lastZ = (hIn->GetDimension() > 1) ? hIn->GetZaxis()->GetLast() : 1;

   TH1 *hintegrated = static_cast<TH1 *>(hIn->Clone(TString(hIn->GetName()) + suffix));
   hintegrated->Reset();
   if (forward) { // Forward computation
      for (Int_t binz = firstZ; binz <= lastZ; ++binz) {
         for (Int_t biny = firstY; biny <= lastY; ++biny) {
            for (Int_t binx = firstX; binx <= lastX; ++binx) {
               const Int_t bin = hintegrated->GetBin(binx, biny, binz);
               Double_t sum = hIn->GetBinContent(bin);
               if (binz != firstZ)
                  sum += hintegrated->GetBinContent(hintegrated->GetBin(binx, biny, binz - 1));
               if (biny != firstY)
                  sum += hintegrated->GetBinContent(hintegrated->GetBin(binx, biny - 1, binz));
               if (binx != firstX)
                  sum += hintegrated->GetBinContent(hintegrated->GetBin(binx - 1, biny, binz));
               if (binz != firstZ && biny != firstY)
                  sum -= hintegrated->GetBinContent(hintegrated->GetBin(binx, biny - 1, binz - 1));
               if (biny != firstY && binx != firstX)
                  sum -= hintegrated->GetBinContent(hintegrated->GetBin(binx - 1, biny - 1, binz));
               if (binx != firstX && binz != firstZ)
                  sum -= hintegrated->GetBinContent(hintegrated->GetBin(binx - 1, biny, binz - 1));
               if (binz != firstZ && biny != firstY && binx != firstX)
                  sum += hintegrated->GetBinContent(hintegrated->GetBin(binx - 1, biny - 1, binz - 1));
               hintegrated->SetBinContent(bin, sum);
               if (hIn->GetSumw2()->fN) {
                  Double_t esum = hIn->GetSumw2()->GetAt(bin);
                  if (binz != firstZ)
                     esum += hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx, biny, binz - 1));
                  if (biny != firstY)
                     esum += hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx, biny - 1, binz));
                  if (binx != firstX)
                     esum += hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx - 1, biny, binz));
                  if (binz != firstZ && biny != firstY)
                     esum -= hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx, biny - 1, binz - 1));
                  if (biny != firstY && binx != firstX)
                     esum -= hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx - 1, biny - 1, binz));
                  if (binx != firstX && binz != firstZ)
                     esum -= hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx - 1, biny, binz - 1));
                  if (binz != firstZ && biny != firstY && binx != firstX)
                     esum += hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx - 1, biny - 1, binz - 1));
                  hintegrated->GetSumw2()->SetAt(esum, bin);
               }
            }
         }
      }
   } else { // Backward computation
      for (Int_t binz = lastZ; binz >= firstZ; --binz) {
         for (Int_t biny = lastY; biny >= firstY; --biny) {
            for (Int_t binx = lastX; binx >= firstX; --binx) {
               const Int_t bin = hintegrated->GetBin(binx, biny, binz);
               Double_t sum = hIn->GetBinContent(bin);
               if (binz != lastZ)
                  sum += hintegrated->GetBinContent(hintegrated->GetBin(binx, biny, binz + 1));
               if (biny != lastY)
                  sum += hintegrated->GetBinContent(hintegrated->GetBin(binx, biny + 1, binz));
               if (binx != lastX)
                  sum += hintegrated->GetBinContent(hintegrated->GetBin(binx + 1, biny, binz));
               if (binz != lastZ && biny != lastY)
                  sum -= hintegrated->GetBinContent(hintegrated->GetBin(binx, biny + 1, binz + 1));
               if (biny != lastY && binx != lastX)
                  sum -= hintegrated->GetBinContent(hintegrated->GetBin(binx + 1, biny + 1, binz));
               if (binx != lastX && binz != lastZ)
                  sum -= hintegrated->GetBinContent(hintegrated->GetBin(binx + 1, biny, binz + 1));
               if (binz != lastZ && biny != lastY && binx != lastX)
                  sum += hintegrated->GetBinContent(hintegrated->GetBin(binx + 1, biny + 1, binz + 1));
               hintegrated->SetBinContent(bin, sum);
               if (hIn->GetSumw2()->fN) {
                  Double_t esum = hIn->GetSumw2()->GetAt(bin);
                  if (binz != lastZ)
                     esum += hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx, biny, binz + 1));
                  if (biny != lastY)
                     esum += hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx, biny + 1, binz));
                  if (binx != lastX)
                     esum += hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx + 1, biny, binz));
                  if (binz != lastZ && biny != lastY)
                     esum -= hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx, biny + 1, binz + 1));
                  if (biny != lastY && binx != lastX)
                     esum -= hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx + 1, biny + 1, binz));
                  if (binx != lastX && binz != lastZ)
                     esum -= hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx + 1, biny, binz + 1));
                  if (binz != lastZ && biny != lastY && binx != lastX)
                     esum += hintegrated->GetSumw2()->GetAt(hintegrated->GetBin(binx + 1, biny + 1, binz + 1));
                  hintegrated->GetSumw2()->SetAt(esum, bin);
               }
            }
         }
      }
   }
   return hintegrated;
}

#endif /* __TH1_GET_CUMULATIVE_INCEXC__ */
