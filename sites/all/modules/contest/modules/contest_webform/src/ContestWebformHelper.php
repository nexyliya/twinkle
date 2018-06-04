<?php

/**
 * @file
 * Contains the ContestWebformHelper class.
 */

/**
 * Helper class for webform contests.
 */
class ContestWebformHelper {

  /**
   * Extract the contest node ID from the form.
   *
   * @param array $form
   *   The webform array.
   *
   * @return int
   *   The contest's node ID.
   */
  public static function getContestId(array $form = array()) {
    if (empty($form['submitted'])) {
      return 0;
    }
    foreach ($form['submitted'] as $element) {
      if (!empty($element['#webform_component']['type']) && $element['#webform_component']['type'] == 'contest') {
        return !empty($element['#webform_component']['value']) ? $element['#webform_component']['value'] : 0;
      }
    }
    return 0;
  }

  /**
   * Process the webform to simplify access.
   *
   * @param string $op
   *   The current operation, (validate, submit).
   * @param array $state
   *   The submitted contest webform entry form.
   *
   * @return array
   *   An array of submitted values.
   */
  public static function getValues($op, array $state = array()) {
    $form_state = array(
      'values' => array(
        'nid'  => 0,
        'uid'  => 0,
        'mail' => '',
      ),
    );
    switch ($op) {
      case 'validate':
        if (isset($state['values']['submitted']['contest_optin']) && !empty($state['values']['submitted']['fieldset'])) {
          $form_state['values'] = array_merge($state['values']['submitted'], $state['values']['submitted']['fieldset']);
          unset($form_state['values']['fieldset']);
        }
        break;

      case 'submit':
        foreach ($state['values']['submitted'] as $values) {
          if (isset($values['contest_optin']) && !empty($values['fieldset'])) {
            $form_state['values'] = array_merge($values, $values['fieldset']);
            $form_state['values']['nid'] = $values['fieldset']['contest_id'];
            unset($form_state['values']['contest_id']);
            unset($form_state['values']['fieldset']);
            break 2;
          }
        }
        break;
    }
    return $form_state;
  }

}
