<?php

/**
 * Shared functions
 */
 
 /**
  * Converts a underscore-delimited string to a human-readable name
	* e.g. meeting_room  becomes Meeting Room
	*/
function convert_to_readable_name($string) {
	$ret_str = strtoupper(substr($string, 0, 1)) . substr($string, 1); // Capitalize first letter
	if (strpos($ret_str, '_') == FALSE) 
		return $ret_str;
		
	$ret_str = str_replace('_', ' ', $ret_str);
	for ($i = 0; $i < strlen($ret_str); $i++) {
		if ($ret_str[$i] == ' ' && ($i+1) < strlen($ret_str))
			$ret_str[++$i] = strtoupper($ret_str[$i]);
	}
	return t($ret_str);
}	

function get_field_table($field_name) 	{ return 'field_data_' . $field_name; }
function get_field_column($field_name)	{ return $field_name . '_value'; }

/**
 * assoc-array string (integer) -> integer
 * Counts all the occurences in the keys of the assoc array that start with term
 */
function array_count_key_start_with($array, $term, $start_index = 0) {
	$sum = 0;
	if (empty($array) || trim($term, ' ') == '') return $sum;
	
	$keys = array_keys($array);
	for ($i = $start_index; $i < sizeof($keys); $i++) {
		if (substr($keys[$i], 0, strlen($term)) == $term) {
			$sum += 1;
		}
	}
	return $sum;
}

function vget_checked_entity_types($nth_setting) {
	return vget_checked_names('is_available_entity_type_' . $nth_setting . '_');
}
function vget_checked_room_types($nth_setting, $et) {
	return vget_checked_names('is_available_resource_type_' . $nth_setting . '_' . $et . '_');
}

function vget_checked_auth_buildings($nth_setting) {
	return vget_checked_names('is_available_auth_building_' . $nth_setting . '_');
}

function vget_checked_content_types($nth_setting) {
	return vget_checked_names('is_available_content_type_%' . $nth_setting . '_');
}

function vget_checked_names($term) {
	$ret_array = array();
	$mysql_query = "SELECT name FROM variable WHERE name LIKE '" . $term . "%'";
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		if (variable_get($q->name, 0) == 1)
			$ret_array[$q->name] = $q->name;
	}
	return $ret_array;
}

/**
 * ===========================================
 * CLASSES
 * ===========================================
 */
 
/**
 * Builds up the display for a given setting
 */
class IsAvailableSettingDisplay {

	private $SPACE = '&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp';
	function get_space() { return $this->SPACE; }

	function get_setting_display_string($nth_setting, $title) {
		
		$result_string = '<b><i style="text-transform: uppercase">Saved settings for ' . $title . ':</i></b><br>';
		if (!$this->check_if_setting_exists($nth_setting)) 
			return $result_string . 
				"<i><p>There aren't any settings yet for $title. 
				Please complete this form and save your settings.</p></i>";
		
		$array_entity_types = $this->display_entity_types($nth_setting);
		if ($this->check_if_empty($array_entity_types, 'entity types', $title)) return;
		
		$entity_types = reset($array_entity_types);
		$array_et_rt = $this->display_resource_types_get_array($nth_setting, $entity_types);	
		if ($this->check_if_empty($array_et_rt, 'resource types', $title)) return;
		
		$str_rt = $this->display_resource_types($nth_setting, $array_et_rt);
		$str_bldg_rn_table = $this->display_building_room_number_table($nth_setting, $array_et_rt);
		$str_auth_bldgs = $this->display_auth_buildings($nth_setting, $array_et_rt);
		
		$str_ct_settings = $this->display_content_types_settings($nth_setting, $array_et_rt);
		
		$result_attrs = array(
			'entity types' => key($array_entity_types),
			'resource types' => $str_rt,
			'building and room number tables' => $str_bldg_rn_table,
			'searchable buildings' => $str_auth_bldgs,
			'content type configuration' => $str_ct_settings,
		);
		
		foreach ($result_attrs as $key => $str) {
			$type = convert_to_readable_name($key);
			if (empty($str)) $this->set_error_message($key, $title);
			else $result_string .= "<b>$type</b>: $str <br><br>";
		}		
		return $result_string;
	}
	
