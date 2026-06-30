#pragma once
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <functional>

using namespace std;

struct Punto {
    int id;
    string nombre;
    double x, y;
    bool accesible;
    bool esInterior;
};

struct Conexion {
    int origen, destino;
    double distanciaBase;
    bool bloqueada;
    bool esEscalera;
    bool esAscensor;
    bool tienePendiente;
};

class MapaCampus {
private:
    vector<Punto> nodos;
    vector<Conexion> aristas;
    map<int, vector<int>> adj;
    int tipoMovilidad; // 0 normal, 1 silla, 2 muletas, 3 pendiente

public:
    MapaCampus() : tipoMovilidad(0) {}

    bool cargarDesdeArchivo(string ruta) {
        nodos.clear();
        aristas.clear();
        adj.clear();

        ifstream f(ruta);
        if (!f.is_open()) return false;

        string linea;
        bool leyendoNodos = false;
        bool leyendoConexiones = false;

        while (getline(f, linea)) {
            // Quitar BOM UTF-8 si aparece al inicio
            if (!linea.empty() && (unsigned char)linea[0] == 0xEF) {
                if (linea.size() >= 3 &&
                    (unsigned char)linea[1] == 0xBB &&
                    (unsigned char)linea[2] == 0xBF) {
                    linea = linea.substr(3);
                }
            }

            // Quitar \r al final (archivos Windows)
            if (!linea.empty() && linea.back() == '\r') {
                linea.pop_back();
            }

            // Ignorar líneas vacías
            if (linea.empty()) continue;

            // Ignorar comentarios tipo //
            if (linea.size() >= 2 && linea[0] == '/' && linea[1] == '/') {
                continue;
            }

            if (linea == "Nodos:") {
                leyendoNodos = true;
                leyendoConexiones = false;
                continue;
            }

            if (linea == "Conexiones:") {
                leyendoNodos = false;
                leyendoConexiones = true;
                continue;
            }

            // Separar por |
            vector<string> partes;
            string token;
            stringstream ss(linea);

            while (getline(ss, token, '|')) {
                // quitar espacios a izquierda/derecha
                while (!token.empty() && (token.front() == ' ' || token.front() == '\t'))
                    token.erase(token.begin());
                while (!token.empty() && (token.back() == ' ' || token.back() == '\t'))
                    token.pop_back();

                partes.push_back(token);
            }

            try {
                if (leyendoNodos) {
                    // Espera 6 campos:
                    // id|nombre|x|y|accesible|interior
                    if (partes.size() != 6) {
                        continue;
                    }

                    Punto p;
                    p.id = stoi(partes[0]);
                    p.nombre = partes[1];
                    p.x = stod(partes[2]);
                    p.y = stod(partes[3]);
                    p.accesible = stoi(partes[4]) != 0;
                    p.esInterior = stoi(partes[5]) != 0;

                    nodos.push_back(p);
                }
                else if (leyendoConexiones) {
                    // Espera 6 campos:
                    // origen|destino|distancia|esEscalera|esAscensor|tienePendiente
                    if (partes.size() != 6) {
                        continue;
                    }

                    Conexion c;
                    c.origen = stoi(partes[0]);
                    c.destino = stoi(partes[1]);
                    c.distanciaBase = stod(partes[2]);
                    c.esEscalera = stoi(partes[3]) != 0;
                    c.esAscensor = stoi(partes[4]) != 0;
                    c.tienePendiente = stoi(partes[5]) != 0;
                    c.bloqueada = false;

                    aristas.push_back(c);
                    adj[c.origen].push_back(c.destino);
                    adj[c.destino].push_back(c.origen);
                }
            }
            catch (...) {
                // Si una línea viene mal, la ignora en vez de romper el programa
                continue;
            }
        }

        return !nodos.empty();
    }

    void setTipoMovilidad(int t) {
        tipoMovilidad = t;
    }

    vector<Punto> getNodos() const {
        return nodos;
    }

    vector<Conexion> getAristas() const {
        return aristas;
    }

