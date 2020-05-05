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
 * Class DrupalBaumspendenBaumspende
 */
class DrupalBaumspendenDonation
{
    /**
     * @var int
     */
    protected $amount;

    /**
     * @var array
     */
    protected $plant_info;

    /**
     * @var string
     */
    protected $certificate_name;

    /**
     * DrupalBaumspendenBaumspende constructor.
     *
     * @param int $amount
     * @param array $plant_info
     * @param string $certificate_name
     */
    public function __construct(
        $amount,
        $plant_info,
        $certificate_name
    ) {
        $this->amount = $amount;
        $this->plant_info = $plant_info;
        $this->certificate_name = $certificate_name;
    }

    /**
     * @return int
     */
    public function getAmount(): int
    {
        return $this->amount;
    }

    /**
     * @return array
     */
    public function getPlantInfo(): array
    {
        return $this->plant_info;
    }

    /**
     * @return string
     */
    public function getCertificateName(): string
    {
        return $this->certificate_name;
    }

    /**
     * Retrieves the Drupal Commerce Product for the given donation criteria.
     */
    public static function getCommerceProduct($plant_info)
    {
        $query = new EntityFieldQuery();
        $query
            ->entityCondition('entity_type', 'commerce_product')
            ->entityCondition('bundle', 'baumspende')
            ->propertyCondition('status', 1)
            ->fieldCondition(
                'baumspenden_plant_period',
                'value',
                $plant_info['plant_period'],
                '='
            )
            ->fieldCondition(
                'baumspenden_plant_region',
                'value',
                $plant_info['plant_region'],
                '='
            )
            ->fieldCondition(
                'baumspenden_plant_tree',
                'value',
                $plant_info['plant_tree'],
                '='
            );
        $result = $query->execute();
        if (
            !isset($result['commerce_product'])
            || count($result['commerce_product']) != 1
        ) {
            throw new Exception(
                t(
                    'Found none or more than one baumspende products with given properties: !properties',
                    [
                        '!properties' => print_r($plant_info, true),
                    ]
                )
            );
        } else {
            $product = commerce_product_load(
                key($result['commerce_product'])
            );
        }

        return $product;
    }
}
