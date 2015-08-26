#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>

#include <unistd.h> //usleep
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <signal.h>
#include "global.h"
#include "debug.h"
#include "library/wdslib.h"

debug_c d;


void cb_console(const char *str, void *data)
{
	echo << "CONSOLE> " << str << '\n';
}

/* Note that unlike what's documented in gdb docs it isn't usable. */
void cb_target(const char *str, void *data)
{
	echo << "TARGET> " << str << '\n';
}

void cb_log(const char *str, void *data)
{
	echo << "LOG> " << str << '\n';
}

void cb_to(const char *str, void *data)
{
	echo << ">> " << str;
}

void cb_from(const char *str, void *data)
{
	echo << "<< " << str << '\n';
}

volatile int async_c=0;


void cb_async(mi_output *o, void *data)
{
	echo << "ASYNC\n";
	async_c++;
}

int wait_for_stop(MIDebugger &d)
{
	int res=1;
	mi_stop *sr;
	
	while (!d.Poll(sr))
		usleep(1000);
	// The end of the async. 
	if (sr){
		printf("Stopped, reason: %s\n",mi_reason_enum_to_str(sr->reason));
		mi_free_stop(sr);
	}
	else{
		printf("Error while waiting\n");
		printf("mi_error: %d\nmi_error_from_gdb: %s\n",mi_error,mi_error_from_gdb);
		res=0;
	}
	return res;
}


int DoTryRun(int res, MIDebugger &d)
{
	if(!res){
		printf("Error in executing!\n");
		return 0;
	}
	if(!wait_for_stop(d))
		return 0;
	return 1;
}

int wait_for_stop(mi_h *h)
{
	int res=1;
	mi_stop *sr;
	
	while (!mi_get_response(h))
		usleep(1000);
	/* The end of the async. */
	sr=mi_res_stop(h);
	if (sr){
		printf("Stopped, reason: %s\n",mi_reason_enum_to_str(sr->reason));
		mi_free_stop(sr);
	}
	else{
		printf("Error while waiting\n");
		res=0;
	}
	return res;
}

#define TryRun(a,b) if (!DoTryRun(a,b)) return 1



int debug_c::connect()
{
	pipe(to_gdb);
	pipe(from_gdb);
	pid = fork();
	if( pid == 0){
		/* We are the child. */
//		char *argv[5];
		/* Connect stdin/out to the pipes. */
		close(to_gdb[1]);
		close(from_gdb[0]);		
		dup2(to_gdb[0],STDIN_FILENO);
		dup2(from_gdb[1],STDOUT_FILENO);
		
		/* Pass the control to gdb. */
		char *argv[]{ 
			"gdb",
			"--interpreter=mi",
			"--quiet",
			"--readnow", //disable_psym_search_workaround?
			nullptr
		};
		execvp(argv[0], argv);
		/* We get here only if exec failed. */
		assert(0);
	}	
	close(to_gdb[0]);
	close(from_gdb[1]);
}

void debug_c::disconnect()
{
	close(to_gdb[1]);
	close(to_gdb[0]);
	close(from_gdb[0]);
	close(from_gdb[1]);
}

void debug_c::send(string str)
{
	stringstream ss{str};
	string s, s1, s2;
	ss >> s >> s1 >> s2;
	echo << "send: " << str << '\n';
	string cs, com, arg, file, bkpt_file, bkpt_line, watch_pt;		
	if ( s == "exe"){
		file = s1;
		arg = s2;
		cs = "-file-exec-and-symbols " + file + '\n';
		write(to_gdb[1], cs.c_str(), cs.size()); 
		cs = "-exec-arguments " + arg + '\n';
		write(to_gdb[1], cs.c_str(), cs.size()); 
//		cs = "tty /dev/tty1\n";
//		write(to_gdb[1], cs.c_str(), cs.size()); 
	}
	else if ( s == "exit"){
		cs = "-gdb-exit\n";
		write(to_gdb[1], cs.c_str(), cs.size()); 
		echo << cs;
		close_terminal();		
	}
	else if (s == "r"){
		cs = "-exec-run\n";		
		write(to_gdb[1], cs.c_str(), cs.size());
	}
	else if (s == "c"){
		cs = "-exec-continue\n";
		write(to_gdb[1], cs.c_str(), cs.size());
	}
	else if (s == "n"){
		cs = "-exec-next\n";
		write(to_gdb[1], cs.c_str(), cs.size());
	}
	else if ( s == "bp" ) {
		bkpt_file = "/home/henry/shelf/cpp/cs/les1.cpp";
		bkpt_line = 17;
		cs = "-break-insert " + bkpt_file + ":17\n";		
		write(to_gdb[1], cs.c_str(), cs.size());
	}
	else if ( s == "wp" ) {
		cs = "-break-watch \"v\"\n";		
		write(to_gdb[1], cs.c_str(), cs.size());
	}
	else if ( s == "tty"){
		cs = "tty " + s1 + '\n';
		echo << "send tty: " << cs;
		write(to_gdb[1], cs.c_str(), cs.size());
	}
	else if ( s == "job"){
		if ( s1 == "le"){
			cs = "exe /home/henry/shelf/cpp/cs/les1 lynx lamb april";
		}
		else if ( s1 == "la"){
			cs = "exe /home/henry/shelf/cpp/clone/lambd -path=/home/henry/shelf/cpp/clone";
		}
		echo << "job: " << cs << '\n';
		connect();
		send(cs);
		send("r");
	}
}

