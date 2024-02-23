#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <condition_variable>

#define GUEST_COUNT 8
#define MAX_VIEW_COUNT 50

struct showroom
{
	std::mutex *mtx;
	std::condition_variable cv;

	int current_guest;
	int party_over;
	int busy;
	int views;
};

struct guest
{
	int id;

	// stats for all guests
	int viewed_vase;
};

void init_showroom( struct showroom *l, std::mutex *mtx )
{
	if ( l == NULL )
		return;

	l->mtx = mtx;
	l->current_guest = -1;
	l->party_over = 0;
	l->busy = 0;
	l->views = 0;
}

void init_guest( struct guest *g, int id )
{
	if ( g == NULL )
		return;

	g->id = id;
	g->viewed_vase = 0;
}

void set_showroom_guest( struct showroom *l, int id )
{
	l->mtx->lock();
	l->current_guest = id;
	l->mtx->unlock();
}

void set_showroom_busy( struct showroom *l, int b )
{
	l->mtx->lock();
	l->busy = b;
	l->mtx->unlock();
}

void guest_logic( int id, struct showroom *l )
{
	// create guest data
	struct guest g;
	init_guest( &g, id );

	while ( 1 )
	{
		// wait to be invited
		std::unique_lock< std::mutex > lk( *l->mtx );
		l->cv.wait( lk, [ &l, &id ]{ return l->current_guest == id || l->party_over; } );

		// check what triggered the wait
		if ( l->party_over )
			break;

		l->views += 1;

		// reset guest
		g.viewed_vase += 1;
		l->current_guest = -1;
		l->busy = 0;

		// stop party once the vase has been viewed N times
		if ( l->views > 50 )
			l->party_over = 1;

		// tell threads that guest is done
		lk.unlock();
		l->cv.notify_all();
	}

	printf( "Guest %d viewed the vase %d times\n", g.id, g.viewed_vase );
}

void select_guest( struct showroom *l )
{
	int next_guest = rand() % GUEST_COUNT;

	// randomly select guest
	set_showroom_guest( l, next_guest );
	set_showroom_busy( l, 1 );
	l->cv.notify_all();

	// wait for guest to exit
	std::unique_lock< std::mutex > lk( *l->mtx );
	l->cv.wait( lk, [ &l ]{ return l->current_guest == -1; } );
}

void start_party( void )
{
	// declair stuff
	std::mutex mtx;
	struct showroom sr;
	std::thread guests[ GUEST_COUNT ];

	// init stuff
	init_showroom( &sr, &mtx );
	srand( time( 0 ) );

	// start each guest thread
	for ( int i = 0; i < GUEST_COUNT; i++ )
		guests[ i ] = std::thread( guest_logic, i, &sr );

	// continue party till vase has been view 50 times
	while ( !sr.party_over )
		select_guest( &sr );

	// wait for each thread to finish
	for ( int i = 0; i < GUEST_COUNT; i++ )
		guests[ i ].join();
}

int main( void )
{
	start_party();
	return 0;
}
