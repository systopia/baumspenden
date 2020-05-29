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
 * Class DrupalBaumspendenCiviMRF
 */
class DrupalBaumspendenCiviMRF
{
    /**
     * Retrieves a CiviMRF connector ID to use for sending calls to the CiviCRM
     * REST API using the CiviMRF module (cmrf_core).
     *
     * @param null $profile
     *
     * @return string
     */
    public static function getConnectorId($profile = null)
    {
        $connector_name = 'baumspenden_cmrf_connector';
        if (!empty($profile)) {
            $connector_name .= ':' . $profile;
        }

        $core = cmrf_core_get_core();
        $connector_id = variable_get($connector_name);
        if (empty($connector_id)) {
            $connector_id = $core->registerConnector($connector_name, $profile);
            variable_set($connector_name, $connector_id);
        }
        return $connector_id;
    }

    /**
     * Sends a call to the CiviCRM REST API using the CiviMRF module
     * (cmrf_core).
     *
     * @param $entity
     * @param $action
     * @param $parameters
     * @param $options
     * @param null $profile
     * @param array $callbacks
     *
     * @return \CMRF\Core\Call
     */
    public static function sendCall(
        $entity,
        $action,
        $parameters = [],
        $options = [],
        $profile = null,
        $callbacks = []
    ) {
        $connector_id = self::getConnectorId($profile);
        $core = cmrf_core_get_core();
        $call = $core->createCall(
            $connector_id,
            $entity,
            $action,
            $parameters,
            $options,
            $callbacks
        );
        $core->executeCall($call);
        return $call;
    }
}