string debug_c::receive()
{
	timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;
	const size_t size_buffer = 10000;
		
	fd_set set;
	FD_ZERO(&set);
	FD_SET(from_gdb[0], &set);
	
	int ready = select(from_gdb[0]+1, &set, nullptr, nullptr, &timeout);
	if( not ready ) {
		echo << "no date evailable\n";
		return {};
	}
	char res[size_buffer];
	int n = read(from_gdb[0], res, size_buffer);
	if (n>0){
		echo << res ;
	}
	return {};
}

void debug_c::close_terminal()
{
//	string com{"/bin/bash exit\n"};
//	int fds = open("/dev/pts/5", O_RDWR);
//	write(fds, com.c_str(), com.size());
	kill(pid_terminal, SIGTERM);
//	kill(pid_terminal, SIGKILL);
}

string debug_c::open_terminal3(string name)
{
}

string debug_c::open_terminal2(string name)
{
	echo << "open terminal2\n";
	
	int fdm, fds;
	int rc;
	char input[150];

	fdm = posix_openpt(O_RDWR);
	if (fdm < 0){
		fprintf(stderr, "Error %d on posix_openpt()\n", errno);
		return {};
	}
	
	rc = grantpt(fdm);
	if (rc != 0){
		fprintf(stderr, "Error %d on grantpt()\n", errno);
		return {};
	}
	
	rc = unlockpt(fdm);
	if (rc != 0){
		fprintf(stderr, "Error %d on unlockpt()\n", errno);
		return {};
	}
	
	// Open the slave side ot the PTY
	fds = open(ptsname(fdm), O_RDWR);
	
		ostringstream oss;
		oss << "-S" << ptsname(fdm) << '/' << fdm;
//		oss << "-S" << ptsname(fdm) ;
		string sn{oss.str()};
		echo << sn << '\n';
	
	pid_terminal = fork();
	if (pid_terminal==0){
		const char* argv[]{
			name.c_str(), 
			sn.c_str(),
		//	"--disable-factory", 
			nullptr
		};
		execvp(argv[0], (char* const*)argv);
		assert(0);
	}
	usleep(500000);
//	string com{"echo hallo\n"};
//	write(to_terminal[1], com.c_str(), com.size());	
//	close(to_terminal[0]);
	return {};
}

string debug_c::open_terminal(string name, string option)
{
	pid_t pid;
	string path{get_lambs_path()};
	string t1{path + "/t1"},
			t2{path + "/t2"};
	ofstream ofs1{t1};
	ofs1 << "#!/bin/sh\n"
		<< "tty > " + t2 + '\n' 
		<< "rm " + t1 + '\n'
		<< "sleep 365d\n";
	ofs1.close();
	pipe(to_terminal);
	pid_terminal = fork();
	if (pid_terminal==0){
		close(to_terminal[1]);
		dup2(to_terminal[0], STDIN_FILENO);
		string sps = "/bin/sh " + t1;
		array<const char*, 5> argv {name.c_str()};
		int c = 1;
		if ( option!=""){
			argv[c++] = option.c_str();
		}
		argv[c++] = "-e";
		argv[c] = sps.c_str();
		execvp(argv[0], (char* const*)argv.data());
		assert(0);
	}
	for (;;){
		ifstream ifs1(t1);
		if (ifs1.fail()){
			break;
		}
		ifs1.close();
		usleep(1000);
	}
	close(to_terminal[0]);
	stringstream ss;
	ss << pid;
	echo << ss.str() << '\n';
	string s;
	/*
	s = "ps -p " + ss.str() + " -o tty > testinfo";
	echo << s << '\n';
	system(s.c_str());
	*/
	ifstream ifs(t2);
	getline(ifs, s);
	return s;
}