	private function check_if_setting_exists($nth) {
		$query = db_query("SELECT count(name) as cnt FROM variable WHERE 
			name LIKE 'is_available%$nth%'");
		$count = 0;
		foreach ($query as $q) {
			$count = $q->cnt;
		}
		return $count > 8;
	}
	
	private function display_entity_types($nth_setting) {
		$array = vget_checked_entity_types($nth_setting);
		$ret_array = array();
		$space = $this->get_space();
		if (empty($array)) return "";
		$ret_string = "<br>$space";
		$len = strlen("is_available_entity_type_" . $nth_setting . "_");
		foreach ($array as $a) {
			$et_name = substr($a, $len);
			$ret_string .= convert_to_readable_name($et_name) . ", ";
			array_push($ret_array, $et_name);
		}
		$ret_string = substr($ret_string, 0, -2); // remove last ", "
		return array($ret_string => $ret_array);
	}
	
	private function display_resource_types_get_array($nth, $array_et) {
		$ret_array = array();
		foreach ($array_et as $et_name) {
			$array_rt = array(); 
			foreach (vget_checked_room_types($nth, $et_name) as  $rt_name) {
				$len = strlen("is_available_resource_type_" . $nth . "_") + strlen($et_name) + 1;
				$rt_name = substr($rt_name, $len);
				$array_rt[$rt_name] = $rt_name;
			}
			$ret_array[$et_name] = $array_rt;
		}
		return $ret_array;
	}
	
	private function display_resource_types($nth, $array_et_rt) {
		$ret_str = "";
		$space = $this->get_space();
		foreach ($array_et_rt as $et_name => $array_rt) {
			$et_name = convert_to_readable_name($et_name);
			$ret_str .= (empty($array_rt)) ? "" : "<br>$space <i>$et_name:</i> ";
			foreach ($array_rt as $rt_name) {
				$rt_name = convert_to_readable_name($rt_name);
				$ret_str .= "$rt_name, ";
			}
			$ret_str = substr($ret_str, 0, -2); // trim off last ", "
		}
		return $ret_str;
	}
	
	private function display_building_room_number_table($nth, $array_et_rt) {
		$ret_str = "";
		$none = "(none set)";
		$space = $this->get_space();
		$db_bldg_table_start = "is_available_building_table_" . $nth . "_";
		$db_rn_table_start = "is_available_resource_number_table_" . $nth . "_";
		foreach ($array_et_rt as $et_name => $array_rt) {
			foreach ($array_rt as $rt_name) {
				$db_bldg_term = $db_bldg_table_start . $et_name . "_" . $rt_name;
				$db_rn_term = $db_rn_table_start . $et_name . "_" . $rt_name;
				$readable_et_name = convert_to_readable_name($et_name);
				$readable_rt_name = convert_to_readable_name($rt_name);
				$bldg_table = variable_get($db_bldg_term, "");
				$rn_table = variable_get($db_rn_term, "");
				if (empty($bldg_table) || empty($rn_table)) {
					if (empty($bldg_table)) {
						$this->set_error_message("building table", "$readable_et_name $readable_rt_name");
						$bldg_str .= $none;
					}
					if (empty($rn_table)) {
						$this->set_error_message("room number table", "$readable_et_name: $readable_rt_name");
						$rn_str .= $none;
					}
					$ret_str .= $bldg_str . $rn_str;
				} else {
					$bldg_str = "<br>$space <i>Building table for $readable_et_name -> $readable_rt_name:</i> $bldg_table";
					$rn_str = "<br>$space <i>Room number table for $readable_et_name -> $readable_rt_name:</i> $rn_table";
					$ret_str .= $bldg_str . $rn_str;
				}		
			}
		}
		return $ret_str;
	}
	
	private function display_auth_buildings($nth, $array_et_rt) {
		$ret_str = "";
		$none = "(none set)";
		$space = $this->get_space();
		$auth_bldg_start = "is_available_auth_building_" . $nth . "_";
		$raw_auth_bldgs_ra = vget_checked_auth_buildings($nth);
		foreach ($array_et_rt as $et_name => $array_rt) {
			foreach ($array_rt as $rt_name) {
				$db_ab_term_start = $auth_bldg_start . $et_name . "_" . $rt_name . "_";
				$len = strlen($db_ab_term_start);
				$readable_et_name = convert_to_readable_name($et_name);
				$readable_rt_name = convert_to_readable_name($rt_name);
				$ab_str = "<br>$space <i>$readable_et_name -> $readable_rt_name: </i>";
				$auth_bldgs = "";
				foreach ($raw_auth_bldgs_ra as $key => $raw_str) {
					if (substr($raw_str, 0, $len) === $db_ab_term_start && 
						variable_get($raw_str, 0) == 1) {
						$auth_bldgs .= substr($raw_str, $len) . ", ";
						unset($raw_auth_bldgs_ra[$key]);
					}
				}
				if (empty($auth_bldgs)) {
					$this->set_error_message("searchable buildings", "readable_$et_name -> $readable_rt_name");
					$ab_str .= $none;
				}
				else $ret_str .= $ab_str . substr($auth_bldgs, 0, -2); // trim off last ", "
			}
		}
		return $ret_str;
	}
	
	private function display_content_types_settings($nth, $array_et_rt) {
		$ret_str = "";
		$none = "(none set)";
		$space = $this->get_space();
		$modname = "is_available";
		$ct_start = $modname . "_content_type_";
		$entref_start = $modname . "_entref_";
		$date_start = $modname . "_date_";
		$raw_ct_ra = vget_checked_content_types($nth);
		foreach ($array_et_rt as $et_name => $array_rt) {
			foreach ($array_rt as $rt_name) {
				$readable_et_name = convert_to_readable_name($et_name);
				$readable_rt_name = convert_to_readable_name($rt_name);
				$ct_str_result = "<br>$space <i>$readable_et_name -> $readable_rt_name:</i> ";
				$db_ct_term_start = $ct_start . $et_name . "_" . $rt_name . "_" . $nth . "_";
				$ct_len = strlen($db_ct_term_start);
				$etrt = $et_name . "_" . $rt_name . "_";
				$ctstr = "<br>$space$space <i>Content type:</i> "; 
				$entref_str = "<br>$space$space <i>Entity reference table:</i> "; 
				$date_str = "<br>$space$space <i>Date table:</i> ";
				$cts = ""; $entref = ""; $date = "";
				$ct_name = "";
				foreach ($raw_ct_ra as $key => $raw_str) {
					if (substr($raw_str, 0, $ct_len) === $db_ct_term_start && 
						variable_get($raw_str, 0) == 1) {
						$ct_name = substr($raw_str, $ct_len);
						$db_entref_term = $entref_start . $etrt  . $ct_name . "_" . $nth;
						$db_date_term = $date_start . $etrt . $nth . "_" . $ct_name;
						$cts .= convert_to_readable_name($ct_name);
						$entref .= variable_get($db_entref_term, "");
						$date .= variable_get($db_date_term, "");
						unset($raw_ct_ra[$key]);
					}
				}
			
				if (empty($cts)) {
					$this->set_error_message("content types", "$readable_et_name -> $readable_rt_name");
					$ctstr .= $none;
				} else $ctstr .= $cts; 
				if (empty($entref)) {
					$this->set_error_message("entity reference table", "$readable_et_name -> $readable_rt_name: $ct_name");
					$entref_str .= $none;
				} else $entref_str .= $entref; 
				if (empty($date)) {
					$this->set_error_message("date table", "$et_name: $rt_name -> $ct_name");
					$date_str .= $none;
				} else $date_str .= $date; 
				$ret_str .= $ct_str_result . $ctstr . $entref_str . $date_str;
			}
		}
		return $ret_str;
	}
	
	private function check_if_empty($array, $name, $title) {
		if (empty($array)) {
			$this->set_error_message($name, $title);
			return true;
		}
		return false;
	}
	
	private function set_error_message($name, $title) {
		drupal_set_message("No selected $name for $title found. Please check your settings.", "error");
	}
	
}


/**
 * Groups together the queries needed for the nested resource search query.
 */
class EmptyResourceQuery {
	private $DROP_TEMP_STR		= "";
	private $CREATE_TEMP_STR	= "";
	private $MAIN_QUERY				= "";
	
