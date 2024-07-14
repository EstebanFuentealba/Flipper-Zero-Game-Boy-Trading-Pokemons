#include <src/include/pokemon_data.h>

/* This just assumes gen ii for now */
/* For a Gen II pokemon to be shiny, the following must be met:
 * Spd, Def, and Spc must all be 10
 * Atk must be 2, 3, 6, 7, 10, 11, 14, or 15
 */
bool pokemon_is_shiny(PokemonData* pdata) {
    uint8_t atk_iv = pokemon_stat_get(pdata, STAT_ATK_IV, NONE);
    uint8_t def_iv = pokemon_stat_get(pdata, STAT_DEF_IV, NONE);
    uint8_t spd_iv = pokemon_stat_get(pdata, STAT_SPD_IV, NONE);
    uint8_t spc_iv = pokemon_stat_get(pdata, STAT_SPC_IV, NONE);
    bool rc = 1;

    if(spd_iv != 10) rc = 0;
    if(def_iv != 10) rc = 0;
    if(spc_iv != 10) rc = 0;
    switch(atk_iv) {
    case 0:
    case 1:
    case 4:
    case 5:
    case 8:
    case 9:
    case 12:
    case 13:
        rc = 0;
        break;
    default:
        break;
    }

    return rc;
}

void pokemon_set_shiny(PokemonData* pdata, bool shiny) {

    if(!shiny) {
        do {
            /* First, reset the IV to the selected stat */
            pokemon_stat_set(pdata, STAT_SEL, NONE, pokemon_stat_get(pdata, STAT_SEL, NONE));

	    /* XXX: This may not be right? */
            /* Next, ensure the current IVs wouldn't make the pokemon shiny */
        } while(pokemon_is_shiny(pdata));
    } else {
        /* Set Def, Spd, Spc to 10 */
        pokemon_stat_set(pdata, STAT_DEF_IV, NONE, 10);
        pokemon_stat_set(pdata, STAT_SPD_IV, NONE, 10);
        pokemon_stat_set(pdata, STAT_SPC_IV, NONE, 10);

        /* Increase ATK IV until we hit a shiny number. Note that, this only
         * affects IVs that are randomly generated, max IV will already be set
         * at 15 which will make it shiny.
         */
        while(!pokemon_is_shiny(pdata)) {
            pokemon_stat_set(
                pdata, STAT_ATK_IV, NONE, pokemon_stat_get(pdata, STAT_ATK_IV, NONE) + 1);
        }
    }
}
