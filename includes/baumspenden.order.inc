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
 * Class DrupalBaumspendenOrder
 */
class DrupalBaumspendenOrder
{
    /**
     * @var $order
     *   The Drupal Commerce order object.
     */
    protected $order;

    /**
     * @var $order_wrapper
     *   The Drupal EntityMetadataWrapper object.
     */
    protected $order_wrapper;

    /**
     * DrupalBaumspendenOrder constructor.
     *
     * @param \DrupalBaumspendenDonation[] $donations
     *
     * @param array $customer_data
     */
    public function __construct($donations = [], $customer_data = [])
    {
        $this->order = commerce_order_new(0, 'checkout_checkout');
        $this->order_wrapper = entity_metadata_wrapper(
            'commerce_order',
            $order
        );

        $this->addDonations($donations);

        $this->addCustomerProfile();

        commerce_order_save($this->order);
    }

    /**
     * Adds donation product line items to the Drupal Commerce order.
     *
     * @param \DrupalBaumspendenDonation[] $donations
     */
    public function addDonations($donations)
    {
        foreach ($donations as $donation) {
            $product = self::getCommerceProduct($this->getPlantInfo());
            $line_item = commerce_product_line_item_new(
                $product,
                $donation->getAmount(),
                $this->order->order_id
            );
            commerce_line_item_save($line_item);

            $this->order_wrapper->commerce_line_items[] = $line_item;
        }

        commerce_order_save($this->order);
    }

    /**
     * Adds a Drupal Commerce customer profile to the Drupal Commerce order.
     *
     * @param $customer_data
     */
    public function addCustomerProfile($customer_data)
    {
        $customer_profile = commerce_customer_profile_new(
            'billing',
            $order->uid
        );
        $customer_profile_wrapper = entity_metadata_wrapper(
            'commerce_customer_profile',
            $customer_profile
        );
        // TODO: Set customer profile properties.
        $customer_profile_wrapper->commerce_customer_address->country = $submitted_values['billing_country'];

        commerce_customer_profile_save($customer_profile);
        $order_wrapper->commerce_customer_billing = $customer_profile->profile_id;

        commerce_order_save($this->order);
    }

    /**
     * Checkout the Drupal Commerce order.
     */
    public function checkout()
    {
        drupal_goto('checkout/' . $this->order->order_id);
    }
}
