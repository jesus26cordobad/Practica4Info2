#include "Red.h"
#include <iostream>
#include <fstream>                  // Para leer archivos de texto
#include <cstdlib>                  // Para funciones aleatorias como rand() y srand()
#include <ctime>
#include <climits>                  // Para usar INT_MAX, que representa infinito
#include <queue>                    // Para usar la cola de prioridad (priority_queue)
#include <unordered_map>            // Para el uso del mapa hash (como un diccionario)
#include <algorithm>                // Para usar std::remove_if y std::find_if


Red::Red() { // Constructor de la clase Red (inicializa el objeto)
}

Red::~Red() { // Destructor: libera la memoria de todos los routers
    for (auto& pair : routers) {
        delete pair.second;        // Elimina cada puntero a Router
    }
}

void Red::agregarRouter(char id) { // Agrega un router nuevo a la red, si no existe aún
    if (routers.find(id) == routers.end()) {     // Si no existe un router con ese ID
        routers[id] = new Router(id);            // Crea un nuevo Router y lo agrega al mapa
    }
}

void Red::eliminarRouter(char id) { // Elimina un router de la red junto con sus conexiones
    if (routers.find(id) == routers.end()) {     // Si no se encuentra el router
        return;
    }

    Router* eliminado = routers[id];  // Guarda un puntero al router que será eliminado

    // Recorre todos los routers de la red
    for (auto& [_, r] : routers) {
        auto& vecinos = r->vecinos;   // Accede a la lista de vecinos de cada router

        // Elimina el router eliminado de la lista de vecinos
        vecinos.erase(
            std::remove_if(vecinos.begin(), vecinos.end(), [&](auto& p) {
                return p.first->idRouter == id;  // Compara por ID
            }),
            vecinos.end()
        );
    }

    delete eliminado;    // Libera la memoria del router eliminado
    routers.erase(id);   // Borra el router del mapa

    actualizarTablas();  // Actualiza las rutas en todos los routers
}

void Red::conectarRouters(char id1, char id2, int costo) {
    // Validación: mismo router
    if (id1 == id2) {
        std::cout << "Error: no se puede conectar un router consigo mismo.\n";
        return;
    }

    // Validación: costo positivo
    if (costo <= 0) {
        std::cout << "Error: el costo debe ser un numero positivo.\n";
        return;
    }

    // Validación: ambos routers existen (se maneja en main ahora)
    if (!routers.count(id1) || !routers.count(id2)) {
        return; // No imprimir mensaje aquí
    }

    // Verificar si ya existe la conexión
    Router* r1 = routers[id1];
    for (const auto& [vecino, _] : r1->vecinos) {
        if (vecino->idRouter == id2) {
            std::cout << "Advertencia: ya existe una conexion entre "
                      << id1 << " y " << id2 << ".\n";
            return;
        }
    }

    routers[id1]->nuevoVecino(routers[id2], costo);
    routers[id2]->nuevoVecino(routers[id1], costo);
    actualizarTablas();
}

void Red::eliminarConexion(char id1, char id2) { // Elimina la conexión entre dos routers
    if (!routers.count(id1) || !routers.count(id2)) {                // Verifica que existan los routers
        return;
    }

    auto& v1 = routers[id1]->vecinos;  // Vecinos del router 1
    auto& v2 = routers[id2]->vecinos;  // Vecinos del router 2

    // Verificar si existe la conexión antes de eliminar
    bool existeConexion = false;
    for (const auto& [vecino, _] : v1) {
        if (vecino->idRouter == id2) {
            existeConexion = true;
            break;
        }
    }

    if (!existeConexion) {
        std::cout << "Error: no existe una conexion entre "
                  << id1 << " y " << id2 << ".\n";
        return;
    }

    // Elimina al router 2 de la lista de vecinos del router 1
    v1.erase(std::remove_if(v1.begin(), v1.end(),
                            [&](auto& p){ return p.first->idRouter == id2; }), v1.end());

    // Elimina al router 1 de la lista de vecinos del router 2
    v2.erase(std::remove_if(v2.begin(), v2.end(),
                            [&](auto& p){ return p.first->idRouter == id1; }), v2.end());

    actualizarTablas();    // Recalcula las rutas
}

