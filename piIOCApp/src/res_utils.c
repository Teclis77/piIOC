#include <stdio.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "res_utils.h"

float mech_to_real(float mres, float mech, int dir)
{
	if (mres == 0.0f) { printf("ERROR mech_to_real: mres=0, divisione impossibile\n");
	return 0.0f;
	}
	return (float)dir * mech / mres;
}

float real_to_mech(float mres, float real, int dir)
{
	return (float)dir * real * mres;
}
