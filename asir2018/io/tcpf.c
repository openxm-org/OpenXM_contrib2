/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 * $OpenXM: OpenXM_contrib2/asir2018/io/tcpf.c,v 1.4 2020/01/09 01:47:41 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include "com.h"
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#if !defined(VISUAL) && !defined(__MINGW32__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#endif

#include "ox.h"

#if defined(VISUAL) || defined(__MINGW32__)
#include <winsock2.h>
#include <process.h>
#endif

#define INIT_TAB_SIZ 64
#define OX_XTERM "ox_xterm"

#if !defined(_PA_RISC1_1)
#define RSH "rsh"
#else
#define RSH "remsh"
#endif

static struct m_c {
  int m,c,af_unix;
} *m_c_tab;

static int m_c_i,m_c_s;
extern int I_am_server;

extern int little_endian;

#if defined(MPI)
extern int mpi_nprocs;
#define valid_mctab_index(ind)\
if((ind)<0||(ind)>=mpi_nprocs){error("invalid server id");}
#define check_valid_mctab_index(ind)\
if((ind)<0||(ind)>=mpi_nprocs){(ind)=-1;}
#else
#define valid_mctab_index(ind)\
if((ind)<I_am_server||(ind)>=m_c_i||\
((m_c_tab[ind].m<0)&&(m_c_tab[ind].c<0))){error("invalid server id");}
#define check_valid_mctab_index(ind)\
if((ind)<I_am_server||(ind)>=m_c_i||\
((m_c_tab[ind].m<0)&&(m_c_tab[ind].c<0))){(ind)=-1;}
#endif

struct IOFP iofp_102[MAXIOFP];
int nserver_102;
int myrank_102;

int register_102(int s,int rank, int is_master);

int register_server(int af_unix,int m,int c,int ind);
int get_mcindex(int);

void Pox_mpi_myid(Z *rp);
void Pox_mpi_nprocs(Z *rp);
void Pox_get_serverinfo(NODE arg,LIST *rp);
void Pgenerate_port(NODE arg,Obj *rp);
void Pox_reset_102(NODE arg,Z *rp);
void Pox_get_rank_102(LIST *rp);
void Pox_set_rank_102(NODE arg,Z *rp);
void Pox_tcp_accept_102(NODE arg,Z *rp);
void Pox_tcp_connect_102(NODE arg,Z *rp);
void Ptry_bind_listen(NODE arg,Z *rp);
void Ptry_connect(NODE arg,Z *rp);
void Ptry_accept(NODE arg,Z *rp);
void Pregister_server(NODE arg,Z *rp);
void Pox_launch_generic(NODE arg,Z *rp);
void Pox_launch(NODE arg,Obj *rp);
void Pox_launch_nox(NODE arg,Obj *rp);
void Pox_select(NODE arg,LIST *rp);
void Pox_flush(NODE arg,Z *rp);
void Pox_send_raw_cmo(NODE arg,Obj *rp);
void Pox_recv_raw_cmo(NODE arg,Obj *rp);
void Pox_send_102(NODE arg,Obj *rp);
void Pox_recv_102(NODE arg,Obj *rp);
void Pox_bcast_102(NODE arg,Obj *rp);
void Pox_reduce_102(NODE arg,Obj *rp);
void Pox_push_local(NODE arg,Obj *rp);
void Pox_push_cmo(NODE arg,Obj *rp);
void Pox_push_vl(NODE arg,Obj *rp);
void Pox_pop_local(NODE arg,Obj *rp);
void Pox_pop_cmo(NODE arg,Obj *rp);
void Pox_pop0_local(NODE arg,Obj *rp);
void Pox_pop0_cmo(NODE arg,Obj *rp);
void Pox_pop0_string(NODE arg,STRING *rp);
void Pox_pop_string(NODE arg,Obj *rp);
void Pox_get(NODE arg,Obj *rp);
void Pox_pops(NODE arg,Obj *rp);
void Pox_execute_function(NODE arg,Obj *rp);
void Pox_setname(NODE arg,Obj *rp);
void Pox_evalname(NODE arg,Obj *rp);
void Pox_execute_string(NODE arg,Obj *rp);
void Pox_rpc(NODE arg,Obj *rp);
void Pox_cmo_rpc(NODE arg,Obj *rp);
void Pox_reset(NODE arg,Z *rp);
void Pox_intr(NODE arg,Z *rp);
void Pox_sync(NODE arg,Z *rp);
void Pox_shutdown(NODE arg,Z *rp);
void Pox_push_cmd(NODE arg,Z *rp);
void ox_launch_generic(char *host,char *launcher,char *server,
    int use_unix,int use_ssh,int use_x,int conn_to_serv,Z *rp);


struct ftab tcp_tab[] = {
  {"ox_send_raw_cmo",Pox_send_raw_cmo,2},
  {"ox_recv_raw_cmo",Pox_recv_raw_cmo,1},
  {"ox_get_serverinfo",Pox_get_serverinfo,-1},
  {"generate_port",Pgenerate_port,-1},

  /* from master to client */
  {"ox_set_rank_102",Pox_set_rank_102,3},
  {"ox_tcp_accept_102",Pox_tcp_accept_102,3},
  {"ox_tcp_connect_102",Pox_tcp_connect_102,4},
  {"ox_reset_102",Pox_reset_102,1},

  {"ox_get_rank_102",Pox_get_rank_102,0},
  {"ox_send_102",Pox_send_102,2},
  {"ox_recv_102",Pox_recv_102,1},
  {"ox_bcast_102",Pox_bcast_102,-2},
  {"ox_reduce_102",Pox_reduce_102,-3},

  {"try_bind_listen",Ptry_bind_listen,1},
  {"try_connect",Ptry_connect,2},
  {"try_accept",Ptry_accept,2},
  {"register_server",Pregister_server,4},
  {"ox_flush",Pox_flush,1},
  {"ox_launch",Pox_launch,-3},
  {"ox_launch_nox",Pox_launch_nox,-3},
  {"ox_launch_generic",Pox_launch_generic,7},
  {"ox_shutdown",Pox_shutdown,1},

  {"ox_rpc",Pox_rpc,-99999999},
  {"ox_cmo_rpc",Pox_cmo_rpc,-99999999},

  {"ox_sync",Pox_sync,1},
#if defined(MPI)
  {"ox_mpi_myid",Pox_mpi_myid,0},
  {"ox_mpi_nprocs",Pox_mpi_nprocs,0},
#endif
#if !defined(MPI)
  {"ox_reset",Pox_reset,-2},
  {"ox_intr",Pox_intr,1},
  {"ox_select",Pox_select,-2},
#endif

