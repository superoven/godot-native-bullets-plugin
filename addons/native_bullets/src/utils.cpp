#include <cmath>
#include "utils.h"

float exp_decay(float a, float b, float decay, float delta) {
    return b + (a - b) * exp(-decay * delta);
}
