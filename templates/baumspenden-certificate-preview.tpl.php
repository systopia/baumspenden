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
    }
</style>

<div id="page-<?php print $certificate_key; ?>" class="preview-page">

    <div class="baumspenden-certificate__certificate-name">
        <?php print $certificate_name; ?>
    </div>

    <div class="baumspenden-certificate__amount">
        <?php print $amount; ?>
    </div>

    <div class="baumspenden-certificate__plant-period">
        <?php print render($plant_info['plant_period']); ?>
    </div>

    <div class="baumspenden-certificate__plant-region">
        <?php print render($plant_info['plant_region']); ?>
    </div>

    <div class="baumspenden-certificate__plant-tree">
        <?php print render($plant_info['plant_tree']); ?>
    </div>

</div>
