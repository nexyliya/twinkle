<?php

/**
 * @file
 * Admin page callbacks for the contest_tnc module.
 */

/**
 * Menu callback to configure module settings.
 */
function contest_tnc_settings_form($form) {
  drupal_add_css('#contest-tnc-settings-form .form-type-radio { display:inline; padding-right:1.0rem; }', 'inline');

  $form['#submit'][] = 'contest_tnc_settings_form_submit';
  $form['#validate'][] = 'contest_tnc_settings_form_validate';

  $form['desc'] = array(
    '#type'   => 'markup',
    '#markup' => '<p>' . t('Configure the contest terms and conditions.') . '</p>',
    '#weight' => 0,
  );
  $form['contest_tnc_enable_sponsor'] = array(
    '#title'         => t('Enable Sponsor'),
    '#description'   => t('Enable the per contest sponsorship field. This allows individual contests to have different sponsors.'),
    '#type'          => 'radios',
    '#default_value' => variable_get('contest_tnc_enable_sponsor', 0),
    '#required'      => TRUE,
    '#options'       => array(0 => t('No'), 1 => t('Yes')),
  );
  $form['host'] = array(
    '#type'              => 'textfield',
    '#title'             => t('Host'),
    '#autocomplete_path' => 'contest/usr-autocomplete',
    '#default_value'     => ContestStorage::getUserName(variable_get('contest_tnc_host_uid', 1)),
    '#size'              => 4,
  );
  $form['contest_tnc_host_title'] = array(
    '#title'         => t('Host Title'),
    '#description'   => t("The host's display title."),
    '#type'          => 'textfield',
    '#default_value' => variable_get('contest_tnc_host_title', 'Contest Host'),
    '#size'          => 30,
    '#maxlength'     => 60,
    '#required'      => TRUE,
    '#weight'        => 8,
  );
  $form['tnc'] = array(
    '#title'       => t('Terms & Conditions'),
    '#description' => t("The contest's terms and conditions."),
    '#type'        => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,
    '#weight'      => 25,
  );
  $form['tnc']['contest_tnc_block_content'] = array(
    '#type'          => 'text_format',
    '#default_value' => ContestStorage::getBlockBody(variable_get('contest_tnc_block', 0)),
    '#format'        => ContestStorage::getBlockFormat(variable_get('contest_tnc_block', 0)),
    '#required'      => FALSE,
    'token_help'     => array(
      '#dialog'      => TRUE,
      '#theme'       => 'token_tree',
      '#token_types' => array('contest', 'node', 'user'),
      '#weight'      => 10,
    ),
  );
  return system_settings_form($form);
}

/**
 * Custom submit function for the contest setting form.
 */
function contest_tnc_settings_form_submit($form, &$state) {
  $uid = ContestStorage::getUserId(trim($state['values']['host']));

  if ($uid) {
    variable_set('contest_tnc_host_uid', $uid);
  }
  // Update the T&C content.
  $fields = array(
    'body'   => $state['values']['contest_tnc_block_content']['value'],
    'format' => $state['values']['contest_tnc_block_content']['format'],
  );
  ContestStorage::updateBlock(variable_get('contest_tnc_block', 0), $fields);

  unset($state['values']['host']);
  unset($state['values']['contest_tnc_block_content']);
}

/**
 * Custom validation function for the contest setting form.
 */
function contest_tnc_settings_form_validate($form, &$state) {

  // Non-existent contest host.
  $host = user_load_by_name($state['values']['host']);
  $host = new ContestUser($host->uid);

  if (empty($host->uid)) {
    form_set_error('host', t('No user with that name: @name.', array('@name' => $state['values']['host'])));
  }
  // Incomplete host profile.
  if (!$host->completeProfile('host')) {
    $opts = array('attributes' => array('target' => '_blank'));
    $args = array(
      '@name' => $state['values']['host'],
      '!link' => l(t('here'), "user/$host->uid/edit", $opts),
    );
    form_set_error('host', t('@name has an incomplete profile. If you have permission, you can click !link to edit.', $args));
  }
}
