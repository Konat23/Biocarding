from tkinter import *
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from comunication import Comunication
import collections

"""
Codigo original de: https://youtu.be/DJY9TFxrYbM
Codigo adaptado por: Sebastian Guerrero.
"""

class Grafica(Frame):
    def __init__(self, master, *args):
        super().__init__(master, *args)
        self.datos_arduino = Comunication()
        self.actualizar_puertos()
        
        self.muestra = 100
        self.datos = 0.0
        
        self.fig, ax = plt.subplots(facecolor='#000000', dpi=100, figsize=(4,2))
        plt.title("Arduino Data",color='white',size =12)
        ax.tick_params(direction = "out", length=5, width=2,colors="white", grid_color="white",grid_alpha=0.5)
        
        self.line, = ax.plot([],[], color = 'g', linewidth=2)
        
        plt.xlim=([0, self.muestra])
        #plt.ylim([0, 5.5])
        ax.set_xlim(xmin=0.0, xmax=100)
        ax.set_ylim(ymin=0, ymax=5)
        """
        Aqui iran algunas configuraciones de color 
        que por ahora no me intersan (9:44)
        
        """
        self.datos_señal_uno = collections.deque([0]*self.muestra, maxlen = self.muestra)
        
        self.widgets()
    
    def animate(self, i):
        self.datos = (self.datos_arduino.datos_recibidos.get())
        dato = self.datos.split(",")
        dato1 = float(dato[0])
        #datos2 = float(dato[1])
        
        self.datos_señal_uno.append(dato1)
        print(dato1)
        #self.datos_señal_dos.append(dato2)
        self.line.set_data(range(self.muestra), self.datos_señal_uno)
        #self.line2.set_data(range(self.muestra), self.datos_señal_dos)
    
    def iniciar(self,):
        self.ani = animation.FuncAnimation(self.fig, self.animate, interval =100, blit = False)
        #self.bt_graficar.config(state = 'disabled')
        #self.bt_pausar.config(state = 'normal')
        self.canvas.draw()
    def pausar(self):
        self.ani.event_source.stop()
        #self.bt_reanudar.config(state = 'normal')
        
    def widgets(self):
        frame = Frame(self.master, bg='black', bd=2)
        frame.grid(column=0, columnspan =2,row=0, sticky='nsew')
        
        self.canvas = FigureCanvasTkAgg(self.fig, master = frame)
        self.canvas.get_tk_widget().pack(padx=0,pady=0,expand=True, fill='both')
        
        #aqui van algunos botones que no voy a incluir por ahora.
        # Graficar
        # Pausar
        # Reanudar
        
        self.bt_conectar = Button(frame, text='Conectar', command= self.conectar_serial)
        self.bt_conectar.pack()
        
        self.bt_desconectar = Button(frame, text='Desconectar', command= self.desconectar_serial)
        self.bt_desconectar.pack()
        self.bt_graficar = Button(frame, text='Graficar', command= self.iniciar)
        self.bt_graficar.pack()
    
    def actualizar_puertos(self):
        self.datos_arduino.puertos_disponibles()
    def conectar_serial(self):
        # actualizar estado de los botones:
        
        self.datos_arduino.arduino.port = self.datos_arduino.puertos[0]
        #self.datos_arduino.arduino.port = 'COM3'
        self.datos_arduino.arduino.baudrate = self.datos_arduino.baudrates[0]
        self.datos_arduino.conexion_serial()
    
    def desconectar_serial(self):
        # actualizar estado de botones
        
        try:
            self.ani.event_source.stop()
        except AttributeError:
            pass
        self.datos_arduino.desconectar()
    
    """
    aqui definio lo que hacen algunos sliders
    
    """
    
if __name__ == "__main__":
    ventana = Tk()
    ventana.geometry=('742x535')
    
    app = Grafica(ventana)
    app.mainloop()
