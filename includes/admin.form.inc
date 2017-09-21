<?php

/**
 * @file
 * The forms for checking children of a book, collection or batch and for
 * generating missing datastreams per object
 *
 *
 *  Copyright 2017 Leiden University Library
 *
 *  This file is part of islandora_check_datastreams.
 *
 *  islandora_check_datastreams is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * The form for generating a datastream of an object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_check_datastreams_generate_form(array $form, array &$form_state, $objectid, $dsid) {
  $form = array();

  $form['generate'] = array(
    '#title' => t('Generate datastream @dsid of object @objectid', array('@dsid' => $dsid, '@objectid' => $objectid)),
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['generate']['objectid'] = array(
    '#type' => 'hidden',
    '#default_value' => $objectid,
  );
  $form['generate']['dsid'] = array(
    '#type' => 'hidden',
    '#default_value' => $dsid,
  );
  $form['generate']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Generate datastream'),
  );
  return $form;
}

/**
 * The form submit for generating a datastream of an object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_check_datastreams_generate_form_submit(array $form, array &$form_state) {
  $objectid = $form_state['values']['objectid'];
  $dsid = $form_state['values']['dsid'];

  $object = islandora_object_load($objectid);
  $success = FALSE;
  if ($object) {
    if ($dsid === 'HDL') {
      if (module_load_include('inc', 'islandora_handle') !== FALSE) {
        if (function_exists('islandora_handle_append_handle_from_configuration')) {
          $result = islandora_handle_append_handle_from_configuration($object, FALSE, array('destination_dsid' => 'MODS'));
        }
        else if (function_exists('islandora_handles_append_handles_from_configuration')) {
          $result = islandora_handle_append_handles_from_configuration($object, FALSE, array('destination_dsid' => 'MODS'));
        }
        if (isset($result) && $result['success'] === TRUE) {
          islandora_handle_add_to_dc_datastream($object);
          $success = TRUE;
        }
      }
    }
    elseif ($dsid === 'DIMENSIONS') {
      if ($object['JP2']) {
        if (module_load_include('inc', 'islandora_paged_content') !== FALSE) {
          islandora_paged_content_add_dimensions_relationships($object);
          $success = TRUE;
        }
      }
    }
    else {
      module_load_include('inc', 'islandora', 'includes/derivatives');
      $logging = islandora_do_derivatives($object, array( 'destination_dsid' => $dsid));
      if (count($logging) > 0) {
        foreach ($logging as $log) {
          if (isset($log['success']) && $log['success']) {
            $success = TRUE;
          }
          else {
            $success = FALSE;
          }
        }
      }
    }
  }

  if ($success) {
    drupal_set_message(t('Derivative @dsid successfully created.', array('@dsid' => $dsid)), 'status');
  }
  else {
    drupal_set_message(t('Failed to create derivative @dsid.', array('@dsid' => $dsid)), 'error');
  }

  $form_state['redirect'] = 'islandora/object/' . $objectid . '/manage/datastreams';
}

/**
 * The form for checking the children of a book, collection or batch.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_check_datastreams_children_form(array $form, array &$form_state, $objectid = FALSE, $objecttype = FALSE) {
  module_load_include('inc', 'islandora_check_datastreams', 'includes/utilities');
  module_load_include('inc', 'islandora_batch', 'includes/db');
  module_load_include('inc', 'islandora', 'includes/utilities');
  module_load_include('inc', 'islandora_paged_content', 'includes/utilities');

  $form = array();

  $form['selection'] = array(
    '#title' => t('Selection'),
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['selection']['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array(
      '' => t('Choose value'),
      'collection' => t('Collection'),
      'book' => t('Book'),
      'compound' => t('Compound'),
      'batch' => t('Batch set'),
    ),
    '#default_value' => $objecttype,
  );
  $form['selection']['identifier'] = array(
    '#type' => 'textfield',
    '#title' => t('Identifier'),
    '#default_value' => $objectid,
    '#size' => 60,
  );
  $form['selection']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Check datastreams'),
    '#id' => 'filter',
  );

  $childids = array();
  if ($objecttype === 'collection') {
    $collection = islandora_object_load($objectid);
    if ($collection && in_array('islandora:collectionCModel', $collection->models)) {
      islandora_check_datastreams_retrieve_all_child_ids($collection, $childids);
    }
  }
  elseif ($objecttype === 'book') {
    $book = islandora_object_load($objectid);
    if ($book && in_array('islandora:bookCModel', $book->models)) {
      islandora_check_datastreams_retrieve_all_child_ids($book, $childids);
    }
  }
  elseif ($objecttype === 'compound') {
    $compound = islandora_object_load($objectid);
    if ($compound && in_array('islandora:compoundCModel', $compound->models)) {
      islandora_check_datastreams_retrieve_all_child_ids($compound, $childids);
    }
  }
  elseif ($objecttype === 'batch') {
    $itemids = islandora_batch_get_queued_items_for_set($objectid);
    foreach ($itemids as $itemid) {
      $childids[$itemid] = $itemid;
    }
  }

  if (count($childids) > 0) {
    $dsidsbycmodel = islandora_check_datastreams_dsids_by_cmodel($childids);
    foreach ($dsidsbycmodel as $cmodel => $dsids) {
      $vdsids = array_values($dsids);
      $total = $vdsids[0]['total'];
      $form[$cmodel] = array(
        '#title' => format_plural($total, '1 object with content model @cmodel', '@count objects with content model @cmodel', array('@cmodel' => $cmodel)),
        '#type' => 'fieldset',
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
      );
      $options = array();
      foreach ($dsids as $dsid => $counts) {
        $remarks = '-';
        $required = $counts['isrequired'] ? t('required') : t('optional');
        if ($counts['found'] < $counts['total']) {
          $remarks = format_plural(($counts['total'] - $counts['found']), 'Missing 1 @required @type datastream', 'Missing @count @required @type datastreams', array('@required' => $required, '@type' => $dsid));
        }
        $key = $cmodel . '//' . $dsid;
        $options[$key] = array(
          'dsid' => $dsid,
          'required' => $required,
          'found' => $counts['found'],
          'remarks' => $remarks,
        );
      }
      $form[$cmodel]['table'] = array(
        '#type' => 'tableselect',
        '#header' => array(
          'dsid' => t('ID'),
          'required' => t('Required'),
          'found' => t('Found'),
          'remarks' => t('Remarks'),
        ),
        '#options' => $options,
        '#empty' => t('No data available.'),
        '#validated' => TRUE,
      );
    }
    $form['downloadCSV'] = array(
      '#type' => 'submit',
      '#value' => t('Download CSV with identifiers of objects with missing datastreams'),
    );
  }

  return $form;
}

/**
 * The form submit for checking the children of a book, collection or batch.
 */