void Red::verElementos()
{
    std::cout << "\nLa red contiene " << routers.size() << " enrutador(es).\n"; // Imprime la cantidad de routers en la red

    // Recorre todos los routers almacenados en el mapa 'routers'
    // El auto& [id, router] desestructura cada par clave-valor: id es el identificador, router es el puntero al objeto Router
    for (const auto& [id, router] : routers) {
        std::cout << "\nEnrutador " << id;  // Imprime el ID del router actual

        if (router->vecinos.empty()) {
            std::cout << " (sin vecinos - AISLADO)";
        } else {
            std::cout << " tiene " << router->vecinos.size() << " vecino(s):";
        }
        std::cout << std::endl;

        // Recorre la lista de vecinos del router actual, donde vecino es un puntero a Router y costo es el peso del enlace
        for (const auto& [vecino, costo] : router->vecinos) {
            std::cout << "    |- Vecino: " << vecino->idRouter << ", Costo: " << costo << std::endl; // Imprime el ID del vecino y el costo del enlace que conecta ambos routers
        }
    }
}

void Red::verTabla()
{
    for (const auto& [id, router] : routers) { // Recorre todos los routers en la red
        std::cout << "\n=== Tabla de enrutamiento del enrutador " << id << " ===\n";

        std::cout << "    Destino: " << id
                  << " | Costo: 0 (si mismo)" << std::endl;   // Imprime que para llegar a sí mismo el costo es 0

        if (router->routingTable.empty()) {
            std::cout << "    (No hay otras rutas calculadas)\n";
            continue;
        }

        // Recorre cada entrada en la tabla de enrutamiento: destino es el ID del router destino,
        // info es un par (costo, siguiente salto)
        for (const auto& [destino, info] : router->routingTable) {
            int costo = info.first; // Extrae el costo desde el par
            std::cout << "    Destino: " << destino
                      << " | Costo: " << costo << std::endl;
        }
    }
}


std::vector<char> Red::obtenerRutaCorta(char origen, char destino) {  // Devuelve la ruta más corta entre dos routers como vector de IDs
    std::vector<char> ruta;
    if (!routers.count(origen) || !routers.count(destino)) {
        return ruta;  // Si alguno no existe, retorna vacío
    }

    if (origen == destino) {
        ruta.push_back(origen);
        return ruta;
    }

    char actual = origen;
    ruta.push_back(actual);                                         // Comienza desde el origen

    int maxIteraciones = routers.size();
    int iteraciones = 0;

    while (actual != destino && iteraciones < maxIteraciones) {
        Router* r = routers[actual];
        char siguienteSalto = r->obtenerSiguienteSalto(destino);    // Obtiene el próximo paso en la ruta

        if (siguienteSalto == -1 || siguienteSalto == actual) {
            return {};                                              // No hay ruta posible
        }

        ruta.push_back(siguienteSalto);                             // Agrega el siguiente nodo a la ruta
        actual = siguienteSalto;
        iteraciones++;
    }

    if (iteraciones >= maxIteraciones) {
        return {};
    }

    return ruta;
}

int Red::obtenerCosto(char origen, char destino) { // Devuelve el costo total del camino más corto entre dos routers
    if (origen == destino) return 0;

    if (!routers.count(origen)) {
        return INT_MAX;
    }

    if (routers[origen]->routingTable.count(destino)) {
        return routers[origen]->routingTable[destino].first;       // Devuelve el costo guardado en la tabla
    }

    return INT_MAX;                                                 // Si no hay ruta, se devuelve "infinito"
}

