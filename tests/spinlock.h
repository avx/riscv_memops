#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

typedef unsigned int spinlock_t;

static inline void spin_lock(spinlock_t *lock)
{
    while(__sync_lock_test_and_set(lock, 1) != 0);
    __sync_synchronize();
}

static inline void spin_unlock(spinlock_t *lock)
{
    __sync_synchronize();
    __sync_lock_release(lock);
}

#endif /* __SPINLOCK_H__ */