function islandora_check_datastreams_children_form_submit(array $form, array &$form_state) {
  if ($form_state['clicked_button']['#id'] === 'filter') {
    $type = $form_state['values']['type'];
    $identifier = $form_state['values']['identifier'];

    $form_state['redirect'] = 'admin/islandora/check_datastreams/check_children/' . $identifier . '/' . $type;
  }
  else {
    $iscsvneeded = FALSE;
    $cmodel2dsids = array();
    foreach ($form_state["values"]["table"] as $key => $isneeded) {
      if ($isneeded !== 0) {
        $iscsvneeded = TRUE;
        $parts = explode("//", $key);
        $selectedcmodel = $parts[0];
        $selecteddsid = $parts[1];
        if (!isset($cmodel2dsids[$selectedcmodel])) {
          $cmodel2dsids[$selectedcmodel] = array();
        }
        $cmodel2dsids[$selectedcmodel][$selecteddsid] = $selecteddsid;
      }
    }
    if ($iscsvneeded) {
      $objecttype = $form_state['values']['type'];
      $objectid = $form_state['values']['identifier'];
      $csv = islandora_check_datastreams_make_csv_content($objectid, $objecttype, $cmodel2dsids);
      $filepath = "temporary://missing.csv";
      $file = file_save_data($csv, $filepath, FILE_EXISTS_REPLACE);
      $headers = array(
        'Content-Type' => 'text/csv',
        'Content-Disposition' => 'attachment; filename="' . $file->filename . '"',
        'Content-Length' => strlen($csv),
      );

      if (strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE')) {
        $headers['Cache-Control'] = 'must-revalidate, post-check=0, pre-check=0';
        $headers['Pragma'] = 'public';
      }
      else {
        $headers['Pragma'] = 'no-cache';
      }

      file_transfer($file->uri, $headers);
 
      file_delete($filepath);
    }
  }
}