	function __construct($drop_str, $create_str, $main_query) {
		$this->DROP_TEMP_STR		= $drop_str;
		$this->CREATE_TEMP_STR	= $create_str;
		$this->MAIN_QUERY				= $main_query;
	}
	
	function get_drop_temp() 		{ return $this->DROP_TEMP_STR; }
	function get_create_temp() 	{ return $this->CREATE_TEMP_STR; }
	function get_main_query() 	{ return $this->MAIN_QUERY; }
	
	function to_string() {
		$br = "<br><br>";
		return "DROP: " . $this->get_drop_temp() . $br .
			"CREATE: " . $this->get_create_temp() . $br .
			"MAIN: " . $this->get_main_query() . $br;
	}
	
	function equals($obj) {
		if ($obj instanceof $this) {
			return $obj->get_drop_temp()	== $this->get_drop_temp() &&
				$obj->get_create_temp() 		== $this->get_create_temp() &&
				$obj->get_main_query() 			== $this->get_main_query();
		}
	}
}


/**
 * Oft-accessed constants for the admin settings form.
 * Specifically for is_available.admin.inc
 */
class IsAvailableAdminSettingsForm {
	
	// Variables
	private $KEY_SETTING_OPTIONS 	= 'settings_options';
	private $KEY_ENTITY_TYPES 		= 'entity_types';

