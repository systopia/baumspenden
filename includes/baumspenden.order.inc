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
     * Machine-readable name of the order type to use for Baumspenden orders.
     */
    public const ORDER_TYPE = 'baumspende';

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
     * @param $order_id
     *   The Drupal commerce order ID to wrap this object around.
     */
    public function __construct($order_id)
    {
        $this->order = commerce_order_load($order_id);
        $this->order_wrapper = entity_metadata_wrapper(
            'commerce_order',
            $this->order
        );
    }

    /**
     * @param \DrupalBaumspendenDonation[] $donations
     *
     * @param array $customer_data
     *
     * @param array $presentee_data
     *
     * @return \DrupalBaumspendenOrder
     *   The order object.
     *
     * @throws \Exception
     *   When the order could not be created.
     */
    public static function create(
        $donations = [],
        $customer_data = [],
        $presentee_data = []
    ) {
        global $user;
        $commerce_order = commerce_order_new(
            $user->uid,
            'checkout_checkout',
            self::ORDER_TYPE
        );

        $order = new self($commerce_order->order_id);
        $order->addDonations($donations);
        $order->addCustomerProfile($customer_data);
        $order->addPresenteeAddress($presentee_data);

        return $order;
    }

    /**
     * Adds donation product line items to the Drupal Commerce order.
     *
     * @param \DrupalBaumspendenDonation[] $donations
     *
     * @throws \Exception
     *   When the donations could not be added to the order.
     */
    public function addDonations($donations)
    {
        foreach ($donations as $donation) {
            $product = DrupalBaumspendenDonation::getCommerceProduct(
                $donation->getPlantInfo()
            );
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
            $this->order->uid
        );
        $customer_profile_wrapper = entity_metadata_wrapper(
            'commerce_customer_profile',
            $customer_profile
        );

        // Set customer profile properties.
        foreach (
            [
                'first_name' => 'first_name',
                'last_name' => 'last_name',
                'street_address' => 'thoroughfare',
                'postal_code' => 'postal_code',
                'city' => 'locality',
            ] as $key => $property
        ) {
            if (!empty($customer_data[$key])) {
                $customer_profile_wrapper->commerce_customer_address->$property = $customer_data[$key];
            }
        }
        // Addressfield tampers with first_name and last_name if name_line is
        // set (it is set to the empty string). Setting it therefore to NULL.
        // @see addressfield_field_presave().
        $customer_profile_wrapper->commerce_customer_address->name_line = null;

        commerce_customer_profile_save($customer_profile);
        $this->order_wrapper->commerce_customer_billing = $customer_profile->profile_id;

        commerce_order_save($this->order);
    }

    /**
     * Fills the presentee field on the Drupal Commerce order.
     *
     * @param array $presentee_data
     */
    public function addPresenteeAddress($presentee_data)
    {
        foreach (
            [
                'first_name' => 'first_name',
                'last_name' => 'last_name',
                'street_address' => 'thoroughfare',
                'postal_code' => 'postal_code',
                'city' => 'locality',
            ] as $key => $property
        ) {
            if (!empty($presentee_data[$key])) {
                $this->order_wrapper->{DrupalBaumspendenConfig::FIELD_NAME_PRESENTEE}->$property = $presentee_data[$key];
            }
        }
        // Addressfield tampers with first_name and last_name if name_line is
        // set (it is set to the empty string). Setting it therefore to NULL.
        // @see addressfield_field_presave().
        $this->order_wrapper->{DrupalBaumspendenConfig::FIELD_NAME_PRESENTEE}->name_line = null;

        commerce_order_save($this->order);
    }

    /**
     * Checkout the Drupal Commerce order.
     */
    public function checkout()
    {
        drupal_set_message('Since products and tree donations cannot be ordered simultaneously, a new order has been created for your tree donation(s). Any products previously added to your cart remain in a separate order and will be available again after checking out this tree donation order.');
        drupal_goto('checkout/' . $this->order->order_id);
    }
}
