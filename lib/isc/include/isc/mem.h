/*
 * Copyright (C) 1997-2000  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* $Id: mem.h,v 1.38 2000/07/26 19:06:20 explorer Exp $ */

#ifndef ISC_MEM_H
#define ISC_MEM_H 1

#include <stdio.h>

#include <isc/lang.h>
#include <isc/mutex.h>
#include <isc/types.h>

ISC_LANG_BEGINDECLS

typedef void * (*isc_memalloc_t)(void *, size_t);
typedef void (*isc_memfree_t)(void *, void *);

/*
 * ISC_MEM_DEBUG is defined by default; define ISC_MEM_DEBUGOFF to disable it.
 */
#if !defined(ISC_MEM_DEBUG) && !defined(ISC_MEM_DEBUGOFF)
#define ISC_MEM_DEBUG
#endif

/*
 * Define ISC_MEM_TRACKLINES to turn on detailed tracing of memory allocation
 * and freeing by file and line number.
 */
#if 0
#define ISC_MEM_TRACKLINES
#endif

/*
 * Define ISC_MEM_CHECKOVERRUN to turn on checks for using memory outside
 * the requested space.  This will increase the size of each allocation.
 */
#if 0
#define ISC_MEM_CHECKOVERRUN
#endif

/*
 * Define ISC_MEM_FILL to fill each block of memory returned to the system
 * with the byte string '0xbe'.  This helps track down uninitialized pointers
 * and the like.  On freeing memory, the space is filled with '0xde' for
 * the same reasons.
 */
#define ISC_MEM_FILL

/*
 * Define this to turn on memory pool names.
 */
#define ISC_MEMPOOL_NAMES

/*
 * _DEBUGTRACE
 *	log (to isc_lctx) each allocation and free.
 *
 * _DEBUGRECORD
 *	remember each allocation, and match them up on free.  Crash if
 *	a free doesn't match an allocation
 */
extern unsigned int isc_mem_debugging;
#define ISC_MEM_DEBUGTRACE		0x00000001U
#define ISC_MEM_DEBUGRECORD		0x00000002U

#ifdef ISC_MEM_TRACKLINES
#define _ISC_MEM_FILELINE	, __FILE__, __LINE__
#define _ISC_MEM_FLARG		, const char *, int
#else
#define _ISC_MEM_FILELINE
#define _ISC_MEM_FLARG
#endif

#define isc_mem_get(c, s)	isc__mem_get((c), (s) _ISC_MEM_FILELINE)
#define isc_mem_allocate(c, s)	isc__mem_allocate((c), (s) _ISC_MEM_FILELINE)
#define isc_mem_strdup(c, p)	isc__mem_strdup((c), (p) _ISC_MEM_FILELINE)
#define isc_mempool_get(c)	isc__mempool_get((c) _ISC_MEM_FILELINE)

#ifdef ISC_MEM_DEBUG
#define isc_mem_put(c, p, s) \
	do { \
		isc__mem_put((c), (p), (s) _ISC_MEM_FILELINE); \
		(p) = NULL; \
	} while (0)
#define isc_mem_free(c, p) \
	do { \
		isc__mem_free((c), (p) _ISC_MEM_FILELINE); \
		(p) = NULL; \
	} while (0)
#define isc_mempool_put(c, p) \
	do { \
		isc__mempool_put((c), (p) _ISC_MEM_FILELINE); \
		(p) = NULL; \
	} while (0)
#else
#define isc_mem_put(c, p, s)	isc__mem_put((c), (p), (s) _ISC_MEM_FILELINE)
#define isc_mem_free(c, p)	isc__mem_free((c), (p) _ISC_MEM_FILELINE)
#define isc_mempool_put(c, p)	isc__mempool_put((c), (p) _ISC_MEM_FILELINE)
#endif

isc_result_t			isc_mem_create(size_t, size_t, isc_mem_t **);
void				isc_mem_attach(isc_mem_t *, isc_mem_t **);
void				isc_mem_detach(isc_mem_t **);
void				isc_mem_destroy(isc_mem_t **);
isc_result_t			isc_mem_ondestroy(isc_mem_t *ctx,
						  isc_task_t *task,
						  isc_event_t **event);
void *				isc__mem_get(isc_mem_t *, size_t
					     _ISC_MEM_FLARG);
void 				isc__mem_put(isc_mem_t *, void *,
					     size_t _ISC_MEM_FLARG);
isc_result_t			isc_mem_preallocate(isc_mem_t *);
void 				isc_mem_stats(isc_mem_t *, FILE *);
isc_boolean_t			isc_mem_valid(isc_mem_t *, void *);
void *				isc__mem_allocate(isc_mem_t *, size_t
						  _ISC_MEM_FLARG);
void				isc__mem_free(isc_mem_t *, void *
					      _ISC_MEM_FLARG);
char *				isc__mem_strdup(isc_mem_t *, const char *
						_ISC_MEM_FLARG);
