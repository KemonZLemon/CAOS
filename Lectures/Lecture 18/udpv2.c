/* udp-server-v2.c */


/* UDP SERVER example:
 *
 * -- socket()             create a socket (endpoint) for communication
 * -- bind()               bind to (assign) a specific port number
 *
 * -- getsockname()        get socket "name" -- IP addr, port number, etc.
 *
 * -- recvfrom()/sendto()  receive/send datagrams (similar to read()/write())
 *
 */


/* To test this server, you can use the following
   command-line netcat tool:
                                    port number
                                        v
   bash$ netcat -u linux-new.cs.rpi.edu 12345
         nc        ^^^^^
                   replace with your hostname

   Note that netcat will act as a client to this UDP server...

   The hostname (e.g., linux-new.cs.rpi.edu) can also be
   localhost (127.0.0.1); and the port number must match what
   the server reports.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define MAXBUFFER 32

/* global socket descriptor -- this will actually be in the fd table */
int sd;

void shutdown_server( int sig )
{
  if ( sig == SIGUSR1 )
  {
    printf( "MAIN: SIGUSR1 received: shutting down the server...\n" );
#if 0
    exit( EXIT_SUCCESS );  /* this shuts down (immediately) all threads */
#else
    close( sd );   /* close the UDP datagram socket descriptor */
    sd = -1;
#endif
  }
}

int main()
{
  signal( SIGINT, SIG_IGN );
  signal( SIGTERM, SIG_IGN );
  signal( SIGUSR2, SIG_IGN );
  signal( SIGUSR1, shutdown_server );

  /* create the socket (endpoint) on the server side */
  sd = socket( AF_INET, SOCK_DGRAM, 0 );
  if ( sd == -1 ) { perror( "socket() failed" ); return EXIT_FAILURE; }

  printf( "Server-side UDP socket created on descriptor %d\n", sd );


  struct sockaddr_in udp_server;
  int length = sizeof( udp_server );

  udp_server.sin_family = AF_INET;  /* IPv4 */
  udp_server.sin_addr.s_addr = htonl( INADDR_ANY );  /* all network interfaces */
  udp_server.sin_port = htons( 12345 );  /* htons( 0 ); */
            /* for bind(), the 0 here means let the OS assign a port number */

  if ( bind( sd, (struct sockaddr *)&udp_server, length ) == -1 )
  {
    perror( "bind() failed" );
    return EXIT_FAILURE;
  }


  /* call getsockname() to obtain the port number that was just assigned */
  if ( getsockname( sd, (struct sockaddr *)&udp_server, (socklen_t *)&length ) == -1 )
  {
    perror( "getsockname() failed" );
    return EXIT_FAILURE;
  }

  printf( "UDP server bound to port %d\n", ntohs( udp_server.sin_port ) );



  /* the code below implements the APPLICATION PROTOCOL */
  while ( 1 )
  {
    char * buffer = calloc( MAXBUFFER + 1, sizeof( char ) );

    struct sockaddr_in remote_client;
    int addrlen = sizeof( remote_client );

    /* read a datagram from the remote client side (BLOCKING) */
    int n = recvfrom( sd, buffer, MAXBUFFER, 0,
                      (struct sockaddr *)&remote_client,
                      (socklen_t *)&addrlen );
    if ( n == -1 && errno == EBADF ) { free( buffer ); buffer = NULL; break; }
    if ( n == -1 ) { perror( "recvfrom() failed" ); continue; }
    if ( n == 0 ) continue;  /* throw the empty datagram away, i.e., ignore */

    /* TO DO: ensure that the first character is either 'D' or 'E' */
    /*        -- if neither, then continue, i.e., ignore faulty datagram */



    /* this ensures that we receive/accept datagrams only from a
     *  specific IP address...
     */
    if ( strcmp( inet_ntoa( remote_client.sin_addr ), "128.113.126.73" ) != 0 )
      continue;

    /* add pthread_create() in here... */



    printf( "Rcvd datagram from %s port %d\n",
            inet_ntoa( remote_client.sin_addr ),
            ntohs( remote_client.sin_port ) );

    printf( "Rcvd %d bytes\n", n );
    *(buffer + n) = '\0';  /* assume this is printable char[] data */
    printf( "Rcvd: \"%s\"\n", buffer );

#if 0
    /* echo the first 3 bytes (at most) plus '\n' back to the client */
    if ( n > 3 ) { n = 4; *(buffer + 3) = '\n'; }
    sendto( sd, buffer, n, 0, (struct sockaddr *)&remote_client, addrlen );
#endif

    short x = 720;
    /* decimal: 720
     *  binary: 0000 0010 1101 0000 (i.e., 512+128+64+16)
     *     hex:    0    2    d    0
     *
     *    big endian (hex):  0x 02 d0
     * little endian (hex):  0x d0 02  (byte order is reversed)
     *                       ^^^^^^^^
     *                 in memory, x is stored as 0xd002
     *
     *                 e.g., if a big endian machine receives 0xd002,
     *                        how does it translate it as a short value?
     *                           1101 0000 0000 0010 ==> 32768+16384+4096+2
     *
     *  to avoid problems where big/little endian machines conflict,
     *   we need to use data marshalling
     *    --- convert everything to network byte order (big endian)
     */
    short z = htons( x );

    sendto( sd, &z, sizeof( short ), 0,
            (struct sockaddr *)&remote_client, addrlen );
    /* what does the remote client side do next,
     *  i.e., when it receives the two bytes?
     *
     *    short data;
     *    data = ntohs( data );
     */

    /* what if we instead had:
     *   short x = -720;     <=== two's complement
     *                              (flip all bits, then add 1)
     * decimal: -720
     *  binary: 0000 0010 1101 0000 (i.e., 720=512+128+64+16)
     *  binary: 1111 1101 0010 1111 <=== flip all bits
     *  binary:                +  1 <=== add 1
     *  binary: 1111 1101 0011 0000 <=== two's complement rep of -720
     *     hex:    f    d    3    0
     *
     *    big endian (hex):  0x fd 30
     * little endian (hex):  0x 30 fd  (byte order is reversed)
     */
  }

  return EXIT_SUCCESS;
}






