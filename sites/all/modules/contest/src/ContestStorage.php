<?php

/**
 * @file
 * Contains the ContestStorage class.
 */

/**
 * Storage class for contests.
 */
class ContestStorage {
  const ADDRESS_MAX = 100;
  const CITY_MAX = 50;
  const DAY = 86400;
  const INT_MAX = 2147483647;
  const NAME_MAX = 50;
  const PHONE_MAX = 20;
  const STRING_MAX = 255;
  const ZIP_MAX = 5;

  /**
   * Fetch a list of matching users.
   *
   * @param string $name
   *   The user's name.
   *
   * @return array
   *   An array of user names.
   */
  public static function autocomplete($name) {
    return db_query("SELECT name, name FROM {users} WHERE name LIKE :name", array(':name' => $name))->fetchAllKeyed();
  }

  /**
   * Clear the winner flag on a contest entry.
   *
   * @param int $nid
   *   The contest's ID.
   * @param int $uid
   *   The user's ID.
   *
   * @return Drupal\Core\Database\Query\Update
   *   A new Update object for this connection.
   */
  public static function clearWinner($nid, $uid) {
    return db_update('contest_entry')
      ->fields(array('winner' => 0))
      ->condition('nid', $nid)
      ->condition('uid', $uid)
      ->execute();
  }

  /**
   * Clear all winning flags for a contest.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return Drupal\Core\Database\Query\Update
   *   A new Update object for this connection.
   */
  public static function clearWinners($nid) {
    return db_update('contest_entry')
      ->fields(array('winner' => 0))
      ->condition('nid', $nid)
      ->execute();
  }

  /**
   * Determine if the contest exists.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return bool
   *   True if the contest exists.
   */
  public static function exists($nid) {
    return (bool) db_query("SELECT 1 FROM {contest} WHERE nid = :nid", array(':nid' => $nid))->fetchField();
  }

  /**
   * Flush the cache table.
   *
   * @param string $cid
   *   The contest's ID.
   *
   * @return Drupal\Core\Database\Query\DeleteQuery
   *   A new DeleteQuery object for this connection.
   */
  public static function flushCache($cid) {
    return db_delete('cache')->condition('cid', $cid, 'LIKE')->execute();
  }

  /**
   * Retrieve a list of unique user IDs entered into all contests.
   *
   * @return array
   *   An array of user IDs.
   */
  public static function getAllEntries() {
    $stmt = "
      SELECT
        DISTINCT(u.uid)
      FROM
        {users} u
        JOIN {contest_entry} e ON e.uid = u.uid
      WHERE
        u.status = 1
      ORDER BY
        u.mail ASC
    ";
    return db_query($stmt)->fetchCol();
  }

  /**
   * Retrieve the block body.
   *
   * @param int $bid
   *   The block ID.
   *
   * @return string
   *   The block body.
   */
  public static function getBlockBody($bid = 0) {
    return db_query("SELECT body FROM {block_custom} WHERE bid = :bid", array(':bid' => $bid))->fetchField();
  }

  /**
   * Retrieve the block body format.
   *
   * @param int $bid
   *   The block ID.
   *
   * @return string
   *   The block body format.
   */
  public static function getBlockFormat($bid = 0) {
    return db_query("SELECT format FROM {block_custom} WHERE bid = :bid", array(':bid' => $bid))->fetchField();
  }

  /**
   * Retrieve an array of contestant entry data for a contest.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return object
   *   An array of database objects with the following properties:
   *   - uid (int) The user's ID.
   *   - name (string) The user's name.
   *   - mail (string) The user's email address.
   *   - qty (int) The number of entries.
   *   - winner (int) A winning entry flag.
   */
  public static function getContestants($nid) {
    $stmt = "
      SELECT
        DISTINCT(u.uid) AS 'uid',
        u.name AS 'name',
        u.mail as 'mail',
        e.qty AS 'qty',
        e.winner AS 'winner'
      FROM
        (
          SELECT
            uid,
            COUNT(uid) AS 'qty',
            nid,
            MAX(winner) AS 'winner'
          FROM
            {contest_entry}
          GROUP BY
            uid,
            nid
          ORDER BY
            winner DESC
        ) e
        JOIN {users} u ON u.uid = e.uid
      WHERE
        e.nid = :nid
        AND u.status = 1
      ORDER BY
        e.qty DESC,
        u.name ASC
    ";
    return db_query($stmt, array(':nid' => $nid))->fetchAll();
  }

