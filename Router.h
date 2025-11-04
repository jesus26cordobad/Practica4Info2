#ifndef ROUTER_H
#define ROUTER_H

#include <vector>               // Para usar el contenedor dinámico std::vector
#include <utility>              // Para usar std::pair
#include <unordered_map>        // Para usar tablas hash con std::unordered_map

class Router {
public:
    Router(char id); // Constructor: inicializa el router con su identificador

    char idRouter; // Identificador único del router

    std::vector<std::pair<Router*, int>> vecinos;   // Lista de vecinos del router: cada elemento es un par (Router*, costo)

    // Tabla de enrutamiento:
    // clave = ID del destino
    // valor = par (costo total hasta el destino, ID del siguiente salto)
    std::unordered_map<char, std::pair<int, char>> routingTable;

    int distancia;   // Almacenar distancia temporal desde el origen
    bool visitado;   // Marcar si el router ha sido visitado en la búsqueda de caminos

    void nuevoVecino(Router* vecino, int costo);    // Agrega o actualiza un vecino con el costo de conexión especificado

    void confDistancia(int distancia);    // Configura el valor de la distancia

    void actualizarTabla(const std::unordered_map<char, Router*>& todosLosRouters);     // Calcula y actualiza la tabla de enrutamiento basada en los routers disponibles

    char obtenerSiguienteSalto(char destino);     // Devuelve el siguiente salto hacia un destino según la tabla de enrutamiento

    void reiniciar();     // Restablece los valores temporales de distancia y visitado
};

#endif
