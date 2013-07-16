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


/**
 * ===========================================
 * CLASSES
 * ===========================================
 */

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
				$name = 'Setting A (unconfigured)';
				variable_set($this->get_module_name() . '_settings_title_a', $name);
			}
			$array_return = array('a' => $name);
		} else {
		$setting_title = $this->get_module_name() . "_settings_title_";
		$mysql_query = "SELECT name FROM variable WHERE name LIKE '" . $setting_title . "%'";
		$query = db_query($mysql_query);
		foreach ($query as $q) {
			dpm($q->name . ", " . variable_get($q->name), "");
			$k = substr($q->name, strlen($setting_title));
				$array_return[$k] =
					variable_get($q->name, 'Setting ' . strtoupper($k) . ' (unconfigured)');
			}
		}
		dpm($array_return);
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
	function get_key_config_resource_type()		{ return $this->get_configure_key($this->KEY_RESOURCE_TYPE); }
	function get_key_config_building() 				{ return $this->get_configure_key($this->KEY_BUILDING); }
	function get_key_config_resource_number()	{ return $this->get_configure_key($this->KEY_RESOURCE_NUMBER); }
	function get_key_config_auth_buildings()	{ return $this->get_configure_key($this->KEY_AUTH_BUILDINGS); }
	function get_key_config_content()		{ return $this->get_configure_key($this->KEY_CONTENT); }
	function get_key_config_entref()		{ return $this->get_configure_key($this->KEY_ENTREF); }
	function get_key_config_date()			{ return $this->get_configure_key($this->KEY_DATE); }
	function get_key_save_config()			{ return $this->KEY_SAVE_CONFIG; }
	
	function get_wrapper_settings() 		{ return $this->get_wrapper($this->KEY_SETTINGS); }
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


	
