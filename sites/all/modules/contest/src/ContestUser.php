<?php

/**
 * @file
 * Contains the ContestUser class.
 */

/**
 * An easy to access contest user.
 */
class ContestUser {
  public $uid;
  public $name;
  public $mail;
  public $status;
  public $fullName;
  public $business;
  public $address;
  public $city;
  public $state;
  public $zip;
  public $phone;
  public $birthdate;
  public $optin;
  public $title;
  public $url;
  protected $fields = array(
    'name'      => 'fullName',
    'business'  => 'business',
    'address'   => 'address',
    'city'      => 'city',
    'state'     => 'state',
    'zip'       => 'zip',
    'birthdate' => 'birthdate',
    'phone'     => 'phone',
    'optin'     => 'optin',
  );

  /**
   * Just make sure there are some defaults that won't throw an error.
   *
   * @param int $uid
   *   The user's ID.
   * @param array $xtra
   *   Any optional fields.
   */
  public function __construct($uid, array $xtra = array()) {
    $usr = !empty($uid) ? user_load((int) $uid) : NULL;

    $this->uid = !empty($usr->uid) ? $usr->uid : 0;
    $this->name = !empty($usr->name) ? $usr->name : '';
    $this->mail = !empty($usr->mail) ? $usr->mail : '';
    $this->status = !empty($usr->status) ? $usr->status : 0;

    foreach (array_keys((array) $usr) as $field) {
      if ($field == 'field_contest_name') {
        $this->fullName = $this->get($usr, $field);
        continue;
      }
      if (!preg_match('/^field_contest_([a-z]+)/', $field, $m) || !property_exists($this, $m[1])) {
        continue;
      }
      $this->{$m[1]} = $this->get($usr, $field);
    }
    $this->title = !empty($xtra['title']) ? $xtra['title'] : '';
    $this->url = !empty($xtra['url']) ? $xtra['url'] : '';
  }

  /**
   * Magic get.
   */
  public function __get($property) {
    $property = ($property == 'full_name') ? 'fullName' : $property;

    return property_exists($this, $property) ? $this->{$property} : NULL;
  }

  /**
   * Magic set.
   */
  public function __set($property, $value) {
    $property = ($property == 'full_name') ? 'fullName' : $property;

    return property_exists($this, $property) ? $this->{$property} = $value : NULL;
  }

  /**
   * Determine if the profile is complete.
   *
   * @param string $role
   *   The role to test.
   *
   * @return bool
   *   True if the user has a completed profile.
   */
  public function completeProfile($role = 'entrant') {
    $status = (bool) ($this->fullName && $this->address && $this->city && $this->state && $this->zip && $this->phone);

    switch ($role) {
      case 'entrant':
        return $status;

      case 'host':
        return (bool) ($status && $this->business);
    }
    return FALSE;
  }

  /**
   * Save the fields to the Drupal user object.
   */
  public function save() {
    $usr = $this->uid ? user_load($this->uid) : user_load(0);

    foreach ($this->fields as $field => $property) {
      $this->set($usr, $field, $this->{$property});
    }
    user_save($usr);
  }

  /**
   * Extract the contest profile value from the user object.
   *
   * @param object $usr
   *   A Drupal user object.
   * @param string $field
   *   The field name.
   * @param int $delta
   *   The fields index.
   * @param string $default
   *   The default value.
   *
   * @return string
   *   The safe_value if set.
   */
  protected function get($usr = NULL, $field = '', $delta = 0, $default = '') {
    if (!is_object($usr) || empty($field)) {
      return NULL;
    }
    if (isset($usr->{$field}[LANGUAGE_NONE][$delta]['safe_value'])) {
      return $usr->{$field}[LANGUAGE_NONE][$delta]['safe_value'];
    }
    elseif (isset($usr->{$field}[LANGUAGE_NONE][$delta]['value'])) {
      return $usr->{$field}[LANGUAGE_NONE][$delta]['value'];
    }
    return $default;
  }

  /**
   * Set a plain text Drupal user field.
   *
   * @param object $usr
   *   A user object.
   * @param string $field
   *   The field name.
   * @param int|string $value
   *   The field's value.
   * @param int $index
   *   The index of the field.
   */
  protected function set($usr, $field = '', $value = '', $index = 0) {
    $field = "field_contest_{$field}";

    $usr->{$field}[LANGUAGE_NONE][$index] = array(
      'value'      => $value,
      'formt'      => 'plain_text',
      'safe_value' => check_plain($value),
    );
  }

}
