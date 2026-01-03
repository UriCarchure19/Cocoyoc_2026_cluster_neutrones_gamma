#include <TFile.h>
#include <TH1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <iostream>
#include <math.h>
#include <TLatex.h>

void Ajuste_lineal() {
    // Datos para el gráfico
    const int n = 2;
    double Channel0[n] = {-9212.38,-8588.17};
    
    double Energy_22Na[n]   = {0.34067,1.06171};
   

    double errX0[n] = {0,0};
    double errY0[n] = {0,0};  

    
    // Crear gráficas con barras de error
    TGraphErrors *g  = new TGraphErrors(n, Channel0, Energy_22Na, errX0, errY0);
   


        // Ajustes independientes
    TF1 *fit1 = new TF1("fit1", "[0]+[1]*x");
    

    TCanvas *c1 = new TCanvas("c1", "Ajuste lineal", 800, 600);
    
    c1->cd(1);
    
    g->SetTitle("Calibration of the 22Na with stilbene (PICOSCOPE);Channel;Energy [MeV]");
            // --- Configuración de marcadores ---
    g->SetMarkerColor(kRed);
    g->SetMarkerStyle(20);     // Círculo sólido
    g->SetMarkerSize(1.5);
    fit1->SetLineColor(kRed);
    fit1->SetLineStyle(8);

    fit1->SetParameters(22158.8672,2.305783);
    g->Draw("AP");
    g->Fit("fit1", "R", "",-9240,-8500);

     // Leyenda para la primera gráfica
    TLegend *leg1 = new TLegend(0.15,0.75,0.45,0.85);
    leg1->AddEntry(g,"Na-22","p");
    //leg1->AddEntry(fit1,"Fit","l");
    leg1->Draw();
    


    // Resultados del ajuste 1
    std::cout << "\n--- Ajuste  (ROJO) Espectro Na22 ---\n";
    for (int i = 0; i < fit1->GetNpar(); i++) {
        std::cout << "p" << i 
                  << " = " << fit1->GetParameter(i) 
                  << " ± " << fit1->GetParError(i) << "\n";
    }
    std::cout << "Chi^2/NDF = " << fit1->GetChisquare() 
              << "/" << fit1->GetNDF() << "\n";
    std::cout << "p-value = " << fit1->GetProb() << "\n";


}
