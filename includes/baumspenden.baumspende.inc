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
class DrupalBaumspendenBaumspende
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
     * TODO: Create a Drupal Commerce Order for this donation.
     */
    public function order() {

    }

    /**
     * TODO: Retrieves the Drupal Commerce Product for the given donation criteria.
     */
    public static function getCommerceProduct() {
        // TODO: Use EntityMetadataWrapper for retrieving a product by its properties.
    }
}
