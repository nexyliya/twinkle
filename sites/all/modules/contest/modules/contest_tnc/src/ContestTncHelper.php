<?php

/**
 * @file
 * Contains the ContestTncHelper class.
 */

/**
 * Helper class for the contest terms and conditions.
 */
class ContestTncHelper {

  /**
   * Get the fully rendered terms and conditions HTML.
   *
   * @param object $node
   *   The contest node object.
   *
   * @return string
   *   The terms and conditions rendered HTML.
   */
  public static function getTnc($node) {
    $args = array(
      'node'   => $node,
      'tnc'    => ContestStorage::getBlockBody(variable_get('contest_tnc_block', 0)),
      'format' => ContestStorage::getBlockFormat(variable_get('contest_tnc_block', 0)),
    );
    return theme('contest_tnc', $args);
  }

}
