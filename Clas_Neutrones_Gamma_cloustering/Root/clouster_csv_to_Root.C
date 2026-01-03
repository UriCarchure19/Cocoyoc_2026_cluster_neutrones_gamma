#include <TVector2.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <iostream>
#include <map>
#include <algorithm>

// Parámetros del código
#define LINES_TO_SKIP 3
#define DELIM ','  // Usamos ',' para separar valores 
#define INLIST "/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/notebooks/cluster1.txt"
#define OUTROOT "/home/uri/Desktop/Dubna/proyectos_congreso/Clas_Neutrones_Gamma_cloustering/Root/clouster1_stilbene_pu_13C.root"

using std::fstream;
using std::string;
using std::getline;
using std::istringstream;
using std::vector;
using std::pair;
using std::make_pair;
using std::cout;
using std::endl;
using std::stof;
using std::map;

// Función para calcular la integral (método trapezoidal)
double calcularIntegral(const vector<float>& tiempos, const vector<float>& voltajes) {
    if (tiempos.size() < 2 || voltajes.size() < 2 || tiempos.size() != voltajes.size()) 
        return 0.0;
    
    double integral = 0.0;
    for (size_t i = 1; i < tiempos.size(); ++i) {
        double dt = tiempos[i] - tiempos[i-1];
        double v_prom = -(voltajes[i] + voltajes[i-1]) / 2.0;
        integral += v_prom * dt;
    }
    return integral;
}

int clouster_csv_to_Root() {
    // 1. Leer lista de archivos
    fstream inList(INLIST);
    string line;
    vector<string> vWfFileList;
    int counter = 0;

    while (getline(inList, line))
    {
        #ifdef FILES_TO_READ 
            if (counter == FILES_TO_READ)
            {
                break;
            }
        #endif /*FILES_TO_READ*/

        vWfFileList.push_back(line);
        counter++;
    }
    inList.close();


    size_t eventNr = 0;
    double integral = 0.;
    vector<float> vTraceX;
    vector<float> vTraceY;


    TFile* outputFile = new TFile(OUTROOT, "recreate");
    TTree* tree = new TTree("wf", "wf");
    tree->Branch("event", &eventNr, "event/I");
    tree->Branch("traceX", &vTraceX);
    tree->Branch("traceY", &vTraceY);
    tree->Branch("integral", &integral, "integral/D");

    #ifdef DEBUG
        TCanvas* cnCommon = new TCanvas("cnCommon", "cnCommon", 700, 700);
    #endif

     std:: cout  <<"valores procesadosssssss: " ;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 3. Procesar cada archivo
    for (size_t i = 0; i < vWfFileList.size(); i++) {
    
        fstream archivo(vWfFileList[i]);
        //std:: cout <<"archivo procesado: "<< vWfFileList[i] << std:: endl ;
        if (!archivo.is_open()) {
            cerr << "Error abriendo: " << vWfFileList[i] << endl;
            continue;
        }

        // Reiniciar variables para cada evento
        vTraceX.clear();
        vTraceY.clear();
        int skip = 0;
        integral = 0.0;
        eventNr = i;
        
        // 4. Leer encabezados (saltar líneas iniciales)
        for (int j = 0; j < LINES_TO_SKIP; j++) {
            if (!getline(archivo, line)) break;
        }

        // 5. Procesar datos
        map<string, size_t> indiceColumnas;
        size_t idx_tiempo = 0, idx_voltaje = 1; // Valores por defecto
        
        if (getline(archivo, line)) {
            // Parsear encabezados
            istringstream ss(line);
            string columna;
            vector<string> columnas;
            
            while (getline(ss, columna, DELIM)) {
                columna.erase(remove(columna.begin(), columna.end(), ' '), columna.end());
                columnas.push_back(columna);
                indiceColumnas[columna] = columnas.size() - 1;
            }
            
            // Identificar índices de columnas
            if (indiceColumnas.find("Time") != indiceColumnas.end() && 
                indiceColumnas.find("ChannelA") != indiceColumnas.end()) {
                idx_tiempo = indiceColumnas["Time"];
                idx_voltaje = indiceColumnas["ChannelA"];
            }
        }

        // 6. Leer datos
        while (getline(archivo, line)) {
            istringstream ss(line);
            string valor;
            vector<string> fila_actual;
            
            while (getline(ss, valor, DELIM)) {
                fila_actual.push_back(valor);
            }
            
            if (fila_actual.size() > max(idx_tiempo, idx_voltaje)) {
                try {
                    float tiempo = stof(fila_actual[idx_tiempo]);
                    float voltaje = stof(fila_actual[idx_voltaje]);
                
                    //printf("%f %f\n", tiempo, voltaje);
                    vTraceX.push_back(tiempo);
                    vTraceY.push_back(voltaje);   
                                  
                } 
                catch (...) {
                    
                }
            }            
        }
        archivo.close();
        integral = calcularIntegral(vTraceX, vTraceY);
        //printf("%f\n", integral);

        // 8. Llenar árbol
        tree->Fill();
        
    }

    // 9. Guardar y cerrar
    outputFile->Write();
    outputFile->Close();

    #ifdef DEBUG
        delete cnCommon;
    #endif

    return 0;
}
