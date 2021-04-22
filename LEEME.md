# Practica 1: Filtro de Sobel Paralelizado

Programa escrito en C++ que implementa el filtro de Sobel de forma paralela, utilizando la libreria pthread. Practica realizada para la materia Computacion Paralela y Distribuida.- Universidad Nacional de Colombia, sede Bogotá.

## Integrantes
- Marcelo Escamilla Sanchez
- Daniel Alejandro Guillén Rodriguez

## Requerimientos
Para correr este programa es necesario tener instalado:
- CMake 3.16.3 >
- GNU make 4.2.1 >
- OpenCV 4.5.2 >

si se sigue el siguiente [tutorial](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html) se tendra certeza de tener todas las herramientas necesarias.

## Compilación
En la carpeta raiz:
```
cmake .
```
Este comando generara el makefile necesario para la compilacion. Una vez se haya corrido:
```
make
```
Se compilara el codigo fuente y el programa estara listo para su uso. El programa posee la siguiente firma:
```
./filtroPosix <nombre_imagen_entrada> <nombre_imagen_salida> <cantidad_de_hilos>
```

## Pruebas
Una vez se haya compilado el pograma, se puede correr el script **runTests** para que se ejecuten una serie de pruebas sobre las imagenes de muestra. Para cada imagen, se correra el programa con 1, 2, 4, 8 y 16 hilos 10 veces para obtener un promedio. Los resultados se guardaran en el archivo **results.txt**

Cerciorese de que el script tenga permisos de ejecución corriendo el siguiente comando:
```
chmod x+ runTests.sh
```