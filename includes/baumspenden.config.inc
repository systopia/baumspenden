<?php
/*-------------------------------------------------------+
| Bergwaldprojekt Baumspenden                            |
| Copyright (C) 2010 SYSTOPIA                            |
+--------------------------------------------------------+
| This program is released as free software under the    |
| Affero GPL license. You can redistribute it and/or     |
| modify it under the terms of this license which you    |
| can read by viewing the included agpl.txt or online    |
| at www.gnu.org/licenses/agpl.html. Removal of this     |
| copyright header is strictly prohibited without        |
| written permission from the original author(s).        |
+--------------------------------------------------------*/

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
        return [
            'fruehling' => 'Frühling',
            'sommer' => 'Sommer',
            'herbst' => 'Herbst',
            'winter' => 'Winter',
        ];
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
        $regions = [];

        if ($period == 'fruehling') {
            $regions['saggsn'] = 'Nationalpark Sächsische Schweiz';
        }
        elseif ($period == 'sommer') {
            $regions['palz'] = 'Naturpark Pfälzerwald';
        }

        return $regions;
    }

    /**
     * Retrieves all available tree species for a given plant region from the
     * configuration.
     *
     * @param $region
     *
     * @return array
     */
    public static function getTreeSpecies($region)
    {
        return [
            'schwarz-pappel' => 'Schwarz-Pappel',
            'erle' => 'Erle',
            'bergulme' => 'Bergulme',
            'feldahorn' => 'Feldahorn',
            'sal-weide' => 'Sal-Weide',
        ];
    }
}
