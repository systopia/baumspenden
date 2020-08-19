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
     * Machine-readable name of the customer profile type to use for
     * Baumspenden orders.
     * This must not exceed 14 characters, since commerce_customer adds its
     * module name as a prefix and a field name may only be 32 characters in
     * length.
     */
    public const CUSTOMER_PROFILE_TYPE_DONOR = 'baumsp_donor';

    /**
     * Machine-readable name of the customer profile type to use for Baumspenden
     * orders.
     * This must not exceed 14 characters, since commerce_customer adds its
     * module name as a prefix and a field name may only be 32 characters in
     * length.
     */
    public const CUSTOMER_PROFILE_TYPE_PRESENTEE = 'baumsp_prsntee';

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
        $shipping_mode,
        $customer_data = [],
        $presentee_data = []
    ) {
        // Create a Drupal Commerce order.
        global $user;
        $commerce_order = commerce_order_new(
            $user->uid,
            'checkout_checkout',
            self::ORDER_TYPE
        );
        commerce_order_save($commerce_order);

        // Add properties.
        $order = new self($commerce_order->order_id);
        $order->addDonations($donations);
        $order->order_wrapper
            ->{DrupalBaumspendenConfig::FIELD_NAME_SHIPPING_MODE} = $shipping_mode;
        $order->addCustomerProfile(
            $customer_data,
            self::CUSTOMER_PROFILE_TYPE_DONOR
        );
        // Add default commerce_customer_billing profile for painless payment
        // processing, cince payment processors sometimes require it.
        $order->addCustomerProfile(
            $customer_data,
            'billing'
        );
        if (!empty($presentee_data)) {
            $order->addCustomerProfile(
                $presentee_data,
                self::CUSTOMER_PROFILE_TYPE_PRESENTEE
            );
        }

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
    public function addCustomerProfile(
        $customer_data,
        $profile_type = self::CUSTOMER_PROFILE_TYPE_DONOR
    ) {
        $customer_profile = commerce_customer_profile_new(
            $profile_type,
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
                'supplemental_address_1' => 'premise',
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

        // Set e-mail address field(s) (created in .install).
        if (
            !empty($customer_data['email'])
            && isset($customer_profile_wrapper->{DrupalBaumspendenConfig::FIELD_NAME_EMAIL})
        ) {
            $customer_profile_wrapper->{DrupalBaumspendenConfig::FIELD_NAME_EMAIL} = $customer_data['email'];
        }

        commerce_customer_profile_save($customer_profile);
        $this->order_wrapper->{'commerce_customer_' . $profile_type} = $customer_profile->profile_id;

        commerce_order_save($this->order);
    }

    /**
     * Checkout the Drupal Commerce order.
     */
    public function checkout()
    {
        drupal_set_message(
            'Since products and tree donations cannot be ordered simultaneously, a new order has been created for your tree donation(s). Any products previously added to your cart remain in a separate order and will be available again after checking out this tree donation order.'
        );
        drupal_goto('checkout/' . $this->order->order_id);
    }

    /**
     * Perform steps when the Drupal Commerce order has completed checkout.
     */
    public function complete()
    {
        // Retrieve latest payment transaction for this order. Since the order
        // should only have one, and exactly one, when completing checkout, this
        // will always retrieve the correct transaction.
        $query = new EntityFieldQuery;
        $result = $query
            ->entityCondition('entity_type', 'commerce_payment_transaction')
            ->propertyCondition('order_id', $this->order->order_id)
            ->propertyOrderBy('transaction_id', 'DESC')
            ->range(0, 1)
            ->execute();
        if (isset($result['commerce_payment_transaction'])) {
            $transaction = commerce_payment_transaction_load(
                key($result['commerce_payment_transaction'])
            );
            $transaction_wrapper = entity_metadata_wrapper(
                'commerce_payment_transaction',
                $transaction
            );
        }

        foreach ($this->order_wrapper->commerce_line_items as $donation) {
            // Add basic data.
            $params = [
                'first_name' => $this->order_wrapper
                    ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_DONOR}
                    ->commerce_customer_address
                    ->first_name->value(),
                'last_name' => $this->order_wrapper
                    ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_DONOR}
                    ->commerce_customer_address
                    ->last_name->value(),
                'street_address' => $this->order_wrapper
                    ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_DONOR}
                    ->commerce_customer_address
                    ->thoroughfare->value(),
                'supplemental_address_1' => $this->order_wrapper
                    ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_DONOR}
                    ->commerce_customer_address
                    ->premise->value(),
                'postal_code' => $this->order_wrapper
                    ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_DONOR}
                    ->commerce_customer_address
                    ->postal_code->value(),
                'city' => $this->order_wrapper
                    ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_DONOR}
                    ->commerce_customer_address
                    ->locality->value(),
                'email' => $this->order_wrapper
                    ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_DONOR}
                    ->{DrupalBaumspendenConfig::FIELD_NAME_EMAIL}->value(),
                'source' => '', // TODO: Add source?
                'payment_method' => $transaction_wrapper->payment_method->value(),
                'unit_price' => $donation->commerce_unit_price->amount->value() / 100,
                'amount' => (int) $donation->quantity->value(),
                'plant_period' => $donation
                    ->commerce_product
                    ->baumspenden_plant_period->value(),
                'plant_region' => $donation
                    ->commerce_product
                    ->baumspenden_plant_region->value(),
                'plant_tree' => $donation
                    ->commerce_product
                    ->baumspenden_plant_tree->value(),
                'plant_period_label' => $donation
                    ->commerce_product
                    ->baumspenden_plant_period->label(),
                'plant_region_label' => $donation
                    ->commerce_product
                    ->baumspenden_plant_region->label(),
                'plant_tree_label' => $donation
                    ->commerce_product
                    ->baumspenden_plant_tree->label(),
                'shipping_mode' => $this->order_wrapper
                    ->{DrupalBaumspendenConfig::FIELD_NAME_SHIPPING_MODE}->value(),
                'as_present' => (int) isset(
                    $this->order_wrapper
                        ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                        ->commerce_customer_address
                ),
            ];
            // Add payment transaction data.
            switch ($transaction->payment_method) {
                case 'sepa_direct_debit':
                    // iban_field does not yet support access via
                    // EntityMetadataWrapper in the latest stable version,
                    // see https://www.drupal.org/node/2589851
                    $iban_field_items = field_get_items(
                        'commerce_payment_transaction',
                        $transaction,
                        DrupalBaumspendenConfig::FIELD_NAME_IBAN
                    );
                    $params['iban'] = $iban_field_items[0]['iban_field'];
                    $params['bic'] = $transaction_wrapper
                        ->{DrupalBaumspendenConfig::FIELD_NAME_BIC}->value();
                    break;
                // TODO: Anything to add for other payment methods?
            }
            // Add presentee data.
            if (isset(
                $this->order_wrapper
                    ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                    ->commerce_customer_address
            )) {
                $params += [
                    'presentee_first_name' => $this->order_wrapper
                        ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                        ->commerce_customer_address
                        ->first_name->value(),
                    'presentee_last_name' => $this->order_wrapper
                        ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                        ->commerce_customer_address
                        ->last_name->value(),
                    'presentee_street_address' => $this->order_wrapper
                        ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                        ->commerce_customer_address
                        ->thoroughfare->value(),
                    'presentee_supplemental_address_1' => $this->order_wrapper
                        ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                        ->commerce_customer_address
                        ->premise->value(),
                    'presentee_postal_code' => $this->order_wrapper
                        ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                        ->commerce_customer_address
                        ->postal_code->value(),
                    'presentee_city' => $this->order_wrapper
                        ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                        ->commerce_customer_address
                        ->locality->value(),
                ];
            }
            if (isset(
                $this->order_wrapper
                    ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                    ->{DrupalBaumspendenConfig::FIELD_NAME_EMAIL}
            )) {
                $params += [
                    'presentee_email' => $this->order_wrapper
                        ->{'commerce_customer_' . self::CUSTOMER_PROFILE_TYPE_PRESENTEE}
                        ->{DrupalBaumspendenConfig::FIELD_NAME_EMAIL}->value(),
                ];
            }
            // Send donation order to CiviCRM.
            $cmrf_result = DrupalBaumspendenCiviMRF::sendCall(
                'BWPBaumspende',
                'submit',
                $params
            );
        }
    }
}