int debug_c::gdb(std::string str)
{
	stringstream ss{str};
	string s, s1, s2, s3;
	ss >> s >> s1 >> s2 >> s3;
	if ( s1 == "connect" or s1 == "con"){
		connect();
	}
	else if (s1 == "disconnect" or s1 == "dis"){
		disconnect();
	}
	else if (s1 == "sen"){
		send(s2 + " " + s3);
	}
	else if (s1 == "rec"){
		string r = receive();				
	}
	else if (s1 == "term"){
		if (s2!="close" and s2!="c"){
			echo << "open terminal: " << str << '\n';
			string terminal, option;
			if (s2=="x" or s2=="xterm"){
				terminal = "/usr/bin/xterm";
			}
			else {
				terminal = "/usr/bin/gnome-terminal"; 
				option = "--disable-factory";
			}
			string res = open_terminal(terminal, option);
//			string res = open_terminal2(ter);
			echo << "terminal's tty: " << res << '\n';
		}
		else{
			echo << "close terminal\n";
			close_terminal();			
		}
	}
	else if (s1 == "jx" or s1 == "jg"){
		connect();
		string tty;
		if (s1=="jx"){
			tty = open_terminal("/usr/bin/xterm", "");
		}
		else {
			tty = open_terminal("/usr/bin/gnome-terminal", "--disable-factory");
		}
		echo << "tty:: " << tty << '\n';
		tty = "tty " + tty;
		send(tty);
		string cs, arg;
		if (s2 == "le" or s2 == "les1"){
			cs = "exe /home/henry/shelf/cpp/cs/les1"; 
			arg = "lynx lamb april";
		}
		else {
			cs = "exe /home/henry/shelf/cpp/clone/lambd"; 
			arg = "-path=/home/henry/shelf/cpp/clone";
		}
		send (cs + " " + arg);
		send ("r");
	}
	else if (s1 == "jl"){
		connect();
//		send(tty);
		string cs, arg;
		if (s2 == "le" or s2 == "les1"){
			cs = "exe /home/henry/shelf/cpp/cs/les1"; 
			arg = "lynx lamb april";
		}
		else {
			cs = "exe /home/henry/shelf/cpp/clone/lambd"; 
			arg = "-path=/home/henry/shelf/cpp/clone";
		}
		send (cs + " " + arg);
		send ("r");

	}
}

void debug_c::run()
{
//	run2("");
}

