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
 * Class DrupalBaumspendenUtils
 */
class DrupalBaumspendenUtils
{
    /**
     * #element_validate callback for the BIC field to validate it according to
     * the IBAN.
     *
     * @param $element
     * @param $form_state
     * @param $form
     */
    public static function elementValidateBIC(&$element, &$form_state, $form)
    {
        $iban_parents = array_merge(
            $element['#field_parents'],
            [
                DrupalBaumspendenConfig::FIELD_NAME_IBAN,
                LANGUAGE_NONE,
                0,
                'iban_field',
            ]
        );
        $iban = drupal_array_get_nested_value(
            $form_state['values'],
            $iban_parents
        );
        if (!empty($iban)) {
            $bic_call = DrupalBaumspendenCiviMRF::sendCall(
                'Bic',
                'getfromiban',
                ['iban' => $iban]
            );

            if (
                $bic_call->getStatus() == \CMRF\Drupal\Call::STATUS_DONE
                && ($bic_reply = $bic_call->getReply())
                && !empty($bic_reply['bic'])
            ) {
                // Set value in $form_state['values'].
                form_set_value($element, $bic_reply['bic'], $form_state);
                // Set value in the element, in order for it to be shown on the rebuilt form after validation.
                $element['#value'] = $bic_reply['bic'];
                // TODO: Why is setting $form_state['input'] also necessary?
                drupal_array_set_nested_value(
                    $form_state['input'],
                    $element['#parents'],
                    $bic_reply['bic'],
                    true
                );
                drupal_set_message(
                    'Ihre BIC wurde automatisch ermittelt: ' . $bic_reply['bic']
                );
            } else {
                // Set value in $form_state['values'].
                form_set_value($element, '', $form_state);
                // Set value in the element, in order for it to be shown on the rebuilt form after validation.
                $element['#value'] = '';
                form_error(
                    $element,
                    'Die BIC konnte nicht automatisch ermittelt werden, bitte geben Sie diese ein.'
                );
            }
        }
    }

    /**
     * Drupal Form API Ajax callback for validation of IBAN/BIC on the Drupal
     * Commerce SEPA Direct Debit payment details form.
     *
     * @param $form
     * @param $form_state
     *
     * @return mixed
     */
    public static function paymentSepaAjaxCallback($form, &$form_state)
    {
        return $form['commerce_payment']['payment_details'];
    }

    /**
     * Submit handler for Ajax-driven validation of IBAN/BIC on the Drupal
     * Commerce SEPA Direct Debit payment details form.
     *
     * @param $form
     * @param $form_state
     */
    public static function paymentSepaBicSubmit($form, &$form_state) {
        // Simply rebuild the form.
        $form_state['rebuild'] = true;
    }
}
