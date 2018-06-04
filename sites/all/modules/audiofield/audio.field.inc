<?php

/**
 * @file
 * Implement an audio field, based on the file module's file field.
 */

/**
 * Implements hook_field_prepare_view().
 */
function audiofield_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {
  // TODO: Check this.
  // Remove files specified to not be displayed.
  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => $item) {
      if (!file_field_displayed($item, $field)) {
        unset($items[$id][$delta]);
      }
      // Ensure consecutive deltas.
      $items[$id] = array_values($items[$id]);
    }
  }
}

/**
 * Implements hook_field_is_empty().
 */
function audiofield_field_is_empty($item, $field) {
  return file_field_is_empty($item, $field);
}

/**
 * Implements hook_field_widget_info().
 */
function audiofield_field_widget_info() {
  return array(
    'audiofield_widget' => array(
      'label' => t('Audio Upload'),
      'field types' => array('file'),
      'settings' => array(
        'progress_indicator' => 'throbber',
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_settings_form().
 */
function audiofield_field_widget_settings_form($field, $instance) {
  $widget = $instance['widget'];
  $settings = $widget['settings'];

  $form['progress_indicator'] = array(
    '#type' => 'radios',
    '#title' => t('Progress indicator'),
    '#options' => array(
      'throbber' => t('Throbber'),
      'bar' => t('Bar with progress meter'),
    ),
    '#default_value' => $settings['progress_indicator'],
    '#description' => t('The throbber display does not show the status of uploads but takes up less space. The progress bar is helpful for monitoring progress on large uploads.'),
    '#weight' => 16,
    '#access' => file_progress_implementation(),
  );

  return $form;
}

/**
 * Implements hook_field_widget_form().
 */
function audiofield_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {

  // Add display_field setting; file_field_widget_form() assumes it is set.
  $field['settings']['display_field'] = 0;

  $elements = file_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);

  if ($field['cardinality'] == 1) {
    // If there's only one field, return it as delta 0.
    if (empty($elements[0]['#default_value']['fid'])) {
      $elements[0]['#description'] = theme('file_upload_help', array(
        'description' => $instance['description'],
        'upload_validators' => $elements[0]['#upload_validators'],
      ));
    }
  }
  else {
    $elements['#file_upload_description'] = theme('file_upload_help', array('upload_validators' => $elements[0]['#upload_validators']));
  }

  return $elements;
}

/**
 * An element #process callback for the audiofield_widget field type.
 *
 * Display audio player in node edit mode.
 */
function audiofield_field_widget_process($element, &$form_state, $form) {
  $fid = isset($element['#value']['fid']) ? $element['#value']['fid'] : 0;

  $item = $element['#value'];
  $item['fid'] = $element['fid']['#value'];
  $field = field_widget_field($element, $form_state);
  $instance = field_widget_instance($element, $form_state);
  // Add the display field if enabled.
  if (!empty($field['settings']['display_field']) && $item['fid']) {
    $element['display'] = array(
      '#type' => empty($item['fid']) ? 'hidden' : 'checkbox',
      '#title' => t('Include file in display'),
      '#value' => isset($item['display']) ? $item['display'] : $field['settings']['display_default'],
      '#attributes' => array('class' => array('file-display')),
    );
  }
  else {
    $element['display'] = array(
      '#type' => 'hidden',
      '#value' => '1',
    );
  }
  if ($fid && $element['#file']) {
    $info = pathinfo($element['#file']->uri);
    $op = $info['extension'];
    $element['filename'] = array(
      'player' => audiofield_get_player($element['#file']->uri, $op),
      '#weight' => -10,
    );
  }

  // Add the description field if enabled.
  if (!empty($instance['settings']['description_field']) && $item['fid']) {
    $element['description'] = array(
      '#title' => t('Description'),
      '#value' => isset($item['description']) ? $item['description'] : '',
      '#type' => variable_get('file_description_type', 'textfield'),
      '#maxlength' => variable_get('file_description_length', 128),
      '#description' => t('The description may be used as the label of the link to the file.'),
    );
  }

  /*
   * Adjust the Ajax settings so that on upload and remove of any individual
   * file, the entire group of file fields is updated together.
   */
  if ($field['cardinality'] != 1) {
    $parents = array_slice($element['#array_parents'], 0, -1);
    $new_path = 'file/ajax/' . implode('/', $parents) . '/' . $form['form_build_id']['#value'];
    $field_element = drupal_array_get_nested_value($form, $parents);
    $new_wrapper = $field_element['#id'] . '-ajax-wrapper';
    foreach (element_children($element) as $key) {
      if (isset($element[$key]['#ajax'])) {
        $element[$key]['#ajax']['path'] = $new_path;
        $element[$key]['#ajax']['wrapper'] = $new_wrapper;
      }
    }
    unset($element['#prefix'], $element['#suffix']);
  }

  /*
   * Add another submit handler to the upload and remove buttons, to implement
   * functionality needed by the field widget. This submit handler, along with
   * the rebuild logic in audiofield_field_widget_form() requires the entire
   * field, not just the individual item, to be valid.
   */
  foreach (array('upload_button', 'remove_button') as $key) {
    $element[$key]['#submit'][] = 'file_field_widget_submit';
    $element[$key]['#limit_validation_errors'] = array(
      array_slice($element['#parents'], 0, -1),
    );
  }

  return $element;
}

/**
 * Implements hook_field_formatter_info().
 */
function audiofield_field_formatter_info() {
  $formatters = array(
    'audiofield_embedded' => array(
      'label' => t('Audio player with download'),
      'field types' => array('file'),
      'description' => t('Displays an audio player and optional download link.'),
    ),
    'audiofield_nodownload' => array(
      'label' => t('Audio player only'),
      'field types' => array('file'),
      'description' => t('Displays only an audio player.'),
    ),
    'audiofield_details' => array(
      'label' => t('Audio player with details'),
      'field types' => array('file'),
      'description' => t('Displays only an audio player with details.'),
    ),
  );

  return $formatters;
}

/**
 * Implements hook_field_formatter_view().
 *
 * TODO: Implement playlists to group audios hold in multiple valued fields.
 */
function audiofield_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $elements = array();

  switch ($display['type']) {
    case 'audiofield_embedded':
    case 'audiofield_nodownload':
    case 'audiofield_details':
      global $user;
      foreach ($items as $delta => $item) {
        $info = pathinfo($item['uri']);
        $op = $info['extension'];
        $options = array(
          'entity_type' => $entity_type,
          'entity' => $entity,
          'field' => $field,
          'instance' => $instance,
          'langcode' => $langcode,
          'item' => $item,
          'display' => $display,
        );

        $elements[$delta] = array(
          'player' => audiofield_get_player($item['uri'], $op, $options),
        );
        // Display the file description if one is available.
        if (isset($item['description']) && !empty($item['description'])) {
          $elements[$delta]['description'] = array(
            '#type' => 'container',
            '#attributes' => array(
              'class' => 'description',
            ),
            '#children' => $item['description'],
          );
        }
        // Display download link.
        if ($display['type'] == 'audiofield_embedded' && (user_access('download all audio files') || ($user->uid == $item['uid'] && user_access('download own audio files')))) {
          $elements[$delta]['download'] = array(
            '#type' => 'container',
            '#attributes' => array(
              'class' => array(
                'audio-download',
              ),
            ),
            '#children' => t('<strong>Download</strong>: !file_link', array(
              '!file_link' => theme('file_link', array('file' => (object) $item)),
            )),
          );
        }
      }
      break;
  }

  return $elements;
}