	private $KEY_SETTINGS 				= 'setting';
	private $KEY_DISPLAY					= 'setting_display';
	private $KEY_ADD_SETTING 			= 'add_setting';
	private $KEY_DEL_SETTING			= 'delete_setting';
	private $KEY_RESOURCE_TYPE		= 'resource_type';
	private $KEY_BUILDING					= 'building';
	private $KEY_RESOURCE_NUMBER	= 'resource_number';
	private $KEY_AUTH_BUILDINGS 	= 'auth_building';
	private $KEY_CONTENT					= 'content_type';
	private $KEY_ENTREF						= 'entref';
	private $KEY_DATE							= 'date_table';
	private $KEY_SAVE_CONFIG			= 'save_config';
	
	
	// Buttons
	private $KEY_BUTTON						= '_button';
	private $KEY_FS_FORM_STAGE		= 'z_array_form_stage';
	private $KEY_CONFIG_WRAPPER		= 'config-wrapper';
	function get_wrapper_config() { return $this->KEY_CONFIG_WRAPPER; }

	
	// Other
	private $MODULE_NAME 					= 'is_available';	
	private $KEY_BLDG_TABLE 			= 'building_table';
	private $KEY_RN_TABLE 				= 'resource_number_table';
	private $KEY_BRN_TABLE 				= 'brn_table';
	private $KEY_ENTITY_TYPE			= 'entity_type';
	private $ALPHABET							= '123456789';
	
	/**
	 * Get the configurable setting options for the admin form
	 */
	function get_settings_options() {
		$array_return = array();
		$j = $this->get_settings_count($this->get_module_name());
		$i ='a';
		$alphabet = $this->ALPHABET;
		$count = substr($alphabet, $j-1, 1);
		if ($j == 0) {
			$name = variable_get($this->get_module_name() . '_settings_title_a', "");
			if ($name == "") {
				$name = 'Setting 1 (unconfigured)';
				variable_set($this->get_module_name() . '_settings_title_a', $name);
			}
			$array_return = array('a' => $name);
		} else {
		$setting_title = $this->get_module_name() . "_settings_title_";
		$mysql_query = "SELECT name FROM variable WHERE name LIKE '" . $setting_title . "%'";
		$query = db_query($mysql_query);
		foreach ($query as $q) {
			$k = substr($q->name, strlen($setting_title));
				$array_return[$k] =
					variable_get($q->name, 'Setting ' . strtoupper($k) . ' (unconfigured)');
			}
		}
		return $array_return;
	}
	
