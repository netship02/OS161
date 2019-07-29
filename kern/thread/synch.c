/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
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
 * Synchronization primitives.
 * The specifications of the functions are in synch.h.
 */

#include <types.h>
#include <lib.h>
#include <spinlock.h>
#include <wchan.h>
#include <thread.h>
#include <current.h>
#include <synch.h>

////////////////////////////////////////////////////////////
//
// Semaphore.

struct semaphore *
sem_create(const char *name, unsigned initial_count)
{
	struct semaphore *sem;

	sem = kmalloc(sizeof(*sem));
	if (sem == NULL) {
		return NULL;
	}

	sem->sem_name = kstrdup(name);
	if (sem->sem_name == NULL) {
		kfree(sem);
		return NULL;
	}

	sem->sem_wchan = wchan_create(sem->sem_name);
	if (sem->sem_wchan == NULL) {
		kfree(sem->sem_name);
		kfree(sem);
		return NULL;
	}

	spinlock_init(&sem->sem_lock);
	sem->sem_count = initial_count;

	return sem;
}

void
sem_destroy(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	/* wchan_cleanup will assert if anyone's waiting on it */
	spinlock_cleanup(&sem->sem_lock);
	wchan_destroy(sem->sem_wchan);
	kfree(sem->sem_name);
	kfree(sem);
}

void
P(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	/*
	 * May not block in an interrupt handler.
	 *
	 * For robustness, always check, even if we can actually
	 * complete the P without blocking.
	 */
	KASSERT(curthread->t_in_interrupt == false);

	/* Use the semaphore spinlock to protect the wchan as well. */
	spinlock_acquire(&sem->sem_lock);
	while (sem->sem_count == 0) {
		/*
		 *
		 * Note that we don't maintain strict FIFO ordering of
		 * threads going through the semaphore; that is, we
		 * might "get" it on the first try even if other
		 * threads are waiting. Apparently according to some
		 * textbooks semaphores must for some reason have
		 * strict ordering. Too bad. :-)
		 *
		 * Exercise: how would you implement strict FIFO
		 * ordering?
		 */
		wchan_sleep(sem->sem_wchan, &sem->sem_lock);
	}
	KASSERT(sem->sem_count > 0);
	sem->sem_count--;
	spinlock_release(&sem->sem_lock);
}

void
V(struct semaphore *sem)
{
	KASSERT(sem != NULL);

	spinlock_acquire(&sem->sem_lock);

	sem->sem_count++;
	KASSERT(sem->sem_count > 0);
	wchan_wakeone(sem->sem_wchan, &sem->sem_lock);

	spinlock_release(&sem->sem_lock);
}

////////////////////////////////////////////////////////////
//
// Lock.

struct lock *
lock_create(const char *name)
{
	struct lock *lock;

	lock = kmalloc(sizeof(*lock));
	if (lock == NULL) {
		return NULL;
	}

	lock->lk_name = kstrdup(name);
	if (lock->lk_name == NULL) {
		kfree(lock);
		return NULL;
	}

	HANGMAN_LOCKABLEINIT(&lock->lk_hangman, lock->lk_name);

	lock->lk_wchan = wchan_create(lock->lk_name);
	if (lock->lk_wchan == NULL){
		kfree(lock->lk_name);
		kfree(lock);
		return NULL;
	}

	spinlock_init(&lock->lk_spinlock);
	lock->lk_status = false;

	return lock;
}

void
lock_destroy(struct lock *lock)
{
	KASSERT(lock != NULL);
	KASSERT(lock->lk_status == false);

	/* wchan_cleanup will assert if anyone's waiting on it */
	spinlock_cleanup(&lock->lk_spinlock);
	wchan_destroy(lock->lk_wchan);
	kfree(lock->lk_name);
	kfree(lock);
}

void
lock_acquire(struct lock *lock)
{
	KASSERT(lock != NULL);

	/*
	 * May not block in an interrupt handler.
	 */

	KASSERT(curthread->t_in_interrupt == false);

	HANGMAN_WAIT(&curthread->t_hangman, &lock->lk_hangman);
	spinlock_acquire(&lock->lk_spinlock);
	HANGMAN_ACQUIRE(&curthread->t_hangman, &lock->lk_hangman);

	while (lock->lk_status) {
		wchan_sleep(lock->lk_wchan, &lock->lk_spinlock);
	}

	KASSERT(lock->lk_status == false);
	lock->lk_status = true;
	lock->lk_holder = curthread;
	spinlock_release(&lock->lk_spinlock);
}

void
lock_release(struct lock *lock)
{

	KASSERT(lock != NULL);
	KASSERT(lock->lk_status == true);
	KASSERT(lock->lk_holder == curthread);

	spinlock_acquire(&lock->lk_spinlock);
	lock->lk_status = false;
	lock->lk_holder = NULL;
	wchan_wakeone(lock->lk_wchan, &lock->lk_spinlock);

	spinlock_release(&lock->lk_spinlock);
	HANGMAN_RELEASE(&curthread->t_hangman, &lock->lk_hangman);
}