  {"ox_pop_string",Pox_pop_string,1},
  {"ox_pop0_string",Pox_pop0_string,1},
  {"ox_pop_local",Pox_pop_local,1},
  {"ox_pop0_local",Pox_pop0_local,1},
  {"ox_pop_cmo",Pox_pop_cmo,1},
  {"ox_pop0_cmo",Pox_pop0_cmo,1},
  {"ox_get",Pox_get,-1},
  {"ox_pops",Pox_pops,-2},

  {"ox_push_vl",Pox_push_vl,1},
  {"ox_push_local",Pox_push_local,-99999999},
  {"ox_push_cmo",Pox_push_cmo,-99999999},
  {"ox_push_cmd",Pox_push_cmd,2},

  {"ox_setname",Pox_setname,2},
  {"ox_evalname",Pox_evalname,2},

  {"ox_execute_string",Pox_execute_string,2},
  {"ox_execute_function",Pox_execute_function,3},

  {0,0,0},
};

extern struct IOFP iofp[];
extern MATHCAP my_mathcap;
extern int ox_exchange_mathcap;

Obj asir_pop_one();
void asir_push_one(Obj);

#if defined(MPI)
extern int mpi_myid, mpi_nprocs;

void Pox_mpi_myid(Z *rp)
{
  STOZ(mpi_myid,*rp);
}

void Pox_mpi_nprocs(Z *rp)
{
  STOZ(mpi_nprocs,*rp);
}
#endif

void Pox_get_serverinfo(NODE arg,LIST *rp)
{
  int i,c;
  Z s_id;
  NODE t,n0,n;
  LIST list,l;

  if ( !arg ) {
    for ( i = I_am_server?1:0, n0 = 0; i < m_c_i; i++ )
      if ( (m_c_tab[i].m>=0) || (m_c_tab[i].c>=0) ) {
        c = m_c_tab[i].c;
        ox_get_serverinfo(c,&list);
        STOZ(i,s_id);
        t = mknode(2,s_id,list);
        MKLIST(l,t);
        NEXTNODE(n0,n);
        BDY(n) = (pointer)l;
      }
    if ( n0 )
      NEXT(n) = 0;
    MKLIST(*rp,n0);  
  } else {
    i = ZTOS((Q)ARG0(arg));
    if ( i >= 0 && i < m_c_i && ((m_c_tab[i].m>=0) || (m_c_tab[i].c>=0)) )
      ox_get_serverinfo(m_c_tab[i].c,rp);
    else {
      MKLIST(*rp,0);
    }
  }
}

/*
  if noarg or arg[0]==0 then use IP socket and return a port number
  else use UNIX socket and return a string which represents a path name
*/

void Pgenerate_port(NODE arg,Obj *rp)
{
  char port_str[BUFSIZ];
  int port;
  char *s;
  STRING str;
  Z q;

  if ( !arg || !ARG0(arg) ) {
    generate_port(0,port_str);
    port = atoi(port_str);
    STOZ(port,q);
    *rp = (Obj)q;
  } else {
    generate_port(1,port_str);
    s = (char *)MALLOC(strlen((char *)port_str)+1);
    strcpy(s,port_str);
    MKSTR(str,s);
    *rp = (Obj)str;
  }
}

void Pox_reset_102(NODE arg,Z *rp)
{
  int s;
  int  index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  ox_send_cmd(s,SM_reset_102);
  ox_flush_stream_force(s);
  *rp = 0;
}

void Pox_get_rank_102(LIST *rp)
{
  Z n,r;
  NODE node;

  STOZ(nserver_102,n);
  STOZ(myrank_102,r);
  node = mknode(2,n,r);
  MKLIST(*rp,node);
}

void Pox_set_rank_102(NODE arg,Z *rp)
{
  Z nserver,rank;
  int s;
  int  index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  nserver = (Z)ARG1(arg);  
  rank = (Z)ARG2(arg);  
  ox_send_data(s,nserver);
  ox_send_data(s,rank);
  ox_send_cmd(s,SM_set_rank_102);
  ox_flush_stream_force(s);
  *rp = 0;
}

/* ox_tcp_accept_102(server,port,rank) */

void Pox_tcp_accept_102(NODE arg,Z *rp)
{
  int s;
  int  index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;

  ox_send_data(s,ARG1(arg));
  ox_send_data(s,ARG2(arg));
  ox_send_cmd(s,SM_tcp_accept_102);
  ox_flush_stream_force(s);
  *rp = 0;
}

/*
 ox_tcp_connect_102(server,host,port,rank)
*/

void Pox_tcp_connect_102(NODE arg,Z *rp)
{
  int s;
  int  index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;

  ox_send_data(s,ARG1(arg));
  ox_send_data(s,ARG2(arg));
  ox_send_data(s,ARG3(arg));
  ox_send_cmd(s,SM_tcp_connect_102);
  ox_flush_stream_force(s);
  *rp = 0;  
}

/*
 try_bind_listen(port)
*/

void Ptry_bind_listen(NODE arg,Z *rp)
{
  char port_str[BUFSIZ];
  int port,s,use_unix;

  if ( IS_CYGWIN || !ARG0(arg) || NUM(ARG0(arg)) ) {
    port = ZTOS((Q)ARG0(arg));
    sprintf(port_str,"%d",port);
    use_unix = 0;
  } else {
    strcpy(port_str,BDY((STRING)ARG0(arg)));
    use_unix = 1;
  }
  s = try_bind_listen(use_unix,port_str);
  STOZ(s,*rp);
}

/*
 try_connect(host,port)
*/

void Ptry_connect(NODE arg,Z *rp)
{
  char port_str[BUFSIZ];
  char *host;
  int port,s,use_unix;

  if ( IS_CYGWIN || !ARG1(arg) || NUM(ARG1(arg)) ) {
    port = ZTOS((Q)ARG1(arg));
    sprintf(port_str,"%d",port);
    use_unix = 0;
  } else {
    strcpy(port_str,BDY((STRING)ARG1(arg)));
    use_unix = 1;
  }
  host = BDY((STRING)ARG0(arg));
  s = try_connect(use_unix,host,port_str);
  STOZ(s,*rp);
}

/*
 try_accept(sock,port)
*/

void Ptry_accept(NODE arg,Z *rp)
{
  int use_unix,s;

  if ( IS_CYGWIN || !ARG1(arg) || NUM(ARG1(arg)) )
    use_unix = 0;
  else
    use_unix = 1;
  s = try_accept(use_unix,ZTOS((Q)ARG0(arg)));
  STOZ(s,*rp);
}

/*
 register_server(cs,cport,ss,sport)
*/

