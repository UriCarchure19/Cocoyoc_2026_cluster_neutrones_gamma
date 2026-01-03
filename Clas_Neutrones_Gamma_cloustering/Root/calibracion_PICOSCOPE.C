// Incluir librerías de ROOT
#include <TFile.h>
#include <TH1.h>
#include <TH1F.h>
#include <TTree.h>
#include <TSpectrum.h>
#include <TCanvas.h>  // Necesario para la visualización gráfica
#include <iostream>   // Para mensajes en consola



void calibracion_PICOSCOPE() {
    //Parametros De Ajuste y=Ax+B del Espectro Calibrado////////
    Double_t gainP1 = 1.1551 , offsetP1 = 10982.1;/////parametros de ajuste Na 22
    Double_t gainP2 = 1.1551 , offsetP2 = 10982.1;    ///Co 60
    
////////////////////carga los archivos ROOT////////////
    TFile *file = TFile::Open("/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/out_stilbene_na22-uri.root");
    TTree *tree = (TTree*)file->Get("wf");
///////////tree 2 es para el mbs//////////////

    TFile *file2 = TFile::Open("/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/clouster0_stilbene_pu_13C.root"); 
    TTree *tree2 = (TTree*)file2->Get("wf");
/////////Creacion de los nuevos Histogramas calibrados/////////////
    TFile *output = new TFile("calib_stilbene_PICOSCOPE.root","recreate"); //(archivo root de salida)
    // Crear canvas
    TCanvas *c1 = new TCanvas("c1", "Spectrum_stilbebe");
    c1->Divide(1,2);

////////////////////////////////////////////////////////////    
    TF1 *fit1 = new TF1("fit1", "[0] / (1 + exp(-[1]*(x-[2])))+[3]");
///////////////////////////////////Generando archivo txt/////////////////
    // Archivo para guardar resultados
    std::ofstream outfile("Fit_parametros_Channel_stilbene.txt");
    outfile << "Resultados de ajuste stilbene \n";
    outfile << "--------------------------------\n";
////////////////////////////////////////////////////////Na22 chanel 0/////////////////
    // Crear y dibujar primer histograma (Canal 0)
    c1->cd(1); // Activa el primer canvas
    tree->Draw("integral>>h0(600,-9550,-8350)");
//////////////////fotopico1//////////////////////  
    TH1F *h0 = (TH1F*)gDirectory->Get("h0");
    h0->SetLineColor(kBlue);
    h0->SetTitle("Spectrum BC404 22Na source; Channel;Cuentas");
    fit1->SetParameters(183.494,-0.1,-9270,29.6);
    h0->Fit("fit1", "R","",-9280,-9120);
    outfile << "\nCanal   340.61 22Na \n";
    outfile << "Amplitud: " << fit1->GetParameter(0) << " ± " << fit1->GetParError(0) << "\n";
    outfile << "Media: " << fit1->GetParameter(1) << " ± " << fit1->GetParError(1) << "\n";
    outfile << "Sigma: " << fit1->GetParameter(2) << " ± " << fit1->GetParError(2) << "\n";
    outfile << "Slope: " << fit1->GetParameter(3) << " ± " << fit1->GetParError(3) << "\n";
    outfile << "Intercept: " << fit1->GetParameter(4) << " ± " << fit1->GetParError(4) << "\n";
/////Segundo Pico///////////////////////////////////////////
    fit1->SetParameters(180,-0.1,-8630,29.6);
    h0->Fit("fit1", "R+","",-8718,-8500);
    h0->SetLineColor(kBlack);
    // Ajuste segundo pico canal 0
    outfile << "\nCanal  1061.71 22Na \n";
    outfile << "Amplitud: " << fit1->GetParameter(0) << " ± " << fit1->GetParError(0) << "\n";
    outfile << "Media: " << fit1->GetParameter(1) << " ± " << fit1->GetParError(1) << "\n";
    outfile << "Sigma: " << fit1->GetParameter(2) << " ± " << fit1->GetParError(2) << "\n";
    outfile << "Slope: " << fit1->GetParameter(3) << " ± " << fit1->GetParError(3) << "\n";
    outfile << "Intercept: " << fit1->GetParameter(4) << " ± " << fit1->GetParError(4) << "\n";
    h0->Draw();
   




/////////////////////////60Co Channel 0/////////////////////


    //Ecuacion de ajuste canal 0:
    c1->cd(2); // Activa el primer canvas
    tree2->Draw("integral>>h00(600,-9550,-5000)");
//////////////////fotopico1//////////////////////  
    TH1F *h00 = (TH1F*)gDirectory->Get("h00");
    h00->SetLineColor(kBlue);
    h00->SetTitle("Spectrum stilbene 22Na source MBS system;canales;Cuentas");
    fit1->SetParameters(8714,-0.133,241.6,1078.6);
    h00->Fit("fit1", "R","",200, 285);
    // Ajuste primer pico canal 0
    outfile << "\nCanal  238Pu+13C \n";
    outfile << "Amplitud: " << fit1->GetParameter(0) << " ± " << fit1->GetParError(0) << "\n";
    outfile << "Media: " << fit1->GetParameter(1) << " ± " << fit1->GetParError(1) << "\n";
    outfile << "Sigma: " << fit1->GetParameter(2) << " ± " << fit1->GetParError(2) << "\n";
    outfile << "Slope: " << fit1->GetParameter(3) << " ± " << fit1->GetParError(3) << "\n";
    outfile << "Intercept: " << fit1->GetParameter(4) << " ± " << fit1->GetParError(4) << "\n";
    // Ajuste segundo pico canal 0
    fit1->SetParameters(1372,-56,502,185);
    h00->Fit("fit1", "R+","",440, 545);
    outfile << "\nCanal 238Pu+13C-\n";
    outfile << "Amplitud: " << fit1->GetParameter(0) << " ± " << fit1->GetParError(0) << "\n";
    outfile << "Media: " << fit1->GetParameter(1) << " ± " << fit1->GetParError(1) << "\n";
    outfile << "Sigma: " << fit1->GetParameter(2) << " ± " << fit1->GetParError(2) << "\n";
    outfile << "Slope: " << fit1->GetParameter(3) << " ± " << fit1->GetParError(3) << "\n";
    outfile << "Intercept: " << fit1->GetParameter(4) << " ± " << fit1->GetParError(4) << "\n";
    h00->Draw();


///////////Nuevos Histogramas para calibraciones////////////////////
   /////22Na
    TH1F *CalibDetectors = new TH1F("Calib22Na", 
    "stilbene detector, 22Na Spectrum;Energy [keV];Counts",
    h0->GetNbinsX(),
    h0->GetXaxis()->GetXmin() * gainP1 + offsetP1,
    h0->GetXaxis()->GetXmax() * gainP1 + offsetP1);


    TH1F *CalibDetectors3 = new TH1F("Calibration_238Pu+13C", 
        "stilbene detector,238Pu+13C Spectrum",
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
        


////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////  Canvas     pulgadas
    ///CalibDetectors   h0  channel 0 22Na   c5  3cm   1.1811
    ///CalibDetectors2  h1  channel 1 22Na   c6  6cm   2.3622
    ///CalibDetectors3  h00 channel 0 60Co   c7  3cm   1.1811
    ///CalibDetectors4  h11 channel 1 60Co   c8  6cm   2.3622
   

///////////////////////////////////////////////////////////////////////////////////

    // Crear canvas dividido en 2x2
    TCanvas *c_combined = new TCanvas("c_combined", "Combined Spectra");
    c_combined->Divide(1, 2);  // 2 columnas, 2 filas

    // Primer espectro (arriba-izquierda)
    c_combined->cd(1);
    CalibDetectors->SetLineColor(kBlue);
    CalibDetectors->SetStats(0);
    CalibDetectors->SetTitle("stilbene detector with 22Na(Picoscope); Energy [keV];Counts");
    CalibDetectors->Draw();
    CalibDetectors->Write();

            // Línea vertical en 340.67
    TLine *line1 = new TLine(340.67, 0, 340.67, 400); // de Y=0 a un poco más del máximo
    line1->SetLineColor(kRed);
    line1->SetLineStyle(1); // línea punteada
    line1->SetLineWidth(2);
    line1->Draw("same");


    // Línea vertical en 1061.71
    TLine *line2 = new TLine(1061.71, 0, 1061.71, 150);
    line2->SetLineColor(kBlack);
    line2->SetLineStyle(1);
    line2->SetLineWidth(2);
    line2->Draw("same");

         // Leyenda para la primera gráfica
    TLegend *leg1 = new TLegend(0.15,0.75,0.45,0.85);
    leg1->AddEntry(line1,"340.67 keV","l");
    leg1->AddEntry(line2,"1061.71 keV","l");
    leg1->Draw();
    

    // Segundo espectro (arriba-derecha)
    c_combined->cd(2);
    CalibDetectors3->SetLineColor(kGreen+2);
    CalibDetectors3->SetStats(0);
    CalibDetectors3->SetTitle("stilbene detector with  22Na (MBS); Energy [keV];Counts");
    CalibDetectors3->Draw();
    CalibDetectors3->Write();

            // Línea vertical en 340.67
    TLine *line3 = new TLine(340.67, 0, 340.67, 400); // de Y=0 a un poco más del máximo
    line3->SetLineColor(kRed);
    line3->SetLineStyle(1); // línea punteada
    line3->SetLineWidth(2);
    line3->Draw("same");


    // Línea vertical en 1061.71
    TLine *line4 = new TLine(1061.71, 0, 1061.71, 150);
    line4->SetLineColor(kBlack);
    line4->SetLineStyle(1);
    line4->SetLineWidth(2);
    line4->Draw("same");

         // Leyenda para la primera gráfica
    TLegend *leg3 = new TLegend(0.15,0.75,0.45,0.85);
    leg3->AddEntry(line1,"340.67 keV","l");
    leg3->AddEntry(line2,"1061.71 keV","l");
    leg3->Draw();
    


/////////////////////////////////////////////////////////////////////////////////
// Crear nuevo canvas para graficar ambos histogramas juntos
/*
TCanvas *c_together = new TCanvas("c_together", "Spectra Comparison");
c_together->cd();

// Ajustar colores y estilos
CalibDetectors->SetLineColor(kBlue);
CalibDetectors->SetLineWidth(2);
CalibDetectors->SetTitle("BC404 detector with 22Na; Energy [keV]; Counts");

// Dibujar primer histograma
CalibDetectors->Draw("HIST");

// Dibujar segundo histograma sobre el primero
CalibDetectors3->SetLineColor(kGreen+2);
CalibDetectors3->SetLineWidth(2);
CalibDetectors3->Draw("HIST SAME");

// Líneas verticales para ambos picos
TLine *line_a = new TLine(340.67, 0, 340.67, 240);
line_a->SetLineColor(kRed);
line_a->SetLineWidth(2);
line_a->Draw("SAME");

TLine *line_b = new TLine(1061.71, 0, 1061.71, 80);
line_b->SetLineColor(kBlack);
line_b->SetLineWidth(2);
line_b->Draw("SAME");

// Leyenda
TLegend *legend = new TLegend(0.15,0.75,0.45,0.88);
legend->AddEntry(CalibDetectors, "Picoscope System", "l");
legend->AddEntry(CalibDetectors3, "MBS System", "l");
legend->AddEntry(line_a, "340.67 keV", "l");
legend->AddEntry(line_b, "1061.71 keV", "l");
legend->Draw();

// Guardar si es necesario
c_together->Write();
*/

    //output->Close();
    //delete output;


    outfile.close();
}