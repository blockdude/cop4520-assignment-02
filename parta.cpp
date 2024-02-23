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

struct labyrinth
{
	std::mutex *mtx;
	std::condition_variable cv;

	int current_guest;
	int cupcake_available;
	int party_over;
};

struct guest
{
	int id;

	// stats for all guests
	int refilled;
	int entered_labyrinth;
	int cupcakes_eaten;
};

void init_labyrinth( struct labyrinth *l, std::mutex *mtx )
{
	if ( l == NULL )
		return;

	l->mtx = mtx;
	l->current_guest = -1;
	l->cupcake_available = 1;
	l->party_over = 0;
}

void init_guest( struct guest *g, int id )
{
	if ( g == NULL )
		return;

	g->id = id;
	g->refilled = 0;
	g->entered_labyrinth = 0;
	g->cupcakes_eaten = 0;
}

void set_labyrinth_guest( struct labyrinth *l, int id )
{
	l->mtx->lock();
	l->current_guest = id;
	l->mtx->unlock();
}

void guest_logic( int id, struct labyrinth *l )
{
	// create guest data
	struct guest g;
	init_guest( &g, id );

	while ( 1 )
	{
		std::unique_lock< std::mutex > lk( *l->mtx );
		l->cv.wait( lk, [ &l, &id ]{ return l->current_guest == id || l->party_over; } );

		// check what triggered the wait
		if ( l->party_over )
			break;

		// logic for counter guest
		if ( g.id == 0 )
		{
			// consume cupcake
			if ( l->cupcake_available )
			{
				l->cupcake_available = 0;
				g.cupcakes_eaten += 1;
			}

			// determine if all guests have entered
			if ( g.cupcakes_eaten == GUEST_COUNT )
			{
				l->party_over = 1;
			}
		}

		// logic for other guests
		else
		{
			// request cupcake
			if ( l->cupcake_available == 0 && g.refilled == 0 )
			{
				g.refilled += 1;
				l->cupcake_available = 1;
			}
		}

		// reset guest
		g.entered_labyrinth = 1;
		l->current_guest = -1;

		// tell threads that guest is done
		lk.unlock();
		l->cv.notify_all();
	}
}

void invite_guest( struct labyrinth *l )
{
	int next_guest = rand() % GUEST_COUNT;

	// assign next guest
	set_labyrinth_guest( l, next_guest );
	l->cv.notify_all();

	// wait for guest to exit
	std::unique_lock< std::mutex > lk( *l->mtx );
	l->cv.wait( lk, [ &l ]{ return l->current_guest == -1; } );
}

void start_party( void )
{
	// declair stuff
	std::mutex mtx;
	struct labyrinth lab;
	std::thread guests[ GUEST_COUNT ];

	// init stuff
	init_labyrinth( &lab, &mtx );
	srand( time( 0 ) );

	// start each guest thread
	for ( int i = 0; i < GUEST_COUNT; i++ )
		guests[ i ] = std::thread( guest_logic, i, &lab );

	// continue to invite guests till a guest determines that
	// everyone has entered the labrinth
	while ( !lab.party_over )
		invite_guest( &lab );

	// wait for each thread to finish
	for ( int i = 0; i < GUEST_COUNT; i++ )
		guests[ i ].join();

	printf( "All guests have visited the labyrinth.\n" );
}

int main( void )
{
	start_party();
	return 0;
}
