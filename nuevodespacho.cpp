#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstring>  // Para usar strncpy
#include <climits>  // Para usar INT_MAX
#include <limits>   // Para usar numeric_limits
#include <algorithm> // Para usar sort

using namespace std;

// Estructura para un despacho
struct Despacho {
    int NroDock;                  // Número de dock (0-7)
    char Producto[10];            // Nombre del producto (10 caracteres)
    char Provincia[19];           // Nombre de la provincia (19 caracteres)
    int Cantidad;                 // Cantidad despachada
};

// Función para leer el archivo binario
vector<Despacho> leerArchivo(const string& nombreArchivo) {
    vector<Despacho> despachos;
    ifstream archivo(nombreArchivo, ios::binary);

    if (!archivo) {
        cerr << "Error al abrir el archivo " << nombreArchivo << endl;
        return despachos;
    }

    Despacho d;
    while (archivo.read(reinterpret_cast<char*>(&d), sizeof(Despacho))) {
        despachos.push_back(d);
    }

    archivo.close();
    return despachos;
}

// Función para limpiar cadenas de caracteres
string limpiarString(const char* str, size_t size) {
    return string(str, strnlen(str, size));
}

// Función para listar despachos por dock
void listarPorDock(const vector<Despacho>& despachos, ofstream& archivoSalida) {
    // Almacena la cantidad de despachos por dock y los productos despachados
    map<int, map<string, int>> docks;  // Dock -> (Producto -> Cantidad Total)
    map<int, int> despachosPorDock;     // Dock -> Cantidad de Despachos

    // Procesar cada despacho
    for (const auto& d : despachos) {
        string producto = limpiarString(d.Producto, sizeof(d.Producto));
        despachosPorDock[d.NroDock]++;
        docks[d.NroDock][producto] += d.Cantidad;
    }

    // Listar resultados por dock en el archivo de salida
    for (const auto& dock : docks) {
        archivoSalida << "Dock Nro: " << dock.first << " - Despachos: " << despachosPorDock[dock.first] << endl;

        // Crear un vector para ordenar los productos
        vector<pair<string, int>> productos(dock.second.begin(), dock.second.end());
        sort(productos.begin(), productos.end()); // Ordenar por nombre del producto

        for (const auto& producto : productos) {
            archivoSalida << "  Producto: " << producto.first << ", Cantidad Total: " << producto.second << endl;
        }
        archivoSalida << endl;
    }
}

// Función para encontrar el dock con menos despachos
int dockConMenosDespachos(const map<int, int>& despachosPorDock) {
    int dockMin = -1;
    int minDespachos = INT_MAX;

    for (const auto& dock : despachosPorDock) {
        if (dock.second < minDespachos) {
            minDespachos = dock.second;
            dockMin = dock.first;
        }
    }

    return dockMin;
}

// Función para encontrar el producto con mayor cantidad en un dock
string productoConMayorCantidad(const map<string, int>& productos) {
    string productoMax;
    int maxCantidad = 0;

    for (const auto& producto : productos) {
        if (producto.second > maxCantidad) {
            maxCantidad = producto.second;
            productoMax = producto.first;
        }
    }

    return productoMax;
}

// Función para listar detalles de un producto en un dock específico
void mostrarDetalleDock(const vector<Despacho>& despachos, int nroDock, const string& productoBuscado, ofstream& archivoSalida) {
    archivoSalida << "Detalle de despachos del Dock " << nroDock << " para el producto " << productoBuscado << ":" << endl;
    for (const auto& d : despachos) {
        if (d.NroDock == nroDock && limpiarString(d.Producto, sizeof(d.Producto)) == productoBuscado) {
            archivoSalida << "Provincia: " << limpiarString(d.Provincia, sizeof(d.Provincia)) << ", Cantidad: " << d.Cantidad << endl;
        }
    }
}

int main() {
    cout << "Iniciando programa..." << endl;

    // Leer el archivo binario de despachos
    vector<Despacho> despachos = leerArchivo("Datos.bin");
    cout << "Archivo leído. Cantidad de despachos: " << despachos.size() << endl;

    // Abrir el archivo de salida para escribir
    ofstream archivoSalida("salida.txt");
    if (!archivoSalida) {
        cerr << "Error al crear el archivo de salida" << endl;
        return 1;
    }

    // Listar despachos por dock
    listarPorDock(despachos, archivoSalida);

    // Encontrar el dock con menos despachos
    map<int, int> despachosPorDock;     // Dock -> Cantidad de Despachos
    map<int, map<string, int>> docks;   // Dock -> (Producto -> Cantidad Total)
    for (const auto& d : despachos) {
        string producto = limpiarString(d.Producto, sizeof(d.Producto));
        despachosPorDock[d.NroDock]++;
        docks[d.NroDock][producto] += d.Cantidad;
    }

    int dockMin = dockConMenosDespachos(despachosPorDock);
    archivoSalida << "Dock con menos despachos: " << dockMin << endl;

    // Verificar si el dock tiene productos despachados
    if (!docks[dockMin].empty()) {
        // Encontrar el producto con mayor cantidad en ese dock
        string productoMax = productoConMayorCantidad(docks[dockMin]);
        archivoSalida << "Producto con mayor cantidad en Dock " << dockMin << ": " << productoMax << endl;

        // Mostrar detalles del producto más despachado en el dock con menos despachos
        mostrarDetalleDock(despachos, dockMin, productoMax, archivoSalida);
    } else {
        archivoSalida << "El Dock " << dockMin << " no tiene productos despachados." << endl;
    }

    // Cerrar el archivo de salida
    archivoSalida.close();

    // Esperar a que el usuario presione Enter antes de cerrar
    cout << "Presiona Enter para salir...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    return 0;
}
