#include "hash.h"
#include <string.h>
#include <stdlib.h>

#define FACTOR_CARGA_MAXIMA 0.7
#define CAPACIDAD_MINIMA_HASH 3

#define VACIO 0
#define OCUPADO 1
#define ELIMINADO 2

typedef struct tabla {
	char *clave;
	void *valor;
	int estado;
} tabla_t;

struct hash {
	tabla_t *tabla;
	size_t tamanio_tabla;
	size_t cantidad;
};

size_t funcion_hash(const char *clave)
{
	size_t hash = 5381;
	int c;

	while ((c = *clave++))
		hash = ((hash << 5) + hash) + (size_t)c;

	return hash;
}

void liberar_elemento(tabla_t *elemento)
{
	if (elemento->clave)
		free(elemento->clave);

	elemento->valor = NULL;
	elemento->estado = VACIO;
}

void insertar_copia_clave(hash_t *hash, const char *clave, void *elemento,
			  size_t iterador)
{
	char *copia_clave = calloc(1,strlen(clave) + 1);

	if (!copia_clave)
		return;

	strcpy(copia_clave, clave);
	hash->tabla[iterador].clave = copia_clave;
	hash->tabla[iterador].valor = elemento;
	hash->tabla[iterador].estado = OCUPADO;
	hash->cantidad++;
}

hash_t *hash_crear(size_t capacidad)
{
	if (capacidad < CAPACIDAD_MINIMA_HASH)
		capacidad = CAPACIDAD_MINIMA_HASH;

	hash_t *hash = calloc(1,sizeof(hash_t));

	if (!hash)
		return NULL;

	hash->tabla = calloc(capacidad, sizeof(tabla_t));

	if (!hash->tabla) {
		free(hash);
		return NULL;
	}

	hash->cantidad = 0;
	hash->tamanio_tabla = capacidad;

	return hash;
}

void rehashear_la_tabla(hash_t *hash)
{
	if (!hash)
		return;

	size_t capacidad_nueva = (hash->tamanio_tabla) * 2;
	hash_t *nuevo_hash = hash_crear(capacidad_nueva);

	if (!nuevo_hash)
		return;

	for (size_t i = 0; i < hash->tamanio_tabla; i++) {
		if (hash->tabla[i].estado == OCUPADO) {
			hash_insertar(nuevo_hash, hash->tabla[i].clave,hash->tabla[i].valor, NULL);
			liberar_elemento(&hash->tabla[i]);
		}
	}

	free(hash->tabla);

	hash->tabla = nuevo_hash->tabla;
	hash->tamanio_tabla = nuevo_hash->tamanio_tabla;

	free(nuevo_hash);
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento,
		      void **anterior)
{
	if (!hash || !clave)
		return NULL;

	float factor_de_carga =
		(float)hash->cantidad / (float)hash->tamanio_tabla;

	if (factor_de_carga >= FACTOR_CARGA_MAXIMA)
		rehashear_la_tabla(hash);

	size_t i = funcion_hash(clave) % hash->tamanio_tabla;

	while (hash->tabla[i].estado == OCUPADO &&
	       strcmp(hash->tabla[i].clave, clave) != 0) {
		i = (i + 1) % hash->tamanio_tabla;
	}

	if (hash->tabla[i].estado == OCUPADO) {
		if (anterior)
			*anterior = hash->tabla[i].valor;
		hash->tabla[i].valor = elemento;
	} else {
		if (anterior)
			*anterior = NULL;
		insertar_copia_clave(hash, clave, elemento, i); 
	}

	return hash;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t i = funcion_hash(clave) % hash->tamanio_tabla;
	void *elemento = NULL;
	bool encontrado = false;

	while (!encontrado && hash->tabla[i].estado != VACIO) {
		if (hash->tabla[i].estado == OCUPADO &&
		    strcmp(clave, hash->tabla[i].clave) == 0) {
			elemento = hash->tabla[i].valor;
			free(hash->tabla[i].clave);
			hash->tabla[i].estado = ELIMINADO;
			hash->cantidad--;
			encontrado = true;
		}
		i = (i + 1) % hash->tamanio_tabla;
	}

	return elemento;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t i = funcion_hash(clave) % hash->tamanio_tabla;
	void *elemento = NULL;
	bool encontrado = false;

	while (!encontrado && hash->tabla[i].estado != VACIO) {
		if (hash->tabla[i].estado == OCUPADO &&
		    strcmp(hash->tabla[i].clave, clave) == 0) {
			elemento = hash->tabla[i].valor;
			encontrado = true;
		}
		i = (i + 1) % hash->tamanio_tabla;
	}

	return elemento;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	bool encontrado = false;

	if (!hash || !clave)
		return encontrado;

	size_t i = funcion_hash(clave) % hash->tamanio_tabla;

	while (!encontrado && hash->tabla[i].estado != VACIO) {
		if (hash->tabla[i].estado == OCUPADO &&
		    strcmp(hash->tabla[i].clave, clave) == 0) {
			encontrado = true;
		}
		i = (i + 1) % hash->tamanio_tabla;
	}

	return encontrado;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;

	return hash->cantidad;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;

	for (size_t i = 0; i < hash->tamanio_tabla; i++) {
		if (hash->tabla[i].estado == OCUPADO) {
			free(hash->tabla[i].clave);
			if (destructor)
				destructor(hash->tabla[i].valor);
		}
	}

	free(hash->tabla);
	free(hash);
}

size_t hash_con_cada_clave(hash_t *hash,
			   bool (*f)(const char *clave, void *valor, void *aux),
			   void *aux)
{
	if (!hash || !f)
		return 0;

	size_t claves_iteradas = 0;
	size_t i = 0;
	bool seguir = true;

	while (i < hash->tamanio_tabla && seguir) {
		if (hash->tabla[i].estado == OCUPADO) {
			claves_iteradas++;
			if (!f(hash->tabla[i].clave, hash->tabla[i].valor, aux))
				seguir = false;
		}
		i++;
	}

	return claves_iteradas;
}