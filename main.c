#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"

#define MAX_NOMBRE 100
#define MAX_DESCRIPCION 500
#define MAX_OBJETOS 10
#define MAX_NOMBRE_OBJETO 50
#define MAX_JUGADORES 2
#define TIEMPO_INICIAL 10

#define DEV 1

#define TITULO \
"    _____                     _      _____                    _   \n\
   |  __ \\                   | |    |  _  |                  | |   \n\
   | |  \\/ _ __  __ _  _ __  | |__  | | | | _   _   ___  ___ | |_  \n\
   | | __ | '__|/ _` || '_ \\ | '_ \\ | | | || | | | / _ \\/ __|| __| \n\
   | |_\\ | |  | (_| || |_) || | | \\ \\/' /| |_| ||  __/\\__ \\| |_  \n\
    \\____/|_|   \\__,_|| .__/ |_| |_| \\_/\\_\\ \\__,_| \\___||___/ \\__| \n\
                      | |                                          \n\
                      |_|                                          \n\
"


// Estructura para representar un objeto
typedef struct {
    char nombre[MAX_NOMBRE_OBJETO];
    int peso;
    int valor;
} Objeto;


// Estructura para representar un escenario (nodo en el grafo)
typedef struct Escenario {
    int id;
    int id_up, id_down, id_left, id_right;

    char nombre[MAX_NOMBRE];
    char descripcion[MAX_DESCRIPCION];

    Objeto objetos[MAX_OBJETOS];
    int cantidadObjetos;

    struct Escenario *arriba;
    struct Escenario *abajo;
    struct Escenario *izquierda;
    struct Escenario *derecha;

    int esFinal;
} Escenario;

// Estructura para representar un jugador
typedef struct {
    char C; // personaje (A-Z)
    Objeto inventario[MAX_OBJETOS];
    float tiempo;
    int n_objetos;
    Escenario* escenario;
} Jugador;

// Estructura para representar una Partida
typedef struct {
    Jugador jugadores[MAX_JUGADORES];
    int n_jugadores;
    int turno;
} Partida;

// Estructura para representar el resultado de un jugador
typedef struct Resultado {
    char jugador;
    int total;
    int gano;   // 1 si ganó, 0 si no
    struct Resultado* siguiente;
    float tiempo;
} Resultado;

// Function prototypes
Partida* nuevaPartida();

// Limpiar buffer
void limpiar(){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}

