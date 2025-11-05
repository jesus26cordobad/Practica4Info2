#include "Router.h"     // Incluye la definición de la clase Router
#include <climits>      // Para usar INT_MAX
#include <queue>        // Para usar priority_queue (cola de prioridad)

// Constructor del Router: inicializa su ID, la distancia en infinito y lo marca como no visitado
Router::Router(char id) : idRouter(id), distancia(INT_MAX), visitado(false) {}

void Router::nuevoVecino(Router* vecino, int costo) { // Agrega un nuevo vecino a la lista, junto con el costo del enlace
    vecinos.emplace_back(vecino, costo);  // emplace_back construye directamente el par (vecino, costo)
}

void Router::confDistancia(int dist) {  // Configura la distancia mínima conocida hacia este router
    distancia = dist;
}

void Router::reiniciar() {  // Reinicia el estado del router antes de aplicar Dijkstra (distancia infinita y no visitado)
    distancia = INT_MAX;
    visitado = false;
}

void Router::actualizarTabla(const std::unordered_map<char, Router*>& todosLosRouters) { // Actualiza la tabla de enrutamiento del router usando el algoritmo de Dijkstra
    // Reinicia todos los routers antes de ejecutar el algoritmo
    for (auto& [_, r] : todosLosRouters) {
        r->reiniciar();
    }

    confDistancia(0); // El router actual se considera como fuente, su distancia es 0

    std::priority_queue<std::pair<int, Router*>> pq; // Cola de prioridad para seleccionar el siguiente router más cercano
    pq.push({0, this});  // Agrega el router actual a la cola

    std::unordered_map<char, char> padres; // Mapa para rastrear el padre (predecesor) de cada router
    padres[idRouter] = -1;               // El router fuente no tiene padre

    while (!pq.empty()) {
        Router* actual = pq.top().second; // Toma el router con menor distancia (usando negativo)
        pq.pop();

        if (actual->visitado) continue;   // Si ya fue visitado, se salta
        actual->visitado = true;

        for (auto& vec : actual->vecinos) {  // Recorre todos los vecinos del router actual
            Router* sig = vec.first;     // Router vecino
            int costo = vec.second;      // Costo del enlace

            int nuevaDist = actual->distancia + costo;  // Calcula nueva distancia posible
            if (nuevaDist < sig->distancia) {           // Si es mejor que la actual
                sig->confDistancia(nuevaDist);          // Actualiza la distancia del vecino
                padres[sig->idRouter] = actual->idRouter; // Guarda el padre
                pq.push({-nuevaDist, sig});             // Se usa negativo porque la cola ordena de mayor a menor
            }
        }
    }

    routingTable.clear();  // Limpia la tabla anterior

    for (auto& [destinoId, padre] : padres) {  // Reconstruye la tabla de enrutamiento
        if (destinoId == idRouter) continue;  // No se agrega a sí mismo

        char salto = destinoId;
        // Retrocede hasta encontrar el siguiente salto desde este router
        while (padres[salto] != idRouter && padres[salto] != -1) {
            salto = padres[salto];
        }

        int costoFinal = todosLosRouters.at(destinoId)->distancia; // Distancia final desde la fuente al destino
        routingTable[destinoId] = {costoFinal, salto}; // Guarda en la tabla: destino -> (costo, siguiente salto)
    }
}

char Router::obtenerSiguienteSalto(char destino) { // Devuelve el siguiente salto que debe seguirse para llegar al destino
    if (routingTable.find(destino) != routingTable.end()) {
        return routingTable[destino].second;
    }
    return -1; // Si no hay una ruta conocida
}