    double obtenerCosto(int u, int v) const {
        for (const auto& c : aristas) {
            if ((c.origen == u && c.destino == v) ||
                (c.origen == v && c.destino == u)) {

                if (c.bloqueada) return 1e9;

                double costo = c.distanciaBase;

                // 0 = normal
                if (tipoMovilidad == 0) {
                    return costo;
                }
                // 1 = silla de ruedas
                else if (tipoMovilidad == 1) {
                    if (c.esEscalera) return 1e9;
                    if (c.esAscensor) costo *= 1.10;
                    if (c.tienePendiente) costo *= 1.30;
                    return costo;
                }
                // 2 = muletas
                else if (tipoMovilidad == 2) {
                    if (c.esEscalera) costo *= 1.80;
                    if (c.esAscensor) costo *= 1.05;
                    if (c.tienePendiente) costo *= 1.20;
                    return costo;
                }
                // 3 = dificultad con pendientes
                else if (tipoMovilidad == 3) {
                    if (c.tienePendiente) costo *= 1.50;
                    return costo;
                }

                return costo;
            }
        }
        return 1e9;
    }

    double obtenerDistancia(int u, int v) const {
        for (const auto& c : aristas) {
            if ((c.origen == u && c.destino == v) ||
                (c.origen == v && c.destino == u)) {
                return c.distanciaBase;
            }
        }
        return 1e9;
    }

    bool esTransitable(int u, int v) const {
        return obtenerCosto(u, v) < 1e9;
    }

    void bloquearNodo(int idNodo, bool estado) {
        for (auto& c : aristas) {
            if (c.origen == idNodo || c.destino == idNodo) {
                c.bloqueada = estado;
            }
        }
    }

    void bloquearArista(int origen, int destino, bool estado) {
        for (auto& c : aristas) {
            if ((c.origen == origen && c.destino == destino) ||
                (c.origen == destino && c.destino == origen)) {
                c.bloqueada = estado;
            }
        }
    }

    vector<int> dfs(int inicio) {
        vector<int> recorrido;
        if (inicio < 0 || inicio >= (int)nodos.size()) return recorrido;

        vector<bool> vis(nodos.size(), false);
        stack<int> st;
        st.push(inicio);

        while (!st.empty()) {
            int u = st.top();
            st.pop();

            if (vis[u]) continue;
            vis[u] = true;
            recorrido.push_back(u);

            auto vecinos = adj[u];
            for (int i = (int)vecinos.size() - 1; i >= 0; i--) {
                int v = vecinos[i];
                if (!vis[v] && esTransitable(u, v)) {
                    st.push(v);
                }
            }
        }

        return recorrido;
    }

    vector<int> bfs(int inicio) {
        vector<int> recorrido;
        if (inicio < 0 || inicio >= (int)nodos.size()) return recorrido;

        vector<bool> vis(nodos.size(), false);
        queue<int> q;
        q.push(inicio);
        vis[inicio] = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            recorrido.push_back(u);

            for (int v : adj[u]) {
                if (!vis[v] && esTransitable(u, v)) {
                    vis[v] = true;
                    q.push(v);
                }
            }
        }

