#include <iostream>
#include <fstream>

#include "BoundedQueue.hpp"
#include "Semaphore_V4.hpp"
#include <ctime>
#include <thread>



using namespace std;

//TIPOS DE DATOS Y VARIABLES COMPARTIDAS
const int NUM_Cl = 10;
const int NUM_Pub = 3;
const int MAX_IMG = 10;
const int MAX_INTENTOS = 2;
const int TAM_COLA_VG = 10;
const int TAM_COLA_VP = 15;
const int PRECIO_BASE = 100;

//declaramos los semaforos
Semaphore testigoColaVG(1); //TODO: inicializar todos los testigos y semaforos bien
Semaphore b1_colaVG = 0;
Semaphore b2_colaVG = 0;
Semaphore b3_colaVG = 0; //para la parte del await c.length()>0 de la linea 226
int d1_colaVG = 0;
int d2_colaVG = 0;
int d3_colaVG = 0; //para la parte del await c.length()>0 de la linea 226

Semaphore testigocolaVP = 1;
Semaphore b1_colaVP = 0;
Semaphore b2_colaVP = 0;
Semaphore b3_colaVP = 0; //para la parte del await c.length()>0 de la linea 226
int d1_colaVP = 0;
int d2_colaVP = 0;
int d3_colaVP = 0; //para la parte del await c.length()>0 de la linea 226

Semaphore testigoFinalizador = 1;
Semaphore b1_finalizador = 0;
int d1_finalizador = 0;

// HAY QUE TERMINAR EL PASO DE TESTIGO DE LAS RESPUESTAS
Semaphore testigoRespuesta = 1;
Semaphore *b[NUM_Cl]; //TODO: inicializa solo la primera a cero
//declarar un array de punteros a semaforos e inicializar con un for
int d[NUM_Cl];



//definicion del tipo de dato Peticion
enum tpPeticion {
    Publicar,
    Finalizar
};

struct Peticion{
    int idC;
    tpPeticion tpPet;
    string imagen;
    int tServ;
};

//definicion del tipo de dato respuesta
struct Respuesta{
    bool estado;
    double precio;
};

void PasarTestigoColaVG(BoundedQueue<Peticion> &colaVG){
    if(colaVG.length() < TAM_COLA_VG and d1_colaVG >0){
        d1_colaVG--;
        signal(b1_colaVG);
    } else if(colaVG.length() < TAM_COLA_VG and d2_colaVG > 0){
        d2_colaVG--;
        signal(b2_colaVG);
    }else if( colaVG.length() > 0 and d3_colaVG > 0){
        d3_colaVG--;
        signal(b3_colaVG);
    }else{
        signal(testigoColaVG);
    }

}

void PasarTestigoColaVP(BoundedQueue<Peticion> &colaVP){
    if(colaVP.length() < TAM_COLA_VP and d1_colaVP >0){
        d1_colaVP--;
        signal(b1_colaVP);
    } else if(colaVP.length() < TAM_COLA_VP and d2_colaVP > 0){
        d2_colaVP--;
        signal(b2_colaVP);
    }else if(colaVP.length() > 0 and d3_colaVP > 0){
        d3_colaVP--;
        signal(b3_colaVP);
    }else{
        signal(testigoColaVP);
    }
}

void PasarTestigoFinalizador(int &numClientFin){
    if(numClientFin < NUM_Cl and d1_finalizador > 0){
        d1_finalizador--;
        signal(b1_finalizador);
    } else{
        signal(testigoFinalizador);
    }
}

void PasarTestigoRespuesta(){
    bool testigoPasado = false;
    int i = 0;
    while(not testigoPasado and i < NUM_Cl){
        if(d[i]>0 and hayRespuesta[i]){
            testigoPasado = true;
            d[i]--;
            
            b[i] -> signal(); //asi porque es un puntero
            //TODO: cambiar los signay y wait de los array
        } 
        i++;
    }
    if(not testigoPasado){
        signal(testigoRespuesta);
    }

}