/*
Función para analizar objetos desde una cadena de texto y guardarlos en el 
programa, se considera que los valores estan separados por comas. Tambien
valida las entradas y muestra un mensaje de error si no se puede leer.
*/
int analizarObjetos(const char* cadenaObjetos, Objeto* objetos, int* cantidadObjetos) {
    if (!cadenaObjetos || !objetos || !cantidadObjetos) {
        printf("Error: Parámetros inválidos en analizarObjetos\n");
        return 0;
    }
    
    // Si la cadena está vacía, no hay objetos
    if (strlen(cadenaObjetos) == 0) {
        *cantidadObjetos = 0;
        return 1;
    }
    
    char* copiaObjetos = strdup(cadenaObjetos);
    if (!copiaObjetos) {
        printf("Error: No se pudo duplicar la cadena de objetos\n");
        return 0;
    }

    if (copiaObjetos[0] == '"') 
        copiaObjetos++;
    
    // Contar número de items (número de ; + 1)
    int numItems = 1;
    char* temp = copiaObjetos;
    while ((temp = strchr(temp, ';')) != NULL) {
        numItems++;
        temp++;
    }
    
    // Reiniciar para el parsing
    temp = copiaObjetos;
    *cantidadObjetos = 0;
    
    // Procesar cada item
    for (int i = 0; i < numItems && *cantidadObjetos < MAX_OBJETOS; i++) {
        // Encontrar el final del item actual
        char* itemEnd = strchr(temp, ';');
        if (itemEnd) {
            *itemEnd = '\0';  // Terminar el item actual
        }
        
        // Eliminar espacios en blanco al inicio y final
        while (*temp == ' ') temp++;
        char* fin = temp + strlen(temp) - 1;
        while (fin > temp && *fin == ' ') *fin-- = '\0';
        
        // Crear una copia del item para analizarlo
        char* itemCopia = strdup(temp);
        if (!itemCopia) {
            printf("Error: No se pudo duplicar el item\n");
            free(copiaObjetos);
            return 0;
        }
        
        // Obtener nombre
        char* nombre = strtok(itemCopia, ",");
        if (!nombre) {
            printf("Error: No se pudo leer el nombre del objeto\n");
            free(itemCopia);
            free(copiaObjetos);
            return 0;
        }
        
        // Obtener peso
        char* pesoStr = strtok(NULL, ",");
        if (!pesoStr) {
            printf("Error: No se pudo leer el peso del objeto\n");
            free(itemCopia);
            free(copiaObjetos);
            return 0;
        }
        int peso = atoi(pesoStr);
        
        // Obtener valor
        char* valorStr = strtok(NULL, ",");
        if (!valorStr) {
            printf("Error: No se pudo leer el valor del objeto\n");
            free(itemCopia);
            free(copiaObjetos);
            return 0;
        }
        int valor = atoi(valorStr);
        
        // Guardar el objeto
        strncpy(objetos[*cantidadObjetos].nombre, nombre, MAX_NOMBRE_OBJETO - 1);
        objetos[*cantidadObjetos].nombre[MAX_NOMBRE_OBJETO - 1] = '\0';
        objetos[*cantidadObjetos].peso = peso;
        objetos[*cantidadObjetos].valor = valor;
        
        (*cantidadObjetos)++;
        free(itemCopia);
        
        // Mover al siguiente item
        if (itemEnd) {
            temp = itemEnd + 1;
        }
    }
    
    if (*cantidadObjetos >= MAX_OBJETOS) {
        printf("Advertencia: Se alcanzó el límite máximo de objetos (%d)\n", MAX_OBJETOS);
    }
    
    free(copiaObjetos);
    return 1;
}

