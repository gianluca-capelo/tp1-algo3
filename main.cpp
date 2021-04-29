#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <set>
#include <map>

using namespace std;

int MININFTY = -10e6; // Valor para indicar que no hubo solución.

// Información de la instancia a resolver.
int n, R;
vector<int> Pesos;
vector<int> Resistencias;

// i: posicion del elemento a considerar en este nodo.
// r: maximo peso que se puede agregar sin aplastar ni otros productos ni el tubo.
// k: cantidad de elementos seleccionados hasta este nodo.
int FB(int i, int r, int k) {
    // Caso base.
    if (i == n) return r >= 0 ? k : MININFTY;

    // Recursión.
    int agrego = FB(i + 1, min(r - Pesos[i], Resistencias[i]), k + 1);
    int no_agrego = FB(i + 1, r, k);

    return max(agrego, no_agrego);
}

// i: posicion del elemento a considerar en este nodo.
// r: maximo peso que se puede agregar sin aplastar otros productos ni el tubo.
// k: cantidad de elementos seleccionados hasta este nodo.
bool poda_factibilidad = true; // define si la poda por factibilidad esta habilitada.
bool poda_optimalidad = true; // define si la poda por optimalidad esta habilitada.
int K = MININFTY; // Mejor solucion hasta el momento.
int BT(int i, int r, int k){
    // Poda por factibilidad.
    if (poda_factibilidad && r <= 0) {
        if (r == 0) K = max(K, k);
        return r == 0 ? k : MININFTY;
    }

    // Poda por optimalidad.
    if (poda_optimalidad && k + (n - i) <= K) return MININFTY;

    // Caso base
    if (i == n) {
        if (r >= 0) K = max(K, k);
        return r >= 0 ? k : MININFTY;
    }

    // Recursión.
    int agrego = BT(i + 1, min(r - Pesos[i], Resistencias[i]), k + 1);
    int no_agrego = BT(i + 1, r, k);

    return max(agrego, no_agrego);
}

vector <vector<int>> M; // Memoria de PD.
const int UNDEFINED = -1;
// PD(i, r): maximo numero de elementos pertenecientes al conjunto
// de Resistencias y Pesos de {i, ... ,n} que puedo agregar en
// un jambotubo de resistencia r.
int PD(int i, int r) {
    if (r < 0) return MININFTY;
    if (i == n) return 0;
    if (M[i][r] == UNDEFINED) {
      int agrego = PD(i + 1, min(r - Pesos[i], Resistencias[i]));
      int no_agrego = PD(i + 1, r);
      M[i][r] = max( 1 + agrego , no_agrego);
    }
    return M[i][r];
}

int main(int argc, char **argv) {

    // Leemos el parametro que indica el algoritmo a ejecutar.
    map <string, string> algoritmos_implementados = {
            {"FB",   "Fuerza Bruta"},
            {"BT",   "Backtracking con podas"},
            {"BT-F", "Backtracking con poda por factibilidad"},
            {"BT-O", "Backtracking con poda por optimalidad"},
            {"PD",   "Programacion dinámica"},
    };

    // Verificar que el algoritmo pedido exista.
    if (argc < 2 || algoritmos_implementados.find(argv[1]) == algoritmos_implementados.end()) {
        cerr << "Algoritmo no encontrado: " << argv[1] << endl;
        cerr << "Los algoritmos existentes son: " << endl;
        for (auto &alg_desc: algoritmos_implementados)
            cerr << "\t- " << alg_desc.first << ": " << alg_desc.second << endl;
        return 0;
    }
    string algoritmo = argv[1];

    // Leemos el input.
    cin >> n >> R;
    Pesos.assign(n, 0);
    Resistencias.assign(n, 0);
    for (int i = 0; i < n; ++i) cin >> Pesos[i] >> Resistencias[i];

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
    int optimum;
    optimum = MININFTY;
    auto start = chrono::steady_clock::now();
    if (algoritmo == "FB") {
        optimum = FB(0, R, 0);
    } else if (algoritmo == "BT") {
        K = MININFTY;
        poda_optimalidad = poda_factibilidad = true;
        optimum = BT(0, R, 0);
    } else if (algoritmo == "BT-F") {
        K = MININFTY;
        poda_optimalidad = false;
        poda_factibilidad = true;
        optimum = BT(0, R, 0);
    } else if (algoritmo == "BT-O") {
        K = MININFTY;
        poda_optimalidad = true;
        poda_factibilidad = false;
        optimum = BT(0, R, 0);
    } else if (algoritmo == "PD") {
        M = vector < vector < int >> (n + 1, vector<int>(R + 1, UNDEFINED));
        optimum = PD(0, R);
    }
    auto end = chrono::steady_clock::now();
    double total_time = chrono::duration<double, milli>(end - start).count();

    // Imprimimos el tiempo de ejecución por stderr.
    clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << (optimum == MININFTY ? -1 : optimum) << endl;
    return 0;
}
