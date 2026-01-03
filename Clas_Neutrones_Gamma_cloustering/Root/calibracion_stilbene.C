// Incluir librerías de ROOT
#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TTree.h>
#include <TSpectrum.h>
#include <TCanvas.h>
#include <iostream>

void calibracion_stilbene() {
    Double_t gainP1 = 1.1551 , offsetP1 = 10982.1;/////parametros de ajuste Na 22
    Double_t gainP2 = 1.1551 , offsetP2 = 10982.1;

    TFile *file = TFile::Open("/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/clouster1_stilbene_pu_13C.root");
    TTree *tree = (TTree*)file->Get("wf");

    TFile *file2 = TFile::Open("/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/clouster0_stilbene_pu_13C.root"); 
    TTree *tree2 = (TTree*)file2->Get("wf");

    TFile *output = new TFile("calib_stilbene_PICOSCOPE.root","recreate");
    TCanvas *c1 = new TCanvas("c1", "Spectrum_stilbene");
    c1->Divide(1,2);

    TF1 *fit1 = new TF1("fit1", "[0] / (1 + exp(-[1]*(x-[2])))+[3]");
    std::ofstream outfile("Fit_parametros_Channel.txt");
    outfile << "Resultados de ajuste Stilbene \n";
    outfile << "--------------------------------\n";

    c1->cd(1);
    tree->Draw("integral>>h0(512,-9550,-5000)");
    TH1F *h0 = (TH1F*)gDirectory->Get("h0");
    h0->SetLineColor(kBlue);
    h0->SetTitle("Spectrum Stilbene 22Na source; Channel; Cuentas");
    fit1->SetParameters(183.494,-0.1,-9384,29.6);
    h0->Fit("fit1", "R","",-9425,-9340);
    outfile << "\nCanal   511 22Na \n";
    outfile << "Amplitud: " << fit1->GetParameter(0) << " ± " << fit1->GetParError(0) << "\n";
    outfile << "Media: " << fit1->GetParameter(1) << " ± " << fit1->GetParError(1) << "\n";
    outfile << "Sigma: " << fit1->GetParameter(2) << " ± " << fit1->GetParError(2) << "\n";
    outfile << "Slope: " << fit1->GetParameter(3) << " ± " << fit1->GetParError(3) << "\n";
    h0->Fit("fit1", "R+","",-9240, -9090);
    h0->SetLineColor(kBlack);
    outfile << "\nCanal  1274.537 22Na \n";
    outfile << "Amplitud: " << fit1->GetParameter(0) << " ± " << fit1->GetParError(0) << "\n";
    outfile << "Media: " << fit1->GetParameter(1) << " ± " << fit1->GetParError(1) << "\n";
    outfile << "Sigma: " << fit1->GetParameter(2) << " ± " << fit1->GetParError(2) << "\n";
    outfile << "Slope: " << fit1->GetParameter(3) << " ± " << fit1->GetParError(3) << "\n";
    h0->Draw();

    c1->cd(2);
    tree2->Draw("integral>>h00(600,-9550,-5000)");
    TH1F *h00 = (TH1F*)gDirectory->Get("h00");
    h00->SetLineColor(kBlue);
    h00->SetTitle("Spectrum Stilbene 22Na source MBS system;canales;Cuentas");
    fit1->SetParameters(8714,-0.133,241.6,1078.6);
    h00->Fit("fit1", "R","",200, 285);
    outfile << "\nCanal  238Pu+13C \n";
    outfile << "Amplitud: " << fit1->GetParameter(0) << " ± " << fit1->GetParError(0) << "\n";
    outfile << "Media: " << fit1->GetParameter(1) << " ± " << fit1->GetParError(1) << "\n";
    outfile << "Sigma: " << fit1->GetParameter(2) << " ± " << fit1->GetParError(2) << "\n";
    outfile << "Slope: " << fit1->GetParameter(3) << " ± " << fit1->GetParError(3) << "\n";
    fit1->SetParameters(1372,-56,502,185);
    h00->Fit("fit1", "R+","",440, 545);
    outfile << "\nCanal 238Pu+13C-\n";
    outfile << "Amplitud: " << fit1->GetParameter(0) << " ± " << fit1->GetParError(0) << "\n";
    outfile << "Media: " << fit1->GetParameter(1) << " ± " << fit1->GetParError(1) << "\n";
    outfile << "Sigma: " << fit1->GetParameter(2) << " ± " << fit1->GetParError(2) << "\n";
    outfile << "Slope: " << fit1->GetParameter(3) << " ± " << fit1->GetParError(3) << "\n";
    h00->Draw();

    TH1F *CalibDetectors = new TH1F("Calib22Na", 
    "Stilbene detector, 22Na Spectrum;Energy [keV];Counts",
    h0->GetNbinsX(),
    h0->GetXaxis()->GetXmin() * gainP1 + offsetP1,
    h0->GetXaxis()->GetXmax() * gainP1 + offsetP1);

    TH1F *CalibDetectors3 = new TH1F("Calibration_238Pu+13C", 
        "Stilbene detector,238Pu+13C Spectrum",
    h00->GetNbinsX(),
    h00->GetXaxis()->GetXmin() * gainP2 + offsetP2,
    h00->GetXaxis()->GetXmax() * gainP2 + offsetP2);

    for (Int_t j = 1; j <= h0->GetNbinsX(); j++) {
        double content = h0->GetBinContent(j);
        CalibDetectors->SetBinContent(j, content);
    }
    
    for (Int_t j = 1; j <= h00->GetNbinsX(); j++) {
        double content = h00->GetBinContent(j);
        CalibDetectors3->SetBinContent(j, content);
    }

    TCanvas *c_combined = new TCanvas("c_combined", "Combined Spectra");
    c_combined->Divide(1, 2);

    c_combined->cd(1);
    CalibDetectors->SetLineColor(kBlue);
    CalibDetectors->SetStats(0);
    CalibDetectors->SetTitle("Stilbene detector with 22Na (Picoscope); Energy [keV];Counts");
    CalibDetectors->Draw();
    CalibDetectors->Write();

    TLine *line1 = new TLine(340.67, 0, 340.67, 400);
    line1->SetLineColor(kRed);
    line1->SetLineStyle(1);
    line1->SetLineWidth(2);
    line1->Draw("same");

    TLine *line2 = new TLine(1061.71, 0, 1061.71, 150);
    line2->SetLineColor(kBlack);
    line2->SetLineStyle(1);
    line2->SetLineWidth(2);
    line2->Draw("same");

    TLegend *leg1 = new TLegend(0.15,0.75,0.45,0.85);
    leg1->AddEntry(line1,"340.67 keV","l");
    leg1->AddEntry(line2,"1061.71 keV","l");
    leg1->Draw();

    c_combined->cd(2);
    CalibDetectors3->SetLineColor(kGreen+2);
    CalibDetectors3->SetStats(0);
    CalibDetectors3->SetTitle("Stilbene detector with 22Na (MBS); Energy [keV];Counts");
    CalibDetectors3->Draw();
    CalibDetectors3->Write();

    TLine *line3 = new TLine(340.67, 0, 340.67, 400);
    line3->SetLineColor(kRed);
    line3->SetLineStyle(1);
    line3->SetLineWidth(2);
    line3->Draw("same");

    TLine *line4 = new TLine(1061.71, 0, 1061.71, 150);
    line4->SetLineColor(kBlack);
    line4->SetLineStyle(1);
    line4->SetLineWidth(2);
    line4->Draw("same");

    TLegend *leg3 = new TLegend(0.15,0.75,0.45,0.85);
    leg3->AddEntry(line1,"340.67 keV","l");
    leg3->AddEntry(line2,"1061.71 keV","l");
    leg3->Draw();

    outfile.close();
}