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
    public const FIELD_NAME_PRESENTEE = 'baumspenden_presentee';

    public const FIELD_NAME_EMAIL = 'baumspenden_email';

    public const FIELD_NAME_IBAN = 'commerce_cop_sepa_iban';

    public const FIELD_NAME_BIC = 'commerce_cop_sepa_bic';

    public const IMAGE_BASE_URL_CERTIFICATE_BACKGROUND = '/sites/default/files/civicrm/baumspenden/';

    /**
     * Retrieves all available plant regions from the configuration.
     *
     * @return array
     */
    public static function getPlantPeriods()
    {
        $field_info = field_info_field('baumspenden_plant_period');
        return array_intersect_key(
            $field_info['settings']['allowed_values'],
            self::getUsedPlantInfoFieldValues('baumspenden_plant_period')
        );
    }

    /**
     * Retrieves all available plant regions for a given plant period from the
     * configuration.
     *
     * @param $period_value
     *
     * @return array
     */
    public static function getPlantRegions($period_value)
    {
        $field_info = field_info_field('baumspenden_plant_region');
        return array_intersect_key(
            $field_info['settings']['allowed_values'],
            self::getUsedPlantInfoFieldValues(
                'baumspenden_plant_region',
                [
                    'baumspenden_plant_period' => $period_value,
                ]
            )
        );
    }

    /**
     * Retrieves all available tree species for a given combination of plant
     * period and plant region from the configuration.
     *
     * @param string $period_value
     *   The plant_period to retrieve tree species for.
     *
     * @param string $region_value
     *   The plant_region to retieve tree species for.
     *
     * @return array
     *   The tree species values available for the given criteria.
     */
    public static function getTreeSpecies($period_value, $region_value)
    {
        $field_info = field_info_field('baumspenden_plant_tree');
        return array_intersect_key(
            $field_info['settings']['allowed_values'],
            self::getUsedPlantInfoFieldValues(
                'baumspenden_plant_tree',
                [
                    'baumspenden_plant_period' => $period_value,
                    'baumspenden_plant_region' => $region_value,
                ]
            )
        );
    }

    /**
     * Retrieves field values being used by Baumspende products, optionally
     * filtered by other field values being used on the same product.
     *
     * Example:
     * For the field "baumspenden_plant_tree", this returns all values already
     * used in this field on Commerce Product entities. When also given the
     * following as dependency fields:
     * [
     *   'baumspenden_plant_period' => 'spring',
     *   'baumspenden_plant_region' => 'middle_east',
     * ]
     * this returns only those used field values for "baumspenden_plant_tree"
     * that are being used with the given field values on the same product, i.e.
     * when a combination of those two dependency fields exists on at least one
     * product.
     *
     * @param $field_name
     *   The Drupal field name to retrieve used values for.
     * @param array $dependency_fields
     *   An array of other field values being present on the same product, keyed
     *   by the Drupal field name.
     *
     * @return array
     *   The available field values for the given critera as keys.
     */
    protected static function getUsedPlantInfoFieldValues(
        $field_name,
        $dependency_fields = []
    ) {
        $field_info = field_info_field($field_name);
        // Filter for periods on active products.
        $table = _field_sql_storage_tablename($field_info);
        $column = _field_sql_storage_columnname(
            $field_name,
            'value'
        );
        $query = db_select($table, 'field');
        $query->leftJoin(
            'commerce_product',
            'product',
            'product.product_id = field.entity_id'
        );
        foreach ($dependency_fields as $dependency_field => $dependency_value) {
            $dependency_field_info = field_info_field($dependency_field);
            $query->leftJoin(
                _field_sql_storage_tablename($dependency_field_info),
                $dependency_field,
                $dependency_field . '.entity_id = product.product_id'
            );
            $query->condition(
                $dependency_field . '.' . _field_sql_storage_columnname(
                    $dependency_field,
                    'value'
                ),
                $dependency_value
            );
        }
        $query
            ->fields('field', [$column])
            ->condition('field.entity_type', 'commerce_product')
            ->condition('field.bundle', DrupalBaumspendenDonation::PRODUCT_TYPE)
            ->condition('field.deleted', 0)
            ->condition('product.status', 1)
            ->groupBy($column);

        return array_fill_keys($query->execute()->fetchCol(), true);
    }
}
