
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <arpa/inet.h>

#include "ufile.h"

/*
	TODO:
	ufile -c rootfs -i target.tar  -o  XX
	ufile -c boot -i elf.eee  -o  XX
	ufile -c file -n ddddd -i dddd -o XX
	ufile -c kernel -i bbb.tar -o XX
	ufile -c boot -i lll.tar -c kkk -i kkk -o xxx
*/

uint32_t checksum_32 (register const void * memory, register size_t extent, register uint32_t checksum)

{
	while (extent >= sizeof (checksum))
	{
		checksum ^= *(typeof (checksum) *)(memory);
		memory += sizeof (checksum);
		extent -= sizeof (checksum);
	}
	return (~checksum);
}



struct args_pair{
	int valid;
	uint16_t content;
	char *in_path;
	char *file_name;
	size_t size;
};

#define MAX_PAIR_ARGS  5

typedef enum{
	PHASE_CONTENT = 0,
	PHASE_FILENAME,
	PHASE_INPATH
}
phase_t;


static uint16_t get_content_type(char *cont)
{
	if (!strcmp(cont, "boot")){
		return UFILE_CONTENT_BOOT;
	}else if (!strcmp(cont, "rootfs")){
		return UFILE_CONTENT_ROOTFS;
	}else if (!strcmp(cont, "kernel")){
		return UFILE_CONTENT_KERNEL;
	}else if (!strcmp(cont, "file")){
		return UFILE_CONTENT_FILE;
	}else if (!strcmp(cont, "config")){
		return UFILE_CONTENT_CONFIG;
	}
	return 0;
}


static int get_pair_args(int argc, char **argv, struct args_pair *pairs)
{
	int i, cnt = 0; uint16_t c;
	phase_t phase = PHASE_CONTENT;


	for (i = 0; i < argc; i ++){
		if ((phase == PHASE_CONTENT) && !strcmp(argv[i], "-c")){
			i ++;
			if ((i < argc) && (argv[i][0] != '-')){
				if ((c = get_content_type(argv[i])) > 0){
					pairs[cnt].content = c;
					phase = PHASE_FILENAME;
					continue;
				}
			}
			return -1;
		}
		if ((phase == PHASE_FILENAME) && !strcmp(argv[i], "-n")){
			i ++;
			if ((i < argc) && (argv[i][0] != '-')){
				pairs[cnt].file_name = argv[i];
				phase = PHASE_INPATH;
				continue;
			}
			return -1;						
		}

		if (((phase == PHASE_FILENAME) || (phase == PHASE_INPATH)) && !strcmp(argv[i], "-i")){
			i ++;
			if ((i < argc) && (argv[i][0] != '-')){
				pairs[cnt].in_path = argv[i];
				pairs[cnt].valid = 1;
				cnt ++;
				phase = PHASE_CONTENT;
				continue;
			}
			return -1;						
		}
		// in any phase , if meet "-o" just skip
		if (!strcmp(argv[i], "-o")){
			i ++;
			if ((i < argc) && (argv[i][0] != '-')){
				continue;
			}
			break;
		}
		// default 
		return -1;
	}

	if (phase != PHASE_CONTENT){
		return -1;
	}
	
	return cnt;	
}


static char *get_output(int argc, char **argv)
{

	int i;
	for (i = 0; i < argc; i ++){
		if (!strcmp(argv[i], "-o")){
			i ++;
			if ((i < argc) && (argv[i][0] != '-')){
				return argv[i];
			}
			break;
		}
	}
	return NULL;
}

static size_t ufile_size(struct args_pair *pairs)
{
	size_t size = 0;
	struct stat buf;
	int fd;

	if (pairs->file_name != NULL){
		size += sizeof(ufile_data_hdr_t);
	}else {
		size += (size_t)(((ufile_data_hdr_t*)0)->file_name);
	}
	fprintf(stderr, "data_header size:%u\n", size);

	if ((fd = open(pairs->in_path, O_RDONLY)) < 0){
		fprintf(stderr, "Conuld not open file %s\n", pairs->in_path);
		return 0;
	}

	if (fstat(fd, &buf) < 0){
		fprintf(stderr, "Conuld not stat file %s\n", pairs->in_path);
		return 0;
	}
	pairs->size = buf.st_size;
	size += buf.st_size;
	close(fd);

	return size;
}


static size_t caculate_size(struct args_pair *pairs)
{
	size_t size = 0, tmp;
	int i;
	size += sizeof(ufile_header_t);
	for (i = 0; pairs[i].valid; i ++){
		tmp = ufile_size(&pairs[i]);
		if (tmp == 0) return 0;
		size += tmp;
	}		
	return size;
}


static size_t ufile_output(const char *path, const unsigned char *buffer, size_t size)
{	
	FILE *fp;
	size_t wlen;
	fp = fopen(path, "w");
	if (fp == NULL){
		fprintf(stderr, "Conuld not open file %s\n", path);		
		return 0;
	}
	
	wlen = fwrite(buffer, sizeof(unsigned char), size,  fp);
	if (wlen != size){
		fprintf(stderr, "File write failed! (wlen=%u:size=%u)\n", wlen, size);
	}
	
	fclose(fp);
	
	return wlen;
}