bool
lock_do_i_hold(struct lock *lock)
{

	return lock->lk_holder == curthread;
}

////////////////////////////////////////////////////////////
//
// CV


struct cv *
cv_create(const char *name)
{
	struct cv *cv;

	cv = kmalloc(sizeof(*cv));
	if (cv == NULL) {
		return NULL;
	}

	cv->cv_name = kstrdup(name);
	if (cv->cv_name==NULL) {
		kfree(cv);
		return NULL;
	}

	cv->cv_wchan = wchan_create(cv->cv_name);
	if (cv->cv_wchan == NULL){
		kfree(cv->cv_name);
		kfree(cv);
		return NULL;
	}

	spinlock_init(&cv->cv_spinlock);
	return cv;
}

void
cv_destroy(struct cv *cv)
{
	KASSERT(cv != NULL);

	spinlock_cleanup(&cv->cv_spinlock);
	wchan_destroy(cv->cv_wchan);
	kfree(cv->cv_name);
	kfree(cv);
}

void
cv_wait(struct cv *cv, struct lock *lock)
{
	KASSERT(cv != NULL);
	KASSERT(lock != NULL);
	KASSERT(lock_do_i_hold(lock));

	spinlock_acquire(&cv->cv_spinlock);

	lock_release(lock);
	wchan_sleep(cv->cv_wchan, &cv->cv_spinlock);	

	// why this is not working
	// 		spinlock_release(&cv->cv_spinlock);
	// 		lock_acquire(lock);

	spinlock_release(&cv->cv_spinlock);
	lock_acquire(lock);
	
}

void
cv_signal(struct cv *cv, struct lock *lock)
{
	KASSERT(cv != NULL);
	KASSERT(lock != NULL);
	KASSERT(lock_do_i_hold(lock));

	spinlock_acquire(&cv->cv_spinlock);

	wchan_wakeone(cv->cv_wchan, &cv->cv_spinlock);

	spinlock_release(&cv->cv_spinlock);

}


void
cv_broadcast(struct cv *cv, struct lock *lock)
{

	KASSERT(cv != NULL);
	KASSERT(lock != NULL);
	KASSERT(lock_do_i_hold(lock));

	spinlock_acquire(&cv->cv_spinlock);

	wchan_wakeall(cv->cv_wchan, &cv->cv_spinlock);

	spinlock_release(&cv->cv_spinlock);
}


////////////////////////////////////////////////////////////
//
// RW

struct rwlock * 
rwlock_create(const char * name) {

	struct rwlock *rw;

	rw = kmalloc(sizeof(*rw));
	if (rw == NULL) {
		return NULL;
	}

	rw->rwlock_name = kstrdup(name);
	if (rw->rwlock_name == NULL) {
		kfree(rw);
		return NULL;
	}

	rw->rwlock_oktoread = cv_create(name);
	if (rw->rwlock_oktoread == NULL){
		kfree(rw->rwlock_name);
		kfree(rw);
		return NULL;
	}

	rw->rwlock_oktowrite =  cv_create(name);
	if (rw->rwlock_oktowrite == NULL) {
		kfree(rw->rwlock_name);
		kfree(rw);
		return NULL;	
	}

	lock_create(name);
	return rw;
}


void 
rwlock_destroy(struct rwlock *rw) {
	KASSERT(rw != NULL);

	lock_destroy(rw->rw_lock);	
	cv_destroy(rw->rwlock_oktowrite);
	cv_destroy(rw->rwlock_oktowrite);
	kfree(rw->rwlock_name);
	kfree(rw);
}


void 
rwlock_acquire_read(struct rwlock *rw) {
	KASSERT(rw != NULL);

	lock_acquire(rw->rw_lock);

	cv_wait(rw->rwlock_oktoread, rw->rw_lock);

	lock_release(rw->rw_lock);
}


void 
rwlock_release_read(struct rwlock *rw) {
	KASSERT(rw != NULL);

	lock_acquire(rw->rw_lock);

	cv_signal(rw->rwlock_oktoread, rw->rw_lock);

	lock_release(rw->rw_lock);
}


void rwlock_acquire_write(struct rwlock *rw) {
	KASSERT(rw != NULL);

	lock_acquire(rw->rw_lock);

	cv_wait(rw->rwlock_oktowrite, rw->rw_lock);

	lock_release(rw->rw_lock);
}


void rwlock_release_write(struct rwlock *rw) {
	KASSERT(rw != NULL);

	lock_acquire(rw->rw_lock);

	cv_signal(rw->rwlock_oktowrite, rw->rw_lock);

	lock_release(rw->rw_lock);
}
