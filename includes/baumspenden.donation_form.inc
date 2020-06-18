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
            if (is_numeric($key)) {
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
                ];

                if (isset($form_state['storage']['donations'][$key]['plant_info']['plant_period'])) {
                    $form['donations'][$key]['plant_info']['plant_region'] = [
                        '#type' => 'select',
                        '#title' => 'Region',
                        '#options' => DrupalBaumspendenConfig::getPlantRegions(
                            $form_state['storage']['donations'][$key]['plant_info']['plant_period']
                        ),
                        '#required' => true,
                        '#ajax' => [
                            'wrapper' => 'baumspenden-plant-info--' . $key,
                            'callback' => [
                                'DrupalBaumspendenDonationForm',
                                'replacePlantInfo',
                            ],
                        ],
                    ];
                }

                if (isset($form_state['storage']['donations'][$key]['plant_info']['plant_region'])) {
                    $form['donations'][$key]['plant_info']['plant_tree'] = [
                        '#type' => 'select',
                        '#title' => 'Baumart',
                        '#options' => DrupalBaumspendenConfig::getTreeSpecies(
                            $form_state['storage']['donations'][$key]['plant_info']['plant_period'],
                            $form_state['storage']['donations'][$key]['plant_info']['plant_region']
                        ),
                        '#required' => true,
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
                    '#attached' => [
                        'css' => [
                            drupal_get_path(
                                'module',
                                'baumspenden'
                            ) . '/css/baumspende.css',
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
        ];
        $form['contact']['last_name'] = [
            '#type' => 'textfield',
            '#title' => 'Nachname',
            '#required' => true,
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
        ];
        $form['contact']['email'] = [
            '#type' => 'textfield',
            '#title' => 'E-Mail-Adresse',
            '#required' => true,
        ];

        $form['as_present'] = [
            '#type' => 'checkbox',
            '#title' => 'Als Geschenk',
            '#description' => 'Ich möchte das Zertifikat verschenken.',
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
        ];
        $form['presentee']['last_name'] = [
            '#type' => 'textfield',
            '#title' => 'Nachname',
            '#states' => [
                'required' => [
                    ':input[name="as_present"]' => ['checked' => true],
                ],
            ],
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
        ];

        $form['newsletter'] = [
            '#type' => 'checkbox',
            '#title' => 'Newsletter abonnieren',
            '#description' => 'Newsletter mit aktuellen Themen rund um das Bergwaldprojekt',
        ];

        $form['gdpr_consent'] = [
            '#type' => 'checkbox',
            '#title' => 'Datenschutz',
            '#description' => 'Ich habe die <a href="' . variable_get(
                    'baumspenden_gdpr_link',
                    '/datenschutz'
                ) . '" target="_blank">Datenschutzerklärung</a> gelesen und akzeptiere diese.',
            '#required' => true,
        ];

        $form['actions'] = [
            '#type' => 'actions',
        ];
        $form['actions']['submit'] = [
            '#type' => 'submit',
            '#baumspenden_op' => 'preview',
            '#value' => 'Vorschau',
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
            self::isStatesRequired($form['contact']['email'], $form_state)
            && !valid_email_address($form_state['values']['contact']['email'])
        ) {
            form_error(
                $form['contact']['email'],
                'Bitte geben Sie eine gültige E-Mail-Adresse ein.'
            );
        }
        if (
            self::isStatesRequired($form['presentee']['email'], $form_state)
            && !valid_email_address($form_state['values']['presentee']['email'])
        ) {
            form_error(
                $form['contact']['email'],
                'Bitte geben Sie eine gültige E-Mail-Adresse ein.'
            );
        }

        // Handle hierarchically dependent submissions of plant info fields.
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
            case 'preview':
                // Preview (next step)
                // TODO: Preview step.
                // break;
            case 'submit':
                form_state_values_clean($form_state);
                $form_state['storage'] = $form_state['values'];
                try {
                    DrupalBaumspendenDonationForm::process(
                        $form_state['storage']
                    );
                } catch (Exception $exception) {
                    watchdog_exception('baumspenden', $exception);
                    drupal_set_message(
                        t('There was an error processing the form.'),
                        'error'
                    );
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

        // Create an order with donations and contact data.
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
        foreach ($element['#states']['required'] as $required_field_key => $conditions) {
            // TODO: This only covers selectors like :input[name="field_name"],
            //   i.e. where the field name is the first occurence of the text
            //   wrapped in double quotes (").
            $required_field = explode('"', $required_field_key);
            $required_field_parents = preg_split(
                '/\]?\[/',
                rtrim($required_field[1], ']')
            );
            foreach ($conditions as $type => $condition) {
                switch ($type) {
                    case 'value':
                        $required_field_value = drupal_array_get_nested_value(
                            $form_state['values'],
                            $required_field_parents
                        );
                        $this_matched = $form_state['values'][$required_field[1]] == $element['#states']['required'][$required_field_key]['value'];
                        if (!isset($matched)) {
                            $matched = $this_matched;
                        } else {
                            $matched = $matched && $this_matched;
                        }
                        break;
                    case 'checked':
                        $required_field_value = drupal_array_get_nested_value(
                            $form_state['values'],
                            $required_field_parents
                        );
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
