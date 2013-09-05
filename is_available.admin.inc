<?php

/**
 * @file
 * Admin settings form
 */

/**
  * Copyright 2013 Mira Leung
  *
  * This file is part of Is Available.
  *
  * Is Available is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  *
  * Is Available is distributed in the hope that it will be useful,
  * but without any warranty; without even the implied warranty of
  * merchantability or fitness for a particular purpose.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with Is Available.  If not, see <http://www.gnu.org/licenses/>.
  */

function is_available_settings($form, &$form_state, $module_name = 'is_available') {
	
	// Constants		
	// // Objects & Arrays
	// // // Initialize and set up $form_object
	$form_object = new IsAvailableAdminSettingsForm;
	$form_object->_is_available_set_module_name($module_name);	
	
	// // // Array of entity types (IsAvailableEntityTaipe objects) to search
	$array_entity_types = array();  
	
	// // // Array of IsAvailableResourceType objects to search
	$array_resource_types = array();
	
	// // // Form object variables
	$key_setting_options 	= $form_object->_is_available_get_key_setting_options();
	$key_config_content 	= $form_object->_is_available_get_wrapper_content();
	
	// // Booleans
	$bool_setting_selected = isset($form_state['values'][$key_setting_options]); 
	
	// // Integers / Values
	$array_settings_options = $form_object->_is_available_get_settings_options();
	$nth_setting = isset($form_state['values'][$key_setting_options]) ? 
		$form_state['values'][$key_setting_options] : 
		key($array_settings_options);
	
	// Build the form
	// // Build the setting options and buttons
	$int_sizeof_setting_options = 
		_is_available_build_form_setting_options($form, $form_object);
	_is_available_build_form_setting_buttons($form, $form_object, $int_sizeof_setting_options);

	// // Wrap the ajax callback sections of thesettings form
	_is_available_build_form_wrap_config_settings($form, $form_object);
		
	$array_entity_types = _is_available_get_checkboxes_value($form_state, 'entity_type');
	
	// array_form_stage_n_elements -- the number of form elements at a given stage
	$fsne_arg = empty($form_state['values']) ? array() : $form_state['values'];
	$array_fsne = 
		_is_available_get_form_stage_n_element_array($fsne_arg, $nth_setting);
	
	$checked_entity_types = 
		_is_available_get_checked_only($form_state, 'entity_type', $nth_setting);
	$checked_et_rt = 
		_is_available_get_checked_et_rt($form_state, $checked_entity_types, $nth_setting);
	
	// Form has 8 steps
	$array_form_stage = array(
		'is_at_entity_type'			=> _is_available_is_at_entity_type($form_object, $array_fsne), 
		'is_at_resource_type' 	=> _is_available_is_at_resource_type($form_object, $array_fsne),
		'is_at_brn_table'				=> _is_available_is_at_brn_table($form_object, $array_fsne), 
		'is_at_auth_buildings'	=> _is_available_is_at_auth_buildings($form_object, $array_fsne),
		'is_at_content_type'		=> _is_available_is_at_content_type($form_object, $array_fsne),
		'is_at_entref'					=> _is_available_is_at_entref($form_object, $array_fsne),
		'is_at_date'						=> _is_available_is_at_date($form_object, $array_fsne),
	);
	
	// // Build the configuration form for the chosen setting
	_is_available_build_form_setting_configuration($form, $form_state, $form_object, 
		$nth_setting, $array_entity_types, $array_form_stage['is_at_entity_type']);
	
	if ($array_form_stage['is_at_entity_type']) {
		$bool_checked_et = _is_available_has_checked_entity_type($checked_entity_types);
		_is_available_build_form_config_resource_type($form, 
			$form_state, $form_object, $nth_setting, $array_entity_types, 
			$bool_checked_et, $array_form_stage['is_at_resource_type']);
		_is_available_build_form_reset_button($form, $form_object);
		if ($array_form_stage['is_at_resource_type']) {
		$bool_checked_rt = _is_available_has_checked_resource_type($checked_et_rt);
		$array_resource_types = 
			_is_available_build_form_config_building_table($form, $form_state, 
			$form_object, $nth_setting, $checked_et_rt, $bool_checked_rt);
		}
		
		if ($array_form_stage['is_at_brn_table']) {
			$bool_selected_brn_table = _is_available_has_selected_brn_table($form_state['values']);
			_is_available_build_form_config_auth_buildings($form, $form_state, $form_object, 
				$nth_setting, $array_resource_types, $bool_selected_brn_table);
			_is_available_build_form_config_content_types($form, $form_state, $form_object, 
				$nth_setting, $array_resource_types); 
		}
		
		
		if ($array_form_stage['is_at_content_type']) {
			$checked_auth_buildings = 
				_is_available_get_checked_auth_buildings($form_state, $form_object, 
				$checked_et_rt, $nth_setting);
				
			$bool_has_checked_auth_bldgs = 
				_is_available_has_checked_for_each_rt($checked_auth_buildings);
				
			_is_available_build_form_config_entref($form, $form_state, $form_object, 
				$nth_setting, $array_resource_types, $bool_has_checked_auth_bldgs);
		}
	
		if ($array_form_stage['is_at_entref']) {
			$bool__is_available_has_selected_entref = _is_available_has_selected_entref($form_state['values']); 
			_is_available_build_form_config_date($form, $form_state, $form_object, $nth_setting, $array_resource_types, $bool__is_available_has_selected_entref);
		}
		
		if ($array_form_stage['is_at_date']) { 
			$bool_selected_date = _is_available_has_selected_date($form_state['values']);
			_is_available_build_sys_settings($form, $bool_selected_date);
		}
	} 
	
	// system_settings_form($form) is taken care of in _is_available_build_sys_settings
	return $form; 
}

/**
 * -----------------------------------------------
 * FORM BUILDERS
 * -----------------------------------------------
 */

 
 /**
 * array IsAvailableAdminSettingsForm -> (void)
 *
 * Build the setting options
 * @param $form	The form to build
 * @param $form_object
 */
function _is_available_build_form_setting_options(&$form, $form_object) {
	$key_setting_options = $form_object->_is_available_get_key_setting_options();
	$array_settings_options = $form_object->_is_available_get_settings_options();
	$wrapper = $form_object->_is_available_get_wrapper_settings(); 
	$options_wrapper = $form_object->_is_available_get_wrapper_options();
	
	$form['notes'] = array(
		 '#markup' => '<i><p>How to use this form <p>' .
			'<p> 1) Proceed downwards through the 7 steps of 
			the form until the "Save Configuration" button appears.</p>
			<p> 2) To change a value from a preceding stage, reload the 
			form by clicking on "Configure Setting."</p>
			<p> 3) Settings are not saved until 
			"Save Configuration" is clicked.</p></i>',
	);
	
	$form[$key_setting_options]  = array(
		'#type' => 'radios',
		'#title' => _is_available_convert_to_readable_name($key_setting_options),
		'#options' => $array_settings_options,
		'#default_value' => key($array_settings_options),
		'#ajax' => array(
			'callback' => 'ajax_config_setting_callback', 
			'event' => 'change',
			'wrapper' => $wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div id="' . $options_wrapper . '">',
		'#suffix' => '</div>',
	);
	
	return sizeof($array_settings_options);
}

function _is_available_build_form_setting_display(&$form, &$form_state, $form_object) {

	$key_setting_options = $form_object->_is_available_get_key_setting_options();
	$array_settings_options = $form_object->_is_available_get_settings_options();
	$key = $form_object->_is_available__is_available_get_key_config_settings();
	$wrapper = $form_object->_is_available_get_wrapper_setting_display();
	$key_display = 'display';
	
	$nth = isset($form_state['values'][$key_setting_options]) ? 
		$form_state['values'][$key_setting_options] : 
		key($array_settings_options);
		
	$title =  $array_settings_options[$nth];
	
	$setting_display_obj = new IsAvailableSettingDisplay;
	$display_string = 
		$setting_display_obj->get_setting_display_string($nth, $title);
	
	$form[$key][$key_display] = array(
		'#markup' => '<p>' . $display_string . '</p>',
		'#prefix' => '<div id="' . $wrapper . '">',
		'#suffix' => '</div>',
	);
}