/*
La funcion cargarEScenarios lee desde un archivo CSV los escenarios con su debida
descripcion y la informacion que contiene cada uno.
Esto se representa en un grafo de nodos, incluyendo el escenario final.
Se devuelve 1 si se pudo leer el archivo y NULL si no se pudo leer.
Tambien se evitan las fugas de memoria.
*/
Escenario* cargarEscenarios(const char* nombreArchivo) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (!archivo) {
        // Para evitar errores de memoria
        printf("Error al abrir el archivo CSV\n");
        return NULL;
    }
    
    // Array temporal para almacenar todos los escenarios
    Escenario* escenarios = NULL;
    int capacidad = 20;
    int cantidad = 0;
    
    escenarios = (Escenario*)malloc(sizeof(Escenario) * capacidad);
    if (!escenarios) {
        fclose(archivo);
        printf("Error al inicializar el array de escenarios\n");
        return NULL;
    }
    
    char linea[1024];
    // Saltar línea de encabezado
    fgets(linea, sizeof(linea), archivo);
    
    int i = 0;
    while (fgets(linea, sizeof(linea), archivo)) {
        if (cantidad >= capacidad) {
            capacidad *= 2;
            Escenario* nuevosEscenarios = (Escenario*)realloc(escenarios, sizeof(Escenario) * capacidad);
            if (!nuevosEscenarios) {
                printf("Error al redimensionar el array de escenarios\n");
                free(escenarios);
                fclose(archivo);
                return NULL;
            }
            escenarios = nuevosEscenarios;
        }
        
        Escenario* escenario = &escenarios[cantidad];
        memset(escenario, 0, sizeof(Escenario)); // Inicializar todos los campos a 0
        
        char cadenaObjetos[500] = "";  // Inicializar como cadena vacía
        char cadenaEsFinal[10];
        char descripcionTemp[1024];  // Buffer temporal para la descripción
        char* token;
        char* lineaCopia = strdup(linea);
        
        if (!lineaCopia) {
            printf("Error al duplicar la línea\n");
            continue;
        }

        // Obtener ID
        token = strtok(lineaCopia, ",");
        if (!token) {
            printf("Error: No se pudo leer el ID\n");
            free(lineaCopia);
            continue;
        }
        escenario->id = atoi(token);

        // Obtener Nombre
        token = strtok(NULL, ",");
        if (!token) {
            printf("Error: No se pudo leer el Nombre\n");
            free(lineaCopia);
            continue;
        }
        strncpy(escenario->nombre, token, MAX_NOMBRE - 1);
        escenario->nombre[MAX_NOMBRE - 1] = '\0';

        // Obtener Descripción (puede estar entre comillas o no)
        token = strtok(NULL, "\"");
        if (!token) {
            printf("Error: No se pudo leer la Descripción\n");
            free(lineaCopia);
            continue;
        }

        // Verificar si la descripción está entre comillas
        if (token[0] == ',') {
            // La descripción no está entre comillas, usar el token actual
            token++; // Saltar la coma inicial
            char* descEnd = strchr(token, ',');
            if (!descEnd) {
                printf("Error: No se encontró el final de la descripción\n");
                free(lineaCopia);
                continue;
            }
            *descEnd = '\0'; // Terminar la descripción
            strncpy(descripcionTemp, token, MAX_DESCRIPCION - 1);
            descripcionTemp[MAX_DESCRIPCION - 1] = '\0';
            token = descEnd + 1; // Mover al siguiente campo
        } else {
            // La descripción está entre comillas
            strncpy(descripcionTemp, token, MAX_DESCRIPCION - 1);
            descripcionTemp[MAX_DESCRIPCION - 1] = '\0';
            
            // Saltar la siguiente comilla y obtener el resto de la línea
            token = strtok(NULL, "\n");
            if (!token) {
                free(lineaCopia);
                break;
            }
            
            // Eliminar la coma inicial si existe
            if (token[0] == ',') token++;
        }

        // Ahora token contiene: items,arriba,abajo,izquierda,derecha,esFinal
        // Obtener Objetos con las tres condiciones
        char* itemsEnd = NULL;
        int numItems = 0;
        
        // Condición 1: No hay items (coma justo después)
        if (token[0] == ',') {
            cadenaObjetos[0] = '\0';  // Cadena vacía
            itemsEnd = token;
        } else {
            // Buscar si hay múltiples items (condición 3)
            char* semicolon = strchr(token, ';');
            if (semicolon) {
                // Contar número de items (número de ; + 1)
                char* temp = token;
                while ((temp = strchr(temp, ';')) != NULL) {
                    numItems++;
                    temp++;
                }
                numItems++; // Añadir el último item
                
                // Encontrar el final del último item
                itemsEnd = token;
                for (int i = 0; i < numItems * 2 + 1; i++) {
                    itemsEnd = strchr(itemsEnd, ',');
                    if (!itemsEnd) {
                        printf("Error: Formato de items inválido\n");
                        free(lineaCopia);
                        continue;
                    }
                    itemsEnd++;
                }
                itemsEnd--; // Retroceder una posición para estar en la coma
            } else {
                // Condición 2: Un solo item
                itemsEnd = token;
                // Saltar dos comas para un solo item (name,weight,value)
                for (int i = 0; i < 3; i++) {
                    itemsEnd = strchr(itemsEnd, ',');
                    if (!itemsEnd) {
                        printf("Error: Formato de item inválido\n");
                        free(lineaCopia);
                        continue;
                    }
                    itemsEnd++;
                }
                itemsEnd--; // Retroceder una posición para estar en la coma
            }
            
            // Copiar los items
            *itemsEnd = '\0';  // Terminar la cadena de objetos
            strncpy(cadenaObjetos, token, sizeof(cadenaObjetos) - 1);
            cadenaObjetos[sizeof(cadenaObjetos) - 1] = '\0';
        }

        // Mover al siguiente campo
        token = itemsEnd + 1;

        // Obtener ID Arriba
        char* arribaEnd = strchr(token, ',');
        if (!arribaEnd) {
            printf("Error: No se encontró el final de Arriba\n");
            free(lineaCopia);
            continue;
        }
        *arribaEnd = '\0';
        escenario->id_up = atoi(token);
        token = arribaEnd + 1;

        // Obtener ID Abajo
        char* abajoEnd = strchr(token, ',');
        if (!abajoEnd) {
            printf("Error: No se encontró el final de Abajo\n");
            free(lineaCopia);
            continue;
        }
        *abajoEnd = '\0';
        escenario->id_down = atoi(token);
        token = abajoEnd + 1;

        // Obtener ID Izquierda
        char* izqEnd = strchr(token, ',');
        if (!izqEnd) {
            printf("Error: No se encontró el final de Izquierda\n");
            free(lineaCopia);
            continue;
        }
        *izqEnd = '\0';
        escenario->id_left = atoi(token);
        token = izqEnd + 1;

        // Obtener ID Derecha
        char* derEnd = strchr(token, ',');
        if (!derEnd) {
            printf("Error: No se encontró el final de Derecha\n");
            free(lineaCopia);
            continue;
        }
        *derEnd = '\0';
        escenario->id_right = atoi(token);
        token = derEnd + 1;


        // Obtener EsFinal (último campo)
        strncpy(cadenaEsFinal, token, sizeof(cadenaEsFinal) - 1);
        cadenaEsFinal[sizeof(cadenaEsFinal) - 1] = '\0';

        // Copiamos la descripción al escenario
        strncpy(escenario->descripcion, descripcionTemp, MAX_DESCRIPCION - 1);
        escenario->descripcion[MAX_DESCRIPCION - 1] = '\0';

        // Analizar objetos
        if (!analizarObjetos(cadenaObjetos, escenario->objetos, &escenario->cantidadObjetos)) {
            printf("Error al analizar objetos para el escenario %d\n", escenario->id);
        }
        
        // Establecer bandera esFinal
        escenario->esFinal = (strcmp(cadenaEsFinal, "Si") == 0);
        
        cantidad++;
        free(lineaCopia);
        i++;
    }

    // Establecer las conexiones entre escenarios
    for (int i = 0; i < cantidad; i++) {
        Escenario* escenario = &escenarios[i];
        
        escenario->arriba = NULL;
        escenario->abajo = NULL;
        escenario->izquierda = NULL;
        escenario->derecha = NULL;

        for (int j = 0; j < cantidad; j++) {
            if (escenario->id_up != -1 && escenario->id_up == escenarios[j].id)
                escenario->arriba = &escenarios[j];
            if (escenario->id_down != -1 && escenario->id_down == escenarios[j].id)
                escenario->abajo = &escenarios[j];
            if (escenario->id_left != -1 && escenario->id_left == escenarios[j].id)
                escenario->izquierda = &escenarios[j];
            if (escenario->id_right != -1 && escenario->id_right == escenarios[j].id)
                escenario->derecha = &escenarios[j];
        }        
    }
    // Buscar el escenario inicial (id 1)
    Escenario* inicio = NULL;
    for (int i = 0; i < cantidad; i++) {
        if (escenarios[i].id == 1) {
            inicio = &escenarios[i];
            break;
        }
    }
    fclose(archivo);
    return inicio;
}

