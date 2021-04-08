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


bool esValida1(const vector<int> &solu) // implementacion de atras para adelante
{
    int PesoAcumulado = 0;
    for (size_t i = n - 1; i >= 0; i--) {
        if (PesoAcumulado > R) return false;
        if (solu[i] == 1 && Resistencias[i] < PesoAcumulado) return false;
        PesoAcumulado = PesoAcumulado + solu[i] * Pesos[i];
    }
    return true;
}

bool esValida(const vector<int> &solu) // podriamos no pasar parametros y usar solo que SolucionParcial es global
{
    int resistenciaActual = R;
    for (size_t i = 0; i < n; i++) {
        if (resistenciaActual < 0) return false;
        if (solu[i] == 1) resistenciaActual = min(resistenciaActual - Pesos[i], Resistencias[i]);
    }
    return resistenciaActual >= 0 ;
}

// i: posicion del elemento a considerar en este nodo.
// k: cantidad de elementos seleccionados hasta este nodo.
vector<int> SolucionParcial; //S[j] va a ser igual a 1 si solo si j<=i
// y el elemento S[j] forma parte de nuestra solucion parcial.
int FB(int i, int k) {
    // Caso base.
    if (i == n) return esValida(SolucionParcial) ? k : MININFTY;

    // Recursión.
    SolucionParcial[i] = 1;
    int agrego = FB(i + 1, k + 1);

    SolucionParcial[i] = 0;
    int no_agrego = FB(i + 1, k);
    
    return max(agrego, no_agrego);
}

// i: posicion del elemento a considerar en este nodo.
// k: cantidad de elementos seleccionados hasta este nodo.
// r: maxima peso que no aplasta el tubo ni ningun producto.
bool poda_factibilidad = true; // define si la poda por factibilidad esta habilitada.
bool poda_optimalidad = true; // define si la poda por optimalidad esta habilitada.
int K = MININFTY; // Mejor solucion hasta el momento.
int BT0(int i, int r, int k) {
    // Caso base.
    if (i == n) {
        if (r >= 0) K = max(K, k);
        if (i == n) return r >= 0 ? k : MININFTY;
    }

    // Poda por factibilidad.
    if (poda_factibilidad && r == 0) {
        K = max(K, k);
        return k;
    }
    if (poda_factibilidad && r < 0) return MININFTY;

    // Poda por optimalidad.
    if (poda_optimalidad && k + (n - i) <= K) return MININFTY;

    // Recursión.
    int no_agrego = BT0(i + 1, r, k);
    int agrego = BT0(i + 1, min(r - Pesos[i], Resistencias[i]), k + 1);

    return max(no_agrego, agrego);
}

int BT(int i, int r, int k) //version con podas primero
{
    // Poda por factibilidad.
    if (poda_factibilidad && r < 0) return MININFTY;
    // Poda por optimalidad.
    if (poda_optimalidad && k + (n - i) <= K) return MININFTY;

    // Caso base y poda factibilidad r==0 suponiendo que no haya productos
    //con peso = 0.
    if (i == n or r == 0) {
        K = max(K, k);
        return k;
    }

    // Recursión.
    int no_agrego = BT(i + 1, r, k);
    int agrego = BT(i + 1, min(r - Pesos[i], Resistencias[i]), k + 1);

    return max(no_agrego, agrego);
}

int BT2(int i, int r, int k) {
    // Caso base.
    if (i == -1) {
        if (r >= 0) K = max(K, k);
        if (i == n) return r >= 0 ? k : MININFTY;
    }

    // Poda por factibilidad.
    if (poda_factibilidad && r < 0) return MININFTY;

    // Poda por optimalidad.
    if (poda_optimalidad && k + (n - i) <= K) return MININFTY;

    // Recursión.
    int no_agrego = BT2(i - 1, r, k);
    int PesoAcumulado = R - r;
    if (Resistencias[i] >= PesoAcumulado)   // otra poda factibilidad
    {
        int agrego = BT2(i - 1, r - Pesos[i], k + 1);
        return max(no_agrego, agrego);
    }
    return no_agrego;
}


vector <vector<int>> M; // Memoria de PD.
const int UNDEFINED = -1;

// PD(i, r):
int PD(int i, int r) {
    if (r < 0) return MININFTY;
    if (i == n) return 0;
    if (M[i][r] == UNDEFINED) M[i][r] = max(PD(i + 1, r), 1 + PD(i + 1, min(r - Pesos[i], Resistencias[i])));
    return M[i][r];
}

int main(int argc, char **argv) {
    // Leemos el parametro que indica el algoritmo a ejecutar.
    map <string, string> algoritmos_implementados = {
            {"FB", "Fuerza Bruta"},
            {"BT", "Backtracking con podas"},
            {"BT-F", "Backtracking con poda por factibilidad"},
            {"BT-O", "Backtracking con poda por optimalidad"},
            {"DP", "Programacion dinámica"}
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
    for (int i = 0; i < n; ++i) cin >> Pesos[i];
    for (int i = 0; i < n; ++i) cin >> Resistencias[i];

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
    int optimum;
    optimum = MININFTY;
    auto start = chrono::steady_clock::now();
    if (algoritmo == "FB") {
        //Prueba para ver que funcione la lectura de parametros
        // cout << "n: " << n << " || " << "R: " << R << endl;
        // for (size_t i = 0; i < n; i++) {
        //   cout << Pesos[i] << " || " << Resistencias[i] << endl;
        // }
        SolucionParcial.assign(n, 0);
        optimum = FB(0, 0);
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
    } else if (algoritmo == "DP") {
        // Precomputamos la solucion para los estados.
        M = vector<vector<int>>(n+1, vector<int>(R+1, UNDEFINED));
        // for (int i = 0; i < n+1; ++i)
        // 	for (int j = 0; j < R+1; ++j)
        // 		PD(i, j);

        // Obtenemos la solucion optima.
        optimum = PD(0, R);
    }
    auto end = chrono::steady_clock::now();
    double total_time = chrono::duration<double, milli>(end - start).count();

    // Imprimimos el tiempo de ejecución por stderr.
    clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << "Optimo:"<<(optimum == MININFTY ? -1 : optimum) << endl;
    return 0;
}
