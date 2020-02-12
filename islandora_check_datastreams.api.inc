<?php

/**
 * @file
 * islandora_check_datastreams.api
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
 * Hook to collect what datastreams should be checked and how, by content model.
 *
 * @return array
 *   Returns an array where the keys are the names of datastreams.
 *   The value of each key is an array containing the following keys:
 *    - available:   boolean, optional key that indicates if the datastream
 *                   exists. If this key does not exist, the availability is
 *                   checked by isset($object[$ds]);
 *    - required:    boolean, optional key that indicates if the datastream must
 *                   exist;
 *    - generatable: boolean, mandatory key that indicates if the datastream can
 *                   be generated;
 *    - generatelink:function, optional key that is a function that return a
 *                   link to a page where the datastream can be generated;
 *    - remark:      string or function returning string, optional key that
 *                   returns a remark string.
 *  The value of the boolean keys is either a boolean or a function returning
 *  a boolean. The function value is an array with two keys:
 *    - function: the name of the function. The function takes two arguments:
 *                $object and $dsid;
 *    - file:     A string denoting the path to the file where the function is
 *                implemented. 
 */
function hook_cmodel_pid_check_datastreams_info($object) {
  // Example implementation for large image content model:
  return array(
    'JP2' => array(
      'required' => TRUE,
      'generatable' => TRUE,
    ),
    'JPG' => array(
      'required' => TRUE,
      'generatable' => TRUE,
    ),
    'TN' => array(
      'required' => TRUE,
      'generatable' => TRUE,
    ),
  );
  // Example implementation for page content models DIMENSIONS (not a real ds):
  return array(
    'DIMENSIONS' => array(
      'available' => array(
        'function' => 'check_if_dimensions_exists',
        'file' => 'path/to/file',
      ),
      'required' => TRUE,
      'generatable' => TRUE,
    ),
  );
}
