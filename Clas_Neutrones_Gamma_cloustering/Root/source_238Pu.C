#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TPaveText.h>
#include <TString.h>
#include <iostream>
#include <algorithm>

void source_238Pu() {
    const Double_t gainP1   = 0.00115512;
    const Double_t offsetP1 = 10.9821;

    TFile *file0 = TFile::Open("/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/clouster0_stilbene_pu_13C.root");
    TFile *file1 = TFile::Open("/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/clouster1_stilbene_pu_13C.root");
    if (!file0 || file0->IsZombie()) { std::cerr << "Error abriendo cluster0\n"; return; }
    if (!file1 || file1->IsZombie()) { std::cerr << "Error abriendo cluster1\n"; return; }

    TTree *t0 = (TTree*)file0->Get("wf");
    TTree *t1 = (TTree*)file1->Get("wf");
    if (!t0) { std::cerr << "No se encontro TTree 'wf' en cluster0\n"; return; }
    if (!t1) { std::cerr << "No se encontro TTree 'wf' en cluster1\n"; return; }

    TFile *output = new TFile("Stilbene_238PU_13C.root", "RECREATE");

    // Rango en canal (integral)
    const Double_t chMin = -9550.0;
    const Double_t chMax = -4500.0;

    // Convertimos a energía (misma calibración)
    const Double_t Emin = chMin * gainP1 + offsetP1;
    const Double_t Emax = chMax * gainP1 + offsetP1;

    // Histogramas calibrados (diferente binning, sin problema)
    TH1F *h0_cal = new TH1F("h0_cal", "Stilbene spectrum for ^{238}Pu + ^{13}C;Energy\\xEDa [MeV];Counts", 256, Emin, Emax);
    TH1F *h1_cal = new TH1F("h1_cal", "Stilbene spectrum for ^{238}Pu + ^{13}C;Energy\\xEDa [MeV];Counts", 512, Emin, Emax);
    h0_cal->Sumw2(); h1_cal->Sumw2();

    // Llenado DIRECTO desde los trees en energía (Entries correctos)
    TString expr0 = Form("(%g*integral + %g)>>h0_cal", gainP1, offsetP1);
    TString expr1 = Form("(%g*integral + %g)>>h1_cal", gainP1, offsetP1);
    t0->Draw(expr0, "", "goff");
    t1->Draw(expr1, "", "goff");

    // Estilos
    h0_cal->SetLineColor(kBlue); h0_cal->SetLineWidth(2);
    h1_cal->SetLineColor(kRed);  h1_cal->SetLineWidth(2);

    // Evitar “corte” en Y
    Double_t maxY = std::max(h0_cal->GetMaximum(), h1_cal->GetMaximum());
    h0_cal->SetMaximum(1.10 * maxY);

    // DESACTIVAR stats automáticas (para que no interfieran)
    h0_cal->SetStats(0);
    h1_cal->SetStats(0);

    TCanvas *c1 = new TCanvas("c1", "Stilbene spectrum for 238Pu+13C", 900, 650);

    // Dibujar superpuestos
    h0_cal->Draw("HIST");
    h1_cal->Draw("HIST SAME");

    // Leyenda
    TLegend *leg = new TLegend(0.65, 0.75, 0.88, 0.88);
    leg->AddEntry(h0_cal, "Neutrons", "l");
    leg->AddEntry(h1_cal, "gamma", "l");
    leg->Draw();

    // -------------------------
    // Cajas manuales con stats (SIEMPRE correctas)
    // -------------------------
    auto makeBox = [](Double_t x1, Double_t y1, Double_t x2, Double_t y2,
                      Color_t color, const char* title, TH1* h) {
        TPaveText *pt = new TPaveText(x1, y1, x2, y2, "NDC");
        pt->SetFillStyle(0);
        pt->SetBorderSize(1);
        pt->SetTextAlign(12);  // left-middle
        pt->SetTextColor(color);

        pt->AddText(title);
        pt->AddText(Form("Entries = %.0f", h->GetEntries()));
        pt->AddText(Form("Mean    = %.6g", h->GetMean()));
        pt->AddText(Form("StdDev  = %.6g", h->GetStdDev()));
        return pt;
    };

    TPaveText *box0 = makeBox(0.12, 0.78, 0.34, 0.92, kBlue, "Cluster 0", h0_cal);
    TPaveText *box1 = makeBox(0.12, 0.60, 0.34, 0.74, kRed,  "Cluster 1", h1_cal);

    box0->Draw();
    box1->Draw();

    c1->Modified();
    c1->Update();

    // Guardar
    output->cd();
    h0_cal->Write();
    h1_cal->Write();
    c1->Write();
    //output->Close();
    file0->Close();
    file1->Close();
}
