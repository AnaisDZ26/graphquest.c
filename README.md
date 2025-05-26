# GraphQuest

### Descripción
GraphQuest es un juego programado en C para uno o dos jugadores locales, basado en un laberinto estructurado como un grafo. En cada turno, los jugadores pueden moverse en distintas direcciones, recolectar objetos y administrar su inventario con estrategia. Además, deben gestionar su tiempo para llegar al escenario final antes de agotarlo. La experiencia combina exploración, toma de decisiones y optimización de rutas, ofreciendo un desafío dinámico.

---

## Cómo compilar y ejecutar el sistema
Este sistema ha sido desarrollado en lenguaje C, como alternativa puede ejecutarse en **Visual Studio Code**, junto con una extension de 
 C/C++ como **C/C++ Extension Pack de Microsoft**. Para comenzar a utilizar este sistema de gestion debe seguir los siguientes pasos:

- Tener instalado [Visual Studio Code]( https://code.visualstudio.com/).- Instalar la extension antes recomendada (**C/C++**).
- Tener instalado un **complidador de C** (como **gcc**). Si el usuario esta utilizando Windows, se recomienda instalar [MinGW](https://www.mingw-w64.org/).

 ## Para compilar y ejecutar el codigo del sistema
- Descarga y **descomprime** el archivo ***.zip*** en una carpeta de tu elección.
- Abre el proyecto en Visual Studio Code
- Inicia Visual Studio Code.
    - Selecciona **Archivo -> Abrir carpeta...** y elige la carpeta donde descomprimiste el proyecto.
- Compila el código
    - Abre el archivo principal (**main.c**).
    - Abre la terminal integrada (**Terminal -> Nueva terminal**).
    - En la terminal, compila el programa con el siguiente comando :
    ```
    gcc main.c -Wno-unused-result -o main
    ```
- Ejecuta el programa
    Una vez compilado, ejecuta el programa con el siguiente comando :
    ```
    ./main
    ```

## Funcionalidades del programa

### Funcionando correctamente :
- El programa carga un archivo que contiene los escenarios del juego, los objetos, y las caracteristicas de ambos.
- Ingresa correctamente las opciones que escogio el usuario.
- Modo de juego para 1 o 2 jugadores LOCALES.
- Movimiento en el laberinto mediante teclas **W** (arriba), **S** (abajo), **A** (izquierda), **D** (derecha).
- Recolección de objetos considerando su peso y valor.
- Sistema de turnos para un jugador o más.
- Cálculo de puntajes y condiciones de victoria o derrota, estas condiciones se establecen con el tiempo utilizado, que funciona correctamente.
- Visualización de resultados finales por jugador.

### Problemas conocidos : 
- El programa podria llegar a no considerar todas las opciones de error, se concentra mas en un usuario ideal. Aun así aborda la mayoría de los escenarios.
- La interfaz podria no llegar a ser la ideal.

### A mejorar:
- Mejorar la interfaz para que sea más amigable y fácil de usar.
- Agregar más opciones de error para que el programa sea más robusto.

## Ejemplo de uso : 
**Paso 1: Iniciar el juego**
El usuario selecciona el modo de juego ( 1 o 2 jugadores) y la **letra** con la que el jugador sera registrado.
```
Ingrese modo de juego (1-1 jugador, 2-2 jugadores): 1
   Ingrese el personaje del jugador 1 (Una letra mayuscula): A
```
El programa ingresa los datos y muestra el escenario actual con el titulo del escenario y el jugador.

**Paso 2: Moverse en el laberinto**
Luego de la descripcion del escenario, el programa muestra las opciones de movimiento y el jugador puede elegir una de ellas o salir del juego.
```
Entrada principal
    +-------------+
    |             |
    |             |
    |             |
    |      A      |
    |             |
    |             |
    |             |
    +----vvvvv----+

 Una puerta rechinante abre paso a esta
 mansion olvidada por los dioses y los
 conserjes. El aire huele a humedad y
 a misterios sin resolver.
Presione enter para continuar...



JUGADOR A

Tiempo restante: 10.00

No tienes objetos en tu inventario.

OPCIONES DISPONIBLES:

Moverse
        Abajo (S)
        Salir (Q)

Ingrese una opcion: S

```
El programa muestra el nuevo escenario y el jugador puede elegir una de las opciones de movimiento o un objeto dependiendo de la dirección.

**Paso 3: Resolver el laberinto**
El jugador sigue moviendose por el laberinto hasta que encuentra el escenario final o se queda sin tiempo.
```
Sala del tesoro
    +----^^^^^----+
    |             |
    |             |
    <             |
    <      A      |
    <             |
    |             |
    |             |
    +-------------+

 Brilla tanto que casi puedes oler el
 valor monetario. Pero si brilla demasiado...
 seguro hay una trampa.
Presione enter para continuar...


 Objetos disponibles:
        - Corona (peso: 50, valor: 10)
        - Collar (peso: 20, valor: 4)


JUGADOR A

Tiempo restante: 4.20

Objetos en tu inventario:
- Pan (peso: 2, valor: 1)
- Cuchillo (peso: 3, valor: 1)
- Copa dorada (peso: 8, valor: 3)
Peso total: 13
Valor total: 5

OPCIONES DISPONIBLES:

Tomar objeto
        Corona (1)
        Collar (2)

Moverse
        Arriba (W)
        Izquierda (A)
        Salir (Q)

Ingrese una opcion: 2 A
```
El usurio sigue escogiendo objetos y direcciones hasta el final del juego.

**Paso 4: Fin del juego**
Finalmente en caso de encontrar o no el escenario final, el juego termina y se muestra el resultado final.

```
    Pasillo
    +----^^^^^----+
    |             |
    |             |
    |             >
    |      A      >
    |             >
    |             |
    |             |
    +----vvvvv----+

 Estrecho, largo y lleno de telara├▒as.
 Cada paso resuena como si alguien (o
 algo) escuchara.
Presione enter para continuar...


 Objetos disponibles:
        - Llave (peso: 5, valor: 1)


JUGADOR A

Tiempo restante: 1.80

Objetos en tu inventario:
- Pan (peso: 2, valor: 1)
- Cuchillo (peso: 3, valor: 1)
- Copa dorada (peso: 8, valor: 3)
- Collar (peso: 20, valor: 4)
Peso total: 33
Valor total: 9

OPCIONES DISPONIBLES:

Tomar objeto
        Llave (1)

Moverse
        Arriba (W)
        Abajo (S)
        Derecha (D)
        Salir (Q)

Ingrese una opcion: 1 d
Has tomado el objeto Llave (-1 TIEMPO)

===== RESULTADOS FINALES =====
Jugador A: 10 puntos
Tiempo total : -2.60
Resultado final : PERDIO :(
```

## Contribuciones
### Daniel Gajardo
- Desarrollo las funciones principales del programa.
- Diseño la interfaz de usuario.
- Documentacion y comentarios en el codigo.
- Auto-evaluación : 7 / 7.

### Anais Diaz
- Desarrollo las funciones secundarias del programa.
- Documentacion y comentarios en el codigo.
- Diseño y redaccion del README.
- Auto-evaluacion : 7 / 7.