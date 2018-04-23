#ifndef __CD_HTTP_CODE_H_
#define __CD_HTTP_CODE_H_

#define HTTP_CODE_SIZE 1024
void cd_http_code_200(char *buf, char *file_type, int *con_len);
void cd_http_code_500(char *buf);
void cd_http_code_400(char *buf);

#endif
