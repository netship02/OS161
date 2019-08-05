/*
 * Copyright (c) 2001, 2002, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Driver code is in kern/tests/synchprobs.c We will
 * replace that file. This file is yours to modify as you see fit.
 *
 * You should implement your solution to the whalemating problem below.
 */

#include <types.h>
#include <lib.h>
#include <thread.h>
#include <test.h>
#include <synch.h>

static unsigned long male_count;
static unsigned long female_count;
static struct lock *lock_male;
static struct lock *lock_female;
static struct lock *lock_mm;
static struct cv *cv_male;
static struct cv *cv_female;
static struct cv *cv_mm;

/*
 * Called by the driver during initialization.
 */

void whalemating_init() {
	male_count = 0;
	female_count = 0;
	lock_male = lock_create("lock_male");
	lock_female = lock_create("lock_female");
	lock_mm = lock_create("lock_mm");
	cv_male = cv_create("cv_male");
	cv_female = cv_create("cv_female");
	cv_mm = cv_create("cv_mm");
}

/*
 * Called by the driver during teardown.
 */

void
whalemating_cleanup() {
	KASSERT(lock_male != NULL);
	KASSERT(lock_female != NULL);
	KASSERT(lock_mm != NULL);
	KASSERT(cv_male != NULL);
	KASSERT(cv_female != NULL);
	KASSERT(cv_mm != NULL);

	lock_destroy(lock_male);
	lock_destroy(lock_female);
	lock_destroy(lock_mm);
	cv_destroy(cv_male);
	cv_destroy(cv_female);
	cv_destroy(cv_mm);

	lock_male = lock_female = lock_mm = NULL;
	cv_male = cv_female = cv_mm = NULL;
}

void
male(uint32_t index)
{
	KASSERT(lock_male != NULL);
	KASSERT(lock_female != NULL);
	KASSERT(lock_mm != NULL);
	KASSERT(cv_male != NULL);
	KASSERT(cv_female != NULL);
	KASSERT(cv_mm != NULL);

	male_start(index);

	lock_acquire(lock_mm);
	cv_signal(cv_mm, lock_mm);
	lock_release(lock_mm);

	male_count++;

	lock_acquire(lock_male);
	cv_wait(cv_male, lock_male);
	male_end(index);
	lock_release(lock_male);
	
}

void
female(uint32_t index)
{
	KASSERT(lock_male != NULL);
	KASSERT(lock_female != NULL);
	KASSERT(lock_mm != NULL);
	KASSERT(cv_male != NULL);
	KASSERT(cv_female != NULL);
	KASSERT(cv_mm != NULL);

	female_start(index);

	lock_acquire(lock_mm);
	cv_signal(cv_mm, lock_mm);
	lock_release(lock_mm);	

	female_count++;

	lock_acquire(lock_female);
	cv_wait(cv_female, lock_female);

	female_end(index);
	lock_release(lock_female);
}

void
matchmaker(uint32_t index)
{
	KASSERT(lock_male != NULL);
	KASSERT(lock_female != NULL);
	KASSERT(lock_mm != NULL);
	KASSERT(cv_male != NULL);
	KASSERT(cv_female != NULL);
	KASSERT(cv_mm != NULL);

	matchmaker_start(index);


	lock_acquire(lock_mm);
	while (male_count <= 0 || female_count <= 0) {
		cv_wait(cv_mm, lock_mm);
	}


	male_count--;
	female_count--;

	lock_acquire(lock_male);
	cv_signal(cv_male, lock_male);
	lock_release(lock_male);


	lock_acquire(lock_female);
	cv_signal(cv_female, lock_female);
	lock_release(lock_female);

	matchmaker_end(index);
	lock_release(lock_mm);
}