int debug_c::runc(std::string str)
{
	stringstream ss{str};
	string s, s1, s2, s3;
	ss >> s >> s1;
	mi_aux_term *child_vt=NULL;
	mi_bkpt *bk;
	mi_wp *wp;

	//This is like a file-handle for fopen.
	//Here we have all the state of gdb "connection". 
	static mi_h *h;

	if ( s == "start"){
	
		//You can use any gdb you want: 
		//mi_set_gdb_exe("/usr/src/gdb-6.1.1/gdb/gdb");
		//You can use a terminal different than xterm:
		//mi_set_xterm_exe("/usr/bin/Eterm");
		
		//Connect to gdb child.
		h=mi_connect_local();
		if (!h){
			printf("Connect failed\n");
			return 1;
		}
		printf("Connected to gdb!\n");
	
		//Set all callbacks.
		mi_set_console_cb(h,cb_console,NULL);
		mi_set_target_cb(h,cb_target,NULL);
		mi_set_log_cb(h,cb_log,NULL);
		mi_set_async_cb(h,cb_async,NULL);
		mi_set_to_gdb_cb(h,cb_to,NULL);
		mi_set_from_gdb_cb(h,cb_from,NULL);
	
		//Look for a free VT where we can run the child.
		child_vt=gmi_look_for_free_vt();
		if (!child_vt)
			printf("Error opening auxiliar terminal, we'll use current one.\n");
		else{
			printf("Free VT @ %s\n",child_vt->tty);
			printf("\n\n***************************************\n");
			printf("Switch to the above mentioned terminal!\n");
			printf("***************************************\n\n\n");
		}
		
		//Tell gdb to attach the child to a terminal.
		if (!gmi_target_terminal(h,child_vt ? child_vt->tty : ttyname(STDIN_FILENO))){
			printf("Error selecting target terminal\n");
			mi_disconnect(h);
			return 1;
		}
		
		//Set the name of the child and the command line aguments.
		string com, arg, bkpt_file;
		int bkpt_line;
		if ( s1 == "lambd"){
			com = "/home/henry/shelf/cpp/clone/lambd";
			arg = "-path=/home/henry/shelf/cpp/clone";
			bkpt_file = "/home/henry/shelf/cpp/wss/lamb.cpp";
			bkpt_line = 1032;
		}
		else if ( s1 == "les1"){
			com= "/home/henry/shelf/cpp/cs/les1";
			arg = "lynx lamb april";
			bkpt_file = "/home/henry/shelf/cpp/cs/les1.cpp";
			bkpt_line = 20;
		}
		if (!gmi_set_exec(h,com.c_str(),arg.c_str())){
			printf("Error setting exec y args\n");
			mi_disconnect(h);
			return 1;
		}
	
	//Set a breakpoint.
	/*
		bk = gmi_break_insert(h, bkpt_file.c_str(), bkpt_line);
		
		if (!bk){
			printf("Error setting breakpoint\n");
			mi_disconnect(h);
			return 1;
		}
		printf("Breakpoint %d @ function: %s\n",bk->number,bk->func);
	*/	
	//You can do things like:
	// gmi_break_delete(h,bk->number);
	// gmi_break_set_times(h,bk->number,2);
	// gmi_break_set_condition(h,bk->number,"1");
	// gmi_break_state(h,bk->number,0);
	//If we no longer need this data we can release it.
//	mi_free_bkpt(bk);
	
	//Set a watchpoint, that's a data breakpoint.
/*	
		wp=gmi_break_watch(h,wm_write,"v");
		if (!wp){
			printf("Error al setting watchpoint\n");
			mi_disconnect(h);
			return 1;
		}
		printf("Watchpoint %d for expression: %s\n",wp->number,wp->exp);
		mi_free_wp(wp);
*/	
		//Run the program.
		cout << "break_insert..\n";
		if (!gmi_exec_run(h)){
			printf("Error in run!\n");
			mi_disconnect(h);
			return 1;
		}
		cout << "break_insert...\n";
		//Here we should be stopped at the breakpoint.
		if (!wait_for_stop(h)){
			mi_disconnect(h);
			return 1;
		}
		cout << "start exit\n";
	}
	else if ( s == "r" ){
		cout << "continue execution\n";
		//Continue execution.
		if (!gmi_exec_continue(h)){
			printf("Error in continue!\n");
			mi_disconnect(h);
			return 1;
		}
	
		//Here we should be terminated.
		if (!wait_for_stop(h)){
			mi_disconnect(h);
			return 1;
		}
		return 0;
	}
	else if ( s == "s" ){
		cout << "continue execution\n";
		//Continue execution
//		if (!gmi_exec_continue(h)){
		if (!gmi_exec_next(h)){
			printf("Error in continue!\n");
			mi_disconnect(h);
			return 1;
		}
	
		//Here we should be terminated.
		if (!wait_for_stop(h)){
			mi_disconnect(h);
			return 1;
		}
		return 0;
	}
	else if ( s == "e" ){
		//Exit from gdb.
		gmi_gdb_exit(h);
		
		//Close the connection.
		mi_disconnect(h);
		
		gmi_end_aux_term(child_vt);
		return 0;
	}
	return 0;
}