  /**
   * Retrieve a list of prior winners within the disqualification time.
   *
   * @param int $created
   *   The maximum creation date of a winning entry.
   *
   * @return array
   *   An array of objects with the uid property.
   */
  public static function getDqs($created) {
    $stmt = "
      SELECT
        uid
      FROM
        {contest_entry}
      WHERE
        created > :created
        AND winner
      ORDER BY
        winner ASC
    ";
    return db_query($stmt, array(':created' => $created))->fetchCol();
  }

  /**
   * Retrieve a uid for every entry into a contest.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return array
   *   An array of objects with the uid property, (with duplicates).
   */
  public static function getEntries($nid) {
    $stmt = "
      SELECT
        u.uid
      FROM
        {users} u
        JOIN {contest_entry} e ON e.uid = u.uid
      WHERE
        u.status = 1
        AND e.nid = :nid
      ORDER BY
        u.mail ASC
    ";
    return db_query($stmt, array(':nid' => $nid))->fetchCol();
  }

  /**
   * Retrieve the number of contest entries.
   *
   * @param int $nid
   *   The contest ID.
   *
   * @return int
   *   The number of contest entries.
   */
  public static function getEntryCount($nid) {
    return db_query_range("SELECT COUNT(uid) FROM {contest_entry} WHERE nid = :nid", 0, 1, array(':nid' => $nid))->fetchField();
  }

  /**
   * Fetch the user's email.
   *
   * @param int $uid
   *   The user ID.
   *
   * @return string
   *   The user's email.
   */
  public static function getMail($uid) {
    return db_query("SELECT mail FROM {users} WHERE uid = :uid", array(':uid' => $uid))->fetchField();
  }

  /**
   * Retrieve the highest winning place.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return int
   *   The highest winning place.
   */
  public static function getMaxPlace($nid) {
    return db_query_range("SELECT winner FROM {contest_entry} WHERE nid = :nid AND winner ORDER BY winner DESC", 0, 1, array(':nid' => $nid))->fetchField();
  }

  /**
   * Fetch a list of contest IDs.
   *
   * @param int $start
   *   The offset.
   * @param int $limit
   *   The max rows.
   *
   * @return array
   *   An array of contest IDs.
   */
  public static function getNids($start = NULL, $limit = 0) {
    $stmt = "
      SELECT
        c.nid
      FROM
        {contest} c
        JOIN {node} n ON n.nid = c.nid
      WHERE
        n.status = 1
      ORDER BY
        n.sticky DESC,
        c.end ASC,
        c.start DESC,
        n.title ASC
    ";
    if (is_numeric($start) && $limit) {
      return db_query_range($stmt, $start, $limit)->fetchCol();
    }
    return db_query($stmt)->fetchCol();
  }

  /**
   * Retrieve the period, (entry frequency) for the contest.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return int
   *   The days between an entry in seconds.
   */
  public static function getPeriod($nid) {
    $args = array(
      ':nid'   => $nid,
      ':end'   => REQUEST_TIME,
      ':start' => REQUEST_TIME,
    );
    return db_query_range("SELECT period FROM {contest} WHERE nid = :nid AND start < :start AND :end < end", 0, 1, $args)->fetchField();
  }

  /**
   * Create an array of contest entry options.
   *
   * @return array
   *   An array of entry period options.
   */
  public static function getPeriodFormats() {
    return array(
      self::DAY       => 'Ymd',
      self::DAY * 7   => 'YW',
      self::DAY * 30  => 'Ym',
      self::DAY * 365 => 'Y',
      self::INT_MAX   => t('Once'),
    );
  }

  /**
   * Create an array of contest entry options.
   *
   * @return array
   *   An array of entry period options.
   */
  public static function getPeriodOptions() {
    return array(
      self::DAY       => t('Daily'),
      self::DAY * 7   => t('Weekly'),
      self::DAY * 30  => t('Monthly'),
      self::DAY * 365 => t('Yearly'),
      self::INT_MAX   => t('Once'),
    );
  }

  /**
   * Retrieve the winners publishing status.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return bool
   *   True if the winners are published.
   */
  public static function getPublished($nid) {
    $args = array(
      ':end' => REQUEST_TIME,
      ':nid' => $nid,
    );
    return (bool) db_query("SELECT publish_winners FROM {contest} WHERE end <= :end AND nid = :nid", $args)->fetchField();
  }

