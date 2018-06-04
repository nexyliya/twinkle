<?php

/**
 * @file
 * Builds placeholder replacement tokens for values specific to Contest nodes.
 */

/**
 * Implements hook_token_info().
 */
function contest_tnc_token_info() {
  $contest['country'] = array(
    'name'        => t("Contest Country"),
    'description' => t("The country where all contests are held."),
  );
  $contest['date-end'] = array(
    'name'        => t("Contest End"),
    'description' => t("This contest's end date."),
  );
  $contest['date-notify'] = array(
    'name'        => t("Contest Notification Date"),
    'description' => t("This contest's notification date."),
  );
  $contest['date-start'] = array(
    'name'        => t("Contest Start"),
    'description' => t("This contest's start date."),
  );
  $contest['host-address'] = array(
    'name'        => t("Contest Host Address"),
    'description' => t("The address of the contest host."),
  );
  $contest['host-business'] = array(
    'name'        => t("Contest Host Business"),
    'description' => t("The host's business name."),
  );
  $contest['host-city'] = array(
    'name'        => t("Contest Host City"),
    'description' => t("The host's city."),
  );
  $contest['host-full-name'] = array(
    'name'        => t("Contest Host Name"),
    'description' => t("The host's full name."),
  );
  $contest['host-state'] = array(
    'name'        => t("Contest Host State"),
    'description' => t("The state of the contest host."),
  );
  $contest['host-link'] = array(
    'name'        => t("Contest Host Link"),
    'description' => t("A link to this website."),
  );
  $contest['host-title'] = array(
    'name'        => t("Contest Host Title"),
    'description' => t("The contest host title."),
  );
  $contest['host-zip'] = array(
    'name'        => t("Contest Host Zipcode"),
    'description' => t("The zipcode of the contest host."),
  );
  $contest['places'] = array(
    'name'        => t("Contest Places"),
    'description' => t("The number of Places in this contest."),
  );
  $contest['server-link'] = array(
    'name'        => t("Server Link"),
    'description' => t("A link to this website."),
  );
  $contest['sponsor'] = array(
    'name'        => t("Contest Sponsor"),
    'description' => t("This contest's sponsor."),
  );
  $contest['timezone'] = array(
    'name'        => t("Contest Time Zone"),
    'description' => t("The time zone where the contest is being held."),
  );
  $info = array(
    'tokens' => array('contest' => $contest),
    'types'  => array(
      'contest' => array(
        'name'        => t('Contest'),
        'description' => t('Tokens related to a contest.'),
        'needs-data'  => 'contest',
      ),
    ),
  );
  return $info;
}

/**
 * Implements hook_tokens().
 */
function contest_tnc_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $host = new ContestUser(variable_get('contest_tnc_host_uid', 1));
  $x = array();

  if (empty($data['node']->type) || $data['node']->type != 'contest') {
    return array();
  }
  $contest = $data['node']->contest;

  foreach ($tokens as $key => $token) {
    switch ($key) {
      case 'country':
        $x[$token] = variable_get('site_default_country', '');
        break;

      case 'date-end':
        $x[$token] = format_date($contest->end, 'contest_date');
        break;

      case 'date-notify':
        $x[$token] = format_date(($contest->end + (60 * 60 * 24 * 31)), 'contest_date');
        break;

      case 'date-start':
        $x[$token] = format_date($contest->start, 'contest_date');
        break;

      case 'host-address':
        $x[$token] = check_plain($host->address);
        break;

      case 'host-business':
        $x[$token] = check_plain($host->business);
        break;

      case 'host-city':
        $x[$token] = check_plain($host->city);
        break;

      case 'host-full-name':
        $x[$token] = check_plain($host->full_name);
        break;

      case 'host-link':
        $x[$token] = l($_SERVER['HTTP_HOST'], base_path(), array('absolute' => TRUE));
        break;

      case 'host-state':
        $x[$token] = check_plain($host->state);
        break;

      case 'host-title':
        $x[$token] = check_plain(variable_get('contest_tnc_host_title', 'Contest Host'));
        break;

      case 'host-zip':
        $x[$token] = check_plain($host->zip);
        break;

      case 'places':
        $x[$token] = intval($contest->places);
        break;

      case 'server-link':
        $x[$token] = l($_SERVER['SERVER_NAME'], base_path(), array('absolute' => TRUE));
        break;

      case 'sponsor':
        $x[$token] = !empty($contest->sponsor_url) ? l($contest->sponsor, $contest->sponsor_url, array('absolute' => TRUE)) : check_plain($contest->sponsor);
        break;

      case 'timezone':
        $x[$token] = date('T');
        break;
    }
  }
  return $x;
}