void Pregister_server(NODE arg,Z *rp)
{
  int cs,ss,cn,sn,ind,use_unix,id;
  char cport_str[BUFSIZ],sport_str[BUFSIZ];
  Obj obj;
  MATHCAP server_mathcap;

  cs = ZTOS((Q)ARG0(arg));    
  ss = ZTOS((Q)ARG2(arg));    
  if ( IS_CYGWIN || !ARG1(arg) || NUM(ARG1(arg)) ) {
    sprintf(cport_str,"%ld",ZTOS((Q)ARG1(arg)));
    use_unix = 0;
  } else {
    strcpy(cport_str,BDY((STRING)ARG1(arg)));
    use_unix = 1;
  }
  if ( !ARG3(arg) || NUM(ARG3(arg)) ) {
    if ( use_unix )
      error("register_server : the protocol should conincide for two sockets");
    sprintf(sport_str,"%ld",ZTOS((Q)ARG3(arg)));
  } else {
    if ( !use_unix )
      error("register_server : the protocol should conincide for two sockets");
    strcpy(sport_str,BDY((STRING)ARG3(arg)));
  }

  /* client mode */
  cn = get_iofp(cs,cport_str,0);
  sn = get_iofp(ss,sport_str,0);
  /* get_iofp returns -1 if the laucher could not spawn the server */
  if ( sn < 0 ) {
    /* we should terminate the launcher */
    ox_send_cmd(cn,SM_shutdown); ox_flush_stream_force(cn);
    STOZ(-1,*rp);
    return;
  }

  /* register server to the server list */
  ind = register_server(use_unix,cn,sn,-1);

  if ( ox_exchange_mathcap ) {
    /* request remote mathcap */
    ox_send_cmd(sn,SM_mathcap);
    ox_send_cmd(sn,SM_popCMO);
    ox_flush_stream_force(sn);
    ox_recv(sn,&id,&obj); server_mathcap = (MATHCAP)obj;
    store_remote_mathcap(sn,server_mathcap);
  
    /* send my mathcap */
    create_my_mathcap("asir");
    ox_send_data(sn,my_mathcap);
    ox_send_cmd(sn,SM_setMathcap);
  }
  /* return the server id */
  STOZ(ind,*rp);
}

#if !defined(VISUAL) && !defined(__MINGW32__)
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

static int find_executable(char *);
static int find_executable_main(char *);

static int find_executable(char *com)
{
  char *c,*s;
  int len;
  char dir[BUFSIZ],path[BUFSIZ];

  for ( s = (char *)getenv("PATH"); s; ) {
    c = (char *)index(s,':');
    if ( c ) {
      len = c-s;
      strncpy(dir,s,len); s = c+1; dir[len] = 0;
    } else {
      strcpy(dir,s); s = 0;
    }
    sprintf(path,"%s/%s",dir,com);
    if ( find_executable_main(path) )
      return 1;
  }
  return 0;
}

static int find_executable_main(char *file)
{
  struct stat buf;

  if ( stat(file,&buf) || (buf.st_mode & S_IFDIR) )
    return 0;
  if ( access(file,X_OK) )
    return 0;
  else
    return 1;
}

#endif
/*
  ox_launch_generic(host,launcher,server,use_unix,use_ssh,use_x,conn_to_serv)

  Input
  host: hostname on which servers run
  launcher: path name of the launcher
  server: path name of the server
  use_unix: use UNIX domain socket if 1
  use_ssh: use ssh if 1
  use_x: use X11 facilities if 1
  conn_to_serv: connect to server if 1
*/

void Pox_launch_generic(NODE arg,Z *rp)
{
  int use_unix,use_ssh,use_x,conn_to_serv;
  char *host,*launcher,*server;
  Z ret;

  host = (arg&&ARG0(arg))?BDY((STRING)ARG0(arg)):0;
  launcher = BDY((STRING)ARG1(arg));
  server = BDY((STRING)ARG2(arg));
  use_unix = !IS_CYGWIN && ARG3(arg) ? 1 : 0;
  use_ssh = ARG4(arg) ? 1 : 0;
  use_x = ARG5(arg) ? 1 : 0;
  conn_to_serv = ZTOS((Q)ARG6(arg));
  if ( !IS_CYGWIN && !host )
    use_unix = 1;
  ox_launch_generic(host,launcher,server,
    use_unix,use_ssh,use_x,conn_to_serv,&ret);
  *rp = ret;
}

#if 0
void ox_launcher_101_generic(char *host,char *launcher,
    int use_unix,int use_ssh,int use_x,int conn_to_serv,Z *rp)
{
  int cs,cn,ind,id;
  char control_port_str[BUFSIZ];
  Obj obj;

#if !defined(VISUAL) && !defined(__MINGW32__)
  if ( use_unix && !find_executable("xterm") ) use_x = 0;
#endif
  control_port_str[0] = 0;
  do { 
    generate_port(use_unix,control_port_str);
    if ( conn_to_serv ) {
      spawn_server_101(host,launcher,
        use_unix,use_ssh,use_x,conn_to_serv,
          control_port_str);
      cs = try_connect(use_unix,host,control_port_str);
    } else {
      cs = try_bind_listen(use_unix,control_port_str);
      if ( cs < 0 ) continue;
      spawn_laucher_101(host,launcher,
        use_unix,use_ssh,use_x,conn_to_serv,
          control_port_str);
      cs = try_accept(use_unix,cs);
    }
  } while ( cs < 0 );

  /* client mode */
  cn = get_iofp(cs,control_port_str,0);

  /* register server to the server list */
  ind = register_server_101(use_unix,cn);

  STOZ(ind,*rp);
}
#endif

