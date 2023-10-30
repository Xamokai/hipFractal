#include <cstdlib>
#include <hip/hip_runtime.h>
#include <stdio.h>

struct rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

__global__ void calc_pixel(rgb* pixels, unsigned int size_x, unsigned int size_y){
    
    unsigned int a = threadIdx.x + blockIdx.x * blockDim.x;
    unsigned int b = threadIdx.y + blockIdx.y * blockDim.y;
    unsigned int id = a + b * size_y;
    double x = ((double) a) /  ((double)size_x / 4) - 2;
    double y = ((double) b) /  ((double)size_y/4) - 2;

    double v = 0;
    double w = 0;
    if(id >= size_y * size_x){
        return;
    }
    
    for (int i = 0; i < 2000; i++) {
        if(sqrtf(x*x + y*y) > 2){
            pixels[id].r = i;
            pixels[id].g = 0;
            pixels[id].b = 128;
            return;
        }
        x = (x*x) - (y * y) +w ;
        y = 2 * x * y + v ;
    
    }

    pixels[id].r = 255;
    pixels[id].g = 255;
    pixels[id].b = 255;
    return;
}

int main(){
    unsigned int size_x =  512;
    unsigned int size_y =   512;

    rgb* pixels, * host_pixels;
    unsigned int size = size_x * size_y * sizeof(rgb);
    hipMalloc(&pixels, size);
    host_pixels = (rgb*) malloc(size);

    unsigned int spb = 64;
    dim3 shader = {spb, spb, 1};
    calc_pixel<<<shader, dim3{size_x/spb, size_y/spb,1}>>>(pixels, size_x, size_y);

    hipDeviceSynchronize();

    hipMemcpyDtoH(host_pixels, pixels, size);// size_x * size_y * sizeof(rgb));

    printf("P3\n");
    printf("%d %d\n", size_x, size_y);
    printf("256\n");

    for(int i = 0; i < size_x * size_y; i++) {
        printf("%d %d %d\n", host_pixels[i].r,host_pixels[i].g, host_pixels[i].b);
    }

    hipFree(pixels);
}
