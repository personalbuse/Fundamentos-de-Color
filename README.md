# Fundamentos del color (RGB, CMY,YIQ, etc)
### Aplicación y visualizacion practica en Cuaderno de Jupyter
### Uso de Codigo en C para comparar tiempo de ejecución de algoritmos _'no supervisados'_
### Uso de Aplicación en Graddle para probar y en directo la segmentación de colores

![Rueda de colores](https://albertodugarte.com/wp-content/uploads/2025/03/teoria-del-color.jpg)

---

## 1. Cuaderno Jupyter: `color.ipynb`

Este cuaderno interactivo demuestra:

1. Fundamentos de color:
   - Brillo
   - Matiz
   - Cromaticidad
2. Conversiones entre modelos de color:
   - RGB
   - CMY
   - YIQ
   - HSI
   - HSV
3. Clasificación de colores usando **K-Means**
4. Validación de los resultados utilizando imágenes aleatorias

### Requisitos
## Crear Virtual Enviroment (VENV) dentro de esta carpeta y activarlo
```bash
python -m venv venv
```
### Una vez activado el enviroment usar el comando

```bash
pip install numpy matplotlib scikit-learn opencv-python pillow ipython requests
```

### Para verificar que todo se instalo correctamente utilizar
```bash
pip freeze
```


### Uso

1. Abrir el cuaderno `color.ipynb` en Jupyter Notebook,JupyterLab, Code o Cursor. (Compilador de preferencia)
2. Seleccionar el kernel adecuado de nuestro VENV
3. Ejecutar las celdas paso a paso para ver la conversión de modelos y la segmentación de colores.
4. Observar los gráficos de validación y resultados de K-Means.
### 5. La ultima celda del cuaderno Jupyter genera el `dataset` , correrla solo una vez, este crea la carpeta automaticamente
*En caso de que genere error la ultima celda informar a los expositores porque caimos la API por multiples peticiones*
*Probar manualmente la pagina*
_https://minimalistic-wallpaper.demolab.com/?random_

---

## 2. Programa en C: `k_means_parrallel_(sistema operativo).c`

Este programa aplica **K-Means** para segmentar el color de imágenes en un dataset y comparar **tiempo de ejecución secuencial vs paralelo** usando OpenMP.

### Cómo funciona

1. El programa recorre la carpeta `dataset_jpg` y carga todas las imágenes usando **stb_image.h**.
2. Convierte cada imagen a un arreglo de píxeles RGB en memoria.
3. Para cada imagen, aplica el algoritmo **K-Means**:
   - Inicializa `k` centroides de colores aleatorios.
   - Asigna cada píxel al cluster más cercano según distancia Euclidiana en RGB.
   - Recalcula los centroides promedio de cada cluster.
   - Repite hasta alcanzar el número máximo de iteraciones.
4. Paraleliza la asignación de píxeles a clusters usando **OpenMP**, acelerando el procesamiento de grandes datasets.
5. Calcula estadísticas:
   - Total de imágenes procesadas
   - Total de píxeles en el dataset
   - Tiempo de ejecución en modo secuencial y paralelo
6. (Opcional) Se pueden guardar las imágenes segmentadas para visualizar los resultados.

**Concepto clave:** Cada píxel se procesa como un vector RGB, agrupándolo con colores similares para simplificar la imagen y visualizar clusters de color.

### Requisitos

- GCC con soporte para OpenMP
- Librerías de cabecera:
  - `stb_image.h`
  - `stb_image_write.h`  
  (disponibles en [stb GitHub](https://github.com/nothings/stb))

### Compilación

```bash
gcc -O2 -fopenmp k_means_parrallel_(OS).c -o kmeans -lm
```

### Uso

1. La ultima celda del cuaderno creara 10 imagenes aleatorias por ejecución que automaticamente 
se guardan en data para procesar despues
2. Ejecutar:

```bash
./kmeans (en Linux)
kmeans.exe (en Windows)
```

3. La salida mostrará algo como:
## Puede demorarse debido a no reescalado y cantidad de pixeles y 50 iteraciones de agrupamiento

```
Renderizando x imágenes
Segmentando x imágenes para K-Means
Pixeles utilizados en dataset: xxxxxx

Tiempo en secuencial: 12.3456 s
Tiempo en paralelo  : 4.1234 s
```

---

## 3. Consideraciones

- El cuaderno Jupyter es útil para **demostración teórica y visual** de conceptos de color y K-Means.
- El programa en C es ideal para **procesamiento eficiente de grandes volúmenes de imágenes**, mostrando la ventaja de paralelizar el algoritmo.
- Para exposiciones, se recomienda mostrar ambos resultados: gráficos del cuaderno y estadísticas de tiempos en C.

---

## 4. Referencias

- [K-Means Clustering](https://scikit-learn.org/stable/modules/clustering.html#k-means)
- [stb_image GitHub](https://github.com/nothings/stb)
- [Modelos de color y conversiones](https://en.wikipedia.org/wiki/Color_space)

