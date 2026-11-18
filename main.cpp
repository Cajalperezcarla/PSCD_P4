#include <iostream>
#include <fstream>

#include "./BoundedQueue.hpp"
#include "./Semaphore_V4.hpp"
#include "./ImageWindow.hpp"
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
Semaphore testigoColaVG(1); 
Semaphore b1_colaVG(0);
Semaphore b2_colaVG(0);
Semaphore b3_colaVG(0); //para la parte del await c.length()>0 de la linea 226
int d1_colaVG = 0;
int d2_colaVG = 0;
int d3_colaVG = 0; //para la parte del await c.length()>0 de la linea 226

Semaphore testigoColaVP(1);
Semaphore b1_colaVP(0);
Semaphore b2_colaVP(0);
Semaphore b3_colaVP(0); //para la parte del await c.length()>0 de la linea 226
int d1_colaVP = 0;
int d2_colaVP = 0;
int d3_colaVP = 0; //para la parte del await c.length()>0 de la linea 226

Semaphore testigoFinalizador(1);
Semaphore b1_finalizador(0);
int d1_finalizador = 0;

// HAY QUE TERMINAR EL PASO DE TESTIGO DE LAS RESPUESTAS
Semaphore testigoRespuesta(1);
Semaphore *b[NUM_Cl]; 
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
        b1_colaVG.signal();
    } else if(colaVG.length() < TAM_COLA_VG and d2_colaVG > 0){
        d2_colaVG--;
        b2_colaVG.signal();
    }else if( colaVG.length() > 0 and d3_colaVG > 0){
        d3_colaVG--;
        b3_colaVG.signal();
    }else{
        testigoColaVG.signal();
    }

}

void PasarTestigoColaVP(BoundedQueue<Peticion> &colaVP){
    if(colaVP.length() < TAM_COLA_VP and d1_colaVP >0){
        d1_colaVP--;
        b1_colaVP.signal();
    } else if(colaVP.length() < TAM_COLA_VP and d2_colaVP > 0){
        d2_colaVP--;
        b2_colaVP.signal();
    }else if(colaVP.length() > 0 and d3_colaVP > 0){
        d3_colaVP--;
        b3_colaVP.signal();
    }else{
        testigoColaVP.signal();
    }
}

void PasarTestigoFinalizador(int &numClientFin){
    if(numClientFin == NUM_Cl and d1_finalizador > 0){
        d1_finalizador--;
        b1_finalizador.signal();
    } else{
        testigoFinalizador.signal();
    }
}

