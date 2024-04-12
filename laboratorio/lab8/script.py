import matplotlib.pyplot as plt
import matplotlib.animation as animation
import serial
import numpy as np


# Configurar y abrir el puerto serial

# Verificar que el puerto se abrio correctamente

# Evento que se ejecuta cuando se cierra la grafica
def handle_close(evt):
    print('#Serial: Puerto cerrado')
    
# Figura donde se va a graficar los datos
fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
# Conectar una función que se ejecutara cuando se cierre la grafica
fig.canvas.mpl_connect('close_event', handle_close)
t = []       # Vector de tiempo
samples = [] # Vector de voltaje

Tplot = 20  # Tiempo de actualizar grafica (ms)
Fs = 100    # Frecuencia de muestreo (Hz)
Ts = 1/Fs   # Periodo de muestreo
Nplot = 30  # Numero de muestras maximas a graficar (N*Ts > Tplot)
Vmin = -1.0 # Voltaje maximo del ADC
Vmax = 1.0  # Voltaje minimo del ADC
first_plot = True

# This function is called periodically from FuncAnimation
def animate(n, t, samples, Ts, N):
    global first_plot
    
    # Dato a graficar
	sample = np.sin(np.pi/5 * n)
    
	# Añadir las muestras a las listas
    t.append(n)
    samples.append(sample)
    
    # Eliminar las muestras pasadas cuando len(samples) > Nplot
    # Y establecer el eje de tiempo min y max
    if first_plot:
        if len(t) < N:
            tmin = 0
            tmax = None
        else:
            first_plot = False
            tmin=t[0]
            tmax=t[-1]
    else:        
        
        if len(t)>N:
            t=t[len(t)-N:len(t)]
            samples=samples[len(samples)-N:len(samples)]    
        tmin = t[0]
        tmax = t[-1]
            
    # Graficar samples vs t
    ax.clear()
    ax.plot(t, samples)

    # Format plot
    plt.xticks(rotation=45, ha='right')
    plt.subplots_adjust(bottom=0.30)
    plt.ylabel('Tiempo (s)')
    plt.ylabel('Voltaje (V)')
    plt.axis([tmin, tmax, Vmin, Vmax])


# Habilitar grafica
ani = animation.FuncAnimation(fig, animate, fargs=(t, samples, Ts, N), interval=Tplot)
fig.show()
