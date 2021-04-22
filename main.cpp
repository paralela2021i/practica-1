#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <string>
#include <chrono>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
using namespace std::chrono;

void applyFilter(Mat1b image);
void sobelFilter(Mat1b& src, Mat1s& dst, int direction);
void *filterChunk(void *arg);

//cantidad de hilos
int THREADS;
Mat1b result;
Mat1b src;

int main(int argc, char** argv )
{
    //verificar cantidad de argumentos
    if ( argc != 4 ){
        printf("uso: ./filtroPosix <ruta_imagen_entrada> <ruta_imagen_salida> <num_threads>\n");
        return -1;
    }

    // leer la imagen
    src = imread( argv[1], IMREAD_GRAYSCALE);


    // verificar que no este vacia
    if ( src.empty() ){
        printf("El archivo no es una imagen \n");
        return -1;
    }

    //verificar que num_threads sea un numero
    if(1 != sscanf(argv[3], "%d", &THREADS)){
        printf("El argumento <num_threads> debe ser un numero\n");
        return -1;
    }


    //inicio del caluclo de duracion
    auto start = high_resolution_clock::now();

    // iniciar matriz acumuladora
    result = Mat1b(src.rows, src.cols);

    //filtro
    applyFilter(src);

    auto end = high_resolution_clock::now();

    //computo de la duracion en nanosegungos
    auto duration = duration_cast<nanoseconds>(end - start);

    printf("%ld\n", duration.count());

    //escritura del resultado
    string outputName = argv[2];
    imwrite(outputName, result);

    return 0;
}

void applyFilter(Mat1b img){
    
    // creacion de hilos
    int threadId[THREADS], *retval;
    pthread_t threads[THREADS];

    for(int i = 0; i < THREADS; i++){
        threadId[i] = i;
        pthread_create(&threads[i], NULL, filterChunk, &threadId[i]);
    }

    for(int i = 0; i < THREADS; i++){
        pthread_join(threads[i], (void **)&retval);
    }

}

void *filterChunk(void *arg){
    int initHeight, endHeight, threadId = *(int *) arg;
    //altura inicial y final con base al threadId
    initHeight = ((src.rows)/THREADS) * threadId;
    endHeight = initHeight + ((src.rows)/THREADS - 1);

    //region pertinente de la imagen de entrada
    Mat1b chunk(src, Rect(0, initHeight, src.cols, ((src.rows)/THREADS)));
    
    //gradientes vertical y horizontal
    Mat1s sx, sy;
    sobelFilter(chunk, sx, 0);
    sobelFilter(chunk, sy, 1);

    //valor absoluto de la magnitud
    Mat1s filteredChunk;
    absdiff(sx, sy, filteredChunk);

    //acumular region en la matriz de resultado
    for(int i = 0; i < filteredChunk.rows; i++)
    filteredChunk.row(i).copyTo(result.row(i+initHeight));
    

    return 0;
}

void sobelFilter(Mat1b& src, Mat1s&dst, int direction)
{
    Mat1s kernel;
    int radius = 1;

    // Matriz de convolucion
    if (direction == 0){
        // kernel X
        kernel = (Mat1s(3,3) << -1, 0, +1, -2, 0, +2, -1, 0, +1);
    }
    else{
        // kernel Y
        kernel = (Mat1s(3, 3) << -1, -2, -1, 0, 0, 0, +1, +2, +1);
    }

    // rellenar bordes para evitar conflictos con indices no existentes
    Mat1b _src;
    copyMakeBorder(src, _src, radius, radius, radius, radius, BORDER_REFLECT101);

    // crear matriz de salida
    dst.create(src.rows, src.cols);

    // Convolucion

    // iterar por la region
    for (int r = radius; r < _src.rows - radius; ++r){
        for (int c = radius; c < _src.cols - radius; ++c){
            short s = 0;

            // Calculo de pixel filtrado
            for (int i = -radius; i <= radius; ++i){
                for (int j = -radius; j <= radius; ++j){
                    s += _src(r + i, c + j) * kernel(i + radius, j + radius);
                }
            }
            dst(r - radius, c - radius) = s;
        }
    }
}