void ox_launch_generic(char *host,char *launcher,char *server,
    int use_unix,int use_ssh,int use_x,int conn_to_serv,Z *rp)
{
  int cs,ss,cn,sn,ind,id;
  char control_port_str[BUFSIZ];
  char server_port_str[BUFSIZ];
  Obj obj;
  MATHCAP server_mathcap;
    Z value;
    char *key;
    int fd=-1;
    NODE opt,n0;

    if ( current_option ) {
        for ( opt = current_option; opt; opt = NEXT(opt) ) {
            n0 = BDY((LIST)BDY(opt));
            key = BDY((STRING)BDY(n0));
            value = (Z)BDY(NEXT(n0));
            if ( !strcmp(key,"fd") && value ) {
                fd = ZTOS(value);
                break;
            }
        }
    }
    if (!available_mcindex(fd)) {
        STOZ(-1,*rp);
        return;
    }
#if !defined(VISUAL) && !defined(__MINGW32__)
  if ( use_unix && !find_executable("xterm") ) use_x = 0;
#endif
  control_port_str[0] = 0;
  server_port_str[0] = 0;
  do { 
    generate_port(use_unix,control_port_str);
    generate_port(use_unix,server_port_str);
    if ( !conn_to_serv ) {
      cs = try_bind_listen(use_unix,control_port_str);
      if ( cs < 0 ) continue;
      ss = try_bind_listen(use_unix,server_port_str);
      if ( ss < 0 ) continue;
    }
    spawn_server(host,launcher,server,
      use_unix,use_ssh,use_x,conn_to_serv,
        control_port_str,server_port_str);
    if ( conn_to_serv ) {
      cs = try_connect(use_unix,host,control_port_str);
      if ( cs < 0 ) continue;
      ss = try_connect(use_unix,host,server_port_str);
      if ( ss < 0 ) continue;
    } else {
      cs = try_accept(use_unix,cs);
      if ( cs < 0 ) continue;
      ss = try_accept(use_unix,ss);
      if ( ss < 0 ) continue;
    }
  } while ( cs < 0 || ss < 0 );

  /* client mode */
  cn = get_iofp(cs,control_port_str,0);
  sn = get_iofp(ss,server_port_str,0);
  /* get_iofp returns -1 if the laucher could not spawn the server */
  if ( sn < 0 ) {
    /* we should terminate the launcher */
    ox_send_cmd(cn,SM_shutdown); ox_flush_stream_force(cn);
    STOZ(-1,*rp);
    return;
  }

  /* register server to the server list */
  ind = register_server(use_unix,cn,sn,fd);

  if ( ox_exchange_mathcap ) {
    /* request remote mathcap */
    ox_send_cmd(sn,SM_mathcap);
    ox_send_cmd(sn,SM_popCMO);
    ox_flush_stream_force(sn);
    ox_recv(sn,&id,&obj); server_mathcap = (MATHCAP)obj;
    store_remote_mathcap(sn,server_mathcap);
  
    /* send my mathcap */
    create_my_mathcap("asir");
    ox_send_data(sn,my_mathcap);
    ox_send_cmd(sn,SM_setMathcap);
  }
  /* return the server id */
  STOZ(ind,*rp);
}

#if defined(__CYGWIN32__)
static void bslash2slash(char *buf)
{
  char *p;

  for ( p = buf; *p; p++ )
    if ( *p == '\\' )
      *p = '/';
}

static int get_start_path(char *buf)
{
  static char start_path[BUFSIZ];
  static int start_initialized = 0;
  char name[BUFSIZ];

  if ( start_initialized ) {
    strcpy(buf,start_path);
    return 1;
  }

  /* Windows98 */
  strcpy(buf,"c:\\windows\\command\\start.exe");
  cygwin_conv_to_full_posix_path(buf,name);
  if ( !access(name,X_OK) ) {
    bslash2slash(buf);
    strcpy(start_path,buf);
    start_initialized  = 1;
    return 1;
  }

  /* Windows2000 */
  strcpy(buf,"c:\\winnt\\system32\\start.exe");
  cygwin_conv_to_full_posix_path(buf,name);
  if ( !access(name,X_OK) ) {
    bslash2slash(buf);
    strcpy(start_path,buf);
    start_initialized  = 1;
    return 1;
  }

  strcpy(buf,"c:\\winnt\\system32\\cmd.exe");
  cygwin_conv_to_full_posix_path(buf,name);
  if ( !access(name,X_OK) ) {
    bslash2slash(buf);
    sprintf(start_path,"%s /c start",buf);
    strcpy(buf,start_path);
    start_initialized  = 1;
    return 1;
  }

  strcpy(buf,"c:\\windows\\system32\\cmd.exe");
  cygwin_conv_to_full_posix_path(buf,name);
  if ( !access(name,X_OK) ) {
    bslash2slash(buf);
    sprintf(start_path,"%s /c start",buf);
    strcpy(buf,start_path);
    start_initialized  = 1;
    return 1;
  }

  return 0;
}

static void get_launcher_path(char *buf)
{
  static char rootname[BUFSIZ];
  static char launcher_path[BUFSIZ];
  static int launcher_initialized = 0;
  char name[BUFSIZ];
  
  if ( launcher_initialized ) {
    strcpy(buf,launcher_path);
    return;
  }

  get_rootdir(rootname,sizeof(rootname));
  sprintf(name,"%s/ox_launch.exe",rootname);
  cygwin_conv_to_full_win32_path(name,launcher_path);    
  bslash2slash(launcher_path);
  launcher_initialized = 1;
  strcpy(buf,launcher_path);
}
#endif

