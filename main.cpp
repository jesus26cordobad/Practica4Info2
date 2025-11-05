#include <iostream>
#include <limits>
#include "Red.h"

using namespace std;

//Función auxiliar para limpiar el buffer de entrada
void limpiarBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

//Función para validar que el ID sea una letra
bool esLetraValida(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

//Función para obtener un ID válido
char obtenerIDValido(const string& mensaje) {
    char id;
    while (true) {
        cout << mensaje;
        cin >> id;

        if (cin.fail()) {
            cout << "Error: entrada invalida. Intente nuevamente.\n";
            limpiarBuffer();
            continue;
        }

        if (!esLetraValida(id)) {
            cout << "Error: el ID debe ser una letra (A-Z).\n";
            limpiarBuffer();
            continue;
        }

        // Convertir a mayúscula para consistencia
        if (id >= 'a' && id <= 'z') {
            id = id - 'a' + 'A';
        }

        limpiarBuffer();
        return id;
    }
}

int main() {

    Red red;
    int opcion;

    do {
        cout << "\n*** MENU DE LA RED DE ENRUTADORES ***\n";
        cout << "1. Agregar un enrutador\n";
        cout << "2. Eliminar un enrutador\n";
        cout << "3. Ver enrutador (vecinos)\n";
        cout << "4. Ver tabla de enrutamiento\n";
        cout << "5. Conectar dos enrutadores\n";
        cout << "6. Eliminar una conexion\n";
        cout << "7. Obtener la ruta mas corta\n";
        cout << "8. Obtener el costo de enviar un paquete\n";
        cout << "9. Generar red aleatoria\n";
        cout << "10. Configurar red desde archivo\n";
        cout << "11. Salir\n";
        cout << "\nSeleccione una opcion: ";
        cin >> opcion;

        // Verificar si la entrada fue válida
        if (cin.fail()) {
            cout << "\nError: debe ingresar un numero.\n";
            limpiarBuffer();
            continue;
        }

        // Validar rango
        if (opcion < 1 || opcion > 11) {
            cout << "\nOpcion no valida. Debe estar entre 1 y 11.\n";
            limpiarBuffer();
            continue;
        }

        limpiarBuffer(); // Limpiar buffer después de leer la opción

        switch (opcion) {
        case 1: {
            char id = obtenerIDValido("ID del enrutador (letra A-Z): ");

            // Verificar si ya existe
            if (red.obtenerRouter(id) != nullptr) {
                cout << "\nError: el enrutador " << id << " ya existe.\n";
            } else {
                red.agregarRouter(id);
                cout << "\nEnrutador " << id << " agregado exitosamente.\n";
            }
            break;
        }
        case 2: {
            char id = obtenerIDValido("ID del enrutador a eliminar: ");

            // Verificar si existe antes de eliminar
            if (red.obtenerRouter(id) == nullptr) {
                cout << "\nError: el enrutador " << id << " no existe.\n";
            } else {
                red.eliminarRouter(id);
                cout << "\nEnrutador " << id << " eliminado exitosamente.\n";
            }
            break;
        }
        case 3: {
            if (red.obtenerRouters().empty()) {
                cout << "\nNo hay enrutadores agregados.\n";
            } else {
                red.verElementos();
            }
            break;
        }
        case 4: {
            if (red.obtenerRouters().empty()) {
                cout << "\nNo hay enrutadores agregados.\n";
            } else {
                red.verTabla();
            }
            break;
        }
        case 5: {
            if (red.obtenerRouters().empty()) {
                cout << "\nNo hay enrutadores agregados.\n";
                break;
            }

            char id1 = obtenerIDValido("ID del primer enrutador: ");
            char id2 = obtenerIDValido("ID del segundo enrutador: ");

            // Verificar que ambos existan
            if (red.obtenerRouter(id1) == nullptr || red.obtenerRouter(id2) == nullptr) {
                cout << "\nError: uno o ambos enrutadores no existen.\n";
                break;
            }

            int costo;
            cout << "Costo del enlace: ";
            cin >> costo;

            if (cin.fail() || costo <= 0) {
                cout << "\nError: el costo debe ser un numero positivo.\n";
                limpiarBuffer();
                break;
            }

            limpiarBuffer();
            red.conectarRouters(id1, id2, costo);
            cout << "\nLa conexion se ha realizado exitosamente.\n";
            break;
        }
        case 6: {
            if (red.obtenerRouters().empty()) {
                cout << "\nNo hay enrutadores agregados.\n";
                break;
            }

            char id1 = obtenerIDValido("ID del primer enrutador: ");
            char id2 = obtenerIDValido("ID del segundo enrutador: ");

            // Verificar que ambos existan
            if (red.obtenerRouter(id1) == nullptr || red.obtenerRouter(id2) == nullptr) {
                cout << "\nError: uno o ambos enrutadores no existen.\n";
                break;
            }

            red.eliminarConexion(id1, id2);
            cout << "\nLa conexion se ha eliminado exitosamente.\n";
            break;
        }
        case 7: {
            if (red.obtenerRouters().empty()) {
                cout << "\nNo hay enrutadores agregados.\n";
                break;
            }

            char origen = obtenerIDValido("ID del enrutador de origen: ");
            char destino = obtenerIDValido("ID del enrutador de destino: ");

            // Verificar que ambos existan
            if (red.obtenerRouter(origen) == nullptr || red.obtenerRouter(destino) == nullptr) {
                cout << "\nError: uno o ambos enrutadores no existen.\n";
                break;
            }

            vector<char> ruta = red.obtenerRutaCorta(origen, destino);
            if (ruta.empty()) {
                cout << "\nNo se encontro una ruta.\n";
            } else {
                cout << "\nRuta mas corta: ";
                for (size_t i = 0; i < ruta.size(); ++i) {
                    cout << ruta[i];
                    if (i < ruta.size() - 1) cout << " -> ";
                }
                cout << "\n";
            }
            break;
        }
        case 8: {
            if (red.obtenerRouters().empty()) {
                cout << "\nNo hay enrutadores agregados.\n";
                break;
            }

            char origen = obtenerIDValido("ID del enrutador de origen: ");
            char destino = obtenerIDValido("ID del enrutador de destino: ");

            // Verificar que ambos existan
            if (red.obtenerRouter(origen) == nullptr || red.obtenerRouter(destino) == nullptr) {
                cout << "\nError: uno o ambos enrutadores no existen.\n";
                break;
            }

            int costo = red.obtenerCosto(origen, destino);
            if (costo == INT_MAX) {
                cout << "\nNo hay ruta disponible.\n";
            } else {
                cout << "\nCosto del envio: " << costo << "\n";
            }
            break;
        }
        case 9: {
            int cantidad, conexiones;
            cout << "Cantidad de enrutadores (1-26): ";
            cin >> cantidad;

            if (cin.fail() || cantidad < 1 || cantidad > 26) {
                cout << "\nError: la cantidad debe estar entre 1 y 26.\n";
                limpiarBuffer();
                break;
            }

            cout << "Maximo de conexiones por enrutador: ";
            cin >> conexiones;

            if (cin.fail() || conexiones < 1) {
                cout << "\nError: debe ser un numero positivo.\n";
                limpiarBuffer();
                break;
            }

            limpiarBuffer();
            red.generarRedAleatoria(cantidad, conexiones);
            cout << "\nLa red aleatoria se ha generado exitosamente.\n";
            break;
        }
        case 10: {
            string nombreArchivo;
            cout << "Nombre del archivo: ";
            cin >> nombreArchivo;
            limpiarBuffer();

            bool exito = red.configurarRedDesdeArchivo(nombreArchivo);
            if (exito) {
                cout << "\nRed configurada exitosamente.\n";
            } else {
                cout << "\nError: no se pudo configurar la red desde el archivo.\n";
            }
            break;
        }
        case 11:
            cout << "\nSaliendo del programa.\n";
            break;
        }
    } while (opcion != 11);

    return 0;
}
