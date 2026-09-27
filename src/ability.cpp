#include "ability.h"

#include "pokemon.h"

using namespace std;

bool ability::Damage(pokemon& target) {
    int final_damage = this->base_damage;
    target.hp -= final_damage;
    if (target.hp < 0) target.hp = 0;
    return target.is_fail();
}