void spawn_server(char *host,char *launcher,char *server,
  int use_unix,int use_ssh,int use_x,int conn_to_serv,
  char *control_port_str,char *server_port_str)
{
  char localhost[BUFSIZ];
  char *dname,*conn_str,*rsh,*dname0,*asirhost;
  char AsirExe[BUFSIZ];
  STRING rootdir;
  char prog[BUFSIZ];
  char *av[BUFSIZ];
#if !defined(VISUAL) && !defined(__MINGW32__)
  char cmd[BUFSIZ];
#endif
#if defined(__CYGWIN__)
  char win_start[BUFSIZ],win_launcher[BUFSIZ];
#endif
  void Pget_rootdir();

  dname0 = (char *)getenv("DISPLAY");
  if ( !dname0 )
    dname0 = "0";
  dname = use_x ? dname0 : 0;
  conn_str = conn_to_serv ? "1" : "0";
  rsh = getenv("ASIR_RSH");
  if ( !rsh )
    rsh = use_ssh ? "ssh" : RSH;
  if ( !use_unix && strstr(rsh,"ssh") ) {
    /*
     * if "ssh" is used to invoke a remote server,
     * we should not specify "-display".
     */
    use_ssh = 1;
  }
  asirhost = (char *)getenv("ASIRHOSTNAME");
  if ( asirhost )
    strcpy(localhost,asirhost);
  else
    gethostname(localhost,BUFSIZ);
#if defined(VISUAL) || defined(__MINGW32__)
  if ( !use_unix )
    error("spawn_server : not implemented on Windows");
  Pget_rootdir(&rootdir);
  sprintf(AsirExe,"%s\\bin\\engine.exe",BDY(rootdir));
  strcpy(prog,server);
  server = strrchr(prog,'/')+1;
  av[0] = "ox_launch";
  av[1] = "127.0.0.1";
  av[2] = conn_str;
  av[3] = control_port_str;
  av[4] = server_port_str;
  av[5] = server;
  av[6] = use_x ? "1" : "0";
  av[7] = 0;

  _spawnv(_P_NOWAIT,AsirExe,av);
//  _spawnv(_P_NOWAIT,"d:\\home\\noro\\engine2000\\debug\\engine.exe",av);
//  printf("ox_launch 127.0.0.1 %s %s %s %s 0\n",conn_str,control_port_str,server_port_str,server);
#else
  if ( use_unix || !host ) {
#if defined(__CYGWIN32__)
    get_launcher_path(win_launcher);
    if ( dname && strchr(dname,':') ) {
      if ( !fork() ) {
        setpgid(0,getpid());
        execlp("xterm","xterm","-name",OX_XTERM,"-T","ox_launch:local","-display",dname,
          "-geometry","60x10","-xrm","XTerm*locale:false","-e",launcher,use_unix?".":"127.1",conn_str,
          control_port_str,server_port_str,server,dname,(char *)0);
      }
    } else if ( dname && get_start_path(win_start) ) {
      sprintf(cmd,"%s %s %s %s %s %s %s 1",
        win_start,win_launcher,use_unix?".":"127.1",conn_str,
        control_port_str,server_port_str,server);  
      system(cmd);
    } else {
      if ( !fork() ) {
        setpgid(0,getpid());
        execlp(launcher,launcher,use_unix?".":"127.1",conn_str,
          control_port_str,server_port_str,server,dname0,"-nolog",(char *)0);
      }
    }
#else
    if ( !fork() ) {
      setpgid(0,getpid());
      if ( dname )
        execlp("xterm","xterm","-name",OX_XTERM,"-T","ox_launch:local","-display",dname,
          "-geometry","60x10","-xrm","XTerm*locale:false","-e",launcher,use_unix?".":"127.1",conn_str,
          control_port_str,server_port_str,server,dname,(char *)0);
      else 
        execlp(launcher,launcher,use_unix?".":"127.1",conn_str,
          control_port_str,server_port_str,server,dname0,"-nolog",(char *)0);
    }
#endif
  } else if ( conn_to_serv == 2 ) {
    /* special support for java */
    if ( dname )
      sprintf(cmd,
        "%s -n %s \"(cd %s; xterm -name %s %s -geometry 60x10 -e java %s -host %s -control %s -data %s)>&/dev/null&\">/dev/null",
        rsh,host,launcher,OX_XTERM,dname,server,localhost,control_port_str,server_port_str);
    else
      sprintf(cmd,
        "%s -n %s \"(cd %s; java %s -host %s -control %s -data %s)>&/dev/null&\">/dev/null",
        rsh,host,launcher,server,localhost,
        control_port_str,server_port_str);
    fprintf(stderr,"%s\n",cmd);
    sleep(20);
/*    system(cmd); */
  } else {
    if ( dname )
      if ( use_ssh )
        sprintf(cmd,
"%s -f -n %s \"xterm -name %s -title ox_launch:%s -geometry 60x10 -xrm 'XTerm*locale:false' -e %s %s %s %s %s %s %s >&/dev/null\">/dev/null",
        rsh,host,OX_XTERM,host,launcher,localhost,conn_str,
        control_port_str,server_port_str,server,"1");
      else
        sprintf(cmd,
"%s -n %s \"xterm -name %s -title ox_launch:%s -display %s -geometry 60x10 -xrm 'XTerm*locale:false' -e %s %s %s %s %s %s %s >&/dev/null&\">/dev/null",
        rsh,host,OX_XTERM,host,dname,launcher,localhost,conn_str,
        control_port_str,server_port_str,server,dname);
    else
      if ( use_ssh )
        sprintf(cmd,
"%s -f -n %s \"%s %s %s %s %s %s %s %s>&/dev/null&\">/dev/null",
        rsh,host,launcher,localhost,conn_str,
        control_port_str,server_port_str,server,"1","-nolog");
      else
        sprintf(cmd,
"%s -n %s \"%s %s %s %s %s %s %s %s>&/dev/null&\">/dev/null",
        rsh,host,launcher,localhost,conn_str,
        control_port_str,server_port_str,server,dname0,"-nolog");
    system(cmd);
  }
#endif /* VISUAL */
}

void Pox_launch(NODE arg,Obj *rp)
{
  ox_launch_main(1,arg,rp);
}

void Pox_launch_nox(NODE arg,Obj *rp)
{
  ox_launch_main(0,arg,rp);
}

/* 
  ox_launch() : invoke local ox_asir
  ox_launch(0,ox_xxx) : invoke local ox_xxx with asir_libdir/ox_launch
  ox_launch(remote,lib,ox_xxx) : invoke remote ox_xxx with lib/ox_launch
*/

void ox_launch_main(int with_x,NODE arg,Obj *p)
{
  char *str;
  char *hostname,*servername;
  char *control;
  int use_unix;
  Z ret;
  extern char *asir_libdir;

  if ( arg && ARG0(arg) && argc(arg) != 3 )
    error("ox_launch : argument mismatch");
  control = (char *)MALLOC(BUFSIZ);
  if ( !arg || ( !ARG0(arg) && argc(arg) == 1 ) ) {
    sprintf(control,"%s/ox_launch",asir_libdir);
    use_unix = IS_CYGWIN ? 0 : 1;
    servername = (char *)MALLOC(BUFSIZ);
    sprintf(servername,"%s/ox_asir",asir_libdir);
  } else if ( !ARG0(arg) && argc(arg) == 2 ) {
    sprintf(control,"%s/ox_launch",asir_libdir);
    use_unix = IS_CYGWIN ? 0 : 1;
    str = BDY((STRING)ARG1(arg));
    if ( str[0] == '/' )
      servername = str;
    else {
      servername = (char *)MALLOC(BUFSIZ);
      sprintf(servername,"%s/%s",asir_libdir,str);
    }
  } else {
    sprintf(control,"%s/ox_launch",BDY((STRING)ARG1(arg)));
    if ( !ARG0(arg) )
      use_unix = IS_CYGWIN ? 0 : 1;
    else
      use_unix = 0;
    str = BDY((STRING)ARG2(arg));
    if ( str[0] == '/' )
      servername = str;
    else {
      servername = (char *)MALLOC(BUFSIZ);
      sprintf(servername,"%s/%s",BDY((STRING)ARG1(arg)),str);
    }
  }
  if ( arg && ARG0(arg) )
    hostname = BDY((STRING)ARG0(arg));
  else
    hostname = 0;
  ox_launch_generic(hostname,control,servername,use_unix,0,with_x,0,&ret);
  *p = (Obj)ret;
}

void extend_mctab(int bound)
{
  int i,n;
  struct m_c *t;
    if ( !m_c_tab ) {
        n = (bound/INIT_TAB_SIZ + 1)*INIT_TAB_SIZ;
        t = (struct m_c *)MALLOC_ATOMIC(n*sizeof(struct m_c));
        for ( i = m_c_s; i < n; i++ ) {
            t[i].af_unix = 0;
            t[i].m = t[i].c = -1;
        }
        m_c_s = n; m_c_tab = t;
    }else if (bound >= m_c_s) {
        n = (bound/INIT_TAB_SIZ + 1)*INIT_TAB_SIZ;
        t = (struct m_c *)MALLOC_ATOMIC(n*sizeof(struct m_c));
        bzero((void *)t,n*sizeof(struct m_c));
        bcopy((void *)m_c_tab,(void *)t,m_c_s*sizeof(struct m_c));
        for ( i = m_c_s; i < n; i++ ) {
            t[i].af_unix = 0;
            t[i].m = t[i].c = -1;
        }
        m_c_s = n; m_c_tab = t;
    }else {
        return;
    }
}