int debug_c::runcc(std::string str)
{
	stringstream ss{str};
	string s, s1, s2, s3;
	ss >> s >> s1 >> s2;
	echo << str << '\n';
	if ( s1 == "start") {
		cout << "start\n";
		mi_bkpt *bk;
		mi_wp *wp;
	
	// Debugging object, used as an auto var.
	//	MIDebugger d;
	
		// You can use any gdb you want:
		//MIDebugger::SetGDBExe("/usr/src/gdb-6.1.1/gdb/gdb");
		// You can use a terminal different than xterm:
//		MIDebugger::SetXTerm("/usr/bin/Eterm");
//		MIDebugger::SetXTermExe("/usr/bin/gnome-terminal");
		MIDebugger::SetXTermExe("/usr/bin/xterm");
		// You can specify commands for gdb
		
		MIDebugger::SetGDBStartFile("/home/henry/libmigdc/examples/cmds.txt");
	
	 // Connect to gdb child.
		if (!d.Connect())
		{
			printf("Connect failed\n");
			return 1;
		}
		printf("Connected to gdb!\n");
	
	/* Set all callbacks. */
		d.SetConsoleCB(cb_console);
		d.SetTargetCB(cb_target);
		d.SetLogCB(cb_log);
		d.SetAsyncCB(cb_async);
		d.SetToGDBCB(cb_to);
		d.SetFromGDBCB(cb_from);
		
	 // Set the name of the child and the command line aguments.
	 // It also opens the xterm.
		string com, arg, bkpt_file;
		int bkpt_line;
		if ( s2 == "lambd"){
			com = "/home/henry/shelf/cpp/clone/lambd";
			arg = "-path=/home/henry/shelf/cpp/clone";
			bkpt_file = "/home/henry/shelf/cpp/wss/lamb.cpp";
			bkpt_line = 1032;
		}
		else if ( s2 == "les1"){
			com= "/home/henry/shelf/cpp/cs/les1";
			arg = "lynx lamb april";
			bkpt_file = "/home/henry/shelf/cpp/cs/les1.cpp";
			bkpt_line = 20;
		}
		
		if ( s == "linux"){
			if (!d.SelectTargetLinux(com.c_str(),arg.c_str())){
				printf("Error setting exec y args\n");
				return 1;
			}
		}
		if ( s == "x11"){
			echo << "sss\n";
			string pste{"/dev/pts/1"};
			if (!d.SelectTargetX11(com.c_str(),arg.c_str())){
				printf("Error setting exec y args\n");
				return 1;
			}
		}
		
//if (!d.SelectTargetX11(com.c_str(),arg.c_str(), pste.c_str())){

//		return 0;	
		/* Set a breakpoint. */
	//bk=d.Breakpoint("test_target.cc",15);
	
//		bk=d.Breakpoint("/home/henry/shelf/cpp/wss/main.cpp",75);
/*
		bk=d.Breakpoint("/home/henry/shelf/cpp/wss/lamb.cpp",1032);
		if (!bk){
			printf("Error setting breakpoint\n");
			return 1;
		}
//		printf("Breakpoint %d @ function: %s\n",bk->number,bk->func);
	
	
	/*You can do things like:
		gmi_break_delete(h,bk->number);
		gmi_break_set_times(h,bk->number,2);
		gmi_break_set_condition(h,bk->number,"1");
		gmi_break_state(h,bk->number,0);
	*/
	
	/* If we no longer need this data we can release it. */
	
	//	mi_free_bkpt(bk);
		
	/*
		bk = d.Breakpoint("les1.cpp",7);
		printf("Breakpoint %d @ function: %s\n",bk->number,bk->func);
		
		mi_free_bkpt(bk);
	*/
	
	/* Set a watchpoint, that's a data breakpoint. */
	
	/*
		wp=d.Watchpoint(wm_write,"v");
		if (!wp){
			printf("Error al setting watchpoint\n");
			return 1;
		}
		printf("Watchpoint %d for expression: %s\n",wp->number,wp->exp);
		mi_free_wp(wp);
	*/
//		TryRun(d.StepOver(),d);
		/* Run the program. */
		
	//	sleep(5);
		TryRun(d.RunOrContinue(),d);
		/* Here we should be stopped at the breakpoint. */
			return 0;
	}
	else if ( s== "s") {	
			std::cout << "continue\n";
	//	sleep(5);
		/* Continue execution. */
		TryRun(d.StepOver(false),d);
//		TryRun(d.RunOrContinue(),d);
	//	sleep(5);
	
	/* Here we should be terminated. */
	
	/* Wait 5 seconds and close the auxiliar terminal. */
//		printf("Waiting 5 seconds\n");
//		sleep(10);
	
		return 0;
	}
	else if ( s == "r"){
		TryRun(d.RunOrContinue(),d);
	}
	else if ( s == "e"){
		TryRun(d.TargetUnselect(),d);
		TryRun(d.Disconnect(),d);
	}
}