  /**
   * Retrieve a random entry for a contest.
   *
   * @param int $nid
   *   The contest's ID.
   * @param array $wids
   *   An array of user IDs from recent winners.
   *
   * @return object
   *   An object with the uid and created properties.
   */
  public static function getRandomEntry($nid, array $wids = array()) {
    $stmt = "
      SELECT
        e.uid,
        e.created
      FROM
        {contest_entry} e
        JOIN {users} u ON u.uid = e.uid
      WHERE
        e.nid = :nid
        AND e.uid NOT IN (:wids)
        AND u.status = 1
      ORDER BY
        RAND()
    ";
    return db_query_range($stmt, 0, 1, array(':nid' => $nid, ':wids' => $wids))->fetchObject();
  }

  /**
   * Retrieve a random entry for a particular user entered into a contest.
   *
   * @param int $nid
   *   The contest's ID.
   * @param int $uid
   *   The user's ID.
   *
   * @return object
   *   An object with the uid and created properties.
   */
  public static function getRandomUserEntry($nid, $uid) {
    $stmt = "
      SELECT
        e.uid,
        e.created
      FROM
        {contest_entry} e
        JOIN {users} u ON u.uid = e.uid
      WHERE
        e.nid = :nid
        AND e.uid = :uid
        AND u.status = 1
      ORDER BY
        RAND()
    ";
    return db_query_range($stmt, 0, 1, array(':nid' => $nid, ':uid' => $uid))->fetchObject();
  }

  /**
   * Gets the states for the selected country.
   *
   * @param string $country
   *   The ISO country code.
   *
   * @return array
   *   An ISO code to country name hash.
   */
  public static function getStates($country = 'US') {
    $states = variable_get('contest_states', array());

    if (!empty($states[$country])) {
      return $states[$country];
    }
    switch ($country) {
      case 'CA':
        return array(
          'AB' => 'Alberta',
          'BC' => 'British Columbia',
          'MB' => 'Manitoba',
          'NB' => 'New Brunswick',
          'NL' => 'Newfoundland and Labrador',
          'NS' => 'Nova Scotia',
          'ON' => 'Ontario',
          'PE' => 'Prince Edward Island',
          'QC' => 'Quebec',
          'SK' => 'Saskatchewan',
          'NT' => 'Northwest Territories',
          'NU' => 'Nunavut',
          'YT' => 'Yukon',
        );

      case 'US':
        return array(
          'AL' => 'Alabama',
          'AK' => 'Alaska',
          'AZ' => 'Arizona',
          'AR' => 'Arkansas',
          'CA' => 'California',
          'CO' => 'Colorado',
          'CT' => 'Connecticut',
          'DE' => 'Delaware',
          'DC' => 'District Of Columbia',
          'FL' => 'Florida',
          'GA' => 'Georgia',
          'HI' => 'Hawaii',
          'ID' => 'Idaho',
          'IL' => 'Illinois',
          'IN' => 'Indiana',
          'IA' => 'Iowa',
          'KS' => 'Kansas',
          'KY' => 'Kentucky',
          'LA' => 'Louisiana',
          'ME' => 'Maine',
          'MD' => 'Maryland',
          'MA' => 'Massachusetts',
          'MI' => 'Michigan',
          'MN' => 'Minnesota',
          'MS' => 'Mississippi',
          'MO' => 'Missouri',
          'MT' => 'Montana',
          'NE' => 'Nebraska',
          'NV' => 'Nevada',
          'NH' => 'New Hampshire',
          'NJ' => 'New Jersey',
          'NM' => 'New Mexico',
          'NY' => 'New York',
          'NC' => 'North Carolina',
          'ND' => 'North Dakota',
          'OH' => 'Ohio',
          'OK' => 'Oklahoma',
          'OR' => 'Oregon',
          'PA' => 'Pennsylvania',
          'RI' => 'Rhode Island',
          'SC' => 'South Carolina',
          'SD' => 'South Dakota',
          'TN' => 'Tennessee',
          'TX' => 'Texas',
          'UT' => 'Utah',
          'VT' => 'Vermont',
          'VI' => 'Virgin Islands',
          'VA' => 'Virginia',
          'WA' => 'Washington',
          'WV' => 'West Virginia',
          'WI' => 'Wisconsin',
          'WY' => 'Wyoming',
        );
    }
    return array();
  }