	/**
	 * Count the number of settings, by title (settings may not have been set yet).
	 */
	function get_settings_count($module_name = '') {
		$module_name = mysql_real_escape_string($this->get_module_name());
		$mysql_query = "SELECT COUNT(DISTINCT name) as cnt FROM variable WHERE name LIKE '" . $module_name . "_settings_title%'";
		$count = 0;
		$query = db_query($mysql_query);
		foreach ($query as $q) {
			$count = $q->cnt;
		}
		return $count;
	}
	
	// Getters
	function get_key_setting_options() 		{ return $this->KEY_SETTING_OPTIONS; }
	function get_key_entity_types()			{ return $this->KEY_ENTITY_TYPES; }
	
	function get_module_name()				 	{ return $this->MODULE_NAME; }
	function get_key_button()						{ return $this->KEY_BUTTON; }
	
	function get_key_config_settings() 				{ return $this->get_configure_key($this->KEY_SETTINGS); }
	function get_key_display_setting()				{ return $this->KEY_DISPLAY; }
	function get_key_config_resource_type()		{ return $this->get_configure_key($this->KEY_RESOURCE_TYPE); }
	function get_key_config_building() 				{ return $this->get_configure_key($this->KEY_BUILDING); }
	function get_key_config_resource_number()	{ return $this->get_configure_key($this->KEY_RESOURCE_NUMBER); }
	function get_key_config_auth_buildings()	{ return $this->get_configure_key($this->KEY_AUTH_BUILDINGS); }
	function get_key_config_content()		{ return $this->get_configure_key($this->KEY_CONTENT); }
	function get_key_config_entref()		{ return $this->get_configure_key($this->KEY_ENTREF); }
	function get_key_config_date()			{ return $this->get_configure_key($this->KEY_DATE); }
	function get_key_save_config()			{ return $this->KEY_SAVE_CONFIG; }
	
	function get_wrapper_settings() 		{ return $this->get_wrapper($this->KEY_SETTINGS); }
	function get_wrapper_setting_display() { return str_replace('_', '-', $this->KEY_DISPLAY); }
	function get_wrapper_add_setting()	{ return str_replace('_', '-', $this->KEY_ADD_SETTING); }
	function get_wrapper_delete_setting()		{ return str_replace('_', '-', $this->KEY_DEL_SETTING); }
	function get_wrapper_resource_type() 		{ return $this->get_wrapper($this->KEY_RESOURCE_TYPE); }
	function get_wrapper_building() 		{ return $this->get_wrapper($this->KEY_BUILDING); }
	function get_wrapper_resource_number() 	{ return $this->get_wrapper($this->KEY_RESOURCE_NUMBER); }
	function get_wrapper_auth_buildings()		{ return $this->get_wrapper($this->KEY_AUTH_BUILDINGS); }
	function get_wrapper_content() 			{ return $this->get_wrapper($this->KEY_CONTENT); }
	function get_wrapper_entref() 			{ return $this->get_wrapper($this->KEY_ENTREF); }
	function get_wrapper_date() 				{ return $this->get_wrapper($this->KEY_DATE); }
	function get_wrapper_save_config()	{ return str_replace('_', '-', $this->KEY_SAVE_CONFIG); }
	
	function get_key_add_setting()			{ return $this->KEY_ADD_SETTING; }
	function get_key_delete_setting()		{ return $this->KEY_DEL_SETTING; }
	function get_key_entity_type()			{ return $this->KEY_ENTITY_TYPE; }
	function get_key_resource_type()		{ return $this->KEY_RESOURCE_TYPE; }
	function get_key_brn_table()				{ return $this->KEY_BRN_TABLE; }
	function get_key_auth_buildings()		{ return $this->KEY_AUTH_BUILDINGS; }
	function get_key_content_type()			{ return $this->KEY_CONTENT; }
	function get_key_entref()						{ return $this->KEY_ENTREF; }
	function get_key_date()							{ return $this->KEY_DATE; }
	
