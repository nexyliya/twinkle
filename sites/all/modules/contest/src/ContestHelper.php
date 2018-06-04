<?php

/**
 * @file
 * Contains the ContestHelper class.
 */

/**
 * Helper class for contests.
 */
class ContestHelper {

  /**
   * Find or add a contestant then return the uid.
   *
   * @param array $state
   *   The submitted contest entry form.
   *
   * @return int
   *   The entrant's user ID.
   */
  public static function contestant(array $state) {
    global $base_url;
    global $user;
    $edit = array();
    $from = variable_get('site_mail', 'webmaster@' . preg_replace('/(\w+\.[a-z]+)$/', "$1", $base_url));

    // Try to get the user by uid or email.
    $usr = user_load($state['values']['uid']);

    if (empty($usr->uid)) {
      $usr = user_load_by_mail($state['values']['mail']);
    }
    // If we have them and the match the global user, update their information.
    if (!empty($usr->uid) && $usr->uid === $user->uid) {
      $usr = self::contestantFormSave($usr, $state);
      return $usr->uid;
    }
    // If we have a valid user return their ID.
    elseif (!empty($usr->uid)) {
      return $usr->uid;
    }
    // If they don't exist, create them.
    $args = array(
      'name'     => self::generateName($state['values']['contest_name']),
      'mail'     => $state['values']['mail'],
      'pass'     => self::generatePassword($state['values']['mail']),
      'roles'    => array(2 => 'authenticated user'),
      'status'   => 1,
      'timezone' => variable_get('date_default_timezone', 'America/New_York'),
    );
    // Save the new user.
    $usr = self::contestantFormSave(user_save($usr, $args), $state);

    // Send out the welcome email and set the welcome message.
    drupal_mail('contest', 'register_no_approval_required', $usr->mail, user_preferred_language($usr), array('user' => $usr), $from, TRUE);

    $tokens = array(
      '@site_name' => variable_get('site_name', 'Contest Host'),
      '@username'  => $usr->name,
    );
    drupal_set_message(t("You have been added to the @site_name website. Below is your login information.<br>Your username is: @username<br>A one-time login link has been emailed to you. To complete<br>If you have a problem logging in, use the password recovery tool located at the top of the user's login page.", $tokens));

    // Log them in.
    user_authenticate($usr->name, trim($args['pass']));
    user_module_invoke('login', $edit, $usr);

    // Return the user's id.
    return $usr->uid;
  }

  /**
   * Save the contest profile fields to the user object.
   *
   * @param object $usr
   *   A Drupal user object.
   * @param array $state
   *   A Drupal form state array.
   *
   * @return object
   *   A fully loaded user object.
   */
  public static function contestantFormSave($usr = NULL, array $state = array()) {
    if (empty($usr->uid)) {
      return new ContestUser(0);
    }
    $usr = new ContestUser($usr->uid);

    if (empty($state)) {
      return $usr;
    }
    $usr->fullName = $state['values']['contest_name'];
    $usr->address = $state['values']['contest_address'];
    $usr->city = $state['values']['contest_city'];
    $usr->state = $state['values']['contest_state'];
    $usr->zip = $state['values']['contest_zip'];
    $usr->phone = $state['values']['contest_phone'];
    $usr->birthdate = strtotime($state['values']['contest_birthdate']);
    $usr->optin = $state['values']['contest_optin'];

    $usr->save();

    return $usr;
  }

  /**
   * The CSV header.
   *
   * @return string
   *   The CSV header string.
   */
  public static function csvHeader() {
    return '"email","name","address","city","state","zip","phone"' . "\n";
  }

  /**
   * Return the data to the browser as a file.
   *
   * @param string $file
   *   The CSV file name.
   * @param string $csv
   *   The CSV string.
   * @param string $type
   *   The content type.
   */
  public static function downloadFile($file = 'contest.csv', $csv = '', $type = 'text/csv') {
    header('Cache-Control: must-revalidate, post-check=0, pre-check=0');
    header('Cache-Control: private');
    header('Content-Description: File Transfer');
    header("Content-Disposition: attachment; filename=\"$file\"");
    header('Content-Length: ' . strlen($csv));
    header('Content-Transfer-Encoding: binary');
    header("Content-Type: $type");
    header('Expires: 0');
    header('Pragma: no-cache');
    header('Pragma: private');

    $fh = fopen('php://output', 'w');
    fwrite($fh, $csv);
    fclose($fh);

    drupal_exit();
  }

