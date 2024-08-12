<div align="right">
<img width="32px" src="img/algo2.svg">
</div>

# TDA HASH

## Repositorio de Sixto Gaspar Nahuel Franco - 108352 - sgfranco@fi.uba.ar

- Para compilar:

```bash
make pruebas_alumno
```

- Para ejecutar:

```bash
make ejecutar_pruebas
```

- Para ejecutar con valgrind:
```bash
make valgrind-alumno
```
---
##  Funcionamiento

El objetivo de este trabajo es implementar un diccionario con una tabla hash cerrada (direccionamiento abierto). Se realizan operaciones básicas de la misma, estas son inserción, eliminación, obtención y verificación sobre la existencia de las claves.

Una tabla hash es un TDA que permite guardar pares clave-valor y también realizar operaciones de búsqueda, eliminación e inserción de manera eficiente.

El programa posee dos estructuras principales:

```c
typedef struct tabla {
	char *clave;
	void *valor;
	int estado;
} tabla_t;
```
Este struct representa una entrada en la tabla hash.
```c
char *clave: es la clave del par clave-valor
void *valor: es el valor asociado a la clave
int estado: indica el estado de la entrada, que son 3 posibilidades: VACIO, OCUPADO o ELIMINADO
```

```c
struct hash {
	tabla_t *tabla;
	size_t tamaño_tabla;
	size_t cantidad;
};
```
Esta estructura representa la tabla hash

```c
tabla_t *tabla: Es un vector de estructuras que hacen referencia a tabla_t
size_t tamaño_tabla: Representa el tamaño del vector
size_t cantidad: La cantidad de claves almacenadas en la tabla
```

Breve descripción sobre las funciones implementadas:

```c
hash_t *hash_crear(size_t capacidad);
```
En esta función me encargo de inicializar la tabla del hash, esta posee una capacidad mínima de 3 (puede tener más capacidad, eso lo decide el usuario).

Diagrama de flujo de la funcion hash_crear:

![](img/diagrama_flujo_hash_crear.svg)

Diagrama de memoria de la funcion hash_crear:

![](img/diagrama_memoria_hash.png)


```c
hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,void **anterior);
```
En esta función me encargo de insertar una nueva clave o reemplazarla en caso de que exista. Se da el caso que si la tabla supera cierto umbral se rehashea la misma y se le asigna una nueva capacidad.

```c
void *hash_quitar(hash_t *hash, const char *clave);
```
En esta función me encargo de eliminar una clave, al eliminarla esa posición se marca como ELIMINADO.


```c
void *hash_obtener(hash_t *hash, const char *clave);
```
Si se encuentra la clave la devuelve, en caso de no encontrarla devuelve NULL.

```c
size_t hash_cantidad(hash_t *hash);
```
En esta función me encargo de devolver la cantidad de elementos que hay en el hash ocupando la tabla.


```c
void hash_destruir_todo(hash_t *hash, void (*destructor)(void *));
```
Con esta función de destrucción me encargo de liberar toda la memoria utilizada por la tabla de hash, le paso una función destructora para eliminar absolutamente todo.

```c
size_t hash_con_cada_clave(hash_t *hash, bool (*f)(const char *clave, void *valor, void *aux),void *aux);
```

Esta función hace uso de un iterador interno, tambien hace uso de una función para que en cada iteración, si encuentra un elemento, devuelva true y siga iterando.

### Aclaraciones

El problema principal del TP fue realizar el tema de la complejidad y velocidad, que lo pude arreglar usando una función de hash distinta que aumentó la velocidad del mismo y también retocando las funciones insertar y quitar, ya que aparentemente tenía muchas colisiones y esta misma me daba los problemas.

Para elaborar este trabajo usé una función de hash encontrada en internet, la adapté para que no tuviera errores de compilación. Esta función se llama djb2, funciona bien con strings.

Este es el link donde se implementa el hash:

[Funcion hash djb2](https://stackoverflow.com/questions/7700400/whats-a-good-hash-function-for-english-words)


## Respuestas a las preguntas teóricas


### ¿Qué es un diccionario?

- Es una estructura de datos que almacena pares clave-valor. Cada una de estas claves está asociada a un valor. Los diccionarios cumplen algunas funciones, como por ejemplo: acceso, inserción y eliminación de elementos según la clave.

#### Formas de implementar un diccionario

##### Tabla de hash:

- En este caso, la tabla transforma la clave en un índice. Si se produce una colisión, si el hash es cerrado, se hace un rehashing y se agranda la tabla.

##### Ventajas:

- Eficiente respecto al tiempo.

- En promedio, las operaciones son O(1).

#### Árbol binario:

- Las claves se almacenan en nodos, de modo que se ordenan las claves de menor valor a la izquierda y las de mayor valor a la derecha.

##### Ventajas:

- No requiere una función hash.

- El promedio de las operaciones es O(log(n)).

#### Lista enlazada

- Una lista enlazada es un TDA, donde cada nodo contiene una clave, un valor y un puntero al siguiente nodo. Para buscar, insertar o eliminar un elemento, se recorre la lista hasta encontrar la clave correspondiente.


##### Ventajas:

- No requiere una función hash.

- Más fácil de implementar.

### ¿Que es una funcion hash?

- Una función de hash es una función que toma una clave y la convierte en un valor entero, esta misma clave se usa como índice en una tabla de hash.

![](img/funcion_hash_graf.jpeg)


#### Características que debe poseer una tabla hash:


- Distribuir las claves de manera eficiente, de modo que la cantidad de colisiones sea baja y mantenga un rendimiento eficiente.

- Debe ser eficiente.

- La misma clave siempre tiene que producir el mismo hash.


### ¿Qué es una tabla de hash?

- Una tabla hash es un tipo de dato que asocia claves con valores mediante una función de hash, a cada clave le asigna una posición específica de una tabla.

![](img/tabla_hash.jpeg)

#### Métodos de resolución de colisiones:

##### Probing lineal:

- En este caso, me encargo de buscar el siguiente espacio vacío de la tabla para poder insertar.

![](img/prob_lineal.svg)


##### Probing cuadrático:

- Es similar al probing lineal, pero la diferencia es que en este caso hace saltos cuadráticos en vez de lineales.
![](img/prob_cuad.jpg)

##### Hash doble:

- Este caso ya es distinto a los dos anteriores, si encuentra una colisión, usa una segunda función de hash para calcular los saltos entre posiciones.