int available_mcindex(int ind)
{
  if (ind < 0) return 1; 
  extend_mctab(ind);
  return m_c_tab[ind].m<0 && m_c_tab[ind].c<0;
}

int register_server(int af_unix,int m,int c,int ind)
{
  int s,i;
  struct m_c *t;
  if ( c < 0 )
    return -1;
  extend_mctab( (ind<0)? 0: ind );
  if(ind >= 0) {
    if (m_c_tab[ind].m<0 && m_c_tab[ind].c<0) {
      m_c_tab[ind].m = m; m_c_tab[ind].c = c;
      m_c_tab[ind].af_unix = af_unix;
      if (ind>=m_c_i) m_c_i = ind+1;
      return ind;
    }
    return -1;
  }
#if !defined(MPI)
  for ( i = 0; i < m_c_i; i++ )
    if ( (m_c_tab[i].m<0) && (m_c_tab[i].c<0) )
      break;
  if ( i < m_c_i ) {
    m_c_tab[i].m = m; m_c_tab[i].c = c;
    m_c_tab[i].af_unix = af_unix;
    return i;
  }
#endif
  if ( m_c_i == m_c_s ) {
    s = (m_c_s+INIT_TAB_SIZ)*sizeof(struct m_c);
    t = (struct m_c *)MALLOC_ATOMIC(s);
    bcopy((void *)m_c_tab,(void *)t,m_c_s*sizeof(struct m_c));
    for ( i = 0; i < INIT_TAB_SIZ; i++ ) {
      t[m_c_s+i].af_unix = 0;
      t[m_c_s+i].m = m_c_tab[m_c_s+i].c = -1;
    }
    m_c_s += INIT_TAB_SIZ; m_c_tab = t;
  }
  m_c_tab[m_c_i].m = m; m_c_tab[m_c_i].c = c;
  m_c_tab[m_c_i].af_unix = af_unix;
  return m_c_i++;
}

/* iofp index => m_c_tab index */

int get_mcindex(int i)
{
  int j;

  for ( j = 0; j < m_c_i; j++ )
    if ( m_c_tab[j].c == i )
      return j;
  return -1;
}

/* arg = [ind0,ind1,...]; indk = index to m_c_tab */

void Pox_select(NODE arg,LIST *rp)
{
  int fd,n,i,index,mcind,s,maxfd,minfd;
  fd_set r,w,e;
  NODE list,t,t1,t0;
  Z q;
  double max;
  struct timeval interval;
  struct timeval *tvp;

  list = BDY((LIST)ARG0(arg)); arg = NEXT(arg);
  if ( arg ) {
    max = ToReal((Num)BDY(arg));
    interval.tv_sec = (int)max;
    interval.tv_usec = (int)((max-(int)max)*1000000);
    tvp = &interval;
  } else
    tvp = 0;

  FD_ZERO(&r); FD_ZERO(&w); FD_ZERO(&e);
  maxfd = minfd = -1;
  for ( t = list, t0 = 0; t; t = NEXT(t) ) {
    index = ZTOS((Q)BDY(t));
    valid_mctab_index(index);
    s = m_c_tab[index].c;
    if ( ox_data_is_available(s) ) {
      MKNODE(t1,(Q)BDY(t),t0); t0 = t1;
    } else {
      fd = get_fd(s); FD_SET((unsigned int)fd,&r);
      maxfd = maxfd<0 ? fd : MAX(fd,maxfd);
      minfd = minfd<0 ? fd : MIN(fd,minfd);
    }
  }
  if ( t0 ) {
    MKLIST(*rp,t0); return;
  }

  n = select(FD_SETSIZE,&r,&w,&e,tvp);
#if defined(VISUAL) || defined(__MINGW32__)
  for ( i = minfd, t = 0; n && i <= maxfd; i++ )
#else
  for ( i = 0, t = 0; n && i < FD_SETSIZE; i++ )
#endif
    if ( FD_ISSET(i,&r) ) {
      /* index : index to iofp array */
      index = get_index(i);
      /* mcind : index to m_c_tab array */
      mcind = get_mcindex(index);
      n--; STOZ(mcind,q); MKNODE(t1,q,t); t = t1;
    }
  MKLIST(*rp,t);
}

void Pox_flush(NODE arg,Z *rp)
{
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  ox_flush_stream_force(m_c_tab[index].c);
  *rp = ONE;
}

void Pox_send_raw_cmo(NODE arg,Obj *rp)
{
  int s;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  ox_write_cmo(s,(Obj)ARG1(arg));
  /* flush always */
  ox_flush_stream(s);
  *rp = 0;
}

void Pox_recv_raw_cmo(NODE arg,Obj *rp)
{
  int s;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  ox_read_cmo(s,rp);
}

void Pox_send_102(NODE arg,Obj *rp)
{
  int rank = ZTOS((Q)ARG0(arg));

  ox_send_data_102(rank,(Obj)ARG1(arg));
  *rp = 0;
}

void Pox_recv_102(NODE arg,Obj *rp)
{
  int id;
  int rank = ZTOS((Q)ARG0(arg));

  ox_recv_102(rank,&id,rp);
}

void Pox_bcast_102(NODE arg,Obj *rp)
{
  int rank = ZTOS((Q)ARG0(arg));
  Obj data;

  if ( argc(arg) > 1 )
    asir_push_one((Obj)ARG1(arg));
  ox_bcast_102(rank);
  *rp = (Obj)asir_pop_one();
}

void Pox_reduce_102(NODE arg,Obj *rp)
{
  int root = ZTOS((Q)ARG0(arg));
  STRING op;
  char *opname;
  void (*func)();

  op = (STRING)ARG1(arg);
  asir_assert(op,O_STR,"ox_reduce_102");
  opname = BDY(op);
  if ( !strcmp(opname,"+") )
    func = arf_add;
  else if ( !strcmp(opname,"*") )
    func = arf_mul;
  else {
    error("ox_reduce_102 : operation not supported");
  }
  if ( argc(arg) > 2 )
    asir_push_one((Obj)ARG2(arg));
  ox_reduce_102(root,func);
  *rp = (Obj)asir_pop_one();
}

