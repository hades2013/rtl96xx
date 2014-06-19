/*
 * html2asp.c
 *
 *  Created on: Jan 9, 2011
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	STATE_COPY = 0,
	STATE_DEMO_START,
	STATE_DEMO_END,
	STATE_REAL_START,
	STATE_REAL_CONV,
	STATE_REAL_END,
	STATE_SUBMIT,
	STATE_FORM,
};

char *func_list[256] = { NULL };

char current_file[128];

#define synerr(a, b) printf("[%s][%d@%s]"a, current_file, __LINE__,__FUNCTION__, b)


void split_string(char *s, char tok, char **ret, int *n)
{
	char *cp;
	cp = s;
	*n = 0;
	if(!*cp) return;
	*n = 1;
	while(*cp) {
//		printf("CP: %s\n", cp);
		ret[*n - 1] = cp;
		while(*cp && *cp != tok) cp ++;
		if(*cp == tok) *n = *n + 1;
		if(*cp == 0) return;
		*cp = 0;
		cp ++;
	}
	ret[*n - 1] = cp;
}

char *trim(char *input)
{
	char *cp = input;
	int i;
	while(*cp && *cp == ' ') cp ++;
	if(cp) {
		for(i = 0; i < strlen(cp); i ++)
			if(cp[i] == ' ') {
				cp[i] = 0;
				break;
			}
	}
	return cp;
}


void func_add(char *f)
{
	FILE *funcl, *tmp;
	char ftmp[256];
	char line[2048];
	char bak[2048];
	int argc, i, has = 0;
	char *argv[64], *file[64];

//	int l = 0;

	strcpy(ftmp, "func_tmp");
	funcl = fopen("func_list", "r");
	tmp = fopen(ftmp, "w");
	
	while(funcl && !feof(funcl)){
		fgets(line, sizeof(line), funcl);
		if (feof(funcl))break;
		strcpy(bak, line);
		split_string(line, ':', argv, &argc);
		if ((argc >= 2) && !strcmp(f, argv[0])){
			has = 1;
			split_string(argv[1], ',', file, &argc);
			for (i = 0; i < argc; i ++){
				if (!strcmp(file[i], current_file)){
					// already exists 
					break;
				}
			}
			// not found this file
			if (i >= argc){
				sprintf(line, ",%s\n", current_file);
				bak[strlen(bak) - 1] = '\0';
				strcat(bak, line);
			}
		}
		fprintf(tmp, "%s", bak);
	//	printf("%d>line=%s", l++, bak);
	}
	if (!has){
		fprintf(tmp, "%s:%s\n", f, current_file);
		//printf("line+%s:%s\n", f, current_file);
	}
	
	if (funcl)
		fclose(funcl);
	fclose(tmp);

	rename(ftmp, "func_list");
}


char *submit_conv(char *input)
{
	static char output[1024];
	char *cp, *pre, *post;

	cp = strstr(input, "'do_cmd.htm'");
	if(!cp) {
		synerr("Syntax error at [%s]\n", input);
		return input;
	}
	*cp = 0;
	cp += strlen("'do_cmd.htm'");
	post = cp;
	sprintf(output, "%s'goform/command'%s", input, post);
//	printf("OUT: %s\n", output);
	return output;
}

char *form_conv(char *input)
{
	static char output[1024];
	char *cp, *formname;

	cp = strstr(input, "name");
	if(!cp) {
		synerr("Syntax error at [%s]\n", input);
		return input;
	}

	while(*cp && *cp != '=') cp ++;
	if(!cp) {
		synerr("Syntax error at [%s]\n", input);
		return input;
	}
	cp ++;

	while(*cp && *cp == ' ') cp ++;
	if(!cp) {
		synerr("Syntax error at [%s]\n", input);
		return input;
	}
	formname = cp;
	while(*cp && *cp != ' ' && *cp != '>') cp ++;
	if(!cp) {
		synerr("Syntax error at [%s]\n", input);
		return input;
	}
	*cp = 0;
	sprintf(output, "<form name=%s action=goform/command method=post>\n", formname);
	return output;
}

char *asp_conv(char *input)
{
	static char output[1024], ps[64];
	char *cp, *ap, *fs, *pf;
	char func[256];
	char *parm[32];
	int items, i;

	pf = input;
	cp = strstr(input, "<%");
	if(!cp) return input;		// Empty line

	*cp = 0;
	cp += 2;

	while(*cp && *cp == ' ') cp ++;
	if(*cp == 0) {
		synerr("Syntax error at [%s]\n", input);
		return input;
	}

	fs = cp;

	while(*cp && *cp != '(') cp ++;
	if(*cp == 0) {
		synerr("Syntax error at [%s]\n", input);
		return input;
	}

	strncpy(func, fs, cp - fs);
	func[cp - fs] = 0;
//	printf("FUNCTION: %s\n", func);

	cp ++;

	while(*cp && *cp == ' ') cp ++;
	if(*cp == 0) {
		synerr("Syntax error at [%s]\n", input);
		return input;
	}

	ap = cp;

	while(*cp && *cp != ')') cp++;
	if(*cp == 0) {
		synerr("Syntax error at [%s]\n", input);
		return input;
	}
	*cp = 0;

//	printf("ARGUMENT: %s\n", ap);
	split_string(ap, ',', parm, &items);
//	for(i = 0; i < items; i ++) {
//		printf("ARG[%d]: [%s]\n", i, trim(parm[i]));
//	}

	if(*pf != 0)
	{
		sprintf(output, "%s<%c %s(", pf, '%', func);
	}
	else
	{
		sprintf(output, "<%c %s(", '%', func);
	}
	for(i = 0; i < items; i ++) {
		sprintf(ps, "\"%s\"", trim(parm[i]));
		strcat(output, ps);
		if(i < items - 1) strcat(output, ", ");
	}
	strcat(output, "); %>\n");

    func_add(func);
	
//	printf("OUTPUT: %s\n", output);

	return output;
}

void do_convert(FILE *src, FILE *dst)
{
	char line[1024];
	int state = STATE_COPY;

	while(!feof(src)) {
		fgets(line, sizeof(line), src);
		if(!feof(src)) {
//			printf("READLINE state=%d\n", state);
			if(strstr(line, "/*DEMO*/")) state = STATE_DEMO_START;
			if(strstr(line, "/*END_DEMO*/")) state = STATE_DEMO_END;
			if(strstr(line, "/*REAL")) state = STATE_REAL_START;
			if(strstr(line, "REAL*/")) state = STATE_REAL_END;
			if(strstr(line, "<form")) state = STATE_FORM;
			if(strstr(line, "subForm")) state = STATE_SUBMIT;
			switch(state) {
			case STATE_COPY:
//				printf("Writing: %s", line);
				fprintf(dst, "%s", line);
				continue;
			case STATE_DEMO_START:
//				printf("DEMO_START\n");
				continue;
			case STATE_DEMO_END:
//				printf("DEMO_END\n");
				state = STATE_COPY;
				continue;
			case STATE_REAL_START:
				state = STATE_REAL_CONV;
				continue;
			case STATE_REAL_END:
				state = STATE_COPY;
				continue;
			case STATE_REAL_CONV:
				fprintf(dst, "%s", asp_conv(line));
				continue;
			case STATE_FORM:
				fprintf(dst, "%s", form_conv(line));
				state = STATE_COPY;
				continue;
			case STATE_SUBMIT:
				fprintf(dst, "%s", submit_conv(line));
				state = STATE_COPY;
				continue;
			}

		}
	}
}