/**
 * array array IsAvailableAdminSettingsForm array Integer Boolean -> (void)
 *
 * Build the setting configuration form
 *
 * @param $form The form to build
 * @param $form_state The form state
 * @param $form_object
 * @param $array_stage_form 
 * @param $nth_setting The selected setting to modify (add a new one if 0)
 * @param $bool_setting_selected Whether a setting has been chosen or not
 */
function _is_available_build_form_setting_configuration(&$form, &$form_state, $form_object, 
	$nth_setting, &$array_entity_types, $bool) {
	
	$module_name = $form_object->_is_available_get_module_name();
	$key_config_settings = $form_object->_is_available__is_available_get_key_config_settings();
	$key_setting_options = $form_object->_is_available_get_key_setting_options();
	
	_is_available_build_form_setting_display($form, $form_state, $form_object);
	_is_available_build_fsc_original_title($form, $form_object, $nth_setting); 
	_is_available_build_fsc_title_textfield($form, $form_object, $nth_setting, $bool);
	$temp_int = _is_available_build_fsc_entity_types($form, $form_object, $nth_setting, $bool);

	// If no entity types, return
	if ($temp_int == 0) {
		drupal_set_message('Sorry, no searchable entity types found. 
			Please check your entity type configurations.', 'error');
		return;
	}
	_is_available_build_fsc_select_entity_type_button($form, $form_state, $form_object, 
		$array_entity_types, $key_config_settings);
	return;
}


function _is_available_build_fsc_original_title(&$form, $form_object, $nth_setting) {
	$module_name = $form_object->_is_available_get_module_name();
	$key_config_settings = $form_object->_is_available__is_available_get_key_config_settings();
	$key_setting_options = $form_object->_is_available_get_key_setting_options();
	$key_config_resource_type = $form_object->_is_available_get_key_config_resource_type();
	$title_wrapper = $form_object->_is_available_get_wrapper_title();	
	
	$form[$key_config_settings][$key_config_settings] = array(
		'#prefix' => '<div id="' . $title_wrapper . '">',
		'#suffix' => '</div>',
	);
	
	$form[$key_config_settings]
		[$key_config_settings]
		[_is_available_append_nth('original_title', $nth_setting)] = array(
			'#markup' => '<b id="config-settings-header"><i>
			<p style="text-transform: uppercase">
						Configure settings for ' . 
						variable_get(_is_available_append_nth('settings_title', $nth_setting), 
						$form[$key_setting_options]['#options'][$nth_setting]) .
			'</p></i><p>Step 1 of 7: Set title and select entity types</p></b>',
		);		
}

function _is_available_build_fsc_title_textfield(&$form, $form_object, $nth_setting, $bool) {
	$key_config_settings = $form_object->_is_available__is_available_get_key_config_settings();
	$key_setting_options = $form_object->_is_available_get_key_setting_options();
	$key_config_resource_type = $form_object->_is_available_get_key_config_resource_type();
	
	$form[$key_config_settings]
		[$key_config_settings]
		[_is_available_append_nth('settings_title', $nth_setting)] = array(
		'#type' => 'textfield',
		'#title' => t('Settings title'),
		'#default_value' => variable_get(_is_available_append_nth('settings_title', $nth_setting), 
			$form[$key_setting_options]['#options'][$nth_setting]),
		'#required' => TRUE,
	);
}

function _is_available_build_fsc_entity_types(&$form, $form_object, $nth_setting, $bool) {
	$module_name = $form_object->_is_available_get_module_name();
	$key_config_settings = $form_object->_is_available__is_available_get_key_config_settings();
	$key_setting_options = $form_object->_is_available_get_key_setting_options();
	$key_config_resource_type = $form_object->_is_available_get_key_config_resource_type();

	// $entity_types['space'] = IsAvailablePair(IsAvailableEntityTaipe('space', 'Space'), array('title', ...))
	$entity_type_pairs = _is_available_get_entity_types();
	$entity_type_names = array();
	if (empty($entity_type_pairs)) {
		return 0;
	}
	
	foreach ($entity_type_pairs as $machine_name => $pair) {
		$entity_type = $pair->_is_available_get_first();
		$title_array = $pair->_is_available_get_second();
		$entity_type_names[$machine_name] = $entity_type->_is_available_get_name();
	}
	
	$form[$key_config_settings][$key_config_settings]['markup_entity_type'] = array(
		'#markup' => '<p><b>Entity types to search</b></p>',
	);
	
	// Setup entity type selection checkboxes
	foreach ($entity_type_names as $machine_name => $name) {
		$form[$key_config_settings][$key_config_settings]
			[_is_available_append_nth('entity_type', $nth_setting) . '_' . $machine_name] = array(
			'#type' => 'checkbox',
			'#title' => t($name),
			'#default_value' => variable_get(_is_available_append_nth('entity_type', $nth_setting) . '_' . $machine_name, 0),
		);
	}
	return 1;
}

function _is_available_build_form_config_resource_type(&$form, $form_state, $form_object, $nth_setting, 
	$array_entity_types, $bool_checked_et, $bool) {
	$key_config_resource_type = $form_object->_is_available_get_key_config_resource_type();
	$resource_type_wrapper = $form_object->_is_available_get_wrapper_resource_type();

	$int_ret = _is_available_build_form_crt_checkboxes($form, $form_state, 
		$form_object, $nth_setting, 
		$array_entity_types, $bool_checked_et, $bool);
	
	if ($int_ret == 1) {
		_is_available_build_form_crt_select_button($form, $form_object);
	}
	return $int_ret;
}

function _is_available_build_form_crt_checkboxes(&$form, $form_state, 
	$form_object, $nth_setting, $array_entity_types, $bool_checked_et, $bool) {
	$key_config_resource_type = $form_object->_is_available_get_key_config_resource_type();
	
	if (!$bool_checked_et) {
		drupal_set_message('Please select an entity type for searching.', 'error');
		$array_entity_types = _is_available_get_checkboxes_value($form_state, 'entity_type');
		_is_available_build_fsc_select_entity_type_button($form, $form_state, $form_object, 
			$array_entity_types, $key_config_resource_type, $key_config_resource_type);
		return 0;
	}
	
	// Build resource checkboxes
	$string_start = _is_available_append_nth('entity_type', $nth_setting); // plus _space
	$string_start_length = (strlen($string_start) + 1);
	$resource_type_wrapper = $form_object->_is_available_get_wrapper_resource_type();
	
	_is_available_build_form_generic_wrap($form, $key_config_resource_type, 
		$resource_type_wrapper);
	$form[$key_config_resource_type]['resource_type'] = array(
		'#markup' => '<p id="step-2"><b>Step 2 of 7: Configure entity types ',
	);
	$markup = &$form[$key_config_resource_type]['resource_type']['#markup'];
	$form[$key_config_resource_type]['et'] = array(
		'#prefix' => '<div id="resource-types">',
		'#suffix' => '</div>',
	);
	
	foreach ($array_entity_types as $et => $value) {
		if ($value == 1) { // Only for checked-off entities
			$entity_type = substr($et, $string_start_length);
			
			$readable_et = _is_available_convert_to_readable_name(substr($et, $string_start_length));
			$array_resource_types = _is_available_get_entity_type_resource_types($entity_type);
			$markup .= $readable_et . ', ';
			
			if (sizeof($array_resource_types) == 0) {
				drupal_set_message('Sorry, no searchable resource types 
					available for entity type ' . 	$readable_et . '. 
					Please check the configurations for ' . 
					$readable_et . '.', 'error');
			} else {
				$form[$key_config_resource_type]['et'][$et . '_resource_type'] = array(
					'#type' => 'fieldset',
					'#title' => t($readable_et . ' Resource Types'),
				);
			
				foreach ($array_resource_types as $machine_name => $label) {
					$form[$key_config_resource_type]['et'][$et . '_resource_type']
						[_is_available_append_nth('resource_type', $nth_setting) . '_' . 
							$entity_type . '_' . $machine_name] = array(
							'#type' => 'checkbox',
							'#title' => $label,
							'#default_value' => 
								variable_get(_is_available_append_nth('resource_type', $nth_setting) . 
									'_' . $entity_type . '_' . $machine_name, 1),	
					);
				}
			}
		}
	}
	$markup = substr($markup, 0, -2) . '</b></p>'; // trim off last ', '.
	return 1;
}

function _is_available_build_form_config_building_table(&$form, $form_state, 
	$form_object, $nth_setting, $checked_et_rt, $bool_checked_rt) {
	
	if (!$bool_checked_rt) {
		$key_building = $form_object->_is_available_get_key_config_building();
		drupal_set_message('Please select at least one 
			resource type for each entity type.', 'error');
		_is_available_build_form_crt_select_button($form, $form_object, $key_building);
		return array();
	}
	
	$array_resource_type = _is_available_build_form_building_options($form, 
		$form_object, $nth_setting, $checked_et_rt); 
	_is_available_build_form_building_button($form, $form_object);
	return $array_resource_type;
	
}

function _is_available_build_form_building_options(&$form, $form_object, $nth_setting, $checked_et_rt) { 
	$key_building = $form_object->_is_available_get_key_config_building();
	$str_start_et = $form_object->_is_available_get_module_name() . 
		'_entity_type_'; // + n_<entity_type>
	$str_start_rt = $form_object->_is_available_get_module_name() . 
	'_resource_type_'; // + n_<entity_type>_<resource_type>
	$str_start_et_len = strlen($str_start_et) + strlen($nth_setting . "_");
	$str_start_rt_len = strlen($str_start_rt) + strlen($nth_setting . "_");
	
	$array_resource_type = array();
	if (empty($checked_et_rt)) {
		drupal_set_message('Sorry, an error has occurred -- 
			no selected entity types 	or resource types were detected. 
			Please check your settings.', 'error');
	} else {
		$building_wrapper = $form_object->_is_available_get_wrapper_building();
		_is_available_build_form_generic_wrap($form, $key_building, $building_wrapper);
		// <mod_name>_entity_type_m_<entity_type> => array
		$form[$key_building]['selected_et_rt'] = array(
			'#markup' => '<p id="step-3"><b>Step 3 of 7: 
				Configure resource type tables for each entity type: <br>',
		);
		$markup = &$form[$key_building]['selected_et_rt']['#markup'];
	
		foreach ($checked_et_rt as $et => $array_rt) {
			// <mod_name>_resource_type_n_<entity_type>_<resource_type>
			$readable_et = _is_available_convert_to_readable_name($et);
			$markup .= $readable_et . ' -> ';
			foreach ($array_rt as $rt) {
				$readable_rt = _is_available_convert_to_readable_name($rt);
				$markup .= $readable_rt . ', ';
				$et_rt = $et . '_' . $rt;
				
				// <entity_type>_<resource_type>_<table_name> => <table_name>
				$building_resource_number_options = _is_available_get_building_table_options($et, $rt);
	
				// <module_name>_resource_type_n_<entity_type>_<resource_type>
				// _building_table
				// e.g. is_available_resource_type_1_space_classresource_building_table
				
				$form[$key_building][$et_rt] = array(
					'#type' => 'fieldset',
					'#title' => t('Building and room number table settings 
						for ' . $readable_et . ': ' . $readable_rt),
				);
				
				// is_available_building_table_1_space_meeting_resource
				$bldg_table = _is_available_append_nth('building_table', $nth_setting) . 
					'_' . $et . '_' . $rt; 
				$form[$key_building][$et_rt][$bldg_table] = array(
					'#type' => 'radios', 
					'#required' => true,
					'#title' => t('Select the building table for ' . 
						$readable_et . ', ' . $readable_rt),
					'#options' => $building_resource_number_options,
					'#default_value' => variable_get($bldg_table, 
						key($building_resource_number_options)), 
				);
				
				// is_available_resource_number_table_1_space_meeting_resource
				$rn_table = _is_available_append_nth('resource_number_table', $nth_setting) . '_' . $et . '_' . $rt; 
				$form[$key_building][$et_rt][$rn_table] = array(
					'#type' => 'radios', // vget
					'#title' => t('Select the room number table for ' . 
						$readable_et . ', ' . $readable_rt),
					'#options' => $building_resource_number_options,
					'#default_value' => variable_get($rn_table, key($building_resource_number_options)),
				);
				
				$object_resource_type = new IsAvailableResourceType($rt);
				$object_resource_type->_is_available_set_entity_type(new IsAvailableEntityTaipe($et));
				$object_resource_type->_is_available_set_form_fbld_key($bldg_table);
				$object_resource_type->_is_available_set_form_frn_key($rn_table);
				$array_resource_type[$rt] = $object_resource_type;	
			}
			$markup = substr($markup, 0, -2) . '<br>'; // trim off last ', '
		}
	}
	$markup .= '</b></p>';
 return $array_resource_type;
}

function _is_available_build_form_config_auth_buildings(&$form, $form_state, $form_object, $nth_setting, $array_resource_types, $bool_selected_brn_table) {
	$key_auth_bldg = $form_object->_is_available_get_key_config_auth_buildings();
	
	if (!$bool_selected_brn_table) {
		drupal_set_message('Please select the building and resource number 
			tables for each of the content types :: resource type combinations.', 
			'error');
		_is_available_build_form_building_button($form, $form_object, $key_auth_bldg);
		return;
	}
	
	if (empty($array_resource_types)) {
		drupal_set_message('Sorry, an error has occurred -- no selected resource types were detected. Please check your settings.', 'error');
		return;
	}
	
	$auth_bldg_wrapper = $form_object->_is_available_get_wrapper_auth_buildings();
	_is_available_build_form_generic_wrap($form, $key_auth_bldg, $auth_bldg_wrapper);
	
	$form[$key_auth_bldg]['description_auth_bldg'] = array(
		'#markup' => '<p id="step-4"><b>Step 4 of 7: Select searchable buildings and 
			content types for each resource type</b>.</p>',
		);
	
	//	The stored field_building and rn values:
	// $form_state['values']
	//		[is_available_resource_type_1_space_meeting_resource_building_table] 
	// 		= field_building
	// $form_state['values']
	//		[is_available_resource_type_1_space_meeting_resource_
	//			resource_number_table] 
	//		= field_resource_number
	foreach ($array_resource_types as $rt_machine_name => $object) {
		$entity_type			= $object->_is_available_get_entity_type();
		$et_label 				= $entity_type->_is_available_get_name();
		$et_machine_name 	= $entity_type->_is_available_get_machine_name();
		$rt_label					= $object->_is_available_get_title();
		
		$bldg_key		= $object->_is_available_get_form_fbld_key();
		$rn_key 		= $object->_is_available_get_form_frn_key();
		$bldg_field = $form_state['values'][$bldg_key];
		$rn_field		= $form_state['values'][$rn_key];
		$bldg_table = _is_available_get_field_table($bldg_key);
		$rn_table 	= _is_available_get_field_table($rn_key);
		
		$object->_is_available_set_field_building($bldg_table);
		$object->_is_available_set_field_room_number($rn_table);
		
		$temp_key_auth_bldg_et_rt = _is_available_append_nth('auth_building', 
			$nth_setting) . '_' . $et_machine_name . '_' . $rt_machine_name;
		
		// is_available_auth_building_1_space_meeting_resource
		$form[$key_auth_bldg][$temp_key_auth_bldg_et_rt] = array(
			'#type' 	=> 'fieldset',
			'#title' 	=> t('Select buildings to search over for ' . 
				$et_label . ' -> ' . $rt_label),
		);
		
		$buildings = 
			_is_available_get_building_names($bldg_field, $et_machine_name, $rt_machine_name);
		
		foreach ($buildings as $b_name) {
			// is_available_auth_building_1_space_meeting_resource_ICCS
			$form[$key_auth_bldg][$temp_key_auth_bldg_et_rt]
				[$temp_key_auth_bldg_et_rt. '_' . $b_name] = array(
				'#type' => 'checkbox',
				'#title' => t($b_name),
				'#default_value' => 
					variable_get($temp_key_auth_bldg_et_rt . '_' . $b_name, 1),
			);
		}
		
	}		
}

function _is_available_build_form_config_content_types(&$form, $form_state, $form_object, 
	$nth_setting, &$array_resource_types) {
	
	$key_content 	= $form_object->_is_available_get_key_config_auth_buildings(); 
	$key_ct 			= $form_object->_is_available_get_key_content_type();
	// form stage control done outside
	
	foreach ($array_resource_types as $rt_machine_name => $object) {
		$entity_type			= $object->_is_available_get_entity_type();
		$et_label 				= $entity_type->_is_available_get_name();
		$et_machine_name 	= $entity_type->_is_available_get_machine_name();
		$rt_label					= $object->_is_available_get_title();
		
		// is_available_content_type_space_meeting_resource_1
		$temp_key_et_rt = _is_available_append_nth($key_ct . '_' . 
			$et_machine_name . '_' . $rt_machine_name, $nth_setting);
		$form[$key_content][$temp_key_et_rt] = array(
			'#type' 	=> 'fieldset',
			'#title' 	=> $et_label . ': ' . $rt_label . ' content type settings',
			'#description' => 'Select booking types made on ' . $rt_label,
		);
		
		$content_type_options = _is_available_get_content_types();
		$i = 0;
		foreach ($content_type_options as $c) {
			$this_content_type = $temp_key_et_rt . '_' . $c;
			$form[$key_content][$temp_key_et_rt][$this_content_type] = array(
				'#type' => 'checkbox',
				'#title' => _is_available_convert_to_readable_name($c),
				'#default_value' => variable_get($this_content_type, $i == 0 ? 1 : 0),
			);
			$i++;
		}
	}
	_is_available_build_content_types_button($form, $form_object);
}


function _is_available_build_form_config_entref(&$form, $form_state, $form_object, 
	$nth_setting, $array_resource_types, $bool_has_checked_auth_bldgs) {
	
	$key_config_entref	= $form_object->_is_available_get_key_config_entref();
	$key_ct							= $form_object->_is_available_get_key_content_type();
	$key_entref					= $form_object->_is_available_get_key_entref();
	$entref_wrapper 		= $form_object->_is_available_get_wrapper_entref();
	$bool_build_entref_button = TRUE;
	
 	if (!$bool_has_checked_auth_bldgs) {
		drupal_set_message('Please select at least one building 
			for each resource type.', 'error');
		_is_available_build_content_types_button($form, $form_object, $key_config_entref);
		return;
	} 
	
	_is_available_build_form_generic_wrap($form, $key_config_entref, $entref_wrapper);
	$form[$key_config_entref]['description_entref'] = array(
		'#markup' => '<p id="step-5"><b>Step 5 of 7: 
			Select the entity reference table for each content type.</b></p>',
	);	
	
	// Empty (no content type selected) condition handled in foreach
	foreach ($array_resource_types as $rt_machine_name => $object) {
		$entity_type			= $object->_is_available_get_entity_type();
		$et_label 				= $entity_type->_is_available_get_name();
		$et_machine_name 	= $entity_type->_is_available_get_machine_name();
		$rt_label					= $object->_is_available_get_title();
		$et_rt 						= $et_machine_name . '_' . $rt_machine_name;
		$key_et_rt 				= $key_entref . '_' . $et_rt;
		
		$content_types = _is_available_get_checked_only($form_state, 
			$key_ct . '_' . $et_rt, $nth_setting);
		$object->_is_available_set_content_types($content_types); // Store in IsAvailableResourceType object
		
		if (empty($content_types)) {
			$bool_build_entref_button = FALSE;
			drupal_set_message('Please select at least one content type for ' . 
				$et_label . ': ' . $rt_label . '.', 'error');
			_is_available_build_content_types_button($form, $form_object, $key_config_entref);
		} else {
			$form[$key_config_entref][$key_et_rt] = array(
				'#type' => 'fieldset',
				'#title' => 'Entity reference table settings for ' . 
					$et_label . ': ' . $rt_label,
			);
			
			foreach ($content_types as $c => $value) {
				// is_available_entref_space_meeting_resource_event_1
				$leading_strlen = 
					_is_available_get_content_type_leading_strlen($form_object->_is_available_get_module_name(), 
						$et_machine_name, $rt_machine_name, $nth_setting);
				$ct = substr($c, $leading_strlen);
				
				$temp_key_et_rt_c = _is_available_append_nth($key_entref . '_' .
					$et_rt . '_' . $ct, $nth_setting);
				$readable_c = _is_available_convert_to_readable_name($ct);
				$entref_table_options = _is_available_get_entref_table_options($ct);
				
				$form[$key_config_entref][$key_et_rt][$temp_key_et_rt_c] = array(
				'#type' 	=> 'radios',
				'#required' => true,
				'#title' 	=> $readable_c . ' settings',
				'#description' => 'Select the entity reference table for ' . 
					$readable_c . ' bookings made on ' . $rt_label,
				'#options' => $entref_table_options,
				'#default_value' =>  variable_get($temp_key_et_rt_c, 
					($entref_table_options == null) ? 0 : key($entref_table_options)),
				);
			}
		}
	}
	_is_available_build_entref_button($form, $form_object, $bool_build_entref_button);
}


function _is_available_build_form_config_date(&$form, $form_state, $form_object, 
	$nth_setting, $array_resource_types, $bool__is_available_has_selected_entref) {
	
	$key_date = $form_object->_is_available_get_key_config_date();
	$key_entref	= $form_object->_is_available_get_key_entref();
	$date_wrapper = $form_object->_is_available_get_wrapper_date();

	if (!$bool__is_available_has_selected_entref) {
		drupal_set_message('Please select an entity reference table for each 
			resource type :: content type combination.', 'error');
		_is_available_build_entref_button($form, $form_object, true, $key_date);
		return;
	}

	_is_available_build_form_generic_wrap($form, $key_date, $date_wrapper);
	$form[$key_date]['description_date'] = array(
		'#markup' => '<p id="step-6"><b>Step 6 of 7: Select the date table for 
			each content type. </b></p>',
	);
	
	foreach($array_resource_types as $rt_machine_name => $object) {
		$entity_type			= $object->_is_available_get_entity_type();
		$et_label 				= $entity_type->_is_available_get_name();
		$et_machine_name 	= $entity_type->_is_available_get_machine_name();
		$rt_label					= $object->_is_available_get_title();
		// e.g. space_classroom
		$et_rt 						= $et_machine_name . '_' . $rt_machine_name; 
		$this_key 				= _is_available_append_nth('date_' . $et_rt, $nth_setting);
		$content_types 		= $object->_is_available_get_content_types();
		
		$form[$key_date][$this_key] = array(
			'#type' => 'fieldset',
			'#title' => 'Date table settings for ' . $et_label . ': ' . $rt_label,
			'#description' => 'Select the date table to use for 
				searching over content types on ' . $et_label . ', ' . $rt_label,
		);
	
		foreach ($content_types as $ct_form_key => $value) {
			$leading_strlen = 
				_is_available_get_content_type_leading_strlen($form_object->_is_available_get_module_name(),
				$et_machine_name, $rt_machine_name, $nth_setting);
			$ct = substr($ct_form_key, $leading_strlen);
			$date_options = _is_available_get_date_options($ct);
			$form[$key_date][$this_key][$this_key . '_' . $ct] = array(
				'#type' => 'radios',
				'#required' => true,
				'#title' => 'Date table for ' . _is_available_convert_to_readable_name($ct),
				'#options' => $date_options,
				'#default_value' => variable_get($this_key . '_' . $ct, 
					key($date_options)),
			);
		}
	}
	_is_available_build_date_button($form, $form_object);	
} 

function _is_available_build_sys_settings(&$form, $bool_date_selected) {
	if (!$bool_date_selected) {
		drupal_set_message('Please select a date table for each 
			entity type -> resource type -> content type combination.', 'error');
		return;
	}
	$form['actions']['description_save'] = array(
		'#markup' => '<p id="step-7"><b>Step 7 of 7: Save settings.</b></p>'
	);
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  if (!empty($_POST) && form_get_errors()) {
    drupal_set_message(t('The settings have not been saved because of 
			the errors.'), 'error');
  }
  $form['#submit'][] = 'system_settings_form_submit';
  // By default, render the form using theme_system_settings_form().
  if (!isset($form['#theme'])) {
    $form['#theme'] = 'system_settings_form';
  }
  return $form;
}

/**
 * ==============================
 * FORM BUTTON BUILDERS
 * ==============================
 */

 /**
 * array IsAvailableAdminSettingsForm -> (void)
 *
 * Build the respective buttons for the form.
 * @param $form	The form to add the button to
 * @param $form_object
 */
function _is_available_build_form_setting_buttons(&$form, $form_object, 	
	$int_sizeof_setting_options) {

	$count = $int_sizeof_setting_options; 
	$key_setting_options 	= $form_object->_is_available_get_key_setting_options();

	if ($count > 0) _is_available_build_form_select_setting_button($form, $form_object);
	_is_available_build_form_add_setting_button($form, $form_object);
	// If only one setting, do not load the delete button
	if ($count <= 1) return; 
		_is_available_build_form_delete_setting_button($form, $form_object);

}

function _is_available_build_form_select_setting_button(&$form, $form_object) {
	$key_config_settings = 
		$form_object->_is_available_get_key_only($form_object->_is_available__is_available_get_key_config_settings());
	$key_button = $form_object->_is_available_get_key_button();
	$setting_config_wrapper = $form_object->_is_available_get_wrapper_settings();
	$key_setting_options = $form_object->_is_available_get_key_setting_options();
	
	_is_available_build_form_generic_ajax_button($form, $key_setting_options, 
		$key_config_settings . $key_button, 
		'Configure', // _is_available_convert_to_readable_name($key_config_settings), 
		'ajax_config_setting_callback', 
		$setting_config_wrapper, 
		$setting_config_wrapper . '-button');
}

function _is_available_build_form_add_setting_button(&$form, $form_object) {
	$key_add_setting = $form_object->_is_available_get_key_add_setting();
	$key_button = $form_object->_is_available_get_key_button();
	$add_setting_wrapper = $form_object->_is_available_get_wrapper_add_setting();
	$key_setting_options = $form_object->_is_available_get_key_setting_options();
	
	$form[$key_setting_options][$key_add_setting . $key_button] = array(
		'#type' => 'submit',
		'#value' => t('Add New'), // _is_available_convert_to_readable_name($key_add_setting),
		'#validate' => array('is_available_form_add_setting_rebuild'),
		'#prefix' => '<div id="' . $add_setting_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}

function _is_available_build_form_delete_setting_button(&$form, $form_object) {
	$key_del_setting = $form_object->_is_available_get_key_delete_setting();
	$key_button = $form_object->_is_available_get_key_button();
	$del_setting_wrapper = $form_object->_is_available_get_wrapper_delete_setting();
	$key_setting_options = $form_object->_is_available_get_key_setting_options();
	
	$form[$key_setting_options][$key_del_setting . $key_button] = array(
		'#type' => 'submit',
		'#value' => t('Delete'), // _is_available_convert_to_readable_name($key_del_setting),
		'#validate' => array('is_available_form_delete_setting_rebuild'),
		'#prefix' => '<div id="' . $del_setting_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}

function _is_available_build_form_reset_button(&$form) {
	$form['reset'] = array(
		'#type' => 'submit',
		'#value' => 'Reset form',
		'#validate' => 'is_available_reset_form',
	);
}

function is_available_reset_form($form, $form_state) {
	$form_state['rebuild'] = TRUE;
}


// $array_entity_types = all the entity types displayed in the checkboxes
function _is_available_build_fsc_select_entity_type_button(&$form, $form_state, 
	$form_object, $array_entity_types, $key_config_settings) {
	
	$module_name = $form_object->_is_available_get_module_name();
	if ($key_config_settings  == "") 
		$key_config_settings = $form_object->_is_available__is_available_get_key_config_settings();
	$key_config_content 	= $form_object->_is_available_get_key_config_content();
	$resource_type_wrapper = $form_object->_is_available_get_wrapper_resource_type();
	$content_config_wrapper = $form_object->_is_available_get_wrapper_content();
	
	$key_config_resource_type = $form_object->_is_available_get_key_config_resource_type();
 	_is_available_build_form_generic_ajax_button($form, $key_config_settings, 
		$key_config_content,	t('Select entity types'), 
		'ajax_select_entity_types_callback', 
		$resource_type_wrapper, $resource_type_wrapper);
}

function _is_available_build_form_crt_select_button(&$form, $form_object, 
	$key_config_resource_type = "") {
	
	if ($key_config_resource_type == "")	
		$key_config_resource_type = $form_object->_is_available_get_key_config_resource_type();
	
	$building_wrapper = $form_object->_is_available_get_wrapper_building();
	$resource_type_wrapper = $form_object->_is_available_get_wrapper_resource_type();
	_is_available_build_form_generic_ajax_button($form, $key_config_resource_type, 
		'select_rt_button',	t('Select resource types'), 
		'ajax_select_resource_types_callback', $building_wrapper);
}

function _is_available_build_form_building_button(&$form, $form_object, $key_building = "") {
	if ($key_building == "") 
		$key_building = $form_object->_is_available_get_key_config_building();
	
	$building_wrapper = $form_object->_is_available_get_wrapper_building();
	$auth_bldg_wrapper = $form_object->_is_available_get_wrapper_auth_buildings();
	_is_available_build_form_generic_ajax_button($form, $key_building, 
		'building_button',	t('Select tables'), 
		'ajax_select_building_table_callback', $auth_bldg_wrapper);
}

function _is_available_build_content_types_button(&$form, $form_object, $key_content = "") {

	if ($key_content == "")	
		$key_content = $form_object->_is_available_get_key_config_auth_buildings(); 

	$entref_wrapper = $form_object->_is_available_get_wrapper_entref();
	$content_wrapper = $form_object->_is_available_get_wrapper_content();
	_is_available_build_form_generic_ajax_button($form, $key_content, 
		'content_type_button',	t('Select content types'), 
		'ajax_select_content_type_callback', $entref_wrapper);
} 

function _is_available_build_entref_button(&$form, $form_object, $bool_build_entref_button, 
	$key_entref = "") {
	
	if ($key_entref == "") 
		$key_entref = $form_object->_is_available_get_key_config_entref();
	
	if (!$bool_build_entref_button) {
		drupal_set_message('Please select at least one content type for 
			each entity type ->	resource type combination before proceeding 
			with this form.', 'error');
		return;
	}
	
	$entref_wrapper = $form_object->_is_available_get_wrapper_entref();
	$date_wrapper = $form_object->_is_available_get_wrapper_date();
	_is_available_build_form_generic_ajax_button($form, $key_entref, 
		'entref_button',	t('Select entity reference table'), 
		'ajax_select_entref_callback', $date_wrapper);
}
 
function _is_available_build_date_button(&$form, $form_object) {
	$key_date = $form_object->_is_available_get_key_config_date();
	$date_wrapper = $form_object->_is_available_get_wrapper_date();
	$save_config_wrapper = $form_object->_is_available_get_wrapper_save_config();
	_is_available_build_form_generic_ajax_button($form, $key_date, 
		'entref_button',	t('Select date table'), 
		'ajax_select_date_callback', $save_config_wrapper);
}

function _is_available_build_form_generic_ajax_button(&$form, $key1, $key2 = 0,
	$button_name, $ajax_callback, $target_wrapper, $button_wrapper = "") {
	if ($button_wrapper == "") $button_wrapper = $target_wrapper;
	$button_array = array(
		'#type' => 'button',
		'#value' => t($button_name),
		'#button_type' => 'submit ajax-trigger',
		'#ajax' => array(
			'submitter' => FALSE,
			'callback' => $ajax_callback,
			'event' => 'mousedown',
			'wrapper' => $target_wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div id="' . $button_wrapper . '">',
		'#suffix' => '</div>',
	);
	if(!$key2) $form[$key1] = $button_array;
	else $form[$key1][$key2] = $button_array;
}

/**
 * ==============================
 * FORM WRAPPERS
 * ==============================
 */
 
/**
 * array IsAvailableAdminSettingsForm -> (void)
 *
 * Wrap the settings form
 * @param $form	The form to build
 * @param $form_object
 */
function _is_available_build_form_wrap_config_settings(&$form, $form_object) {
	$key_config_settings = $form_object->_is_available__is_available_get_key_config_settings();
	$setting_config_wrapper = $form_object->_is_available_get_wrapper_settings();
	_is_available_build_form_generic_wrap($form, $key_config_settings, $setting_config_wrapper);
}

function _is_available_build_form_generic_wrap(&$form, $key, $wrapper) {
	$form[$key] = array(
		'#prefix' => '<div id="' . $wrapper . '">',
		'#suffix' => '</div>',
	);
}



/**
 * ================================================
 * AJAX CALLBACKS & VALIDATION HANDLERS
 * ================================================
 */

 function ajax_config_setting_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_config_settings = $form_object->_is_available__is_available_get_key_config_settings();
	
	unset($form_state['values'][$key_config_settings]);
	return $form[$key_config_settings];
}

function ajax_select_entity_types_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_config_resource_type = $form_object->_is_available_get_key_config_resource_type();
	$resource_type_wrapper = $form_object->_is_available_get_wrapper_resource_type();
	$key_config_settings = $form_object->_is_available__is_available_get_key_config_settings();
	
 	unset($form_state['values'][$key_config_resource_type]);
	return $form[$key_config_resource_type]; 
}

function ajax_select_resource_types_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_config_building = $form_object->_is_available_get_key_config_building();
	$key_config_resource_type = $form_object->_is_available_get_key_config_resource_type();
	$wrapper_building = $form_object->_is_available_get_wrapper_building();
	unset($form_state['values'][$key_config_building]);
	
	return $form[$key_config_building];
}

function ajax_select_building_table_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_auth_bldg = $form_object->_is_available_get_key_config_auth_buildings();

	unset($form_state['values'][$key_auth_bldg]);
	return $form[$key_auth_bldg];
}

function ajax_select_content_type_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_entref = $form_object->_is_available_get_key_config_entref();

	unset($form_state['values'][$key_entref]);
	return $form[$key_entref];
}

function ajax_select_entref_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_date = $form_object->_is_available_get_key_config_date();

	unset($form_state['values'][$key_date]);
	return $form[$key_date];
}

function ajax_select_date_callback(&$form, &$form_state) {
	unset($form_state['complete form']['actions']);
	return $form['actions'];
}

function is_available_form_add_setting_rebuild($form, $form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$module_name = $form_object->_is_available_get_module_name();
	$alphabet = '123456789';
	$count = _is_available_get_setting_multi_id_min_unused_id($module_name, $alphabet);
	_is_available_insert_into_variable_new_setting($count, $module_name);
	$form_state['rebuild'] = TRUE;
}

function is_available_form_delete_setting_rebuild(&$form, $form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_setting_options 	= $form_object->_is_available_get_key_setting_options();
	$bool_setting_selected = isset($form_state['values'][$key_setting_options]); 
	$nth_setting = $bool_setting_selected ? 
		$form_state['values'][$key_setting_options] : 
		'a';
	$module_name = $form_object->_is_available_get_module_name();
	
	global $conf;
	
	$settings_options = $form_object->_is_available_get_settings_options();
	$current_title = $settings_options[$nth_setting];
	$sid = _is_available_get_setting_multi_id_to_delete($current_title, $module_name);
	if (!is_string($sid)) return; // no setting to delete
	
	$name = $module_name . '_%' . $sid . '%';
	
	unset($form_state['values'][$key_setting_options]);
	
  db_delete('variable')->condition('name', $name, 'LIKE')->execute();
	$form_state['rebuild'] = TRUE; 
}

function _is_available_get_setting_multi_id_min_unused_id($module_name, $alphabet) {
	$setting_title = mysql_real_escape_string($module_name . "_settings_title_");
	$mysql_query = "SELECT name FROM variable WHERE name LIKE '$setting_title%'";
	$query = db_query($mysql_query);
	$i = 0;
	$id = "";
	foreach ($query as $q) {
		$id = substr($q->name, strlen($setting_title));

		if (substr($alphabet, $i, 1) != $id) return substr($alphabet, $i, 1);
		$i++;
	}
	return $id++;
}

function _is_available_get_setting_multi_id_to_delete($current_title, $module_name) {
	$setting_title = mysql_real_escape_string($module_name . "_settings_title_");
	$mysql_query = "SELECT name FROM variable WHERE name LIKE '$setting_title%'";
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		
		if ($current_title == variable_get($q->name, "")) 
			return substr($q->name, strlen($setting_title));
	}
	return -1;
}

/**
 * ================================================
 * HELPERS
 * ================================================
 */

/**
 * Integer IsAvailableAdminSettingsForm -> (void)
 * Insert a new setting title into the variable array.
 */
function _is_available_insert_into_variable_new_setting($count, $module_name) {
	$setting_title = $module_name . '_settings_title_' . $count;
	if (trim(variable_get($setting_title, "")) == "") {
		variable_set($setting_title, 'Setting ' . 
			strtoupper($count) . ' (default)');
		return;
	}

	do {
		$count++;
		$setting_title = $module_name . '_settings_title_' . $count;
	} while (variable_get($setting_title, "") != ""); 

	variable_set($setting_title, 'Setting ' . 
		strtoupper($count) . ' (default)');
}

/**
 * ================================================
 * HELPERS -- VALUE GETTERS
 * ================================================
 */

 /**
 *
 * e.g. array('space' => array('meeting_resource, ...))
 */
function _is_available_get_checked_et_rt(&$form_state, $checked_entity_types, 
	$nth_setting = -1) {
	
	$ret_array = array();
	$form_object = new IsAvailableAdminSettingsForm;
	$mod_name = $form_object->_is_available_get_module_name();
	foreach ($checked_entity_types as $et_key => $et_value) {
		// is_available_entity_type_1_space
		// is_available_resource_type_1_space_classresource
		$leading_et_strlen = _is_available_get_leading_et_strlen($mod_name, $nth_setting);
		$et = substr($et_key, $leading_et_strlen);
		$leading_rt_strlen = _is_available_get_leading_rt_strlen($mod_name, $et, $nth_setting);
		$et_rt_array = array();
		$array_checked_rt = _is_available_get_checked_only($form_state, 
			'resource_type', $nth_setting);
		
		foreach ($array_checked_rt as $rt_key => $rt_value) {
			array_push($et_rt_array, substr($rt_key, $leading_rt_strlen));
		}
		$ret_array[$et] = $et_rt_array;
	}
	return $ret_array;
}

function _is_available_get_entity_types() {
	$entity_types = array();
	$mysql_query1 = "SELECT name, label FROM eck_entity_type";
	$query1 = db_query($mysql_query1);
	if (empty($query1)) return $entity_types;
	
	foreach ($query1 as $q) {
		$title_column_array = array();
		$mysql_query2 = "SHOW columns from eck_" . $q->name . 
			" WHERE field != 'id' AND field != 'type' AND type like '%char%'";
		$query2 = db_query($mysql_query2);
		foreach ($query2 as $q2) {
			array_push($title_column_array, $q2->Field);
		}
		// $entity_types['space'] = IsAvailablePair(IsAvailableEntityTaipe('space', 
		//		'Space'), array('title', ...))
		$entity_types[$q->name] = new IsAvailablePair(
			new IsAvailableEntityTaipe($q->name, $q->label), $title_column_array);
	}
	return $entity_types;
}

function _is_available_get_content_types() {
	$ret_array = array();
	$mysql_query = "SELECT bundle FROM field_config FC INNER JOIN 
		field_config_instance FCI
		ON FC.field_name = FCI.field_name WHERE module = 'date'";
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		$ret_array[$q->bundle] = $q->bundle;
	}
	return $ret_array;
}

function _is_available_get_building_names($bldg_field, $et_machine_name, $rt_machine_name) {
	$ret_array = array(); // array['ICCS'] = 'ICCS'
	
	$bldg_table 			= mysql_real_escape_string(_is_available_get_field_table($bldg_field));
	$bldg_col 				= mysql_real_escape_string(_is_available_get_field_column($bldg_field));
	$et_machine_name	= mysql_real_escape_string($et_machine_name);
	$rt_machine_name	= mysql_real_escape_string($rt_machine_name);
	
	$mysql_query = "SELECT DISTINCT(" . $bldg_col . ") AS value FROM " . 
		$bldg_table . " WHERE entity_type = '" . $et_machine_name . 
		"' AND bundle = '" . $rt_machine_name . "'";
	$query = db_query($mysql_query);
	
	foreach ($query as $q) {
		$ret_array[$q->value] = $q->value;
	}
	
	return $ret_array;
}

function _is_available_get_checked_auth_buildings($form_state, $form_object, 
	$checked_et_rt, $nth_setting) {
	
	// array['space_meeting_resource'] = _is_available_get_checked_only...
	$ret_array = array(); 
	foreach ($checked_et_rt as $et => $rt_array) {
		// is_available_auth_building_1_space_meeting_resource_ICCS 
		foreach ($rt_array as $rt) {
			// auth_building_1_space_meeting_resource
			$term = $form_object->_is_available_get_key_auth_buildings() . 
				'_' . $nth_setting . '_' . $et . '_' . $rt;
			$ret_array[$et . '_' . $rt] =	_is_available_get_checked_only($form_state, $term);
		}
	}
	return $ret_array;
}

function _is_available_get_building_table_options($entity_type, $resource_type) {
	$ret_array = array();
	$entity_type = mysql_real_escape_string($entity_type);
	$resource_type = mysql_real_escape_string($resource_type);
	$mysql_query = 
		"SELECT field_name 
		FROM field_config_instance 
		WHERE entity_type = '$entity_type' 
		AND bundle = '$resource_type'";
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		$ret_array[$q->field_name] = $q->field_name;
	}
	return $ret_array;
}

function _is_available_get_entity_type_resource_types($entity_type) {
	$ret_array = array();	
	if ($entity_type == '') return $ret_array;
	$entity_type = mysql_real_escape_string($entity_type);
	$query = db_query(
		"SELECT DISTINCT(name), label 
		FROM eck_bundle EB
		INNER JOIN eck_" . $entity_type . " ET 
		ON EB.name = ET.type");
	foreach ($query as $q) {
		$ret_array[$q->name] = $q->label;
	}
	return $ret_array;
}

function _is_available_get_entref_table_options($content_type_machine_name) {
	$ret_array = array();
	$ct = mysql_real_escape_string($content_type_machine_name);
	$mysql_query = "SELECT fci.field_name, bundle 
		FROM field_config fc 
		INNER JOIN field_config_instance fci 
			ON fc.field_name = fci.field_name 
		WHERE bundle IN (
			SELECT bundle 
			FROM field_config fc2 
			INNER JOIN field_config_instance fci2 
				ON fc2.field_name = fci2.field_name
			WHERE fc2.module = 'date') 
		AND fc.module = 'entityreference'
		AND bundle = '$ct'";
	
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		$ret_array[$q->field_name] = $q->field_name;
	}
	return $ret_array;
}

function _is_available_get_date_options($content_type) {
	$ret_array = array();
	$content_type = mysql_real_escape_string($content_type);
	$mysql_query = "SELECT FCI.field_name FROM field_config_instance FCI
		INNER JOIN field_config FC ON FCI.field_name = FC.field_name
		WHERE bundle = '$content_type' AND FC.module = 'date'";
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		$ret_array[$q->field_name] = $q->field_name;
	}
	return $ret_array;
}


function _is_available_get_form_stage_n_element_array($form_state_values, $nth_setting) {
	if (empty($form_state_values)) $form_state_values = array();
	$form_object 	= new IsAvailableAdminSettingsForm;
	$mod_name 		= $form_object->_is_available_get_module_name(); 
	// settings_options
	$key_setting_options 	= $form_object->_is_available_get_key_setting_options(); 
	
	// is_available_entity_type_1_||space
	$key_entity_type = 
		_is_available_append_nth('entity_type', $nth_setting) . '_'; 
	
	// is_available_resource_type_1_||space_meeting_resource
	$key_resource_type = 
		_is_available_append_nth($form_object->_is_available_get_key_resource_type(), $nth_setting) . '_';
	
	// is_available_building_table_1_||space_classresource
	$key_bldg_table	= 
		_is_available_append_nth($form_object->_is_available_get_key_building_table(),  $nth_setting) . '_';
	
	// is_available_resource_number_table_1_||space_classresource
	$key_rn_table	= 
		_is_available_append_nth($form_object->_is_available_get_key_resource_number_table(), 
		$nth_setting) . '_';
	
	// is_available_auth_building_1_space_meeting_resource_ICCS
	$key_auth_building = 
		_is_available_append_nth($form_object->_is_available_get_key_auth_buildings(), $nth_setting) . '_';
		
	// is_available_content_type_space_meeting_resource_1_event
	$key_content_type = $mod_name . '_content_type_';
	
	// is_available_entref_space_meeting_resource_classresource_1 
	$key_entref	= $mod_name . '_entref_'; 
	
	// is_available_date_space_meeting_resource_classresource_1 
	$key_date	= $mod_name . '_date_'; 
	
	// rest of the keys are submit, form_build_id, form_token, form_id, and op
	// number of elements in $form_state['values'] at the given stage in the form

	$a = 8;
	$b = ($a + _is_available_array_count_key_start_with($form_state_values, 
		$key_entity_type, $a - 5) + 2); // +1 for the title
	$c = ($b + _is_available_array_count_key_start_with($form_state_values, 
		$key_resource_type, $b - 5) + 1); // +1 for the button element
	$d = ($c + _is_available_array_count_key_start_with($form_state_values, 
		$key_bldg_table, $c - 5) + 
		_is_available_array_count_key_start_with($form_state_values, $key_rn_table, $c - 5) + 1);
	$e = ($d + _is_available_array_count_key_start_with($form_state_values, 
		$key_auth_building, $d - 5) + 1);
	$f = ($e + _is_available_array_count_key_start_with($form_state_values, 
		$key_content_type, $e - 5) + 1);
	$g = ($f + _is_available_array_count_key_start_with($form_state_values, 
		$key_entref, $f - 5) + 1);
	$h = ($g + _is_available_array_count_key_start_with($form_state_values, 
		$key_date, $g - 5) + 1);
	
	$ret_array = array(
		$key_setting_options 										=> $a, 
		$form_object->_is_available_get_key_entity_type()			=> $b, 
		$form_object->_is_available_get_key_resource_type()		=> $c,
		$form_object->_is_available_get_key_brn_table()				=> $d, 
		$form_object->_is_available_get_key_auth_buildings()	=> $e,
		$form_object->_is_available_get_key_content_type()	 	=> $f, 
		$form_object->_is_available_get_key_entref()					=> $g,
		$form_object->_is_available_get_key_date() 						=> $h,
	);
	return $ret_array;
}

function _is_available_get_checkboxes_value($form_state, $string, $n = -1) {
	$ret_array = array();
	$form_object = new IsAvailableAdminSettingsForm;
	$term = '_' . $string . '_';
	$term .= ($n != -1) ? $n . '_' : '';
	$term_string_length = strlen($term);
	
	$mod_name = $form_object->_is_available_get_module_name();
	if (empty($form_state['values'])) return $ret_array;
	foreach ($form_state['values'] as $k => $f) {
		if (substr($k, 0, strlen($mod_name) + 
			$term_string_length) === $mod_name . $term) {
			$ret_array[$k] = $f;
		}
	}
	return $ret_array;
}

function _is_available_get_checked_only(&$form_state, $string, $n = -1) {
	return array_filter(_is_available_get_checkboxes_value($form_state, $string, $n), 
		'_is_available_is_checked');
}

function _is_available_get_leading_et_strlen($mod_name, $nth_setting) { 
	return strlen($mod_name) + strlen('_entity_type_') 
		+ strlen($nth_setting . "_");
}

function _is_available_get_leading_rt_strlen($mod_name, $entity_type_machine_name, 
	$nth_setting) {
	
	return strlen($mod_name) + strlen('_resource_type_') + 
		strlen($nth_setting . "_") + 
		strlen($entity_type_machine_name) + 1;
}
function _is_available_get_content_type_leading_strlen($module_name, $et_machine_name, 
	$rt_machine_name, $nth_setting) { 
	
	// e.g. is_available_content_type_space_meeting_resource_1_classresource, return string length of
	// everything up to classresource (the content type's machine name)
	return strlen($module_name) + 
		strlen('_content_type_') + 
		strlen($et_machine_name) + 1 + 
		strlen($rt_machine_name) + 
		strlen("_" . $nth_setting . "_");
}

/**
 * ================================================
 * HELPERS -- BOOLEAN GETTERS
 * ================================================
 */

/**
 * array (reg) -> Boolean
 * True if array has 1 (has checked value)
 */
function _is_available_has_checked($array) {
	return !empty($array) && in_array(1, $array);
}

/**
 *
 * Returns true if at least one x for eaach resource type is checked.
 * @param $checked_key_maps_array array('key' => array()) inner array 
 * is checked values only
 */
function _is_available_has_checked_for_each_rt($checked_key_maps_array) {
	if (empty($checked_key_maps_array)) return FALSE;
	
	$has_checked = TRUE;
	foreach ($checked_key_maps_array as $key => $array_value) {
		$has_checked = $has_checked && (sizeof($array_value) >= 1);
	}
	return $has_checked;
}

function _is_available_has_checked_entity_type($checked_entity_types) {
	return sizeof($checked_entity_types) >= 1;
}

function _is_available_has_checked_resource_type($checked_et_rt) {
	$each_has_checked = TRUE;
	foreach ($checked_et_rt as $et => $rt_array) {
		$each_has_checked = $each_has_checked && (sizeof($rt_array) >= 1);
	}
	return $each_has_checked;
}

/**
 * Checks to see if every entity_type::resource_type::content_type combo has an
 * entity reference table selected.
 */
function _is_available_has_selected_brn_table($form_state_values) {
	return has_selected_radios($form_state_values, 'select_rt_button', 
		'building_button');
}
	
function _is_available_has_selected_entref($form_state_values) {
	return has_selected_radios($form_state_values, 'content_type_button', 
		'entref_button');
}

function _is_available_has_selected_date($form_state_values) {
	return has_selected_radios($form_state_values, 'entref_button', 
		'date_button');
}

function has_selected_radios($form_state_values, $start_str_index, 
	$end_str_index) {
	
	$keys = array_keys($form_state_values);
	// e.g. $form_state_values['content_type_button'] - 1;
	$start_index = array_search($start_str_index, $keys) + 1; 
	
	$end_index = array_search($end_str_index, $keys);
	$bool_has_selected = TRUE;
	
	for ($i = $start_index; $i < $end_index; $i++) {
		$bool_has_selected = $bool_has_selected && 
			($form_state_values[$keys[$i]] != null);
	}
	return $bool_has_selected;
}


function _is_available_at_this_stage($array_fsne, $key1, $key2) {
	return $array_fsne[$key1] < $array_fsne[$key2] - 1;
}

function _is_available_is_checked($item) { return $item == 1; }

function _is_available_is_at_entity_type($form_object, $array_fsne) {
	$key_setting_options 	= $form_object->_is_available_get_key_setting_options();
	$key_entity_type 			= $form_object->_is_available_get_key_entity_type();
	return $array_fsne[$key_setting_options] < $array_fsne[$key_entity_type] - 2;
}

function _is_available_is_at_resource_type($form_object, $array_fsne) {
	return _is_available_at_this_stage($array_fsne, $form_object->_is_available_get_key_entity_type(), 
		$form_object->_is_available_get_key_resource_type());
}

function _is_available_is_at_brn_table($form_object, $array_fsne) { 
	return _is_available_at_this_stage($array_fsne, $form_object->_is_available_get_key_resource_type(), 
		$form_object->_is_available_get_key_brn_table());
}

function _is_available_is_at_auth_buildings($form_object, $array_fsne) {
	return _is_available_at_this_stage($array_fsne, $form_object->_is_available_get_key_brn_table(), 
		$form_object->_is_available_get_key_auth_buildings());
}

function _is_available_is_at_content_type($form_object, $array_fsne) {
	return _is_available_at_this_stage($array_fsne, $form_object->_is_available_get_key_auth_buildings(), 
		$form_object->_is_available_get_key_content_type());
}

function _is_available_is_at_entref($form_object, $array_fsne) {
	return _is_available_at_this_stage($array_fsne, $form_object->_is_available_get_key_content_type(), 
		$form_object->_is_available_get_key_entref());
}

function _is_available_is_at_date($form_object, $array_fsne) {
	return _is_available_at_this_stage($array_fsne, $form_object->_is_available_get_key_entref(), 
		$form_object->_is_available_get_key_date());
}

function is_setting_selected(&$form_state, $key_setting_options) {
	return isset($form_state['values'][$key_setting_options]);
}

function is_cb_checked(&$form_state, $option) {
	return ($form_state['values'][$options] == 1);
}



/**
 * ==========================================
 * OTHER HELPERS
 * ==========================================
 */

/**
 * String Integer -> String
 * @param $string
 * @param $n
 *
 * @return 
 */
function _is_available_append_nth($string, $n) {
	$form_object = new IsAvailableAdminSettingsForm;
	return $form_object->_is_available_get_module_name() . '_' . $string . '_' . $n;
}

