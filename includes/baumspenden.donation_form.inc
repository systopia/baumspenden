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
 * Class DrupalBaumspendenDonationForm
 */
class DrupalBaumspendenDonationForm
{
    /**
     * Builds the donation form.
     *
     * @param $form
     * @param $form_state
     *
     * @return array
     */
    public static function build($form, &$form_state)
    {
        $form['#after_build'] = [
            ['DrupalBaumspendenDonationForm', 'addRequiredStatesValidation'],
        ];
        $form['#validate'] = [
            ['DrupalBaumspendenDonationForm', 'validate'],
        ];
        $form['#submit'] = [
            ['DrupalBaumspendenDonationForm', 'submit'],
        ];
        $form['#attached']['css'][] = drupal_get_path(
                'module',
                'baumspenden'
            ) . '/css/baumspenden.css';

        if (empty($form_state['step'])) {
            $form_state['step'] = 'form';
        }
        switch ($form_state['step']) {
            case 'form':
                $form = self::buildForm($form, $form_state);
                break;
            case 'preview':
                $form = self::buildPreview($form, $form_state);
                break;
        }
        return $form;
    }

    /**
     * Builds the "form" step of the donation form.
     *
     * @param $form
     * @param $form_state
     *
     * @return array
     *   The $form array.
     */
    public static function buildForm($form, &$form_state)
    {
        $form['donations'] = [
            '#type' => 'fieldset',
            '#title' => 'Baumspenden',
            '#tree' => true,
            '#prefix' => '<div id="baumspenden-donations">',
            '#suffix' => '</div>',
        ];
        if (empty($form_state['storage']['donations'])) {
            $form_state['storage']['donations'][] = [];
        }
        foreach ($form_state['storage']['donations'] as $key => $donation) {
            $form['donations'][$key] = [
                '#type' => 'fieldset',
            ];
            $form['donations'][$key]['amount'] = [
                '#type' => 'textfield',
                '#title' => 'Ich spende',
                '#description' => 'Für mehr als 100 Bäume und Unternehmensspenden kontaktieren Sie uns bitte unter <a href="mailto:baumspende@bergwaldprojekt.de">baumspende@bergwaldprojekt.de</a>',
                '#field_suffix' => 'Baum/Bäume',
                '#required' => true,
                '#element_validate' => [
                    'element_validate_integer_positive',
                ],
                '#default_value' => isset($donation['amount']) ? $donation['amount'] : null,
            ];
            $form['donations'][$key]['plant_info'] = [
                '#prefix' => '<div id="baumspenden-plant-info--' . $key . '">',
                '#suffix' => '</div>',
            ];
            $form['donations'][$key]['plant_info']['plant_period'] = [
                '#type' => 'select',
                '#title' => 'Pflanzperiode',
                '#options' => DrupalBaumspendenConfig::getPlantPeriods(),
                '#empty_option' => '- Auswählen -',
                '#required' => true,
                '#ajax' => [
                    'wrapper' => 'baumspenden-plant-info--' . $key,
                    'callback' => [
                        'DrupalBaumspendenDonationForm',
                        'replacePlantInfo',
                    ],
                ],
                '#default_value' => isset($donation['plant_info']['plant_period']) ? $donation['plant_info']['plant_period'] : null,
            ];

            if (!empty($form_state['storage']['donations'][$key]['plant_info']['plant_period'])) {
                $form['donations'][$key]['plant_info']['plant_region'] = [
                    '#type' => 'select',
                    '#title' => 'Region',
                    '#options' => DrupalBaumspendenConfig::getPlantRegions(
                        $form_state['storage']['donations'][$key]['plant_info']['plant_period']
                    ),
                    '#empty_option' => '- Auswählen -',
                    '#required' => true,
                    '#ajax' => [
                        'wrapper' => 'baumspenden-plant-info--' . $key,
                        'callback' => [
                            'DrupalBaumspendenDonationForm',
                            'replacePlantInfo',
                        ],
                    ],
                    '#default_value' => isset($donation['plant_info']['plant_region']) ? $donation['plant_info']['plant_region'] : null,
                ];
            }

            if (!empty($form_state['storage']['donations'][$key]['plant_info']['plant_region'])) {
                $form['donations'][$key]['plant_info']['plant_tree'] = [
                    '#type' => 'select',
                    '#title' => 'Baumart',
                    '#options' => DrupalBaumspendenConfig::getTreeSpecies(
                        $form_state['storage']['donations'][$key]['plant_info']['plant_period'],
                        $form_state['storage']['donations'][$key]['plant_info']['plant_region']
                    ),
                    '#empty_option' => '- Auswählen -',
                    '#required' => true,
                    '#default_value' => isset($donation['plant_info']['plant_tree']) ? $donation['plant_info']['plant_tree'] : null,
                ];
            }
            $form['donations'][$key]['plant_info']['update'] = [
                '#type' => 'submit',
                '#baumspenden_op' => 'update_plant_info',
                // Distinctly identify button for this donation.
                '#name' => 'baumspenden_plant_info_' . $key,
                '#value' => t('Update'),
                '#attributes' => [
                    'class' => [
                        'hide-js',
                    ],
                ],
                // Do not validate anything (Ajax-like behavior).
                '#limit_validation_errors' => [],
                // #submit is required when using #limit_validation_errors.
                '#submit' => [
                    ['DrupalBaumspendenDonationForm', 'submit'],
                ],
                '#baumspenden_donation_key' => $key,
            ];

            $form['donations'][$key]['certificate_name'] = [
                '#type' => 'textfield',
                '#title' => 'Name auf Zertifikat',
                '#description' => 'max. 40 Zeichen',
                '#attributes' => [
                    'placeholder' => 'Waldmariechen',
                ],
                '#maxlength' => 40,
                '#default_value' => isset($donation['certificate_name']) ? $donation['certificate_name'] : null,
            ];

            if ($key != 0) {
                $form['donations'][$key]['remove'] = [
                    '#type' => 'submit',
                    '#baumspenden_op' => 'remove_donation',
                    // Distinctly identify button for this donation.
                    '#name' => 'baumspenden_remove_donation_' . $key,
                    '#value' => t('Remove'),
                    // Do not validate anything.
                    '#limit_validation_errors' => [],
                    // #submit is required when using #limit_validation_errors.
                    '#submit' => [
                        ['DrupalBaumspendenDonationForm', 'submit'],
                    ],
                    '#ajax' => [
                        'wrapper' => 'baumspenden-donations',
                        'callback' => [
                            'DrupalBaumspendenDonationForm',
                            'replaceDonations',
                        ],
                    ],
                    '#baumspenden_donation_key' => $key,
                ];
            }
        }
        $form['donations']['add'] = [
            '#type' => 'submit',
            '#baumspenden_op' => 'add_donation',
            '#value' => 'Baumspende hinzufügen',
            // Only validate related fields.
            '#limit_validation_errors' => [],
            // #submit is required when using #limit_validation_errors.
            '#submit' => [
                ['DrupalBaumspendenDonationForm', 'submit'],
            ],
            '#ajax' => [
                'wrapper' => 'baumspenden-donations',
                'callback' => [
                    'DrupalBaumspendenDonationForm',
                    'replaceDonations',
                ],
            ],
        ];

        $form['shipping_mode'] = [
            '#type' => 'select',
            '#title' => 'Versandoption',
            '#options' => [
                'email' => 'als PDF per E-Mail',
                'postal' => 'postalisch',
                'none' => 'kein Zertifikat',
            ],
            '#default_value' => isset($form_state['storage']['shipping_mode']) ? $form_state['storage']['shipping_mode'] : null,
        ];

        $form['contact'] = [
            '#type' => 'fieldset',
            '#title' => 'Meine Kontaktdaten',
            '#tree' => true,
        ];
        $form['contact']['first_name'] = [
            '#type' => 'textfield',
            '#title' => 'Vorname',
            '#required' => true,
            '#default_value' => isset($form_state['storage']['contact']['first_name']) ? $form_state['storage']['contact']['first_name'] : null,
        ];
        $form['contact']['last_name'] = [
            '#type' => 'textfield',
            '#title' => 'Nachname',
            '#required' => true,
            '#default_value' => isset($form_state['storage']['contact']['last_name']) ? $form_state['storage']['contact']['last_name'] : null,
        ];
        $form['contact']['street_address'] = [
            '#type' => 'textfield',
            '#title' => 'Straße, Hausnummer',
            '#states' => [
                'required' => [
                    ':input[name="shipping_mode"]' => ['value' => 'postal'],
                    ':input[name="presentee_shipping"]' => ['checked' => false],
                ],
            ],
            '#default_value' => isset($form_state['storage']['contact']['street_address']) ? $form_state['storage']['contact']['street_address'] : null,
        ];
        $form['contact']['postal_code'] = [
            '#type' => 'textfield',
            '#title' => 'Postleitzahl',
            '#states' => [
                'required' => [
                    ':input[name="shipping_mode"]' => ['value' => 'postal'],
                    ':input[name="presentee_shipping"]' => ['checked' => false],
                ],
            ],
            '#default_value' => isset($form_state['storage']['contact']['postal_code']) ? $form_state['storage']['contact']['postal_code'] : null,
        ];
        $form['contact']['city'] = [
            '#type' => 'textfield',
            '#title' => 'Ort',
            '#states' => [
                'required' => [
                    ':input[name="shipping_mode"]' => ['value' => 'postal'],
                    ':input[name="presentee_shipping"]' => ['checked' => false],
                ],
            ],
            '#default_value' => isset($form_state['storage']['contact']['city']) ? $form_state['storage']['contact']['city'] : null,
        ];
        $form['contact']['email'] = [
            '#type' => 'textfield',
            '#title' => 'E-Mail-Adresse',
            '#required' => true,
            '#default_value' => isset($form_state['storage']['contact']['email']) ? $form_state['storage']['contact']['email'] : null,
        ];

        $form['as_present'] = [
            '#type' => 'checkbox',
            '#title' => 'Als Geschenk',
            '#description' => 'Ich möchte das Zertifikat verschenken.',
            '#default_value' => isset($form_state['storage']['as_present']) ? $form_state['storage']['as_present'] : null,
        ];
        $form['presentee_shipping'] = [
            '#type' => 'checkbox',
            '#title' => 'An Beschenkte/n versenden',
            '#description' => 'Das Zertifikat soll direkt an die/den Beschenkte/n versendet werden.',
            '#states' => [
                'visible' => [
                    ':input[name="as_present"]' => [
                        'checked' => true,
                    ],
                    ':input[name="shipping_mode"]' => [
                        ['value' => 'email'],
                        ['value' => 'postal'],
                    ],
                ],
            ],
            '#default_value' => isset($form_state['storage']['presentee_shipping']) ? $form_state['storage']['presentee_shipping'] : null,
        ];

        $form['presentee'] = [
            '#type' => 'fieldset',
            '#title' => 'Verschenken an',
            '#tree' => true,
            '#states' => [
                'visible' => [
                    ':input[name="as_present"]' => [
                        'checked' => true,
                    ],
                ],
            ],
        ];
        $form['presentee']['first_name'] = [
            '#type' => 'textfield',
            '#title' => 'Vorname',
            '#states' => [
                'required' => [
                    ':input[name="as_present"]' => ['checked' => true],
                ],
            ],
            '#default_value' => isset($form_state['storage']['presentee']['first_name']) ? $form_state['storage']['presentee']['first_name'] : null,
        ];
        $form['presentee']['last_name'] = [
            '#type' => 'textfield',
            '#title' => 'Nachname',
            '#states' => [
                'required' => [
                    ':input[name="as_present"]' => ['checked' => true],
                ],
            ],
            '#default_value' => isset($form_state['storage']['presentee']['first_name']) ? $form_state['storage']['presentee']['first_name'] : null,
        ];
        $form['presentee']['street_address'] = [
            '#type' => 'textfield',
            '#title' => 'Straße, Hausnummer',
            '#states' => [
                'required' => [
                    ':input[name="as_present"]' => ['checked' => true],
                    ':input[name="shipping_mode"]' => ['value' => 'postal'],
                    ':input[name="presentee_shipping"]' => ['checked' => true],
                ],
            ],
            '#default_value' => isset($form_state['storage']['presentee']['street_address']) ? $form_state['storage']['presentee']['street_address'] : null,
        ];
        $form['presentee']['postal_code'] = [
            '#type' => 'textfield',
            '#title' => 'Postleitzahl',
            '#states' => [
                'required' => [
                    ':input[name="as_present"]' => ['checked' => true],
                    ':input[name="shipping_mode"]' => ['value' => 'postal'],
                    ':input[name="presentee_shipping"]' => ['checked' => true],
                ],
            ],
            '#default_value' => isset($form_state['storage']['presentee']['postal_code']) ? $form_state['storage']['presentee']['postal_code'] : null,
        ];
        $form['presentee']['city'] = [
            '#type' => 'textfield',
            '#title' => 'Ort',
            '#states' => [
                'required' => [
                    ':input[name="as_present"]' => ['checked' => true],
                    ':input[name="shipping_mode"]' => ['value' => 'postal'],
                    ':input[name="presentee_shipping"]' => ['checked' => true],
                ],
            ],
            '#default_value' => isset($form_state['storage']['presentee']['city']) ? $form_state['storage']['presentee']['city'] : null,
        ];
        $form['presentee']['email'] = [
            '#type' => 'textfield',
            '#title' => 'E-Mail-Adresse',
            '#states' => [
                'required' => [
                    ':input[name="as_present"]' => ['checked' => true],
                    ':input[name="shipping_mode"]' => ['value' => 'email'],
                    ':input[name="presentee_shipping"]' => ['checked' => true],
                ],
            ],
            '#default_value' => isset($form_state['storage']['presentee']['email']) ? $form_state['storage']['presentee']['email'] : null,
        ];

        $form['newsletter'] = [
            '#type' => 'checkbox',
            '#title' => 'Newsletter abonnieren',
            '#description' => 'Newsletter mit aktuellen Themen rund um das Bergwaldprojekt',
            '#default_value' => isset($form_state['storage']['newsletter']) ? $form_state['storage']['newsletter'] : null,
        ];

        $form['gdpr_consent'] = [
            '#type' => 'checkbox',
            '#title' => 'Datenschutz',
            '#description' => 'Ich habe die <a href="' . variable_get(
                    'baumspenden_gdpr_link',
                    '/datenschutz'
                ) . '" target="_blank">Datenschutzerklärung</a> gelesen und akzeptiere diese.',
            '#required' => true,
            '#default_value' => isset($form_state['storage']['gdpr_consent']) ? $form_state['storage']['gdpr_consent'] : null,
        ];

        $form['actions'] = [
            '#type' => 'actions',
        ];
        $form['actions']['preview'] = [
            '#type' => 'submit',
            '#baumspenden_op' => 'preview',
            '#value' => 'Vorschau',
            '#states' => [
                'visible' => [
                    ':input[name="shipping_mode"]' => [
                        ['value' => 'postal'],
                        ['value' => 'email'],
                    ],
                ],
            ],
        ];
        $form['actions']['submit'] = [
            '#type' => 'submit',
            '#baumspenden_op' => 'submit',
            '#value' => 'Spenden',
            '#states' => [
                'visible' => [
                    ':input[name="shipping_mode"]' => [
                        ['value' => 'none'],
                    ],
                ],
            ],
        ];

        return $form;
    }

