/*
   Copyright 2015 Bloomberg Finance L.P.
  
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
       http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and 
   limitations under the License.
 */

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/time.h>
#include <sys/types.h>
#include <poll.h>
#include <unistd.h>

#include <lockassert.h>
#include <plink.h>
#include "plbitlib.h"


int gbl_mynode = 0;


/* read defaults file? read paulbitscfg.dir? comdb2_local.lrl? */

int paulbits_init(int max) {
    return 0;
}

int comdb2_shm_master_set(int db, int master) {
    /* no-op */
    return 0;
}

enum FASTSEEDPARAMS
{
	 MCHSHIFT=18
	,MAXNODE=(1<<(32-MCHSHIFT))-1 /* max node = 16383 */
	,MAXDUP=(1<<MCHSHIFT)-1       /* max dupe = 262143 */
};


int machine_(void) {
    return gbl_mynode;
}

static pthread_mutex_t fastseedlk = PTHREAD_MUTEX_INITIALIZER;
static int fastseed_last_epoch = 0;
static int fastseed_dup = 0;

static inline int fastseed_get_lastepoch(void) {
    return fastseed_last_epoch;
}

static inline void fastseed_set_lastepoch(int epoch) {
    fastseed_last_epoch = epoch;
}

static inline int fastseed_get_dup(void) {
    return fastseed_dup;
}

static inline void fastseed_set_dup(int dup) {
    fastseed_dup = dup;
}

int fastseed( int seed[2] )
{
	int epoch, node, dup;
	int firstepoch = 0;
	int retries;

	node=machine_();
	if (node<1 || node>MAXNODE)
	{
		fprintf(stderr,"err:fastseed:bad machine number %d, must be 1<=x<=%d\n"
		,node, MAXNODE);
		seed[0]=seed[1]=0;
		return -1;
	}

	retries=0;
	do
	{
        assert_pthread_mutex_lock(&fastseedlk);
		epoch=time_epoch();
		if (epoch==0) /* uh oh.. something broken */
		{
			assert_pthread_mutex_unlock(&fastseedlk);
			fprintf(stderr,"err:fastseed:zero epoch! epoch can't be 0!\n");
			seed[0]=seed[1]=0;
			return -1;
		}
		if (fastseed_get_lastepoch() != epoch)
		{
			/* this is different epoch, so we're good */
			dup=0;
			fastseed_set_dup(0);
			fastseed_set_lastepoch(epoch);
			break;/* got one*/
		}
		dup=fastseed_get_dup()+1;
		if (dup<MAXDUP)
		{
			/* requested more than once in this second, so increment dupe */
			fastseed_set_dup(dup);
			break;
		}
		assert_pthread_mutex_unlock(&fastseedlk);

		epoch=time_epoch();
		if (retries==0) firstepoch=epoch;

		if (retries>8 && firstepoch==epoch)
		{
			fprintf(stderr,"fastseed():ERROR! EPOCH IS NOT UPDATING!!! original %d now %d\n"
			, firstepoch, epoch);
			seed[0]=seed[1]=0;
			return -99;
		}
		if (retries>20)
		{
			fprintf(stderr,"fastseed():ERROR! HIGH CONTENTION, CANNOT GET A SEED!\n");
			seed[0]=seed[1]=0;
			return -98;
		}

		fprintf(stderr,"warn:fastseed():reached dupe limit, waiting for next second, retries %d epoch %d.\n",retries, epoch);
		poll(NULL, 0, 250);
		++retries;
	}
	while (1);

/* if here, got 1, and lock is still held */

	assert_pthread_mutex_unlock(&fastseedlk);

	seed[0]=epoch;
	seed[1]=(node << MCHSHIFT) | (dup & MAXDUP);
	return 0;
}

/* f77 wrapper */
void fastseed_(int seed[2], int *rcodep)
{
	*rcodep=fastseed(seed);
}

uint64_t bbipc_global_fastseed(void) {
    return fastseed64();
}

extern char* ___plink_constants[PLINK_____END];
const char *plink_constant(int which) {
    if (which < 0 || which >= PLINK_____END)
        return NULL;
    return ___plink_constants[which];
}

/* TODO:  this is TEMPORARY */
int disable_rmt_dbupdates(int mach) {
    return 0;
}

int rtcpu_is_cpu_down(int node) {
    return 0;
}

int rtcpu_is_cpu_up(int node) {
    return 1;
}

int getcomputerroom_(int *node) {
    return 0;
}

int getlclbfpoolwidthbigsnd(void) {
    return 16*1024-1;
}
