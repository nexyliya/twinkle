<?php

/**
 * @file
 * Contains the ContestWebformStorage class.
 */

/**
 * Storage class for contest webforms.
 */
class ContestWebformStorage {

  /**
   * Fetch data to deciding available actions.
   *
   * @param int $nid
   *   The webform's node ID.
   *
   * @return bool
   *   True if the webform exists.
   */
  public static function webformExists($nid) {
    return (bool) db_query("SELECT 1 FROM {node} WHERE nid = :nid AND  type = 'webform'", array(':nid' => $nid))->fetchField();
  }

}
