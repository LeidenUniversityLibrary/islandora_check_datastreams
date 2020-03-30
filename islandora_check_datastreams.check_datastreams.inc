<?php

/**
 * @file
 * islandora_check_datastreams.check_datastreams
 *
 *
 *  Copyright 2020 Leiden University Library
 *
 *  This file is part of islandora_check_datastreams.
 *
 *  islandora_check_datastreams is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published by
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
 */

/**
 * Implements hook_check_datastreams_info().
 */
function islandora_check_datastreams_check_datastreams_info($object) {
  $result = array();

  if (module_load_include('inc', 'islandora_handle', 'includes/handle') !== FALSE) {
    $handle_results = islandora_handle_retrieve_configurations_by_cmodels($object->models);
    $result['HDL']['required'] = (count($handle_results) > 0);
    $result['HDL']['generatable'] = TRUE;
    $target = NULL;
    if (function_exists('islandora_handle_retrieve_selected_handler')) {
      $handler_class = islandora_handle_retrieve_selected_handler();
      $handler = new $handler_class();
      $target = $handler->readHandle($object);
      $handle = $handler->getFullHandle($object);
      $newtarget = $handler->constructTargetUrl($object);
    }
    elseif (function_exists('islandora_handle_query_for_handle')) {
      $target = islandora_handle_query_for_handle($object);
      $handle = islandora_handle_construct_url($object);
      $suffix = islandora_handle_construct_suffix($object);
      $newtarget = islandora_handle_construct_target_url($suffix);
    }
    if ($target) {
      $result['HDL']['available'] = TRUE;
      $result['HDL']['include'] = TRUE;
      $result['HDL']['remark'] = t('Handle %handle targets %target', array('%handle' => $handle, '%target' => $target));
      if ($target !== $newtarget) {
        $result['HDL']['regeneratable'] = TRUE;
      }
      else {
        $result['HDL']['generatable'] = FALSE;
      }
    }
    else {
      $result['HDL']['available'] = FALSE;
      if ($result['HDL']['required']) {
        $result['HDL']['remark'] = t('Missing required handle');
      }
    }
  }
  if (module_load_include('module', 'islandora_fits', 'islandora_fits') !== FALSE) {
    $techmd = variable_get('islandora_fits_techmd_dsid', 'TECHMD');
    $result[$techmd]['required'] = FALSE;
    $result[$techmd]['generatable'] = isset($object['OBJ']);
  }
  return $result;
}

/**
 * Implements hook_cmodel_pid_check_datastreams_info().
 */
function islandora_check_datastreams_islandora_sp_basic_image_check_datastreams_info($object) {
  $isgeneratable = (module_load_include('inc', 'islandora_basic_image', 'includes/derivatives') !== FALSE);
  return array(
    'MEDIUM_SIZE' => array(
      'required' => FALSE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
    'TN' => array(
      'required' => FALSE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
  );
}

/**
 * Implements hook_cmodel_pid_check_datastreams_info().
 */
function islandora_check_datastreams_islandora_sp_large_image_cmodel_check_datastreams_info($object) {
  $isgeneratable = (module_load_include('inc', 'islandora_large_image', 'includes/derivatives') !== FALSE);
  return array(
    'JP2' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
    'JPG' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
    'TN' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
  );
}

/**
 * Implements hook_cmodel_pid_check_datastreams_info().
 */
function islandora_check_datastreams_islandora_bookcmodel_check_datastreams_info($object) {
  $hasbookmodule = (module_load_include('module', 'islandora_book', 'islandora_book') !== FALSE);
  $haspdfmodule = (module_load_include('inc', 'islandora_pdf', 'includes/derivatives') !== FALSE);
  $hasocrmodule = (module_load_include('inc', 'islandora_ocr', 'includes/derivatives') !== FALSE);
  $generatelink = l(t('generate'), 'islandora/object/' . $object->id . '/manage/book');
  return array(
    'PDF' => array(
      'required' => FALSE,
      'generatable' => ($hasbookmodule && $haspdfmodule),
      'generatelink' => $generatelink,
    ),
    'OCR' => array(
      'required' => FALSE,
      'generatable' => ($hasbookmodule && $hasocrmodule),
      'generatelink' => $generatelink,
    ),
    'HOCR' => array(
      'required' => FALSE,
      'generatable' => ($hasbookmodule && $hasocrmodule),
      'generatelink' => $generatelink,
    ),
    'TN' => array(
      'required' => FALSE,
      'generatable' => $hasbookmodule,
      'generatelink' => $generatelink,
    ),
  );
}

/**
 * Implements hook_cmodel_pid_check_datastreams_info().
 */
function islandora_check_datastreams_islandora_sp_pdf_check_datastreams_info($object) {
  $isgeneratable = (module_load_include('inc', 'islandora_pdf', 'includes/derivatives') !== FALSE);
  return array(
    'TN' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
    'PREVIEW' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
    'FULL_TEXT' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
    'PDFA' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
  );
}

/**
 * Implements hook_cmodel_pid_check_datastreams_info().
 */
function islandora_check_datastreams_islandora_pagecmodel_check_datastreams_info($object) {
  $isgeneratable = (module_load_include('inc', 'islandora_paged_content', 'includes/utilities') !== FALSE);
  return array(
    'DIMENSIONS' => array(
      'available' => (isset($object['JP2']) && $object['JP2']->relationships->get(ISLANDORA_RELS_EXT_URI, 'width')),
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['JP2'], $object['OBJ']),
    ),
    'JP2' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
    'JPG' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
    'TN' => array(
      'required' => TRUE,
      'generatable' => $isgeneratable && isset($object['OBJ']),
    ),
  );
}

/**
 * Implements hook_cmodel_pid_check_datastreams_info().
 */
function islandora_check_datastreams_ir_citationcmodel_check_datastreams_info($object) {
  $isgeneratable = (module_load_include('module', 'islandora_fits', 'islandora_fits') !== FALSE);
  $techmd = variable_get('islandora_fits_techmd_dsid', 'TECHMD');
  return array(
    $techmd => array(
      'required' => FALSE,
      'generatable' => $isgeneratable && isset($object['PDF']),
    ),
  );
}
