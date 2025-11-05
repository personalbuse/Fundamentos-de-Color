#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>  // Incluido para stat()
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct {
    unsigned char r,g,b;
} Pixel;

double distance(Pixel a, Pixel b) {
    return sqrt((a.r - b.r)*(a.r - b.r) + (a.g - b.g)*(a.g - b.g) + (a.b - b.b)*(a.b - b.b));
}

// K-means secuencial/paralelo
void kmeans(Pixel *pixels, int n, int k, int max_iter, int paralelo) {
    Pixel *centroids = malloc(sizeof(Pixel)*k);
    int *labels = malloc(sizeof(int)*n);

    srand(time(NULL));
    for(int i=0; i<k; i++) centroids[i] = pixels[rand()%n];

    for(int iter=0; iter<max_iter; iter++) {
        // Asignar cluster
        if(paralelo)
            #pragma omp parallel for
            for(int i=0; i<n; i++){
                double min_dist = 1e9;
                int label = 0;
                for(int j=0; j<k; j++){
                    double d = distance(pixels[i], centroids[j]);
                    if(d < min_dist){ min_dist=d; label=j; }
                }
                labels[i]=label;
            }
        else
            for(int i=0; i<n; i++){
                double min_dist = 1e9;
                int label = 0;
                for(int j=0; j<k; j++){
                    double d = distance(pixels[i], centroids[j]);
                    if(d < min_dist){ min_dist=d; label=j; }
                }
                labels[i]=label;
            }

        // Recalcular centroides
        Pixel *new_centroids = calloc(k, sizeof(Pixel));
        int *counts = calloc(k, sizeof(int));
        for(int i=0; i<n; i++){
            int l = labels[i];
            new_centroids[l].r += pixels[i].r;
            new_centroids[l].g += pixels[i].g;
            new_centroids[l].b += pixels[i].b;
            counts[l]++;
        }
        for(int j=0; j<k; j++){
            if(counts[j] != 0){
                centroids[j].r = new_centroids[j].r / counts[j];
                centroids[j].g = new_centroids[j].g / counts[j];
                centroids[j].b = new_centroids[j].b / counts[j];
            }
        }
        free(new_centroids);
        free(counts);
    }

    if(paralelo)
        #pragma omp parallel for
        for(int i=0; i<n; i++) pixels[i] = centroids[labels[i]];
    else
        for(int i=0; i<n; i++) pixels[i] = centroids[labels[i]];

    free(centroids);
    free(labels);
}

int main() {
    char *folder = "./dataset_jpg";
    DIR *dir = opendir(folder);
    if(!dir){ perror("No se puede abrir carpeta"); return 1; }

    struct dirent *entry;
    int total_images = 0;
    long long total_pixels = 0;
    struct stat path_stat;
    char path[512];

    // Contar imágenes y pixeles
    while((entry = readdir(dir)) != NULL){
        snprintf(path, sizeof(path), "%s/%s", folder, entry->d_name);
        if(stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode)){
            total_images++;
            int w, h, nc;
            unsigned char *img = stbi_load(path, &w, &h, &nc, 3);
            if(img) { total_pixels += (long long)w*h; stbi_image_free(img); }
        }
    }
    closedir(dir);

    printf("Renderizando %d imágenes\n", total_images);
    printf("Segmentando %d imágenes para K-means\n", total_images);
    printf("Pixeles utilizados en dataset: %lld\n\n", total_pixels);

    // Medir tiempo secuencial
    double t1 = omp_get_wtime();
    dir = opendir(folder);
    while((entry = readdir(dir)) != NULL){
        snprintf(path, sizeof(path), "%s/%s", folder, entry->d_name);
        if(stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode)){
            int w, h, nc;
            unsigned char *img_data = stbi_load(path, &w, &h, &nc, 3);
            if(!img_data) continue;

            Pixel *pixels = malloc(sizeof(Pixel) * w * h);
            for(int i=0; i<w*h; i++){
                pixels[i].r = img_data[i*3];
                pixels[i].g = img_data[i*3 + 1];
                pixels[i].b = img_data[i*3 + 2];
            }

            kmeans(pixels, w*h, 5, 50, 0); // secuencial

            free(pixels);
            stbi_image_free(img_data);
        }
    }
    closedir(dir);
    double t_seq = omp_get_wtime() - t1;

    // Medir tiempo paralelo
    double t2 = omp_get_wtime();
    dir = opendir(folder);
    while((entry = readdir(dir)) != NULL){
        snprintf(path, sizeof(path), "%s/%s", folder, entry->d_name);
        if(stat(path, &path_stat) == 0 && S_ISREG(path_stat.st_mode)){
            int w, h, nc;
            unsigned char *img_data = stbi_load(path, &w, &h, &nc, 3);
            if(!img_data) continue;

            Pixel *pixels = malloc(sizeof(Pixel) * w * h);
            for(int i=0; i<w*h; i++){
                pixels[i].r = img_data[i*3];
                pixels[i].g = img_data[i*3 + 1];
                pixels[i].b = img_data[i*3 + 2];
            }

            kmeans(pixels, w*h, 5, 50, 1); // paralelo

            free(pixels);
            stbi_image_free(img_data);
        }
    }
    closedir(dir);
    double t_par = omp_get_wtime() - t2;

    printf("Tiempo en secuencial: %.4f s\n", t_seq);
    printf("Tiempo en paralelo  : %.4f s\n", t_par);

    return 0;
}
