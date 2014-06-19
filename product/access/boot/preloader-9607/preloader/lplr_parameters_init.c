#include <preloader.h>

void arch_parameters(void);

void parameters_init(void) {
	memcpy((void *)(&parameters.soc),
	       (void *)&((soc_configuration_t *)(SRAM_BASE + SOC_CONF_OFFSET))->soc,
	       sizeof(soc_t));
	//parameters._memcpy = &memcpy;

	arch_parameters();

	return;
}
