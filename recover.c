/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */
 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t BYTE;

int main(void)
{
    // Set size of title of out file
    char title [12];
    
    // Set name of first file
    int picName = 0;
    
    // Create a buffer to hold bytes read
    BYTE buffer [512];
    
    // Open file and set in pointer
    FILE* inptr = fopen("card.raw", "r");
    if (inptr == NULL)
    {
        printf("Could not open file");
        return 1;
    }
    
    // Set out file to NULL
    FILE* img = NULL;
    
    // Loop to read 512 bytes at a time of input file until end of file
    while (fread(buffer, sizeof(buffer), 1, inptr))
    {   
        // Check if new jpg is found    
        if  (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && 
            (buffer[3] == 0xe0 || buffer[3] == 0xe1))
        {
            if (img != NULL)
            {
                fclose(img);
            }
                // Creat, open, and write to new jpg 512 bytes at a time
                sprintf(title, "%03d.jpg", picName);
                img = fopen(title, "w");
                picName++;
                if (img == NULL)
                {
                    printf("Could not create file\n");
                    return 2;
                }

                fwrite(buffer, sizeof(buffer), 1, img);
       }
       
       // If in middle of writing jpg, continue writing   
       else if (img != NULL)
       {
            fwrite(buffer, sizeof(buffer), 1, img);
       }
            
    }
     
     // Close both files
     fclose(inptr);
     fclose(img);
     
     return 0;
}
