<?php

/**
 * @file
 * Contains the ContestCache class.
 */

/**
 * Cache contest data.
 */
class ContestCache {
  const CID = 'contest-';
  const MAX = 247;

  /**
   * Flush the cache table.
   *
   * @return Drupal\Core\Database\Query\DeleteQuery
   *   A new DeleteQuery object for this connection.
   */
  public static function flushCache() {
    return ContestStorage::flushCache(self::CID . '%');
  }

  /**
   * Build and return a cache ID and resoectuve cached data.
   *
   * @param ...
   *   Data used to build a unique cache ID.
   *
   * @return array
   *   A two element ordered array of: cache ID, cached data.
   */
  public static function get() {
    $cid = FALSE;

    if (!user_access('administer contest')) {
      $cid = self::cid(func_get_args());

      $cache = $cid ? cache_get($cid) : FALSE;

      if ($cache !== FALSE) {
        return array($cid, $cache->data);
      }
    }
    return array($cid, FALSE);
  }

  /**
   * Set the cache and return the data.
   *
   * @param string $cid
   *   The cache ID.
   * @param mixed $data
   *   The cached data.
   *
   * @return mixed
   *   The submitted, (and cached) data.
   */
  public static function set($cid, $data) {
    if (!user_access('administer contest') && !empty($cid)) {
      cache_set($cid, $data, 'cache', CACHE_TEMPORARY);
    }
    return $data;
  }

  /**
   * Generate a cache ID.
   *
   * @param string|array $args
   *   The string(s) to build the cache ID from.
   *
   * @return string
   *   A unique cache ID.
   */
  protected static function cid($args = '') {
    $cid = self::cider($args);

    if ($cid && strlen($cid) <= self::MAX) {
      return self::CID . $cid;
    }
    elseif ($cid) {
      return self::CID . md5($cid);
    }
    return '';
  }

  /**
   * Generate a cache ID.
   *
   * @param string|array $args
   *   The string(s) to build the cache ID from.
   *
   * @return string
   *   A unique cache ID.
   */
  protected static function cider($args = '') {
    $args = (array) $args;
    $cid = '';

    foreach ($args as $arg) {
      $cid .= (is_array($arg) || is_object($arg)) ? '-' . self::cider($arg) : "-$arg";
    }
    return self::format($cid);
  }

  /**
   * Convert the provided string to a lowercase stroke delimited string.
   *
   * @param string $txt
   *   The string to convert.
   *
   * @return string
   *   A lowercase stroke delimited string.
   */
  protected static function format($txt) {
    return preg_replace(array('/[^a-z0-9]+/', '/^-+|-+$/'), array('-', ''), strtolower($txt));
  }

}