void				isc_mem_setdestroycheck(isc_mem_t *,
							isc_boolean_t);
void				isc_mem_setsplit(isc_mem_t *, isc_boolean_t);
void				isc_mem_setquota(isc_mem_t *, size_t);
size_t				isc_mem_getquota(isc_mem_t *);
size_t				isc_mem_inuse(isc_mem_t *);

isc_result_t			isc_mem_createx(size_t, size_t,
						isc_memalloc_t memalloc,
						isc_memfree_t memfree,
						void *arg, isc_mem_t **);
isc_result_t			isc_mem_restore(isc_mem_t *);

/*
 * Memory pools
 */

/*
 * Internal (but public) functions.  Don't call these from application
 * code.  Use isc_mempool_get() and isc_mempool_put() instead.
 */
void *		isc__mempool_get(isc_mempool_t * _ISC_MEM_FLARG);
void 		isc__mempool_put(isc_mempool_t *, void * _ISC_MEM_FLARG);

isc_result_t
isc_mempool_create(isc_mem_t *mctx, size_t size, isc_mempool_t **mpctxp);
/*
 * Create a memory pool.
 *
 * Requires:
 *	mctx is a valid memory context.
 *	size > 0
 *	mpctxp != NULL and *mpctxp == NULL
 *
 * Defaults:
 *	maxalloc = UINT_MAX
 *	freemax = 1
 *	fillcount = 1
 *
 * Returns:
 *	ISC_R_NOMEMORY		-- not enough memory to create pool
 *	ISC_R_SUCCESS		-- all is well.
 */

void
isc_mempool_destroy(isc_mempool_t **mpctxp);
/*
 * Destroy a memory pool.
 *
 * Requires:
 *	mpctxp != NULL && *mpctxp is a valid pool.
 *	The pool has no un"put" allocations outstanding
 */

void
isc_mempool_setname(isc_mempool_t *mpctx, const char *name);
/*
 * Associate a name with a memory pool.  At most 15 characters may be used.
 *
 * Requires:
 *	mpctx is a valid pool.
 *	name != NULL;
 */

void
isc_mempool_associatelock(isc_mempool_t *mpctx, isc_mutex_t *lock);
/*
 * Associate a lock with this memory pool.
 *
 * This lock is used when getting or putting items using this memory pool,
 * and it is also used to set or get internal state via the isc_mempool_get*()
 * and isc_mempool_set*() set of functions.
 *
 * Mutiple pools can each share a single lock.  For instance, if "manager"
 * type object contained pools for various sizes of events, and each of
 * these pools used a common lock.  Note that this lock must NEVER be used
 * by other than mempool routines once it is given to a pool, since that can
 * easily cause double locking.
 *
 * Requires:
 *
 *	mpctpx is a valid pool.
 *
 *	lock != NULL.
 *
 *	No previous lock is assigned to this pool.
 *
 *	The lock is initialized before calling this function via the normal
 *	means of doing that.
 */

/*
 * The following functions get/set various parameters.  Note that due to
 * the unlocked nature of pools these are potentially random values unless
 * the imposed externally provided locking protocols are followed.
 *
 * Also note that the quota limits will not always take immediate effect.
 * For instance, setting "maxalloc" to a number smaller than the currently
 * allocated count is permitted.  New allocations will be refused until
 * the count drops below this threshold.
 *
 * All functions require (in addition to other requirements):
 *	mpctx is a valid memory pool
 */

unsigned int
isc_mempool_getfreemax(isc_mempool_t *mpctx);
/*
 * Returns the maximum allowed size of the free list.
 */

void
isc_mempool_setfreemax(isc_mempool_t *mpctx, unsigned int limit);
/*
 * Sets the maximum allowed size of the free list.
 */

unsigned int
isc_mempool_getfreecount(isc_mempool_t *mpctx);
/*
 * Returns current size of the free list.
 */

unsigned int
isc_mempool_getmaxalloc(isc_mempool_t *mpctx);
/*
 * Returns the maximum allowed number of allocations.
 */

void
isc_mempool_setmaxalloc(isc_mempool_t *mpctx, unsigned int limit);
/*
 * Sets the maximum allowed number of allocations.
 *
 * Additional requirements:
 *	limit > 0
 */

unsigned int
isc_mempool_getallocated(isc_mempool_t *mpctx);
/*
 * Returns the number of items allocated from this pool.
 */

unsigned int
isc_mempool_getfillcount(isc_mempool_t *mpctx);
/*
 * Returns the number of items allocated as a block from the parent memory
 * context when the free list is empty.
 */

void
isc_mempool_setfillcount(isc_mempool_t *mpctx, unsigned int limit);
/*
 * Sets the fillcount.
 *
 * Additional requirements:
 *	limit > 0
 */

ISC_LANG_ENDDECLS

#endif /* ISC_MEM_H */