static size_t ufile_write_header(unsigned char *buf, struct args_pair *pairs)
{
	int i = 0, cnt = 0;
	uint16_t content = 0;
	
	ufile_header_t *hdr = (ufile_header_t *)buf;
	for (i = 0; pairs[i].valid; i ++){
		content |= pairs[i].content;
		cnt ++;
	}
	hdr->content = htons(content);
	strcpy(hdr->identify, UFILE_IDENTIFY);
	hdr->data_offset = htons(sizeof(ufile_header_t));
	hdr->flags = 0;
	hdr->file_num = cnt;
	hdr->target_type = TARGET_TYPE_EOC;
	hdr->target_model = TARGET_MODEL_CLT502;

	hdr->checksum = 0;
	hdr->total_length = 0;
	
	return sizeof(ufile_header_t);
}


static size_t ufile_write(unsigned char *buf, struct args_pair *pairs)
{
	ufile_data_hdr_t *hdr = (ufile_data_hdr_t *)buf;
	int fd;
	uint16_t offset;
	size_t rlen;
	char *pdata;

	hdr->content =  htons(pairs->content);
	hdr->flags = 0;
	hdr->file_type = 0;

	if (pairs->file_name != NULL){
		hdr->has_file_name = 1;
		strcpy(hdr->file_name, pairs->file_name);
		offset = sizeof(ufile_data_hdr_t); 
	}else {
		offset = (size_t)(((ufile_data_hdr_t*)0)->file_name);
	}
	pdata = buf + offset;	

	fd = open(pairs->in_path, O_RDONLY);
	if (fd < 0){
		fprintf(stderr, "Conuld not open file %s\n", pairs->in_path);
		return 0;		
	}

	rlen = read(fd, pdata, pairs->size);
	if (rlen != pairs->size){
		fprintf(stderr, "File '%s' read error (rlen=%u:size=%u)\n", pairs->in_path, rlen, pairs->size);
		close(fd);
		return 0;		
	}
	close(fd);

	hdr->data_offset = htons(offset);
	hdr->length = htonl(rlen + offset);
	
	return rlen + offset;
}


static size_t ufile_write_rootfs(unsigned char *buf, struct args_pair *pairs)
{
	return 0;



}

static size_t ufile_write_kernel(unsigned char *buf, struct args_pair *pairs)
{


return 0;

}

static size_t ufile_write_file(unsigned char *buf, struct args_pair *pairs)
{

return 0;


}


static size_t ufile_write_config(unsigned char *buf, struct args_pair *pairs)
{
	return 0;
}

static void usage(void)
{
	fprintf(stderr, "ufile -c CONTENT [ -n UFILE_NAME ] -i PATH [-c CONTENT [ -n IFILE_NAME ] -i PATH] ...\n");
	fprintf(stderr, "    CONTENT     is one of 'boot, rootfs, kernel, file, config'\n");
	fprintf(stderr, "    UFILE_NAME  is used to save in target board\n");
}




int main(int argc, char **argv)
{
	unsigned char *p;
	int i;
	ufile_header_t *hdr;
	int args;
	char *pout;
	uint32_t chksum;
	size_t size, wlen = 0;
	struct args_pair pairs[MAX_PAIR_ARGS];

	memset(pairs, 0, sizeof(pairs));

	if ((args = get_pair_args(argc - 1, &argv[1], pairs)) <= 0){
		usage();
		exit(1);
	}

	if ((pout = get_output(argc - 1, &argv[1])) == NULL){
		fprintf(stderr, "Conuld not find output path, set to 'ufile.bin'\n");
		pout = "ufile.bin";
	}

	for (i = 0; i < args; i ++){
		if (pairs[i].valid){
			fprintf(stderr, "pairs[%d] is valid\n", i);			
			fprintf(stderr, " content : %04X\n", pairs[i].content);			
			fprintf(stderr, " filename : %s\n", pairs[i].file_name);
			fprintf(stderr, " inpath : %s\n", pairs[i].in_path);			
		}else {
			fprintf(stderr, "pairs[%d] is invalid\n", i);
		}
	}



	if ((size = caculate_size(pairs)) <= 0){
		fprintf(stderr, "Could not Caculate the size!\n");
		exit(1);		
	}

	fprintf(stderr, "get size %u\n", size);

	p = (unsigned char*)malloc(size);
	if (p == NULL){
		fprintf(stderr, "Malloc failed!\n");
		exit(1);		
	}

	memset(p, 0, size);

	wlen = ufile_write_header(p, pairs);

	for (i = 0; pairs[i].valid; i ++){
		switch(pairs[i].content)
		{
			case UFILE_CONTENT_BOOT:
				wlen += ufile_write(p + wlen, &pairs[i]);
				break;
			case UFILE_CONTENT_KERNEL:
				wlen += ufile_write(p + wlen, &pairs[i]);				
				break;
			case UFILE_CONTENT_ROOTFS:
				wlen += ufile_write(p + wlen, &pairs[i]);					
				break;
			case UFILE_CONTENT_FILE:
				wlen += ufile_write(p + wlen, &pairs[i]);					
				break;
			case UFILE_CONTENT_CONFIG:
				wlen += ufile_write(p + wlen, &pairs[i]);					
				break;
			default:
				fprintf(stderr, "Unknown content type:%d\n", pairs[i].content);
				free(p);
				exit(1);
		}
	}	

	if (wlen != size){
		fprintf(stderr, "Ufile write error, length unmatch(wlen = %u:size = %u)\n", wlen, size);
		free(p);
		exit(1);		
	}

	hdr = (ufile_header_t *)p;

	hdr->total_length = htonl(wlen);
	hdr->checksum = htonl(checksum_32(p, wlen, 0));

	fprintf(stderr, "CHKSUM:%08X\n", hdr->checksum);
	
	ufile_output(pout, p, wlen);
	free(p);
	exit(0);
}