	function get_key_fs_form_stage()		{ return $this->KEY_FS_FORM_STAGE; }
	function get_key_building_table()		{ return $this->KEY_BLDG_TABLE; }
	function get_key_resource_number_table() { return $this->KEY_RN_TABLE; }
	
	function get_alphabet()							{ return $this->ALPHABET; }
	
	// Setters
	function set_module_name($mod_name) { $this->MODULE_NAME = $mod_name; }

	
	/**
	 * string -> string
	 * Returns the key of the relevant form stage. For get_wrapper, replaces _ with -
	 * @param $key The key, with terms separated by _
	 * 
	 * @return The key with 'configure_' appended to the front
	 */
	function get_configure_key($key)	{ return $this->get_module_name() . '_configure_' . $key; }
	function get_key_only($key_with_module_name) { 
		return substr($key_with_module_name, strlen($this->get_module_name()) + 1); 
	}
	function get_wrapper($key) 				{ return 'configure-' . str_replace('_', '-', $key); }
	
}

// ========================
// ========================
/**
 * A searchable entity type.
 */
class EntityTaipe {
	private $MACHINE_NAME = ''; // e.g. space
	private $LABEL = ''; // e.g. Space
	
	private $ECK_TABLE = ''; // e.g. eck_<machine_name>
	private $ECK_TITLE_COLUMN_NAME = 'title'; // select <this> from <eck_table> 
	
	private $ARRAY_RESOURCE_TYPES = array(); // array(machine_name => ResourceType object)
	
	function __construct($machine_name, $label = '') {
		$this->MACHINE_NAME = $machine_name;
		$this->LABEL = $label == '' ? convert_to_readable_name($machine_name) : $label;
		$this->ECK_TABLE = 'eck_' . $machine_name;
	}	
	
	// Getters
	function get_machine_name() 	{ return $this->MACHINE_NAME; }
	function get_name() 					{ return $this->LABEL; }
	function get_eck_table()			{ return $this->ECK_TABLE; }
	function get_title_column() 	{ return $this->ECK_TITLE_COLUMN_NAME; }	
	function get_resource_types()	{ return $this->ARRAY_RESOURCE_TYPES; }
	
	// Setters
	function set_machine_name($machine_name)		{ $this->MACHINE_NAME = $machine_name; 
/*function set_eck_table()	*/									$this->ECK_TABLE = 'eck_' . $this->MACHINE_NAME; }
	function set_label($label)									{ $this->LABEL = $label; }
	function set_title_column($col_name) 				{ $this->TITLE_COLUMN_NAME = $col_name; }
	
	function set_resource_types($array) 			{ $this->ARRAY_RESOURCE_TYPES = $array; }
	function add_to_resource_types($object)		{ $this->ARRAY_RESOURCE_TYPES[$object->get_machine_name()] = $object; }
	function rem_from_resource_types($object) { unset($this->ARRAY_RESOURCE_TYPES[$object->get_machine_name()]); }
	
	function to_string() {
		return '<br>Machine Name: ' . $this->get_machine_name() .
			' <br>Label: ' . $this->get_name() .
			' <br>Table: ' . $this->get_eck_table() .
			' <br>Title Column: ' . $this->get_title_column();
	}
	
	function equals($obj) {
		$isEqual = FALSE;
		if ($obj instanceof $this) 
			$isEqual = $this->get_machine_name() == $obj->get_machine_name()
				&& $this->get_name() == $obj->get_name()
				&& $this->get_table() == $obj->get_table()
				&& $this->get_title_column() == $obj->get_title_column();
		return isEqual;
	}
}

/**
 * A searchable resource type.
 */
 class ResourceType {
	/**
	 * Resource type
	 */
	private $S_ID 							= ''; // type<n>
	private $TITLE							= '';	// e.g. Meeting Resource, as entered
	private $CB_VALUE 					= ' '; // cb_meeting_resource (for checkboxes)
	private $PLURAL_NAME 				= ''; // meeting resources (for results display)
	private $MACHINE_NAME 			= ''; // meeting_resource (for the database)
	private $ENTITY_TAIPE 			= ''; // The associated entity type
	
