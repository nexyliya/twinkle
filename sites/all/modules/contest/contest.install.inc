<?php

/**
 * @file
 * Support functions for installation and updates.
 */

/**
 * Send the feedback to the appropriate place.
 *
 * @param string $txt
 *   The string to print.
 */
function _contest_install_feedback($txt) {
  $t = get_t();

  if (defined('STDIN')) {
    drush_log($t($txt));
  }
  else {
    drupal_set_message($t($txt));
  }
}

/**
 * Create the contest content type.
 */
function _contest_install_type_create() {
  $contest_node_type = array(
    'type'        => 'contest',
    'name'        => t('Contest'),
    'base'        => 'contest',
    'description' => t('A <em>contest</em> is a content type that allows a user to enter to win prizes via a random selection.'),
    'custom'      => FALSE,
    'modified'    => FALSE,
    'locked'      => TRUE,
  );
  $contest_node_type = node_type_set_defaults($contest_node_type);

  node_add_body_field($contest_node_type);

  node_type_save($contest_node_type);

  variable_set('node_options_contest', array('status'));
  variable_set('node_preview_contest', '1');
  variable_set('node_submitted_contest', 0);
}

/**
 * Create the user fields used by a contest.
 */
function _contest_install_user_fields() {
  $field_prefix = 'field_contest';
  $fields = array(
    (object) array(
      'label'  => 'Real Name',
      'name'   => 'name',
      'desc'   => 'Your real name for contact/legal reasons.',
      'type'   => 'text',
      'widget' => 'textfield',
      'length' => 50,
    ),
    (object) array(
      'label'  => 'Business',
      'name'   => 'business',
      'desc'   => '',
      'type'   => 'text',
      'widget' => 'textfield',
      'length' => 50,
    ),
    (object) array(
      'label'  => 'Address',
      'name'   => 'address',
      'desc'   => '',
      'type'   => 'text',
      'widget' => 'textfield',
      'length' => 50,
    ),
    (object) array(
      'label'  => 'City',
      'name'   => 'city',
      'desc'   => '',
      'type'   => 'text',
      'widget' => 'textfield',
      'length' => 50,
    ),
    (object) array(
      'label'  => 'State',
      'name'   => 'state',
      'desc'   => '',
      'type'   => 'text',
      'widget' => 'textfield',
      'length' => 50,
    ),
    (object) array(
      'label'  => 'Zip',
      'name'   => 'zip',
      'desc'   => '',
      'type'   => 'text',
      'widget' => 'textfield',
      'length' => 10,
    ),
    (object) array(
      'label'  => 'Birthday',
      'name'   => 'birthdate',
      'desc'   => '',
      'type'   => 'datestamp',
      'widget' => 'date_popup',
      'length' => 1,
    ),
    (object) array(
      'label'  => 'Phone',
      'name'   => 'phone',
      'desc'   => '',
      'type'   => 'text',
      'widget' => 'textfield',
      'length' => 20,
    ),
    (object) array(
      'label'  => 'Opt In',
      'name'   => 'optin',
      'desc'   => 'I would like to receive promotional efforts from the contest sponsors and promoters.',
      'type'   => 'list_boolean',
      'widget' => 'options_onoff',
      'length' => 1,
    ),
  );
  $weight = (count($fields) + 1) * -10;

  foreach ($fields as $index => $field) {
    if (field_info_field("{$field_prefix}_{$field->name}")) {
      continue;
    }
    $prototype = array(
      'field_name' => "{$field_prefix}_{$field->name}",
      'type'       => $field->type,
      'settings'   => array('max_length' => $field->length),
    );
    if ($field->type == 'list_boolean') {
      $prototype['settings']['display_label'] = 1;
      $prototype['settings']['allowed_values'] = array(
        '0' => 'No',
        '1' => $field->label,
      );
    }
    elseif ($field->type == 'datestamp') {
      $prototype['settings']['granularity'] = array(
        'month'  => 'month',
        'day'    => 'day',
        'year'   => 'year',
        'hour'   => 0,
        'minute' => 0,
        'second' => 0,
      );
    }
    field_create_field($prototype);

    $instance = array(
      'field_name'  => "{$field_prefix}_{$field->name}",
      'entity_type' => 'user',
      'label'       => $field->label,
      'bundle'      => 'user',
      'required'    => FALSE,
      'settings'    => array('user_register_form' => 0),
      'widget'      => array(
        'type'   => $field->widget,
        'weight' => $weight + ($index * 10),
      ),
    );
    if ($field->type == 'list_boolean') {
      $instance['settings']['allowed_values'] = array(
        '0' => 'No',
        '1' => $field->label,
      );
      $instance['widget']['settings']['display_label'] = 1;
      $instance['display']['default']['type'] = 'inline';
    }
    elseif ($field->type == 'datestamp') {
      $instance['widget']['settings']['date_increment'] = ContestStorage::DAY;
      $instance['widget']['settings']['date_timezone'] = date_default_timezone();
      $instance['widget']['settings']['input_format'] = 'Y-m-d';
      $instance['widget']['settings']['label_position'] = 'invisible';
      $instance['widget']['settings']['year_range'] = '-100:+0';
    }
    field_create_instance($instance);
  }
}