  /**
   * Fetch a list of unfinished contest data.
   *
   * @return array
   *   An array of unfinished contest data.
   */
  public static function getUnFinished() {
    $stmt = "
      SELECT
        c.nid,
        n.title,
        c.start,
        c.end
      FROM
        {contest} c
        JOIN {node} n ON n.nid = c.nid
      WHERE
        c.end > UNIX_TIMESTAMP()
      ORDER BY
        n.title ASC
    ";
    return db_query($stmt)->fetchAll();
  }

  /**
   * Retrieve a list of unique user IDs entered into a contest.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return array
   *   An array of objects with the uid property, (no duplicates).
   */
  public static function getUniqueEntries($nid) {
    $stmt = "
      SELECT
        DISTINCT(u.uid)
      FROM
        {users} u
        JOIN {contest_entry} e ON e.uid = u.uid
      WHERE
        u.status = 1
        AND e.nid = :nid
      ORDER BY
        u.mail ASC
    ";
    return db_query($stmt, array(':nid' => $nid))->fetchCol();
  }

  /**
   * Fetch the user ID if it exists.
   *
   * @param int $name
   *   The user's name.
   *
   * @return int
   *   The user ID if it exists.
   */
  public static function getUserId($name = '') {
    return db_query("SELECT uid FROM {users} WHERE name = :name", array(':name' => $name))->fetchField();
  }

  /**
   * Fetch the user name if it exists.
   *
   * @param int $uid
   *   The user's ID.
   *
   * @return string
   *   The user name if it exists.
   */
  public static function getUserName($uid = 0) {
    return db_query("SELECT name FROM {users} WHERE uid = :uid", array(':uid' => $uid))->fetchField();
  }

  /**
   * Retrieve the number of winners that have been selected for a contest.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return int
   *   The number of winners for a particular contest.
   */
  public static function getWinnerCount($nid) {
    return db_query_range("SELECT COUNT(uid) FROM {contest_entry} WHERE nid = :nid AND winner", 0, 1, array(':nid' => $nid))->fetchField();
  }

  /**
   * Retrieve the contest winners in order.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return array
   *   An array of winning user IDs.
   */
  public static function getWinners($nid) {
    return db_query("SELECT uid FROM {contest_entry} WHERE nid = :nid AND winner ORDER BY winner ASC", array(':nid' => $nid))->fetchCol();
  }

  /**
   * Retrieve the date of the last entry for a user.
   *
   * @param array $fields
   *   An array of the contest fields.
   *
   * @return object
   *   A new InsertQuery object for this connection.
   */
  public static function insert(array $fields) {
    return db_insert('contest')->fields($fields)->execute();
  }

  /**
   * Retrieve the date of the last entry for a user.
   *
   * @param int $nid
   *   The contest's ID.
   * @param int $uid
   *   The user's ID.
   *
   * @return int
   *   The Unix timstamp of a user's last entry.
   */
  public static function latestUsrEntryDate($nid, $uid) {
    return db_query_range("SELECT created FROM {contest_entry} WHERE uid = :uid AND nid = :nid ORDER BY created DESC", 0, 1, array(':nid' => $nid, ':uid' => $uid))->fetchField();
  }

  /**
   * Fetch the contest and sponsor fields.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return object
   *   An object with the contest and sponsor fields.
   */
  public static function loadData($nid) {
    $stmt = "
      SELECT
        u.name AS 'sponsor',
        u.mail AS 'sponsor_email',
        c.sponsor_url,
        c.start,
        c.end,
        c.places,
        c.period,
        c.publish_winners
      FROM
        {contest} c
        LEFT JOIN {users} u ON u.uid =  c.sponsor_uid
      WHERE
        c.nid = :nid
    ";
    return db_query_range($stmt, 0, 1, array(':nid' => $nid))->fetchObject();
  }

  /**
   * Set the publish_winners flag on a contest to 1.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return Drupal\Core\Database\Query\Update
   *   A new Update object for this connection.
   */
  public static function publishWinners($nid) {
    return db_update('contest')->fields(array('publish_winners' => 1))->condition('nid', $nid)->execute();
  }

