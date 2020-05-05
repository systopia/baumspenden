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
    public static function process($storage)
    {
        $donations = [];
        foreach ($storage['donations'] as $key => $donation_data) {
            $donations[] = new DrupalBaumspendenDonation(
                $donation_data['amount'],
                $donation_data['plant_info'],
                $donation_data['certificate_name']
            );
        }

        $order = new DrupalBaumspendenOrder($donations, $storage['contact']);
        $order->checkout();
    }
}
