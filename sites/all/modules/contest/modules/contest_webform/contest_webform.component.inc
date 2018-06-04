<?php

/**
 * @file
 * Webform module contest component.
 */

/**
 * Theme function for component.
 */
function theme_webform_display_contest($vars) {
  return ($vars['element']['#format'] == 'html') ? check_plain($vars['element']['#markup']) : $vars['element']['#markup'];
}

/**
 * Implements _webform_action_set_component().
 */
function _webform_action_set_contest($component, &$element, &$form_state, $value) {
  $element['#value'] = $value;
  form_set_value($element, $value, $form_state);
}

/**
 * Implements _webform_analysis_component().
 */
function _webform_analysis_contest($component, $sids = array(), $single = FALSE, $join = NULL) {
  $nonblanks = 0;
  $submissions = 0;
  $wordcount = 0;

  $query = db_select('webform_submitted_data', 'wsd', array('fetch' => PDO::FETCH_ASSOC))
    ->fields('wsd', array('no', 'data'))
    ->condition('wsd.nid', $component['nid'])
    ->condition('wsd.cid', $component['cid']);

  if (count($sids)) {
    $query->condition('wsd.sid', $sids, 'IN');
  }
  if ($join) {
    $query->innerJoin($join, 'ws2_', 'wsd.sid = ws2_.sid');
  }
  $result = $query->execute();

  foreach ($result as $data) {
    if (strlen(trim($data['data'])) > 0) {
      $nonblanks++;
      $wordcount += str_word_count(trim($data['data']));
    }
    $submissions++;
  }
  $rows[0] = array(t('Empty'), ($submissions - $nonblanks));
  $rows[1] = array(t('Non-empty'), $nonblanks);
  $other[0] = array(t('Average submission length in words (ex blanks)'), ($nonblanks != 0 ? number_format($wordcount / $nonblanks, 2) : '0'));

  return array('table_rows' => $rows, 'other_data' => $other);
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_contest($component, $export_options, $value) {
  return isset($value[0]) ? $value[0] : '';
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_contest($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2] = $export_options['header_keys'] ? $component['form_key'] : $component['name'];

  return $header;
}

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_contest() {
  return array(
    'name'     => '',
    'form_key' => NULL,
    'pid'      => 0,
    'weight'   => 0,
    'value'    => '',
    'extra'    => array(
      'private'     => FALSE,
      'hidden_type' => 'value',
      'analysis'    => FALSE,
    ),
  );
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_contest($component, $value, $format = 'html', $submission = array()) {
  $element = array(
    '#title'          => $component['name'],
    '#markup'         => isset($value[0]) ? $value[0] : NULL,
    '#weight'         => $component['weight'],
    '#format'         => $format,
    '#theme'          => 'webform_display_hidden',
    '#theme_wrappers' => $format == 'text' ? array('webform_element_text') : array('webform_element'),
    '#translatable'   => array('title'),
  );
  return $element;
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_contest($component) {
  $form['value'] = array(
    '#title'         => t('Contest'),
    '#description'   => t('The contest to link to this webform.') . ' ' . theme('webform_token_help'),
    '#type'          => 'select',
    '#default_value' => $component['value'],
    '#options'       => ContestHelper::getFormOptions(),
    '#weight'        => 0,
  );
  $form['hidden_type'] = array(
    '#type'    => 'value',
    '#value'   => 'value',
    '#parents' => array('extra', 'hidden_type'),
  );
  return $form;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_contest($component, $value = NULL, $filter = TRUE, $submission = NULL) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;

  $default_value = $filter ? webform_replace_tokens($component['value'], $node) : $component['value'];

  if (isset($value[0])) {
    $default_value = $value[0];
  }
  $element = array(
    '#title'        => $filter ? NULL : $component['name'],
    '#weight'       => $component['weight'],
    '#translatable' => array('title'),
  );
  if ($component['extra']['hidden_type'] == 'value') {
    $element['#type'] = 'value';
    $element['#value'] = $default_value;
  }
  else {
    $element['#type'] = 'hidden';
    $element['#default_value'] = $default_value;

    $element['#theme_wrappers'] = array('webform_element');
    $element['#wrapper_attributes']['class'] = array();
    $element['#wrapper_attributes']['style'] = array('display: none');
  }
  return $element;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_table_contest($component, $value) {
  return check_plain(empty($value[0]) ? '' : $value[0]);
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_contest() {
  return array(
    'webform_display_contest' => array(
      'render element' => 'element',
    ),
  );
}
