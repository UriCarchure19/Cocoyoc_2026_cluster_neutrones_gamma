#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TPaveText.h>
#include <TString.h>
#include <iostream>
#include <algorithm>

void total_onda_source_238Pu() {
    const Double_t gainP1   = 0.00115512;
    const Double_t offsetP1 = 10.9821;

    // -------------------------
    // Abrir archivos
    // -------------------------
    TFile *file0 = TFile::Open("/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/clouster0_stilbene_pu_13C.root");
    TFile *file1 = TFile::Open("/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/clouster1_stilbene_pu_13C.root");
    TFile *file2 = TFile::Open("/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/Total_ondas_stilbene_pu_13C.root");

    if (!file0 || file0->IsZombie()) { std::cerr << "Error abriendo cluster0\n"; return; }
    if (!file1 || file1->IsZombie()) { std::cerr << "Error abriendo cluster1\n"; return; }
    if (!file2 || file2->IsZombie()) { std::cerr << "Error abriendo total (file2)\n"; return; }

    TTree *t0 = (TTree*)file0->Get("wf");
    TTree *t1 = (TTree*)file1->Get("wf");
    TTree *t2 = (TTree*)file2->Get("wf");

    if (!t0) { std::cerr << "No se encontro TTree 'wf' en cluster0\n"; return; }
    if (!t1) { std::cerr << "No se encontro TTree 'wf' en cluster1\n"; return; }
    if (!t2) { std::cerr << "No se encontro TTree 'wf' en total\n";    return; }

    // -------------------------
    // Salida
    // -------------------------
    TFile *output = new TFile("Stilbene_238PU_13C.root", "RECREATE");

    // Rango en canal (integral)
    const Double_t chMin = -9550.0;
    const Double_t chMax = -4500.0;

    // Convertimos a energía (misma calibración)
    const Double_t Emin = chMin * gainP1 + offsetP1;
    const Double_t Emax = chMax * gainP1 + offsetP1;

    // -------------------------
    // Histogramas calibrados
    // -------------------------
    TH1F *h0_cal = new TH1F("h0_cal", "Clusters calibrados;Energ\\xEDa [MeV];Cuentas", 256, Emin, Emax);
    TH1F *h1_cal = new TH1F("h1_cal", "Clusters calibrados;Energ\\xEDa [MeV];Cuentas", 512, Emin, Emax);

    // Histograma "Total" (arriba) con stats por default
    TH1F *h2_cal = new TH1F("h2_cal", "Total calibrado;Energ\\xEDa [MeV];Cuentas", 512, Emin, Emax);

    h0_cal->Sumw2(); h1_cal->Sumw2(); h2_cal->Sumw2();

    // Llenado directo desde trees en energía
    TString expr0 = Form("(%g*integral + %g)>>h0_cal", gainP1, offsetP1);
    TString expr1 = Form("(%g*integral + %g)>>h1_cal", gainP1, offsetP1);
    TString expr2 = Form("(%g*integral + %g)>>h2_cal", gainP1, offsetP1);

    t0->Draw(expr0, "", "goff");
    t1->Draw(expr1, "", "goff");
    t2->Draw(expr2, "", "goff");

    // Estilos
    h0_cal->SetLineColor(kBlue); h0_cal->SetLineWidth(2);
    h1_cal->SetLineColor(kRed);  h1_cal->SetLineWidth(2);
    h2_cal->SetLineColor(kBlack); h2_cal->SetLineWidth(2);

    // -------------------------
    // Canvas con 2 pads (arriba / abajo)
    // -------------------------
    TCanvas *c1 = new TCanvas("c1", "Espectros Calibrados", 900, 800);

    // Pad superior (Total)
    TPad *pad_top = new TPad("pad_top", "Total", 0.0, 0.52, 1.0, 1.0);
    pad_top->SetBottomMargin(0.03);
    pad_top->SetTopMargin(0.10);
    pad_top->Draw();

    // Pad inferior (Clusters combinados)
    TPad *pad_bot = new TPad("pad_bot", "Clusters", 0.0, 0.0, 1.0, 0.52);
    pad_bot->SetTopMargin(0.03);
    pad_bot->SetBottomMargin(0.12);
    pad_bot->Draw();

    // =========================
    // Arriba: TOTAL (stats por default)
    // =========================
    pad_top->cd();

    // Stats default ON en el total
    h2_cal->SetStats(1);
    gStyle->SetOptStat(1110); // entries, mean, stddev (rms)

    // Evitar “corte” en Y arriba
    h2_cal->SetMaximum(1.10 * h2_cal->GetMaximum());

    h2_cal->Draw("HIST");
    pad_top->Update(); // para que aparezca la stats box

    // (Opcional) mover la stats box default del total si estorba
    // auto st = (TPaveStats*)h2_cal->FindObject("stats");
    // if (st) { st->SetX1NDC(0.75); st->SetX2NDC(0.95); st->SetY1NDC(0.70); st->SetY2NDC(0.90); }

    // =========================
    // Abajo: CLUSTERS combinados (con tus cajas manuales)
    // =========================
    pad_bot->cd();

    // Desactivar stats automáticas para clusters (usamos cajas manuales)
    h0_cal->SetStats(0);
    h1_cal->SetStats(0);

    // Evitar “corte” en Y abajo
    Double_t maxY = std::max(h0_cal->GetMaximum(), h1_cal->GetMaximum());
    h0_cal->SetMaximum(1.10 * maxY);

    // Dibujar superpuestos
    h0_cal->Draw("HIST");
    h1_cal->Draw("HIST SAME");

    // Leyenda abajo
    TLegend *leg = new TLegend(0.65, 0.75, 0.88, 0.88);
    leg->AddEntry(h0_cal, "Cluster 0", "l");
    leg->AddEntry(h1_cal, "Cluster 1", "l");
    leg->Draw();

    // Cajas manuales con stats (clusters)
    auto makeBox = [](Double_t x1, Double_t y1, Double_t x2, Double_t y2,
                      Color_t color, const char* title, TH1* h) {
        TPaveText *pt = new TPaveText(x1, y1, x2, y2, "NDC");
        pt->SetFillStyle(0);
        pt->SetBorderSize(1);
        pt->SetTextAlign(12);
        pt->SetTextColor(color);

        pt->AddText(title);
        pt->AddText(Form("Entries = %.0f", h->GetEntries()));
        pt->AddText(Form("Mean    = %.6g", h->GetMean()));
        pt->AddText(Form("StdDev  = %.6g", h->GetStdDev()));
        return pt;
    };

    // NOTA: estas coordenadas NDC son del PAD inferior
    TPaveText *box0 = makeBox(0.12, 0.78, 0.34, 0.92, kBlue, "Cluster 0", h0_cal);
    TPaveText *box1 = makeBox(0.12, 0.60, 0.34, 0.74, kRed,  "Cluster 1", h1_cal);
    box0->Draw();
    box1->Draw();

    pad_bot->Modified();
    pad_bot->Update();

    // -------------------------
    // Guardar
    // -------------------------
    output->cd();
    h0_cal->Write();
    h1_cal->Write();
    h2_cal->Write();
    c1->Write();

    //output->Close();
    file0->Close();
    file1->Close();
    file2->Close();
}
