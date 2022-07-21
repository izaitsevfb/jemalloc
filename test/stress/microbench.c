#include "test/jemalloc_test.h"
#include "test/bench.h"

static inline void
time_single_func(
    uint64_t nwarmup, uint64_t niter, const char *name_a, void(*func_a)) {
	timedelta_t timer_a;
	void *p;

	p = mallocx(1, 0);
	if (p == NULL) {
		test_fail("Unexpected mallocx() failure");
		return;
	}

	time_func(&timer_a, nwarmup, niter, func_a);

	uint64_t usec_a = timer_usec(&timer_a);
	char buf_a[FMT_NSECS_BUF_SIZE];
	fmt_nsecs(usec_a, niter, buf_a);

	malloc_printf("%"FMTu64" iterations, %s=%"FMTu64"us (%s ns/iter), ",
	    niter, name_a, usec_a, buf_a);

	dallocx(p, 0);
}

static void
malloc_free(void) {
	/* The compiler can optimize away free(malloc(1))! */
	void *p = malloc(1);
	if (p == NULL) {
		test_fail("Unexpected malloc() failure");
		return;
	}
	free(p);
}

static void * ptrs[256];

static void
malloc_free_256(void) {
	/* The compiler can optimize away free(malloc(1))! */

	for (size_t i = 0; i < 256; i++) {
		void *p = malloc(1);
		if (p == NULL) {
			test_fail("Unexpected malloc() failure");
			return;
		}
		ptrs[i] = p;
	}

	for (size_t i = 0; i < 256; i++) {
		free(ptrs[i]);
	}
}

TEST_BEGIN(bench_malloc_free) {
	time_single_func(
	    100 * 1000, 100 * 1000, "malloc_free_256", malloc_free_256);
}
TEST_END
//TEST_BEGIN(bench_malloc_free) {
//	time_single_func(
//	    10 * 1000 * 1000, 10 * 1000 * 1000, "malloc_free", malloc_free);
//}
//TEST_END

int
main(void) {
	return test_no_reentrancy(bench_malloc_free);
}
