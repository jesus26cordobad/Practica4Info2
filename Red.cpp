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
        std::cout << "No se puede eliminar: el enrutador con ID " << id << " no existe.\n";
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

void Red::conectarRouters(char id1, char id2, int costo) { // Conecta dos routers con un costo dado
    if (routers.count(id1) && routers.count(id2)) {                   // Asegura que ambos routers existan
        routers[id1]->nuevoVecino(routers[id2], costo);               // Agrega vecino al router 1
        routers[id2]->nuevoVecino(routers[id1], costo);               // Agrega vecino al router 2
        actualizarTablas();                                           // Actualiza las rutas
    } else {
        std::cout << "No se puede conectar: uno o ambos enrutadores no existen.\n";
    }
}

void Red::eliminarConexion(char id1, char id2) { // Elimina la conexión entre dos routers
    if (!routers.count(id1) || !routers.count(id2)) {                // Verifica que existan los routers
        std::cout << "No se puede eliminar la conexion: uno o ambos enrutadores no existen.\n";
        return;
    }

    auto& v1 = routers[id1]->vecinos;  // Vecinos del router 1
    auto& v2 = routers[id2]->vecinos;  // Vecinos del router 2

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
    std::cout << "\nLa red contiene " << routers.size() << " enrutadore(s)." << std::endl; // Imprime la cantidad de routers en la red

    // Recorre todos los routers almacenados en el mapa 'routers'
    // El auto& [id, router] desestructura cada par clave-valor: id es el identificador, router es el puntero al objeto Router
    for (const auto& [id, router] : routers) {
        std::cout << "\nEnrutador " << id << " tiene vecinos:" << std::endl;  // Imprime el ID del router actual

        // Recorre la lista de vecinos del router actual, donde vecino es un puntero a Router y costo es el peso del enlace
        for (const auto& [vecino, costo] : router->vecinos) {
            std::cout << "    |- Vecino: " << vecino->idRouter << ", Costo: " << costo << std::endl; // Imprime el ID del vecino y el costo del enlace que conecta ambos routers
        }
    }
}