void PasarTestigoRespuesta(bool hayRespuesta[NUM_Cl]){
    bool testigoPasado = false;
    int i = 0;
    while(not testigoPasado and i < NUM_Cl){
        if(d[i]>0 and hayRespuesta[i]){
            testigoPasado = true;
            d[i]--;
            
            b[i] -> signal(); //asi porque es un puntero
            
        } 
        i++;
    }
    if(not testigoPasado){
        testigoRespuesta.signal();
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

void mostrarImagen(ImageWindow &win, const string &rutaImagen, int tServ){

    //muestra la imagen en la ventana correspondiente
    win.show(rutaImagen);

    // Mantiene la imagen visible durante el tiempo de servicio solicitado 
    this_thread::sleep_for(chrono::milliseconds(tServ));
    
}

void Cliente(const int id, BoundedQueue<Peticion> &colaVG, BoundedQueue<Peticion> &colaVP, 
             bool hayRespuesta[NUM_Cl], Respuesta tipoRespuesta[NUM_Cl], int &numClientFin)
            {
                Peticion pet;
                Respuesta resp;
                int numPetAceptadas = 0;
                int numPetRechazadas = 0;
                double precioTotal = 0.0;

                for(int i=0; i < MAX_IMG; i++){
                    int numIntentos = 0;
                    //configuramos las peticiones
                    resp.estado = false;
                    pet.idC = id;
                    pet.tpPet = Publicar;
                    pet.imagen = "./images/" + escogerImagen();
                    pet.tServ = 100 + rand() % 400; //entre 100 y 500ms

                    //determinar la cola de publicacion
                    int numCola = rand() % 2;
                    while(not resp.estado and numIntentos < MAX_INTENTOS){
                        if( numCola == 1 ){
                            /*
                            <await colaVG.length() < TAM_COLA_VG
                                colaVG.enqueue(pet);
                            > 
                            */
                            testigoColaVG.wait();
                            if(not colaVG.length() < TAM_COLA_VG){
                                d1_colaVG++;
                                testigoColaVG.signal();
                                b1_colaVG.wait();
                            }
                            colaVG.enqueue(pet);
                            PasarTestigoColaVG(ref(colaVG));
                            


                        } else{
                            /*<await colaVP.length() < TAM_COL_VP
                                colaVP.enqueue(resp);
                            >*/
                            testigoColaVP.wait();
                            if(not colaVP.length() < TAM_COLA_VP){
                                d1_colaVP++;
                                testigoColaVP.signal();
                                b1_colaVP.wait();
                            }
                            colaVP.enqueue(pet);
                            PasarTestigoColaVP(ref(colaVP));
                        }
                        //espera la respuesta del publicador
                        /*<await hayRespuesta[id]
                            resp = tipoRespuesta[id];
                        >*/
                        testigoRespuesta.wait();
                        if(not hayRespuesta[id]){
                            d[id]++;
                            testigoRespuesta.signal();
                            b[id] ->wait();
                        }
                        resp = tipoRespuesta[id];
                        PasarTestigoRespuesta(hayRespuesta);

                        //analizar el resultado de la respuesta
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
                    }
                }
                /*<numClientFin = numClientFin + 1>*/
                testigoFinalizador.wait();
                numClientFin = numClientFin + 1;
                PasarTestigoFinalizador(ref(numClientFin));
            }
void Publicador(const int id, BoundedQueue<Peticion> &c, const bool esVG, bool hayRespuesta[NUM_Cl], Respuesta tipoRespuesta[NUM_Cl], ImageWindow &win){
    int probabilidad = rand() % 100;
    bool fin = false;
    double precio = 0.0;

    Peticion pet;
    Respuesta resp;

    int numPetAtendidas = 0;
    int numPetAceptadas = 0;
    double precioTotal = 0.0;

    bool seguir = true;

    while(seguir){ //pet.tpPet != Finalizar
        /*<await c.length() > 0
            pet = c.first();
            c.dequeue();
        >*/
        if(esVG){
            testigoColaVG.wait();
            if(not c.length()>0){
                d3_colaVG++;
                testigoColaVG.signal();
                b3_colaVG.wait();
            }
            pet = c.first();
            c.dequeue();
            PasarTestigoColaVG(ref(c));
        }else {
            testigoColaVP.wait();
            if(not c.length()>0){
                d3_colaVP++;
                testigoColaVP.signal();
                b3_colaVP.wait();
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
            resp.estado = fin;

            if(fin){
                precio = PRECIO_BASE * (1.0/(c.length()+1));
                resp.precio = precio;
            }else{
                resp.precio = 0.0;
            }

            int indice = pet.idC;
            /*< tipoRespuesta[indice] = resp;
              hayRespuesta[indice] = true;
            >*/
            testigoRespuesta.wait();
            tipoRespuesta[indice] = resp;
            hayRespuesta[indice] = true;
            PasarTestigoRespuesta(hayRespuesta);

            if(fin){
                mostrarImagen(win, pet.imagen, pet.tServ);
            }

            numPetAtendidas++;
        }else{
            seguir = false;
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
   testigoFinalizador.wait();
   if(numClientFin < NUM_Cl){
        d1_finalizador++;
        testigoFinalizador.signal();
        b1_finalizador.wait();
    }
    PasarTestigoFinalizador(ref(numClientFin));


   /*<await colaVG.length() < TAM_COLA_VG
        colaVG.enqueue(pet);
    >*/
    testigoColaVG.wait();
    if(not (colaVG.length() < TAM_COLA_VG)){
        d2_colaVG++;
        testigoColaVG.signal();
        b2_colaVG.wait();
    }
    colaVG.enqueue(pet);
    PasarTestigoColaVG(ref(colaVG));
    


    /*<await colaVP.length() < TAM_COLA_VP
          colaVP.enqueue(pet);
    >*/
    /*
    testigoColaVP.wait();
    if(not (colaVP.length() < TAM_COLA_VP)){
        d2_colaVP++;
        testigoColaVP.signal();
        b2_colaVP.wait();
    }
    colaVP.enqueue(pet);
    PasarTestigoColaVP(ref(colaVP));
    */
   for(int k = 0; k<2; k++){
        testigoColaVP.wait();
        if(not (colaVP.length() < TAM_COLA_VP)){
            d2_colaVP++;
            testigoColaVP.signal();
            b2_colaVP.wait();
        }
        colaVP.enqueue(pet);
        PasarTestigoColaVP(ref(colaVP));
    }
}

void inicializar(bool hayRespuesta[NUM_Cl]){
    for (int i=0; i<NUM_Cl; i++){
        d[i] = 0;
        b[i] = new Semaphore(0);
        hayRespuesta[i] = false;
    }

}

void finalizar(){
    for (int i=0; i<NUM_Cl; i++){
        delete b[i];
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));

    //ruta al directorio que contiene "viewer.py"
    const string pathViewer = "./";

    BoundedQueue<Peticion> colaVG(TAM_COLA_VG);
    BoundedQueue<Peticion> colaVP(TAM_COLA_VP);

    bool hayRespuesta[NUM_Cl];
    Respuesta tipoRespuesta[NUM_Cl];

    inicializar(hayRespuesta);

    int numClientFin = 0;

    // Creación de las 3 ventanas con diferentes dimensiones y posiciones: 
    // imWin1 -> Valla Grande (800x637) 
    // imWin2 -> Valla Pequeña 1 (400x300 en x=860, y=0) 
    // imWin3 -> Valla Pequeña 2 (400x300 en x=860, y=365) 
    ImageWindow imWin1(pathViewer, 800, 637); 
    ImageWindow imWin2(pathViewer, 400, 300, 860, 0); 
    ImageWindow imWin3(pathViewer, 400, 300, 860, 365);

    thread clientes [NUM_Cl];
    for(int i = 0; i < NUM_Cl; i++){
        clientes[i] = thread(&Cliente, i, ref(colaVG), ref(colaVP), hayRespuesta, tipoRespuesta, ref(numClientFin));
    }

    thread publicador [NUM_Pub];
    publicador[0] = thread(&Publicador, 0, ref(colaVG), true, hayRespuesta, tipoRespuesta, ref(imWin1));
    publicador[1] = thread(&Publicador, 1, ref(colaVP), false, hayRespuesta, tipoRespuesta, ref(imWin2));
    publicador[2] = thread(&Publicador, 2, ref(colaVP), false, hayRespuesta, tipoRespuesta, ref(imWin3));

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