int main(int argc, char **argv)
{
	FILE *src, *dst;
	char *asp_name, *cp, out_fn[128];
	int i;

	if(argc < 3) {
		printf("Usage html2asp SOURCE DESTINATION_DIR\n");
		exit(1);
	}

	src = fopen(argv[1], "r");
	if(!src) {
		printf("Can not open source file: %s\n", argv[1]);
		exit(1);
	}

	cp = argv[1];
	for(i = strlen(argv[1]); i >= 0; i --) {
		if(argv[1][i] == '/') {
			cp = &argv[1][i];
			break;
		}
	}

	if(*cp == '/') cp ++;
	asp_name = cp;

	while(*cp && *cp != '.') cp ++;
	if(*cp == 0) {
		printf("Invalid file name: %s\n", argv[1]);
		exit(1);
	}

	*cp = 0;

	sprintf(current_file, "%s.asp", asp_name);

	sprintf(out_fn, "%s/%s.asp", argv[2], asp_name);

//	printf("Output file: %s\n", out_fn);

	dst = fopen(out_fn, "w");
	if(!dst) {
		printf("Can not open destination file: %s\n", argv[2]);
		exit(1);
	}

//	printf("Converting %s\n", argv[1]);
	do_convert(src, dst);
	fclose(src);
	fclose(dst);
	return 0;
}