    /**
     * Builds the "preview" step of the donation form.
     *
     * @param $form
     * @param $form_state
     *
     * @return array
     *   The $form array.
     */
    public static function buildPreview($form, &$form_state)
    {
        foreach ($form_state['storage']['donations'] as $key => $donation_data) {
            // Build certificate name when empty (initiator/presentee
            // first and last name).
            if (empty($certificate_name = $donation_data['certificate_name'])) {
                $certificate_name =
                    $form_state['storage'][($form_state['storage']['as_present'] ? 'presentee' : 'contact')]['first_name']
                    . ' '
                    . $form_state['storage'][($form_state['storage']['as_present'] ? 'presentee' : 'contact')]['last_name'];
            }
            // Build certificate background image URL (derive from
            // plant_region).
            $certificate_background_url = DrupalBaumspendenConfig::IMAGE_BASE_URL_CERTIFICATE_BACKGROUND . $donation_data['plant_info']['plant_region'] . '.jpg';

            // Build render arrays for plant_info field values.
            $plant_info = [];
            $commerce_product = DrupalBaumspendenDonation::getCommerceProduct(
                $donation_data['plant_info']
            );
            foreach ($donation_data['plant_info'] as $plant_info_name => $plant_info_value) {
                $plant_info[$plant_info_name] = field_view_field(
                    'commerce_product',
                    $commerce_product,
                    'baumspenden_' . $plant_info_name,
                    ['label' => 'hidden']
                );
            }

            // Add a collapsible fieldset for each certificate preview,
            // with only the first being expanded.
            $form[$key] = [
                '#type' => 'fieldset',
                '#title' => 'Zertifikat für ' . $certificate_name,
                '#collapsible' => true,
                '#collapsed' => ($key != 0),
                // The actual preview uses a theme element.
                // @see baumspenden_theme().
                // @see baumspenden-certificate-preview.tpl.php.
                'preview' => [
                    '#theme' => 'baumspenden_certificate_preview',
                    '#certificate_key' => $key,
                    '#plant_info' => $plant_info,
                    '#amount' => $donation_data['amount'],
                    '#certificate_name' => $certificate_name,
                    '#background_image_url' => $certificate_background_url,
                ],
            ];
        }

        $form['actions'] = [
            '#type' => 'actions',
        ];
        $form['actions']['to_form'] = [
            '#type' => 'submit',
            '#baumspenden_op' => 'to_form',
            '#value' => 'Zurück',
            '#limit_validation_errors' => [],
        ];
        $form['actions']['submit'] = [
            '#type' => 'submit',
            '#baumspenden_op' => 'submit',
            '#value' => 'Spenden',
            '#limit_validation_errors' => [],
        ];

        return $form;
    }