		// Field tables and column names -- append 'field_data_' to the front
	// for the table name, and append '_value'to the end for the column name
	private $FIELD_ROOM_NUMBER	= ''; // e.g. field_resource_number
	private $FIELD_BUILDING 		= '';
	private $FIElD_RN_TABLE 		= '';
	private $FIELD_RN_COL				= '';
	private $FIELD_BLDG_TABLE		= '';
	private $FIELD_BLDG_COL			= '';
	
	private $FORM_FBLD_KEY 			= ''; // e.g. field_building
	private $FORM_FRN_KEY 			= '';  // e.g. field_room_numbeer
	
	private $ARRAY_CONTENT_TYPES = array(); // array of string names of content types
	private $ARRAY_ENTREF_DATE_TABLES	= array(); // array(entref_table => date_table)
	private $ARRAY_BUILDINGS 					= array(); // The buildings in which this resource is located
	
	
	/**
	 * Integer String Boolean -> void
	 * @param $nth_type the nth option on the list
	 * @param $name A human readable name for the resource type, e.g. "Meeting Resource"
	 * @param $auth_display Whether the type should be displayed to the user
	 */
	function __construct($machine_name, $buildings = array(), $label = '') {
		$this->MACHINE_NAME = $machine_name;
		$this->S_ID = $machine_name; // : 'type' . $nth_type;
		$this->TITLE = (trim($label, ' ') == '') ? convert_to_readable_name($machine_name) : $label;
		$this->CB_VALUE = 'cb_' . $machine_name;						
		$this->PLURAL_NAME = strtolower($this->TITLE);
		$this->PLURAL_NAME .= 's';
		if (!empty($buildings))	$this->BUILDINGS = $buildings;
	}
	 
	 // Getters
	function get_str_id()				{ return $this->S_ID; }
	function get_title() 				{ return $this->TITLE; }
	function get_cb_value() 		{ return $this->CB_VALUE; }
	function get_plural_name() 	{ return $this->PLURAL_NAME; }
	function get_machine_name()	{ return $this->MACHINE_NAME; }
	function get_buildings() 		{ return $this->ARRAY_BUILDINGS; }
	
	function get_field_room_number() 	{ return $this->FIELD_ROOM_NUMBER; }
	function get_field_building()			{ return $this->FIELD_BUILDING; }
	function get_entity_type()				{ return $this->ENTITY_TAIPE; }
	
	function get_form_fbld_key()			{ return $this->FORM_FBLD_KEY; }
	function get_form_frn_key()				{ return $this->FORM_FRN_KEY; }
	
	function get_content_types()			{ return $this->ARRAY_CONTENT_TYPES; }
	function get_entref_date_table() 	{ return $this->ARRAY_ENTREF_DATE_TABLES; }
	
	function get_room_number_table()	{ return $this->FIElD_RN_TABLE; }
	function get_room_number_field()	{ return $this->FIELD_RN_COL; }
	function get_building_table()			{ return $this->FIELD_BLDG_TABLE; }
	function get_building_field()			{ return $this->FIELD_BLDG_COL; }
	
	// Setters -- don't let users change values other than set_auth_display
	function set_buildings($buildings) 		{ $this->ARRAY_BUILDINGS = $buildings; }
	
	function set_field_room_number($rn) { 
		$this->FIELD_ROOM_NUMBER	= $rn; 
		$this->FIElD_RN_TABLE 		= get_field_table($rn);
		$this->FIELD_RN_COL				= get_field_column($rn);
	}
	
	function set_field_building($bldg) { 
		$this->FIELD_BUILDING 		= $bldg; 
		$this->FIELD_BLDG_TABLE		= get_field_table($bldg);
		$this->FIELD_BLDG_COL			= get_field_column($bldg);
	}
	
	function set_entity_type($et)					{ $this->ENTITY_TAIPE = $et; }
	
