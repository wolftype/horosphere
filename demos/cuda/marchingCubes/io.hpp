/*
 * =====================================================================================
 *
 *       Filename:  io.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/08/2014 17:03:05
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef  io_INC
#define  io_INC


////////////////////////////////////////////////////////////////////////////////
// Load raw data from disk
////////////////////////////////////////////////////////////////////////////////
uchar *loadRawFile(const char *filename, int size)
{
	FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error opening file '%s'\n", filename);
        return 0;
    }

	uchar *data = (uchar *) malloc(size);
	size_t read = fread(data, 1, size, fp);
	fclose(fp);

    printf("Read '%s', %d bytes\n", filename, (int)read);

    return data;
}



#endif   /* ----- #ifndef io_INC  ----- */
