/* $OpenXM: OpenXM/src/asir99/parse/asir_sm_main.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
#include "ca.h"
#include "parse.h"

extern jmp_buf env;
extern int Asir_OperandStackPtr;

main() {
	char buf[BUFSIZ*100];
	char *data;
	int size,cur_sig;

	Asir_Start();

	while ( 1 ) {
		while ( 1 ) {
			cur_sig = sigsetmask(~0);
			if ( !fgets(buf,BUFSIZ*100,stdin) )
				exit(0);
			sigsetmask(cur_sig);
			switch ( buf[0] ) {
				case 'u':
					Asir_ExecuteString(buf+1);
					break;
				case 'o':
					printf("%s\n",Asir_PopString());
					break;
				case 's':
					Asir_Set(buf+1);
					break;
				case 'O':
					data = (char *)Asir_PopBinary(&size);
					Asir_PushBinary(size,data);
					break;
				default:
					break;
			}
		}
	}
}

KSstart(){}
KSexecuteString(){}
KSpopString(){}
