<?php

// Make sure that the varbase.settings.inc is included.
$conf['varbase_extra_settings_file'] = TRUE;

// Disable Drupal's poormanscron (Automatic Cron).
$conf['cron_safe_threshold'] = 0;

/**
 * Memcache Settings.
 *
 * uncomment settings bellow to enable memcache settings, this is only a default
 * template check memecache module README.txt for more advance options.
 */
#$conf['cache_backends'][] = 'profiles/varbase/modules/performance/memcache/memcache.inc';
#$conf['lock_inc'] = 'profiles/varbase/modules/performance/memcache/memcache-lock.inc';

#$conf['cache_default_class'] = 'MemCacheDrupal';
#$conf['cache_class_cache_form'] = 'DrupalDatabaseCache';

// Aggressive cacheing settings.
#$conf['page_cache_without_database'] = TRUE;
#$conf['page_cache_invoke_hooks'] = FALSE;

// Check memcache module README for more information about servers and
// cache bins.
#$conf['memcache_servers'] = array (
#  '127.0.0.1:11211' => 'default',
#);

// If you are using the  memcache instance for multiple sites, you have to set a
// unique prefix for every site uses this instance.
#$conf['memcache_key_prefix'] = 'site_prefix';

/**
 * HTTPS connection settings.
 *
 */
// Disallow sharing cookies Between secure and non-secure sessions, if this is
// set to true you users will be able to use the same session for http and https
// conections.
#$conf['https'] = FALSE;

/**
 * General settings that will prove useful in any environment, in most cases
 * you don't have to edit any thing here.
 */
// Map requests IP address behind a proxy or load balancer IP to the real
// client request IP.
if (isset($_SERVER['HTTP_X_FORWARDED_FOR'])) {
  $_SERVER['REMOTE_ADDR'] = $_SERVER['HTTP_X_FORWARDED_FOR'];
}

// Add Acquia varnish reverse proxy headers if available.
if (!empty($_ENV['AH_SITE_ENVIRONMENT'])) {
  $conf['reverse_proxy_header'] = 'HTTP_X_AH_CLIENT_IP';
}

// Set the $base_url and $_SERVER['HTTPS'] behind a proxy server with the SSL
// Certificate on it.
if (isset($_SERVER['HTTP_X_FORWARDED_PROTO'])) {
  $base_root = $_SERVER['HTTP_X_FORWARDED_PROTO'];

  // Make sure to set $_SERVER variable correctly.
  if ($_SERVER['HTTP_X_FORWARDED_PROTO'] == 'https') {
    $_SERVER['HTTPS'] = 'on';
  }

  // Make sure that set $_SERVER variable correctly.
  if (!isset($base_url)) {
    $base_url = $base_root .= '://'. $_SERVER['HTTP_HOST'];
  } else {
    // You can add you $base_url settings here for HTTPS connections.
  }
}