void Red::generarRedAleatoria(int numRouters, int conexionesMax) {
    if (numRouters <= 0 || numRouters > 26) {
        return;
    }

    srand(time(0));  // Inicializa la semilla para números aleatorios

    // Elimina memoria y vacía el contenedor
    for (auto& par : routers) {
        delete par.second;  // Elimina cada router creado con new
    }
    routers.clear();

    // Crear y agregar los routers nuevos con IDs consecutivos desde 'A'
    for (int i = 0; i < numRouters; ++i) {
        char id = 'A' + i;
        agregarRouter(id);
    }

    // Garantizar conectividad básica (árbol de expansión)
    for (int i = 0; i < numRouters - 1; ++i) {
        char id1 = 'A' + i;
        char id2 = 'A' + i + 1;
        int costo = rand() % 10 + 1;

        routers[id1]->nuevoVecino(routers[id2], costo);
        routers[id2]->nuevoVecino(routers[id1], costo);
    }

    // Agregar conexiones aleatorias adicionales
    // Nota: conexionesMax es el máximo de conexiones ADICIONALES, no total
    for (int i = 0; i < numRouters; ++i) {
        char idActual = 'A' + i;
        Router* routerActual = routers[idActual];

        // Calcular cuántas conexiones adicionales puede tener
        int conexionesActuales = routerActual->vecinos.size();
        int conexionesRestantes = conexionesMax - conexionesActuales;

        if (conexionesRestantes <= 0) continue;

        int conexionesAdicionales = rand() % (conexionesRestantes + 1);

        for (int j = 0; j < conexionesAdicionales; ++j) {
            int vecino = rand() % numRouters;
            char idVecino = 'A' + vecino;
            Router* routerVecino = routers[idVecino];

            if (vecino != i &&
                std::none_of(routerActual->vecinos.begin(), routerActual->vecinos.end(),
                             [routerVecino](const std::pair<Router*, int>& p) {
                                 return p.first == routerVecino;
                             }))
            {
                int costo = rand() % 10 + 1;
                routerActual->nuevoVecino(routerVecino, costo);
                routerVecino->nuevoVecino(routerActual, costo);
            }
        }
    }

    actualizarTablas();
}

bool Red::configurarRedDesdeArchivo(const std::string& archivo) {
    std::ifstream file(archivo);
    if (!file) {
        std::cout << "Error: el archivo '" << archivo << "' no existe o no se puede abrir.\n";
        return false;
    }

    // Limpiar red existente
    for (auto& par : routers) {
        delete par.second;
    }
    routers.clear();

    char id1, id2;
    int costo;
    int linea = 0;
    bool exitoso = true;

    while (file >> id1 >> id2 >> costo) {
        linea++;

        // Validar que sean letras
        if (!((id1 >= 'A' && id1 <= 'Z') || (id1 >= 'a' && id1 <= 'z')) ||
            !((id2 >= 'A' && id2 <= 'Z') || (id2 >= 'a' && id2 <= 'z'))) {
            std::cerr << "Linea " << linea << ": IDs invalidos ("
                      << id1 << ", " << id2 << "). Deben ser letras.\n";
            exitoso = false;
            continue;
        }

        // Convertir a mayúsculas
        if (id1 >= 'a' && id1 <= 'z') id1 = id1 - 'a' + 'A';
        if (id2 >= 'a' && id2 <= 'z') id2 = id2 - 'a' + 'A';

        if (id1 == id2) {
            std::cerr << "Linea " << linea << ": ignorando conexion de "
                      << id1 << " consigo mismo.\n";
            continue;
        }

        if (costo <= 0) {
            std::cerr << "Linea " << linea << ": costo invalido " << costo
                      << " entre " << id1 << " y " << id2 << ". Ignorando.\n";
            continue;
        }

        agregarRouter(id1);
        agregarRouter(id2);

        // Verificar si ya existe la conexión
        bool existe = false;
        if (routers.count(id1)) {
            for (const auto& [vecino, _] : routers[id1]->vecinos) {
                if (vecino->idRouter == id2) {
                    existe = true;
                    break;
                }
            }
        }

        if (!existe) {
            routers[id1]->nuevoVecino(routers[id2], costo);
            routers[id2]->nuevoVecino(routers[id1], costo);
        }
    }

    if (linea == 0) {
        std::cout << "Advertencia: el archivo esta vacio.\n";
        return false;
    }

    actualizarTablas();
    std::cout << "Red configurada desde archivo. " << linea
              << " linea(s) procesada(s).\n";

    return exitoso && !routers.empty();
}

void Red::actualizarTablas() {
    for (auto& [_, r] : routers) {
        r->actualizarTabla(routers);
    }
}

Router* Red::obtenerRouter(char id) const {
    if (routers.count(id)) {
        return routers.at(id);
    }
    return nullptr;
}

std::unordered_map<char, Router*>& Red::obtenerRouters() {
    return routers;
}
