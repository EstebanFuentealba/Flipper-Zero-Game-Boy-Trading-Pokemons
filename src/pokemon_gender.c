#include <gui/modules/submenu.h>

#include <src/include/pokemon_app.h>
#include <src/include/pokemon_data.h>
#include <src/include/pokemon_gender.h>

#include <src/scenes/include/pokemon_scene.h>

static const char* gender_str[] = {
    "Unknown",
    "Female",
    "Male",
};

/* This returns a string pointer if the gender is static, NULL if it is not and
 * the gender needs to be calculated.
 */
const char* pokemon_gender_is_static(PokemonData* pdata, uint8_t ratio) {
    switch(ratio) {
    case 0xFF:
        return gender_str[0];
    case 0xFE:
        return gender_str[1];
    case 0x00:
        if(pokemon_stat_get(pdata, STAT_NUM, NONE) != 0xEB) { // Tyrogue can be either gender
            return gender_str[2];
        }
        break;
    default:
        break;
    }

    return NULL;
}

const char* pokemon_gender_get(PokemonData* pdata) {
    uint8_t ratio = table_stat_base_get(
        pdata->pokemon_table,
        pokemon_stat_get(pdata, STAT_NUM, NONE),
        STAT_BASE_GENDER_RATIO,
        NONE);
    uint8_t atk_iv;
    const char* rc;

    rc = pokemon_gender_is_static(pdata, ratio);
    if(rc) return rc;

    /* Falling through here means now we need to calculate the gender from
     * its ratio and ATK_IV.
     */
    atk_iv = pokemon_stat_get(pdata, STAT_ATK_IV, NONE);
    if(atk_iv * 17 <= ratio)
        return gender_str[1];
    else
        return gender_str[2];
}

void pokemon_gender_set(PokemonData* pdata, Gender gender) {

    uint8_t ratio = table_stat_base_get(
        pdata->pokemon_table,
        pokemon_stat_get(pdata, STAT_NUM, NONE),
        STAT_BASE_GENDER_RATIO,
        NONE);
    uint8_t atk_iv = pokemon_stat_get(pdata, STAT_ATK_IV, NONE);

    /* If we need to make the pokemon a male, increase atk IV until it exceeds
     * the gender ratio.
     *
     * Note that, there is no checking here for impossible situations as the
     * scene enter function will immediately quit if its not possible to change
     * the gender (the extremes of gender_ratio value).
     *
     * The check for gender is a percentage, if ATK_IV*(255/15) <= the ratio,
     * then the pokemon is a female. The gender ratio values end up being:
     * DEF GENDER_F0      EQU   0 percent
     * DEF GENDER_F12_5   EQU  12 percent + 1
     * DEF GENDER_F25     EQU  25 percent
     * DEF GENDER_F50     EQU  50 percent
     * DEF GENDER_F75     EQU  75 percent
     * DEF GENDER_F100    EQU 100 percent - 1
     * Where percent is (255/100)
     */
    if(gender == GENDER_MALE) {
        while((atk_iv * 17) <= ratio) atk_iv++;
    } else {
        while((atk_iv * 17) > ratio) atk_iv--;
    }

    pokemon_stat_set(pdata, STAT_ATK_IV, NONE, atk_iv);
}
