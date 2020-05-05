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
        $field_info = field_info_field('baumspenden_plant_period');
        return $field_info['settings']['allowed_values'];
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
        // TODO: Retrieve depending on given region.
        $field_info = field_info_field('baumspenden_plant_region');
        return $field_info['settings']['allowed_values'];
    }

    /**
     * Retrieves all available tree species for a given plant region from the
     * configuration.
     *
     * @param $region
     *
     * @return array
     */
    public static function getTreeSpecies($period, $region)
    {
        // TODO: Retrieve depending on given region.
        $field_info = field_info_field('baumspenden_plant_tree');
        return $field_info['settings']['allowed_values'];
    }
}
