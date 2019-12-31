#include<stdlib.h>
#include<stdio.h>
#include<string.h>

typedef struct pixel {
    unsigned char r, g, b;
} Pixel;

typedef struct image {
    unsigned int width, height, max;
    Pixel **data;
} Image;

Image *readPPM(char *file_name);
int writePPM(char *file_name, Image *image);
Image *convert(Image *inImg, Image *outImg, int *kernel, int size, int divisor);
unsigned char alterPixel(Image *inImage, int *kernel, int size, int divisor, int i, int j, char color);

int main(int argc, char** argv) {
    if (argc != 4) {
        printf("Usage: ./filter input_file, kernel_file, output_file");
        return -1;
    }
    char * input = argv[1];
    char * ker = argv[2];
    char * output = argv[3];
    /* read kerndl */
    FILE *kerFile = fopen(ker, "r");
    int size, divisor;
    fscanf(kerFile, "%d", &size);
    fscanf(kerFile, "%d", &divisor);
    int kernel[size * size];
    int i;
    for (i = 0; i < size * size; i++) {
       fscanf(kerFile, " %d", &kernel[i]); 
    }
    fclose(kerFile);
    /* create images from files */
    Image * inImg = malloc(sizeof(Image));
    inImg = readPPM(input);
    Image *outImg = malloc(sizeof(Image));
    outImg = readPPM(input);
    outImg = convert(inImg, outImg, kernel, size, divisor);
    writePPM(output, outImg);
    /* free memory */
    for(i = 0; i < inImg->height; i++){
        inImg->data[i] = malloc(sizeof(Pixel) * inImg->width);
    }
    free(inImg->data);
    free(inImg);
    for(i = 0; i < outImg->height; i++){
        outImg->data[i] = malloc(sizeof(Pixel) * outImg->width);
    }
    free(outImg->data);
    free(outImg);
    return 0;
}

Image * convert(Image *inImg, Image *image, int *kernel, int size, int divisor) {
    int i, j;
    /* perform this action on every square of the matrix */
    for(i = 0; i < image->height; i++){
        for(j = 0; j < image->width; j++){
            image->data[i][j].r = alterPixel(inImg, kernel, size, divisor, i, j, 'r');
            image->data[i][j].g = alterPixel(inImg, kernel, size, divisor, i, j, 'g');
            image->data[i][j].b = alterPixel(inImg, kernel, size, divisor, i, j, 'b');
        }
    }
    return image;
}

unsigned char alterPixel(Image *image, int *kernel, int size, int divisor, int r, int c, char color) {
    int i, j;
    int total = 0;
    int counter = 0;
    int padding = size / 2;
    /* calculate for all surrounding values */
    for (i = r - padding; i <= r + padding; i++) {
        for (j = c - padding; j <= c + padding; j++) {
            if (i < 0 || j < 0 || j >= image->width || i >= image-> height) {
                total += 0;
            } else if (color == 'r') {
                total += (image->data[i][j].r) * kernel[counter];
            } else if (color == 'g') {
                total += (image->data[i][j].g) * kernel[counter];
            } else if (color == 'b') {
                total += (image->data[i][j].b) * kernel[counter];
            }
            counter++;
        }
    }
    total = total / divisor;
    /* check of overflow */
    if (total > 255) {
        return 255;
    }
    if (total < 0) {
        return 0;
    }
    return total;
}

Image *readPPM(char *file_name){
    int i, j;
    FILE *file = fopen(file_name, "r"); 
    if (!file){ /*File could not be opened*/
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        return NULL;
    }

    char format[3];
    fscanf(file, "%s\n", format);
    if(strcmp(format, "P3")){ /*P3 is a colour image*/
        printf("error 1");
        return NULL;
    }

    Image *image = malloc(sizeof(Image));

    if(fscanf(file, "%u %u %u", &image->width, &image->height, &image->max) != 3){ /*Since fscanf returns how many vars were scanned*/
        printf("error 2");
        return NULL;
    }

    image->data = malloc(sizeof(Pixel *) * image->height);  

    for(i = 0; i < image->height; i++){
        image->data[i] = malloc(sizeof(Pixel) * image->width);
    }

    for(i = 0; i < image->height; i++){
        for(j = 0; j < image->width; j++){
            int pixels_read = fscanf(file, "%hhu %hhu %hhu", &image->data[i][j].r, &image->data[i][j].g, &image->data[i][j].b);
            if(pixels_read != 3){
                printf("error 2");
                return NULL;
            }
        }
    }
    fclose(file);
    return image;
}

int writePPM(char *file_name, Image *image){

    int i, j;
    FILE *file = fopen(file_name, "w");
    if (!file){
        fprintf(stderr, "Unable to open file \"%s\"\n", file_name);
        return -1;
    }

    fprintf(file, "P3\n");
    fprintf(file, "%u %u\n", image->width, image->height);
    fprintf(file, "%u\n", image->max);

    for (i = 0; i < image->height; i++){
        for (j = 0; j < image->width; j++){
            fprintf(file, "%d %d %d ", image->data[i][j].r, image->data[i][j].g, image->data[i][j].b);
        }
        fprintf(file, "\n");
    }    

    fclose(file);
    return 0;
}
