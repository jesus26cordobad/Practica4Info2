#include <iostream>
#include "Red.h"

using namespace std;

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

        switch (opcion) {
        case 1: {
            char id;
            cout << "ID del enrutador (letra): ";
            cin >> id;
            red.agregarRouter(id);
            cout << "\nEnrutador " << id << " agregado exitosamente" << endl;
            break;
        }
        case 2: {
            char id;
            cout << "ID del enrutador a eliminar: ";
            cin >> id;
            red.eliminarRouter(id);
            cout << "\nEnrutador " << id << " eliminado exitosamente" << endl;
            break;
        }
        case 3: {
            red.verElementos();
            break;
        }
        case 4: {
            red.verTabla();
            break;
        }
        case 5: {
            char id1, id2;
            int costo;
            cout << "ID del primer enrutador: ";
            cin >> id1;
            cout << "ID del segundo enrutador: ";
            cin >> id2;
            cout << "Costo del enlace: ";
            cin >> costo;
            red.conectarRouters(id1, id2, costo);
            cout << "\nLa conexion se ha realizado exitosamente" << endl;
            break;
        }
        case 6: {
            char id1, id2;
            cout << "ID del primer enrutador: ";
            cin >> id1;
            cout << "ID del segundo enrutador: ";
            cin >> id2;
            red.eliminarConexion(id1, id2);
            cout << "\nLa conexion se ha eliminado exitosamente" << endl;
            break;
        }
        case 7: {
            char origen, destino;
            cout << "ID del enrutador de origen: ";
            cin >> origen;
            cout << "ID del enrutador de destino: ";
            cin >> destino;
            vector<char> ruta = red.obtenerRutaCorta(origen, destino);
            if (ruta.empty()) {
                cout << "No se encontro una ruta.\n";
            } else {
                cout << "Ruta mas corta: ";
                for (size_t i = 0; i < ruta.size(); ++i) {
                    cout << ruta[i];
                    if (i < ruta.size() - 1) cout << " -> ";
                }
                cout << "\n";
            }
            break;
        }
        case 8: {
            char origen, destino;
            cout << "ID del enrutador de origen: ";
            cin >> origen;
            cout << "ID del enrutador de destino: ";
            cin >> destino;
            int costo = red.obtenerCosto(origen, destino);
            if (costo == INT_MAX) {
                cout << "No hay ruta disponible.\n";
            } else {
                cout << "Costo del envio: " << costo << "\n";
            }
            break;
        }
        case 9: {
            int cantidad, conexiones;
            cout << "Cantidad de enrutadores: ";
            cin >> cantidad;
            cout << "Maximo de conexiones por enrutador: ";
            cin >> conexiones;
            red.generarRedAleatoria(cantidad, conexiones);
            cout << "\nLa red aleatoria se ha generado exitosamente" << endl;
            break;
        }
        case 10: {
            string nombreArchivo;
            cout << "Nombre del archivo: ";
            cin >> nombreArchivo;
            red.configurarRedDesdeArchivo(nombreArchivo);
            cout << "\nRed configurada exitosamente" << endl;
            break;
        }
        case 11:
            cout << "Saliendo del programa.\n";
            break;
        default:
            cout << "Opcion no valida.\n";
            break;
        }
    } while (opcion != 11);

    return 0;
}