  /**
   * Generate a unique username.
   *
   * @param string $name
   *   The user's name.
   *
   * @return string
   *   A unique username.
   */
  public static function generateName($name) {
    $min = 10;
    $max = 99;
    $username = $name;

    for ($i = $min; $i <= $max; $i++) {
      if (!ContestStorage::usrNameExists($username)) {
        return $username;
      }
      $username = preg_replace('/\W/', '', strtolower($name)) . '-' . rand($min, $max);
    }
    return self::generateName($username);
  }

  /**
   * Generate a password from an email address.
   *
   * @param string $email
   *   The user's email address.
   *
   * @return string
   *   A password that hopefully isn't too terrible.
   */
  public static function generatePassword($email = '') {
    if ($email) {
      return substr(preg_replace('/@.*/', '', $email), 0, rand(6, 8)) . '-' . substr(md5(REQUEST_TIME . rand(0, 100)), 0, rand(8, 12));
    }
    return substr(md5(REQUEST_TIME . rand(0, 1000)), 0, rand(8, 12));
  }

  /**
   * Return true if entered in the contest during this period, (configuarble).
   *
   * @param int $uid
   *   The node ID.
   * @param int $nid
   *   The user's ID.
   * @param int $period
   *   The seconds allowed between entries.
   *
   * @return bool
   *   True if the user has entered the contest already during this period.
   */
  public static function getEntered($uid, $nid, $period) {
    $fmt = ContestStorage::getPeriodFormats();
    $periods = ContestStorage::getPeriodOptions();

    // If it's a one entry contest check for an entry and return.
    if ($periods[$period] == t('Once')) {
      return ContestStorage::usrEnteredOnce($nid, $uid);
    }
    // If we can't figure out the format return TRUE.
    if (empty($fmt[$period])) {
      return TRUE;
    }
    // Determine if the user has already enter the conetest.
    $today = date($fmt[$period], REQUEST_TIME);
    $entered = date($fmt[$period], ContestStorage::latestUsrEntryDate($nid, $uid));

    return ($entered == $today) ? TRUE : FALSE;
  }

  /**
   * Build a set of form options.
   *
   * @return array
   *   An ordered array of form options.
   */
  public static function getFormOptions() {
    $options = array(0 => t('Select Contest'));

    foreach (ContestStorage::getUnFinished() as $obj) {
      $options[$obj->nid] = trim($obj->title) . ' - From: ' . date('Y-m-d', $obj->start) . ' To: ' . date('Y-m-d', $obj->end);
    }
    return $options;
  }

  /**
   * Try to get the site's email address in several ways.
   *
   * @return string
   *   The site's email address.
   */
  public static function getSiteMail() {
    global $base_url;
    $email = variable_get('site_mail', ini_get('sendmail_from'));

    if (empty($email)) {
      $email = ContestStorage::getMail(1);
    }
    return $email ? $email : 'webmaster@' . preg_replace('/^https?:\/\//', '', strtolower($base_url));
  }

  /**
   * Laod or add a user and return their ID.
   *
   * @param string $name
   *   The user name.
   * @param string $email
   *   The user email.
   *
   * @return int
   *   The user's ID.
   */
  public static function getUid($name = '', $email = '') {
    $email = strtolower(trim($email));

    if (!$email) {
      return 0;
    }
    $usr = user_load_by_mail($email);

    if (!empty($usr->uid)) {
      return $usr->uid;
    }
    if (!$name) {
      return 0;
    }
    $args = array(
      'mail'   => $email,
      'name'   => self::generateName($name),
      'pass'   => self::generatePassword($email),
      'roles'  => array(2 => 'authenticated user'),
      'status' => 1,
    );
    $usr = user_save($usr, $args);

    return !empty($usr->uid) ? $usr->uid : 0;
  }

