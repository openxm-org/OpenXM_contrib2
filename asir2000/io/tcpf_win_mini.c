/* $OpenXM: OpenXM_contrib2/asir2000/io/tcpf_win_mini.c,v 1.1 2006/09/26 05:35:26 noro Exp $ */

#include "ca.h"
#include "parse.h"
#include "com.h"
#include <signal.h>
#include <string.h>
#include "ox.h"
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>
static int control_port,server_port;
int win_ox_launch();
void win_spawn_server(char *control_port_str,char *server_port_str);
void win_get_rootdir(rootdir);
char *win_pop_string();
static int launched=0;
extern int terminate;

int win_ox_launch()
{
	int cs,ss,cn,sn;
	char control_port_str[BUFSIZ];
	char server_port_str[BUFSIZ];

	if ( launched ) return 0;

	control_port_str[0] = 0;
	server_port_str[0] = 0;
	do { 
		generate_port(0,control_port_str);
		generate_port(0,server_port_str);
		cs = try_bind_listen(0,control_port_str);
		if ( cs < 0 ) continue;
		ss = try_bind_listen(0,server_port_str);
		if ( ss < 0 ) continue;
		win_spawn_server(control_port_str,server_port_str);
		cs = try_accept(0,cs);
		if ( cs < 0 ) continue;
		ss = try_accept(0,ss);
		if ( ss < 0 ) continue;
	} while ( cs < 0 || ss < 0 );

	/* client mode */
	cn = get_iofp(cs,control_port_str,0);
	sn = get_iofp(ss,server_port_str,0);
	/* get_iofp returns -1 if the laucher could not spawn the server */
	if ( sn < 0 ) {
		/* we should terminate the launcher */
		ox_send_cmd(cn,SM_shutdown); ox_flush_stream_force(cn);
		return -1;
	}

	/* register server to the server list */
	control_port = cn;
	server_port = sn;
	launched = 1;
	return 0;
}

void win_get_rootdir(char *dir);

void win_spawn_server(char *control_port_str,char *server_port_str)
{
	char AsirExe[BUFSIZ];
	char AsirExePath[BUFSIZ];
	char rootdir[BUFSIZ];
	char *av[BUFSIZ];

	win_get_rootdir(rootdir);
//	strcpy(rootdir,"c:\\program files\\asir");
	sprintf(AsirExe,"%s\\bin\\engine.exe",rootdir);
	sprintf(AsirExePath,"\"%s\"",AsirExe);
	av[0] = "ox_launch";
	av[1] = "127.0.0.1";
	av[2] = "0";
	av[3] = control_port_str;
	av[4] = server_port_str;
	av[5] = "ox_asir_wfep";
	av[6] = "1";
	av[7] = AsirExePath;
	av[8] = 0;

	_spawnv(_P_NOWAIT,AsirExe,av);
}

void win_get_rootdir(char *dir)
{
	char *slash;

	GetCurrentDirectory(BUFSIZ,dir);
	slash = strrchr(dir,'\\');
	if ( slash ) *slash = 0;
}

char *win_pop_string()
{
	Obj obj;
	int id,serial;

	if ( !launched ) return "";
	serial = ox_recv(server_port,&id,&obj);
	if ( serial < 0 ) return 0;

	if ( id == OX_DATA ) {
		if ( obj && OID(obj) == O_STR ) return BDY((STRING)obj);
		else return "";
	} else if ( id == OX_SYNC_BALL ) {
		ox_send_sync(server_port);
		ox_flush_stream_force(server_port);
		return "";
	} else return "";
}

void win_execute_string(char *cmd)
{
	struct oSTRING str;

	if ( !launched ) return;
	OID(&str) = O_STR;
	BDY(&str) = cmd;
	ox_send_data(server_port,&str);
	ox_send_cmd(server_port,SM_executeStringByLocalParser);
	ox_send_cmd(server_port,SM_popString);
	ox_flush_stream_force(server_port);
}

void ox_reset()
{
	USINT t;
	int id;
	Obj obj;

	if ( !launched ) return;
	ox_send_cmd(control_port,SM_control_reset_connection);
	ox_flush_stream_force(control_port);
	ox_recv(control_port,&id,&obj); t = (USINT)obj;
	Sleep(100);
	ox_send_cmd(server_port,SM_nop);
	ox_flush_stream_force(server_port);
}

void ox_shutdown()
{
	if ( !launched ) return;
	terminate = 1;
	ox_send_cmd(server_port,SM_shutdown);
	Sleep(100);
	ox_send_cmd(control_port,SM_shutdown);
}