    /**
     * Validates submissions of the donation form.
     *
     * @param $form
     * @param $form_state
     */
    public static function validate($form, &$form_state)
    {
        // Require a valid e-mail address.
        if (
            isset($form['contact']['email'])
            && self::isStatesRequired($form['contact']['email'], $form_state)
            && !valid_email_address($form_state['values']['contact']['email'])
        ) {
            form_error(
                $form['contact']['email'],
                'Bitte geben Sie eine gültige E-Mail-Adresse ein.'
            );
        }
        if (
            isset($form['presentee']['email'])
            && self::isStatesRequired($form['presentee']['email'], $form_state)
            && !valid_email_address($form_state['values']['presentee']['email'])
        ) {
            form_error(
                $form['contact']['email'],
                'Bitte geben Sie eine gültige E-Mail-Adresse ein.'
            );
        }

        // Handle hierarchically dependent submissions of plant info fields.
        if (!empty($form['donations'])) {
            foreach (element_children($form['donations']) as $key) {
                if (is_numeric($key)) {
                    $submitted_plant_info = &$form_state['values']['donations'][$key]['plant_info'];

                    // Plant period field changes.
                    if (
                        isset($submitted_plant_info['plant_period'])
                        && isset($form_state['storage']['donations'][$key]['plant_info']['plant_period'])
                        && $submitted_plant_info['plant_period'] != $form_state['storage']['donations'][$key]['plant_info']['plant_period']
                    ) {
                        if (isset($form['donations'][$key]['plant_info']['plant_region'])) {
                            // Reset dependent field for plant region.
                            form_set_value(
                                $form['donations'][$key]['plant_info']['plant_region'],
                                null,
                                $form_state
                            );
                        }
                        if (isset($form['donations'][$key]['plant_info']['plant_tree'])) {
                            // Reset dependent field for tree species.
                            form_set_value(
                                $form['donations'][$key]['plant_info']['plant_tree'],
                                null,
                                $form_state
                            );
                        }
                    }

                    // Plant region field changes.
                    if (
                        isset($submitted_plant_info['plant_region'])
                        && isset($Form_state['storage']['donations'][$key]['plant_info']['plant_region'])
                        && $submitted_plant_info['plant_region'] != $form_state['storage']['donations'][$key]['plant_info']['plant_period']
                    ) {
                        if (isset($form['donations'][$key]['plant_info']['plant_tree'])) {
                            // Reset dependent field for tree species.
                            form_set_value(
                                $form['donations'][$key]['plant_info']['plant_tree'],
                                null,
                                $form_state
                            );
                        }
                    }

                    // Save updated plant info field values in the storage.
                    $form_state['storage']['donations'][$key]['plant_info'] = $form_state['values']['donations'][$key]['plant_info'];

                    // Validate whether product exists with given plant info.
                    // This should never be necessary, since available plant information
                    // is being retrieved from existing Drupal Commerce products, but we
                    // are better avoiding invalid form submissions.
                    if (
                        !empty($form_state['values']['donations'][$key]['plant_info']['plant_period'])
                        && !empty($form_state['values']['donations'][$key]['plant_info']['plant_region'])
                        && !empty($form_state['values']['donations'][$key]['plant_info']['plant_tree'])
                    ) {
                        try {
                            DrupalBaumspendenDonation::getCommerceProduct(
                                $submitted_plant_info
                            );
                        } catch (Exception $exception) {
                            form_error(
                                $form['donations'][$key]['plant_info'],
                                $exception->getMessage()
                            );
                        }
                    }
                }
            }
        }
    }

