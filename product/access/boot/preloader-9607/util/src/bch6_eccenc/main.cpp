// -csz 2048 -bso 5 -bdo 2000 example

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "bch6.h"

static void
show_message(const char *n="eccenc") {
    fprintf(stderr, 
        "This is bch6 encoder.\n"
        "usage: %s --chunk-size <n> --bbi-swap-offset <n> --bbi-dma-offset <n> <input-file>\n"
        "\t--chunk-size <n>: chunk size in byte (eq -csz <n>)\n"
        "\t--bbi-swap-offset <n>: swap offset for bbi (eq -bso <n>)\n"
        "\t--bbi-dma-offset <n>: dma offset for bbi (eq -bdo <n>)\n"
        "\n"
        "\tNotice: --bbi-swap-offset and --bbi-dma-offset should be defined together.\n"
        "\tif neither --bbi-swap-offset nor --bbi-dma-offset is defined, no swap will be performanced\n"
        , n);
}
#define MINUS_ONE 0xffffffff
unsigned int chunk_size=MINUS_ONE;
unsigned int bbi_swap_offset=MINUS_ONE;
unsigned int bbi_dma_offset=MINUS_ONE;
unsigned int chunk_per_block=0;
unsigned int page_size=0;
unsigned int oob_size=0;
unsigned int ecc_size=0;
unsigned int unit_size=0;
const char *input_file_name=NULL;
bool enable_swap=true;
ecc_encoder_t *ecc_encoder=NULL;
static void
append_ecc_one_chunk(int output_handle, const unsigned char *chunk_data, unsigned int len, const unsigned char *spare) {
    unsigned char local_chunk_data[chunk_size+oob_size+ecc_size];
    unsigned int iteration=chunk_size/page_size;
    unsigned char local_spare[iteration*oob_size];
    unsigned char ecc[ecc_size];
	unsigned char * tmp=local_chunk_data;
	int chunkallone=0;
    
    if (len>chunk_size) len=chunk_size;
    
    // copy source into local buffer
    memset(local_chunk_data, 0xff, sizeof(local_chunk_data));
    memset(local_spare, 0xff, sizeof(local_spare));
    memset(ecc, 0xff, sizeof(ecc));

#if 1
	chunkallone=memcmp(tmp,chunk_data,chunk_size);
//	printf("chunkallone is %d\n",chunkallone);
	if(chunkallone==0){
		//printf(" find all is 0xff\n");
        for (unsigned i=0;i<chunk_size;i+=page_size)
            write(output_handle, local_chunk_data, unit_size);
        return;
	}
#endif

	
    if (chunk_data!=NULL) memcpy(local_chunk_data, chunk_data, len);
    if (spare!=NULL) memcpy(local_spare, spare, sizeof(local_spare));
    
    
	
    // compute ECC and write out
    if ((chunk_data==NULL)&&(spare==NULL)) {
        // write out 0xff for whole chunk
        for (unsigned i=0;i<chunk_size;i+=page_size)
            write(output_handle, local_chunk_data, unit_size);
    } else {
        // bbi swap
        if (enable_swap) {
            local_spare[bbi_swap_offset]=local_chunk_data[bbi_dma_offset];
            local_chunk_data[bbi_dma_offset]=0xff;
        }
        // write out one chunk
        for (unsigned i=0,j=0; i<chunk_size; i+=page_size, j+=oob_size) {
            ecc_encoder->encode_512B(ecc, local_chunk_data+i, local_spare+j);
            write(output_handle, local_chunk_data+i, page_size);
            write(output_handle, local_spare+j, oob_size);
            write(output_handle, ecc, ecc_size);
        }
    }
}

static int 
write_out() {
    char ofile[1024];
    int hi=open(input_file_name, O_RDONLY);
    if (hi<0) {
        fprintf(stderr, "unable to open input file (%s)\n", input_file_name);
        return 1;
    }
    sprintf(ofile, "%s.ecc", input_file_name);
    int ho=open(ofile, O_WRONLY|O_TRUNC|O_CREAT, 0666);
    if (ho<0) {
        fprintf(stderr, "unable to create output file (%s)\n", ofile);
        close(hi);
        return 1;
    }
    
    unsigned char chunk_data[chunk_size];
    unsigned int ci=0;
    while (1) {
        int r=read(hi, chunk_data, chunk_size);
        if (r>0) append_ecc_one_chunk(ho, chunk_data, r, NULL);
        if (chunk_per_block>0) ci=(ci+1) % chunk_per_block;
        if (r!=(int)chunk_size) break;
    }
    
    if ((chunk_per_block>0)&&(ci>0)) {
        memset(chunk_data, 0xff, chunk_size);
        for (;ci<chunk_per_block; ++ci) 
            append_ecc_one_chunk(ho, chunk_data, chunk_size, NULL);
    }
    
    close(ho);
    close(hi);
    return 0;
}
int
main(int argc, char *argv[]) {
    bool error_out=false;
    
    
    if (argc==1) {
        show_message();
        return 0;
    }
    while (*(++argv)!=NULL) {
        if ((strcmp(*argv, "--chunk-size")==0)||(strcmp(*argv, "-csz")==0)) {
            chunk_size=strtoul(*(++argv), NULL, 0);
        } else if ((strcmp(*argv, "--bbi-swap-offset")==0)||(strcmp(*argv, "-bso")==0)) {
            bbi_swap_offset=strtoul(*(++argv), NULL, 0);
        } else if ((strcmp(*argv, "--bbi-dma-offset")==0)||(strcmp(*argv, "-bdo")==0)) {
            bbi_dma_offset=strtoul(*(++argv), NULL, 0);
        } else if ((strcmp(*argv, "--chunk-per-block")==0)||(strcmp(*argv, "-cpb")==0)) {
            chunk_per_block=strtoul(*(++argv), NULL, 0);
        } else {
            input_file_name=*argv;
        }
    }
    
    if (input_file_name==NULL) {
        fprintf(stderr, "input file name was defined\n");
        error_out=true;
    }
    switch (chunk_size) {
        case 512:
        case 2048:
        case 4096: 
            break;
        default:
            fprintf(stderr, "chunk size of %d bytes is not supported\n", chunk_size);
            error_out=true;
            break;
    }
    if (error_out) {
        show_message();
        return 1;
    }
    // select bch6 by default
    ecc_encoder=new ecc_bch6_encode_t();
    page_size=ecc_encoder->get_page_size();
    oob_size=ecc_encoder->get_oob_size();
    ecc_size=ecc_encoder->get_ecc_size();
    unit_size=ecc_encoder->get_unit_size();

    // limitation
    unsigned int max_swap_offset = oob_size * chunk_size / page_size;
    
    if ((bbi_swap_offset==MINUS_ONE)&&(bbi_dma_offset==MINUS_ONE)) {
        enable_swap=false;
    } else {
        if (bbi_swap_offset==MINUS_ONE) {
            fprintf(stderr, "--bbi-swap-offset and --bbi-dma-offset should be defined together.\n");
            error_out=true;
        } else if (bbi_swap_offset>=max_swap_offset) {
            fprintf(stderr, "bbi-swap-offset should be between 0 and %d\n", oob_size-1);
            error_out=true;
        } else if (bbi_dma_offset==MINUS_ONE) {
            fprintf(stderr, "--bbi-swap-offset and --bbi-dma-offset should be defined together.\n");
            error_out=true;
        } else if (bbi_dma_offset>=chunk_size) {
            fprintf(stderr, "bbi-dma-offset should be less than the chunk size\n");
            error_out=true;
        }
    }
    if (error_out) {
        show_message();
        return 1;
    }
    
    return write_out();
}
