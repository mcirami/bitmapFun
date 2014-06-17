/**
 * copy.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize int infile outfile\n");
        return 1;
    }

    // remember filenames & convert 1st argument to integer
    int n = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];
    
    // Ensure 1st argument is between 1 and 100(inclusive)
    if (n < 1 || n > 100)
    {
        printf("Must enter a positive number between 1 and 100\n");
        return 2;
    }

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr,"Could not create %s.\n", outfile);
        return 4;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);
    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 5;
    }
 
    // determine padding for infile scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // Create bitmap headers for outfile
    BITMAPFILEHEADER out_bf;
    BITMAPINFOHEADER out_bi;
    
    // Set outfile headers equal to infile
    out_bf = bf;
    out_bi = bi;
    
    // Resize out file dimensions according to users input value
    out_bi.biWidth = n * bi.biWidth;
    out_bi.biHeight = n * bi.biHeight;
    
    // Determine padding for outfile
    int out_padding = (4 - (out_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // Set outfile size in info and file header
    out_bi.biSizeImage = (out_bi.biWidth * abs(out_bi.biHeight) * 3) + (out_padding * abs(out_bi.biHeight));
    out_bf.bfSize = out_bi.biWidth * abs(out_bi.biHeight) * 3 + 54 + (out_padding * abs(out_bi.biHeight));
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&out_bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    
    // write outfile's BITMAPINFOHEADER
    fwrite(&out_bi, sizeof(BITMAPINFOHEADER), 1, outptr);
    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate to resize vertically
        for (int y = 0; y < n; y++)  
        {      
            // iterate over pixels in scanline
            for (int j = 0; j < bi.biWidth; j++)
            {
                // temporary storage
                RGBTRIPLE triple;
                
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                
                // write RGB triple to outfile n times
                for (int x = 0; x < n; x++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
                // add padding if necessary
                for (int k = 0; k < out_padding; k++)
                {
                    fputc(0x00, outptr);
                }
                
                // Move cursor to beginning of infile current line to read again and resize vertically
                fseek(inptr, -bi.biWidth * 3, SEEK_CUR);
        }
            // Move cursor to end of line to go to next row in bitmap
            fseek(inptr, bi.biWidth * 3 + padding, SEEK_CUR);
     }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
