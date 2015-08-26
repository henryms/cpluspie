#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "socket.h"

bool create_tcp_socket(int*);
//char *get_ip(char *host);
bool get_ip(char *host, char*& ip);
char *build_get_query(char *host, char *page);
void usage();

#define HOST "coding.debuntu.org"
#define PAGE "/"
#define PORT 80
#define USERAGENT "LAMB 1.0"

#define BUFSIZZ BUFSIZ 


bool socket_c::get_webpage(string name, string& result)
{
	cout << name << '\n';
	
	struct sockaddr_in *remote;
	int sock;
	int tmpres;
	char *host;
	char *page;
	char *ip;
	char *get;
	char buf[BUFSIZZ+1];
	string s, s1, s2;
	s=name;
	size_t pos = name.find("/");
	if (pos != string::npos){
		s1=name.substr(0,pos);
		s2=name.substr(pos);
	}
	else{
		s1=name;
		s2="/";
	}	
	
	
//		s1 = HOST;
//		s2 = PAGE;		
	host=(char*)s1.c_str();
	page=(char*)s2.c_str();

	cout << "host: " << host << '\n';		
	cout << "page: " << page << '\n';		
								
	create_tcp_socket(&sock);
	if(!get_ip(host,ip)){
		return false;
	}
	fprintf(stderr, "IP is %s\n", ip); 
	remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
	remote->sin_family = AF_INET;
	tmpres = inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr)));
	if( tmpres < 0){
		perror("Can't set remote->sin_addr.s_addr");
		return 0;
	}
	else if(tmpres == 0){
		fprintf(stderr, "%s is not a valid IP address\n", ip);
		return 0;
	}
	remote->sin_port = htons(PORT);
	if(connect(sock, (struct sockaddr *)remote, sizeof(struct sockaddr)) < 0){
		perror("Could not connect");
		return 0;
	}
	get = build_get_query(host, page);
	fprintf(stderr, "Query is:\n<<START>>\n%s<<END>>\n", get);
  
	//Send the query to the server
	int sent = 0;
	while(sent < strlen(get)){ 
		tmpres = send(sock, get+sent, strlen(get)-sent, 0);
		if(tmpres == -1){
			perror("Can't send query");
			return 0;
		}
		sent += tmpres;
	}
	//now it is time to receive the page
	memset(buf, 0, sizeof(buf));
	int htmlstart = 0;
	char * htmlcontent;
	while((tmpres = recv(sock, buf, BUFSIZZ, 0)) > 0){
		if(htmlstart == 0){
			/* Under certain conditions this will not work.
			* If the \r\n\r\n part is splitted into two messages
			* it will fail to detect the beginning of HTML content
			*/
			htmlcontent = strstr(buf, "\r\n\r\n");
			if(htmlcontent != 0){
				htmlstart = 1;
				htmlcontent += 4;
			}
		}
		else{
			htmlcontent = buf;
		}
		if(htmlstart){
			fprintf(stdout,"%s", htmlcontent);
			result+=htmlcontent;
		}
		memset(buf, 0, tmpres);
	}
	if(tmpres < 0)
	{
		perror("Error receiving data");
	}
	free(get);
	free(remote);
	free(ip);
	close(sock);
	return 1;
}

void usage()
{
	fprintf(stderr, "USAGE: htmlget host [page]\n\
	\thost: the website hostname. ex: coding.debuntu.org\n\
	\tpage: the page to retrieve. ex: index.html, default: /\n");
}

bool create_tcp_socket(int* psock)
{
	int& sock=*psock;
	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		perror("Can't create TCP socket");
		return false;
	}
	return true;
}

//char *get_ip(char *host)
bool get_ip(char *host, char*& ip)
{
	struct hostent *hent;
	int iplen = 15; //XXX.XXX.XXX.XXX
//	char *ip = (char *)malloc(iplen+1);
	ip = (char *)malloc(iplen+1);
	memset(ip, 0, iplen+1);
	if((hent = gethostbyname(host)) == NULL){
		herror("Can't get IP");
		return false;
	}
	if(inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL){
		perror("Can't resolve host");
		return false;
	}
	//return ip;
	return true;
}

char *build_get_query(char *host, char *page)
{
	char *query;
	char *getpage = page;
	char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
	if(getpage[0] == '/'){
		getpage = getpage + 1;
		fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
	}
	// -5 is to consider the %s %s %s in tpl and the ending \0
	query = (char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
	sprintf(query, tpl, getpage, host, USERAGENT);
	return query;
}