  /**
   * Determine if the contest is currently running.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return bool
   *   True if the contest is currently running.
   */
  public static function running($nid) {
    $args = array(
      ':nid'   => $nid,
      ':start' => REQUEST_TIME,
      ':end'   => REQUEST_TIME,
    );
    $stmt = "
      SELECT
        1
      FROM
        {contest} c
        JOIN {node} n ON n.nid = c.nid
      WHERE
        n.nid = :nid
        AND n.status = 1
        AND c.start < :start
        AND c.end > :end
    ";
    return (bool) db_query($stmt, $args)->fetchField();
  }

  /**
   * Insert an entry for a contest.
   *
   * @param array $fields
   *   An array of entry fields
   *   - nid (int) The contest's ID.
   *   - uid (int) The user's ID
   *   - created (int) The current time.
   *   - ip (string) The IP the entry was submitted by.
   *
   * @return Drupal\Core\Database\Query\InsertQuery
   *   An InsertQuery object for this connection.
   */
  public static function saveEntry(array $fields) {
    if (empty($fields['nid']) || empty($fields['uid'])) {
      return FALSE;
    }
    $fields['created'] = !empty($fields['created']) ? $fields['created'] : REQUEST_TIME;
    $fields['ip'] = !empty($fields['ip']) ? $fields['ip'] : ip_address();

    return db_insert('contest_entry')->fields($fields)->execute();
  }

  /**
   * Set the place in the winner field.
   *
   * @param int $nid
   *   The contest's ID.
   * @param int $uid
   *   The user's ID.
   * @param int $place
   *   Winner's place (1, 2, 3...) Sequential -w- gaps, (1st, 3rd, 6th, etc.).
   * @param int $created
   *   The timestamp of the winning entry.
   *
   * @return Drupal\Core\Database\Query\Update
   *   A new Update object for this connection.
   */
  public static function setWinner($nid, $uid, $place, $created) {
    return db_update('contest_entry')
      ->fields(array('winner' => $place))
      ->condition('nid', $nid)
      ->condition('uid', $uid)
      ->condition('created', $created)
      ->execute();
  }

  /**
   * Set the publish_winners flag on a contest to 0.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return \Drupal\Core\Database\Query\Update
   *   A new Update object for this connection.
   */
  public static function unpublishWinners($nid) {
    return db_update('contest')->fields(array('publish_winners' => 0))->condition('nid', $nid)->execute();
  }

  /**
   * Update the contest.
   *
   * @param int $vid
   *   The contest's version ID.
   * @param array $fields
   *   The contest's fields.
   *
   * @return \Drupal\Core\Database\Query\Update
   *   A new Update object for this connection.
   */
  public static function update($vid, array $fields) {
    return db_update('contest')->fields($fields)->condition('vid', $vid)->execute();
  }

  /**
   * Update the contest.
   *
   * @param int $bid
   *   The block ID.
   * @param array $fields
   *   The block fields.
   *
   * @return \Drupal\Core\Database\Query\Update
   *   A new Update object for this connection.
   */
  public static function updateBlock($bid, array $fields) {
    return db_update('block_custom')->fields($fields)->condition('bid', $bid)->execute();
  }

  /**
   * Determine if the user has an entry in a contest.
   *
   * @param int $nid
   *   The contest's ID.
   * @param int $uid
   *   The user's ID.
   *
   * @return bool
   *   True of the user has an entry in the contest.
   */
  public static function usrEnteredOnce($nid, $uid) {
    return (bool) db_query_range("SELECT 1 FROM {contest_entry} WHERE uid = :uid AND nid = :nid", 0, 1, array(':nid' => $nid, ':uid' => $uid))->fetchField();
  }

  /**
   * Determine if the user name exists.
   *
   * @param string $name
   *   A prospective user name.
   *
   * @return bool
   *   True of the user name exists.
   */
  public static function usrNameExists($name = '') {
    return (bool) db_query("SELECT 1 FROM {users} WHERE name = :name", array(':name' => $name))->fetchField();
  }

  /**
   * Fetch data to deciding available actions.
   *
   * @param int $nid
   *   The contest's ID.
   *
   * @return object
   *   Contest data including:
   *   - end (int) A Unix timestamp of the end of the contest.
   *   - places (int) The number of winning places in a contest.
   *   - publish_winners (int) A flag to indicate if the winners are published.
   */
  public static function winnerPreSelect($nid) {
    return db_query("SELECT end, places, publish_winners FROM {contest} WHERE nid = :nid", array(':nid' => $nid))->fetchObject();
  }

}
