<?php

/**
 * @file
 * Contains the ContestPollHelper class.
 */

/**
 * Helper class for poll contests.
 */
class ContestPollHelper {

  /**
   * Process the poll form to simplify variable access.
   *
   * @param array $state
   *   The submitted contest poll entry form.
   *
   * @return array
   *   An array of submitted values.
   */
  public static function getValues(array $state = array()) {
    $form_state = array(
      'values' => array(
        'nid'  => 0,
        'uid'  => 0,
        'mail' => '',
      ),
    );
    if (!empty($state['values'])) {
      $form_state['values'] = array_merge($form_state['values'], $state['values']);
      $form_state['values']['nid'] = $state['values']['contest_id'];
    }
    return $form_state;
  }

}
