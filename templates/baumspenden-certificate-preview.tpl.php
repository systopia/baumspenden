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

?>

<style type="text/css">
    #page-<?php print $certificate_key; ?> {
        /* Background image fully viewable without repeating. */
        background-image: url('<?php print $background_image_url; ?>');
        background-size: contain;
        background-repeat: no-repeat;
        /* Proportional scaling for ISO 216 "A4" paper format. */
        width: auto;
        max-width: 210mm;
        height: 0;
        padding-bottom: 141.42857%;
        /* Center in parent element. */
        margin: 0 auto;
    }
</style>

<div id="page-<?php print $certificate_key; ?>">

    <div class="baumspenden-certificate__certificate-name">
        <?php print $certificate_name; ?>
    </div>

    <div class="baumspenden-certificate__amount">
        <?php print $amount; ?>
    </div>

    <div class="baumspenden-certificate__plant-period">
        <?php print $plant_info['plant_period']; ?>
    </div>

    <div class="baumspenden-certificate__plant-region">
        <?php print $plant_info['plant_region']; ?>
    </div>

    <div class="baumspenden-certificate__plant-tree">
        <?php print $plant_info['plant_tree']; ?>
    </div>

</div>
