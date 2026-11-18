//------------------------------------------------------------------------------
// File:   ImageWindow.hpp
// Author: PSCD
// Date:   septiembre 2026
// Coms:   Parte del práctica 3 de PSCD
//         Permite mostrar imágenes en una ventana en C++
//         Require python3 y la librería Pillow. Si no está, 
//         se puede instalar localmente mediante
//              python3 -m pip install --user Pillow
//         Dibuja enviando ciertos comandos a un proceso python, que
//         es el que muestra las imágenes.
//------------------------------------------------------------------------------
#ifndef IMAGE_WINDOW_HPP
#define IMAGE_WINDOW_HPP

#include <cstdio>
#include <string>

// Separadores para directorios dependen del SO
#ifdef _WIN32
const std::string DIRSEP("\\");
#else //linux, unis, MacOS, ....
const std::string DIRSEP("/");
#endif

class ImageWindow {
private:
    // pipe para enviar comandos a python
    FILE* py;
    // código python para gestionar las imágenes
    const std::string VIEWER = "viewer.py";
    // posiblemente habrá que adaptar, dependiendo de la instalación 
    // de python, como
    // const std::string PYTHON = "python";
    const std::string PYTHON = "python3";
    // En el caso de lab000, es 
    // const std::string PYTHON = "python3.6";

public:
    // Constructor: coloca la ventana en posición y tamaño por defecto
    // path2Viewer: path completo al directorio donde se encuentra "viewer,py"
    ImageWindow(const std::string& path2Viewer);

    // Constructor: coloca la ventana en posición por defecto y tamaño (width,height)
    // La imagen se redimensiona para el tamaño dado
    ImageWindow(const std::string& path2Viewer, int width, int height);

    // Constructor: coloca la ventana en posición en (X,Y) y tamaño (width,height)
    // La imagen se redimensiona para el tamaño dado
    ImageWindow(const std::string& path2Viewer, int width, int height, int X, int Y);

    // Destructor
    ~ImageWindow();

    // Muestra la imagen, hasta que el usuario cierre la ventana, o es
    // remplazada mediante otra ejecución de "show"
    void show(const std::string& imageFileName);

    // Cierra la ventana y el pipe
    void close();
};

#endif
