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
        // Filter for periods on active products.
        $table = _field_sql_storage_tablename($field_info);
        $column = _field_sql_storage_columnname(
            'baumspenden_plant_period',
            'value'
        );
        $query = db_select(
            $table,
            'field'
        );
        $query->leftJoin(
            'commerce_product',
            'product',
            'product.product_id = field.entity_id'
        );
        $query
            ->fields('field', [$column])
            ->condition('field.entity_type', 'commerce_product')
            ->condition('field.bundle', 'baumspende')
            ->condition('field.deleted', 0)
            ->condition('product.status', 1)
            ->groupBy($column);
        $used_values = array_fill_keys($query->execute()->fetchCol(), true);

        return array_intersect_key(
            $field_info['settings']['allowed_values'],
            $used_values
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
        $field_info = field_info_field('baumspenden_plant_region');
        // TODO: Filter for regions on active products with given period.
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
        $field_info = field_info_field('baumspenden_plant_tree');
        // TODO: Filter for trees on active products with given period and region.
        return $field_info['settings']['allowed_values'];
    }
}