// Funcion para pausar la ejecucion del programa durante un tiempo especifico (ms)
void sleep(int ms) {
    int i = 0;
    while (i < ms * 2000000 * (1 - DEV)) {
        i++;
    }
}

/* Funcion creada mas que nada por estetica y controlar el ancho de linea al momento
de imprimir el texto en pantalla */
int typewriter(const char* text) {
    int w = 0;
    int h = 0;
    printf(" ");
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] == ' ' && w > 35) {
            printf("\n ");
            w = 0;
            i++;
            h++;
        }
        printf("%c", text[i]);
        sleep(10); // Pausa de 10 milisegundos
        w++;
    }
    return h;
}

// Función para mostrar el escenario actual
void mostrarEscenario(Escenario* escenario, Partida* partida) {
    // Limpiar pantalla (Windows)
    system("cls");

    const char* W = escenario->arriba ? "^^^^^" : "-----";
    const char* A = escenario->izquierda ? "<" : "|";
    const char* D = escenario->derecha ? ">" : "|";
    const char* S = escenario->abajo ? "vvvvv" : "-----";

    char J[4] = {' ', ' ', ' ', '\0'};
    if (partida->n_jugadores == 1) {
        J[1] = partida->jugadores[0].C;
    } else {
        J[0] = partida->jugadores[0].escenario == escenario ? partida->jugadores[0].C : ' ';
        J[2] = partida->jugadores[1].escenario == escenario ? partida->jugadores[1].C : ' ';
    }
    
    // Mostrar el nombre del escenario centrado
    printf("    %s\n", escenario->nombre);

    // Mostrar el escenario como una caja
    printf("    +----%s----+\n", W);
    printf("    |             |\n");
    printf("    |             |\n");
    printf("    %s             %s\n", A, D);
    printf("    %s     %s     %s\n", A, J, D);
    printf("    %s             %s\n", A, D);
    printf("    |             |\n");
    printf("    |             |\n");
    printf("    +----%s----+\n", S);

    printf("\n");
    
    // Mostrar descripción
    typewriter(escenario->descripcion);
    puts("\nPresione enter para continuar...");
    printf("\n");
    
    // Mostrar objetos disponibles
    if (escenario->cantidadObjetos > 0) {
        printf("\n Objetos disponibles:\n");
        for (int i = 0; i < escenario->cantidadObjetos; i++) {
            printf("\t- %s (peso: %d, valor: %d)\n",
                   escenario->objetos[i].nombre,
                   escenario->objetos[i].peso,
                   escenario->objetos[i].valor);
        }
    }
    limpiar();
}

