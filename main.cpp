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
void *callFilter(void *arg);

//cantidad de hilos
int THREADS;
Mat1b result;
Mat1b src;

int main(int argc, char** argv )
{
    //verificar cantidad de argumentos
    if ( argc != 3 ){
        printf("uso: filtroPosix <Image_Path> <num_threads>\n");
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
    if(1 != sscanf(argv[2], "%d", &THREADS)){
        printf("El argumento <num_threads> debe ser un numero\n");
        return -1;
    }

    auto start = high_resolution_clock::now();

    // iniciar matriz acumuladora
    result = Mat1b(src.rows, src.cols);

    //filtro
    applyFilter(src);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start);

    printf("%ld\n", duration.count());
    string arg1(argv[1]);
    imwrite("output.jpg", result);

    return 0;
}

void applyFilter(Mat1b img){
    // Compute custom Sobel 3x3 derivatives

    int threadId[THREADS], *retval;
    pthread_t threads[THREADS];

    for(int i = 0; i < THREADS; i++){
        threadId[i] = i;
        pthread_create(&threads[i], NULL, callFilter, &threadId[i]);
    }

    for(int i = 0; i < THREADS; i++){
        pthread_join(threads[i], (void **)&retval);
    }

}

// void sobelFilter(const Mat1b& src, Mat1s& dst, int direction)
void *callFilter(void *arg){
    int initHeight, endHeight, threadId = *(int *) arg;
    initHeight = ((src.rows)/THREADS) * threadId;
    endHeight = initHeight + ((src.rows)/THREADS - 1);
    Mat1b chunk(src, Rect(0, initHeight, src.cols, ((src.rows)/THREADS)));
    Mat1s sx, sy;
    sobelFilter(chunk, sx, 0);
    sobelFilter(chunk, sy, 1);
    // Edges L1 norm
    Mat1s filteredChunk;
    absdiff(sx, sy, filteredChunk);
    //imwrite("file_"+to_string(threadId)+".jpg", filteredChunk);
    for(int i = 0; i < filteredChunk.rows; i++)
    filteredChunk.row(i).copyTo(result.row(i+initHeight));
    
    //printf("id: %d i=%d, e=%d\n", threadId, filteredChunk.cols, filteredChunk.rows);
    return 0;
}

void sobelFilter(Mat1b& src, Mat1s&dst, int direction)
{
    Mat1s kernel;
    int radius = 0;

    // Create the kernel
    if (direction == 0)
    {
        // Sobel 3x3 X kernel
        kernel = (Mat1s(3,3) << -1, 0, +1, -2, 0, +2, -1, 0, +1);
        radius = 1;
    }
    else
    {
        // Sobel 3x3 Y kernel
        kernel = (Mat1s(3, 3) << -1, -2, -1, 0, 0, 0, +1, +2, +1);
        radius = 1;
    }

    // Handle border issues
    Mat1b _src;
    copyMakeBorder(src, _src, radius, radius, radius, radius, BORDER_REFLECT101);

    // Create output matrix
    dst.create(src.rows, src.cols);

    // Convolution loop

    // Iterate on image 
    for (int r = radius; r < _src.rows - radius; ++r)
    {
        for (int c = radius; c < _src.cols - radius; ++c)
        {
            short s = 0;

            // Iterate on kernel
            for (int i = -radius; i <= radius; ++i)
            {
                for (int j = -radius; j <= radius; ++j)
                {
                    s += _src(r + i, c + j) * kernel(i + radius, j + radius);
                }
            }
            dst(r - radius, c - radius) = s;
        }
    }
}