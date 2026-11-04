//*****************************************************************
// File:   BoundedQueue.hpp
// Author: PSCD-Unizar
// Date:   septiembre 2026
// Coms:   Especificación de una cola FIFO acotada.
//         Una cola será denotada como "< d_1 ... d_n >", de manera que "d_1"
//         hace referencia al primero de la cola (el dato más antiguo) y "d_n"
//         al último. "n>0" implicará que la cola es no vacía. "this" se referirá a la propia cola.
//         Denotaremos mediante "#this" el número máximo de elementos que puede contener.
//*****************************************************************
#ifndef BOUNDED_QUEUE
#define BOUNDED_QUEUE

#include <iostream>
#include <string>
#include <cassert>
#include <vector>

using namespace std;

template <class T>
class BoundedQueue {
public:

    //-----------------------------------------------------
    // Pre:  0 < N
    // Post: this = <> AND #this = N
    // Com:  constructor
    explicit BoundedQueue(int N);

    //-----------------------------------------------------
    // Pre:
    // Post:
    // Com:  destructor
    ~BoundedQueue();

    //-----------------------------------------------------
    // Pre:
    // Post: this = <>
    // Com:  vacía la cola
    void empty();

    //-----------------------------------------------------
    // Pre:  this = <d1 ... dn> AND n < #this
    // Post: this = <d1 ... dn d>
    // Com:  inserta un elemento al final
    void enqueue(const T& d);

    //-----------------------------------------------------
    // Pre:  this = <d1 ... dn> AND 0 < n
    // Post: this = <d2 ... dn>
    // Com:  elimina el primer elemento
    void dequeue();

    //-----------------------------------------------------
    // Pre:  this = <d1 ... dn> AND 0 < n
    // Post: first() = d1
    // Com:  devuelve el primer elemento
    T first() const;

    //-----------------------------------------------------
    // Pre:  this = <d1 ... dn>
    // Post: length() = n
    // Com:  número de elementos almacenados
    int length() const;

    //-----------------------------------------------------
    // Pre:
    // Post: size() = #this
    // Com:  capacidad máxima de la cola
    int size() const;

    //-----------------------------------------------------
    // Pre:
    // Post:
    // Com:  muestra la cola por la salida estándar,
    //       separando los elementos mediante comas
    void print() const;

    //-----------------------------------------------------
    // Pre:  #copy = #this
    // Post: copy = this
    // Com:  clonación de la cola
    void clone(BoundedQueue<T>& copy) const;

    //-----------------------------------------------------
    // Pre:
    // Post:
    // Com:  constructor de copia deshabilitado
    BoundedQueue(const BoundedQueue&) = delete;

    //-----------------------------------------------------
    // Pre:
    // Post:
    // Com:  asignación deshabilitada
    BoundedQueue& operator=(const BoundedQueue&) = delete;

private:
    int N;             // capacidad máxima
    int nEl;           // número de elementos

    vector<T> data;    // los datos

    int pri;           // posición del primer elemento
    int sig;           // primera posición libre
};

// Para poder trabajar con tipos genéricos,
// declaración e implementación deben estar juntas
// Pegamos debajo la implementación
#include "BoundedQueue.cpp"

#endif