void Pox_push_local(NODE arg,Obj *rp)
{
  int s;
  struct oLIST dummy;
  VL vl;
  int index;

  if ( !arg )
    error("ox_push_local : too few arguments.");
  index = ZTOS((Q)ARG0(arg));
  valid_mctab_index(index);
  s = m_c_tab[index].c; arg = NEXT(arg);

  dummy.id = O_LIST; dummy.body = arg;
  get_vars_recursive((Obj)&dummy,&vl);

  ox_send_local_ring(s,vl);
  for ( ; arg; arg = NEXT(arg) )
    ox_send_local_data(s,BDY(arg));
  *rp = 0;
}

void Pox_push_cmo(NODE arg,Obj *rp)
{
  int s;
  int index;

  if ( !arg )
    error("ox_push_cmo : too few arguments.");
  index = ZTOS((Q)ARG0(arg));
  valid_mctab_index(index);
  s = m_c_tab[index].c; arg = NEXT(arg);
  for ( ; arg; arg = NEXT(arg) )
    ox_send_data(s,BDY(arg));
  *rp = 0;
}

void Pox_push_vl(NODE arg,Obj *rp)
{
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  ox_send_local_ring(m_c_tab[index].c,CO);
  *rp = 0;
}

void Pox_pop_local(NODE arg,Obj *rp)
{
  int s;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  ox_send_cmd(s,SM_popSerializedLocalObject);
  ox_flush_stream_force(s);
  ox_get_result(s,rp);
}

void Pox_pop_cmo(NODE arg,Obj *rp)
{
  int s;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  ox_send_cmd(s,SM_popCMO);
  ox_flush_stream_force(s);
  ox_get_result(s,rp);
}

void Pox_pop0_local(NODE arg,Obj *rp)
{
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  ox_send_cmd(m_c_tab[index].c,SM_popSerializedLocalObject);
  *rp = 0;
}

void Pox_pop0_cmo(NODE arg,Obj *rp)
{
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  ox_send_cmd(m_c_tab[index].c,SM_popCMO);
  *rp = 0;
}

void Pox_pop0_string(NODE arg,STRING *rp)
{
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  ox_send_cmd(m_c_tab[index].c,SM_popString);
  *rp = 0;
}

void Pox_pop_string(NODE arg,Obj *rp)
{
  int s;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  ox_send_cmd(s,SM_popString);
  ox_flush_stream_force(s);
  ox_get_result(s,rp);
}

void Pox_get(NODE arg,Obj *rp)
{
  int index;
  int s;

  if ( !arg ) {
    /* client->server */
    ox_get_result(0,rp);
  } else {
    /* server->client */
    index = ZTOS((Q)ARG0(arg));
    valid_mctab_index(index);
    s = m_c_tab[index].c;
    ox_flush_stream_force(s);
    ox_get_result(s,rp);
  }
}

void Pox_pops(NODE arg,Obj *rp)
{
  int s;
  USINT n;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  if ( NEXT(arg) )
    MKUSINT(n,ZTOS((Q)ARG1(arg)));
  else
    MKUSINT(n,1);
  ox_send_data(s,n);
  ox_send_cmd(s,SM_pops);
  *rp = 0;
}

void Pox_execute_function(NODE arg,Obj *rp)
{
  int s;
  USINT ui;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  MKUSINT(ui,ZTOS((Q)ARG2(arg)));
  ox_send_data(s,ui);
  ox_send_data(s,ARG1(arg));
  ox_send_cmd(s,SM_executeFunction);
  *rp = 0;
}

void Pox_setname(NODE arg,Obj *rp)
{
  int s;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  ox_send_data(s,ARG1(arg));
  ox_send_cmd(s,SM_setName);
  *rp = 0;
}

void Pox_evalname(NODE arg,Obj *rp)
{
  int s;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c;
  ox_send_data(s,ARG1(arg));
  ox_send_cmd(s,SM_evalName);
  *rp = 0;
}

void Pox_execute_string(NODE arg,Obj *rp)
{
  int s;
  int index = ZTOS((Q)ARG0(arg));

  asir_assert(ARG1(arg),O_STR,"ox_execute_string");
  valid_mctab_index(index);
  s = m_c_tab[index].c;
  ox_send_data(s,ARG1(arg));
  ox_send_cmd(s,SM_executeStringByLocalParser);
  *rp = 0;
}

/* arg=[sid,fname,arg0,arg1,...,arg{n-1}] */

void Pox_rpc(NODE arg,Obj *rp)
{
  int s,i,n;
  STRING f;
  USINT ui;
  pointer *w;
  NODE t;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  s = m_c_tab[index].c; arg = NEXT(arg);
  f = (STRING)BDY(arg); arg = NEXT(arg);
  ox_send_local_ring(s,CO);
  for ( n = 0, t = arg; t; t = NEXT(t), n++ );
  w = (pointer *)ALLOCA(n*sizeof(pointer));
  for ( i = 0, t = arg; i < n; t = NEXT(t), i++ )
    w[i] = BDY(t);
  for ( i = n-1; i >= 0; i-- )
    ox_send_local_data(s,w[i]);
  MKUSINT(ui,n);
  ox_send_data(s,ui);
  ox_send_data(s,f);
  ox_send_cmd(s,SM_executeFunction);
  *rp = 0;
}

void Pox_cmo_rpc(NODE arg,Obj *rp)
{
  int s,i,n;
  STRING f;
  USINT ui;
  NODE t;
  Obj dmy;
  pointer *w;
  int index = ZTOS((Q)ARG0(arg));
  int find;
  Obj sync;

  find = get_opt("sync",&sync);
  valid_mctab_index(index);
  s = m_c_tab[index].c; arg = NEXT(arg);
  f = (STRING)BDY(arg); arg = NEXT(arg);
  for ( n = 0, t = arg; t; t = NEXT(t), n++ );
  w = (pointer *)ALLOCA(n*sizeof(pointer));
  for ( i = 0, t = arg; i < n; t = NEXT(t), i++ )
    w[i] = BDY(t);
  for ( i = n-1; i >= 0; i-- )
    ox_send_data(s,w[i]);
  MKUSINT(ui,n);
  ox_send_data(s,ui);
  ox_send_data(s,f);
  if ( find && sync ) {
    ox_send_cmd(s,SM_executeFunctionSync);
    ox_get_result(s,&dmy);
  } else
    ox_send_cmd(s,SM_executeFunction);
  *rp = 0;
}

int No_ox_reset;
extern Z ox_pari_stream;
extern int ox_pari_stream_initialized;

