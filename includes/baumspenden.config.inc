<?php

/**
 * Class DrupalBaumspendenConfig
 */
class DrupalBaumspendenConfig
{
    /**
     * Retrieves all available plant regions from the configuration.
     *
     * @return array
     */
    public static function getPlantPeriods()
    {
        return array(
            'fruehling' => 'Frühling',
            'sommer' => 'Sommer',
            'herbst' => 'Herbst',
            'winter' => 'Winter',
        );
    }

    /**
     * Retrieves all available plant regions for a given plant period from the
     * configuration.
     *
     * @param $period
     *
     * @return array
     */
    public static function getPlantRegions($period)
    {
        return array(
            'Nationalpark Sächsische Schweiz',
            'Naturpark Pfälzerwald',
        );
    }

    /**
     * Retrieves all available tree species for a given plant region from the
     * configuration.
     *
     * @param $region
     *
     * @return array
     */
    public static function getTreeSpecies($region) {
        return array(
            'Schwarz-Pappel',
            'Erle',
            'Bergulme',
            'Feldahorn',
            'Sal-Weide',
        );
    }
}