string escogerImagen(){
    int nImagen = 1 + rand() % 20 ;
    string pref = "nature_";
    if (nImagen < 10){
        return pref + "0" + to_string(nImagen) + ".jpg";
    }
    else{
        return pref + to_string(nImagen) + ".jpg";
    }
}

void Cliente(const int id, BoundedQueue<Peticion> &colaVG, BoundedQueue<Respuesta> &colaVP, 
             bool hayRespuesta[NUM_Cl], Respuesta tipoRespuesta[NUM_Cl], int &numClientFin)
            {
                Peticion pet;
                Respuesta resp;
                int numPetAceptadas = 0;
                int numPetRechazadas = 0;
                double precioTotal = 0.0;

                for(int i; i < MAX_IMG; i++){
                    int numIntentos = 0;
                    //configuramos las peticiones
                    resp.estado = false;
                    pet.idC = id;
                    pet.tpPet = Publicar;
                    pet.imagen = "./images" + escogerImagen();
                    pet.tServ = rand();

                    //determinar la cola de publicacion
                    int numCola = rand() % 2;
                    while(not resp.estado and numIntentos < MAX_INTENTOS){
                        if( numCola == 1 ){
                            /*
                            <await colaVG.length() < TAM_COLA_VG
                                colaVG.enqueue(pet);
                            > 
                            */
                            wait(testigoColaVG);
                            if(not colaVG.length() < TAM_COLA_VG){
                                d1_colaVG++;
                                signal(testigoColaVG);
                                wait(b1_colaVG);
                            }
                            colaVG.enqueue(pet);
                            pasarTestigoColaVG(ref(colaVG));
                            


                        } else{
                            /*<await colaVP.length() < TAM_COL_VP
                                colaVP.enqueue(resp);
                            >*/
                            wait(testigocolaVP);
                            if(colaVP.length() < TAM_COLA_VP){
                                d1_colaVP++;
                                signal(testigoColaVP);
                                wait(b1_colaVP);
                            }
                            colaVP.enqueue(pet);
                            pasarTestigoColaVP();
                        }
                        //espera la respuesta del publicador
                        /*<await hayRespuesta[id]
                            resp = tipoRespuesta[id];
                        >*/
                        wait(testigoRespuesta);
                        if(not hayRespuesta[id]){
                            d[id]++;
                            signal(testigoRespuesta);
                            wait(b[id]);
                        }
                        resp = tipoRespuesta[id];
                        pasarTestigoRespuesta();

                        /analizar el resultado de la respuesta
                        if(resp.estado){
                            numPetAceptadas++;
                            precioTotal += resp.precio;
                        } else{
                            numIntentos++;
                            if(numIntentos == MAX_INTENTOS){
                                numPetRechazadas++;
                            }
                        }
                        cout << "Peticiones aceptadas: " << numPetAceptadas << endl;
                        cout << "Peticiones rechazadas: " << numPetRechazadas << endl; 
                        cout << "Precio total: " << precioTotal << endl;

                        /*<numClientFin = numClientFin + 1>*/
                        wait(testigoFinalizador);
                        numClientFin = numClientFin + 1;
                        pasarTestigoFinalizador(ref(numClientFin));

                        
                    }
                }
            }
