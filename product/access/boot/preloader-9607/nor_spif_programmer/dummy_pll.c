#include <caffeine600mg.h>

uint32_t lx_freq = 0, ocp_freq = 0;

static uint32_t probe_lx_freq(void) {
	uint32_t j;
	const uint8_t bdiv[] = {
		148, 141, 135, 128, 121, 114, 108, 101,
		94,  87,  80,  74,  70,  67,  63,  60,
		57,  53,  26,  25,  24,  23,  22,  21,
		20,  19,  17,  16,  15,  14,  13,  12,
		11,  10,  9,   8,   7,   6
	};

	j = 0;
	while (1) {
		otto_NS16550_init(bdiv[j]);

		if ((!is_frame_error()) &&
		    (otto_NS16550_getc() == ' ')) {
			lx_freq = (bdiv[j] + 1) * 16 * BAUDRATE / 1000000;
			break;
		}

		j = (j+1) % sizeof(bdiv);
	}

	return bdiv[j];
}

static void probe_ocp_freq(void) {
	const int8_t qstr[] = "\rProbing CP0 COUNT frequency....";
	const uint32_t iter = 4;
	uint32_t overall, i;

	i = iter;
	overall = OTTO_CPU_CYCLE();
	while (i--) {
		pblr_puts(qstr);
	}
	overall = OTTO_CPU_CYCLE() - overall;
	pblr_puts("\r");
	/* E.g., baud-rate 115200 means 11520 bytes per seconds. */
	ocp_freq = overall*(BAUDRATE/10/sizeof(qstr)/iter)/1000/1000;
	return;
}

uint32_t pll_query_freq(uint32_t dev) {
	if (dev == 0) {
		return ocp_freq;
	} else {
		return lx_freq;
	}
}

uint32_t baudrate_divisor(void) {
	uint32_t bdiv = probe_lx_freq();

	probe_ocp_freq();

	return bdiv;
}