  /**
   * Retrieve the contest winners in order.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return array
   *   An ordered array of user IDs to winning place.
   */
  public static function getWinners($nid) {
    $winners = ContestStorage::getWinners($nid);

    return count($winners) ? array_combine(range(1, count($winners)), array_values($winners)) : array();
  }

  /**
   * Determine if the minimum age requirement has been met.
   *
   * @param int|array $age
   *   The birthdate in either UNIX time or an array with year, month, day.
   *
   * @return bool
   *   True if the minimum age requirement is met, otherwise false.
   */
  public static function minAge($age = NULL) {
    $min_date = mktime(0, 0, 0, intval(date('n')), intval(date('j')), (intval(date('Y')) - variable_get('contest_min_age', 18)));

    if (is_int($age)) {
      $birthday = $age;
    }
    elseif (isset($age['day']) && isset($age['month']) && isset($age['year'])) {
      $birthday = mktime(0, 0, 0, intval($age['month']), intval($age['day']), intval($age['year']));
    }
    elseif (is_string($age) && strtotime($age) !== FALSE) {
      $birthday = strtotime($age);
    }
    else {
      return FALSE;
    }
    return ($birthday <= $min_date) ? TRUE : FALSE;
  }

  /**
   * Insert the options in the correct position.
   *
   * @param array $options
   *   The initial set of options.
   * @param array $xtras
   *   Extra options to add.
   *
   * @return bool
   *   True if the minimum age requirement is met, otherwise false.
   */
  public static function optionInsert(array $options = array(), array $xtras = array()) {
    $options += $xtras;
    ksort($options);

    return $options;
  }

  /**
   * Scan theme and module directories for the templates.
   *
   * @param string $module
   *   The module name, (without the suffix).
   * @param string $template
   *   The template name, (without the suffixes).
   *
   * @return string
   *   Drupal path to the template's directory, (default .../module/templates).
   */
  public static function templatePath($module, $template = '') {
    $theme_path = drupal_get_path('theme', variable_get('theme_default', 'garland'));

    $scan = file_scan_directory($theme_path, "/^$template\.tpl\.php$/", array('recurse' => TRUE, 'filename' => TRUE));

    if (!count($scan)) {
      $scan = file_scan_directory(drupal_get_path('module', $module), "/^$template\.tpl\.php$/", array('recurse' => TRUE, 'filename' => TRUE));
    }
    return count($scan) ? preg_replace('/(\/[^\/]+)$/', '', key($scan)) : drupal_get_path('module', $module) . '/templates';
  }

  /**
   * Return a single csv line for a contest.
   *
   * @param object $usr
   *   A ContestUser object.
   *
   * @return string
   *   A comma separated list of users.
   */
  public static function toCsv($usr) {
    $csv = '"' . preg_replace('/"/', '\"', $usr->mail) . '"';
    $csv .= ',"' . preg_replace('/"/', '\"', $usr->fullName) . '"';
    $csv .= ',"' . preg_replace('/"/', '\"', $usr->address) . '"';
    $csv .= ',"' . preg_replace('/"/', '\"', $usr->city) . '"';
    $csv .= ',"' . preg_replace('/"/', '\"', $usr->state) . '"';
    $csv .= ',"' . preg_replace('/"/', '\"', $usr->zip) . '"';
    $csv .= ',"' . preg_replace('/"/', '\"', $usr->phone) . '"';

    return "$csv\n";
  }

  /**
   * Create a summary from the provided text.
   *
   * @param string $string
   *   The string to trim.
   * @param int $max
   *   The target length of the string.
   *
   * @return string
   *   The provided text truncated to the requested length.
   */
  public static function trimTxt($string, $max = 150) {
    $txt = '';

    foreach (preg_split('/\s+/', $string) as $atom) {
      $length = strlen($atom);
      if (($length + strlen($txt) + 1) > $max) {
        return preg_match('/<\/p>$/', $txt) ? preg_replace('/<\/p>$/', '&hellip;</p>', $txt) : "$txt&hellip;</p>";
      }
      $txt .= $txt ? " $atom" : $atom;
    }
    return $txt;
  }

}