/* La funcion menuInicio muestra el menu principal del juego, donde
se ingresa la cantidad de jugadores, y que el jugador ingrese ua letra 
mayuscula (se muestra un error si la letra es minuscula), y el programa
guarda los datos iniciales (que no tiene ningun objeto, el nombre del jugador, etc.)
*/
Partida* menuInicio(Escenario* inicio) {
    Partida* partida = (Partida*)malloc(sizeof(Partida));

    system("cls");

    printf("\n\n%s\n\n\n", TITULO);
    printf("                 Cargando escenarios desde CSV...\n");
    sleep(500);

    system("cls");

    printf("\n\n%s\n\n\n", TITULO);
    printf("    Ingrese modo de juego (1-1 jugador, 2-2 jugadores): ");
    scanf("%d", &partida->n_jugadores);

    for (int i = 0; i < partida->n_jugadores; i++) {
        printf("   Ingrese el personaje del jugador %d (Una letra mayuscula): ", i + 1);
        do {
            scanf(" %c", &partida->jugadores[i].C);
            if (partida->jugadores[i].C < 'A' || partida->jugadores[i].C > 'Z') {
                printf("Caracter invalido, ingrese una letra mayuscula: ");
            }
        } while (partida->jugadores[i].C < 'A' || partida->jugadores[i].C > 'Z');

        partida->jugadores[i].n_objetos = 0;
        partida->jugadores[i].escenario = inicio;
        partida->jugadores[i].tiempo = TIEMPO_INICIAL;
    }
    return partida;
}

int tomarObjeto(Jugador *jugador, int aux) {
    if (!jugador || aux >= jugador->escenario->cantidadObjetos) return 0;
    if (jugador->n_objetos >= MAX_OBJETOS) return 0;

    jugador->inventario[jugador->n_objetos++] = jugador->escenario->objetos[aux];

    for (int i = aux; i < jugador->escenario->cantidadObjetos - 1; i++) {
        jugador->escenario->objetos[i] = jugador->escenario->objetos[i + 1];
    }
    jugador->escenario->cantidadObjetos--;
    jugador->tiempo--;
    return 1;
}

