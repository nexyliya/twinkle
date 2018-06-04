<?php

/**
 * @file
 * Admin page callbacks for the contest module.
 */

/**
 * Menu callback to export all contestants.
 */
function contest_export_form() {
  $opts['attributes']['class'] = array('button', 'form-submit');

  $form['desc'] = array(
    '#type'   => 'markup',
    '#markup' => '<p>' . t('Export all contestants from all contests to a CSV.') . '</p>',
    '#weight' => 0,
  );
  $form['export'] = array(
    '#type'   => 'markup',
    '#markup' => '<div>' . l(t('Export All Contestants'), 'contest/export-all', $opts) . '</div>',
    '#weight' => 40,
  );
  return $form;
}

/**
 * Menu callback to configure module settings.
 */
function contest_settings_form() {
  $nintey = array(ContestStorage::DAY * 90 => t('90 Days'));

  $form['desc'] = array(
    '#type'   => 'markup',
    '#markup' => '<p>' . t('Configuration options for the contest module.') . '</p>',
    '#weight' => 0,
  );
  $form['contest_dq_days'] = array(
    '#title'         => t('Days Between Wins'),
    '#type'          => 'select',
    '#options'       => ContestHelper::optionInsert(ContestStorage::getPeriodOptions(), $nintey),
    '#default_value' => variable_get('contest_dq_days', 90),
    '#required'      => TRUE,
    '#weight'        => 10,
  );
  $form['contest_min_age'] = array(
    '#title'         => t('Minumum Age'),
    '#description'   => t("The minimum age required to enter a contest."),
    '#type'          => 'select',
    '#options'       => array_combine(range(1, 100), range(1, 100)),
    '#default_value' => variable_get('contest_min_age', 18),
    '#required'      => TRUE,
    '#weight'        => 30,
  );
  return system_settings_form($form);
}