void Red::verTabla()
{
    for (const auto& [id, router] : routers) { // Recorre todos los routers en la red
        std::cout << "\nTabla de enrutamiento del enrutador " << id << ":\n";

        std::cout << "    Destino: " << id
                  << " | Costo: 0" << std::endl;   // Imprime que para llegar a sí mismo el costo es 0

        if (router->routingTable.empty()) {
            std::cout << "    (Vacia o no calculada aun)\n";
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
    if (!routers.count(origen) || !routers.count(destino)) return ruta;  // Si alguno no existe, retorna vacío

    char actual = origen;
    ruta.push_back(actual);                                         // Comienza desde el origen

    while (actual != destino) {
        Router* r = routers[actual];
        char siguienteSalto = r->obtenerSiguienteSalto(destino);    // Obtiene el próximo paso en la ruta

        if (siguienteSalto == -1 || siguienteSalto == actual) {
            return {};                                              // No hay ruta posible
        }

        ruta.push_back(siguienteSalto);                             // Agrega el siguiente nodo a la ruta
        actual = siguienteSalto;
    }

    return ruta;
}

int Red::obtenerCosto(char origen, char destino) { // Devuelve el costo total del camino más corto entre dos routers
    if (origen == destino) return 0;
    if (routers.count(origen) && routers[origen]->routingTable.count(destino)) {
        return routers[origen]->routingTable[destino].first;       // Devuelve el costo guardado en la tabla
    }
    return INT_MAX;                                                 // Si no hay ruta, se devuelve "infinito"
}

void Red::generarRedAleatoria(int numRouters, int conexionesMax) {
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

    // Para cada router, crea entre 1 y conexionesMax conexiones aleatorias
    for (int i = 0; i < numRouters; ++i) {
        char idActual = 'A' + i;  // ID del router actual
        Router* routerActual = routers[idActual];  // Obtiene puntero al router

        int conexiones = rand() % conexionesMax + 1;  // Número aleatorio de conexiones

        for (int j = 0; j < conexiones; ++j) {
            int vecino = rand() % numRouters;  // Índice aleatorio para vecino
            char idVecino = 'A' + vecino;      // ID del vecino
            Router* routerVecino = routers[idVecino];  // Puntero al router vecino

            // Verifica que no se conecte consigo mismo y que la conexión no exista
            if (vecino != i &&
                std::none_of(routerActual->vecinos.begin(), routerActual->vecinos.end(),
                             [routerVecino](const std::pair<Router*, int>& p) {
                                 return p.first == routerVecino;  // Ya está conectado a este vecino
                             }))
            {
                int costo = rand() % 10 + 1;  // Costo aleatorio entre 1 y 10
                conectarRouters(idActual, idVecino, costo);  // Crea la conexión
            }
        }
    }
}

void Red::configurarRedDesdeArchivo(const std::string& archivo) {  // Carga los datos de la red desde un archivo de texto
    std::ifstream file(archivo);
    if (!file) {
        std::cerr << "Error abriendo el archivo\n";
        return;
    }

    char id1, id2;
    int costo;
    while (file >> id1 >> id2 >> costo) {   // Lee tripletas de valores (conexiones)
        agregarRouter(id1);
        agregarRouter(id2);
        conectarRouters(id1, id2, costo);   // Crea la conexión leída del archivo
    }
}

void Red::actualizarTablas() {  // Actualiza las tablas de enrutamiento de todos los routers
    for (auto& [_, r] : routers) {
        r->actualizarTabla(routers);   // Cada router recalcula sus rutas
    }
}

Router* Red::obtenerRouter(char id) const { // Retorna el puntero al router con el ID dado
    if (routers.count(id)) {
        return routers.at(id);          // Accede al router si existe
    }
    return nullptr;
}

std::unordered_map<char, Router*>& Red::obtenerRouters() {  // Devuelve una referencia al mapa de routers completo
    return routers;
}

// Algoritmo de Dijkstra que retorna el camino más corto desde 'fuente' hasta 'destino'
std::vector<char> dijkstra(Router* fuente, char destino) {
    std::unordered_map<char, char> padres;    // Mapa para reconstruir la ruta
    fuente->confDistancia(0);               // Establece la distancia de la fuente a 0
    padres[fuente->idRouter] = -1;          // La fuente no tiene padre

    std::priority_queue<std::pair<int, Router*>> pq; // Cola de prioridad para el más cercano
    pq.push({0, fuente});                            // Comienza con la fuente

    while (!pq.empty()) {
        Router* actual = pq.top().second;
        pq.pop();

        if (actual->visitado) continue;           // Si ya fue visitado, lo ignora
        actual->visitado = true;

        for (auto& vec : actual->vecinos) {       // Recorre todos los vecinos del actual
            Router* sigRouter = vec.first;
            int costoBorde = vec.second;

            int nuevaDistancia = actual->distancia + costoBorde;   // Distancia total hasta el vecino
            if (nuevaDistancia < sigRouter->distancia) {           // Si mejora la distancia
                sigRouter->confDistancia(nuevaDistancia);          // Actualiza distancia
                padres[sigRouter->idRouter] = actual->idRouter;    // Guarda el padre

                sigRouter->routingTable[sigRouter->idRouter] = {sigRouter->distancia, actual->idRouter};

                pq.push({-nuevaDistancia, sigRouter});  // Inserta en la cola (negativo por prioridad mínima)
            }
        }
    }

    // Reconstrucción del camino
    std::vector<char> ruta;
    int actual = destino;
    while (actual != -1) {
        ruta.push_back(actual);    // Agrega el nodo actual a la ruta
        actual = padres[actual];   // Retrocede al nodo padre
    }
    std::reverse(ruta.begin(), ruta.end());   // Invierte para mostrar desde origen a destino
    return ruta;
}
