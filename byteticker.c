
/* getconf LFS_CFLAGS */

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <locale.h>

void *byteticker(void *);
int bytepiper(unsigned long long int bufsize);
void byebye(int);
char *speedstr(double bps);

void usage(int argc, char **argv)
{
	char *cp;

	if(cp = strrchr(argv[0], '/')) cp++;
	else cp = argv[0];
	fprintf(stderr, "usage: %s <bufsize>\n", cp);
	exit(1);
}

pthread_t tickerthread;

int main(int argc, char **argv)
{
	char *cp;
	unsigned long long int bufsiz = 1;
	char s[1024];
	pthread_t th;

  setlocale(LC_NUMERIC, "");

	if(argc < 2)
	{
		usage(argc, argv);
	}

	signal(SIGINT, byebye);

	strncpy(s, argv[1], 1023);

	if(cp = strchr(s, '\0')) { cp--; }
	if(cp && !isdigit(*cp))
	{
		switch(*cp)
		{
			case 'P': case 'p': bufsiz *= 1024;
			case 'T': case 't': bufsiz *= 1024;
			case 'G': case 'g': bufsiz *= 1024;
			case 'M': case 'm': bufsiz *= 1024;
			case 'K': case 'k': bufsiz *= 1024; break;
			default: usage(argc, argv);
			
		}
		*cp = '\0';
	}
	bufsiz *= atoi(s);
//	return 0;

	pthread_create(&th, NULL, byteticker, NULL);

	bytepiper(bufsiz);

	pthread_join(th, NULL);

	return 0;
}

static struct timespec tp_start;
static struct timespec tp_stop;

long starttimer()
{
	clock_gettime(CLOCK_REALTIME, &tp_start);
	return 1;
}

unsigned long long stoptimer()
{
	clock_gettime(CLOCK_REALTIME, &tp_stop);
	
	return ( ( tp_stop.tv_sec - tp_start.tv_sec ) * 1000000000 ) \
		+ ( tp_stop.tv_nsec - tp_start.tv_nsec );
}

void nsleep(int ns)
{
	struct timespec ts;
	ts.tv_nsec = ns;
	ts.tv_sec = 0;
	nanosleep(&ts, NULL);
}

#define double1 (              1.0 )
#define doubleK ( double1 * 1024.0 )
#define doubleM ( doubleK * 1024.0 )
#define doubleG ( doubleM * 1024.0 )
#define doubleT ( doubleG * 1024.0 )
#define doubleP ( doubleT * 1024.0 )
#define doubleX ( doubleP * 1024.0 )
#define doubleZ ( doubleX * 1024.0 )
#define doubleY ( doubleZ * 1024.0 )

long whichsifx(double x, double *d)
{
//	fprintf(stderr, "\nwhichsifx: x = %g, doubleK=%g\n", x, doubleK);

	if(x < (doubleK)) { *d = double1; return 0; } // -
	if(x < (doubleM)) { *d = doubleK; return 1; } // K
	if(x < (doubleG)) { *d = doubleM; return 2; } // M
	if(x < (doubleT)) { *d = doubleG; return 3; } // G
	if(x < (doubleP)) { *d = doubleT; return 4; } // T
	if(x < (doubleX)) { *d = doubleP; return 5; } // P
	if(x < (doubleZ)) { *d = doubleX; return 6; } // X
	if(x < (doubleY)) { *d = doubleZ; return 7; } // Z
	                  { *d = doubleY; return 8; } // Y
}

char *speedstr(double bps)
{
	static char s[1024];
	static long kexp;
	static double divisor;
	static char *sifx[] = {
		" B/s" , // 0 --
		"kB/s" , // 1 -- kilo
		"MB/s" , // 2 -- mega
		"GB/s" , // 3 -- giga
		"TB/s" , // 4 -- tera
		"PB/s" , // 5 -- peta
		"EB/s" , // 6 -- exa
		"ZB/s" , // 7 -- zetta
		"YB/s"   // 8 -- yotta
		};

	kexp = whichsifx(bps, &divisor);

//	fprintf(stderr, "\nkexp = %d, divisor = %g\n", kexp, divisor);

  sprintf(s, "%'5lld %s", (long long)(bps / divisor), sifx[kexp]);
	
	return s;
}

unsigned long long int accumulator = 0;
int finished = 0;

int bytepiper(unsigned long long int bufsize)
{
	int fdin  = STDIN_FILENO;
	int fdout = STDOUT_FILENO;
	unsigned char *buffer = (unsigned char *)malloc(bufsize);
	ssize_t rdv;

	starttimer();
	while( (rdv = read(fdin, buffer, bufsize)) > 0 )
	{
		accumulator += rdv;
		write(fdout, buffer, rdv);
	}

	free(buffer);

	finished = 1;

	return rdv;
}

void *byteticker(void *arg)
{
	double elapsed;
	double speed;

	for( ; finished == 0 ; nsleep(10000000*1))
	{
		elapsed = ( (double)stoptimer() ) / 1000000000;
		speed = ( (double)accumulator )  / elapsed;

		fprintf(stderr, "\r%'20lld bytes ++ %s", accumulator, speedstr(speed));
		fflush(stdout);
	}
	fprintf(stderr, "\n");
}

void byebye(int n)
{
	fprintf(stderr, "\n");
	exit(0);
}

