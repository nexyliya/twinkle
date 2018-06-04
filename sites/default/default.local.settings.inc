<?php

/**
 * Development PHP settings:
 *
 * Force showing PHP errors and certain settings for local development
 * environment.
 */
error_reporting(E_ALL);
ini_set('display_errors', '1');
ini_set('display_startup_errors', '1');

/**
 * Development database settings:
 *
 * Edits the database connection to match what you use fro your local
 * development machine, don't edit settings in settings.php ever.
 */
$databases = array (
  'default' =>
  array (
    'default' =>
    array (
      'database' => 'DATABASE_NAME',
      'username' => 'USERNAME',
      'password' => 'PASSWORD',
      'host' => 'localhost',
      'port' => '',
      'driver' => 'mysql',
      'prefix' => '',
    ),
  ),
);

/**
 * Drupal core development settings:
 *
 * Force certain settings for local development environment.
 */
$cookie_domain = '.' + $_SERVER['HTTP_HOST'];
$base_url = $_SERVER['REQUEST_SCHEME'] . '://' . $_SERVER['HTTP_HOST'];
$conf['error_level'] = 2;
$conf['file_temporary_path'] = '/tmp';
$conf['cache'] = 0;
$conf['block_cache'] = 0;
$conf['preprocess_css'] = 0;
$conf['preprocess_js'] = 0;
$conf['cache_lifetime'] = 0;
$conf['page_cache_maximum_age'] = 0;
$conf['page_compression'] = 0;

/**
 * Optional Drupal core development settings:
 *
 * Force certain settings for local development environment.
 */
#$conf['theme_debug'] = TRUE;
#$conf['mail_system'] = array('default-system' => 'DevelMailLog');

/**
 * Memcache:
 *
 * Turn off memcache on local environment by default, it is prefarable that you
 * have memecache even on local development environment, if so comment out
 * these lines.
 */
$conf['cache_default_class'] = 'DrupalDatabaseCache';
unset($conf['memcache_servers']);
unset($conf['page_cache_invoke_hooks']);
unset($conf['page_cache_without_database']);
#$conf['memcache_servers'] = array (
#  '127.0.0.1:11211' => 'default',
#);

/**
 * Development Search API:
 *
 * Override Search API settings to prevent staging and development servers from
 * writing to the production index and stop timing out the local enviroment.
 */
$conf['search_api_override_mode'] = 'load';
$conf['search_api_override_servers'] = array(
  'default' => array(
    'name' => 'Default local',
    'options' => array(
      'host' => '',
      'port' => '',
      'path' => '',
      'http_user' => '',
      'http_pass' => '',
      'excerpt' => 0,
      'retrieve_data' => 0,
      'highlight_data' => 0,
      'http_method' => 'POST',
    ),
  ),
);

/**
 * MISC modules configuration:
 *
 * - Disable Secure Pages and Secure Site module functionality.
 */
$conf['environment_indicator_enabled'] = TRUE;
$conf['environment_indicator_text'] = 'LOCAL DEVELOPMENT';
$conf['securepages_enable'] = 0;
$conf['securesite_enabled'] = 0;
$conf['image_toolkit'] = 'gd';

# Disable traking services on local or other development enviroment.
$conf['googleanalytics_account'] = 'UA-XXXXXXXX-X';
$conf['chartbeat_api_key'] = '';
