/*
 * asp_variable.h
 *
 *  Created on: Jan 10, 2011
 *      Author: root
 */

#ifndef ASP_VARIABLE_H_
#define ASP_VARIABLE_H_

typedef enum{
	VAR_TYPE_INT = 0,
	VAR_TYPE_STRING,
	VAR_TYPE_DATETIME,
	VAR_TYPE_STR_ARRAY,
	VAR_TYPE_STR_ARRAY2,
}
asp_variable_type_t;

typedef int (*var_func_t)(int argc, char_t **argv);

typedef struct {
	char *name;
	union {
		int integer;
		char *string;
		time_t time;
	} data;
	int type;
	var_func_t func;
	char *nvram_name;
}
asp_variable_table_t;


#define CGI_OK  0
#define CGI_ERR_ARGS  1
#define CGI_IPC_FAIL   2
#define CGI_IPC_TIMEOUT 3	
#define CGI_NOMEM    4
#define CGI_NOCMD  5
#define CGI_UNKNOWN_CMD  6

#define MSG_REBOOT "reboot"
#define MSG_UNKNOWN "unknown"
#define MSG_FAIL  "fail"
#define MSG_UPGRADE "upgrade"
#define MSG_RESTORE "restore"
#define MSG_OK "ok"


typedef struct cgi_result{
	char *msg;
	int error;	
	int nowait;
	int delay; 
}
cgi_result_t;

#define SET_RESULT(res, m, e, n, d)  do {\
									(res)->msg = (m); \
									(res)->error = (e); \
									(res)->nowait = (n); \
									(res)->delay = (d); \
								}while(0)

#define IPC_RESULT(pack, res) do { \
		if (pack){ \
			if (pack->hdr.status == IPC_STATUS_OK){ \
				SET_RESULT(res, MSG_OK, CGI_OK, 0, 0); \
			}else { \
				SET_RESULT(res, MSG_FAIL, CGI_IPC_FAIL, 0, 0); \
			} \
		}else { \
			SET_RESULT(res, MSG_FAIL, CGI_IPC_TIMEOUT, 0, 0); \
		} \
	}while(0)

#define IPC_RESULT_NOWAIT(pack, res) do { \
					if (pack){ \
						if (pack->hdr.status == IPC_STATUS_OK){ \
							SET_RESULT(res, MSG_OK, CGI_OK, 1, 0); \
						}else { \
							SET_RESULT(res, MSG_FAIL, CGI_IPC_FAIL, 0, 0); \
						} \
					}else { \
						SET_RESULT(res, MSG_FAIL, CGI_IPC_TIMEOUT, 0, 0); \
					} \
				}while(0)


int asp_get_variable(int eid, webs_t wp, int argc, char_t **argv);

#endif /* ASP_VARIABLE_H_ */