// Funcion para verificar si el usuario llego al escenario final
void verificar(Jugador *j, Escenario *destino, int pesoTotal) {
    j->escenario = destino;
    j->tiempo -= ((float)pesoTotal + 1) / 10;

    printf("[DEBUG] Escenario actual: ID %d | esFinal = %d\n", j->escenario->id, j->escenario->esFinal);
    limpiar();
    if (j->escenario->esFinal) {
        printf("\n ¡Jugador %c ha llegado al escenario final! \n", j->C);
        printf("Presione enter para continuar...\n");
        limpiar();
    }

}

/* La funcion mostrarTurno muestra el turno actual de cada jugador, muestra
los objetos que tiene cada jugador, muestra el tiempo que le quedan a cada jugador
y la descripcion del escenario donde se encuentra. Se printea las opciones
validas para el usuario ingrese, como en que direccion quiere moverse o los objetos que
tiene disponibles para usar.
*/
void mostrarTurno(Partida* partida) {
    for(int i = 0; i < partida->n_jugadores; i++) {
        system("cls");

        Jugador *jugador = &partida->jugadores[i];

        if (partida->n_jugadores > 1) {
            printf("\n\n\n\n\n\n%*s %c\n\n\n\n\n\n", 25, "TURNO DEL JUGADOR", jugador->C);

            sleep(500);
            printf("    Presione Enter para continuar...");
            limpiar();
        }

        system("cls");

        mostrarEscenario(jugador->escenario, partida);

        printf("\n\nJUGADOR %c\n\n", jugador->C);

        printf("Tiempo restante: %.2f\n\n", jugador->tiempo);

        int pesoTotal = 0;
        int valorTotal = 0;
        if(jugador->n_objetos == 0) {
            printf("No tienes objetos en tu inventario.\n");
        } else {
            printf("Objetos en tu inventario:\n");
            for(int j = 0; j < jugador->n_objetos; j++) {
                printf("- %s (peso: %d, valor: %d)\n",
                       jugador->inventario[j].nombre,
                       jugador->inventario[j].peso,
                       jugador->inventario[j].valor);
                pesoTotal += jugador->inventario[j].peso;
                valorTotal += jugador->inventario[j].valor;
            }
            printf("Peso total: %d\n", pesoTotal);
            printf("Valor total: %d\n", valorTotal);
        }

        printf("\nOPCIONES DISPONIBLES:\n");

        printf("\nTomar objeto\n");
        if(jugador->escenario->cantidadObjetos > 0) {
            for(int i = 0; i < jugador->escenario->cantidadObjetos && i < 2; i++) {
                printf("\t%s (%d)\n", jugador->escenario->objetos[i].nombre, i + 1);
            }
        } else {
            printf("\tNo hay objetos disponibles en este escenario.\n");
        }

        printf("\nMoverse\n");

        if(jugador->escenario->arriba) printf("\tArriba (W)\n");
        if(jugador->escenario->abajo) printf("\tAbajo (S)\n");
        if(jugador->escenario->izquierda) printf("\tIzquierda (A)\n");
        if(jugador->escenario->derecha) printf("\tDerecha (D)\n");

        printf("\tSalir (Q)\n");

        char opcion;
        int flag = 0;
        do {
            printf("\nIngrese una opcion: ");
            scanf(" %c", &opcion);
            opcion = tolower(opcion);

            if (opcion == '1' || opcion == '2') {
                int aux = opcion - '1';
                if (tomarObjeto(jugador, aux)) {
                    printf("Objeto tomado exitosamente.\n");
                    flag = 1;
                } else {
                    printf("No se pudo tomar el objeto.\n");
                }
            } else if (opcion == 'w' && jugador->escenario->arriba) {
                verificar(jugador, jugador->escenario->arriba, pesoTotal);
                flag = 1;
            } else if (opcion == 's' && jugador->escenario->abajo) {
                verificar(jugador, jugador->escenario->abajo, pesoTotal);
                flag = 1;
            } else if (opcion == 'a' && jugador->escenario->izquierda) {
                verificar(jugador, jugador->escenario->izquierda, pesoTotal);
                flag = 1;
            } else if (opcion == 'd' && jugador->escenario->derecha) {
                verificar(jugador, jugador->escenario->derecha, pesoTotal);
                flag = 1;
            } else if (opcion == 'q') {
                printf("\nSaliendo del juego...\n");
                exit(0);
            } else {
                printf("Ingrese una opcion valida.\n");
            }
        } while (!flag);   
    }
}

