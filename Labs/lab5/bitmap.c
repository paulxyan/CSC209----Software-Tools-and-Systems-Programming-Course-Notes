#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"

/*
 * Read in the location of the pixel array, the image width, and the image 
 * height in the given bitmap file.
 * Use fseek to move the file position. Don't read the whole file.
 */
void read_bitmap_metadata(FILE *image, int *pixel_array_offset, int *width, int *height) {
    // TODO: Complete this function

    if (fseek(image, 10, SEEK_SET) == -1) {

        fprintf(stderr, "fseek error");
        exit(1);
    }

    fwrite(pixel_array_offset, sizeof(int), 1, image);

    if (fseek(image, 18, SEEK_SET) == -1) {

        fprintf(stderr, "fseek error");
        exit(1);
    }

    fwrite(width, sizeof(int), 1, image);

    if (fseek(image, 21, SEEK_SET) == -1) {

        fprintf(stderr, "fseek error");
        exit(1);
    }

    fwrite(height, sizeof(int), 1, image);

}

/*
 * Read in pixel array by following these instructions:
 *
 * 1. First, allocate space for m `struct pixel *` values, where m is the 
 *    height of the image.  Each pointer will eventually point to one row of 
 *    pixel data.
 * 2. For each pointer you just allocated, initialize it to point to 
 *    heap-allocated space for an entire row of pixel data.
 * 3. Use the given file and pixel_array_offset to initialize the actual 
 *    struct pixel values. Assume that `sizeof(struct pixel) == 3`, which is 
 *    consistent with the bitmap file format.
 *    NOTE: We've tested this assumption on the Teaching Lab machines, but 
 *    if you're trying to work on your own computer, we strongly recommend 
 *    checking this assumption!
 * 4. Hint: Try reading a whole row of pixels in one fread call
 * 5. Return the address of the first `struct pixel *` you initialized.
 */
struct pixel **read_pixel_array(FILE *image, int pixel_array_offset, int width, int height) {
    // TODO: Complete this function

    
	
    struct pixel **pixel_array = malloc(sizeof(struct pixel *) * height);

    for (int i = 0; i < height; height ++) {

        pixel_array[i] = malloc(sizeof(struct pixel) * width);

    }

    fseek(image, pixel_array_offset, SEEK_SET);

    int blue1, green1, red1;

    for (int i = 0; i < height; i ++) {


        for (int j = 0; j < width; j ++) {
            
            fread(&blue1, sizeof(char), 1, image);
            fread(&green1, sizeof(char), 1, image);
            fread(&red1, sizeof(char), 1, image);

            pixel_array[i][j].blue = blue1;
            pixel_array[i][j].green = green1;
            pixel_array[i][j].red = red1;
            

        }

    }

	return pixel_array;

	}

   


/*
 * Print the blue, green, and red colour values of a pixel.
 * You should not change this function.
 */
void print_pixel(struct pixel p) {
    printf("(%u, %u, %u)\n", p.blue, p.green, p.red);
}