void Pox_reset(NODE arg,Z *rp)
{
  USINT t;
  int id,c,m;
  Obj obj;
  NODE nd;
  Z q;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  m = m_c_tab[index].m;
  c = m_c_tab[index].c;
  if ( m >= 0 ) {
    if ( no_ox_reset(c) == 1 ) {
      STOZ(index,q);
      nd = mknode(1,q);
      switch ( No_ox_reset ) {
      case 1:
         fprintf(stderr,"The server does not implenent OX reset protocol.\n");
         fprintf(stderr,"The server is terminated.\n");
         Pox_shutdown(nd,rp);
         if ( index == ZTOS(ox_pari_stream) ) ox_pari_stream_initialized = 0;
         break;
      case 2:
         Pox_shutdown(nd,rp);
         if ( index == ZTOS(ox_pari_stream) ) ox_pari_stream_initialized = 0;
         break;
      default:
         error("The server does not implement OX reset protocol.");
         *rp = ONE;
         break;
      }
      return;
    } 

    if ( argc(arg) == 1 ) {
      ox_send_cmd(m,SM_control_reset_connection);
      ox_flush_stream_force(m);
#if 0
      /* XXX obsolete */
      ox_recv(m,&id,&obj); t = (USINT)obj;
#endif
    }
    *rp = ONE;
#if defined(VISUAL) || defined(__MINGW32__)
    Sleep(100);
    ox_send_cmd(c,SM_nop);
    ox_flush_stream_force(c);
#endif
    while ( 1 ) {
      ox_recv(c,&id,&obj);
      if ( id == OX_SYNC_BALL )
        break;
    }
    ox_send_sync(c);
  } else
    *rp = 0;
}

void Pox_intr(NODE arg,Z *rp)
{
  int m;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  m = m_c_tab[index].m;
  if ( m >= 0 ) {
    if ( argc(arg) == 1 ) {
      ox_send_cmd(m,SM_control_intr);
      ox_flush_stream_force(m);
    }
    *rp = ONE;
  } else
    *rp = 0;
}

void Pox_sync(NODE arg,Z *rp)
{
  int c;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  c = m_c_tab[index].c;
  ox_send_sync(c);
  *rp = 0;
}

void Pox_shutdown(NODE arg,Z *rp)
{
  int s;
  int index = ZTOS((Q)ARG0(arg));
#if !defined(VISUAL) && !defined(__MINGW32__)
  int status;
#endif

  valid_mctab_index(index);
  s = m_c_tab[index].m;
  ox_send_cmd(s,SM_shutdown);
  free_iofp(s);
  s = m_c_tab[index].c;
  free_iofp(s);
#if !defined(MPI) && !defined(VISUAL) && !defined(__MINGW32__)
  if ( m_c_tab[index].af_unix )
    wait(&status);
#endif
  m_c_tab[index].m = -1; m_c_tab[index].c = -1;
  m_c_tab[index].af_unix = 0;
  *rp = 0;
}

void Pox_push_cmd(NODE arg,Z *rp)
{
  int ui;
  int index = ZTOS((Q)ARG0(arg));

  valid_mctab_index(index);
  ui = ZTOS((Q)ARG1(arg));
  ox_send_cmd(m_c_tab[index].c,ui);
  *rp = 0;
}

void shutdown_all() {
  int s;
  int i,index;
#if !defined(VISUAL) && !defined(__MINGW32__)
  int status;
#endif

  for ( i = I_am_server?1:0; i < m_c_i; i++ ) {
    index = i;
    check_valid_mctab_index(index);
    if ( index < 0 )
      continue;
    s = m_c_tab[index].m;
    ox_send_cmd(s,SM_shutdown);
#if defined(VISUAL) || defined(__MINGW32__)
  Sleep(1000);
#endif
    free_iofp(s);
    s = m_c_tab[index].c;
    free_iofp(s);
#if !defined(MPI) && !defined(VISUAL) && !defined(__MINGW32__)
    if ( m_c_tab[index].af_unix )
      wait(&status);
#endif
    m_c_tab[index].m = 0; m_c_tab[index].c = 0;
    m_c_tab[index].af_unix = 0;
  }
}

char *ox_get_servername(int);

int is_ox_plot(int index)
{
  char *name;

  check_valid_mctab_index(index);
  if ( index < 0 )
    return 0;
  /* m : client, c : server ??? */
  name = ox_get_servername(m_c_tab[index].c);
  return strcmp(name,"ox_plot") ? 0 : 1;
}

int debug_plot;

int validate_ox_plot_stream(int index)
{
  int i;
  NODE arg;
  STRING name;
  Obj r;

  if ( is_ox_plot(index) )
    return index;
  for ( i = 0; i < m_c_i; i++ )
    if ( is_ox_plot(i) )
      return i;

  /* create an ox_plot server */
  MKSTR(name,"ox_plot");
  arg = mknode(2,NULL,name);
  if ( debug_plot ) Pox_launch(arg,&r);
  else Pox_launch_nox(arg,&r);
  i = ZTOS((Q)r);
#if defined(VISUAL) || defined(__MINGW32__)
  Sleep(100);
  ox_send_cmd(m_c_tab[i].c,SM_nop);
  ox_flush_stream_force(m_c_tab[i].c);
#endif
  return i;
}

int get_ox_server_id(int index)
{
  valid_mctab_index(index);
  return m_c_tab[index].c;
}

int register_102(int s1,int rank,int is_master)
{
  unsigned char c,rc;

  if ( rank >= MAXIOFP ) return -1;
  iofp_102[rank].s = s1;
#if defined(VISUAL) || defined(__MINGW32__)
  iofp_102[rank].in = WSIO_open(s1,"r");
  iofp_102[rank].out = WSIO_open(s1,"w");
#else
  iofp_102[rank].in = fdopen(s1,"r");
  iofp_102[rank].out = fdopen(dup(s1),"w");
#if !defined(__CYGWIN__)
  setbuffer(iofp_102[rank].in,iofp_102[rank].inbuf = 
    (char *)MALLOC_ATOMIC(LBUFSIZ),LBUFSIZ);
  setbuffer(iofp_102[rank].out,iofp_102[rank].outbuf = 
    (char *)MALLOC_ATOMIC(LBUFSIZ),LBUFSIZ);
#endif
#endif
  if ( little_endian )
    c = 1;
  else
    c = 0xff;
  if ( is_master ) {
    /* server : write -> read */
    write_char((FILE *)iofp_102[rank].out,&c);
    ox_flush_stream_force_102(rank);
    read_char((FILE *)iofp_102[rank].in,&rc);
  } else {
    /* client : read -> write */
    read_char((FILE *)iofp_102[rank].in,&rc);
    /* special care for a failure of spawing a server */
    if ( rc !=0 && rc != 1 && rc != 0xff )
      return -1;  
    write_char((FILE *)iofp_102[rank].out,&c);
    ox_flush_stream_force_102(rank);
  }
  iofp_102[rank].conv = c == rc ? 0 : 1;
  iofp_102[rank].socket = 0;
  return 0;
}

