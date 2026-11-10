//*****************************************************************
// File:   BoundedQueue.cpp
// Author: PSCD-Unizar
// Date:   Octubre 2026
// Coms:   Implemetación de la clase
//*****************************************************************

#include "BoundedQueue.hpp"

//-----------------------------------------------------
template <class T>
BoundedQueue<T>::BoundedQueue(int N)
: N(N), nEl(0), data(N)
{
    assert(N > 0);

    empty();
}

//-----------------------------------------------------
template <class T>
BoundedQueue<T>::~BoundedQueue() {

}

//-----------------------------------------------------
template <class T>
void BoundedQueue<T>::empty() {

    pri = 0;
    sig = 0;
    nEl = 0;
}

//-----------------------------------------------------
template <class T>
void BoundedQueue<T>::enqueue(const T& d) {

    assert(nEl < N);

    data[sig] = d;
    sig = (sig + 1) % N;
    nEl++;
}

//-----------------------------------------------------
template <class T>
void BoundedQueue<T>::dequeue() {

    assert(nEl > 0);

    pri = (pri + 1) % N;
    nEl--;
}

//-----------------------------------------------------
template <class T>
T BoundedQueue<T>::first() const {

    assert(nEl > 0);

    return data[pri];
}

//-----------------------------------------------------
template <class T>
int BoundedQueue<T>::length() const {

    return nEl;
}

//-----------------------------------------------------
template <class T>
int BoundedQueue<T>::size() const {

    return N;
}

//-----------------------------------------------------
template <class T>
void BoundedQueue<T>::print() const {

    for (int i = 0; i < nEl; i++) {
        if (i > 0) {
            cout << ",";
        }
        cout << data[(pri + i) % N];
    }
}

//-----------------------------------------------------
template <class T>
void BoundedQueue<T>::clone(BoundedQueue<T>& copy) const {

    assert(copy.N == N);

    copy.nEl = nEl;
    copy.pri = pri;
    copy.sig = sig;
    copy.data = data;
}
//-----------------------------------------------------
