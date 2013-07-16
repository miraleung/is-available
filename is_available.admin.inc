<?php

function is_available_settings($form, &$form_state, $module_name = 'is_available') {
	
	// Constants		
	// // Objects & Arrays
	// // // Initialize and set up $form_object
	$form_object = new IsAvailableAdminSettingsForm;
	$form_object->set_module_name($module_name);	
	
	// // // Array of entity types (EntityTaipe objects) to search
	$array_entity_types = array();  
	
	// // // Array of ResourceType objects to search
	$array_resource_types = array();
	
	// // // Form object variables
	$key_setting_options 	= $form_object->get_key_setting_options();
	$key_config_content 	= $form_object->get_wrapper_content();
	
	// // Booleans
	$bool_setting_selected = isset($form_state['values'][$key_setting_options]); 
	
	// // Integers / Values
	$nth_setting = $bool_setting_selected ? $form_state['values'][$key_setting_options] : -1;
	
	// Build the form
	// // Build the setting options and buttons
	$int_sizeof_setting_options = build_form_setting_options($form, $form_object);
	build_form_setting_buttons($form, $form_object, $int_sizeof_setting_options);

	// // Wrap the ajax callback sections of thesettings form
	build_form_wrappers($form, $form_object);
	
	$array_entity_types = get_checkboxes_value($form_state, 'entity_type');
	
	// array_form_stage_n_elements -- the number of form elements at a given stage
	$array_fsne = get_form_stage_n_element_array($form_state['values'], $nth_setting);
	
	$checked_entity_types = get_checked_only($form_state, 'entity_type', $nth_setting);
	$checked_et_rt = get_checked_et_rt($form_state, $checked_entity_types, $nth_setting);
	
	// Form has 8 steps
	$array_form_stage = array(
		'is_at_entity_type'			=> is_at_entity_type($form_object, $array_fsne), 
		'is_at_resource_type' 			=> is_at_resource_type($form_object, $array_fsne),
		'is_at_brn_table'				=> is_at_brn_table($form_object, $array_fsne), 
		'is_at_auth_buildings'	=> is_at_auth_buildings($form_object, $array_fsne),
		'is_at_content_type'		=> is_at_content_type($form_object, $array_fsne),
		'is_at_entref'					=> is_at_entref($form_object, $array_fsne),
		'is_at_date'						=> is_at_date($form_object, $array_fsne),
	);
	
	// // Build the configuration form for the chosen setting
	build_form_setting_configuration($form, $form_state, $form_object, 
		$nth_setting, $bool_setting_selected, $array_entity_types);
	
	if ($array_form_stage['is_at_entity_type']) {
		$bool_checked_et = has_checked_entity_type($checked_entity_types);
		build_form_config_resource_type($form, 
			$form_state, $form_object, $nth_setting, $array_entity_types, $bool_checked_et);
		
		if ($array_form_stage['is_at_resource_type']) {
		$bool_checked_rt = has_checked_resource_type($checked_et_rt);
		$array_resource_types = build_form_config_building_table($form, 
			$form_state, $form_object, $nth_setting, $checked_et_rt, $bool_checked_rt);
		}
		
		if ($array_form_stage['is_at_brn_table']) {
			$bool_selected_brn_table = has_selected_brn_table($form_state['values']);
			build_form_config_auth_buildings($form, $form_state, $form_object, $nth_setting, $array_resource_types, $bool_selected_brn_table);
		}
		
		if ($array_form_stage['is_at_auth_buildings']) {
			$checked_auth_buildings = get_checked_auth_buildings($form_state, $form_object, $checked_et_rt, $nth_setting);
			$bool_has_checked_auth_bldgs = has_checked_for_each_rt($checked_auth_buildings);
			build_form_config_content_types($form, 
				$form_state, $form_object, $nth_setting, $array_resource_types, $bool_has_checked_auth_bldgs);
		}
		
		
		if ($array_form_stage['is_at_content_type']) {
			build_form_config_entref($form, $form_state, $form_object, $nth_setting, $array_resource_types);
		}
	
		if ($array_form_stage['is_at_entref']) {
			$bool_has_selected_entref = has_selected_entref($form_state['values']); 
			build_form_config_date($form, $form_state, $form_object, $nth_setting, $array_resource_types, $bool_has_selected_entref);
		}
		
		if ($array_form_stage['is_at_date']) { 
			$bool_selected_date = has_selected_date($form_state['values']);
			build_sys_settings($form, $bool_selected_date);
		}
	} 
	// system_settings_form($form) is taken care of in build_sys_settings
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
function build_form_setting_options(&$form, $form_object) {
	$key_setting_options = $form_object->get_key_setting_options();
	$array_settings_options = $form_object->get_settings_options();

	$form[$key_setting_options]  = array(
		'#type' => 'radios',
		'#title' => convert_to_readable_name($key_setting_options),
		'#options' => $array_settings_options,
		'#default_value' => key($array_settings_options),
	);
	
	return sizeof($array_settings_options);
}

/**
 * array IsAvailableAdminSettingsForm string -> (void)
 *
 * Build the "modify setting" button
 * @param $form	The form to build
 * @param $form_object
 */

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
function build_form_setting_configuration(&$form, &$form_state, $form_object, 
	$nth_setting, $bool_setting_selected, &$array_entity_types) {
	
	$module_name = $form_object->get_module_name();
	$key_config_settings = $form_object->get_key_config_settings();
	$key_setting_options = $form_object->get_key_setting_options();

	if (!$bool_setting_selected || empty($bool_setting_selected)) { // If no setting is selected, don't build the form
		$form[$key_config_settings][append_nth('original_title', $nth_setting)] = array(
			'#markup' => '<p>Please select a setting option to configure.</p>',
		);
		return;
	} 
	
	// Build the form for real
	build_fsc_original_title($form, $form_object, $nth_setting); 
	build_fsc_title_textfield($form, $form_object, $nth_setting);
	$temp_int = build_fsc_entity_types($form, $form_object, $nth_setting);

	// If no entity types, return
	if ($temp_int == 0) {
		$form[$key_config_settings]['no_entity_types'] = array(
			'#markup'	=> 'Sorry, no searchable entity types found. 
										Please check your entity type configurations.',
		);	
		return;
	}
	build_fsc_select_entity_type_button($form, $form_state, $form_object, $array_entity_types);
	return;
}


function build_fsc_original_title(&$form, $form_object, $nth_setting) {
	$module_name = $form_object->get_module_name();
	$key_config_settings = $form_object->get_key_config_settings();
	$key_setting_options = $form_object->get_key_setting_options();
	
	$form[$key_config_settings][append_nth('original_title', $nth_setting)] = 
		array(
			'#markup' => '<p><b>Setting configuration for ' . 
			variable_get(append_nth('settings_title', $nth_setting), 
				$form[$key_setting_options]['#options'][$nth_setting]) .
			'</b></p>',
		);		
}

function build_fsc_title_textfield(&$form, $form_object, $nth_setting) {
	$key_config_settings = $form_object->get_key_config_settings();
	$key_setting_options = $form_object->get_key_setting_options();

	$form[$key_config_settings][append_nth('settings_title', $nth_setting)] = array(
		'#type' => 'textfield',
		'#title' => 'Settings title',
		'#default_value' => variable_get(append_nth('settings_title', $nth_setting), 
			$form[$key_setting_options]['#options'][$nth_setting]),
		'#required' => TRUE,
	);
}

function build_fsc_entity_types(&$form, $form_object, $nth_setting) {
	$module_name = $form_object->get_module_name();
	$key_config_settings = $form_object->get_key_config_settings();
	$key_setting_options = $form_object->get_key_setting_options();

	// $entity_types['space'] = Pair(EntityTaipe('space', 'Space'), array('title', ...))
	$entity_type_pairs = get_entity_types();
	$entity_type_names = array();
	if (empty($entity_type_pairs)) {
		return 0;
	}
	
	foreach ($entity_type_pairs as $machine_name => $pair) {
		$entity_type = $pair->get_first();
		$title_array = $pair->get_second();
		$entity_type_names[$machine_name] = $entity_type->get_name();
	}
	
	$form[$key_config_settings]['markup_entity_type'] = array(
		'#markup' => '<p><b>Entity types to search</b></p>',
	);
	
	// Setup entity type selection checkboxes
	foreach ($entity_type_names as $machine_name => $name) {
		$form[$key_config_settings][append_nth('entity_type', $nth_setting) . '_' . $machine_name] = array(
			'#type' => 'checkbox',
			'#title' => t($name),
			'#default_value' => variable_get(append_nth('entity_type', $nth_setting) . '_' . $machine_name, 0),
		);
	}
	return 1;
}

function build_form_config_resource_type(&$form, $form_state, $form_object, $nth_setting, $array_entity_types, $bool_checked_et) {
	$key_config_resource_type = $form_object->get_key_config_resource_type();
	$resource_type_wrapper = $form_object->get_wrapper_resource_type();

	$int_ret = build_form_crt_checkboxes($form, $form_state, $form_object, $nth_setting, $array_entity_types, $bool_checked_et);
	
	if ($int_ret == 1) {
		build_form_crt_select_button($form, $form_object);
	}
	return $int_ret;
}

function build_form_crt_checkboxes(&$form, $form_state, 
	$form_object, $nth_setting, $array_entity_types, $bool_checked_et) {
	$key_config_resource_type = $form_object->get_key_config_resource_type();
	
	if (!$bool_checked_et) {
		$form[$key_config_resource_type]['resource_type'] = array(
			'#markup' => '<p>Please select an entity type for searching.</p>'
		);
		return 0;
	} else {
		// Build resource checkboxes
		$string_start = append_nth('entity_type', $nth_setting); // plus _space
		
		$string_start_length = (strlen($string_start) + 1);
		$form[$key_config_resource_type]['resource_type'] = array(
			'#markup' => '<p><b>Selected entity types: ',
		);
		$markup = &$form[$key_config_resource_type]['resource_type']['#markup'];
		
		foreach ($array_entity_types as $et => $value) {
			if ($value == 1) { // Only for checked-off entities
				$entity_type = substr($et, $string_start_length);
				
				$readable_et = convert_to_readable_name(substr($et, $string_start_length));
				$array_resource_types = get_entity_type_resource_types($entity_type);
				$markup .= $readable_et . ', ';
				
				if (sizeof($array_resource_types) == 0) {
					$form[$key_config_resource_type][$et . '_resource_type'] = array(
						'#markup' => 'Sorry, no searchable resource types available for entity type ' . $readable_et .
							'. Please check the configurations for ' . $readable_et . '.',
					);
				} else {
					$form[$key_config_resource_type][$et . '_resource_type'] = array(
						'#type' => 'fieldset',
						'#title' => $readable_et . ' Resource Types',
					);
					foreach ($array_resource_types as $machine_name => $label) {
		
						$form[$key_config_resource_type][$et . '_resource_type']
							[append_nth('resource_type', $nth_setting) . '_' . $entity_type . '_' . $machine_name] = array(
								'#type' => 'checkbox',
								'#title' => $label,
								'#default_value' => 
									variable_get(append_nth('resource_type', $nth_setting) . '_' . $entity_type . '_' . $machine_name, 1),	
						);
					}
				}
			}
		}
		$markup = substr($markup, 0, -2) . '</b></p>'; // trim off last ', '.
		return 1;
	}
}

function build_form_config_building_table(&$form, $form_state, $form_object, $nth_setting, $checked_et_rt, $bool_checked_rt) {
	
	if (!$bool_checked_rt) {
		$form[$form_object->get_key_config_building()]['error'] = array(
			'#markup' => 'Please select at least one resource type for each entity type.',
		);
		return array();
	} else {
	
	$array_resource_type = build_form_building_options($form, $form_object, $nth_setting, $checked_et_rt); /// $array_et_rt);
	build_form_building_button($form, $form_object);
	return $array_resource_type;
	}
}

function build_form_building_options(&$form, $form_object, $nth_setting, $checked_et_rt) { 
	$key_building = $form_object->get_key_config_building();
	$str_start_et = $form_object->get_module_name() . '_entity_type_'; // + n_<entity_type>
	$str_start_rt = $form_object->get_module_name() . '_resource_type_'; // + n_<entity_type>_<resource_type>
	$str_start_et_len = strlen($str_start_et) + 2;
	$str_start_rt_len = strlen($str_start_rt) + 2;
	
	$array_resource_type = array();
	if (empty($checked_et_rt)) {
		$form[$key_building]['no_et'] = array(
			'#markup' => 'Sorry, an error has occurred -- 
				no selected entity types or resource types were detected. Please check your settings.',
		);
	} else {
		// <mod_name>_entity_type_m_<entity_type> => array
		$form[$key_building]['selected_et_rt'] = array(
			'#markup' => '<p><b>Selected resource types for each entity type: <br>',
		);
		$markup = &$form[$key_building]['selected_et_rt']['#markup'];
		foreach ($checked_et_rt as $et => $array_rt) {
			// <mod_name>_resource_type_n_<entity_type>_<resource_type>
			$readable_et = convert_to_readable_name($et);
			$markup .= $readable_et . ': ';
			foreach ($array_rt as $rt) {
				$readable_rt = convert_to_readable_name($rt);
				$markup .= $readable_rt . ', ';
				$et_rt = $et . '_' . $rt;
				
				// <entity_type>_<resource_type>_<table_name> => <table_name>
				$building_resource_number_options = get_building_table_options($et, $rt);
	
				// <module_name>_resource_type_n_<entity_type>_<resource_type>_building_table
				// e.g. is_available_resource_type_1_space_classresource_building_table
				
				$form[$key_building][$et_rt] = array(
					'#type' => 'fieldset',
					'#title' => t('Building and room number table settings 
						for ' . $readable_et . ': ' . $readable_rt),
				);
				
				// is_available_building_table_1_space_meeting_resource
				$bldg_table = append_nth('building_table', $nth_setting) . '_' . $et . '_' . $rt; 
				$form[$key_building][$et_rt][$bldg_table] = array(
					'#type' => 'radios', 
					'#title' => 'Select the building table for ' . $readable_et . ', ' . $readable_rt,
					'#options' => $building_resource_number_options,
					'#default_value' => variable_get($bldg_table, key($building_resource_number_options)),
				);
				
				// is_available_resource_number_table_1_space_meeting_resource
				$rn_table = append_nth('resource_number_table', $nth_setting) . '_' . $et . '_' . $rt; 
				$form[$key_building][$et_rt][$rn_table] = array(
					'#type' => 'radios', // vget
					'#title' => 'Select the room number table for ' . $readable_et . ', ' . $readable_rt,
					'#options' => $building_resource_number_options,
					'#default_value' => variable_get($rn_table, key($building_resource_number_options)),
				);
				
				$object_resource_type = new ResourceType($rt);
				$object_resource_type->set_entity_type(new EntityTaipe($et));
				$object_resource_type->set_form_fbld_key($bldg_table);
				$object_resource_type->set_form_frn_key($rn_table);
				$array_resource_type[$rt] = $object_resource_type;	
			}
			$markup = substr($markup, 0, -2) . '<br>'; // trim off last ', '
		}
	}
	$markup .= '</b></p>';
 return $array_resource_type;
}

function build_form_config_auth_buildings(&$form, $form_state, $form_object, $nth_setting, $array_resource_types, $bool_selected_brn_table) {
	$key_auth_bldg = $form_object->get_key_config_auth_buildings();
	
	if (!$bool_selected_brn_table) {
		$form[$key_auth_bldg]['no_brn_table'] = array(
			'#markup' => '<p>Please select the 
				building and resource number tables for each of the content types :: resource type combinations.</p>',
		);
		return;
	}
	
	if (empty($array_resource_types)) {
		$form[$key_auth_bldg]['no_brn_table'] = array(
			'#markup' => '<p>Sorry, an error has occurred -- no selected resource types were detected. Please check your settings.</p>'
		);
		return;
	}

	//	The stored field_building and rn values:
	// $form_state['values'][is_available_resource_type_1_space_meeting_resource_building_table] = field_building
	// $form_state['values'][is_available_resource_type_1_space_meeting_resource_resource_number_table] = field_resource_number
	foreach ($array_resource_types as $rt_machine_name => $object) {
		$entity_type			= $object->get_entity_type();
		$et_label 				= $entity_type->get_name();
		$et_machine_name 	= $entity_type->get_machine_name();
		$rt_label					= $object->get_title();
		
		$bldg_key		= $object->get_form_fbld_key();
		$rn_key 		= $object->get_form_frn_key();
		$bldg_field = $form_state['values'][$bldg_key];
		$rn_field		= $form_state['values'][$rn_key];
		
		$object->set_field_building($bldg_table);
		$object->set_field_room_number($rn_table);
		
		$temp_key_auth_bldg_et_rt = append_nth('auth_building', 
			$nth_setting) . '_' . $et_machine_name . '_' . $rt_machine_name;
		
		// is_available_auth_building_1_space_meeting_resource
		$form[$key_auth_bldg][$temp_key_auth_bldg_et_rt] = array(
			'#type' 	=> 'fieldset',
			'#title' 	=> 'Select buildings to search over for ' . $et_label . ': ' . $rt_label,
		);
		
		$buildings = get_building_names($bldg_field, $et_machine_name, $rt_machine_name);
		foreach ($buildings as $b_name) {
			// is_available_auth_building_1_space_meeting_resource_ICCS
			$form[$key_auth_bldg][$temp_key_auth_bldg_et_rt]
				[$temp_key_auth_bldg_et_rt. '_' . $b_name] = array(
				'#type' => 'checkbox',
				'#title' => $b_name,
				'#default_value' => variable_get($temp_key_auth_bldg_et_rt. '_' . $b_name, 1),
			);
		}
	}		
	build_auth_buildings_button($form, $form_object);
}

function build_form_config_content_types(&$form, $form_state, $form_object, $nth_setting, &$array_resource_types,
	$bool_has_checked_auth_bldgs) {
	$key_content 	= $form_object->get_key_config_content();
	$key_ct 			= $form_object->get_key_content_type();
	// form stage control done outside
	
	if (!$bool_has_checked_auth_bldgs) {
		$form[$key_content]['error'] = array(
			'#markup' => 'Please select at least one building for each resource type.'
		);
		return;
	}
	
	foreach ($array_resource_types as $rt_machine_name => $object) {
		$entity_type			= $object->get_entity_type();
		$et_label 				= $entity_type->get_name();
		$et_machine_name 	= $entity_type->get_machine_name();
		$rt_label					= $object->get_title();
		
		// is_available_content_type_space_meeting_resource_1
		$temp_key_et_rt = append_nth($key_ct . '_' . $et_machine_name . '_' . $rt_machine_name, $nth_setting);
		$form[$key_content][$temp_key_et_rt] = array(
			'#type' 	=> 'fieldset',
			'#title' 	=> $et_label . ': ' . $rt_label . ' content type settings',
			'#description' => 'Select booking types made on ' . $rt_label,
		);
		
		$content_type_options = get_content_types();
		$i = 0;
		foreach ($content_type_options as $c) {
			$this_content_type = $temp_key_et_rt . '_' . $c;
			$form[$key_content][$temp_key_et_rt][$this_content_type] = array(
				'#type' => 'checkbox',
				'#title' => convert_to_readable_name($c),
				'#default_value' => variable_get($this_content_type, $i == 0 ? 1 : 0),
			);
			$i++;
		}
	}
	build_content_types_button($form, $form_object);
}


function build_form_config_entref(&$form, $form_state, $form_object, $nth_setting, $array_resource_types) {
	$key_config_entref				= $form_object->get_key_config_entref();
	$key_ct										= $form_object->get_key_content_type();
	$key_entref								= $form_object->get_key_entref();
	$bool_build_entref_button = TRUE;
	
	// Empty (no content type selected) condition handled in foreach
	foreach ($array_resource_types as $rt_machine_name => $object) {
		$entity_type			= $object->get_entity_type();
		$et_label 				= $entity_type->get_name();
		$et_machine_name 	= $entity_type->get_machine_name();
		$rt_label					= $object->get_title();
		$et_rt 						= $et_machine_name . '_' . $rt_machine_name;
		$key_et_rt 				= $key_entref . '_' . $et_rt;
		
		$content_types = get_checked_only($form_state, $key_ct . '_' . $et_rt, $nth_setting);
		$object->set_content_types($content_types); // Store in this ResourceType object
		
		if (empty($content_types)) {
			$bool_build_entref_button = FALSE;
			$form[$key_config_entref][$et_rt] = array(
				'#markup' => 'Please select at least one content type for ' . $et_label . ': ' . $rt_label.' <br><br>'
			);
		} else {
			$form[$key_config_entref][$key_et_rt] = array(
				'#type' => 'fieldset',
				'#title' => 'Entity reference table settings for ' .$et_label . ': ' . $rt_label,
			);
			
			foreach ($content_types as $c => $value) {
				// is_available_entref_space_meeting_resource_event_1
				$leading_strlen = 
					get_content_type_leading_strlen($form_object->get_module_name(), $et_machine_name, $rt_machine_name);
				$ct = substr($c, $leading_strlen);
				
				$temp_key_et_rt_c = append_nth($key_entref . '_' . $et_rt . '_' . $ct, $nth_setting);
				$readable_c = convert_to_readable_name($ct);
				$entref_table_options = get_entref_table_options($ct);
				
				$form[$key_config_entref][$key_et_rt][$temp_key_et_rt_c] = array(
				'#type' 	=> 'radios',
				'#title' 	=> $readable_c . ' settings',
				'#description' => 'Select the entity reference table for ' . $readable_c . ' bookings made on ' . $rt_label,
				'#options' => $entref_table_options,
				'#default_value' =>  variable_get($temp_key_et_rt_c, key($ent_ref_table_options)),
				);
			}
		}
	}
	build_entref_button($form, $form_object, $bool_build_entref_button);
}


function build_form_config_date(&$form, $form_state, $form_object, $nth_setting, $array_resource_types, $bool_has_selected_entref) {
	$key_date = $form_object->get_key_config_date();
	
	if (!$bool_has_selected_entref) {
		$form[$key_date]['no_entref'] = array(
			'#markup' => 'Please select an entity reference table for each resource type :: content type combination.'
		);
		return;
	}

	foreach($array_resource_types as $rt_machine_name => $object) {
		$entity_type			= $object->get_entity_type();
		$et_label 				= $entity_type->get_name();
		$et_machine_name 	= $entity_type->get_machine_name();
		$rt_label					= $object->get_title();
		$et_rt 						= $et_machine_name . '_' . $rt_machine_name; // e.g. space_classroom
		$this_key 				= append_nth('date_' . $et_rt, $nth_setting);
		$content_types 		= $object->get_content_types();
		
		$form[$key_date][$this_key] = array(
			'#type' => 'fieldset',
			'#title' => 'Date table settings for ' . $et_label . ': ' . $rt_label,
			'#description' => 'Select the date table to use for searching over content types on ' . 
				$et_label . ', ' . $rt_label,
		);
	
		foreach ($content_types as $ct_form_key => $value) {
			$leading_strlen = get_content_type_leading_strlen($form_object->get_module_name(),
				$et_machine_name, $rt_machine_name);
			$ct = substr($ct_form_key, $leading_strlen);
			$date_options = get_date_options($ct);
			$form[$key_date][$this_key][$this_key . '_' . $ct] = array(
				'#type' => 'radios',
				'#title' => 'Date table for ' . convert_to_readable_name($ct),
				'#options' => $date_options,
				'#default_value' => variable_get($this_key . '_' . $ct, key($date_options)),
			);
		}
	}
	build_date_button($form, $form_object);	
} 

function build_sys_settings(&$form, $bool_date_selected) {
	if (!$bool_date_selected) {
		$form['actions']['no_date'] = array(
			'#markup' => 'Please select a date table for each entity type :: resource type :: content type combination.'
		);
		return;
	}
	
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  if (!empty($_POST) && form_get_errors()) {
    drupal_set_message(t('The settings have not been saved because of the errors.'), 'error');
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
function build_form_setting_buttons(&$form, $form_object, $int_sizeof_setting_options) {

	$count = $int_sizeof_setting_options; 
	$key_setting_options 	= $form_object->get_key_setting_options();

	if ($count > 0) build_form_select_setting_button($form, $form_object);
	build_form_add_setting_button($form, $form_object);
	// If only one setting, do not load the delete button
	if ($count <= 1) return; 
		build_form_delete_setting_button($form, $form_object);

}

function build_form_select_setting_button(&$form, $form_object) {
	$key_config_settings = $form_object->get_key_only($form_object->get_key_config_settings());
	$key_button = $form_object->get_key_button();
	$setting_config_wrapper = $form_object->get_wrapper_settings();
	
	$form[$key_config_settings . $key_button] = array(
		'#type' => 'button',
		'#value' => convert_to_readable_name($key_config_settings),
		'#button_type' => 'submit ajax-trigger',
		'#ajax' => array(
			'submitter' => FALSE,
			'callback' => 'ajax_config_setting_callback',
			'event' => 'mousedown',
			'wrapper' => $setting_config_wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div class="' . $setting_config_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}

function build_form_add_setting_button(&$form, $form_object) {
	$key_add_setting = $form_object->get_key_add_setting();
	$key_button = $form_object->get_key_button();
	$add_setting_wrapper = $form_object->get_wrapper_add_setting();
	
	$form[$key_add_setting . $key_button] = array(
		'#type' => 'submit',
		'#value' => convert_to_readable_name($key_add_setting),
		'#validate' => array('is_available_form_add_setting_rebuild'),
		'#prefix' => '<div class="' . $add_setting_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}

function build_form_delete_setting_button(&$form, $form_object) {
	$key_del_setting = $form_object->get_key_delete_setting();
	$key_button = $form_object->get_key_button();
	$del_setting_wrapper = $form_object->get_wrapper_delete_setting();
	
	$form[$key_del_setting . $key_button] = array(
		'#type' => 'submit',
		'#value' => convert_to_readable_name($key_del_setting),
		'#validate' => array('is_available_form_delete_setting_rebuild'),
		'#prefix' => '<div class="' . $del_setting_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}


// $array_entity_types = all the entity types displayed in the checkboxes
function build_fsc_select_entity_type_button(&$form, $form_state, $form_object, $array_entity_types) {
	$module_name = $form_object->get_module_name();
	$key_config_settings = $form_object->get_key_config_settings();
	$key_config_content 	= $form_object->get_key_config_content();
	$resource_type_wrapper = $form_object->get_wrapper_resource_type();
	$content_config_wrapper = $form_object->get_wrapper_content();
 	
	$form[$key_config_settings]['select_ent_button'] = array(
		'#type' => 'button',
		'#value' => t('Select entity types'),
		'#button_type' => 'submit ajax-trigger',
		'#ajax' => array(
			'submitter' => FALSE,
			'callback' => 'ajax_select_entity_types_callback',
			'event' => 'mousedown',
			'wrapper' => $resource_type_wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div class="' . $content_config_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}

function build_form_crt_select_button(&$form, $form_object) {
	$key_config_resource_type = $form_object->get_key_config_resource_type();
	$building_wrapper = $form_object->get_wrapper_building();
	$resource_type_wrapper = $form_object->get_wrapper_resource_type();
	
	$form[$key_config_resource_type]['select_rt_button'] = array(
		'#type' => 'button',
		'#value' => t('Select resource types'),
		'#button_type' => 'submit ajax-trigger',
		'#ajax' => array(
			'submitter' => FALSE,
			'callback' => 'ajax_select_resource_types_callback',
			'event' => 'mousedown',
			'wrapper' => $building_wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div class="' . $resource_type_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}

function build_form_building_button(&$form, $form_object) {
	$key_building = $form_object->get_key_config_building();
	$building_wrapper = $form_object->get_wrapper_building();
	$auth_bldg_wrapper = $form_object->get_wrapper_auth_buildings();
	
	$form[$key_building]['building_button'] = array(
		'#type' => 'button',
		'#value' => t('Select tables'),
		'#button_type' => 'submit ajax-trigger',
		'#ajax' => array(
			'submitter' => FALSE,
			'callback' => 'ajax_select_building_table_callback',
			'event' => 'mousedown',
			'wrapper' => $auth_bldg_wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div class="' . $building_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}

function build_auth_buildings_button(&$form, $form_object) {
	$key_auth_bldg = $form_object->get_key_config_auth_buildings();
	$auth_bldg_wrapper = $form_object->get_wrapper_auth_buildings();
	$content_wrapper = $form_object->get_wrapper_content();
	
	$form[$key_auth_bldg]['auth_building_button'] = array(
		'#type' => 'button',
		'#value' => t('Select buildings'),
		'#button_type' => 'submit ajax-trigger',
		'#ajax' => array(
			'submitter' => FALSE,
			'callback' => 'ajax_select_auth_building_callback',
			'event' => 'mousedown',
			'wrapper' => $content_wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div class="' . $auth_bldg_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}
 
function build_content_types_button(&$form, $form_object) {
	$key_content = $form_object->get_key_config_content();
	$entref_wrapper = $form_object->get_wrapper_entref();
	$content_wrapper = $form_object->get_wrapper_content();
	
	$form[$key_content]['content_type_button'] = array(
		'#type' => 'button',
		'#value' => t('Select content types'),
		'#button_type' => 'submit ajax-trigger',
		'#ajax' => array(
			'submitter' => FALSE,
			'callback' => 'ajax_select_content_type_callback',
			'event' => 'mousedown',
			'wrapper' => $entref_wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div class="' . $content_wrapper . '-button">',
		'#suffix' => '</div>',
	);
} 

function build_entref_button(&$form, $form_object, $bool_build_entref_button) {
	$key_entref = $form_object->get_key_config_entref();
	
	if (!$bool_build_entref_button) {
		$form[$key_entref]['not_all_has_ct'] = array(
			'#markup' => 'Please select at least one content type for each entity type :: resource type combination before proceeding with this form.',
		);
		return;
	}
	
	$entref_wrapper = $form_object->get_wrapper_entref();
	$date_wrapper = $form_object->get_wrapper_date();
	
	$form[$key_entref]['entref_button'] = array(
		'#type' => 'button',
		'#value' => t('Select entity reference table'),
		'#button_type' => 'submit ajax-trigger',
		'#ajax' => array(
			'submitter' => FALSE,
			'callback' => 'ajax_select_entref_callback',
			'event' => 'mousedown',
			'wrapper' => $date_wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div class="' . $entref_wrapper . '-button">',
		'#suffix' => '</div>',
	);
}
 
function build_date_button(&$form, $form_object) {
	$key_date = $form_object->get_key_config_date();
	$date_wrapper = $form_object->get_wrapper_date();
	$save_config_wrapper = $form_object->get_wrapper_save_config();
	
	$form[$key_date]['date_button'] = array(
		'#type' => 'button',
		'#value' => t('Select date table'),
		'#button_type' => 'submit ajax-trigger',
		'#ajax' => array(
			'submitter' => FALSE,
			'callback' => 'ajax_select_date_callback',
			'event' => 'mousedown',
			'wrapper' => $save_config_wrapper,
			'method' => 'replace',
			'effect' => 'fade',
		),
		'#prefix' => '<div class="' . $date_wrapper . '-button">',
		'#suffix' => '</div>',
	);
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
function build_form_wrappers(&$form, $form_object) {
	build_form_wrap_config_settings($form, $form_object);
	build_form_wrap_resource_type($form, $form_object);
	build_form_wrap_building($form, $form_object);
	build_form_wrap_auth_buildings($form, $form_object);
	build_form_wrap_content_types($form, $form_object);
	build_form_wrap_entref($form, $form_object);
	build_form_wrap_date($form, $form_object);
	build_form_wrap_save_config($form, $form_object);
}
 
/**
 * array IsAvailableAdminSettingsForm -> (void)
 *
 * Wrap the settings form
 * @param $form	The form to build
 * @param $form_object
 */
function build_form_wrap_config_settings(&$form, $form_object) {
	$module_name = $form_object->get_module_name();
	$key_config_settings = $form_object->get_key_config_settings();
	$setting_config_wrapper = $form_object->get_wrapper_settings();

	$form[$key_config_settings] = array(
		'#prefix' => '<div id="' . $setting_config_wrapper . '">',
		'#suffix' => '</div>',
	);
}

function build_form_wrap_resource_type(&$form, $form_object) {
	$key_config_resource_type = $form_object->get_key_config_resource_type();
	$resource_type_wrapper = $form_object->get_wrapper_resource_type();
	
	// Wrap the form
	$form[$key_config_resource_type] = array(
		'#prefix' => '<div id="' . $resource_type_wrapper . '">',
		'#suffix' => '</div>',
	);
}

function build_form_wrap_building(&$form, $form_object) {
	$key_building = $form_object->get_key_config_building();
	$building_wrapper = $form_object->get_wrapper_building();
	
	$form[$key_building] = array(
		'#prefix' => '<div id="' . $building_wrapper . '">',
		'#suffix' => '</div>',
	);
}

function build_form_wrap_auth_buildings(&$form, $form_object) {
	$key_auth_bldg = $form_object->get_key_config_auth_buildings();
	$auth_bldg_wrapper = $form_object->get_wrapper_auth_buildings();
	
	$form[$key_auth_bldg] = array(
		'#prefix' => '<div id="' . $auth_bldg_wrapper . '">',
		'#suffix' => '</div>',
	);
}

function build_form_wrap_content_types(&$form, $form_object) {
	$key_content_types = $form_object->get_key_config_content();
	$content_types_wrapper = $form_object->get_wrapper_content();	
		$form[$key_content_types] = array(
		'#prefix' => '<div id="' . $content_types_wrapper . '">',
		'#suffix' => '</div>',
	);
}
function build_form_wrap_entref(&$form, $form_object) {
	$entref_wrapper = $form_object->get_wrapper_entref();
	$key_entref = $form_object->get_key_config_entref();
	$form[$key_entref] = array(
		'#prefix' => '<div id="' . $entref_wrapper . '">',
		'#suffix' => '</div>',
	);
}

function build_form_wrap_date(&$form, $form_object) {
		$date_wrapper = $form_object->get_wrapper_date();
		$key_date = $form_object->get_key_config_date();
		$form[$key_date] = array(
		'#prefix' => '<div id="' . $date_wrapper . '">',
		'#suffix' => '</div>',
	);
}

function build_form_wrap_save_config(&$form, $form_object) {
		$save_config_wrapper = $form_object->get_wrapper_save_config();
		$key_save_config = $form_object->get_key_save_config();
		$form['actions'] = array(
		'#prefix' => '<div id="' . $save_config_wrapper . '">',
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
	$key_config_settings = $form_object->get_key_config_settings();
	
	unset($form_state['values'][$key_config_settings]);
	return $form[$key_config_settings];
}

function ajax_select_entity_types_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_config_resource_type = $form_object->get_key_config_resource_type();
	
	unset($form_state['values'][$key_config_resource_type]);
	return $form[$key_config_resource_type];
}

function ajax_select_resource_types_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_config_building = $form_object->get_key_config_building();
	
	unset($form_state['values'][$key_config_building]);
	return $form[$key_config_building];
}

function ajax_select_building_table_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_auth_bldg = $form_object->get_key_config_auth_buildings();

	unset($form_state['values'][$key_auth_bldg]);
	return $form[$key_auth_bldg];
}

function ajax_select_auth_building_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_content = $form_object->get_key_config_content();	

	unset($form_state['values'][$key_content]);
	return $form[$key_content];
}

function ajax_select_content_type_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_entref = $form_object->get_key_config_entref();

	unset($form_state['values'][$key_entref]);
	return $form[$key_entref];
}

function ajax_select_entref_callback(&$form, &$form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_date = $form_object->get_key_config_date();

	unset($form_state['values'][$key_date]);
	return $form[$key_date];
}

function ajax_select_date_callback(&$form, &$form_state) {
	unset($form_state['complete form']['actions']);
	return $form['actions'];
}

function is_available_form_add_setting_rebuild($form, $form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$module_name = $form_object->get_module_name();
	$alphabet = '123456789';
	$count = get_setting_multi_id_min_unused_id($module_name, $alphabet);
	insert_into_variable_new_setting($count, $module_name);
	$form_state['rebuild'] = TRUE;
}

function is_available_form_delete_setting_rebuild(&$form, $form_state) {
	$form_object = new IsAvailableAdminSettingsForm;
	$key_setting_options 	= $form_object->get_key_setting_options();
	$bool_setting_selected = isset($form_state['values'][$key_setting_options]); 
	$nth_setting = $bool_setting_selected ? $form_state['values'][$key_setting_options] : 'a';
	$module_name = $form_object->get_module_name();
	
	global $conf;
	
	$current_title = $form_object->get_settings_options()[$nth_setting];
	$sid = get_setting_multi_id_to_delete($current_title, $module_name);
	if (!is_string($sid)) return; // no setting to delete
	
	$name =$module_name . '_%' . $sid . '%';
	
	unset($form_state['values'][$key_setting_options]);
	
  db_delete('variable')->condition('name', $name, 'LIKE')->execute();
	$form_state['rebuild'] = TRUE; 
}

function get_setting_multi_id_min_unused_id($module_name, $alphabet) {
	$setting_title = $module_name . "_settings_title_";
	$mysql_query = "SELECT name FROM variable WHERE name LIKE '" . $setting_title . "%'";
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

function get_setting_multi_id_to_delete($current_title, $module_name) {
	$setting_title = $module_name . "_settings_title_";
	$mysql_query = "SELECT name FROM variable WHERE name LIKE '" . $setting_title . "%'";
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
function insert_into_variable_new_setting($count, $module_name) {
	$setting_title = $module_name . '_settings_title_' . $count;
	if (trim(variable_get($setting_title, "")) == "") {
		variable_set($setting_title, 'Setting ' . strtoupper($count) . ' (default)');
		return;
	}

	do {
		$count++;
		$setting_title = $module_name . '_settings_title_' . $count;
	} while (variable_get($setting_title, "") != ""); 

	variable_set($setting_title, 'Setting ' . strtoupper($count) . ' (default)');
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
function get_checked_et_rt(&$form_state, $checked_entity_types, $nth_setting = -1) {
	$ret_array = array();
	$form_object = new IsAvailableAdminSettingsForm;
	$mod_name = $form_object->get_module_name();
	foreach ($checked_entity_types as $et_key => $et_value) {
		// is_available_entity_type_1_space
		// is_available_resource_type_1_space_classresource
		$leading_et_strlen = get_leading_et_strlen($mod_name);
		$et = substr($et_key, $leading_et_strlen);
		$leading_rt_strlen = get_leading_rt_strlen($mod_name, $et);
		$et_rt_array = array();
		$array_checked_rt = get_checked_only($form_state, 'resource_type', $nth_setting);
		
		foreach ($array_checked_rt as $rt_key => $rt_value) {
			array_push($et_rt_array, substr($rt_key, $leading_rt_strlen));
		}
		$ret_array[$et] = $et_rt_array;
	}
	return $ret_array;
}

function get_entity_types() {
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
		// $entity_types['space'] = Pair(EntityTaipe('space', 'Space'), array('title', ...))
		$entity_types[$q->name] = new Pair(new EntityTaipe($q->name, $q->label), $title_column_array);
	}
	return $entity_types;
}

function get_content_types() {
	$ret_array = array();
	$mysql_query = "SELECT bundle FROM field_config FC INNER JOIN field_config_instance FCI
		ON FC.field_name = FCI.field_name WHERE module = 'date'";
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		$ret_array[$q->bundle] = $q->bundle;
	}
	return $ret_array;
}

function get_building_names($bldg_field, $et_machine_name, $rt_machine_name) {
	$ret_array = array(); // array['ICCS'] = 'ICCS'
	
	$bldg_table 			= mysql_real_escape_string(get_field_table($bldg_field));
	$bldg_col 				= mysql_real_escape_string(get_field_column($bldg_field));
	$et_machine_name	= mysql_real_escape_string($et_machine_name);
	$rt_machine_name	= mysql_real_escape_string($rt_machine_name);
	
	$mysql_query = "SELECT DISTINCT(" . $bldg_col . ") AS value FROM " . $bldg_table .
		" WHERE entity_type = '" . $et_machine_name . "' AND bundle = '" . $rt_machine_name . "'";
	$query = db_query($mysql_query);
	
	foreach ($query as $q) {
		$ret_array[$q->value] = $q->value;
	}
	
	return $ret_array;
}

function get_checked_auth_buildings($form_state, $form_object, $checked_et_rt, $nth_setting) {
	$ret_array = array(); // array['space_meeting_resource'] = get_checked_only...
	foreach ($checked_et_rt as $et => $rt_array) {
		// is_available_auth_building_1_space_meeting_resource_ICCS 
		foreach ($rt_array as $rt) {
			// auth_building_1_space_meeting_resource
			$term = $form_object->get_key_auth_buildings() . '_' . $nth_setting . '_' . $et . '_' . $rt;
			$ret_array[$et . '_' . $rt] =	get_checked_only($form_state, $term);
		}
	}
	return $ret_array;
}

function get_building_table_options($entity_type, $resource_type) {
	$ret_array = array();
	$entity_type = mysql_real_escape_string($entity_type);
	$resource_type = mysql_real_escape_string($resource_type);
	$mysql_query = "SELECT field_name FROM field_config_instance WHERE entity_type = '" . $entity_type .
		"' AND bundle = '" . $resource_type. "'";
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		$ret_array[$q->field_name] = $q->field_name;
	}
	return $ret_array;
}

function get_entity_type_resource_types($entity_type) {
	$ret_array = array();	
	if ($entity_type == '') return $ret_array;
	$query = db_query("SELECT DISTINCT(name), label FROM eck_bundle EB
		INNER JOIN eck_" . $entity_type . " ET oN EB.name = ET.type");
	foreach ($query as $q) {
		$ret_array[$q->name] = $q->label;
	}
	return $ret_array;
}

function get_entref_table_options($content_type_machine_name) {
	$ret_array = array();
	$ct = mysql_real_escape_string($content_type_machine_name);
	$mysql_query = "select fci.field_name, bundle from field_config fc inner join
		field_config_instance fci on fc.field_name = fci.field_name 
		where bundle in (select bundle from field_config fc2 inner join
		field_config_instance fci2 on fc2.field_name = fci2.field_name
		where fc2.module = 'date') and fc.module = 'entityreference'
		and bundle = '" . $ct . "'";
	
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		$ret_array[$q->field_name] = $q->field_name;
	}
	return $ret_array;
}

function get_date_options($content_type) {
	$ret_array = array();
	$mysql_query = "SELECT FCI.field_name FROM field_config_instance FCI
		INNER JOIN field_config FC ON FCI.field_name = FC.field_name
		WHERE bundle = '" . $content_type . "' AND FC.module = 'date'";
	$query = db_query($mysql_query);
	foreach ($query as $q) {
		$ret_array[$q->field_name] = $q->field_name;
	}
	return $ret_array;
}


function get_form_stage_n_element_array($form_state_values, $nth_setting) {
	$form_object 	= new IsAvailableAdminSettingsForm;
	$mod_name 		= $form_object->get_module_name(); 
	$key_setting_options 	= $form_object->get_key_setting_options(); // settings_options
	// $key_button = configure_setting_button
	// $key_settings_title = $mod_name . '_sttings_title_' $nth_setting; // is_available_settings_title_1
	$key_entity_type		 	= append_nth('entity_type', $nth_setting) . '_'; // is_available_entity_type_1_||space
	// $key_et_button = 'select_ent_button';
	
	// is_available_resource_type_1_||space_meeting_resource
	$key_resource_type				= append_nth($form_object->get_key_resource_type(), $nth_setting) . '_';
	// is_available_building_table_1_||space_classresource
	$key_bldg_table				= append_nth($form_object->get_key_building_table(),  $nth_setting) . '_';
	// is_available_resource_number_table_1_||space_classresource
	$key_rn_table					= append_nth($form_object->get_key_resource_number_table(), $nth_setting) . '_';
	// $key_bldg_button = 'building_button';
	
	// is_available_auth_building_1_space_meeting_resource_ICCS
	$key_auth_building 		= append_nth($form_object->get_key_auth_buildings(), $nth_setting) . '_';
	// is_available_content_type_space_meeting_resource_1_event
	$key_content_type 		= $mod_name . '_content_type_';
	// $key_ct_button = 'content_type_button';
	
	// is_available_entref_space_meeting_resource_classresource_1 
	$key_entref						= $mod_name . '_entref_'; 
	// $key_entref_button = 'entref_button';
	
	// is_available_date_space_meeting_resource_classresource_1 
	$key_date							= $mod_name . '_date_'; 
	
	// rest of the keys are submit, form_build_id, form_token, form_id, and op
	
	// number of elements in $form_state['values'] at the given stage in the form

	$a = 8;
	$b = ($a + array_count_key_start_with($form_state_values, $key_entity_type, $a - 5) + 2); // +1 for the title
	$c = ($b + array_count_key_start_with($form_state_values, $key_resource_type, $b - 5) + 1); // +1 for the button element
	$d = ($c + array_count_key_start_with($form_state_values, $key_bldg_table, $c - 5) + 
		array_count_key_start_with($form_state_values, $key_rn_table, $c - 5) + 1);
	$e = ($d + array_count_key_start_with($form_state_values, $key_auth_building, $d - 5) + 1);
	$f = ($e + array_count_key_start_with($form_state_values, $key_content_type, $e - 5) + 1);
	$g = ($f + array_count_key_start_with($form_state_values, $key_entref, $f - 5) + 1);
	$h = ($g + array_count_key_start_with($form_state_values, $key_date, $g - 5) + 1);
	
	$ret_array = array(
		$key_setting_options 										=> $a, 
		$form_object->get_key_entity_type()			=> $b, 
		$form_object->get_key_resource_type()				=> $c,
		$form_object->get_key_brn_table()				=> $d, 
		$form_object->get_key_auth_buildings()	=> $e,
		$form_object->get_key_content_type()	 	=> $f, 
		$form_object->get_key_entref()					=> $g,
		$form_object->get_key_date() 						=> $h,
	);
	return $ret_array;
}

function get_checkboxes_value($form_state, $string, $n = -1) {
	$ret_array = array();
	$form_object = new IsAvailableAdminSettingsForm;
	$term = '_' . $string . '_';
	$term .= ($n != -1) ? $n . '_' : '';
	$term_string_length = strlen($term);
	
	$mod_name = $form_object->get_module_name();
	if (empty($form_state['values'])) return $ret_array;
	foreach ($form_state['values'] as $k => $f) {
		if (substr($k, 0, strlen($mod_name) + $term_string_length) === $mod_name . $term) {
			$ret_array[$k] = $f;
		}
	}
	return $ret_array;
}

function get_checked_only(&$form_state, $string, $n = -1) {
	return array_filter(get_checkboxes_value($form_state, $string, $n), 'is_checked');
}

function get_leading_et_strlen($mod_name) { return strlen($mod_name) + strlen('_entity_type_') + 2; }
function get_leading_rt_strlen($mod_name, $entity_type_machine_name) {
	return strlen($mod_name) + strlen('_resource_type_') + 2 + strlen($entity_type_machine_name) + 1;
}
function get_content_type_leading_strlen($module_name, $et_machine_name, $rt_machine_name) { 
	// e.g. is_available_content_type_space_meeting_resource_1_classresource, return string length of
	// everything up to classresource (the content type's machine name)
	return strlen($module_name) + 
		strlen('_content_type_') + 
		strlen($et_machine_name) + 1 + 
		strlen($rt_machine_name) + 3;
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
function has_checked($array) {
	return !empty($array) && in_array(1, $array);
}

/**
 *
 * Returns true if at least one x for eaach resource type is checked.
 * @param $checked_key_maps_array array('key' => array()) inner array is checked values only
 */
function has_checked_for_each_rt($checked_key_maps_array) {
	if (empty($checked_key_maps_array)) return FALSE;
	
	$has_checked = TRUE;
	foreach ($checked_key_maps_array as $key => $array_value) {
		$has_checked = $has_checked && (sizeof($array_value) >= 1);
	}
	return $has_checked;
}

function has_checked_entity_type($checked_entity_types) {
	return sizeof($checked_entity_types) >= 1;
}

function has_checked_resource_type($checked_et_rt) {
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
function has_selected_brn_table($form_state_values) {
	return has_selected_radios($form_state_values, 'select_rt_button', 'building_button');
}
	
function has_selected_entref($form_state_values) {
	return has_selected_radios($form_state_values, 'content_type_button', 'entref_button');
}

function has_selected_date($form_state_values) {
	return has_selected_radios($form_state_values, 'entref_button', 'date_button');
}

function has_selected_radios($form_state_values, $start_str_index, $end_str_index) {
	
	$keys = array_keys($form_state_values);
	// e.g. $form_state_values['content_type_button'] - 1;
	$start_index = array_search($start_str_index, $keys) + 1; 
	
	$end_index = array_search($end_str_index, $keys);
	$bool_has_selected = TRUE;
	
	for ($i = $start_index; $i < $end_index; $i++) {
		$bool_has_selected = $bool_has_selected && ($form_state_values[$keys[$i]] != null);
	}
	return $bool_has_selected;
}


function at_this_stage($array_fsne, $key1, $key2) {
	return $array_fsne[$key1] < $array_fsne[$key2] - 1;
}

function is_checked($item) { return $item == 1; }

function is_at_entity_type($form_object, $array_fsne) {
	$key_setting_options 	= $form_object->get_key_setting_options();
	$key_entity_type 			= $form_object->get_key_entity_type();
	return $array_fsne[$key_setting_options] < $array_fsne[$key_entity_type] - 2;
}

function is_at_resource_type($form_object, $array_fsne) {
	return at_this_stage($array_fsne, $form_object->get_key_entity_type(), $form_object->get_key_resource_type());
}

function is_at_brn_table($form_object, $array_fsne) { 
	return at_this_stage($array_fsne, $form_object->get_key_resource_type(), $form_object->get_key_brn_table());
}

function is_at_auth_buildings($form_object, $array_fsne) {
	return at_this_stage($array_fsne, $form_object->get_key_brn_table(), $form_object->get_key_auth_buildings());
}

function is_at_content_type($form_object, $array_fsne) {
	return at_this_stage($array_fsne, $form_object->get_key_auth_buildings(), $form_object->get_key_content_type());
}

function is_at_entref($form_object, $array_fsne) {
	return at_this_stage($array_fsne, $form_object->get_key_content_type(), $form_object->get_key_entref());
}

function is_at_date($form_object, $array_fsne) {
	return at_this_stage($array_fsne, $form_object->get_key_entref(), $form_object->get_key_date());
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
function append_nth($string, $n) {
	$form_object = new IsAvailableAdminSettingsForm;
	return $form_object->get_module_name() . '_' . $string . '_' . $n;
}