/* Esta funcion inicializa una nueva partida y devuelve el escenario inicial
Se considera el caso donde el escenario no cargo bien
*/

Partida *nuevaPartida() {
    Escenario* inicio = cargarEscenarios("data/graphquest.csv");
    if (!inicio) {
        fprintf(stderr, "Error al cargar escenarios desde CSV\n");
        return NULL;
    }
    return menuInicio(inicio);
}

/* Funcion creada mas que nada para mostrar el escenario final, ingresa al jugador,
el valor total de los items guardados anteriormente, y si el jugador gano o perdio
la partida.
*/
Resultado* agregarResultado(Resultado* lista, char jugador, int total, int gano, float tiempo){
    Resultado* new = (Resultado*) malloc(sizeof(Resultado));
    new->jugador = jugador;
    new->total = total;
    new->gano = gano;
    new->tiempo = tiempo;
    new->siguiente = NULL;

    if (!lista) return new;

    Resultado *aux = lista;
    while (aux->siguiente) {
        aux = aux->siguiente;
    }
    aux->siguiente = new;
    return lista;
}

// Para liberar la memoria utilizada en la lista de resultados
void liberarLista(Resultado *lista){
    while (lista){
        Resultado *aux = lista;
        lista = lista->siguiente;
        free(aux);
    }
}

// Función Principal
int main() {
    // Se crea una nueva partida
    Partida *partida = (Partida *) nuevaPartida();
    if (!partida) return 1;
    // Se inicializa una lista vacía para guardar los resultados finales
    Resultado *lista_resultados = NULL;

    int flag = 0;
    do {
        mostrarTurno(partida);
        // Se asume que el juego terminara a menos que cumpla la siguiente condicion
        flag = 0;
        for(int i = 0; i < partida->n_jugadores; i++) {
            if(partida->jugadores[i].escenario->esFinal == 1) {
                flag = 1; // Si algun jugador no ha llegado al escenario final, continua.
            }
        }

        int sumaTiempo = 0;
        for(int i = 0; i < partida->n_jugadores; i++) {
            sumaTiempo += partida->jugadores[i].tiempo;
        }
        // En caso de que el tiempo se agote, flag cambiara para finalizar el bucle
        flag = flag || (sumaTiempo <= 0);
        partida->turno++;
            
    } while (flag == 0);

    // Resultados finales
    for(int i = 0; i < partida->n_jugadores ; i++){
        Jugador *jgdr = &partida->jugadores[i];
        int gano = (jgdr->escenario->esFinal && jgdr->tiempo > 0) ? 1 : 0;

        int valor = 0;
        for (int j = 0 ; j < jgdr->n_objetos ; j++) {
            valor += jgdr->inventario[j].valor;
        }
        lista_resultados = agregarResultado(lista_resultados, jgdr->C, valor, gano, jgdr->tiempo);
    }

    puts ("\n===== RESULTADOS FINALES =====");
    Resultado *aux = lista_resultados;
    while (aux) {
        printf ("Jugador %c: %d puntos\n", aux->jugador, aux->total);
        printf ("Tiempo total : %.2f\n", aux->tiempo);
        printf ("Resultado final : %s\n\n\n", aux->gano ? "GANO !!" : "PERDIO :(");
        aux = aux->siguiente;
    }
    liberarLista(lista_resultados);
    free(partida);

    return 0;
}