    /**
     * Handles validated submissions of the donation form.
     *
     * @param $form
     * @param $form_state
     */
    public static function submit($form, &$form_state)
    {
        // Depend on which element trigered the submission.
        switch ($form_state['triggering_element']['#baumspenden_op']) {
            case 'update_plant_info':
                // Updating plant info fields. Simply rebuild the form.
                $form_state['rebuild'] = true;
                break;
            case 'add_donation':
                // Add values for another donation fieldset and rebuild the form.
                $form_state['storage']['donations'][] = [];
                $form_state['rebuild'] = true;
                break;
            case 'remove_donation':
                // Remove the donation fieldset values and rebuild the form.
                $key = $form_state['triggering_element']['#baumspenden_donation_key'];
                unset($form_state['storage']['donations'][$key]);
                $form_state['rebuild'] = true;
                break;
            case 'to_form':
                // Rebuild the form with "form" step.
                $form_state['step'] = 'form';
                $form_state['rebuild'] = true;
                break;
            case 'preview':
                // Store input values and rebuild the form with "preview" step.
                form_state_values_clean($form_state);
                $form_state['storage'] = $form_state['values'];
                $form_state['step'] = 'preview';
                $form_state['rebuild'] = true;
                break;
            case 'submit':
                // Finally process the form, or rebuild the form with "form"
                // step, when processing fails.
                try {
                    DrupalBaumspendenDonationForm::process(
                        $form_state['storage']
                    );
                } catch (Exception $exception) {
                    // Log the exception to the watchdog.
                    watchdog_exception('baumspenden', $exception);
                    // Show a generic error message to the user.
                    drupal_set_message(
                        t('There was an error processing the form.'),
                        'error'
                    );
                    // Rebuild the form.
                    $form_state['step'] = 'form';
                    $form_state['rebuild'] = true;
                }
                break;
        }
    }