	function set_form_fbld_key($fbld_key)		{ $this->FORM_FBLD_KEY = $fbld_key; }
	function set_form_frn_key($frn_key)			{ $this->FORM_FRN_KEY = $frn_key; }
	
	function set_entref_date_tables($array)  { $this->ARRAY_ENTREF_DATE_TABLES = $array; }
	function add_to_entref_date_table($entref, $date)	{ $this->ARRAY_ENTREF_DATE_TABLES[$entref] = $date; }
	function rem_entref_from_entref_date_table($entref) { unset($this->ARRAY_ENTREF_DATE_TABLES[$entref]); }
	function rem_date_from_entref_date_table($date) { 
		$this->rem_from_array($date, $this->ARRAY_ENTREF_DATE_TABLES); 
	}
	
	function set_content_types($array)			{ $this->ARRAY_CONTENT_TYPES = $array; }
	
	private function rem_from_array($value, $array) {
		if (in_array($value, $array)) unset($array[array_search($value)]); 
	}

	function to_string() {
	$ret_str = 
		'<br><br>S_ID: ' . $this->get_str_id() .
		'<br>Title: ' . $this->get_title() . 
		'<br>cb_value: ' . $this->get_cb_value() .
		'<br>plural name: ' . $this->get_plural_name() .
		'<br>machine name: ' . $this->get_machine_name() . 
		'<br>Buildings: ';
		foreach ($this->get_buildings as $b) 
			$ret_str .= $b->to_string();
	return $ret_str;
	}
	
	// Not case sensitive
	function equals($obj) {
		$isEqual = FALSE;
		if ($obj instanceof $this) {
			$isEqual = (
				$obj->get_str_id() == $this->get_str_id()
				&& $obj->get_title() == $this->get_title() 
				&& $obj->get_cb_value() == $this->get_cb_value()
				&& $obj->get_plural_name() == $this->get_plural_name()
				&& $obj->get_machine_name() == $this->get_machine_name() 
				&& $obj->get_buildings() == $this->get_buildings()
			);
		}
		return $isEqual;
	}
}

/**
 * A building in which a resource type is located.
 */
class Building {
	private $S_B_ID = ''; // e.g. building<n>, for the dropdown
	private $NAME = ''; // building's name as stored in the db
	
	function __construct($name) {
		$this->S_B_ID = $name; 
		$this->NAME = convert_to_readable_name($name);
	}
	
	// Getters
	function get_str_id()	{ return $this->S_B_ID; }
	function get_name()		{ return $this->NAME; }
	
	function to_string() {
		return
			'<br><br>S_B_ID: ' . $this->get_str_id() .
			'<br>Name: ' . $this->get_name(); 
	}
	
	function equals($obj) {
		$isEqual = FALSE;
		if ($obj instanceof $this) {
			$isEqual = (
				$obj->get_str_id() == $this->get_str_id()
				&& $obj->get_name() == $this->get_name() 
			);
		}
		return $isEqual;
	}
}

	/**
	 * Your standard Pair<T1, T2> class. :)
	 */
class Pair {
	private $first;
	private $second;
	function __construct($one, $two) {
		$this->first = $one; $this->second = $two;
	}
	
	// Getters
	function get_first() 	{ return $this->first; }
	function get_second() { return $this->second; }
	
	// Setters
	function set_first($newfirst) 	{ $this->first = $newfirst; }
	function set_second($newsecond)	{ $this->second = $newsecond; }
	
	// to_string
	function to_string() {
		$string = '<br>First: ' . is_scalar($this->first) ?
			strval($this->first) :
			$this->first->to_string(); // Assuming first has its own to_string function
		$string .= '<br>Second: ' . is_scalar($this->second) ?
			strval($this->second) :
			$this->second->to_string(); // Assuming second has its own to_string function
		return $string;
	}
	
	function equals($obj) {
		$isEqual = FALSE;
		if ($obj instanceof $this) {
			$isEqual = $obj->get_first() == $this->get_first()
				&& $obj->get_second() == $this->get_second();
		}
		return $isEqual;
	}
}


	
