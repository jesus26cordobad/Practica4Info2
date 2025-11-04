#ifndef RED_H
#define RED_H

#include "Router.h"
#include <unordered_map>           // Para usar mapas hash con claves únicas (IDs de routers)
#include <vector>                  // Para listas dinámicas como rutas
#include <string>

class Red {
private:
    std::unordered_map<char, Router*> routers; // Mapa que almacena los routers en la red, donde la clave es el ID del router

public:
    Red();   // Constructor: inicializa la red vacía

    ~Red();

    void agregarRouter(char id);   // Agrega un router nuevo con un ID específico

    void eliminarRouter(char id);  // Elimina el router con el ID dado y sus conexiones

    void conectarRouters(char id1, char id2, int costo);   // Conecta dos routers existentes con un costo dado

    void eliminarConexion(char id1, char id2);    // Elimina la conexión entre dos routers

    void verElementos();

    void verTabla();

    std::vector<char> obtenerRutaCorta(char origen, char destino);  // Devuelve el camino más corto (como lista de IDs) entre dos routers

    int obtenerCosto(char origen, char destino);     // Devuelve el costo total del envío de un paquete entre dos routers

    void generarRedAleatoria(int numRouters, int conexionesMax);   // Genera una red aleatoria con la cantidad de routers y conexiones máximas

    void configurarRedDesdeArchivo(const std::string& archivo);    // Carga la configuración de la red desde un archivo

    void actualizarTablas();    // Actualiza todas las tablas de enrutamiento de los routers en la red

    Router* obtenerRouter(char id) const;     // Devuelve el puntero al router con el ID dado (o nullptr si no existe)

    std::unordered_map<char, Router*>& obtenerRouters();   // Devuelve una referencia al mapa completo de routers
};

std::vector<char> dijkstra(Router* fuente, char destino);  // Encontrar la ruta más corta entre una fuente y un destino dado

#endif // RED_H