    /**
     * Processes final submissions of the donation form.
     *
     * @param $storage
     *
     * @throws \Exception
     *   When the form could not be processed.
     */
    public static function process($storage)
    {
        // TODO: Process newsletter request.

        // Process donations.
        $donations = [];
        foreach ($storage['donations'] as $key => $donation_data) {
            $donations[] = new DrupalBaumspendenDonation(
                $donation_data['amount'],
                $donation_data['plant_info'],
                $donation_data['certificate_name']
            );
        }

        // Create and checkout an order with donations and contact data.
        if (!isset($storage['presentee'])) {
            $storage['presentee'] = [];
        }
        $order = DrupalBaumspendenOrder::create(
            $donations,
            $storage['contact'],
            $storage['presentee']
        );
        $order->checkout();
        // Checkout issues a redirect, therefore, we're done here.
    }

    /**
     * Adds an #element_validate callback for validating conditionally required
     * form elements according to their #states definition.
     *
     * This is called as an #after_build callback for the donation form.
     *
     * @param array $element
     *   The form or form element to process.
     *
     * @return array
     *   The potentially altered form element.
     */
    public static function addRequiredStatesValidation($element)
    {
        // Call recursively for all element children.
        foreach (element_children($element) as $child_element) {
            $element[$child_element] = self::addRequiredStatesValidation(
                $element[$child_element]
            );
        }

        if (
            !empty($element['#states'])
            && array_key_exists('required', $element['#states'])
        ) {
            $element['#element_validate'][] = [
                'DrupalBaumspendenDonationForm',
                'validateRequiredStates',
            ];
        }

        return $element;
    }