        return recorrido;
    }

    vector<pair<int, double>> dfsConDistancias(int inicio) {
        vector<pair<int, double>> recorrido;
        if (inicio < 0 || inicio >= (int)nodos.size()) return recorrido;

        vector<bool> visitado(nodos.size(), false);
        stack<pair<int, double>> pila;
        pila.push({ inicio, 0.0 });

        while (!pila.empty()) {
            auto actualPar = pila.top();
            pila.pop();

            int actual = actualPar.first;
            double distActual = actualPar.second;

            if (!visitado[actual]) {
                visitado[actual] = true;
                recorrido.push_back({ actual, distActual });

                vector<int> vecinos = adj[actual];
                for (int i = (int)vecinos.size() - 1; i >= 0; i--) {
                    int v = vecinos[i];
                    double costo = obtenerCosto(actual, v);

                    if (!visitado[v] && costo < 1e9) {
                        pila.push({ v, distActual + costo });
                    }
                }
            }
        }

        return recorrido;
    }

    vector<pair<int, double>> bfsConDistancias(int inicio) {
        vector<pair<int, double>> recorrido;
        if (inicio < 0 || inicio >= (int)nodos.size()) return recorrido;

        vector<bool> visitado(nodos.size(), false);
        queue<pair<int, double>> cola;

        cola.push({ inicio, 0.0 });
        visitado[inicio] = true;

        while (!cola.empty()) {
            auto actualPar = cola.front();
            cola.pop();

            int actual = actualPar.first;
            double distActual = actualPar.second;

            recorrido.push_back({ actual, distActual });

            for (int v : adj[actual]) {
                double costo = obtenerCosto(actual, v);

                if (!visitado[v] && costo < 1e9) {
                    visitado[v] = true;
                    cola.push({ v, distActual + costo });
                }
            }
        }

        return recorrido;
    }

    vector<int> encontrarCaminoDFS(int ini, int fin) {
        vector<int> camino;
        if (ini < 0 || ini >= (int)nodos.size() || fin < 0 || fin >= (int)nodos.size()) {
            return camino;
        }

        vector<bool> vis(nodos.size(), false);
        bool found = false;

        function<void(int)> dfsRec = [&](int u) {
            if (found) return;

            vis[u] = true;
            camino.push_back(u);

            if (u == fin) {
                found = true;
                return;
            }

            for (int v : adj[u]) {
                if (!vis[v] && obtenerCosto(u, v) < 1e9) {
                    dfsRec(v);
                    if (found) return;
                }
            }

            camino.pop_back();
            };

        dfsRec(ini);
        return camino;
    }

    pair<vector<int>, double> dijkstra(int ini, int fin) {
        int n = (int)nodos.size();
        vector<double> dist(n, 1e9);
        vector<int> prev(n, -1);
        vector<bool> visitado(n, false);

        dist[ini] = 0;

        for (int i = 0; i < n; i++) {
            int u = -1;

            for (int j = 0; j < n; j++) {
                if (!visitado[j] && (u == -1 || dist[j] < dist[u])) {
                    u = j;
                }
            }

            if (u == -1 || dist[u] >= 1e9) break;

            visitado[u] = true;

            for (int v : adj[u]) {
                double w = obtenerCosto(u, v);
                if (w < 1e9 && dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    prev[v] = u;
                }
            }
        }

        vector<int> path;
        if (dist[fin] >= 1e9) {
            return { path, 1e9 };
        }

        for (int at = fin; at != -1; at = prev[at]) {
            path.push_back(at);
        }

        reverse(path.begin(), path.end());
        return { path, dist[fin] };
    }

    pair<vector<int>, double> caminoMasCorto(int ini, int fin) {
        return dijkstra(ini, fin);
    }

    pair<vector<int>, double> caminoConReferencias(int ini, int fin, const vector<int>& refs) {
        vector<int> total;
        double costoTotal = 0.0;

        vector<int> puntos;
        puntos.push_back(ini);
        for (int r : refs) puntos.push_back(r);
        puntos.push_back(fin);

        for (size_t i = 0; i + 1 < puntos.size(); i++) {
            auto tramo = dijkstra(puntos[i], puntos[i + 1]);

            if (tramo.first.empty()) {
                return { {}, 1e9 };
            }

            if (!total.empty() && !tramo.first.empty()) {
                tramo.first.erase(tramo.first.begin());
            }

            total.insert(total.end(), tramo.first.begin(), tramo.first.end());
            costoTotal += tramo.second;
        }

        return { total, costoTotal };
    }

    bool esConexo() {
        if (nodos.empty()) return true;
        auto recorrido = bfs(0);
        return recorrido.size() == nodos.size();
    }

    string nombreNodo(int id) const {
        if (id >= 0 && id < (int)nodos.size()) {
            return nodos[id].nombre;
        }
        return "Desconocido";
    }

    int buscarNodoPorNombre(string n) const {
        for (int i = 0; i < (int)nodos.size(); i++) {
            if (nodos[i].nombre == n) return i;
        }
        return -1;
    }
};