void Publicador(const int id, BoundedQueue<Peticion> &c, const bool esVG, bool hayRespuesta[NUM_Cl], Respuesta tipoRespuesta[NUM_Cl]){
    int probabilidad = rand() % 100;
    bool fin = false;
    double precio = 0.0;

    Peticion pet;
    Respuesta resp;

    int numPetAtendidas = 0;
    int numPetAceptadas = 0;
    double precioTotal = 0.0;

    while(pet.tpPet != Finalizar){
        /*<await c.length() > 0
            pet = c.first();
            c.dequeue();
        >*/
        if(esVG){
            wait(testigoColaVG);
            if(not c.length()>0){
                d3_colaVG++;
                signal(testigoColaVG);
                wait(b3_colaVG);
            }
            pet = c.first();
            c.dequeue();
            PasarTestigoColaVG(ref(c));
        }else {
            wait(testigocolaVP);
            if(not c.length()>0){
                d3_colaVP++;
                signal(testigocolaVP);
                wait(b3_colaVP);
            }
            pet = c.first();
            c.dequeue();
            PasarTestigoColaVP(ref(c));
        }

        if(pet.tpPet == Publicar){
            if (probabilidad < 5){
                fin = false;
            } else{
                fin = true;
                numPetAceptadas++;
            }
            resp.estado = Aceptada;

            if(fin){
                precio = PRECIO_BASE * (1/(c.length()+1));
                resp.precio = precio;
            }

            int indice = pet.idC;
            /*< tipoRespuesta[indice] = resp;
              hayRespuesta[indice] = true;
            >*/
            wait(testigoRespuesta);
            tipoRespuesta[indice] = resp;
            hayRespuesta[indice] = true;
            pasarTestigoRespuesta();

            if(fin){
                mostrarImagen();
            }

            numPetAtendidas++;
        } 
        cout << "Peticiones atendidas: " << numPetAtendidas << endl;
        cout << "Peticiones aceptadas: " << numPetAceptadas << endl;
        cout << "Peticiones rechazadas: " << numPetAtendidas - numPetAceptadas << endl;
    }
    
}

void Finalizador(BoundedQueue<Peticion> &colaVG, BoundedQueue<Peticion> &colaVP, int &numClientFin){
   Peticion pet;
   pet.tpPet = Finalizar;

   /*<await numClientFin < NUM_Cl>*/
   wait(testigoFinalizador);
   if(not numClientFin < NUM_Cl){
        d1_finalizador++;
        signal(testigoFinalizador);
        wait(b1_finalizador);
    }
    pasarTestigoFinalizador();


   /*<await colaVG.length() < TAM_COLA_VG
        colaVG.enqueue(pet);
    >*/
    wait(testigoColaVG);
    if(not colaVG.length() < TAM_COLA_VG){
        d2_colaVG++;
        signal(testigoColaVG);
        wait(b2_colaVG);
    }
    colaVG.enqueue(pet);
    pasarTestigoColaVG(ref(colaVG));
    


    /*<await colaVP.length() < TAM_COLA_VP
          colaVP.enqueue(pet);
    >*/
    wait(testigoColaVP);
    if(not colaVP.length() < TAM_COLA_VP){
        d2_colaVP++;
        signal(testigoColaVP);
        wait(b2_colaVP);
    }
    colaVP.enqueue(pet);
    pasarTestigoColaVP(ref(colaVP));

}

void inicializar(){
    for (int i=0; i<NUM_Cl; i++){
        d[i] = 0;
        b[i] = new Semaphore(0);
    }

}

void finalizar(){
    for (int i=0; i<NUM_Cl; i++){
        delete b[i];
    }
}

int main(int argc, char *argv[]) {

    inicializar();

    srand(time(NULL));
    BoundedQueue<Peticion> colaVG(TAM_COLA_VG);
    BoundedQueue<Peticion> colaVP(TAM_COLA_VP);

    bool hayRespuesta[NUM_Cl];
    Respuesta tipoRespuesta[NUM_Cl];

    int numClientFin = 0;

    thread clientes [NUM_Cl];
    for(int i = 0; i < NUM_Cl; i++){
        clientes[i] = thread(&Cliente, i, ref(colaVG), ref(colaVP), hayRespuesta, tipoRespuesta, ref(numClientFin));
    }

    thread publicador [NUM_Pub];
    publicador[0] = thread(&Publicador, 0, ref(colaVG), true, hayRespuesta, tipoRespuesta);
    publicador[1] = thread(&Publicador, 1, ref(colaVP), false, hayRespuesta, tipoRespuesta);
    publicador[2] = thread(&Publicador, 2, ref(colaVP), false, hayRespuesta, tipoRespuesta);

    thread finalizador = thread(&Finalizador, ref(colaVG), ref(colaVP), ref(numClientFin));

    for(int i = 0; i < NUM_Cl; i++){
        clientes[i].join();
    }

    for(int i = 0; i < NUM_Pub; i++){
        publicador[i].join();
    }

    finalizador.join();

    finalizar();

    return 0;   
}

