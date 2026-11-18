# -*- coding: utf-8 -*-
# #------------------------------------------------------------------------------
# File:   viewer.py
# Author: PSCD
# Date:   septiembre 2026
# Coms:   Visor gráfico sencillo que recibe nombres de imágenes por la entrada
#         estándar y las muestra redimensionadas en una ventana
#------------------------------------------------------------------------------
from tkinter import Tk, Label
from PIL import Image, ImageTk
import sys, os

WIDTH = 800
HEIGHT = 600
X = 0
Y = 0

# Permite indicar tamaño y posición de la ventana por línea de comandos.
if len(sys.argv) >= 3:
    WIDTH = int(sys.argv[1])
    HEIGHT = int(sys.argv[2])
    if len(sys.argv) >= 5:
        X = int(sys.argv[3])
        Y = int(sys.argv[4])

root = Tk()
root.geometry(f"{WIDTH}x{HEIGHT}+{X}+{Y}")
root.resizable(False, False)

label = Label(root)
label.pack(fill="both", expand=True)

# print("Viewer ready", flush=True)

#------------------------------------------------------
def check_input():
    # Lee una línea de la entrada estándar.
    line = sys.stdin.readline()

    # Si no hay datos, vuelve a intentarlo más tarde.
    if not line:
        root.after(100, check_input)
        return

    name = line.strip()

    # print(f"Showing: {name}", flush=True)

    # Finaliza el visor.
    if name == "quit":
        root.destroy()
        return

    try:
        # Muestra el nombre del fichero en el título de la ventana.
        root.title(os.path.basename(name))

        # Carga la imagen.
        img = Image.open(name)

        # Mantiene la proporción ajustándola al tamaño de la ventana.
        img.thumbnail((WIDTH, HEIGHT), Image.LANCZOS)

        photo = ImageTk.PhotoImage(img)

        # Actualiza la imagen mostrada.
        label.config(image=photo)

        # Mantiene una referencia para evitar que Tk la libere.
        label.image = photo
    except Exception as e:
        print(e, flush=True)

    # Programa la siguiente comprobación de entrada.
    root.after(100, check_input)
#------------------------------------------------------

root.after(100, check_input)
root.mainloop()