    /**
     * Validates conditionally required form elements according to their #states
     * definition.
     *
     * @param $element
     *   The form element to validate.
     * @param $form_state
     */
    public static function validateRequiredStates($element, &$form_state)
    {
        $element_value = drupal_array_get_nested_value(
            $form_state['values'],
            $element['#parents']
        );
        if (
            self::isStatesRequired($element, $form_state)
            && empty($element_value)
        ) {
            form_error(
                $element,
                t(
                    '!name field is required.',
                    ['!name' => $element['#title']]
                )
            );
        }
    }

    /**
     * Checks whether a form element is required according to its #states
     * definition.
     *
     * @param $element
     *   The form element to check for requiredness.
     * @param $form_state
     *
     * @return bool
     *   Whether the element is required.
     */
    public static function isStatesRequired($element, &$form_state)
    {
        if (!empty($element['#states']['required'])) {
            foreach ($element['#states']['required'] as $required_field_key => $conditions) {
                // TODO: This only covers selectors like :input[name="field_name"],
                //   i.e. where the field name is the first occurence of the text
                //   wrapped in double quotes (").
                $required_field = explode('"', $required_field_key);
                $required_field_parents = preg_split(
                    '/\]?\[/',
                    rtrim($required_field[1], ']')
                );
                $required_field_value = drupal_array_get_nested_value(
                    $form_state['values'],
                    $required_field_parents
                );
                foreach ($conditions as $type => $condition) {
                    switch ($type) {
                        case 'value':
                            $this_matched = ($form_state['values'][$required_field[1]] == $element['#states']['required'][$required_field_key]['value']);
                            if (!isset($matched)) {
                                $matched = $this_matched;
                            } else {
                                $matched = $matched && $this_matched;
                            }
                            break;
                        case 'checked':
                            // Unchecked checkboxes have an integer value of 0,
                            // whereas a string value of "0" might be a valid value.
                            $this_matched = ($condition == ($required_field_value !== 0));
                            if (!isset($matched)) {
                                $matched = $this_matched;
                            } else {
                                $matched = $matched && $this_matched;
                            }
                            break;
                        /* TODO: Add cases for the following states, if needed:
                         *     - empty
                         *     - filled
                         *     - unchecked
                         *     - expanded
                         *     - collapsed
                         *     - value
                         */
                    }
                }
            }
        }

        return (isset($matched) && $matched);
    }

    /**
     * Returns plant info elements of the "baumspenden_donation" form to be
     * replaced by an Ajax request.
     *
     * @param $form
     * @param $form_state
     *
     * @return array
     */
    public static function replacePlantInfo($form, &$form_state)
    {
        $triggering_element_parents = $form_state['triggering_element']['#array_parents'];
        $plant_info_parents = array_slice($triggering_element_parents, 0, 3);
        return drupal_array_get_nested_value($form, $plant_info_parents);
    }

    /**
     * Returns donations elements of the "baumspenden_donation" form to be
     * replaced by an Ajax request.
     *
     * @param $form
     * @param $form_state
     *
     * @return array
     */
    public static function replaceDonations($form, &$form_state)
    {
        return $form['donations'];
    }
}
